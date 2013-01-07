/* linux/arch/arm/mach-s5pv210/leds.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/clk.h>
#include <linux/mutex.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/leds.h>
#include <asm/mach-types.h>

#include <plat/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/gpio-bank-h0.h>

#include <mach/map.h>
#include <asm/mach/irq.h>
#include "leds.h"

static irqreturn_t eint11_switch(int irq, void *dev_id)
{
	
	printk("EINT11 interrupt occures!!!\n");
	return IRQ_HANDLED;
}


static int __init
s5pv210_leds_init(void)
{
	if (machine_is_SMDKV210())
		leds_event = s5pv210_leds_event;
	else
		return -1;

	if (machine_is_SMDKV210())
	{
               gpio_request(S5PV2XX_GPH1(4), "GPH1");
               gpio_direction_output(S5PV2XX_GPH1(4), 1);
               if(gpio_get_value(S5PV2XX_GPH1(4)) == 0)
               {
                      printk(KERN_WARNING "LED: can't set GPH1(4) to output mode\n");
               }

               gpio_request(S5PV2XX_GPH1(5), "GPH1");
               gpio_direction_output(S5PV2XX_GPH1(5), 1);
               if(gpio_get_value(S5PV2XX_GPH1(5)) == 0)
               {
                      printk(KERN_WARNING "LED: can't set GPH1(5) to output mode\n");
               }

               gpio_request(S5PV2XX_GPH1(6), "GPH1");
               gpio_direction_output(S5PV2XX_GPH1(6), 1);
               if(gpio_get_value(S5PV2XX_GPH1(6)) == 0)
               {
                      printk(KERN_WARNING "LED: can't set GPH1(6) to output mode\n");
               }

               gpio_request(S5PV2XX_GPH1(7), "GPH1");
               gpio_direction_output(S5PV2XX_GPH1(7), 1);
               if(gpio_get_value(S5PV2XX_GPH1(7)) == 0)
               {
                      printk(KERN_WARNING "LED: can't set GPH1(7) to output mode\n");
               }
	}

	/* Get irqs */
	set_irq_type(IRQ_EINT11, IRQ_TYPE_EDGE_FALLING);
	s3c_gpio_setpull(S5PV2XX_GPH1(3), S3C_GPIO_PULL_NONE);
        if (request_irq(IRQ_EINT11, eint11_switch, IRQF_DISABLED, "EINT11", NULL)) {
                return -EIO;
        }

	leds_event(led_start);
	return 0;
}

__initcall(s5pv210_leds_init);