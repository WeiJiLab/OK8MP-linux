/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Configuration for Linux inmate on J721E based platforms
 * 1 CPUs, 512MB RAM, 1 serial port
 *
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef CONFIG_INMATE_BASE
#define CONFIG_INMATE_BASE 0x0000000
#endif

struct {
	struct jailhouse_cell_desc cell;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[25];
	struct jailhouse_irqchip irqchips[4];
	struct jailhouse_pci_device pci_devices[1];
	__u32 stream_ids[2];
} __attribute__((packed)) config = {
	.cell = {
		.signature = JAILHOUSE_CELL_DESC_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.name = "k3-j721e-evm-linux-demo",
		.flags = JAILHOUSE_CELL_PASSIVE_COMMREG,

		.cpu_set_size = sizeof(config.cpus),
		.num_memory_regions = ARRAY_SIZE(config.mem_regions),
		.num_irqchips = ARRAY_SIZE(config.irqchips),
		.num_pci_devices = ARRAY_SIZE(config.pci_devices),
		.num_stream_ids = ARRAY_SIZE(config.stream_ids),
		.cpu_reset_address = 0x0,
		.vpci_irq_base = 195 - 32,
		.console = {
			.address = 0x2810000,
			.divider = 0x1b,
			.type = JAILHOUSE_CON_TYPE_8250,
			.flags = JAILHOUSE_CON_ACCESS_MMIO |
				 JAILHOUSE_CON_REGDIST_4,
		},
	},

	.cpus = {
		0x2,
	},

	.mem_regions = {
		/* IVSHMEM shared memory region for 00:01.0 */
		JAILHOUSE_SHMEM_NET_REGIONS(0x89fe00000, 1),
		/* ctrl mmr */ {
			.phys_start = 0x00100000,
			.virt_start = 0x00100000,
			.size = 0x00020000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* Main.uart1 */ {
			.phys_start = 0x02810000,
			.virt_start = 0x02810000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* sdhci0 */ {
			.phys_start = 0x4f80000,
			.virt_start = 0x4f80000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* sdhci0 */ {
			.phys_start = 0x4f88000,
			.virt_start = 0x4f88000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* main_gpio2 */ {
			.phys_start = 0x00610000,
			.virt_start = 0x00610000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* main_gpio3 */ {
			.phys_start = 0x00611000,
			.virt_start = 0x00611000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* main sproxy target_data host_id=A72_3 */ {
			.phys_start = 0x3240f000,
			.virt_start = 0x3240f000,
			.size = 0x05000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* main sproxy rt host_id=A72_3 */ {
			.phys_start = 0x3280f000,
			.virt_start = 0x3280f000,
			.size = 0x05000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* main sproxy scfg host_id=A72_3 */ {
			.phys_start = 0x32c0f000,
			.virt_start = 0x32c0f000,
			.size = 0x05000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* D552 decoder */ {
			.phys_start = 0x4300000,
			.virt_start = 0x4300000,
			.size = 0x100000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* GPU */ {
			.phys_start = 0x4e20000000,
			.virt_start = 0x4e20000000,
			.size = 0x80000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* dss.common_s1 */ {
			.phys_start = 0x4B00000,
			.virt_start = 0x4B00000,
			.size = 0x00010000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* dss.vidl1 */ {
			.phys_start = 0x4A20000,
			.virt_start = 0x4A20000,
			.size = 0x00010000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* dss.ovr1 */ {
			.phys_start = 0x4A70000,
			.virt_start = 0x4A70000,
			.size = 0x00010000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* dss.vp1 */ {
			.phys_start = 0x4A80000,
			.virt_start = 0x4A80000,
			.size = 0x00010000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* serdes 10G */ {
			.phys_start = 0x05050000,
			.virt_start = 0x05050000,
			.size = 0x10000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* DSS_EDP0_V2A_CORE_VP_REGS_AP */ {
			.phys_start = 0x0A000000,
			.virt_start = 0x0A000000,
			.size = 0x31000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* DSS_EDP0_INTG_CFG_VP */ {
			.phys_start = 0x04F40000,
			.virt_start = 0x04F40000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* linux-loader space */ {
			.phys_start = 0x89ff00000,
			.virt_start = 0x0,
			.size = 0x10000,	/* 64KB */
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE,
		},
		/* RAM. */ {
			.phys_start = 0x8a0000000,
			.virt_start = 0x8a0000000,
			.size = 0x60000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_DMA |
				JAILHOUSE_MEM_LOADABLE,
		},
		/* communication region */ {
			.virt_start = 0x80000000,
			.size = 0x00001000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_COMM_REGION,
		},
	},

	.irqchips = {
		/*
		 * offset = (SPI_NR + 32 - base) / 32
		 * bit = (SPI_NR + 32 - base) % 32
		 */
		{
			.address = 0x01800000,
			.pin_base = 32,
			/*
			 * sdhci0 SPI_3				=> idx 0 bit 3
			 * sproxy_rx_016 SPI_71			=> idx 1 bit 7
			 * usb1.host SPI_104			=> idx 3 bit 8
			 * usb1.peripheral SPI_110		=> idx 3 bit 14
			 * usb1.otg SPI_121			=> idx 3 bit 25
			 */
			.pin_bitmap = {
				0x00000008, 0x00000080, 0x00000000, 0x02004100,
			},
		},
		{
			.address = 0x01800000,
			.pin_base = 160,
			/*
			 * virtual PCI SPI_163 to 166		=> idx 1 bit [6:3]
			 * main_uart1 SPI_193 to 166		=> idx 2 bit 1
			 */
			.pin_bitmap = {
				0x00000000, 0x00000078, 0x00000002, 0x00000000,
			},
		},
		{
			.address = 0x01800000,
			.pin_base = 416,
			/*
			 * main_gpio_intr SPI_392 to 416	=> idx 0 bit [31:8]
			 * ^^^^THIS^^^^ should match with SYSFW rm-cfg.c
			 */
			/* GPIO INTR */
			.pin_bitmap = {
				0xffffff00, 0x00000000, 0x00000000, 0x00000000,
			},
		},
		{
			.address = 0x01800000,
			.pin_base = 544,
			.pin_bitmap = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
			},
		},
	},

	.pci_devices = {
		/* 00:01.0 */ {
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 1,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_VETH,
		},
	},

	.stream_ids = {
		/* Non PCIe peripherals */
		0x0003, 0xf003,
	},
};
