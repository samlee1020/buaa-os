#include <asm/asm.h>

LEAF(msyscall)
	// Just use 'syscall' instruction and return.

	/* Exercise 4.1: Your code here. */
	// 只需要调用syscall然后返回
	syscall
	jr	ra

END(msyscall)
