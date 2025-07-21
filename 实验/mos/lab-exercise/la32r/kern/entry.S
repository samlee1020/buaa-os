#include <asm/asm.h>
#include <stackframe.h>

.section .text.exc_gen_entry
.align 8
.global exc_gen_entry
exc_gen_entry:
	SAVE_ALL
/* Exercise 3.9: Your code here. */
