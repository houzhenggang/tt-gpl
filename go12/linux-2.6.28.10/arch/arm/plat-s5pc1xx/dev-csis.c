/* linux/arch/arm/plat-s5pc1xx/dev-csis.c
 *
 * Copyright 2009 Samsung Electronics
 *	Jinsung Yang <jsgood.yang@samsung.com>
 *	http://samsungsemi.com/
 *
 * S5PC1XX series device definition for MIPI-CSI2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/platform_device.h>

#include <mach/map.h>

#include <plat/csis.h>
#include <plat/devs.h>
#include <plat/cpu.h>

static struct resource s3c_csis_resource[] = {
	[0] = {
		.start = S5PC1XX_PA_CSIS,
		.end   = S5PC1XX_PA_CSIS + S5PC1XX_SZ_CSIS - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_MIPICSI,
		.end   = IRQ_MIPICSI,
		.flags = IORESOURCE_IRQ,
	},
};

struct platform_device s3c_device_csis = {
	.name		  = "s3c-csis",
	.id		  = 0,
	.num_resources	  = ARRAY_SIZE(s3c_csis_resource),
	.resource	  = s3c_csis_resource,
};

static struct s3c_platform_csis default_csis_data __initdata = {
	.clk_name = "mipi-csis",
};

void __init s3c_csis_set_platdata(struct s3c_platform_csis *pd)
{
	struct s3c_platform_csis *npd;

	if (!pd)
		pd = &default_csis_data;

	npd = kmemdup(pd, sizeof(struct s3c_platform_csis), GFP_KERNEL);
	if (!npd)
		printk(KERN_ERR "%s: no memory for platform data\n", __func__);

	npd->cfg_gpio = s3c_csis_cfg_gpio;
	npd->cfg_phy_global = s3c_csis_cfg_phy_global;

	s3c_device_csis.dev.platform_data = npd;
}

