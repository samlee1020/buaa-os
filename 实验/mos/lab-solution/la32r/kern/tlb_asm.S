#include <asm/asm.h>
#include <mmu.h>

LEAF(tlb_invalidate)
	/* Hint: Use invtlb 0x6 to invalidate page table entry. */
	/* Exercise 2.8: Your code here. */
	invtlb  0x6, a0, a1
	jr      ra
END(tlb_invalidate)

.section .text.tlb_miss_entry
.align 8
.global tlb_miss_entry
tlb_miss_entry:
	/* Step 1: Save general registers that we will use. */
	csrwr   t0, csr_save0
	csrwr   t1, csr_save1
	csrwr   t2, csr_save2
	/* Step 2: Get the corresponding page directory entry. */
	csrrd   t0, csr_pgd
	csrrd   t2, csr_badv
	srli.w  t1, t2, 20
	andi    t1, t1, 0xffc
	add.w   t1, t0, t1
	ld.w    t0, t1, 0
	/* Step 3: If the corresponding page table is not existent, */
	/* write an empty entry to TLB to trigger Page Invalid exception. */
	andi    t1, t0, PTE_V
	bne     t1, zero, 1f
	csrwr   zero, csr_tlbelo0
	csrwr   zero, csr_tlbelo1
	// Just use 'tlbfill' to write an empty entry to TLB
	/* Exercise 2.9: Your code here. (1/2) */
	tlbfill
	b       2f
1:
	/* Step 4: Get the corresponding page table entry, */
	/* and write to tlb. */
	li.w    t1, 0xfffff000
	and     t0, t0, t1
	srli.w  t1, t2, 10
	andi    t1, t1, 0xff8
	add.w   t1, t0, t1
	ld.w    t0, t1, 0
	ld.w    t2, t1, 4
	srli.w  t0, t0, 4
	srli.w  t2, t2, 4
	csrwr   t0, csr_tlbelo0
	csrwr   t2, csr_tlbelo1
	// Just use 'tlbfill' to write the corresponding page table entry to TLB
	/* Exercise 2.9: Your code here. (2/2) */
	tlbfill
2:
	/* Step 5: Restore general registers and return from excption. */
	csrrd   t2, csr_save2
	csrrd   t1, csr_save1
	csrrd   t0, csr_save0
	ertn
