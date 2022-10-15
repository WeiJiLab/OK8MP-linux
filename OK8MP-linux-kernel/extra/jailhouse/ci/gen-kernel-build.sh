#!/bin/bash
#
# Jailhouse, a Linux-based partitioning hypervisor
#
# Copyright (c) Siemens AG, 2014-2016
#
# Authors:
#  Jan Kiszka <jan.kiszka@siemens.com>
#
# This work is licensed under the terms of the GNU GPL, version 2.  See
# the COPYING file in the top-level directory.
#

BASEDIR=`cd \`dirname $0\`; pwd`

if test -z $KERNEL; then
	KERNEL=https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.13.tar.xz
fi
if test -z $PARALLEL_BUILD; then
	PARALLEL_BUILD=-j16
fi
if test -z $OUTDIR; then
	OUTDIR=$BASEDIR/out
fi

prepare_out()
{
	rm -rf $OUTDIR
	mkdir -p $OUTDIR
	cd $OUTDIR
}

prepare_kernel()
{
	ARCHIVE_FILE=`basename $KERNEL`
	if ! test -f $BASEDIR/$ARCHIVE_FILE; then
		wget $KERNEL -O $BASEDIR/$ARCHIVE_FILE
	fi
	tar xJf $BASEDIR/$ARCHIVE_FILE
	ln -s linux-* linux
	cd linux
	patch -p1 << EOF
diff --git a/arch/arm/kernel/armksyms.c b/arch/arm/kernel/armksyms.c
index 8e8d20cdbce7..a13adac97508 100644
--- a/arch/arm/kernel/armksyms.c
+++ b/arch/arm/kernel/armksyms.c
@@ -20,6 +20,7 @@
 
 #include <asm/checksum.h>
 #include <asm/ftrace.h>
+#include <asm/virt.h>
 
 /*
  * libgcc functions - functions that are used internally by the
@@ -181,3 +182,7 @@ EXPORT_SYMBOL(__pv_offset);
 EXPORT_SYMBOL(__arm_smccc_smc);
 EXPORT_SYMBOL(__arm_smccc_hvc);
 #endif
+
+#ifdef CONFIG_ARM_VIRT_EXT
+EXPORT_SYMBOL_GPL(__boot_cpu_mode);
+#endif
diff --git a/arch/arm/kernel/hyp-stub.S b/arch/arm/kernel/hyp-stub.S
index ec7e7377d423..83e65f01ca7f 100644
--- a/arch/arm/kernel/hyp-stub.S
+++ b/arch/arm/kernel/hyp-stub.S
@@ -19,6 +19,7 @@
 #include <linux/init.h>
 #include <linux/irqchip/arm-gic-v3.h>
 #include <linux/linkage.h>
+#include <asm-generic/export.h>
 #include <asm/assembler.h>
 #include <asm/virt.h>
 
@@ -281,4 +282,5 @@ __hyp_stub_trap:	W(b)	__hyp_stub_do_trap
 __hyp_stub_irq:		W(b)	.
 __hyp_stub_fiq:		W(b)	.
 ENDPROC(__hyp_stub_vectors)
+EXPORT_SYMBOL_GPL(__hyp_stub_vectors)
 
diff --git a/arch/arm64/kernel/hyp-stub.S b/arch/arm64/kernel/hyp-stub.S
index e1261fbaa374..061b32ef7c74 100644
--- a/arch/arm64/kernel/hyp-stub.S
+++ b/arch/arm64/kernel/hyp-stub.S
@@ -21,6 +21,7 @@
 #include <linux/linkage.h>
 #include <linux/irqchip/arm-gic-v3.h>
 
+#include <asm-generic/export.h>
 #include <asm/assembler.h>
 #include <asm/kvm_arm.h>
 #include <asm/kvm_asm.h>
@@ -51,6 +52,7 @@ ENTRY(__hyp_stub_vectors)
 	ventry	el1_fiq_invalid			// FIQ 32-bit EL1
 	ventry	el1_error_invalid		// Error 32-bit EL1
 ENDPROC(__hyp_stub_vectors)
+EXPORT_SYMBOL_GPL(__hyp_stub_vectors)
 
 	.align 11
 
diff --git a/lib/ioremap.c b/lib/ioremap.c
index 4bb30206b942..5629eeaba5ae 100644
--- a/lib/ioremap.c
+++ b/lib/ioremap.c
@@ -177,3 +177,4 @@ int ioremap_page_range(unsigned long addr,
 
 	return err;
 }
+EXPORT_SYMBOL_GPL(ioremap_page_range);
EOF
}

build_kernel()
{
	mkdir build-$1
	cp $BASEDIR/kernel-config-$1 build-$1/.config
	make O=build-$1 vmlinux $PARALLEL_BUILD ARCH=$2 CROSS_COMPILE=$3
	# clean up some unneeded build output
	find build-$1 \( -name "*.o" -o -name "*.cmd" -o -name ".tmp_*" \) -exec rm -rf {} \;
}

package_out()
{
	cd $OUTDIR
	tar cJf kernel-build.tar.xz linux-* linux
}

prepare_out
prepare_kernel
build_kernel x86 x86_64
build_kernel banana-pi arm arm-linux-gnueabihf-
build_kernel amd-seattle arm64 aarch64-linux-gnu-
package_out
