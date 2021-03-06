#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/clk.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/irq.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>

#include <asm/io.h>
#include <asm/atomic.h>
#include <mach/map.h>

#include <plat/regs-g2d.h>

#define G2D_MMAP_SIZE		0x1000
#define G2D_MINOR		240

#define G2D_IOCTL_MAGIC		'G'
#define G2D_DMA_CACHE_INVAL	_IOWR(G2D_IOCTL_MAGIC, 0, struct g2d_dma_info)
#define G2D_DMA_CACHE_CLEAN	_IOWR(G2D_IOCTL_MAGIC, 1, struct g2d_dma_info)
#define G2D_DMA_CACHE_FLUSH	_IOWR(G2D_IOCTL_MAGIC, 2, struct g2d_dma_info)
#define G2D_WAIT_FOR_IRQ	_IO(G2D_IOCTL_MAGIC, 3)

struct g2d_info {
	struct clk *clock;
	int irq;
	struct resource *mem;
	void __iomem *base;
	struct mutex *lock;
	wait_queue_head_t wq;
	struct device *dev;
	atomic_t in_use;
};

struct g2d_dma_info {
	unsigned long addr;
	size_t size;
};

static struct g2d_info *g2d;

irqreturn_t g2d_irq(int irq, void *dev_id)
{
	if (readl(g2d->base + G2D_INTC_PEND_REG) & G2D_INTP_CMD_FIN) {
		writel(0, g2d->base + G2D_INTEN_REG);
		writel(G2D_INTP_CMD_FIN, g2d->base + G2D_INTC_PEND_REG);
		atomic_set(&g2d->in_use, 1);
		wake_up(&g2d->wq);
	}

	return IRQ_HANDLED;
}

static int g2d_open(struct inode *inode, struct file *file)
{
	clk_enable(g2d->clock);

	return 0;
}

static int g2d_release(struct inode *inode, struct file *file)
{
	clk_disable(g2d->clock);

	return 0;
}

static int g2d_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long pfn = 0;
	unsigned long size;

	size = vma->vm_end - vma->vm_start;
	pfn = __phys_to_pfn(g2d->mem->start);

	if (size > G2D_MMAP_SIZE) {
		printk(KERN_ERR "g2d: invalid mmap size\n");
		return -EINVAL;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	if ((vma->vm_flags & VM_WRITE) && !(vma->vm_flags & VM_SHARED)) {
		printk(KERN_ERR "g2d: mapping must be shared\n");
		return -EINVAL;
	}

	/* remap kernel memory to userspace */
	if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot)) {
		printk(KERN_ERR "g2d: failed to mmap\n");
		return -EINVAL;
	}

	return 0;
}

static unsigned int g2d_poll(struct file *file, struct poll_table_struct *wait)
{
	u32 mask = 0;

	if (atomic_read(&g2d->in_use) == 1) {
		mask = POLLOUT | POLLWRNORM;
		atomic_set(&g2d->in_use, 0);
	} else {
		poll_wait(file, &g2d->wq, wait);
	}

	return mask;
}

static int g2d_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct g2d_dma_info dma_info;
	void *vaddr;

	if (cmd == G2D_WAIT_FOR_IRQ) {
		wait_event_timeout(g2d->wq, (atomic_read(&g2d->in_use) == 1), 10000);
		atomic_set(&g2d->in_use, 0);
		return 0;
	}

	if (copy_from_user(&dma_info, (struct g2d_dma_info *)arg, sizeof(dma_info)))
		return -EFAULT;

	vaddr = phys_to_virt(dma_info.addr);

	switch(cmd) {
	case G2D_DMA_CACHE_INVAL:
		dma_cache_maint(vaddr, dma_info.size, DMA_FROM_DEVICE);
		break;

	case G2D_DMA_CACHE_CLEAN:
		dma_cache_maint(vaddr, dma_info.size, DMA_TO_DEVICE);
		break;

	case G2D_DMA_CACHE_FLUSH:
		dma_cache_maint(vaddr, dma_info.size, DMA_BIDIRECTIONAL);
		break;

	default:
		break;
	}

	return 0;
}

struct file_operations g2d_fops = {
	.owner		= THIS_MODULE,
	.open		= g2d_open,
	.release	= g2d_release,
	.mmap		= g2d_mmap,
	.poll		= g2d_poll,
	.ioctl		= g2d_ioctl,
};

static struct miscdevice g2d_dev = {
	.minor		= G2D_MINOR,
	.name		= "g2d",
	.fops		= &g2d_fops,
};

static int g2d_probe(struct platform_device *pdev)
{
	struct resource *res;
	int ret;

	/* global structure */
	g2d = kzalloc(sizeof(*g2d), GFP_KERNEL);
	if (!g2d) {
		dev_err(&pdev->dev, "no memory for g2d info\n");
		ret = -ENOMEM;
		goto err_no_mem;
	}

	/* memory region */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!res) {
		dev_err(&pdev->dev, "failed to get resource\n");
		ret = -ENOENT;
		goto err_res;
	}

	g2d->mem = request_mem_region(res->start,
			((res->end) - (res->start)) + 1, pdev->name);
	if (!g2d->mem) {
		dev_err(&pdev->dev, "failed to request memory region\n");
		ret = -ENOMEM;
		goto err_res;
	}

	/* ioremap */
	g2d->base = ioremap(res->start, res->end - res->start + 1);
	if (!g2d->base) {
		dev_err(&pdev->dev, "failed to ioremap\n");
		ret = -ENOENT;
		goto err_map;
	}

	/* irq */
	g2d->irq = platform_get_irq(pdev, 0);
	if (!g2d->irq) {
		dev_err(&pdev->dev, "failed to get irq resource\n");
		ret = -ENOENT;
		goto err_map;
	}

	ret = request_irq(g2d->irq, g2d_irq, IRQF_DISABLED, pdev->name, NULL);
	if (ret) {
		dev_err(&pdev->dev, "failed to request_irq(g2d)\n");
		ret = -ENOENT;
		goto err_res;
	}

	/* clock */
	g2d->clock = clk_get(&pdev->dev, "g2d");
	if(IS_ERR(g2d->clock)) {
		dev_err(&pdev->dev, "failed to enable clock\n");
		ret = -ENOENT;
		goto err_clk;
	}
	clk_enable(g2d->clock);
	/* blocking I/O */
	init_waitqueue_head(&g2d->wq);

	/* atomic init */
	atomic_set(&g2d->in_use, 0);

	/* misc register */
	ret = misc_register(&g2d_dev);
	if (ret) {
		dev_err(&pdev->dev, "failed to register misc driver\n");
		goto err_reg;
	}

	/* mutex */
	g2d->lock = (struct mutex *) kmalloc(sizeof(*g2d->lock), GFP_KERNEL);
	if (IS_ERR(g2d->lock)) {
		dev_err(&pdev->dev, "failed to initialize mutex\n");
		ret = -ENOENT;
		goto err_lock;
	}

	mutex_init(g2d->lock);

	/* device */
	g2d->dev = &pdev->dev;

	dev_info(&pdev->dev, "g2d driver loaded successfully\n");

	return 0;

err_lock:
	misc_deregister(&g2d_dev);

err_reg:
	clk_put(g2d->clock);

err_clk:
	iounmap(g2d->base);

err_map:
	release_resource(g2d->mem);
	kfree(g2d->mem);

err_res:
	free_irq(g2d->irq, NULL);

err_no_mem:
	kfree(g2d);
	return ret;
}

static int g2d_remove(struct platform_device *pdev)
{
	free_irq(g2d->irq, NULL);

	if (g2d->mem) {
		iounmap(g2d->base);
		release_resource(g2d->mem);
		kfree(g2d->mem);
	}

	misc_deregister(&g2d_dev);

	return 0;
}

static int g2d_suspend(struct platform_device *pdev, pm_message_t state)
{
	clk_disable(g2d->clock);

	return 0;
}
static int g2d_resume(struct platform_device *pdev)
{
	clk_enable(g2d->clock);

	return 0;
}

static struct platform_driver g2d_driver = {
	.probe		= g2d_probe,
	.remove		= g2d_remove,
	.suspend	= g2d_suspend,
	.resume		= g2d_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "g2d",
	},
};

static int __init g2d_register(void)
{
	platform_driver_register(&g2d_driver);

	return 0;
}

static void __exit g2d_unregister(void)
{
	platform_driver_unregister(&g2d_driver);
}

module_init(g2d_register);
module_exit(g2d_unregister);

MODULE_AUTHOR("Jinhee Hyeon <jh0722.hyen@samsung.com>");
MODULE_AUTHOR("Jinsung Yang <jsgood.yang@samsung.com>");
MODULE_DESCRIPTION("Samsung FIMG-2D v3.0 Device Driver");
MODULE_LICENSE("GPL");

