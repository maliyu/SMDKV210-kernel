/*
 * linux/arch/arm/mach-s5pv210/setup-sdhci.c
 *
 * Copyright 2008 Simtec Electronics
 * Copyright 2008 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *	http://armlinux.simtec.co.uk/
 *
 * S5PV210 - Helper functions for settign up SDHCI device(s) (HSMMC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/irq.h>

#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/regs-sdhci.h>
#include <plat/sdhci.h>
#include <mach/map.h>
#include <plat/regs-gpio.h>
#include <plat/gpio-bank-g2.h>

#define S3C_SDHCI_CTRL3_FCSELTX_INVERT (0)
#define S3C_SDHCI_CTRL3_FCSELTX_BASIC (S3C_SDHCI_CTRL3_FCSEL3 | S3C_SDHCI_CTRL3_FCSEL2)
#define S3C_SDHCI_CTRL3_FCSELRX_INVERT (0)
#define S3C_SDHCI_CTRL3_FCSELRX_BASIC (S3C_SDHCI_CTRL3_FCSEL1 | S3C_SDHCI_CTRL3_FCSEL0)

/* clock sources for the mmc bus clock, order as for the ctrl2[5..4] */
char *s3c6410_hsmmc_clksrcs[4] = {
	[0] = "hsmmc",
	[1] = "hsmmc",
	[2] = "mmc_bus",
};

void s3c64xx_setup_sdhci0_cfg_gpio(struct platform_device *dev, int width)
{
	unsigned int gpio;

	switch(width)
	{
	/* Channel 0 supports 4 and 8-bit bus width */
	case 8:
	        /* Set all the necessary GPIO function and pull up/down */
	        for (gpio = S5PV2XX_GPG1(3); gpio <= S5PV2XX_GPG1(6); gpio++) {
	                s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
	                s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
	        }

		writel(0x3fc0, S5PV2XX_GPG1DRV);

	case 0:
	case 1:
	case 4:
	        /* Set all the necessary GPIO function and pull up/down */
	        for (gpio = S5PV2XX_GPG0(0); gpio <= S5PV2XX_GPG0(6); gpio++) {
	                s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(2));
	                s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
	        }

		writel(0x3fff, S5PV2XX_GPG0DRV);

	        /* Chip detect pin Pull up*/
		s3c_gpio_setpull(S5PV2XX_GPG0(2), S3C_GPIO_PULL_UP);

		break;
	default:
		printk("Wrong SD/MMC bus width : %d\n", width);
	}
}

void s3c6410_setup_sdhci0_cfg_card(struct platform_device *dev,
				    void __iomem *r,
				    struct mmc_ios *ios,
				    struct mmc_card *card)
{
	u32 ctrl2;
	u32 ctrl3;

	/* don't need to alter anything acording to card-type */

	writel(S3C64XX_SDHCI_CONTROL4_DRIVE_9mA, r + S3C64XX_SDHCI_CONTROL4);

        /* No need for any delay values in the HS-MMC interface */
	ctrl2 = readl(r + S3C_SDHCI_CONTROL2);
	ctrl2 &= S3C_SDHCI_CTRL2_SELBASECLK_MASK;
	ctrl2 |= (S3C64XX_SDHCI_CTRL2_ENSTAASYNCCLR |
		  S3C64XX_SDHCI_CTRL2_ENCMDCNFMSK |
		  S3C_SDHCI_CTRL2_DFCNT_NONE |
		  S3C_SDHCI_CTRL2_ENCLKOUTHOLD);

        if (ios->clock <= (400 * 1024)) {
		ctrl2 &= ~(S3C_SDHCI_CTRL2_ENFBCLKTX | S3C_SDHCI_CTRL2_ENFBCLKRX);
		ctrl3 = 0;
	} else {
		u32 range_start;
		u32 range_end;

		ctrl2 |= S3C_SDHCI_CTRL2_ENFBCLKTX | S3C_SDHCI_CTRL2_ENFBCLKRX;

		if (card->type & MMC_TYPE_MMC)		/* MMC */
			range_start = 20 * 1024 * 1024;
		else					/* SD */
			range_start = 25 * 1024 * 1024;

		range_end = 37 * 1024 * 1024;

		if ((ios->clock > range_start) && (ios->clock < range_end))
			ctrl3 = S3C_SDHCI_CTRL3_FCSELTX_BASIC |
				S3C_SDHCI_CTRL3_FCSELRX_BASIC;
		else
			ctrl3 = S3C_SDHCI_CTRL3_FCSELTX_BASIC |
				S3C_SDHCI_CTRL3_FCSELRX_INVERT;
	}

	writel(ctrl2, r + S3C_SDHCI_CONTROL2);
	writel(ctrl3, r + S3C_SDHCI_CONTROL3);
}

void s3c64xx_setup_sdhci1_cfg_gpio(struct platform_device *dev, int width)
{
	unsigned int gpio;

	switch(width)
	{
	/* Channel 1 supports 4-bit bus width */
	case 0:
	case 1:
	case 4:
	        /* Set all the necessary GPIO function and pull up/down */
	        for (gpio = S5PV2XX_GPG1(0); gpio <= S5PV2XX_GPG1(6); gpio++) {
	                s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(2));
	                s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
	        }

		writel(0x3fff, S5PV2XX_GPG1DRV);

	        /* Chip detect pin Pull up*/
		s3c_gpio_setpull(S5PV2XX_GPG1(2), S3C_GPIO_PULL_UP);

		break;
	default:
		printk("Wrong SD/MMC bus width : %d\n", width);
	}
}

void s3c64xx_setup_sdhci2_cfg_gpio(struct platform_device *dev, int width)
{
	unsigned int gpio;

	switch(width)
	{
	/* Channel 2 supports 4 and 8-bit bus width */
	case 8:
	        /* Set all the necessary GPIO function and pull up/down */
	        for (gpio = S5PV2XX_GPG3(3); gpio <= S5PV2XX_GPG3(6); gpio++) {
	                s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
	                s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
	        }

		writel(0x3fc0, S5PV2XX_GPG3DRV);
	case 0:
	case 1:
	case 4:
	        /* Set all the necessary GPIO function and pull up/down */
	        for (gpio = S5PV2XX_GPG2(0); gpio <= S5PV2XX_GPG2(6); gpio++) {
	                s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(2));
	                s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
	        }

		writel(0x3fff, S5PV2XX_GPG2DRV);

	        /* Chip detect pin Pull up*/
		s3c_gpio_setpull(S5PV2XX_GPG2(2), S3C_GPIO_PULL_UP);

		break;
	default:
		printk("Wrong SD/MMC bus width : %d\n", width);
	}
}

void s3c64xx_setup_sdhci3_cfg_gpio(struct platform_device *dev, int width)
{
	unsigned int gpio;

	switch(width)
	{
	/* Channel 3 supports 4-bit bus width */
	case 0:
	case 1:
	case 4:
	        /* Set all the necessary GPIO function and pull up/down */
	        for (gpio = S5PV2XX_GPG3(0); gpio <= S5PV2XX_GPG3(6); gpio++) {
	                s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(2));
	                s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
	        }

		writel(0x3fff, S5PV2XX_GPG3DRV);

	        /* Chip detect pin Pull up*/
		s3c_gpio_setpull(S5PV2XX_GPG3(2), S3C_GPIO_PULL_UP);

		break;
	default:
		printk("Wrong SD/MMC bus width : %d\n", width);
	}
}

#if defined(CONFIG_SMDKV210_SD_CH0_8BIT)
static struct s3c_sdhci_platdata hsmmc0_platdata = {
	.max_width	= 8,
	.host_caps	= (MMC_CAP_8_BIT_DATA | MMC_CAP_ON_BOARD |
			   MMC_CAP_BOOT_ONTHEFLY | MMC_CAP_MMC_HIGHSPEED |
			   MMC_CAP_SD_HIGHSPEED),
};
#endif

#if defined(CONFIG_SMDKV210_SD_CH2_8BIT)
static struct s3c_sdhci_platdata hsmmc2_platdata = {
	.max_width	= 8,
	.host_caps	= (MMC_CAP_8_BIT_DATA | MMC_CAP_ON_BOARD |
			   MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED),
};
#endif

void s3c_sdhci_set_platdata(void)
{
#if defined(CONFIG_SMDKV210_SD_CH0_8BIT)
	s3c_sdhci0_set_platdata(&hsmmc0_platdata);
#endif
#if defined(CONFIG_SMDKV210_SD_CH2_8BIT)
	s3c_sdhci2_set_platdata(&hsmmc2_platdata);
#endif
};