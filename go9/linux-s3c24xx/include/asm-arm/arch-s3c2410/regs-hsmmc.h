#ifndef REGS_HSMMC_H
#define REGS_HSMMC_H

#include <asm/arch/map.h>

/*
 * HS MMC Interface (chapter28)
 */
#define S3C_HSMMC_REG(x)    	((x) + S3C2443_VA_HSMMC)

#define HM_SYSAD                (0x00)
#define HM_BLKSIZE              (0x04)
#define HM_BLKCNT               (0x06)
#define HM_ARGUMENT             (0x08)
#define HM_TRNMOD               (0x0c)
#define HM_CMDREG               (0x0e)
#define HM_RSPREG0              (0x10)
#define HM_RSPREG1              (0x14)
#define HM_RSPREG2              (0x18)
#define HM_RSPREG3              (0x1c)
#define HM_BDATA                (0x20)
#define HM_PRNSTS               (0x24)
#define HM_HOSTCTL              (0x28)
#define HM_PWRCON               (0x29)
#define HM_BLKGAP               (0x2a)
#define HM_WAKCON               (0x2b)
#define HM_CLKCON               (0x2c)
#define HM_TIMEOUTCON           (0x2e)
#define HM_SWRST                (0x2f)
#define HM_NORINTSTS            (0x30)
#define HM_ERRINTSTS            (0x32)
#define HM_NORINTSTSEN          (0x34)
#define HM_ERRINTSTSEN          (0x36)
#define HM_NORINTSIGEN          (0x38)
#define HM_ERRINTSIGEN          (0x3a)
#define HM_ACMD12ERRSTS         (0x3c)
#define HM_CAPAREG              (0x40)
#define HM_MAXCURR              (0x48)
#define HM_CONTROL2             (0x80)
#define HM_CONTROL3             (0x84)
#define HM_HCVER                (0xfe)

#define S3C2450_HM_CONTROL2_CDINVRXD3_1		(0x1  << 30)
#define S3C2450_HM_CONTROL2_CDINVRXD3_0		(0x1  << 29)
#define S3C2450_HM_CONTROL2_SELCARDOUT		(0x1  << 28)
#define S3C2450_HM_CONTROL2_FLTCLKSEL		(0x1  << 24)
#define S3C2450_HM_CONTROL2_LVLDAT_MASK		(0xff << 16)
#define S3C2450_HM_CONTROL2_ENFBCLKTX		(0x1 << 15)
#define S3C2450_HM_CONTROL2_ENFBCLKRX		(0x1 << 14)
#define S3C2450_HM_CONTROL2_SDCDSEL		(0x1 << 13)
#define S3C2450_HM_CONTROL2_CDSYNSEL		(0x1 << 12)
#define S3C2450_HM_CONTROL2_ENBUSYCHKTXSTA	(0x1 << 11)
#define S3C2450_HM_CONTROL2_DFCNT_NODBC		(0x0 << 9)
#define S3C2450_HM_CONTROL2_DFCNT_4SDCLK	(0x1 << 9)
#define S3C2450_HM_CONTROL2_DFCNT_16SDCLK	(0x2 << 9)
#define S3C2450_HM_CONTROL2_DFCNT_64SDCLK	(0x3 << 9)
#define S3C2450_HM_CONTROL2_ENCLKOUTHOLD	(0x1 << 8)
#define S3C2450_HM_CONTROL2_RWAITMODE		(0x1 << 7)
#define S3C2450_HM_CONTROL2_DISBUFRD		(0x1 << 6)
#define S3C2450_HM_CONTROL2_SELBASECLK_HCLK	(0x1 << 4)
#define S3C2450_HM_CONTROL2_SELBASECLK_SCLK	(0x2 << 4)
#define S3C2450_HM_CONTROL2_SELBASECLK_EXT	(0x3 << 4)
#define S3C2450_HM_CONTROL2_PWRSYNC		(0x1 << 3)
#define S3C2450_HM_CONTROL2_ENCLKOUTMSKCON	(0x1 << 1)
#define S3C2450_HM_CONTROL2_HWINITFIN		(0x1 << 0)

#endif /* REGS_HSMMC */

