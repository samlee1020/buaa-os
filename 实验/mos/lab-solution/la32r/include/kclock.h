#ifndef _KCLOCK_H_
#define _KCLOCK_H_

#define TIMER_CONFIG (0xfffd) // WARNING: DO NOT MODIFY THIS LINE!

// clang-format off
.macro RESET_KCLOCK
	li.w    t0, TIMER_CONFIG
	/*
	 * Hint:
	 *	Use 'csrwr' to write TIMER_CONFIG into the csr_tcfg registers.
	 */
	/* Exercise 3.11: Your code here. */
	csrwr   t0, csr_tcfg
.endm
// clang-format on

#endif
