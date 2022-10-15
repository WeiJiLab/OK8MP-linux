/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (c) ARM Limited, 2014
 *
 * Authors:
 *  Jean-Philippe Brucker <jean-philippe.brucker@arm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#ifndef _JAILHOUSE_ASM_CONTROL_H
#define _JAILHOUSE_ASM_CONTROL_H

#define SGI_INJECT	0
#define SGI_EVENT	1

#ifndef __ASSEMBLY__

#include <jailhouse/percpu.h>

void arch_handle_sgi(u32 irqn, unsigned int count_event);
bool arch_handle_phys_irq(u32 irqn, unsigned int count_event);

union registers* arch_handle_exit(union registers *regs);

void arch_shutdown_self(struct per_cpu *cpu_data);

unsigned int arm_cpu_by_mpidr(struct cell *cell, unsigned long mpidr);

void arm_cpu_reset(unsigned long pc);
void arm_cpu_park(void);

#endif /* !__ASSEMBLY__ */

#endif /* !_JAILHOUSE_ASM_CONTROL_H */
