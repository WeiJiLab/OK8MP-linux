/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (c) Siemens AG, 2019-2020
 *
 * Authors:
 *  Jan Kiszka <jan.kiszka@siemens.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <errno.h>
#include <error.h>
#include <libgen.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/signalfd.h>

struct ivshm_regs {
	uint32_t id;
	uint32_t max_peers;
	uint32_t int_control;
	uint32_t doorbell;
	uint32_t state;
};

static volatile uint32_t *state, *rw, *in, *out;
static uint32_t id, int_count;

static inline uint32_t mmio_read32(void *address)
{
        return *(volatile uint32_t *)address;
}

static inline void mmio_write32(void *address, uint32_t value)
{
        *(volatile uint32_t *)address = value;
}

static int uio_read_mem_size(char *devpath, int idx)
{
	char sysfs_path[64];
	char output[20] = "";
	int fd, ret, size;

	snprintf(sysfs_path, sizeof(sysfs_path),
		 "/sys/class/uio/%s/maps/map%d/size",
		 basename(devpath), idx);
	fd = open(sysfs_path, O_RDONLY);
	if (fd < 0)
		return fd;
	ret = read(fd, output, sizeof(output));
	if (ret < 0)
		return ret;
	sscanf(output, "0x%x", &size);
	return size;
}

static void print_shmem(void)
{
	printf("state[0] = %d\n", state[0]);
	printf("state[1] = %d\n", state[1]);
	printf("state[2] = %d\n", state[2]);
	printf("rw[0] = %d\n", rw[0]);
	printf("rw[1] = %d\n", rw[1]);
	printf("rw[2] = %d\n", rw[2]);
	printf("in@0x0000 = %d\n", in[0/4]);
	printf("in@0x2000 = %d\n", in[0x2000/4]);
	printf("in@0x4000 = %d\n", in[0x4000/4]);
}

int main(int argc, char *argv[])
{
	char sysfs_path[64];
	struct ivshm_regs *regs;
	uint32_t int_no, target = 0;
	struct signalfd_siginfo siginfo;
	struct pollfd fds[2];
	sigset_t sigset;
	char *path;
	int has_msix;
	int ret, size, offset, pgsize;

	pgsize = getpagesize();

	if (argc !=  3)
		error(1, EINVAL, "Usage: ivshmem-demo </dev/uioX> <peer_id>");
	path = strdup(argv[1]);
	target = atoi(argv[2]);

	fds[0].fd = open(path, O_RDWR);
	if (fds[0].fd < 0)
		error(1, errno, "open(%s)", path);
	fds[0].events = POLLIN;

	snprintf(sysfs_path, sizeof(sysfs_path),
		 "/sys/class/uio/%s/device/msi_irqs", basename(path));
	has_msix = access(sysfs_path, R_OK) == 0;

	offset = 0;
	size = uio_read_mem_size(path, 0);
	regs = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED,
		    fds[0].fd, offset);
	if (regs == MAP_FAILED)
		error(1, errno, "mmap(regs)");

	id = mmio_read32(&regs->id);
	printf("ID = %d\n", id);
	if (target >= regs->max_peers || target == id)
		error(1, EINVAL, "invalid peer number");

	offset += pgsize;
	size = uio_read_mem_size(path, 1);
	state = mmap(NULL, size, PROT_READ, MAP_SHARED, fds[0].fd, offset);
	if (state == MAP_FAILED)
		error(1, errno, "mmap(state)");

	offset += pgsize;
	size = uio_read_mem_size(path, 2);
	rw = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED,
		  fds[0].fd, offset);
	if (rw == MAP_FAILED)
		error(1, errno, "mmap(rw)");

	offset += pgsize;
	size = uio_read_mem_size(path, 3);
	in = mmap(NULL, size, PROT_READ, MAP_SHARED, fds[0].fd, offset);
	if (in == MAP_FAILED)
		error(1, errno, "mmap(in)");

	offset += pgsize;
	size = uio_read_mem_size(path, 4);
	out = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED,
		   fds[0].fd, offset);
	if (out == MAP_FAILED)
		error(1, errno, "mmap(out)");

	mmio_write32(&regs->state, id + 1);
	rw[id] = 0;
	out[0] = 0;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	fds[1].fd = signalfd(-1, &sigset, 0);
	if (fds[1].fd < 0)
		error(1, errno, "signalfd");
	fds[1].events = POLLIN;

	mmio_write32(&regs->int_control, 1);
	alarm(1);

	print_shmem();

	while (1) {
		ret = poll(fds, 2, -1);
		if (ret < 0)
			error(1, errno, "poll");

		if (fds[0].revents & POLLIN) {
			ret = read(fds[0].fd, &int_count, sizeof(int_count));
			if (ret != sizeof(int_count))
				error(1, errno, "read(uio)");

			rw[id] = int_count;
			out[0] = int_count * 10;
			printf("\nInterrupt #%d\n", int_count);
			print_shmem();

			mmio_write32(&regs->int_control, 1);
		}
		if (fds[1].revents & POLLIN) {
			ret = read(fds[1].fd, &siginfo, sizeof(siginfo));
			if (ret != sizeof(siginfo))
				error(1, errno, "read(sigfd)");

			int_no = has_msix ? (id + 1) : 0;
			printf("\nSending interrupt %d to peer %d\n",
			       int_no, target);
			mmio_write32(&regs->doorbell, int_no | (target << 16));

			alarm(1);
		}
	}
}
