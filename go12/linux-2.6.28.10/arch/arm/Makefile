#
# arch/arm/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1995-2001 by Russell King

LDFLAGS_vmlinux	:=-p --no-undefined -X
ifeq ($(CONFIG_CPU_ENDIAN_BE8),y)
LDFLAGS_vmlinux	+= --be8
endif
CPPFLAGS_vmlinux.lds = -DTEXT_OFFSET=$(TEXT_OFFSET)
OBJCOPYFLAGS	:=-O binary -R .note -R .note.gnu.build-id -R .comment -S
GZFLAGS		:=-9
#KBUILD_CFLAGS	+=-pipe
# Explicitly specifiy 32-bit ARM ISA since toolchain default can be -mthumb:
KBUILD_CFLAGS	+=$(call cc-option,-marm,)

# Do not use arch/arm/defconfig - it's always outdated.
# Select a platform tht is kept up-to-date
KBUILD_DEFCONFIG := versatile_defconfig

# defines filename extension depending memory manement type.
ifeq ($(CONFIG_MMU),)
MMUEXT		:= -nommu
endif

ifeq ($(CONFIG_FRAME_POINTER),y)
KBUILD_CFLAGS	+=-fno-omit-frame-pointer -mapcs -mno-sched-prolog
endif
KBUILD_CFLAGS	+=-fno-delete-null-pointer-checks

ifeq ($(CONFIG_CPU_BIG_ENDIAN),y)
KBUILD_CPPFLAGS	+= -mbig-endian
AS		+= -EB
LD		+= -EB
else
KBUILD_CPPFLAGS	+= -mlittle-endian
AS		+= -EL
LD		+= -EL
endif

comma = ,

# This selects which instruction set is used.
# Note that GCC does not numerically define an architecture version
# macro, but instead defines a whole series of macros which makes
# testing for a specific architecture or later rather impossible.
arch-$(CONFIG_CPU_32v7)		:=-D__LINUX_ARM_ARCH__=7 $(call cc-option,-march=armv7-a,-march=armv5t -Wa$(comma)-march=armv7-a)
arch-$(CONFIG_CPU_32v6)		:=-D__LINUX_ARM_ARCH__=6 $(call cc-option,-march=armv6,-march=armv5t -Wa$(comma)-march=armv6)
# Only override the compiler option if ARMv6. The ARMv6K extensions are
# always available in ARMv7
ifeq ($(CONFIG_CPU_32v6),y)
arch-$(CONFIG_CPU_32v6K)	:=-D__LINUX_ARM_ARCH__=6 $(call cc-option,-march=armv6k,-march=armv5t -Wa$(comma)-march=armv6k)
endif
arch-$(CONFIG_CPU_32v5)		:=-D__LINUX_ARM_ARCH__=5 $(call cc-option,-march=armv5te,-march=armv4t)
arch-$(CONFIG_CPU_32v4T)	:=-D__LINUX_ARM_ARCH__=4 -march=armv4t
arch-$(CONFIG_CPU_32v4)		:=-D__LINUX_ARM_ARCH__=4 -march=armv4
arch-$(CONFIG_CPU_32v3)		:=-D__LINUX_ARM_ARCH__=3 -march=armv3

# This selects how we optimise for the processor.
tune-$(CONFIG_CPU_ARM610)	:=-mtune=arm610
tune-$(CONFIG_CPU_ARM710)	:=-mtune=arm710
tune-$(CONFIG_CPU_ARM7TDMI)	:=-mtune=arm7tdmi
tune-$(CONFIG_CPU_ARM720T)	:=-mtune=arm7tdmi
tune-$(CONFIG_CPU_ARM740T)	:=-mtune=arm7tdmi
tune-$(CONFIG_CPU_ARM9TDMI)	:=-mtune=arm9tdmi
tune-$(CONFIG_CPU_ARM940T)	:=-mtune=arm9tdmi
tune-$(CONFIG_CPU_ARM946E)	:=$(call cc-option,-mtune=arm9e,-mtune=arm9tdmi)
tune-$(CONFIG_CPU_ARM920T)	:=-mtune=arm9tdmi
tune-$(CONFIG_CPU_ARM922T)	:=-mtune=arm9tdmi
tune-$(CONFIG_CPU_ARM925T)	:=-mtune=arm9tdmi
tune-$(CONFIG_CPU_ARM926T)	:=-mtune=arm9tdmi
tune-$(CONFIG_CPU_SA110)	:=-mtune=strongarm110
tune-$(CONFIG_CPU_SA1100)	:=-mtune=strongarm1100
tune-$(CONFIG_CPU_XSCALE)	:=$(call cc-option,-mtune=xscale,-mtune=strongarm110) -Wa,-mcpu=xscale
tune-$(CONFIG_CPU_XSC3)		:=$(call cc-option,-mtune=xscale,-mtune=strongarm110) -Wa,-mcpu=xscale
tune-$(CONFIG_CPU_V6)		:=$(call cc-option,-mtune=arm1136j-s,-mtune=strongarm)

ifeq ($(CONFIG_AEABI),y)
CFLAGS_ABI	:=-mabi=aapcs-linux -mno-thumb-interwork
else
CFLAGS_ABI	:=$(call cc-option,-mapcs-32,-mabi=apcs-gnu) $(call cc-option,-mno-thumb-interwork,)
endif

ifeq ($(CONFIG_ARM_UNWIND),y)
CFLAGS_ABI	+=-funwind-tables
endif

ifeq ($(CONFIG_THUMB2_KERNEL),y)
AFLAGS_NOWARN	:=$(call as-option,-Wa$(comma)-mno-warn-deprecated,-Wa$(comma)-W)
CFLAGS_THUMB2	:=-mthumb $(AFLAGS_NOWARN)
AFLAGS_THUMB2	:=-Wa$(comma)-mthumb $(AFLAGS_NOWARN)
endif

# Need -Uarm for gcc < 3.x
KBUILD_CFLAGS	+=$(CFLAGS_ABI) $(CFLAGS_THUMB2) $(arch-y) $(tune-y) $(call cc-option,-mshort-load-bytes,$(call cc-option,-malignment-traps,)) -msoft-float -Uarm
KBUILD_AFLAGS	+=$(CFLAGS_ABI) $(AFLAGS_THUMB2) $(arch-y) $(tune-y) -include asm/unified.h -msoft-float

CHECKFLAGS	+= -D__arm__

#Default value
head-y		:= arch/arm/kernel/head$(MMUEXT).o arch/arm/kernel/init_task.o
textofs-y	:= 0x00008000

 machine-$(CONFIG_ARCH_RPC)	   := rpc
 machine-$(CONFIG_ARCH_EBSA110)	   := ebsa110
 machine-$(CONFIG_ARCH_CLPS7500)   := clps7500
 machine-$(CONFIG_FOOTBRIDGE)	   := footbridge
 machine-$(CONFIG_ARCH_SHARK)	   := shark
 machine-$(CONFIG_ARCH_SA1100)	   := sa1100
ifeq ($(CONFIG_ARCH_SA1100),y)
# SA1111 DMA bug: we don't want the kernel to live in precious DMA-able memory
 textofs-$(CONFIG_SA1111)	   := 0x00208000
endif
 machine-$(CONFIG_ARCH_PXA)	   := pxa
 machine-$(CONFIG_ARCH_L7200)	   := l7200
 machine-$(CONFIG_ARCH_INTEGRATOR) := integrator
 textofs-$(CONFIG_ARCH_CLPS711X)   := 0x00028000
 machine-$(CONFIG_ARCH_CLPS711X)   := clps711x
 machine-$(CONFIG_ARCH_IOP32X)	   := iop32x
 machine-$(CONFIG_ARCH_IOP33X)	   := iop33x
 machine-$(CONFIG_ARCH_IOP13XX)	   := iop13xx
    plat-$(CONFIG_PLAT_IOP)	   := iop
 machine-$(CONFIG_ARCH_IXP4XX)	   := ixp4xx
 machine-$(CONFIG_ARCH_IXP2000)    := ixp2000
 machine-$(CONFIG_ARCH_IXP23XX)    := ixp23xx
 machine-$(CONFIG_ARCH_OMAP1)	   := omap1
 machine-$(CONFIG_ARCH_OMAP2)	   := omap2
 machine-$(CONFIG_ARCH_OMAP3)	   := omap2
    plat-$(CONFIG_ARCH_OMAP)	   := omap
 machine-$(CONFIG_ARCH_S3C2410)	   := s3c2410 s3c2400 s3c2412 s3c2440 s3c2442 s3c2443
 machine-$(CONFIG_ARCH_S3C24A0)	   := s3c24a0
    plat-$(CONFIG_PLAT_S3C24XX)	   := s3c24xx s3c
 machine-$(CONFIG_ARCH_S3C64XX)	   := s3c6400 s3c6410
    plat-$(CONFIG_PLAT_S3C64XX)	   := s3c64xx s3c
 machine-$(CONFIG_ARCH_S5P64XX)	   := s5p6440
    plat-$(CONFIG_PLAT_S5P64XX)	   := s5p64xx s3c
 machine-$(CONFIG_ARCH_S5PC1XX)	   := s5pc100 s5pc110
    plat-$(CONFIG_PLAT_S5PC1XX)	   := s5pc1xx s3c
    plat-$(CONFIG_PLAT_TOMTOM)	   += tomtom
    plat-$(CONFIG_PLAT_MENDOZA)    += tomtom/mendoza
    plat-$(CONFIG_PLAT_IRVINE)     += tomtom/irvine
 machine-$(CONFIG_ARCH_LH7A40X)	   := lh7a40x
 machine-$(CONFIG_ARCH_VERSATILE)  := versatile
 machine-$(CONFIG_ARCH_IMX)	   := imx
 machine-$(CONFIG_ARCH_H720X)	   := h720x
 machine-$(CONFIG_ARCH_AAEC2000)   := aaec2000
 machine-$(CONFIG_ARCH_REALVIEW)   := realview
 machine-$(CONFIG_ARCH_AT91)	   := at91
 machine-$(CONFIG_ARCH_EP93XX)	   := ep93xx
 machine-$(CONFIG_ARCH_PNX4008)	   := pnx4008
 machine-$(CONFIG_ARCH_NETX)	   := netx
 machine-$(CONFIG_ARCH_NS9XXX)	   := ns9xxx
    plat-$(CONFIG_PLAT_BCM476X)    += bcm476x
 machine-$(CONFIG_ARCH_BCM4760)	   := bcm476x
  incdir-$(CONFIG_ARCH_BCM4760)	   := bcm4760
 machine-$(CONFIG_ARCH_DAVINCI)	   := davinci
 machine-$(CONFIG_ARCH_KIRKWOOD)   := kirkwood
 machine-$(CONFIG_ARCH_KS8695)     := ks8695
    plat-$(CONFIG_ARCH_MXC)	   := mxc
 machine-$(CONFIG_ARCH_MX2)	   := mx2
 machine-$(CONFIG_ARCH_MX3)	   := mx3
 machine-$(CONFIG_ARCH_ORION5X)	   := orion5x
    plat-$(CONFIG_PLAT_ORION)	   := orion
 machine-$(CONFIG_ARCH_MSM)	   := msm
 machine-$(CONFIG_ARCH_LOKI)       := loki
 machine-$(CONFIG_ARCH_MV78XX0)    := mv78xx0

ifeq ($(CONFIG_ARCH_EBSA110),y)
# This is what happens if you forget the IOCS16 line.
# PCMCIA cards stop working.
CFLAGS_3c589_cs.o :=-DISA_SIXTEEN_BIT_PERIPHERAL
export CFLAGS_3c589_cs.o
endif

# The byte offset of the kernel image in RAM from the start of RAM.
TEXT_OFFSET := $(textofs-y)

ifeq ($(CONFIG_XIP_UBOOT_HDR),y)
  # we're adding a u-boot header with mkimage so offset the address 0x40
  CONFIG_XIP_PHYS_ADDR := $(shell echo $(CONFIG_XIP_PHYS_ADDR) 0x40 | \
		awk --non-decimal-data '/[:xdigit:]/ \
		 { printf("0x%x\n", $$1 + $$2) }' )
endif

ifeq ($(incdir-y),)
incdir-y := $(machine-y)
endif
INCDIR   := arch-$(incdir-y)

# The first directory contains additional information for the boot setup code
ifneq ($(machine-y),)
MACHINE  := arch/arm/mach-$(word 1,$(machine-y))/
else
MACHINE  :=
endif

machdirs := $(patsubst %,arch/arm/mach-%/,$(machine-y))
platdirs := $(patsubst %,arch/arm/plat-%/,$(plat-y))

ifeq ($(KBUILD_SRC),)
KBUILD_CPPFLAGS += $(patsubst %,-I%include,$(machdirs) $(platdirs))
else
KBUILD_CPPFLAGS += $(patsubst %,-I$(srctree)/%include,$(machdirs) $(platdirs))
endif

export	TEXT_OFFSET GZFLAGS MMUEXT

# Do we have FASTFPE?
FASTFPE		:=arch/arm/fastfpe
ifeq ($(FASTFPE),$(wildcard $(FASTFPE)))
FASTFPE_OBJ	:=$(FASTFPE)/
endif

# If we have a machine-specific directory, then include it in the build.
core-y				+= arch/arm/kernel/ arch/arm/mm/ arch/arm/common/
core-y				+= $(machdirs) $(platdirs)
core-$(CONFIG_FPE_NWFPE)	+= arch/arm/nwfpe/
core-$(CONFIG_FPE_FASTFPE)	+= $(FASTFPE_OBJ)
core-$(CONFIG_VFP)		+= arch/arm/vfp/

drivers-$(CONFIG_OPROFILE)      += arch/arm/oprofile/

libs-y				:= arch/arm/lib/ $(libs-y)

# Default target when executing plain make
ifeq ($(CONFIG_XIP_KERNEL),y)
KBUILD_IMAGE := xipImage
else
KBUILD_IMAGE := zImage
endif

all:	$(KBUILD_IMAGE)

boot := arch/arm/boot

#	Update machine arch and proc symlinks if something which affects
#	them changed.  We use .arch to indicate when they were updated
#	last, otherwise make uses the target directory mtime.

ifeq ($(CONFIG_PLAT_BCM476X),y)
MAKETOOLSDEP := include/asm-arm/.arch
include/asm-arm/.arch: $(wildcard include/config/arch/*.h) include/config/auto.conf
	@echo '  SYMLINK include/asm-arm/arch -> include/asm-arm/$(INCDIR)'
ifneq ($(KBUILD_SRC),)
	$(Q)mkdir -p include/asm-arm
	$(Q)ln -fsn $(srctree)/include/asm-arm/$(INCDIR) include/asm-arm/arch
	$(Q)ln -fsn $(srctree)/include/asm-arm/$(INCDIR) include/mach
	$(Q)ln -fsn $(srctree)/include/asm-arm/$(INCDIR)/hardware.h include/asm-arm/hardware.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/sizes.h include/asm-arm/sizes.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/gpio.h include/asm-arm/gpio.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/page.h include/asm-arm/page.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/setup.h include/asm-arm/setup.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/memory.h include/asm-arm/memory.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/linkage.h include/asm-arm/linkage.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/page-nommu.h include/asm-arm/page-nommu.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/posix_types.h include/asm-arm/posix_types.h
	$(Q)ln -fsn $(srctree)/arch/arm/include/asm/errno.h include/asm-arm/errno.h
	$(Q)ln -fsn $(srctree)/include/linux/semaphore.h include/asm-arm/semaphore.h
else
	$(Q)ln -fsn $(INCDIR) include/asm-arm/arch
endif
	@touch $@
else
MAKETOOLSDEP	:=
endif

archprepare: maketools

PHONY += maketools FORCE
maketools: include/linux/version.h $(MAKETOOLSDEP) FORCE
	$(Q)$(MAKE) $(build)=arch/arm/tools include/asm-arm/mach-types.h

# Convert bzImage to zImage
bzImage: zImage

# These targets cannot be built in parallel
.NOTPARALLEL: zImage Image xipImage bootpImage uImage

zImage Image xipImage bootpImage uImage: vmlinux
	$(Q)$(MAKE) $(build)=$(boot) MACHINE=$(MACHINE) $(boot)/$@

zinstall install: vmlinux
	$(Q)$(MAKE) $(build)=$(boot) MACHINE=$(MACHINE) $@

CLEAN_FILES += include/asm-arm/mach-types.h \
	       include/asm-arm/arch include/asm-arm/.arch

# We use MRPROPER_FILES and CLEAN_FILES now
archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

# My testing targets (bypasses dependencies)
bp:;	$(Q)$(MAKE) $(build)=$(boot) MACHINE=$(MACHINE) $(boot)/bootpImage
i zi:;	$(Q)$(MAKE) $(build)=$(boot) MACHINE=$(MACHINE) $@


define archhelp
  echo  '* zImage        - Compressed kernel image (arch/$(ARCH)/boot/zImage)'
  echo  '  Image         - Uncompressed kernel image (arch/$(ARCH)/boot/Image)'
  echo  '* xipImage      - XIP kernel image, if configured (arch/$(ARCH)/boot/xipImage)'
  echo  '  uImage        - U-Boot wrapped zImage'
  echo  '  bootpImage    - Combined zImage and initial RAM disk' 
  echo  '                  (supply initrd image via make variable INITRD=<path>)'
  echo  '  install       - Install uncompressed kernel'
  echo  '  zinstall      - Install compressed kernel'
  echo  '                  Install using (your) ~/bin/installkernel or'
  echo  '                  (distribution) /sbin/installkernel or'
  echo  '                  install to $$(INSTALL_PATH) and run lilo'
endef
