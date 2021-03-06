/* linux/arch/arm/plat-s5pc1xx/include/plat/gpio-bank-mp21.h
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

#define S5PV2XX_MP21CON			(S5PV2XX_MP21_BASE + 0x00)
#define S5PV2XX_MP21DAT			(S5PV2XX_MP21_BASE + 0x04)
#define S5PV2XX_MP21PUD			(S5PV2XX_MP21_BASE + 0x08)
#define S5PV2XX_MP21DRV			(S5PV2XX_MP21_BASE + 0x0c)
#define S5PV2XX_MP21CONPDN		(S5PV2XX_MP21_BASE + 0x10)
#define S5PV2XX_MP21PUDPDN		(S5PV2XX_MP21_BASE + 0x14)

#define S5PV2XX_MP21_CONMASK(__gpio)	(0xf << ((__gpio) * 4))
#define S5PV2XX_MP21_INPUT(__gpio)	(0x0 << ((__gpio) * 4))
#define S5PV2XX_MP21_OUTPUT(__gpio)	(0x1 << ((__gpio) * 4))

#define S5PV2XX_MP21_0_LD1_ADDR_8	(0x2 << 0)
#define S5PV2XX_MP21_1_LD1_ADDR_9	(0x2 << 4)
#define S5PV2XX_MP21_2_LD1_ADDR_10	(0x2 << 8)
#define S5PV2XX_MP21_3_LD1_ADDR_11	(0x2 << 12)
#define S5PV2XX_MP21_4_LD1_ADDR_12	(0x2 << 16)
#define S5PV2XX_MP21_5_LD1_ADDR_13	(0x2 << 20)
#define S5PV2XX_MP21_6_LD1_ADDR_14	(0x2 << 24)
#define S5PV2XX_MP21_7_LD1_ADDR_15	(0x2 << 28)

