/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Configuration for K3 based J721E-EVM
 *
 * Authors:
 *  Nikhil Devshatwar <nikhil.nd@ti.com>
 *  Lokesh Vutla <lokeshvutla@ti.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_system header;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[36];
	struct jailhouse_irqchip irqchips[6];
	struct jailhouse_pci_device pci_devices[1];
	__u32 stream_ids[30];
} __attribute__((packed)) config = {
	.header = {
		.signature = JAILHOUSE_SYSTEM_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.flags = JAILHOUSE_SYS_VIRTUAL_DEBUG_CONSOLE,
		.hypervisor_memory = {
			.phys_start = 0x89fa00000,
			.size = 0x400000,
		},
		.debug_console = {
			.address = 0x02800000,
			.size = 0x1000,
			.type = JAILHOUSE_CON_TYPE_8250,
			.flags = JAILHOUSE_CON_ACCESS_MMIO |
				 JAILHOUSE_CON_MDR_QUIRK |
				 JAILHOUSE_CON_REGDIST_4,
		},
		.platform_info = {
			.pci_mmconfig_base = 0x76000000,
			.pci_mmconfig_end_bus = 0,
			.pci_is_virtual = 1,
			.pci_domain = 3,
			.arm = {
				.gic_version = 3,
				.gicd_base = 0x01800000,
				.gicr_base = 0x01900000,
				.maintenance_irq = 25,
			},
			.arm.iommu_units= {
				{
					.type = JAILHOUSE_IOMMU_SMMUV3,
					.base = 0x36600000,
					.size = 0x100000,
				},
				{
					.type = JAILHOUSE_IOMMU_PVU,
					.base = 0x30f80000,
					.size = 0x1000,
					.tipvu.tlb_base = 0x36000000,
					.tipvu.tlb_size = 0x40000,
				},
				{
					.type = JAILHOUSE_IOMMU_PVU,
					.base = 0x30f81000,
					.size = 0x1000,
					.tipvu.tlb_base = 0x36040000,
					.tipvu.tlb_size = 0x40000,
				},
				{
					.type = JAILHOUSE_IOMMU_PVU,
					.base = 0x30f83000,
					.size = 0x1000,
					.tipvu.tlb_base = 0x360c0000,
					.tipvu.tlb_size = 0x40000,
				},
			},

		},
		.root_cell = {
			.name = "k3-j721e-evm",

			.cpu_set_size = sizeof(config.cpus),
			.num_memory_regions = ARRAY_SIZE(config.mem_regions),
			.num_irqchips = ARRAY_SIZE(config.irqchips),
			.num_pci_devices = ARRAY_SIZE(config.pci_devices),
			.num_stream_ids = ARRAY_SIZE(config.stream_ids),
			.vpci_irq_base = 191 - 32,
		},
	},

	.cpus = {
		0x3,
	},

	.mem_regions = {
		/* IVSHMEM shared memory region for 00:01.0 */
		JAILHOUSE_SHMEM_NET_REGIONS(0x89fe00000, 0),
		/* ctrl mmr */ {
			.phys_start = 0x00100000,
			.virt_start = 0x00100000,
			.size = 0x00020000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* gpio */ {
			.phys_start = 0x00600000,
			.virt_start = 0x00600000,
			.size = 0x00032000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* serdes */ {
			.phys_start = 0x00900000,
			.virt_start = 0x00900000,
			.size = 0x00012000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* timesync router */ {
			.phys_start = 0x00A40000,
			.virt_start = 0x00A40000,
			.size = 0x00001000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* usbss0 */ {
			.phys_start = 0x06000000,
			.virt_start = 0x06000000,
			.size = 0x00400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* usbss1 */ {
			.phys_start = 0x06400000,
			.virt_start = 0x06400000,
			.size = 0x00400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* Most peripherals */ {
			.phys_start = 0x01000000,
			.virt_start = 0x01000000,
			.size = 0x0af03000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MAIN NAVSS */ {
			.phys_start = 0x30800000,
			.virt_start = 0x30800000,
			.size = 0x0bc00000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* PCIe Core */ {
			.phys_start = 0x0d000000,
			.virt_start = 0x0d000000,
			.size = 0x01000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* PCIe DAT */ {
			.phys_start = 0x10000000,
			.virt_start = 0x10000000,
			.size = 0x10000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_DMA,
		},
		/* C71 */ {
			.phys_start = 0x64800000,
			.virt_start = 0x64800000,
			.size = 0x00800000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* C66_0 */ {
			.phys_start = 0x4D80800000,
			.virt_start = 0x4D80800000,
			.size = 0x00800000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* C66_1 */ {
			.phys_start = 0x4D81800000,
			.virt_start = 0x4D81800000,
			.size = 0x00800000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPU */ {
			.phys_start = 0x4E20000000,
			.virt_start = 0x4E20000000,
			.size = 0x00080000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MSMC SRAM */ {
			.phys_start = 0x4E20000000,
			.virt_start = 0x4E20000000,
			.size = 0x00080000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_DMA,
		},

		/* MCU NAVSS */ {
			.phys_start = 0x28380000,
			.virt_start = 0x28380000,
			.size = 0x03880000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU First peripheral window */ {
			.phys_start = 0x40200000,
			.virt_start = 0x40200000,
			.size = 0x00999000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU CTRL_MMR0 */ {
			.phys_start = 0x40f00000,
			.virt_start = 0x40f00000,
			.size = 0x00020000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU R5F Core0 */ {
			.phys_start = 0x41000000,
			.virt_start = 0x41000000,
			.size = 0x00020000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU R5F Core1 */ {
			.phys_start = 0x41400000,
			.virt_start = 0x41400000,
			.size = 0x00020000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU SRAM */ {
			.phys_start = 0x41c00000,
			.virt_start = 0x41c00000,
			.size = 0x00100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_DMA,
		},
		/* MCU WKUP peripheral window */ {
			.phys_start = 0x42040000,
			.virt_start = 0x42040000,
			.size = 0x03ac3000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU MMRs, remaining NAVSS */ {
			.phys_start = 0x45100000,
			.virt_start = 0x45100000,
			.size = 0x00c24000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU CPSW */ {
			.phys_start = 0x46000000,
			.virt_start = 0x46000000,
			.size = 0x00200000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU OSPI register space */ {
			.phys_start = 0x47000000,
			.virt_start = 0x47000000,
			.size = 0x00069000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* MCU FSS OSPI0/1 data region 0 */ {
			.phys_start = 0x50000000,
			.virt_start = 0x50000000,
			.size = 0x10000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_DMA,
		},
		/* MCU FSS OSPI0 data region 3 */ {
			.phys_start = 0x500000000,
			.virt_start = 0x500000000,
			.size = 0x100000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_DMA,
		},
		/* MCU FSS OSPI1 data region 3 */ {
			.phys_start = 0x700000000,
			.virt_start = 0x700000000,
			.size = 0x100000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_DMA,
		},

		/* RAM - first bank*/ {
			.phys_start = 0x80000000,
			.virt_start = 0x80000000,
			.size = 0x80000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_DMA |
				JAILHOUSE_MEM_LOADABLE,
		},
		/* RAM - second bank */ {
			.phys_start = 0x880000000,
			.virt_start = 0x880000000,
			.size = 0x1fa00000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_DMA |
				JAILHOUSE_MEM_LOADABLE,
		},
		/* RAM - reserved for ivshmem and baremetal apps */ {
			.phys_start = 0x89fe00000,
			.virt_start = 0x89fe00000,
			.size = 0x200000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* RAM - reserved for inmate */ {
			.phys_start = 0x8a0000000,
			.virt_start = 0x8a0000000,
			.size = 0x60000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
	},
	.irqchips = {
		{
			.address = 0x01800000,
			.pin_base = 32,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		{
			.address = 0x01800000,
			.pin_base = 160,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		{
			.address = 0x01800000,
			.pin_base = 288,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		{
			.address = 0x01800000,
			.pin_base = 416,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		{
			.address = 0x01800000,
			.pin_base = 544,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
		{
			.address = 0x01800000,
			.pin_base = 800,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
	},

	.pci_devices = {
		/* 0003:00:01.0 */ {
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 3,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
		},
	},

	.stream_ids = {
		/* Non PCIe peripherals */
		0x0002, 0xf002,
		/* PCI1 */
		0x0100, 0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107,
		0x0108, 0x0109, 0x010a, 0x010b, 0x010c, 0x010d, 0x010e, 0x010f,
		/* PCI2 */
		0x4100, 0x4101, 0x4102, 0x4103, 0x4104, 0x4105,
		/* PCI3 */
		0x8100, 0x8101, 0x8102, 0x8103, 0x8104, 0x8105,
	},
};
