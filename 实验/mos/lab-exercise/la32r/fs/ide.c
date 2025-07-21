/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <lib.h>
#include <mmu.h>

/* Overview:
 *  read data from IDE disk. First issue a read request through
 *  disk register and then copy data from disk buffer
 *  (512 bytes, a sector) to destination array.
 *
 * Parameters:
 *  diskno: disk number.
 *  secno: start sector number.
 *  dst: destination for data read from IDE disk.
 *  nsecs: the number of sectors to read.
 *
 * Post-Condition:
 *  Panic if any error occurs. (you may want to use 'panic_on')
 *
 * Hint: Use syscalls to access device registers and buffers.
 * Hint: Use the physical address and offsets defined in 'include/malta.h'.
 */
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int offset = 0;
	nsecs += secno;

	while (secno < nsecs) {
		/* Exercise 5.3: Your code here. (1/2) */

		offset += SECT_SIZE;
		secno += 1;
	}
}

/* Overview:
 *  write data to IDE disk.
 *
 * Parameters:
 *  diskno: disk number.
 *  secno: start sector number.
 *  src: the source data to write into IDE disk.
 *  nsecs: the number of sectors to write.
 *
 * Post-Condition:
 *  Panic if any error occurs.
 *
 * Hint: Use syscalls to access device registers and buffers.
 * Hint: Use the physical address and offsets defined in 'include/malta.h'.
 */
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int offset = 0;
	nsecs += secno;

	while (secno < nsecs) {
		/* Exercise 5.3: Your code here. (2/2) */

		offset += SECT_SIZE;
		secno += 1;
	}
}
