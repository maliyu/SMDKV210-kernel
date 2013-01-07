/* linux/arch/arm/plat-s5pv2xx/include/plat/gpio-bank-e0.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * S5PV2XX GPIO BANK register
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#define S5PV2XX_GPE0CON			(S5PV2XX_GPE0_BASE + 0x00)
#define S5PV2XX_GPE0DAT			(S5PV2XX_GPE0_BASE + 0x04)
#define S5PV2XX_GPE0PUD			(S5PV2XX_GPE0_BASE + 0x08)
#define S5PV2XX_GPE0DRV			(S5PV2XX_GPE0_BASE + 0x0c)
#define S5PV2XX_GPE0CONPDN		(S5PV2XX_GPE0_BASE + 0x10)
#define S5PV2XX_GPE0PUDPDN		(S5PV2XX_GPE0_BASE + 0x14)

#define S5PV2XX_GPE0_CONMASK(__gpio)	(0xf << ((__gpio) * 4))
#define S5PV2XX_GPE0_INPUT(__gpio)	(0x0 << ((__gpio) * 4))
#define S5PV2XX_GPE0_OUTPUT(__gpio)	(0x1 << ((__gpio) * 4))

#define S5PV2XX_GPE0_0_CAM_A_PCLK	(0x2 << 0)
#define S5PV2XX_GPE0_0_GPIO_INT8_0	(0xf << 0)

#define S5PV2XX_GPE0_1_CAM_A_VSYNC	(0x2 << 4)
#define S5PV2XX_GPE0_1_GPIO_INT8_1	(0xf << 4)

#define S5PV2XX_GPE0_2_CAM_A_HREF	(0x2 << 8)
#define S5PV2XX_GPE0_2_GPIO_INT8_2	(0xf << 8)

#define S5PV2XX_GPE0_3_CAM_A_DATA_0	(0x2 << 12)
#define S5PV2XX_GPE0_3_GPIO_INT8_3	(0xf << 12)

#define S5PV2XX_GPE0_4_CAM_A_DATA_1	(0x2 << 16)
#define S5PV2XX_GPE0_4_GPIO_INT8_4	(0xf << 16)

#define S5PV2XX_GPE0_5_CAM_A_DATA_2	(0x2 << 20)
#define S5PV2XX_GPE0_5_GPIO_INT8_5	(0xf << 20)

#define S5PV2XX_GPE0_6_CAM_A_DATA_3	(0x2 << 24)
#define S5PV2XX_GPE0_6_GPIO_INT8_6	(0xf << 24)

#define S5PV2XX_GPE0_7_CAM_A_DATA_4	(0x2 << 28)
#define S5PV2XX_GPE0_7_GPIO_INT8_7	(0xf << 28)
