/*
 * i.MX8MM Target
 *
 * Copyright 2018 NXP
 *
 * Authors:
 *  Peng Fan <peng.fan@nxp.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 * Reservation via device tree: reg = <0x0 0xffaf0000 0x0 0x510000>
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_system header;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[15];
	struct jailhouse_irqchip irqchips[3];
	struct jailhouse_pci_device pci_devices[2];
} __attribute__((packed)) config = {
	.header = {
		.signature = JAILHOUSE_SYSTEM_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.flags = JAILHOUSE_SYS_VIRTUAL_DEBUG_CONSOLE,
		.hypervisor_memory = {
			.phys_start = 0xfdc00000,
			.size =       0x00400000,
		},
		.debug_console = {
			.address = 0x30890000,
			.size = 0x1000,
			.flags = JAILHOUSE_CON_TYPE_IMX |
				 JAILHOUSE_CON_ACCESS_MMIO |
				 JAILHOUSE_CON_REGDIST_4,
			.type = JAILHOUSE_CON_TYPE_IMX,
		},
		.platform_info = {
			.pci_mmconfig_base = 0xfd700000,
			.pci_mmconfig_end_bus = 0,
			.pci_is_virtual = 1,
			.pci_domain = 0,

			.arm = {
				.gic_version = 3,
				.gicd_base = 0x38800000,
				.gicr_base = 0x38880000,
				.maintenance_irq = 25,
			},
		},
		.root_cell = {
			.name = "imx8mp",

			.num_pci_devices = ARRAY_SIZE(config.pci_devices),
			.cpu_set_size = sizeof(config.cpus),
			.num_memory_regions = ARRAY_SIZE(config.mem_regions),
			.num_irqchips = ARRAY_SIZE(config.irqchips),
			/* gpt5/4/3/2 not used by root cell */
			.vpci_irq_base = 51, /* Not include 32 base */
		},
	},

	.cpus = {
		0xf,
	},

	.mem_regions = {
		/* IVHSMEM shared memory region for 00:00.0 (demo )*/ {
			.phys_start = 0xfd900000,
			.virt_start = 0xfd900000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ,
		},
		{
			.phys_start = 0xfd901000,
			.virt_start = 0xfd901000,
			.size = 0x9000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		{
			.phys_start = 0xfd90a000,
			.virt_start = 0xfd90a000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE ,
		},
		{
			.phys_start = 0xfd90c000,
			.virt_start = 0xfd90c000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ,
		},
		{
			.phys_start = 0xfd90e000,
			.virt_start = 0xfd90e000,
			.size = 0x2000,
			.flags = JAILHOUSE_MEM_READ,
		},
		/* IVSHMEM shared memory regions for 00:01.0 (networking) */
		JAILHOUSE_SHMEM_NET_REGIONS(0xfda00000, 0),
		/* IO */ {
			.phys_start = 0x00000000,
			.virt_start = 0x00000000,
			.size =	      0x40000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* RAM 00*/ {
			.phys_start = 0x40000000,
			.virt_start = 0x40000000,
			.size = 0x80000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* Inmate memory */{
			.phys_start = 0xc0000000,
			.virt_start = 0xc0000000,
			.size = 0x3d700000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* Loader */{
			.phys_start = 0xfdb00000,
			.virt_start = 0xfdb00000,
			.size = 0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
		/* OP-TEE reserved memory?? */{
			.phys_start = 0xfe000000,
			.virt_start = 0xfe000000,
			.size = 0x2000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		/* RAM04 */{
			.phys_start = 0x100000000,
			.virt_start = 0x100000000,
			.size = 0xC0000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
	},

	.irqchips = {
		/* GIC */ {
			.address = 0x38800000,
			.pin_base = 32,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		/* GIC */ {
			.address = 0x38800000,
			.pin_base = 160,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		/* GIC */ {
			.address = 0x38800000,
			.pin_base = 288,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
	},

	.pci_devices = {
		{ /* IVSHMEM 0000:00:00.0 (demo) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 0,
			.bdf = 0 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 0,
			.shmem_peers = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
		{ /* IVSHMEM 0000:00:01.0 (networking) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 0,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 5,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
		},
	},
};
