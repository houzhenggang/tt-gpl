/*
 * s5m8751_audio.h  --  S5M8751 Power-Audio IC ALSA Soc Audio driver
 *
 * Copyright 2009 Samsung Electronics.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#ifndef __LINUX_S5M8751_AUDIO_H_
#define __LINUX_S5M8751_AUDIO_H_

#define S5M8751_CODEC				11

/*
 * Register values
 */
#define S5M8751_DA_PDB1				0x17
#define S5M8751_DA_AMIX1			0x18
#define S5M8751_DA_AMIX2			0x19
#define S5M8751_DA_ANA				0x1A
#define S5M8751_DA_DWA				0x1B
#define S5M8751_DA_VOLL				0x1C
#define S5M8751_DA_VOLR				0x1D
#define S5M8751_DA_DIG1				0x1E
#define S5M8751_DA_DIG2				0x1F
#define S5M8751_DA_LIM1				0x20
#define S5M8751_DA_LIM2				0x21
#define S5M8751_DA_LOF				0x22
#define S5M8751_DA_ROF				0x23
#define S5M8751_DA_MUX				0x24
#define S5M8751_DA_LGAIN			0x25
#define S5M8751_DA_RGAIN			0x26
#define S5M8751_IN1_CTRL1			0x27
#define S5M8751_IN1_CTRL2			0x28
#define S5M8751_IN1_CTRL3			0x29
#define S5M8751_SLOT_L2				0x2A
#define S5M8751_SLOT_L1				0x2B
#define S5M8751_SLOT_R2				0x2C
#define S5M8751_SLOT_R1				0x2D
#define S5M8751_TSLOT				0x2E

#define S5M8751_SPK_SLOPE			0x30
#define S5M8751_SPK_DT				0x31
#define S5M8751_SPK_S2D				0x32
#define S5M8751_SPK_CM				0x33
#define S5M8751_SPK_DUM				0x34
#define S5M8751_HP_VOL1				0x35
#define S5M8751_HP_VOL2				0x36
#define S5M8751_AMP_EN				0x37
#define S5M8751_AMP_MUTE			0x38
#define S5M8751_AMP_CTRL			0x39
#define S5M8751_AMP_VMID			0x3A
#define S5M8751_LINE_CTRL			0x3B

#define S5M8751_AUDIO_STATUS			0x45

/* 
 * R23 (0x17) - DA_PDB1
 */
#define S5M8751_VMID_MASK			(0x3 << 6)
#define S5M8751_VMID_5K				(0 << 6)
#define S5M8751_VMID_50K			(1 << 6)
#define S5M8751_VMID_250K			(1 << 7)
#define S5M8751_VMID_OFF			(0x3 << 6)
#define S5M8751_DA_PDB1_MASK			0xFF
#define S5M8751_PDB_REF				0x20
#define S5M8751_PDB_REF_SHIFT			5
#define S5M8751_PDB_DACL			0x10
#define S5M8751_PDB_DACL_SHIFT			4
#define S5M8751_PDB_DACR			0x08
#define S5M8751_PDB_DACR_SHIFT			3
#define S5M8751_PDB_MR				0x04
#define S5M8751_PDB_MR_SHIFT			2
#define S5M8751_PDB_MM				0x02
#define S5M8751_PDB_MM_SHIFT			1
#define S5M8751_PDB_ML				0x01
#define S5M8751_PDB_ML_SHIFT			0
#define S5M8751_ALL_DA_PDB1_OFF			0x00
#define S5M8751_DA_PDB1_SPK		(S5M8751_PDB_REF | S5M8751_PDB_DACL | \
					S5M8751_PDB_DACR | S5M8751_PDB_MM)

#define S5M8751_DA_PDB1_SPK		(S5M8751_PDB_REF | S5M8751_PDB_DACL | \
					S5M8751_PDB_DACR | S5M8751_PDB_MM)
#define S5M8751_ALL_DA_PDB1_OFF			0x00

/*
 * R24 (0x18) - DA_AMIX1
 */
#define S5M8751_DA_AMIX1_MASK			0x3F
#define S5M8751_RDAC_ML				0x20
#define S5M8751_RDAC_ML_SHIFT			5
#define S5M8751_RDAC_MM				0x10
#define S5M8751_RDAC_MM_SHIFT			4
#define S5M8751_RDAC_MR				0x08
#define S5M8751_RDAC_MR_SHIFT			3
#define S5M8751_LDAC_ML				0x04
#define S5M8751_LDAC_ML_SHIFT			2
#define S5M8751_LDAC_MM				0x02
#define S5M8751_LDAC_MM_SHIFT			1
#define S5M8751_LDAC_MR				0x01
#define S5M8751_LDAC_MR_SHIFT			0

#define S5M8751_DA_AMIX1_HP		(S5M8751_RDAC_MR | S5M8751_LDAC_ML)
#define S5M8751_DA_AMIX1_SPK		(S5M8751_RDAC_MM | S5M8751_LDAC_MM)

/*
 * R25 (0x19) - DA_AMIX2
 */
#define S5M8751_DA_AMIX2_MASK			0x3F
#define S5M8751_RBYP_ML				0x20
#define S5M8751_RBYP_ML_SHIFT			5
#define S5M8751_RBYP_MM				0x10
#define S5M8751_RBYP_MM_SHIFT			4
#define S5M8751_RBYP_MR				0x08
#define S5M8751_RBYP_MR_SHIFT			3
#define S5M8751_LBYP_ML				0x04
#define S5M8751_LBYP_ML_SHIFT			2
#define S5M8751_LBYP_MM				0x02
#define S5M8751_LBYP_MM_SHIFT			1
#define S5M8751_LBYP_MR				0x01
#define S5M8751_LBYP_MR_SHIFT			0

/*
 * R26 (0x1A) - DA_ANA
 */
#define S5M8751_DA_ANA_MASK			0xFF
#define S5M8751_CTIQ_MRL_MASK			0xC0
#define S5M8571_CTIQ_MRL_SHIFT			6
#define S5M8751_CTIQ_MM_MASK			0x30
#define S5M8571_CTIQ_MM_SHIFT			4
#define S5M8751_INT_BC_MASK			0x0F
#define S5M8751_INT_BC_SHIFT			0

/*
 * R27 (0x1B) - DA_DWA
 */
#define S5M8751_DA_DWA_MASK			0x07
#define S5M8751_DWAB_MASK			0x04
#define S5M8751_DWAB_SHIFT			2
#define S5M8751_DWAMODE_MASK			0x03
#define S5M8751_DWAMODE_SHIFT			0

/*
 * R28 (0x1C) - DA_VOLL
 */
#define S5M8751_DA_VOLL_MASK			0xFF
#define S5M8751_DA_VOLL_SHIFT			0
#define S5M8751_DAC_VOLL_DEFAULT		0x18
/*
 * R29 (0x1D) - DA_VOLR
 */
#define S5M8751_DA_VOLR_MASK			0xFF
#define S5M8751_DA_VOLR_SHIFT			0
#define S5M8751_DAC_VOLR_DEFAULT		0x18

/*
 * R30 (0x1E) - DA_DIG1
 */
#define S5M8751_DA_DIG1_MASK			0xFF
#define S5M8751_VBP_MASK			0xC0
#define S5M8751_VBP_SHIFT			6
#define S5M8751_DNS_MASK			0x20
#define S5M8751_DNS_SHIFT			5
#define S5M8751_AIC_MASK			0x10
#define S5M8751_AIC_SHIFT			4
#define S5M8751_DEM_MASK			0x08
#define S5M8751_DEM_SHIFT			3
#define S5M8751_FS1_FS0_MASK			0x06
#define S5M8751_FS1_FS0_SHIFT			1
#define S5M8751_MAN_DSR_MASK			0x01
#define S5M8751_MAN_DSR_SHIFT			0

/*
 * R31 (0x1F) - DA_DIG2
 */
#define S5M8751_DA_DIG2_MASK			0xFF
#define S5M8751_MUTE_DAC_MASK			0x80
#define S5M8751_MUTE_DAC_SHIFT			7
#define S5M8751_DAC_MUTE			(1 << 7)
#define S5M8751_DTHON_MASK			0x40
#define S5M8751_DTHON_SHIFT			6
#define S5M8751_DAMT_MASK			0x38
#define S5M8751_DAMT_SHIFT			3
#define S5M8751_SDTH_MASK			0x04
#define S5M8751_SDTH_SHIFT			2
#define S5M8751_TMOD_MASK			0x03
#define S5M8751_TMOD_SHIFT			0

/*
 * R32 (0x20) - DA_LIM1
 */
#define S5M8751_DA_LIM1_MASK			0xFF
#define S5M8751_SLIM_EN_MASK			0x08
#define S5M8751_SLIM_EN_SHIFT			7
#define S5M8751_THR_MASK			0x7F
#define S5M8751_THR_SHIFT			0

/*
 * R33 (0x21) - DA_LIM2
 */
#define S5M8751_DA_LIM2_MASK			0xFF
#define S5M8751_ATT_MASK			0xF0
#define S5M8751_ATT_SHIFT			4
#define S5M8751_REL_MASK			0x0F
#define S5M8751_REL_SHIFT			0

/*
 * R34 (0x22) - DA_LOF
 */
#define S5M8751_LOFFSET_MASK			0xFF
#define S5M8751_LOFFSET_SHIFT			0

/*
 * R35 (0x23) - DA_LOF
 */
#define S5M8751_ROFFSET_MASK			0xFF
#define S5M8751_ROFFSET_SHIFT			0

/*
 * R36 (0x24) - DA_MUX
 */
#define S5M8751_DA_MUX_MASK			0x3F
#define S5M8751_ROF_SGN_MASK			0x20
#define S5M8751_ROF_SGN_SHIFT			5
#define S5M8751_LOF_SGN_MASK			0x10
#define S5M8751_LOF_SGN_SHIFT			4
#define S5M8751_RIN_SEL_MASK			0x0C
#define S5M8751_RIN_SEL_SHIFT			2
#define S5M8751_LIN_SEL_MASK			0x03
#define S5M8751_LIN_SEL_SHIFT			0

/*
 * R37 (0x25) - DA_LGAIN
 */
#define S5M8751_DA_LGAIN_MASK			0xFF
#define S5M8751_PDB_LIN_MASK			0x80
#define S5M8751_PDB_LIN_SHIFT			7
#define S5M8751_LIN_GAIN_MASK			0x7F
#define S5M8751_LIN_GAIN_SHIFT			0

/*
 * R38 (0x26) - DA_RGAIN
 */
#define S5M8751_DA_RGAIN_MASK			0xFF
#define S5M8751_PDB_RIN_MASK			0x80
#define S5M8751_PDB_RIN_SHIFT			7
#define S5M8751_RIN_GAIN_MASK			0x7F
#define S5M8751_RIN_GAIN_SHIFT			0

/*
 * R39 (0x27) - IN1_CTRL1
 */
#define S5M8751_LOGIC_PDN			(1 << 7)
#define S5M8751_IN1_CTRL1_MASK			0xFF
#define S5M8751_PDN_MASK			0x80
#define S5M8751_PDN_SHIFT			7
#define S5M8751_RESET_MASK			0x40
#define S5M8751_RESET_SHIFT			6
#define S5M8751_SAMP_RATE1_MASK			0x3C
#define S5M8751_SAMP_RATE1_SHIFT		2
#define S5M8751_SAMP_RATE_8K			(0x0 << 2)
#define S5M8751_SAMP_RATE_11_025K		(0x1 << 2)
#define S5M8751_SAMP_RATE_16K			(0x2 << 2)
#define S5M8751_SAMP_RATE_22_05K		(0x3 << 2)
#define S5M8751_SAMP_RATE_32K			(0x4 << 2)
#define S5M8751_SAMP_RATE_44_1K			(0x5 << 2)
#define S5M8751_SAMP_RATE_48K			(0x6 << 2)
#define S5M8751_SAMP_RATE_64K			(0x7 << 2)
#define S5M8751_SAMP_RATE_88_2K			(0x8 << 2)
#define S5M8751_SAMP_RATE_96K			(0x9 << 2)
#define S5M8751_SCK_POL1_MASK			0x02
#define S5M8751_SCK_POL1_SHIFT			1
#define S5M8751_SCK_POL				(1 << 1)
#define S5M8751_I2S_PCM1_MASK			0x01
#define S5M8751_I2S_PCM1_SHIFT			0
#define S5M8751_I2S_PCM				(1 << 0)

/*
 * R40 (0x28) - IN1_CTRL2
 */
#define S5M8751_IN1_CTRL2_MASK			0xFF
#define S5M8751_CROSS_MASK			0x80
#define S5M8751_CROSS_SHIFT			7
#define S5M8751_I2S_XFS1_MASK			0x60
#define S5M8751_I2S_XFS1_SHIFT			5
#define S5M8751_I2S_SCK_32FS			(0 << 5)
#define S5M8751_I2S_SCK_48FS			(1 << 5)
#define S5M8751_I2S_SCK_64FS			(1 << 6)
#define S5M8751_LRCK_POL1_MASK			0x10
#define S5M8751_LRCK_POL1_SHIFT			4
#define S5M8751_LRCK_POL			(1 << 4)

#define S5M8751_I2S_DF1_MASK			0x0C
#define S5M8751_I2S_DF1_SHIFT			2
#define S5M8751_I2S_STANDARD			(0 << 2)
#define S5M8751_I2S_LJ				(1 << 2)
#define S5M8751_I2S_RJ				(1 << 3)
#define S5M8751_I2S_DL1_MASK			0x03
#define S5M8751_I2S_DL1_SHIFT			0
#define S5M8751_I2S_DL_16BIT			(0 << 0)
#define S5M8751_I2S_DL_18BIT			(1 << 0)
#define S5M8751_I2S_DL_20BIT			(1 << 1)
#define S5M8751_I2S_DL_24BIT			(3 << 0)

/*
 * R41 (0x29) - IN1_CTRL3
 */
#define S5M8751_IN1_CTRL3_MASK			0x0F
#define S5M8751_PCM_DF1_MASK			0x08
#define S5M8751_PCM_DF1_SHIFT			3
#define S5M8751_PCM_DL1_MASK			0x04
#define S5M8751_PCM_DL1_SHIFT			2
#define S5M8751_PCM_LAW1_MASK			0x02
#define S5M8751_PCM_LAW1_SHIFT			1
#define S5M8751_PCM_COMP1_MASK			0x01
#define S5M8751_PCM_COMP1_SHIFT			0

/*
 * R42 (0x2A) - SLOT_L2
 */
#define S5M8751_SLOT_L2_MASK			0xFF
#define S5M8751_LSLOT_15			0x80
#define S5M8751_LSLOT_14			0x40
#define S5M8751_LSLOT_13			0x20
#define S5M8751_LSLOT_12			0x10
#define S5M8751_LSLOT_11			0x08
#define S5M8751_LSLOT_10			0x04
#define S5M8751_LSLOT_09			0x02
#define S5M8751_LSLOT_08			0x01

/*
 * R43 (0x2B) - SLOT_L1
 */
#define S5M8751_SLOT_L1_MASK			0xFF
#define S5M8751_LSLOT_07			0x80
#define S5M8751_LSLOT_06			0x40
#define S5M8751_LSLOT_05			0x20
#define S5M8751_LSLOT_04			0x10
#define S5M8751_LSLOT_03			0x08
#define S5M8751_LSLOT_02			0x04
#define S5M8751_LSLOT_01			0x02
#define S5M8751_LSLOT_00			0x01

/*
 * R44 (0x2C) - SLOT_R2
 */
#define S5M8751_SLOT_R2_MASK			0xFF
#define S5M8751_RSLOT_15			0x80
#define S5M8751_RSLOT_14			0x40
#define S5M8751_RSLOT_13			0x20
#define S5M8751_RSLOT_12			0x10
#define S5M8751_RSLOT_11			0x08
#define S5M8751_RSLOT_10			0x04
#define S5M8751_RSLOT_09			0x02
#define S5M8751_RSLOT_08			0x01

/*
 * R45 (0x2D) - SLOT_R1
 */
#define S5M8751_SLOT_R1_MASK			0xFF
#define S5M8751_RSLOT_07			0x80
#define S5M8751_RSLOT_06			0x40
#define S5M8751_RSLOT_05			0x20
#define S5M8751_RSLOT_04			0x10
#define S5M8751_RSLOT_03			0x08
#define S5M8751_RSLOT_02			0x04
#define S5M8751_RSLOT_01			0x02
#define S5M8751_RSLOT_00			0x01

/*
 * R46 (0x2E) - TSLOT
 */
#define S5M8751_TSLOT_MASK			0x0F
#define S5M8751_TSLOT_SHIFT			0

/*
 * R48 (0x30) - SPK_SLOPE
 */
#define S5M8751_SPK_SLOPE_MASK			0x0F
#define S5M8751_SPK_SLOPE_SHIFT			0

enum {
	S5M8751_SPK_SLOPE_0 = 0,
	S5M8751_SPK_SLOPE_1,
	S5M8751_SPK_SLOPE_2,
	S5M8751_SPK_SLOPE_3,
	S5M8751_SPK_SLOPE_4,
	S5M8751_SPK_SLOPE_5,
	S5M8751_SPK_SLOPE_6,
	S5M8751_SPK_SLOPE_7,
	S5M8751_SPK_SLOPE_8,
	S5M8751_SPK_SLOPE_9,
	S5M8751_SPK_SLOPE_10,
	S5M8751_SPK_SLOPE_11,
	S5M8751_SPK_SLOPE_12,
	S5M8751_SPK_SLOPE_13,
	S5M8751_SPK_SLOPE_14,
	S5M8751_SPK_SLOPE_15,
};

/*
 * R49 (0x31) - SPK_DT
 */
#define S5M8751_SPK_DT_MASK			0x1F
#define S5M8751_SPK_DT_SHIFT			0
#define S5M8751_SPK_DT_1ns			(0x1 << 0)
#define S5M8751_SPK_DT_2ns			(0x1 << 1)
#define S5M8751_SPK_DT_3ns			(0x1 << 2)
#define S5M8751_SPK_DT_4ns			(0x3 << 1)
#define S5M8751_SPK_DT_5ns			(0x1 << 3)
#define S5M8751_SPK_DT_7ns			(0x1 << 4)

/*
 * R50 (0x32) - SPK_S2D
 */
#define S5M8751_SPK_S2D_MASK			0x33
#define S5M8751_SPK_PAMP_GAIN_MASK		0x30
#define S5M8751_SPK_PAMP_GAIN_0DB		0x00
#define S5M8751_SPK_PAMP_GAIN_6DB		0x01
#define S5M8751_SPK_PAMP_GAIN_12DB		0x02
#define S5M8751_SPK_PAMP_GAIN_SHIFT		4
#define S5M8751_SPK_PTR_OFF_MASK		0x03
#define S5M8751_SPK_PTR_OFF_SHIFT		0

/*
 * R51 (0x33) - SPK_CM
 */
#define S5M8751_SPK_CM_MASK			0x7F
#define S5M8751_SPK_CM_SHIFT			0

/*
 * R52 (0x34) - SPK_DUM(reserved)
 */

/*
 * R53 (0x35) - HP_VOL1
 */
#define S5M8751_HP_VOL1_MASK			0x7F
#define S5M8751_HP_VOL1_SHIFT			0
#define S5M8751_HP_LHPVOL_DEFAULT		0x14

/*
 * R54 (0x36) - HP_VOL2
 */
#define S5M8751_HP_VOL2_MASK			0x7F
#define S5M8751_HP_VOL2_SHIFT			0
#define S5M8751_HP_RHPVOL_DEFAULT		0x14

/*
 * R55 (0x37) - AMP_EN
*/
#define S5M8751_ALL_AMP_OFF			0x00
#define S5M8751_AMP_EN_MASK			0xFF
#define S5M8751_SPK_S2D_ENA			0x80
#define S5M8751_SPK_ENA				0x40
#define S5M8751_RLIN_INV_ENA			0x20
#define S5M8751_VMID_BUFF_ENA			0x10
#define S5M8751_LHP_ENA				0x08
#define S5M8571_RHP_ENA				0x04
#define S5M8751_LLINE_ENA			0x02
#define S5M8751_RLINE_ENA			0x01
#define S5M8751_AMP_EN_SPK_S2D_MASK		0x80
#define S5M8751_AMP_EN_SPK_S2D_SHIFT		7
#define S5M8751_AMP_EN_SPK_MASK			0x40
#define S5M8751_AMP_EN_SPK_SHIFT		6
#define S5M8751_AMP_EN_LHP_MASK			0x08
#define S5M8751_AMP_EN_LHP_SHIFT		3
#define S5M8751_AMP_EN_RHP_MASK			0x04
#define S5M8751_AMP_EN_RHP_SHIFT		2
#define S5M8751_AMP_EN_LLINE_MASK		0x02
#define S5M8751_AMP_EN_LLINE_SHIFT		1
#define S5M8751_AMP_EN_RLINE_MASK		0x01
#define S5M8751_AMP_EN_RLINE_SHIFT		0

#define S5M8751_HP_AMP_EN		(S5M8751_SPK_S2D_ENA \
					| S5M8751_LHP_ENA | \
					S5M8751_RHP_ENA)
#define S5M8751_SPK_AMP_EN		(S5M8751_SPK_S2D_ENA | S5M8751_SPK_ENA)
#define S5M8751_ALL_AMP_OFF			0x00


/*
 * R56 (0x38) - AMP_MUTE
 */
#define S5M8751_ALL_AMP_MUTE			0x00
#define S5M8751_AMP_MUTE_MASK			0x7F
#define S5M8751_SPK_S2D_MUTEB			0x10
#define S5M8751_LHP_MUTEB			0x08
#define S5M8751_RHP_MUTEB			0x04
#define S5M8751_LLINE_MUTEB			0x02
#define S5M8751_RLINE_MUTEB			0x01
#define S5M8751_AMP_MUTE_SPK_S2D_MASK		0x10
#define S5M8751_AMP_MUTE_SPK_S2D_SHIFT		4
#define S5M8751_AMP_MUTE_LHP_MASK		0x08
#define S5M8751_AMP_MUTE_LHP_SHIFT		3
#define S5M8751_AMP_MUTE_RHP_MASK		0x04
#define S5M8751_AMP_MUTE_RHP_SHIFT		2
#define S5M8751_AMP_MUTE_LLINE_MASK		0x02
#define S5M8751_AMP_MUTE_LLINE_SHIFT		1
#define S5M8751_AMP_MUTE_RLINE_MASK		0x01
#define S5M8751_AMP_MUTE_RLINE_SHIFT		0
#define S5M8751_SPK_S2D_12dB			(0x2 << 4)
#define S5M8751_SPK_S2D_6dB			(0x1 << 4)
#define S5M8751_SPK_S2D_0dB			(0x0 << 4)
#define S5M8751_SPK_PTR_OFF			(1 << 0)

/*
 * R57 (0x39) - AMP_CTRL
 */
#define S5M8751_AMP_CTRL_MASK			0xFF
#define S5M8751_HP_AMP_MUTE_CONTROL_ON		(1 << 7)
#define S5M8751_MCTRL_MASK			0x80
#define S5M8751_MCTRL_SHIFT			7
#define S5M8751_PROT_ENB_MASK			0x40
#define S5M8751_PROT_ENB_SHIFT			6
#define S5M8751_CTRL_CHRG_MASK			0x30
#define S5M8751_CTRL_CHRG_SHIFT			4
#define S5M8751_CTRL_BIAS_MASK			0x0F
#define S5M8751_CTRL_BIAS_SHIFT			0

/*
 * R58 (0x3A) - AMP_VMID
 */
#define S5M8751_AMP_VMID_MASK			0x3F
#define S5M8751_HPL_DCOC_MASK			0x38
#define S5M8751_HPL_DCOC_SHIFT			3
#define S5M8751_HPR_DCOC_MASK			0x07
#define S5M8751_HPR_DCOC_SHIFT			0

/*
 * R59 (0x3B) - LINE_CTRL
 */
#define S5M8751_LINE_CTRL_MASK			0x6F
#define S5M8751_HP_BIAS_CTRL_MASK		0x60
#define S5M8751_HP_BIAS_CTRL_SHIFT		5
#define S5M8751_CTRL_BIAS_MASK			0x0F
#define S5M8751_CTRL_BIAS_SHIFT			0

#define S5M8751_SYSCLK 0
#define S5M8751_MCLK   1
#define S5M8751_BCLK   2

#define MUTE_OFF  0
#define MUTE_ON	  1

/*
 * S5M8751 audio platform data
 */
struct s5m8751_audio_platform_data {
	unsigned char dac_voll;
	unsigned char dac_volr;

	unsigned char lhpvol;
	unsigned char rhpvol;

	unsigned char spk_s2d;
	unsigned char spk_slope;
	unsigned char spk_dt;
};

extern struct snd_soc_dai s5m8751_dai;
extern struct snd_soc_codec_device soc_codec_dev_s5m8751;
extern struct snd_soc_dai *cpu_dai;
extern struct s3c_i2sv2_rate_calc div;

struct snd_soc_codec;

struct s5m8751_codec {
	struct platform_device *pdev;
	struct snd_soc_codec *codec;
	struct s5m8751_audio_platform_data *platform_data;
};

#endif
