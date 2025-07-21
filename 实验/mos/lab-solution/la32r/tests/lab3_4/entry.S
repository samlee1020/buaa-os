#include <asm/asm.h>

.text
.globl _start
_start:
	bl     main
	b      finish
.org 0x180
finish:
	b       finish
