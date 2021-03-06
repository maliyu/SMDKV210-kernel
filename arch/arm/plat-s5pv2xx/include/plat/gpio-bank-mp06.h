/* linux/arch/arm/plat-s5pc1xx/include/plat/gpio-bank-mp06.h
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

#define S5PV2XX_MP06CON			(S5PV2XX_MP06_BASE + 0x00)
#define S5PV2XX_MP06DAT			(S5PV2XX_MP06_BASE + 0x04)
#define S5PV2XX_MP06PUD			(S5PV2XX_MP06_BASE + 0x08)
#define S5PV2XX_MP06DRV			(S5PV2XX_MP06_BASE + 0x0c)
#define S5PV2XX_MP06CONPDN		(S5PV2XX_MP06_BASE + 0x10)
#define S5PV2XX_MP06PUDPDN		(S5PV2XX_MP06_BASE + 0x14)

#define S5PV2XX_MP06_CONMASK(__gpio)	(0xf << ((__gpio) * 4))
#define S5PV2XX_MP06_INPUT(__gpio)	(0x0 << ((__gpio) * 4))
#define S5PV2XX_MP06_OUTPUT(__gpio)	(0x1 << ((__gpio) * 4))

#define S5PV2XX_MP06_0_EBI_DATA_0	(0x2 << 0)
#define S5PV2XX_MP06_1_EBI_DATA_1	(0x2 << 4)
#define S5PV2XX_MP06_2_EBI_DATA_2	(0x2 << 8)
#define S5PV2XX_MP06_3_EBI_DATA_3	(0x2 << 12)
#define S5PV2XX_MP06_4_EBI_DATA_4	(0x2 << 16)
#define S5PV2XX_MP06_5_EBI_DATA_5	(0x2 << 20)
#define S5PV2XX_MP06_6_EBI_DATA_6	(0x2 << 24)
#define S5PV2XX_MP06_7_EBI_DATA_7	(0x2 << 28)

