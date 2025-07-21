#include <asm/asm.h>
#include <stackframe.h>

.macro BUILD_HANDLER exception handler
LEAF(handle_\exception)
	move    a0, sp
	bl      \handler
	b       ret_from_exception
END(handle_\exception)
.endm

.text;
.globl ret_from_exception;                                                                             \
.align 3;
.type ret_from_exception, @function;
ret_from_exception:
	RESTORE_SOME
	ld.w    sp, sp, TF_REG3 /* Deallocate stack */
	ertn

LEAF(handle_int)
	csrrd   t0, csr_estat
	andi    t1, t0, ESTAT_TI
	bne     t1, zero, timer_irq
timer_irq:
	li.w    t0, 1
	csrwr   t0, csr_ticlr
	li.w    a0, 0
	b       schedule
END(handle_int)

BUILD_HANDLER tlb do_tlb_invalid

#if !defined(LAB) || LAB >= 4
BUILD_HANDLER mod do_tlb_mod
BUILD_HANDLER sys do_syscall
#endif

BUILD_HANDLER reserved do_reserved
