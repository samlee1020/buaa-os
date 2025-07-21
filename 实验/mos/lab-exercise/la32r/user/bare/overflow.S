#include <asm/asm.h>
# This user-space program makes LA32R overflow exception.
#
# To test this manually:
# - Add the following one line in init/init.c:
#	ENV_CREATE(user_bare_overflow);
# - Then build the kernel with `make`

.text
.globl _start
_start:
	li.w    t0, 0x80000000
	addi.w  t1, t0, 0x3ff  # overflow
