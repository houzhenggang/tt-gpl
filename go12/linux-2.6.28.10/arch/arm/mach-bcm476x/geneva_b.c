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

extern void register_pps_device(void);
extern unsigned int bcm476x_get_boot_sdmmc_device(void);

/* Virtual GPIO definitions */
static struct vgpio_pin geneva_b_vgpio0_pins[] = 
{
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_BACKLIGHT_PWM,	13),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_BACKLIGHT_ENABLE, 	12),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_ON,		 	39),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_PMU_IRQ,			1),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_BT_EN,			40),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_BT_RST,			16),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_KILL_PWRn,		4),
	VGPIO_DEF_INVPIN(TT_VGPIO_BASE, TT_VGPIO_USB_HOST_DETECT,	2),	
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_PB,			0),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_GSM_RESET,		19),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_GSM_POWER,		24),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_DOCK_RESET,		38),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_DOCK_DET0,		67),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_DOCK_DET1,		69),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_DOCK_I2C_PWR,		14),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_DOCK_FM_INT,		21),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_DOCK_MUTE,		66),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LOW_DC_VCC,		55),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_AMP_PWR_EN,		56),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_TSP_ATTN,			47),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_TSP_CE,			54),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_TSP_PWR,			18),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_LCD_ID,			25),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_DOCK_HPDETECT,		68),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_PPS,			61),
	VGPIO_DEF_PIN(TT_VGPIO_BASE, TT_VGPIO_SD_PWR_EN,		46),

	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_MMC0_CD),
	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_WL_RST),
	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_REG_ON),
	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_MMC1_CD),
	VGPIO_DEF_NCPIN(TT_VGPIO_BASE, TT_VGPIO_MMC2_CD),
};

static struct vgpio_platform_data geneva_b_vgpio_pdata[] = 
{
	[0] = 
	{
		.gpio_base	= TT_VGPIO_BASE,
		.gpio_number	= ARRAY_SIZE(geneva_b_vgpio0_pins),
		.pins		= geneva_b_vgpio0_pins,
	},
};

static struct platform_device geneva_b_vgpio[] = 
{
	[0] = 
	{
		.name	= "vgpio",
		.id	= 0,
		.dev	= {
			.platform_data  = &geneva_b_vgpio_pdata[0],
		},
	},
};

/* Regulator consumers */
static struct regulator_bulk_data _geneva_b_consumer_supplies[] = 
{
        FOR_ALL_REGULATORS(geneva_b,IRVINE_CONSUMER_SUPPLY)
};

static struct regulator_userspace_consumer_data _geneva_b_consumer_data[] = 
{
        FOR_ALL_REGULATORS(geneva_b,IRVINE_CONSUMER_DATA)
};

static struct platform_device _geneva_b_userspace_consumers[] = {
        FOR_ALL_REGULATORS(geneva_b,_IRVINE_USERSPACE_CONSUMER)
};

IRVINE_USERSPACE_CONSUMER_SUPPLY_NULL(geneva_b, LDO1,1);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(geneva_b, LDO2,1);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(geneva_b, LDO3,2);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(geneva_b, LDO4,3);
IRVINE_USERSPACE_CONSUMER_SUPPLY_NULL(geneva_b, LDO5,1);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(geneva_b, LDO6,1);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(geneva_b, CSR ,6);
IRVINE_USERSPACE_CONSUMER_SUPPLY_1(geneva_b, IOSR,7);

IRVINE_REGULATOR(geneva_b,LDO1, 3200000, SUSPEND_OFF, BOOT_OFF);
IRVINE_REGULATOR(geneva_b,LDO2, 2500000, SUSPEND_OFF, BOOT_ON);
IRVINE_REGULATOR(geneva_b,LDO3, 3000000, SUSPEND_OFF, BOOT_ON);
IRVINE_REGULATOR(geneva_b,LDO4, 3200000, SUSPEND_OFF, BOOT_ON);
IRVINE_REGULATOR(geneva_b,LDO5, 3200000, SUSPEND_ON,  BOOT_ON);
IRVINE_REGULATOR(geneva_b,LDO6, 3200000, SUSPEND_OFF, BOOT_ON);
IRVINE_REGULATOR_DVS(geneva_b,CSR,   900000, 1340000,       0, BOOT_ON);
IRVINE_REGULATOR_DVS(geneva_b,IOSR, 1710000, 1890000, 1800000, BOOT_ON);

static struct platform_device geneva_b_device_regulators[] = 
{
	[0] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO1_ID,
		.dev =
		{
			.platform_data	= &_geneva_b_LDO1_data,
		},
	},
	[1] =
	{
		.name   = "bcmpmu_regulator",
		.id	= BCM59040_LDO2_ID,
		.dev =
		{
			.platform_data  = &_geneva_b_LDO2_data,
		},
	},
	[2] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO3_ID,
		.dev =
		{
			.platform_data  = &_geneva_b_LDO3_data,
		},
	},
	[3] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO4_ID,
		.dev =
		{
			.platform_data  = &_geneva_b_LDO4_data,
		},
	},
	[4] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO5_ID,
		.dev =
		{
			.platform_data  = &_geneva_b_LDO5_data,
		},
	},
	[5] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_LDO6_ID,
		.dev =
		{
			.platform_data  = &_geneva_b_LDO6_data,
		},
	},
	[6] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_CSR_ID,
		.dev =
		{
			.platform_data  = &_geneva_b_CSR_data,
		},
	},
	[7] =
	{
		.name	= "bcmpmu_regulator",
		.id	= BCM59040_IOSR_ID,
		.dev =
		{
			.platform_data  = &_geneva_b_IOSR_data,
		},
	},
};

static void __init geneva_b_lcd_init(void)
{
#ifdef CONFIG_FB_BCM476X_CLCD
	irvine_clcd_register(&LMS430HF37_WithPLL);
	irvine_clcd_register(&LMS500HF10_WithPLL);
        irvine_clcd_register(&LB043WQ3_WithPLL);
        irvine_clcd_register(&LD050WQ1_WithPLL);
	irvine_clcd_register(&LMS430_WithPLL);
#endif
}

static void __init geneva_b_init_machine( void )
{
	volatile uint32_t val;

	irvine_init_machine ();

	bcm59040_register_pnd_defaults();
	geneva_b_lcd_init();

	platform_device_register(&geneva_b_vgpio[0]);

	/* LCD_G_0 -> GPIO_46 (SD_PWR_EN) */
	val = readl(IO_ADDRESS(CMU_R_CHIP_PIN_MUX1_MEMADDR)) & ~CMU_F_LCD_G_0_MXSEL_MASK;
	writel(val | (3 << CMU_F_LCD_G_0_MXSEL_R), IO_ADDRESS(CMU_R_CHIP_PIN_MUX1_MEMADDR));

	/* Enable I2S MCLK out on GPIO27 pin */
	val = readl(IO_ADDRESS(CMU_R_CHIP_PIN_MUX6_MEMADDR)) & ~CMU_F_GPIO_27_MXSEL_MASK;
	writel(val | (3 << CMU_F_GPIO_27_MXSEL_R), IO_ADDRESS(CMU_R_CHIP_PIN_MUX6_MEMADDR));

	/* Force I2S to 12Mhz */
	val = readl(IO_ADDRESS(CMU_R_CLOCK_GATE_CTL_MEMADDR));
	writel(val | CMU_F_CKG_I2S_SEL_12MHZ_MASK, IO_ADDRESS(CMU_R_CLOCK_GATE_CTL_MEMADDR));

	/* I2S_SDO connected to BT_UART_RTS */
	val = readl(IO_ADDRESS(CMU_R_CHIP_PIN_MUX6_MEMADDR)) & ~CMU_F_GPIO_23_MXSEL_MASK;
	writel(val | (1 << CMU_F_GPIO_23_MXSEL_R), IO_ADDRESS(CMU_R_CHIP_PIN_MUX6_MEMADDR));

	/* OFFVBUS is BT_UART_CTS */
	val = readl(IO_ADDRESS(CMU_R_CHIP_PIN_MUX10_MEMADDR)) & ~CMU_F_USB_OFFVBUSN_MXSEL_MASK;
	writel(val | (1 << CMU_F_USB_OFFVBUSN_MXSEL_R), IO_ADDRESS(CMU_R_CHIP_PIN_MUX10_MEMADDR));

	/* bypassing BCD (charger detect) */
	/* printk(KERN_ERR"WRITING 6 CMU_R_BC_DETECT_CTRL_MEMADDR\n"); */
	writel((CMU_F_BCD_SYS_RDY_MASK | CMU_F_BCD_SW_OPEN_MASK), IO_ADDRESS(CMU_R_BC_DETECT_CTRL_MEMADDR));

        /* 2mA drive strength for LCD HSYNC */
        val = readl(IO_ADDRESS(GIO1_R_GPCTR62_MEMADDR)) & ~GIO1_F_SEL_MASK;
        writel(val | (0x1 << GIO1_F_SEL_R), IO_ADDRESS(GIO1_R_GPCTR62_MEMADDR));

        /* 2mA drive strength for LCD VSYNC */
        val = readl(IO_ADDRESS(GIO1_R_GPCTR63_MEMADDR)) & ~GIO1_F_SEL_MASK;
        writel(val | (0x1 << GIO1_F_SEL_R), IO_ADDRESS(GIO1_R_GPCTR63_MEMADDR));

        /* 4mA drive strength for LCD pixel CLK */
        val = readl(IO_ADDRESS(GIO1_R_GPCTR65_MEMADDR)) & ~GIO1_F_SEL_MASK;
        writel(val | (0x2 << GIO1_F_SEL_R), IO_ADDRESS(GIO1_R_GPCTR65_MEMADDR));

	/* set GPIO 15 BT_EN to IO pin to avoid SD card write protect*/
	val = readl(IO_ADDRESS(CMU_R_CHIP_PIN_MUX5_MEMADDR)) & ~CMU_F_GPIO_15_MXSEL_MASK;
	writel(val | (0 << CMU_F_GPIO_15_MXSEL_R), IO_ADDRESS(CMU_R_CHIP_PIN_MUX5_MEMADDR));

	/*set GPIO 15 to input, pull down*/
	val = (readl(IO_ADDRESS(GIO1_R_GPCTR15_MEMADDR)) & ~0x4) | 0x1;
	writel(val, IO_ADDRESS(GIO1_R_GPCTR15_MEMADDR));

	platform_device_register(&_geneva_b_userspace_consumers[0]);
	platform_device_register(&_geneva_b_userspace_consumers[1]);
	platform_device_register(&_geneva_b_userspace_consumers[2]);
	platform_device_register(&_geneva_b_userspace_consumers[3]);
	platform_device_register(&_geneva_b_userspace_consumers[4]);
	platform_device_register(&_geneva_b_userspace_consumers[5]);
	platform_device_register(&_geneva_b_userspace_consumers[6]);
	platform_device_register(&_geneva_b_userspace_consumers[7]);

	platform_device_register(&geneva_b_device_regulators[0]);
	platform_device_register(&geneva_b_device_regulators[1]);
	platform_device_register(&geneva_b_device_regulators[2]);
	platform_device_register(&geneva_b_device_regulators[3]);
	platform_device_register(&geneva_b_device_regulators[4]);
	platform_device_register(&geneva_b_device_regulators[5]);
	platform_device_register(&geneva_b_device_regulators[6]);
	platform_device_register(&geneva_b_device_regulators[7]);

#ifdef CONFIG_SOUND
	irvine_sound_init();
#endif

	register_pps_device();
	bcm476x_set_pci_emu_devid (BCM_SD0, PCI_DEVICE_ID_BCM_SD);
	bcm476x_set_pci_emu_devid (BCM_SD1, PCI_DEVICE_ID_BCM_SD);
	bcm476x_set_pci_emu_devid (BCM_SD2, BCM_SD_DISABLE);

	if(bcm476x_get_boot_sdmmc_device() == 1)
	{
		/* SDIO1_CLK  - GPIO 76 enable HYSTEN   */
		val = readl(IO_ADDRESS(GIO1_R_GPCTR76_MEMADDR));
		writel(val | GIO1_F_HYSTEN_MASK, IO_ADDRESS(GIO1_R_GPCTR76_MEMADDR));

	}

}

/****************************************************************************
*
*   Machine Description
:*
*****************************************************************************/

MACHINE_START(GENEVA_B4, "GENEVA_B4")
	.phys_io 	= BCM47XX_ARM_PERIPH_BASE,
	.io_pg_offst 	= (IO_ADDRESS(BCM47XX_ARM_PERIPH_BASE) >> 18) & 0xfffc,
	.boot_params 	= (BCM47XX_ARM_DRAM + 0x100),
	.map_io 	= bcm476x_map_io,
	.init_irq 	= bcm476x_init_irq,
	.timer  	= &bcm476x_timer,
	.init_machine 	= geneva_b_init_machine,
MACHINE_END

MACHINE_START(GENEVA_B5, "GENEVA_B5")
	.phys_io 	= BCM47XX_ARM_PERIPH_BASE,
	.io_pg_offst 	= (IO_ADDRESS(BCM47XX_ARM_PERIPH_BASE) >> 18) & 0xfffc,
	.boot_params 	= (BCM47XX_ARM_DRAM + 0x100),
	.map_io 	= bcm476x_map_io,
	.init_irq 	= bcm476x_init_irq,
	.timer  	= &bcm476x_timer,
	.init_machine 	= geneva_b_init_machine,
MACHINE_END
