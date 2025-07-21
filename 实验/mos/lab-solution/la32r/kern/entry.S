#include <asm/asm.h>
#include <stackframe.h>

.section .text.exc_gen_entry
.align 8
.global exc_gen_entry
exc_gen_entry:
	SAVE_ALL
/* Exercise 3.9: Your code here. */
	csrrd   t0, csr_estat
	srli.w  t0, t0, 14
	andi    t0, t0, 0xfc
	la      t1, exception_handlers
	add.w   t1, t0, t1
	ld.w    t0, t1, 0
	jr      t0
