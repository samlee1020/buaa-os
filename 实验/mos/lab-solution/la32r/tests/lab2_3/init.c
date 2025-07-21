#include <pmap.h>
#include <trap.h>

extern void do_tlb_invalid(struct Trapframe *);

void tlb_refill_check(void) {
	struct Trapframe fake_tf;
	struct Page *pp, *pp0, *pp1, *pp2, *pp3, *pp4;

	// should be able to allocate a page for directory
	assert(page_alloc(&pp) == 0);
	Pde *boot_pgdir = (Pde *)page2kva(pp);
	cur_pgdir = boot_pgdir;
	asm volatile("csrwr %0, 0x19\n"
		     "csrwr %0, 0x1a\n"
		     :
		     : "r"(PADDR((u_int)cur_pgdir))
		     : "memory");

	// should be able to allocate three pages
	pp0 = pp1 = pp2 = pp3 = pp4 = 0;
	assert(page_alloc(&pp0) == 0);
	assert(page_alloc(&pp1) == 0);
	assert(page_alloc(&pp2) == 0);
	assert(page_alloc(&pp3) == 0);
	assert(page_alloc(&pp4) == 0);

	// temporarily steal the rest of the free pages
	// now this page_free list must be empty!!!!
	LIST_INIT(&page_free_list);

	// free pp0 and try again: pp0 should be used for page table
	page_free(pp0);
	// check if PTE != PP
	assert(page_insert(boot_pgdir, 0, pp1, 0x0, 0) == 0);
	// should be able to map pp2 at PAGE_SIZE because pp0 is already allocated for page table
	assert(page_insert(boot_pgdir, 0, pp2, PAGE_SIZE, 0) == 0);

	printk("tlb_refill_check() begin!\n");

	Pte *walk_pte;
	assert(page_lookup(boot_pgdir, PAGE_SIZE, &walk_pte) != NULL);
	assert(page2pa(pp2) == va2pa(boot_pgdir, PAGE_SIZE));

	printk("test point 1 ok\n");

	page_free(pp4);
	page_free(pp3);

	assert(page_lookup(boot_pgdir, 0x00400000, &walk_pte) == NULL);
	fake_tf.badv = 0x00400000;
	do_tlb_invalid(&fake_tf);
	assert((pp = page_lookup(boot_pgdir, 0x00400000, &walk_pte)) != NULL);
	assert(va2pa(boot_pgdir, 0x00400000) == page2pa(pp3));

	printk("test point 2 ok\n");

	u_long badv;
	badv = 0x00400000;
	*(u_long *)badv = 0x12345678;

	assert(*(u_long *)badv == 0x12345678);

	printk("tlb_refill_check() succeed!\n");
}
void la32r_init() {
	printk("init.c:\tla32r_init() is called\n");

	la32r_detect_memory();
	la32r_vm_init();
	page_init();

	tlb_refill_check();
	halt();
}
