#include <asm/asm.h>
#include <cache.h>
#include <stdint.h>

void flush_cache(unsigned long start_addr, unsigned long size) {
	unsigned long ilsize = icache_line_size();
	unsigned long dlsize = dcache_line_size();

	/* aend will be miscalculated when size is zero, so we return here */
	if (size == 0) {
		return;
	}

	if (ilsize == dlsize) {
		/* flush I-cache & D-cache simultaneously */
		cache_loop(start_addr, start_addr + size, ilsize, HIT_WRITEBACK_INV_D,
			   HIT_INVALIDATE_I);
		goto ops_done;
	}

	/* flush D-cache */
	cache_loop(start_addr, start_addr + size, dlsize, HIT_WRITEBACK_INV_D);

	/* flush I-cache */
	cache_loop(start_addr, start_addr + size, ilsize, HIT_INVALIDATE_I);

ops_done:
	return;
}

void flush_dcache_range(unsigned long start_addr, unsigned long stop) {
	unsigned long lsize = dcache_line_size();
	// unsigned long slsize = scache_line_size();

	/* aend will be miscalculated when size is zero, so we return here */
	if (start_addr == stop) {
		return;
	}

	cache_loop(start_addr, stop, lsize, HIT_WRITEBACK_INV_D);
}

void invalidate_dcache_range(unsigned long start_addr, unsigned long stop) {
	unsigned long lsize = dcache_line_size();

	/* aend will be miscalculated when size is zero, so we return here */
	if (start_addr == stop) {
		return;
	}

	cache_loop(start_addr, stop, lsize, HIT_INVALIDATE_D);
}
