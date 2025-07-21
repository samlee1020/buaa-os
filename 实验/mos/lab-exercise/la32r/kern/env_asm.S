#include <asm/asm.h>
#include <mmu.h>
#include <trap.h>
#include <kclock.h>

.text
LEAF(env_pop_tf)
	csrwr   a1, csr_asid
	move    sp, a0
	RESET_KCLOCK
	b       ret_from_exception
END(env_pop_tf)
