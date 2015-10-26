/*****************************************************************************
* Copyright 2003 - 2009 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/
#include <linux/platform_device.h>
#include <linux/io.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <linux/pci_ids.h>
#include <linux/gpio.h>
#include <linux/vgpio.h>
#include <mach/pinmux.h>
#include <mach/hw_cfg.h>

#include <plat/bcm476x.h>
#include <plat/irvine.h>
#include <plat/irvine-lcd.h>
#include <plat/irvine-pmu.h>
#include <plat/irvine-usb.h>
#include <plat/irvine-sound.h>
#include <plat/irvine-lcd-data.h>
#include <plat/irvine-regulator.h>

#include <mach/bcm59040-pnd.h>

/* Virtual GPIO definitions */
static struct vgpio_pin catania_b_vgpio0_pins[] = 
{
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_BACKLIGHT_PWM,	13),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_BACKLIGHT_ENABLE, 	12),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_ON,		 	24),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_ID,			25),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_SCLK, 		66),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_SDI, 			69),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_CS, 			23),

	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_PMU_IRQ,			1),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_BT_EN,			15),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_BT_RST,			20),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_KILL_PWRn,		4),
	VGPIO_DEF_INVPIN(TT_VGPIO_BASE, TT_VGPIO_USB_HOST_DETECT,	2),	
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_PB,			0),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_EEPROM_WP,		14),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LOW_DC_VCC,		21),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_AMP_PWR_EN,		22),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_TILT_PWR,			19),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_TILT_OUT, 		18),

	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_MMC0_CD),
	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_WL_RST),
	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_REG_ON),
	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_MMC1_CD),
	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_MMC2_CD),
};

static struct vgpio_platform_data catania_b_vgpio_pdata[] = 
{
	[0] = 
	{
		.gpio_base	= TT_VGPIO_BASE,
		.gpio_number	= ARRAY_SIZE(catania_b_vgpio0_pins),
		.pins		= catania_b_vgpio0_pins,
	},
};

static struct platform_device catania_b_vgpio[] = 
{
	[0] = 
	{
		.name	= "vgpio",
		.id	= 0,
		.dev	= {
			.platform_data  = &catania_b_vgpio_pdata[0],
		},
	},
};

/* Regulator consumers */
static struct regulator_bulk_data _catania_b_consumer_supplies[] = 
{
        FOR_ALL_REGULATORS(catania_b,IRVINE_CONSUMER_SUPPLY)
};

static struct regulator_userspace_consumer_data _catania_b_consumer_data[] = 
{
        FOR_ALL_REGULATORS(catania_b,IRVINE_CONSUMER_DATA)
};

static struct platform_device _catania_b_userspace_consumers[] = {
        FOR_ALL_REGULATORS(catania_b,_IRVINE_USERSPACE_CONSUMER)
};

IRVINE_USERSPACE_CONSUMER_SUPPLY_NULL(catania_b, LDO1,1);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(catania_b, LDO2,1);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(catania_b, LDO3,2);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(catania_b, LDO4,3);
IRVINE_USERSPACE_CONSUMER_SUPPLY_NULL(catania_b, LDO5,1);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(catania_b, LDO6,1);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(catania_b, CSR ,6);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(catania_b, IOSR,7);

IRVINE_REGULATOR(catania_b,LDO1, 3200000, SUSPEND_OFF, BOOT_OFF);
IRVINE_REGULATOR(catania_b,LDO2, 2500000, SUSPEND_OFF, BOOT_ON);
IRVINE_REGULATOR(catania_b,LDO3, 3000000, SUSPEND_OFF, BOOT_ON);
IRVINE_REGULATOR(catania_b,LDO4, 3200000, SUSPEND_OFF, BOOT_ON);
IRVINE_REGULATOR(catania_b,LDO5, 3200000, SUSPEND_ON,  BOOT_ON);
IRVINE_REGULATOR(catania_b,LDO6, 3200000, SUSPEND_OFF, BOOT_ON);
IRVINE_REGULATOR_DVS(catania_b,CSR,   900000, 1340000,       0, BOOT_ON);
IRVINE_REGULATOR_DVS(catania_b,IOSR, 1710000, 1890000, 1800000, BOOT_ON);

static struct platform_device catania_b_device_regulators[] = 
{
	[0] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO1_ID,
		.dev =
		{
			.platform_data	= &_catania_b_LDO1_data,
		},
	},
	[1] =
	{
		.name   = "bcmpmu_regulator",
		.id	= BCM59040_LDO2_ID,
		.dev =
		{
			.platform_data  = &_catania_b_LDO2_data,
		},
	},
	[2] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO3_ID,
		.dev =
		{
			.platform_data  = &_catania_b_LDO3_data,
		},
	},
	[3] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO4_ID,
		.dev =
		{
			.platform_data  = &_catania_b_LDO4_data,
		},
	},
	[4] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO5_ID,
		.dev =
		{
			.platform_data  = &_catania_b_LDO5_data,
		},
	},
	[5] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO6_ID,
		.dev =
		{
			.platform_data  = &_catania_b_LDO6_data,
		},
	},
	[6] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_CSR_ID,
		.dev =
		{
			.platform_data  = &_catania_b_CSR_data,
		},
	},
	[7] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_IOSR_ID,
		.dev =
		{
			.platform_data  = &_catania_b_IOSR_data,
		},
	},
};

static void __init catania_b_lcd_init(void)
{
#ifdef CONFIG_FB_BCM476X_CLCD
	irvine_clcd_register(&LMS430_WithPLL);
	irvine_clcd_register(&LMS500_WithPLL);
	irvine_clcd_register(&A043FW05V4_WithPLL);
	irvine_clcd_register(&A050FW03V4_WithPLL);
#endif
}

static void __init catania_b_init_machine( void )
{
	volatile uint32_t val;

	irvine_init_machine ();

	bcm59040_register_pnd_defaults();
	catania_b_lcd_init();

	platform_device_register(&catania_b_vgpio[0]);

	/* Enable I2S MCLK out on GPIO27 pin */
	val = readl(IO_ADDRESS(CMU_R_CHIP_PIN_MUX6_MEMADDR)) & ~CMU_F_GPIO_27_MXSEL_MASK;
	writel(val | (3 << CMU_F_GPIO_27_MXSEL_R), IO_ADDRESS(CMU_R_CHIP_PIN_MUX6_MEMADDR));

	/* Force I2S to 12Mhz */
	val = readl(IO_ADDRESS(CMU_R_CLOCK_GATE_CTL_MEMADDR));
	writel(val | CMU_F_CKG_I2S_SEL_12MHZ_MASK, IO_ADDRESS(CMU_R_CLOCK_GATE_CTL_MEMADDR));

	/* Set drive strength to 4mA for LCD HSYNC */
	val = readl(IO_ADDRESS(GIO1_R_GPCTR62_MEMADDR)) & ~GIO1_F_SEL_MASK;
	writel(val | (0x2 << GIO1_F_SEL_R), IO_ADDRESS(GIO1_R_GPCTR62_MEMADDR));

	/* Set drive strength to 4mA for LCD VSYNC */
	val = readl(IO_ADDRESS(GIO1_R_GPCTR63_MEMADDR)) & ~GIO1_F_SEL_MASK;
	writel(val | (0x2 << GIO1_F_SEL_R), IO_ADDRESS(GIO1_R_GPCTR63_MEMADDR));

	/* Set drive strength to 4mA for LCD pixel CLK */
	val = readl(IO_ADDRESS(GIO1_R_GPCTR65_MEMADDR)) & ~GIO1_F_SEL_MASK;
	writel(val | (0x2 << GIO1_F_SEL_R), IO_ADDRESS(GIO1_R_GPCTR65_MEMADDR));

	/* I2S_SDO connected to BT_UART_RTS */
//	val = readl(IO_ADDRESS(CMU_R_CHIP_PIN_MUX6_MEMADDR)) & ~CMU_F_GPIO_23_MXSEL_MASK;
//	writel(val | (1 << CMU_F_GPIO_23_MXSEL_R), IO_ADDRESS(CMU_R_CHIP_PIN_MUX6_MEMADDR));

	/* OFFVBUS is BT_UART_CTS */
//	val = readl(IO_ADDRESS(CMU_R_CHIP_PIN_MUX10_MEMADDR)) & ~CMU_F_USB_OFFVBUSN_MXSEL_MASK;
//	writel(val | (1 << CMU_F_USB_OFFVBUSN_MXSEL_R), IO_ADDRESS(CMU_R_CHIP_PIN_MUX10_MEMADDR));

	/* bypassing BCD (charger detect) */
	/* printk(KERN_ERR"WRITING 6 CMU_R_BC_DETECT_CTRL_MEMADDR\n"); */
	writel((CMU_F_BCD_SYS_RDY_MASK | CMU_F_BCD_SW_OPEN_MASK), IO_ADDRESS(CMU_R_BC_DETECT_CTRL_MEMADDR));

	platform_device_register(&_catania_b_userspace_consumers[0]);
	platform_device_register(&_catania_b_userspace_consumers[1]);
	platform_device_register(&_catania_b_userspace_consumers[2]);
	platform_device_register(&_catania_b_userspace_consumers[3]);
	platform_device_register(&_catania_b_userspace_consumers[4]);
	platform_device_register(&_catania_b_userspace_consumers[5]);
	platform_device_register(&_catania_b_userspace_consumers[6]);
	platform_device_register(&_catania_b_userspace_consumers[7]);

	platform_device_register(&catania_b_device_regulators[0]);
	platform_device_register(&catania_b_device_regulators[1]);
	platform_device_register(&catania_b_device_regulators[2]);
	platform_device_register(&catania_b_device_regulators[3]);
	platform_device_register(&catania_b_device_regulators[4]);
	platform_device_register(&catania_b_device_regulators[5]);
	platform_device_register(&catania_b_device_regulators[6]);
	platform_device_register(&catania_b_device_regulators[7]);

#ifdef CONFIG_SOUND
	irvine_sound_init();
#endif

	bcm476x_set_pci_emu_devid (BCM_SD0, PCI_DEVICE_ID_BCM_SD);
	bcm476x_set_pci_emu_devid (BCM_SD1, BCM_SD_DISABLE);
	bcm476x_set_pci_emu_devid (BCM_SD2, BCM_SD_DISABLE);
}

/****************************************************************************
*
*   Machine Description
:*
*****************************************************************************/

MACHINE_START(CATANIA_B, "CATANIA_B")
	.phys_io 	= BCM47XX_ARM_PERIPH_BASE,
	.io_pg_offst 	= (IO_ADDRESS(BCM47XX_ARM_PERIPH_BASE) >> 18) & 0xfffc,
	.boot_params 	= (BCM47XX_ARM_DRAM + 0x100),
	.map_io 	= bcm476x_map_io,
	.init_irq 	= bcm476x_init_irq,
	.timer  	= &bcm476x_timer,
	.init_machine 	= catania_b_init_machine,
MACHINE_END


