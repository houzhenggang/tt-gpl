/**
 * kmemcheck - a heavyweight memory checker for the linux kernel
 * Copyright (C) 2008  Janboe Ye <janboe.ye@gmail.com>
 * Base on x86 memcheck.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (version 2) as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/kdebug.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/page-flags.h>
#include <linux/percpu.h>
#include <linux/stacktrace.h>
#include <linux/timer.h>
#include <linux/kmemcheck.h>
#include <linux/string.h>

#include <asm/cacheflush.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>

enum kmemcheck_method {
	KMEMCHECK_READ,
	KMEMCHECK_WRITE,
};

enum shadow {
	SHADOW_UNALLOCATED,
	SHADOW_UNINITIALIZED,
	SHADOW_INITIALIZED,
	SHADOW_FREED,
};

enum kmemcheck_error_type {
	ERROR_INVALID_ACCESS,
	ERROR_BUG,
};

#define SHADOW_COPY_SIZE (1 << CONFIG_KMEMCHECK_SHADOW_COPY_SHIFT)

struct kmemcheck_error {
	enum kmemcheck_error_type type;

	union {
		/* ERROR_INVALID_ACCESS */
		struct {
			/* Kind of access that caused the error */
			enum shadow	state;
			/* Address and size of the erroneous read */
			unsigned long	address;
			unsigned int	size;
		};
	};

	struct pt_regs		regs;
	struct stack_trace	trace;
	unsigned long		trace_entries[32];

	/* We compress it to a char. */
	unsigned char		shadow_copy[SHADOW_COPY_SIZE];
};

/*
 * Create a ring queue of errors to output. We can't call printk() directly
 * from the kmemcheck traps, since this may call the console drivers and
 * result in a recursive fault.
 */
static struct kmemcheck_error error_fifo[CONFIG_KMEMCHECK_QUEUE_SIZE];
static unsigned int error_count;
static unsigned int error_rd;
static unsigned int error_wr;
static unsigned int error_missed_count;

static struct timer_list kmemcheck_timer;
#define KMEMCHECK_RETURN_INSTRUCTION	0xe1a0f00e	/* mov pc, lr */
#define SET_R0_TRUE_INSTRUCTION		0xe3a00001	/* mov	r0, #1 */
struct asi {
	unsigned long insn[2];
};

union reg_pair {
	long long	dr;
#ifdef __LITTLE_ENDIAN
	struct { long	r0, r1; };
#else
	struct { long	r1, r0; };
#endif
};
#define	truecc_insn(insn)	(((insn) & 0xf0000000) | \
				 (SET_R0_TRUE_INSTRUCTION & 0x0fffffff))
typedef long (insn_1arg_fn_t)(long);
typedef long (insn_3arg_fn_t)(long, long, long);
typedef long long (insn_llret_3arg_fn_t)(long, long, long);
DEFINE_PER_CPU(struct asi, kmemcheck_insn);
DEFINE_PER_CPU(pte_t *, kmemcheck_pte);
DEFINE_PER_CPU(unsigned long, kmemcheck_address);

static long __init find_str_pc_offset(void)
{
	int addr, scratch, ret;

	__asm__ (
		"sub	%[ret], pc, #4		\n\t"
		"str	pc, %[addr]		\n\t"
		"ldr	%[scr], %[addr]		\n\t"
		"sub	%[ret], %[scr], %[ret]	\n\t"
		: [ret] "=r" (ret), [scr] "=r" (scratch), [addr] "+m" (addr));

	return ret;
}

static inline unsigned long decode_address(unsigned long insn,
		unsigned long rnv, unsigned long rmv)
{
	int ibit = 25;
	int ibitv = insn & (1 << ibit);
	int ubit = insn & (1 << 23);
	int pbit = insn & (1 << 24);
	unsigned long addr = rnv;
	if (pbit) {
		if (ibitv) {
			if (insn && 0xff0) {
				int shift = (insn & 0x60) >> 5;
				int shift_imm = (insn & 0xf80) >> 7;
				int index = 0;
				if (shift == 0 /*LSL*/) {
					index = rmv << shift_imm;
				} else if (shift == 0x1 /* LSR */) {
					index = rmv >> shift_imm;
				}
				if (ubit) {
					addr += index;
				} else {
					addr -= index;
				}
			} else {
				if (ubit) {
					addr += rmv;
				} else {
					addr -= rmv;
				}
			}
		} else {
			unsigned long offset = insn & 0xfff;
			if (ubit) {
				addr += offset;
			} else {
				addr -= offset;
			}
		}
	}
	return addr;
}

static inline unsigned long decode_address_misc(unsigned long insn,
		unsigned long rnv, unsigned long rmv)
{
	int ibit = insn & (1 << 22);
	int ubit = insn & (1 << 23);
	int pbit = insn & (1 << 24);
	unsigned long addr = rnv;
	if (pbit) {
		if (!ibit) {
				if (ubit)
					addr += rmv;
				else
					addr -= rmv;
		} else {
			unsigned long offset = insn & 0xf;
			offset += (insn & 0xf00) >> 4;
			if (ubit)
				addr += offset;
			else
				addr -= offset;
		}
	}
	return addr;
}

static inline long
insnslot_1arg_rflags(long r0, long cpsr, insn_1arg_fn_t *fn)
{
	register long rr0 asm("r0") = r0;
	register long ret asm("r0");

	__asm__ __volatile__ (
			"msr	cpsr_fs, %[cpsr]	\n\t"
			"mov	lr, pc			\n\t"
			"mov	pc, %[fn]		\n\t"
			: "=r" (ret)
			: "0" (rr0), [cpsr] "r" (cpsr), [fn] "r" (fn)
			: "lr", "cc"
			);
	return ret;
}
static inline long long
insnslot_llret_3arg_rflags(long r0, long r1, long r2, long cpsr,
			   insn_llret_3arg_fn_t *fn)
{
	register long rr0 asm("r0") = r0;
	register long rr1 asm("r1") = r1;
	register long rr2 asm("r2") = r2;
	register long ret0 asm("r0");
	register long ret1 asm("r1");
	union reg_pair fnr;

	__asm__ __volatile__ (
		"msr	cpsr_fs, %[cpsr]	\n\t"
		"mov	lr, pc			\n\t"
		"mov	pc, %[fn]		\n\t"
		: "=r" (ret0), "=r" (ret1)
		: "0" (rr0), "r" (rr1), "r" (rr2),
		  [cpsr] "r" (cpsr), [fn] "r" (fn)
		: "lr", "cc"
	);
	fnr.r0 = ret0;
	fnr.r1 = ret1;
	return fnr.dr;
}
static inline long
insnslot_3arg_rflags(long r0, long r1,
		long r2, long cpsr, insn_3arg_fn_t *fn)
{
	register long rr0 asm("r0") = r0;
	register long rr1 asm("r1") = r1;
	register long rr2 asm("r2") = r2;
	register long ret asm("r0");

	__asm__ __volatile__ (
		"msr	cpsr_fs, %[cpsr]	\n\t"
		"mov	lr, pc			\n\t"
		"mov	pc, %[fn]		\n\t"
		: "=r" (ret)
		: "0" (rr0), "r" (rr1), "r" (rr2),
		  [cpsr] "r" (cpsr), [fn] "r" (fn)
		: "lr", "cc"
	);
	return ret;
}

static void emulate_str(unsigned long insn, struct pt_regs *regs)
{
	insn_3arg_fn_t *i_fn = (insn_3arg_fn_t *)&(__get_cpu_var(kmemcheck_insn).insn[0]);
	long iaddr = regs->ARM_pc;
	int rd = (insn >> 12) & 0xf;
	int rn = (insn >> 16) & 0xf;
	int rm = insn & 0xf;
	unsigned long rdv = (rd == 15) ? iaddr + find_str_pc_offset(): regs->uregs[rd];
	unsigned long rnv = (rn == 15) ? iaddr +  8 : regs->uregs[rn];
	unsigned long rmv = regs->uregs[rm];  /* rm/rmv may be invalid, don't care. */
#ifdef DEBUG
	unsigned int value;
	unsigned long src_value = rdv;
	int bbit = insn & (1 << 22);
	unsigned long addr = (insn & 0x0e000000)?decode_address(insn, rnv, rmv):decode_address_misc(insn, rnv, rmv);
#endif

	/* Save Rn in case of writeback. */
	regs->uregs[rn] =
		insnslot_3arg_rflags(rnv, rdv, rmv, regs->ARM_cpsr, i_fn);
#ifdef DEBUG
	value = *((unsigned long *) addr);
	if (insn & 0x0e000000) {
		if (bbit) {
			src_value = rdv & 0xff;
			value = value & 0xff;
		}
	} else {
		value &= 0xffff;
		src_value = rdv & 0xffff;
	}
	if (value != src_value) {
		printk(KERN_ERR "Warning: 0x%lx value %lx:%lx is err\n", insn, rdv, value);
		printk(KERN_ERR "rd %d:%lx, rn %d:%lx, rm %d:%lx, addr: %lx, insn: %lx\n", rd, rdv, rn, rnv,
			rm, rmv, addr, __get_cpu_var(kmemcheck_insn).insn[0]);
	}
#endif
	regs->ARM_pc += 4;
}

static void emulate_ldr(unsigned long insn, struct pt_regs *regs)
{
	insn_llret_3arg_fn_t *i_fn = (insn_llret_3arg_fn_t *)&(__get_cpu_var(kmemcheck_insn).insn[0]);
	union reg_pair fnr;
	int rd = (insn >> 12) & 0xf;
	int rn = (insn >> 16) & 0xf;
	int rm = insn & 0xf;
	long rdv = regs->uregs[rd];
	unsigned long rnv  = regs->uregs[rn];
	unsigned long rmv  = regs->uregs[rm]; /* rm/rmv may be invalid, don't care. */
	unsigned long cpsr = regs->ARM_cpsr;
#ifdef DEBUG
	unsigned int value;
	int bbit = insn & (1 << 22);
	unsigned long src_value;
	unsigned long addr = (insn & 0x0e000000)?decode_address(insn, rnv, rmv):decode_address_misc(insn, rnv, rmv);
	value = *((unsigned long *) addr);
#endif
	fnr.dr = insnslot_llret_3arg_rflags(rnv, rdv, rmv, cpsr, i_fn);
	regs->uregs[rn] = fnr.r0;  /* Save Rn in case of writeback. */
	rdv = fnr.r1;
#ifdef DEBUG
	if (insn & 0x0e000000) {
		src_value = rdv;
		if (bbit) {
			src_value = rdv & 0xff;
			value = value & 0xff;
		}
	} else {
		value &= 0xffff;
		src_value = rdv & 0xffff;
	}
	if (value != src_value) {
		printk(KERN_ERR "Warning: 0x%lx value %lx:%lx is err\n", insn, rdv, value);
		printk(KERN_ERR "rd %d:%lx, rn %d:%lx, rm %d:%lx, addr: %lx, insn: %lx\n", rd, rdv, rn, rnv,
			rm, rmv, addr, __get_cpu_var(kmemcheck_insn).insn[0]);
	}
#endif
	if (rd == 15) {
#if __LINUX_ARM_ARCH__ >= 5
		cpsr &= ~PSR_T_BIT;
		if (rdv & 0x1)
			cpsr |= PSR_T_BIT;
		regs->ARM_cpsr = cpsr;
		rdv &= ~0x1;
#else
		rdv &= ~0x2;
#endif
	} else {
		regs->ARM_pc += 4;
	}
	regs->uregs[rd] = rdv;
}

static void
emulate_ldr_str(unsigned long orig_insn, struct pt_regs *regs)
{
	unsigned long insn = orig_insn;

	insn &= 0xfff00fff;
	insn |= 0x00001000;	/* Rn = r0, Rd = r1 */
	if (insn & (1 << 25)) {
		insn &= ~0xf;
		insn |= 2;	/* Rm = r2 */
	}
	__get_cpu_var(kmemcheck_insn).insn[0] = insn;
	__cpuc_coherent_kern_range(__get_cpu_var(kmemcheck_insn).insn, __get_cpu_var(kmemcheck_insn).insn+PAGE_SIZE);
	if (insn & (1 << 20))
		emulate_ldr(orig_insn, regs);
	else
		emulate_str(orig_insn, regs);
	return;
}

static void
emulate_ldrh_strh(unsigned long orig_insn, struct pt_regs *regs)
{
	unsigned long insn = orig_insn;

	insn &= 0xfff00fff;
	insn |= 0x00001000;	/* Rn = r0, Rd = r1 */
	if (!(insn & (1 << 22))) {
		insn &= ~0xf;
		insn |= 2;	/* Rm = r2 */
	}
	__get_cpu_var(kmemcheck_insn).insn[0] = insn;
	__cpuc_coherent_kern_range(__get_cpu_var(kmemcheck_insn).insn, __get_cpu_var(kmemcheck_insn).insn+PAGE_SIZE);
	if (insn & (1 << 20))
		emulate_ldr(orig_insn, regs);
	else
		emulate_str(orig_insn, regs);
	return;
}

static void simulate_ldm1stm1(unsigned int insn, struct pt_regs *regs)
{
	int rn = (insn >> 16) & 0xf;
	int lbit = insn & (1 << 20);
	int wbit = insn & (1 << 21);
	int ubit = insn & (1 << 23);
	int pbit = insn & (1 << 24);
	int mode = (insn >> 23) & 0x3;
	unsigned long *addr = (unsigned long *)regs->uregs[rn];
	unsigned long *end;
	int reg_bit_vector;
	int reg_count;

	reg_count = 0;
	reg_bit_vector = insn & 0xffff;
	while (reg_bit_vector) {
		reg_bit_vector &= (reg_bit_vector - 1);
		++reg_count;
	}
	/*
	if (!ubit)
		addr -= reg_count;
	addr += (((!pbit) ^ (!ubit))?0:1);
	*/
	switch (mode) {
	case 1:
		end = addr + reg_count;
		break;
	case 3:
		end = addr + reg_count;
		addr += 1;
		break;
	case 0:
		end = addr - reg_count;
		addr = end + 1;
		break;
	case 2:
		end = addr - reg_count;
		addr = end;
		break;
	}
	reg_bit_vector = insn & 0xffff;
	while (reg_bit_vector) {
		int reg = __ffs(reg_bit_vector);
		reg_bit_vector &= (reg_bit_vector - 1);
		if (lbit)
			regs->uregs[reg] = *addr++;
		else
			*addr++ = regs->uregs[reg];
	}

	if (wbit) {
/*
		if (!ubit)
			addr -= reg_count;
		addr -= (((!pbit) ^ (!ubit))?0:1);
*/
		regs->uregs[rn] = (long)end;
	}
}

static void emulate_stm_ldm(unsigned int insn, struct pt_regs *regs)
{
	insn_1arg_fn_t *i_fn = (insn_1arg_fn_t *)&(__get_cpu_var(kmemcheck_insn).insn[0]);
	__get_cpu_var(kmemcheck_insn).insn[0] = truecc_insn(insn);
	mb();
	__cpuc_coherent_kern_range(__get_cpu_var(kmemcheck_insn).insn, __get_cpu_var(kmemcheck_insn).insn+PAGE_SIZE);

	if (!insnslot_1arg_rflags(0, regs->ARM_cpsr, i_fn)) {
		regs->ARM_pc += 4;
		return;
	}

	if ((insn & 0x108000) == 0x008000) {
		unsigned int addr = regs->ARM_pc;
		regs->ARM_pc += find_str_pc_offset();
		simulate_ldm1stm1(insn, regs);
		regs->ARM_pc = addr;
	} else {
		simulate_ldm1stm1(insn, regs);
		if ((insn & 0x108000) != 0x108000)
			regs->ARM_pc += 4;
	}
}

static void singlestep(struct pt_regs *regs, pte_t *pte, unsigned long address)
{
	__get_cpu_var(kmemcheck_insn).insn[1] = KMEMCHECK_RETURN_INSTRUCTION;
	unsigned int insn = *(unsigned long *)(regs->ARM_pc);
#if 1
	if ((insn & 0x0c000000) == 0x04000000) {
		set_pte_ext(pte, __pte(pte_val(*pte) | L_PTE_PRESENT), 0);
		flush_tlb_kernel_page(address);
		emulate_ldr_str(insn, regs);
		__cpuc_coherent_kern_range(address, address+PAGE_SIZE);
		set_pte_ext(pte, __pte(((pte_val(*pte) & ~L_PTE_PRESENT)|L_PTE_HIDDEN)), 0);
		flush_tlb_kernel_page(address);
		return;
	}
#endif
#if 1
	if ((insn & 0x0e000000) == 0x08000000) {
		set_pte_ext(pte, __pte(pte_val(*pte) | L_PTE_PRESENT), 0);
		flush_tlb_kernel_page(address);
		emulate_stm_ldm(insn, regs);
		__cpuc_coherent_kern_range(address, address+PAGE_SIZE);
		set_pte_ext(pte, __pte(((pte_val(*pte) & ~L_PTE_PRESENT)|L_PTE_HIDDEN)), 0);
		flush_tlb_kernel_page(address);
		return;
	}
#endif
	#if 0
	if ((insn & 0x0e0000f0) == 0x000000b0) {
#if 0
		if ((insn & 0x0fb000f0) == 0x01000090) {
			/* SWP/SWPB */
			goto UNKNOWN_INSN;
		} else if ((insn & 0x0e1000d0) == 0x00000d0) {
			/* STRD/LDRD */
			goto UNKNOWN_INSN;
		}
#endif
		printk ("single_step: 0x%x, %x, %x\n", insn, address, regs->ARM_pc);
		set_pte_ext(pte, __pte(pte_val(*pte) | L_PTE_PRESENT), 0);
		flush_tlb_kernel_page(address);
		emulate_ldrh_strh(insn, regs);
		__cpuc_coherent_kern_range(address, address+PAGE_SIZE);
		set_pte_ext(pte, __pte(((pte_val(*pte) & ~L_PTE_PRESENT)|L_PTE_HIDDEN)), 0);
		flush_tlb_kernel_page(address);
		return;
	}
	#endif
UNKNOWN_INSN:
	set_pte_ext(pte, __pte(pte_val(*pte) | L_PTE_PRESENT), 0);
	return;
}

static struct kmemcheck_error *error_next_wr(void)
{
	struct kmemcheck_error *e;

	if (error_count == ARRAY_SIZE(error_fifo)) {
		++error_missed_count;
		return NULL;
	}

	e = &error_fifo[error_wr];
	if (++error_wr == ARRAY_SIZE(error_fifo))
		error_wr = 0;
	++error_count;
	return e;
}

static struct kmemcheck_error *error_next_rd(void)
{
	struct kmemcheck_error *e;

	if (error_count == 0)
		return NULL;

	e = &error_fifo[error_rd];
	if (++error_rd == ARRAY_SIZE(error_fifo))
		error_rd = 0;
	--error_count;
	return e;
}

static void *address_get_shadow(unsigned long address);

/*
 * Save the context of an error.
 */
static void error_save(enum shadow state,
	unsigned long address, unsigned int size, struct pt_regs *regs)
{
	static unsigned long prev_pc;

	struct kmemcheck_error *e;
	enum shadow *shadow_copy;

	/* Don't report several adjacent errors from the same EIP. */
	if (regs->ARM_pc == prev_pc)
		return;
	prev_pc = regs->ARM_pc;

	e = error_next_wr();
	if (!e)
		return;

	e->type = ERROR_INVALID_ACCESS;

	e->state = state;
	e->address = address;
	e->size = size;

	/* Save regs */
	memcpy(&e->regs, regs, sizeof(*regs));

	/* Save stack trace */
	e->trace.nr_entries = 0;
	e->trace.entries = e->trace_entries;
	e->trace.max_entries = ARRAY_SIZE(e->trace_entries);
	e->trace.skip = 0;
	save_stack_trace(&e->trace);

	/* Round address down to nearest 16 bytes */
	shadow_copy = address_get_shadow(address & ~(SHADOW_COPY_SIZE - 1));
	BUG_ON(!shadow_copy);

	memcpy(e->shadow_copy, shadow_copy, SHADOW_COPY_SIZE);
}

/*
 * Save the context of a kmemcheck bug.
 */
static void error_save_bug(struct pt_regs *regs)
{
	struct kmemcheck_error *e;

	e = error_next_wr();
	if (!e)
		return;

	e->type = ERROR_BUG;

	memcpy(&e->regs, regs, sizeof(*regs));

	e->trace.nr_entries = 0;
	e->trace.entries = e->trace_entries;
	e->trace.max_entries = ARRAY_SIZE(e->trace_entries);
	e->trace.skip = 1;
	save_stack_trace(&e->trace);
}

static void error_recall(void)
{
	static const char *desc[] = {
		[SHADOW_UNALLOCATED]	= "unallocated",
		[SHADOW_UNINITIALIZED]	= "uninitialized",
		[SHADOW_INITIALIZED]	= "initialized",
		[SHADOW_FREED]		= "freed",
	};

	static const char short_desc[] = {
		[SHADOW_UNALLOCATED]	= 'a',
		[SHADOW_UNINITIALIZED]	= 'u',
		[SHADOW_INITIALIZED]	= 'i',
		[SHADOW_FREED]		= 'f',
	};

	struct kmemcheck_error *e;
	unsigned int i;

	e = error_next_rd();
	if (!e)
		return;

	switch (e->type) {
	case ERROR_INVALID_ACCESS:
		printk(KERN_ERR  "kmemcheck: Caught %d-bit read "
			"from %s memory (%p)\n",
			e->size, e->state < ARRAY_SIZE(desc) ?
				desc[e->state] : "(invalid shadow state)",
			(void *) e->address);

		printk(KERN_INFO);
		for (i = 0; i < SHADOW_COPY_SIZE; ++i) {
			if (e->shadow_copy[i] < ARRAY_SIZE(short_desc))
				printk("%c", short_desc[e->shadow_copy[i]]);
			else
				printk("?");
		}
		printk("\n");
		printk(KERN_INFO "%*c\n",
			1 + (int) (e->address & (SHADOW_COPY_SIZE - 1)), '^');
		break;
	case ERROR_BUG:
		printk(KERN_EMERG "kmemcheck: Fatal error\n");
		break;
	}

	__show_regs(&e->regs);
	print_stack_trace(&e->trace, 0);
}

static void do_wakeup(unsigned long data)
{
	while (error_count > 0)
		error_recall();

	if (error_missed_count > 0) {
		printk(KERN_WARNING "kmemcheck: Lost %d error reports because "
			"the queue was too small\n", error_missed_count);
		error_missed_count = 0;
	}

	mod_timer(&kmemcheck_timer, kmemcheck_timer.expires + HZ);
}

void __init kmemcheck_init(void)
{
	printk(KERN_INFO "kmemcheck: \"Bugs, beware!\"\n");

#ifdef CONFIG_SMP
	/* Limit SMP to use a single CPU. We rely on the fact that this code
	 * runs before SMP is set up. */
	if (setup_max_cpus > 1) {
		printk(KERN_INFO
			"kmemcheck: Limiting number of CPUs to 1.\n");
		setup_max_cpus = 1;
	}
#endif

	setup_timer(&kmemcheck_timer, &do_wakeup, 0);
	mod_timer(&kmemcheck_timer, jiffies + HZ);
}

#ifdef CONFIG_KMEMCHECK_DISABLED_BY_DEFAULT
int kmemcheck_enabled;
#endif

#ifdef CONFIG_KMEMCHECK_ENABLED_BY_DEFAULT
int kmemcheck_enabled = 1;
#endif

#ifdef CONFIG_KMEMCHECK_ONESHOT_BY_DEFAULT
int kmemcheck_enabled = 2;
#endif

/*
 * We need to parse the kmemcheck= option before any memory is allocated.
 */
static int __init param_kmemcheck(char *str)
{
	if (!str)
		return -EINVAL;

	sscanf(str, "%d", &kmemcheck_enabled);
	return 0;
}

early_param("kmemcheck", param_kmemcheck);

static pte_t *address_get_pte(unsigned long address)
{
	pte_t *pte;
	unsigned int level;

	pte = lookup_address(address, &level);
	if (!pte)
		return NULL;
	if (!pte_hidden(*pte))
		return NULL;

	return pte;
}

/*
 * Return the shadow address for the given address. Returns NULL if the
 * address is not tracked.
 *
 * We need to be extremely careful not to follow any invalid pointers,
 * because this function can be called for *any* possible address.
 */
static void *address_get_shadow(unsigned long address)
{
	pte_t *pte;
	struct page *page;
#if 0
	if (!virt_addr_valid(address))
		return NULL;

	pte = address_get_pte(address);
	if (!pte)
		return NULL;
#endif
	page = virt_to_page(address);
	if (!page->shadow)
		return NULL;
	return page->shadow + (address & (PAGE_SIZE - 1));
}
static int show_addr(unsigned long address)
{
	pte_t *pte;

	pte = address_get_pte(address);
	if (!pte)
		return 0;
	set_pte_ext(pte, __pte(pte_val(*pte) | L_PTE_PRESENT), 0);
	flush_tlb_kernel_page(address);
	return 1;
}

static int hide_addr(unsigned long address)
{
	pte_t *pte;

	pte = address_get_pte(address);
	if (!pte)
		return 0;
	set_pte_ext(pte, __pte(((pte_val(*pte) & ~L_PTE_PRESENT)|L_PTE_HIDDEN)), 0);
	flush_tlb_kernel_page(address);
	return 1;
}
struct kmemcheck_context {
	bool busy;
	int balance;

	unsigned long addr1;
	unsigned long addr2;
	unsigned long flags;
};

static DEFINE_PER_CPU(struct kmemcheck_context, kmemcheck_context);

bool kmemcheck_active(struct pt_regs *regs)
{
	struct kmemcheck_context *data = &__get_cpu_var(kmemcheck_context);

	return data->balance > 0;
}

void kmemcheck_show_pages(struct page *p, unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; ++i) {
		unsigned long address;
		pte_t *pte;
		unsigned int level;

		address = (unsigned long) page_address(&p[i]);
		pte = lookup_address(address, &level);
		set_pte_ext(pte, __pte((pte_val(*pte) | L_PTE_PRESENT) & ~L_PTE_HIDDEN), 0);
		flush_tlb_kernel_page(address);
	}
}

bool kmemcheck_page_is_tracked(struct page *p)
{
	/* This will also check the "hidden" flag of the PTE. */
	return address_get_pte((unsigned long) page_address(p));
}

void kmemcheck_hide_pages(struct page *p, unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; ++i) {
		unsigned long address;
		pte_t *pte;
		unsigned int level;

		address = (unsigned long) page_address(&p[i]);
		pte = lookup_address(address, &level);
		set_pte_ext(pte, __pte((pte_val(*pte) & ~L_PTE_PRESENT)|L_PTE_HIDDEN), 0);
		flush_tlb_kernel_page(address);
	}
}

static void mark_shadow(void *address, unsigned int n, enum shadow status)
{
	void *shadow;

	shadow = address_get_shadow((unsigned long) address);
	if (!shadow)
		return;
	memset(shadow, status, n);
}

void kmemcheck_mark_unallocated(void *address, unsigned int n)
{
	mark_shadow(address, n, SHADOW_UNALLOCATED);
}

void kmemcheck_mark_uninitialized(void *address, unsigned int n)
{
	mark_shadow(address, n, SHADOW_UNINITIALIZED);
}

/*
 * Fill the shadow memory of the given address such that the memory at that
 * address is marked as being initialized.
 */
void kmemcheck_mark_initialized(void *address, unsigned int n)
{
	mark_shadow(address, n, SHADOW_INITIALIZED);
}

void kmemcheck_mark_freed(void *address, unsigned int n)
{
	mark_shadow(address, n, SHADOW_FREED);
}

void kmemcheck_mark_unallocated_pages(struct page *p, unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; ++i)
		kmemcheck_mark_unallocated(page_address(&p[i]), PAGE_SIZE);
}

void kmemcheck_mark_uninitialized_pages(struct page *p, unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; ++i)
		kmemcheck_mark_uninitialized(page_address(&p[i]), PAGE_SIZE);
}

/* This is a VERY crude opcode decoder. We only need to find the size of the
 * load/store that caused our #PF and this should work for all the opcodes
 * that we care about. Moreover, the ones who invented this instruction set
 * should be shot. */
static unsigned int opcode_get_size(const unsigned long insn)
{
	/* Default operand size */
	int operand_size_override = 0;
	if ((insn & 0x0c000000) == 0x04000000)
		operand_size_override = 32;
	if ((insn & 0x0e000000) == 0x08000000) {
		int reg_bit_vector;
		int reg_count;
		reg_count = 0;
		reg_bit_vector = insn & 0xffff;
		while (reg_bit_vector) {
			reg_bit_vector &= (reg_bit_vector - 1);
			++reg_count;
		}
		operand_size_override = reg_count * 32;
	}
	/*
	if (((insn & 0x0e0000f0) == 0x000000b0)){
		operand_size_override = 16;
	}
	*/
	return operand_size_override;
}

/*
 * Check that an access does not span across two different pages, because
 * that will mess up our shadow lookup.
 */
static bool check_page_boundary(struct pt_regs *regs,
	unsigned long addr, unsigned int size)
{
	if (size == 8)
		return false;
	if (size == 16 && (addr & PAGE_MASK) == ((addr + 1) & PAGE_MASK))
		return false;
	if (size == 32 && (addr & PAGE_MASK) == ((addr + 3) & PAGE_MASK))
		return false;

	/*
	 * XXX: The addr/size data is also really interesting if this
	 * case ever triggers. We should make a separate class of errors
	 * for this case. -Vegard
	 */
	error_save_bug(regs);
	return true;
}

static inline enum shadow test(void *shadow, unsigned int size)
{
	uint8_t *x;

	x = shadow;

#ifdef CONFIG_KMEMCHECK_PARTIAL_OK
	/*
	 * Make sure _some_ bytes are initialized. Gcc frequently generates
	 * code to access neighboring bytes.
	 */
	switch (size) {
	case 32:
		if (x[3] == SHADOW_INITIALIZED)
			return x[3];
		if (x[2] == SHADOW_INITIALIZED)
			return x[2];
	case 16:
		if (x[1] == SHADOW_INITIALIZED)
			return x[1];
	case 8:
		if (x[0] == SHADOW_INITIALIZED)
			return x[0];
	}
#else
	switch (size) {
	case 32:
		if (x[3] != SHADOW_INITIALIZED)
			return x[3];
		if (x[2] != SHADOW_INITIALIZED)
			return x[2];
	case 16:
		if (x[1] != SHADOW_INITIALIZED)
			return x[1];
	case 8:
		if (x[0] != SHADOW_INITIALIZED)
			return x[0];
	}
#endif

	return x[0];
}

static inline void set(void *shadow, unsigned int size)
{
	uint8_t *x;
	int i;

	x = shadow;

	for (i = 0; i < size/8; i++)
		x[i] = SHADOW_INITIALIZED;
	return;
}

static void kmemcheck_read(struct pt_regs *regs,
	unsigned long address, unsigned int size)
{
	void *shadow;
	enum shadow status;

	shadow = address_get_shadow(address);
	if (!shadow)
		return;
#if 0
	if (check_page_boundary(regs, address, size))
		return;
#endif
	status = test(shadow, size);
	if (status == SHADOW_INITIALIZED)
		return;

	if (kmemcheck_enabled)
		error_save(status, address, size, regs);

	if (kmemcheck_enabled == 2)
		kmemcheck_enabled = 0;

	/* Don't warn about it again. */
	set(shadow, size);
}

static void kmemcheck_write(struct pt_regs *regs,
	unsigned long address, unsigned int size)
{
	void *shadow;

	shadow = address_get_shadow(address);
	if (!shadow)
		return;
#if 0
	if (check_page_boundary(regs, address, size))
		return;
#endif
	set(shadow, size);
}

void kmemcheck_access(struct pt_regs *regs,
	unsigned long fallback_address, enum kmemcheck_method fallback_method)
{
	const uint8_t *insn;
	const uint8_t *insn_primary;
	unsigned int size;

	struct kmemcheck_context *data = &__get_cpu_var(kmemcheck_context);

	insn = (const uint8_t *) regs->ARM_pc;
	size = opcode_get_size(insn);
	if (size) {
	/* If the opcode isn't special in any way, we use the data from the
	 * page fault handler to determine the address and type of memory
	 * access. */
	switch (fallback_method) {
	case KMEMCHECK_READ:
		kmemcheck_read(regs, fallback_address, size);
		data->addr1 = fallback_address;
		data->addr2 = 0;
		data->busy = false;
		return;
	case KMEMCHECK_WRITE:
		kmemcheck_write(regs, fallback_address, size);
		data->addr1 = fallback_address;
		data->addr2 = 0;
		data->busy = false;
		return;
	}
		}
}

bool kmemcheck_fault(struct pt_regs *regs, unsigned long address,
	unsigned long error_code, pte_t *pte)
{
	if (error_code)
		kmemcheck_access(regs, address, KMEMCHECK_WRITE);
	else
		kmemcheck_access(regs, address, KMEMCHECK_READ);
	local_irq_disable();
	singlestep(regs, pte, address);
	local_irq_enable();
	return true;
}

/*
 * A faster implementation of memset() when tracking is enabled where the
 * whole memory area is within a single page.
 */
static void memset_one_page(void *s, int c, size_t n)
{
	unsigned long addr;
	void *x;
	unsigned long flags;

	addr = (unsigned long) s;

	x = address_get_shadow(addr);
	if (!x) {
		/* The page isn't being tracked. */
		memset(s, c, n);
		return;
	}

	/* While we are not guarding the page in question, nobody else
	 * should be able to change them. */
	local_irq_save(flags);

	show_addr(addr);
	memset(s, c, n);
	memset(x, SHADOW_INITIALIZED, n);

	local_irq_restore(flags);
}

/*
 * A faster implementation of memset() when tracking is enabled. We cannot
 * assume that all pages within the range are tracked, so copying has to be
 * split into page-sized (or smaller, for the ends) chunks.
 */
void *kmemcheck_memset(void *s, int c, size_t n)
{
	unsigned long addr;
	unsigned long start_page, start_offset;
	unsigned long end_page, end_offset;
	unsigned long i;

	if (!n)
		return s;

	if (!slab_is_available()) {
		memset(s, c, n);
		return s;
	}

	addr = (unsigned long) s;

	start_page = addr & PAGE_MASK;
	end_page = (addr + n) & PAGE_MASK;

	if (start_page == end_page) {
		/* The entire area is within the same page. Good, we only
		 * need one memset(). */
		memset_one_page(s, c, n);
		return s;
	}

	start_offset = addr & ~PAGE_MASK;
	end_offset = (addr + n) & ~PAGE_MASK;

	/* Clear the head, body, and tail of the memory area. */
	if (start_offset < PAGE_SIZE)
		memset_one_page(s, c, PAGE_SIZE - start_offset);
	for (i = start_page + PAGE_SIZE; i < end_page; i += PAGE_SIZE)
		memset_one_page((void *) i, c, PAGE_SIZE);
	if (end_offset > 0)
		memset_one_page((void *) end_page, c, end_offset);

	return s;
}
EXPORT_SYMBOL(kmemcheck_memset);
