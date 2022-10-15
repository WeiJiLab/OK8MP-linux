/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Minimal configuration for PCI demo inmate:
 * 1 CPU, 1 MB RAM, serial ports, 1 Intel HDA PCI device
 *
 * Copyright (c) Siemens AG, 2014
 *
 * Authors:
 *  Jan Kiszka <jan.kiszka@siemens.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_cell_desc cell;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[3];
	struct jailhouse_pio pio_regions[3];
	struct jailhouse_pci_device pci_devices[1];
	struct jailhouse_pci_capability pci_caps[1];
} __attribute__((packed)) config = {
	.cell = {
		.signature = JAILHOUSE_CELL_DESC_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.name = "pci-demo",
		.flags = JAILHOUSE_CELL_PASSIVE_COMMREG |
			JAILHOUSE_CELL_VIRTUAL_CONSOLE_PERMITTED,

		.cpu_set_size = sizeof(config.cpus),
		.num_memory_regions = ARRAY_SIZE(config.mem_regions),
		.num_irqchips = 0,
		.num_pio_regions = ARRAY_SIZE(config.pio_regions),
		.num_pci_devices = ARRAY_SIZE(config.pci_devices),
		.num_pci_caps = ARRAY_SIZE(config.pci_caps),

		.console = {
			.type = JAILHOUSE_CON_TYPE_8250,
			.flags = JAILHOUSE_CON_ACCESS_PIO,
			.address = 0x3f8,
		},
	},

	.cpus = {
		0x4,
	},

	.mem_regions = {
		/* RAM */ {
			.phys_start = 0x3ee00000,
			.virt_start = 0,
			.size = 0x00100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* communication region */ {
			.virt_start = 0x00100000,
			.size = 0x00001000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_COMM_REGION,
		},
		/* HDA BAR0 */ {
			.phys_start = 0xfebd4000,
			.virt_start = 0xfebd4000,
			.size = 0x00004000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
	},

	.pio_regions = {
		PIO_RANGE(0x2f8, 8), /* serial 2 */
		PIO_RANGE(0x3f8, 8), /* serial 1 */
		PIO_RANGE(0xe010, 8), /* OXPCIe952 serial2 */
	},

	.pci_devices = {
		{ /* Intel HDA @00:1b.0 */
			.type = JAILHOUSE_PCI_TYPE_DEVICE,
			.domain = 0x0000,
			.bdf = 0x00d8,
			.caps_start = 0,
			.num_caps = 1,
			.num_msi_vectors = 1,
			.msi_64bits = 1,
		},
	},

	.pci_caps = {
		{ /* Intel HDA @00:1b.0 */
			.id = PCI_CAP_ID_MSI,
			.start = 0x60,
			.len = 14,
			.flags = JAILHOUSE_PCICAPS_WRITE,
		},
	},
};
