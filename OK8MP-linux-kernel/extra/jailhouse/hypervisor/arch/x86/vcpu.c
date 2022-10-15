/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (c) Siemens AG, 2013
 * Copyright (c) Valentine Sinitsyn, 2014
 *
 * Authors:
 *  Jan Kiszka <jan.kiszka@siemens.com>
 *  Valentine Sinitsyn <valentine.sinitsyn@gmail.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/control.h>
#include <jailhouse/mmio.h>
#include <jailhouse/paging.h>
#include <jailhouse/pci.h>
#include <jailhouse/printk.h>
#include <jailhouse/string.h>
#include <jailhouse/types.h>
#include <asm/apic.h>
#include <asm/i8042.h>
#include <asm/ioapic.h>
#include <asm/pci.h>
#include <jailhouse/percpu.h>
#include <asm/vcpu.h>

#define for_each_pio_region(pio, config, counter)	\
	for ((pio) = jailhouse_cell_pio(config),	\
	     (counter) = 0;				\
	     (counter) < (config)->num_pio_regions;	\
	     (pio)++, (counter)++)

static u8 __attribute__((aligned(PAGE_SIZE))) parking_code[PAGE_SIZE] = {
	0xfa, /* 1: cli */
	0xf4, /*    hlt */
	0xeb,
	0xfc  /*    jmp 1b */
};

int vcpu_early_init(void)
{
	int err;

	err = vcpu_vendor_early_init();
	if (err)
		return err;

	/* Map guest parking code (shared between cells and CPUs) */
	return paging_create(&parking_pt, paging_hvirt2phys(parking_code),
			     PAGE_SIZE, 0, PAGE_READONLY_FLAGS | PAGE_FLAG_US,
			     PAGING_NON_COHERENT | PAGING_NO_HUGE);
}

/* Can be overridden in vendor-specific code if needed */
const u8 *vcpu_get_inst_bytes(const struct guest_paging_structures *pg_structs,
			      unsigned long pc, unsigned int *size)
	__attribute__((weak, alias("vcpu_map_inst")));

const u8 *vcpu_map_inst(const struct guest_paging_structures *pg_structs,
			unsigned long pc, unsigned int *size)
{
	unsigned short bytes_avail;
	u8 *page = NULL;

	if (!*size)
		goto out_err;
	page = paging_get_guest_pages(pg_structs, pc,
			1, PAGE_READONLY_FLAGS);
	if (!page)
		goto out_err;

	/* Number of bytes available before page boundary */
	bytes_avail = PAGE_SIZE - (pc & PAGE_OFFS_MASK);
	if (*size > bytes_avail)
		*size = bytes_avail;

	return &page[pc & PAGE_OFFS_MASK];

out_err:
	return NULL;
}

static void pio_allow_access(u8 *bm, const struct jailhouse_pio *pio,
			     bool access)
{
	void (*access_method)(unsigned int, volatile unsigned long*) =
		access ? clear_bit : set_bit;
	unsigned int length, start_bit = pio->base;

	for (length = pio->length; length > 0; length--, start_bit++)
		access_method(start_bit, (unsigned long*)bm);
}

int vcpu_cell_init(struct cell *cell)
{
	const unsigned int io_bitmap_pages = vcpu_vendor_get_io_bitmap_pages();
	const struct jailhouse_pio *pio;
	unsigned int n, pm_timer_addr;
	int err;

	cell->arch.io_bitmap = page_alloc(&mem_pool, io_bitmap_pages);
	if (!cell->arch.io_bitmap)
		return -ENOMEM;

	err = vcpu_vendor_cell_init(cell);
	if (err) {
		page_free(&mem_pool, cell->arch.io_bitmap, io_bitmap_pages);
		return err;
	}

	/* initialize io bitmap to trap all accesses */
	memset(cell->arch.io_bitmap, -1, io_bitmap_pages * PAGE_SIZE);

	/* cells have no access to i8042, unless the port is whitelisted */
	cell->arch.pio_i8042_allowed = false;

	for_each_pio_region(pio, cell->config, n) {
		pio_allow_access(cell->arch.io_bitmap, pio, true);

		/* moderate i8042 only if the config allows it */
		if (pio->base <= I8042_CMD_REG &&
		    pio->base + pio->length > I8042_CMD_REG)
			cell->arch.pio_i8042_allowed = true;
	}

	/* but always intercept access to i8042 command register */
	cell->arch.io_bitmap[I8042_CMD_REG / 8] |= 1 << (I8042_CMD_REG % 8);

	if (cell != &root_cell) {
		/*
		 * Shrink PIO access of root cell corresponding to new cell's
		 * access rights.
		 */
		for_each_pio_region(pio, cell->config, n)
			pio_allow_access(root_cell.arch.io_bitmap, pio, false);
	}

	/* permit access to the PM timer if there is any */
	pm_timer_addr = system_config->platform_info.x86.pm_timer_address;
	if (pm_timer_addr)
		for (n = 0; n < 4; n++, pm_timer_addr++)
			cell->arch.io_bitmap[pm_timer_addr / 8] &=
				~(1 << (pm_timer_addr % 8));

	return 0;
}

void vcpu_cell_exit(struct cell *cell)
{
	const struct jailhouse_pio *cell_wl, *root_wl;
	unsigned int interval_start, interval_end, m, n;
	struct jailhouse_pio refund;

	/* Hand back ports to the root cell. But only hand back those ports
	 * that overlap with the root cell's config. This is done by pairwise
	 * comparison of the cell's and the root cell's whitelist entries. */
	for_each_pio_region(cell_wl, cell->config, m)
		for_each_pio_region(root_wl, root_cell.config, n) {
			interval_start = MAX(cell_wl->base, root_wl->base);
			interval_end = MIN(cell_wl->base + cell_wl->length,
					   root_wl->base + root_wl->length);
			if (interval_start < interval_end) {
				refund.base = interval_start;
				refund.length = interval_end - interval_start;
				pio_allow_access(root_cell.arch.io_bitmap,
						 &refund, true);
			}
		}

	page_free(&mem_pool, cell->arch.io_bitmap,
		  vcpu_vendor_get_io_bitmap_pages());

	vcpu_vendor_cell_exit(cell);
}

void vcpu_handle_hypercall(void)
{
	union registers *guest_regs = &this_cpu_data()->guest_regs;
	u16 cs_attr = vcpu_vendor_get_cs_attr();
	bool long_mode = (vcpu_vendor_get_efer() & EFER_LMA) &&
		(cs_attr & VCPU_CS_L);
	unsigned long arg_mask = long_mode ? (u64)-1 : (u32)-1;
	unsigned long code = guest_regs->rax;
	unsigned int cpu_id = this_cpu_id();

	vcpu_skip_emulated_instruction(X86_INST_LEN_HYPERCALL);

	if ((!long_mode && (vcpu_vendor_get_rflags() & X86_RFLAGS_VM)) ||
	    (cs_attr & VCPU_CS_DPL_MASK) != 0) {
		guest_regs->rax = -EPERM;
		return;
	}

	guest_regs->rax = hypercall(code, guest_regs->rdi & arg_mask,
				    guest_regs->rsi & arg_mask);
	if (guest_regs->rax == -ENOSYS)
		printk("CPU %d: Unknown hypercall %ld, RIP: 0x%016llx\n",
		       cpu_id, code,
		       vcpu_vendor_get_rip() - X86_INST_LEN_HYPERCALL);

	if (code == JAILHOUSE_HC_DISABLE && guest_regs->rax == 0) {
		/*
		 * Restore full per_cpu region access so that we can switch
		 * back to the common stack mapping and to Linux page tables.
		 */
		paging_map_all_per_cpu(cpu_id, true);

		/*
		 * Switch the stack back to the common mapping.
		 * Preexisting pointers to the stack remain valid until we also
		 * switch the page tables in arch_cpu_restore.
		 */
		asm volatile(
			"sub %0,%%rsp"
			: : "g" (LOCAL_CPU_BASE -
				 (unsigned long)per_cpu(cpu_id)));

		vcpu_deactivate_vmm();
	}
}

bool vcpu_handle_io_access(void)
{
	struct vcpu_io_intercept io;
	int result = 0;

	vcpu_vendor_get_io_intercept(&io);

	/* string and REP-prefixed instructions are not supported */
	if (io.rep_or_str)
		goto invalid_access;

	result = x86_pci_config_handler(io.port, io.in, io.size);
	if (result == 0)
		result = i8042_access_handler(io.port, io.in, io.size);

	/* Also ignore byte access to port 80, often used for delaying IO. */
	if (result == 1 || (io.port == 0x80 && io.size == 1)) {
		vcpu_skip_emulated_instruction(io.inst_len);
		return true;
	}

invalid_access:
	/* report only unhandled access failures */
	if (result == 0)
		panic_printk("FATAL: Invalid PIO %s, port: %x size: %d\n",
			     io.in ? "read" : "write", io.port, io.size);
	return false;
}

bool vcpu_handle_mmio_access(void)
{
	union registers *guest_regs = &this_cpu_data()->guest_regs;
	enum mmio_result result = MMIO_UNHANDLED;
	struct guest_paging_structures pg_structs;
	struct mmio_access mmio = {.size = 0};
	struct vcpu_mmio_intercept intercept;
	struct mmio_instruction inst;
	unsigned long *reg;

	vcpu_vendor_get_mmio_intercept(&intercept);

	vcpu_get_guest_paging_structs(&pg_structs);

	inst = x86_mmio_parse(&pg_structs, intercept.is_write);
	if (!inst.inst_len)
		goto invalid_access;

	mmio.is_write = intercept.is_write;
	if (mmio.is_write)
		mmio.value = inst.out_val;

	mmio.address = intercept.phys_addr;
	mmio.size = inst.access_size;

	result = mmio_handle_access(&mmio);
	if (result == MMIO_HANDLED) {
		if (!mmio.is_write) {
			reg= &guest_regs->by_index[inst.in_reg_num];
			*reg = (*reg & inst.reg_preserve_mask) | mmio.value;
		}
		vcpu_skip_emulated_instruction(inst.inst_len);
		return true;
	}

invalid_access:
	/* report only unhandled access failures */
	if (result == MMIO_UNHANDLED)
		panic_printk("FATAL: Invalid MMIO/RAM %s, "
			     "addr: 0x%016llx size: %d\n",
			     intercept.is_write ? "write" : "read",
			     intercept.phys_addr, mmio.size);
	return false;
}

bool vcpu_handle_msr_read(void)
{
	struct per_cpu *cpu_data = this_cpu_data();

	switch (cpu_data->guest_regs.rcx) {
	case MSR_X2APIC_BASE ... MSR_X2APIC_END:
		x2apic_handle_read();
		break;
	case MSR_IA32_PAT:
		cpu_data->public.stats[JAILHOUSE_CPU_STAT_VMEXITS_MSR_OTHER]++;
		set_rdmsr_value(&cpu_data->guest_regs, cpu_data->pat);
		break;
	case MSR_IA32_MTRR_DEF_TYPE:
		cpu_data->public.stats[JAILHOUSE_CPU_STAT_VMEXITS_MSR_OTHER]++;
		set_rdmsr_value(&cpu_data->guest_regs,
				cpu_data->mtrr_def_type);
		break;
	default:
		panic_printk("FATAL: Unhandled MSR read: %lx\n",
			     cpu_data->guest_regs.rcx);
		return false;
	}

	vcpu_skip_emulated_instruction(X86_INST_LEN_WRMSR);
	return true;
}

bool vcpu_handle_msr_write(void)
{
	struct per_cpu *cpu_data = this_cpu_data();
	unsigned int bit_pos, pa;
	unsigned long val;

	switch (cpu_data->guest_regs.rcx) {
	case MSR_X2APIC_BASE ... MSR_X2APIC_END:
		if (!x2apic_handle_write())
			return false;
		break;
	case MSR_IA32_PAT:
		cpu_data->public.stats[JAILHOUSE_CPU_STAT_VMEXITS_MSR_OTHER]++;
		val = get_wrmsr_value(&cpu_data->guest_regs);
		for (bit_pos = 0; bit_pos < 64; bit_pos += 8) {
			pa = (val >> bit_pos) & 0xff;
			/* filter out reserved memory types */
			if (pa == 2 || pa == 3 || pa > 7) {
				printk("FATAL: Invalid PAT value: %lx\n", val);
				return false;
			}
		}
		cpu_data->pat = val;
		if (cpu_data->mtrr_def_type & MTRR_ENABLE)
			vcpu_vendor_set_guest_pat(val);
		break;
	case MSR_IA32_MTRR_DEF_TYPE:
		cpu_data->public.stats[JAILHOUSE_CPU_STAT_VMEXITS_MSR_OTHER]++;
		/*
		 * This only emulates the difference between MTRRs enabled
		 * and disabled. When disabled, we turn off all caching by
		 * setting the guest PAT to 0. When enabled, guest PAT +
		 * host-controlled MTRRs define the guest's memory types.
		 */
		val = get_wrmsr_value(&cpu_data->guest_regs);
		cpu_data->mtrr_def_type &= ~MTRR_ENABLE;
		cpu_data->mtrr_def_type |= val & MTRR_ENABLE;
		vcpu_vendor_set_guest_pat((val & MTRR_ENABLE) ?
					  cpu_data->pat : 0);
		break;
	default:
		panic_printk("FATAL: Unhandled MSR write: %lx\n",
			     cpu_data->guest_regs.rcx);
		return false;
	}

	vcpu_skip_emulated_instruction(X86_INST_LEN_WRMSR);
	return true;
}

void vcpu_handle_cpuid(void)
{
	static const char signature[12] = "Jailhouse";
	union registers *guest_regs = &this_cpu_data()->guest_regs;
	u32 function = guest_regs->rax;

	this_cpu_data()->public.stats[JAILHOUSE_CPU_STAT_VMEXITS_CPUID]++;

	switch (function) {
	case JAILHOUSE_CPUID_SIGNATURE:
		guest_regs->rax = JAILHOUSE_CPUID_FEATURES;
		guest_regs->rbx = *(u32 *)signature;
		guest_regs->rcx = *(u32 *)(signature + 4);
		guest_regs->rdx = *(u32 *)(signature + 8);
		break;
	case JAILHOUSE_CPUID_FEATURES:
		guest_regs->rax = 0;
		guest_regs->rbx = 0;
		guest_regs->rcx = 0;
		guest_regs->rdx = 0;
		break;
	default:
		/* clear upper 32 bits of the involved registers */
		guest_regs->rax &= 0xffffffff;
		guest_regs->rbx &= 0xffffffff;
		guest_regs->rcx &= 0xffffffff;
		guest_regs->rdx &= 0xffffffff;

		cpuid((u32 *)&guest_regs->rax, (u32 *)&guest_regs->rbx,
		      (u32 *)&guest_regs->rcx, (u32 *)&guest_regs->rdx);
		if (function == 0x01) {
			if (this_cell() != &root_cell) {
				guest_regs->rcx &= ~X86_FEATURE_VMX;
				guest_regs->rcx |= X86_FEATURE_HYPERVISOR;
			}

			guest_regs->rcx &= ~X86_FEATURE_OSXSAVE;
			if (vcpu_vendor_get_guest_cr4() & X86_CR4_OSXSAVE)
				guest_regs->rcx |= X86_FEATURE_OSXSAVE;
		} else if (function == 0x80000001) {
			if (this_cell() != &root_cell)
				guest_regs->rcx &= ~X86_FEATURE_SVM;
		}
		break;
	}

	vcpu_skip_emulated_instruction(X86_INST_LEN_CPUID);
}

void vcpu_reset(unsigned int sipi_vector)
{
	struct per_cpu *cpu_data = this_cpu_data();

	vcpu_vendor_reset(sipi_vector);

	memset(&cpu_data->guest_regs, 0, sizeof(cpu_data->guest_regs));

	if (sipi_vector == APIC_BSP_PSEUDO_SIPI) {
		cpu_data->pat = PAT_RESET_VALUE;
		cpu_data->mtrr_def_type &= ~MTRR_ENABLE;
		vcpu_vendor_set_guest_pat(0);
	}
}
