SECTIONS
{
 . = 0xffffc0200000;
 .header : { *(.header) }
 . = ALIGN(16);
 .text : {
  __text_start = .;
  *(.text)
 }
 . = ALIGN(16);
 .rodata : { *(.rodata) }
 . = ALIGN(16);
 .data : { *(.data) }
 . = ALIGN(8);
 .init_array : {
  __init_array_start = .;
  *(SORT(.init_array.*)) *(.init_array)
  __init_array_end = .;
 }
 .units : {
  __unit_array_start = .;
  *(.units);
  __unit_array_end = .;
 }
 . = ALIGN((1 << 12)); .bootstrap_page_tables : { bootstrap_pt_l0 = .; . = . + (1 << 12); bootstrap_pt_l1_hyp_uart = .; . = . + (1 << 12); bootstrap_pt_l1_trampoline = .; . = . + (1 << 12); bootstrap_pt_l2_hyp_uart = .; . = . + (1 << 12); } .trampoline : { __trampoline_start = .; *(.trampoline) }
 . = ALIGN((1 << 12));
 .console : { *(.console) }
 . = ALIGN((1 << 12));
 .bss : { *(.bss) }
 . = ALIGN((1 << 12));
 __page_pool = .;
 .eh_frame : { *(.eh_frame*) }
}
