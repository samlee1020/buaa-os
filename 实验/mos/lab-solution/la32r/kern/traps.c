#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);

void (*exception_handlers[64])(void) = {
    [0 ... 63] = handle_reserved, [0] = handle_int,   [1 ... 3] = handle_tlb,
#if !defined(LAB) || LAB >= 4
    [0x4] = handle_mod,		  [0xb] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	u_int badva;
	u_int exc_code = (tf->estat >> 16) & 0x3f;
	if (exc_code == 0xd) {
		asm("csrrd %0, 0x7" : "=r"(badva) :);
		printk("Unknown inst is 0x%08x\n", *((u_int *)badva));
	}
	panic("Unknown ExcCode %x", exc_code);
}
