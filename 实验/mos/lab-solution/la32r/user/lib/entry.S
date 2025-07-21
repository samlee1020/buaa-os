#include <asm/asm.h>
#include <mmu.h>

.data

.align 12

.globl fsipcbuf
fsipcbuf:
.space PAGE_SIZE

.text
LEAF(_start)
	ld.w    a0, sp, 0
	ld.w    a1, sp, 4
	bl      libmain
END(_start)
