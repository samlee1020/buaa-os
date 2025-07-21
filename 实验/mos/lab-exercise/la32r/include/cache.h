#ifndef _CACHE_H_
#define _CACHE_H_

#define Cache_I 0x00
#define Cache_D 0x01
#define Cache_V 0x02
#define Cache_S 0x03

#define INDEX_INVALIDATE 0x08
#define INDEX_WRITEBACK_INV 0x08
#define HIT_INVALIDATE 0x10
#define HIT_WRITEBACK_INV 0x10

#define INDEX_INVALIDATE_I (Cache_I | INDEX_INVALIDATE)
#define INDEX_WRITEBACK_INV_D (Cache_D | INDEX_WRITEBACK_INV)
#define HIT_INVALIDATE_I (Cache_I | HIT_INVALIDATE)
#define HIT_INVALIDATE_D (Cache_D | HIT_INVALIDATE)
#define HIT_WRITEBACK_INV_D (Cache_D | HIT_WRITEBACK_INV)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static inline void la_cache(int op, const volatile void *addr) {
	if (op == 0) {
		asm volatile("cacop	%0, %1" : : "i"(0), "R"(*(unsigned char *)(addr)));
	} else if (op == 1) {
		asm volatile("cacop	%0, %1" : : "i"(1), "R"(*(unsigned char *)(addr)));
	} else if (op == 2) {
		asm volatile("cacop	%0, %1" : : "i"(2), "R"(*(unsigned char *)(addr)));
	} else if (op == 3) {
		asm volatile("cacop	%0, %1" : : "i"(3), "R"(*(unsigned char *)(addr)));
	} else if (op == 4) {
		asm volatile("cacop	%0, %1" : : "i"(4), "R"(*(unsigned char *)(addr)));
	} else if (op == 5) {
		asm volatile("cacop	%0, %1" : : "i"(5), "R"(*(unsigned char *)(addr)));
	} else if (op == 6) {
		asm volatile("cacop	%0, %1" : : "i"(6), "R"(*(unsigned char *)(addr)));
	} else if (op == 7) {
		asm volatile("cacop	%0, %1" : : "i"(7), "R"(*(unsigned char *)(addr)));
	} else if (op == 8) {
		asm volatile("cacop	%0, %1" : : "i"(8), "R"(*(unsigned char *)(addr)));
	} else if (op == 9) {
		asm volatile("cacop	%0, %1" : : "i"(9), "R"(*(unsigned char *)(addr)));
	} else if (op == 10) {
		asm volatile("cacop	%0, %1" : : "i"(10), "R"(*(unsigned char *)(addr)));
	} else if (op == 11) {
		asm volatile("cacop	%0, %1" : : "i"(11), "R"(*(unsigned char *)(addr)));
	} else if (op == 12) {
		asm volatile("cacop	%0, %1" : : "i"(12), "R"(*(unsigned char *)(addr)));
	} else if (op == 13) {
		asm volatile("cacop	%0, %1" : : "i"(13), "R"(*(unsigned char *)(addr)));
	} else if (op == 14) {
		asm volatile("cacop	%0, %1" : : "i"(14), "R"(*(unsigned char *)(addr)));
	} else if (op == 15) {
		asm volatile("cacop	%0, %1" : : "i"(15), "R"(*(unsigned char *)(addr)));
	}
}

#define cache_loop(start, end, lsize, ops...)                                                      \
	do {                                                                                       \
		const void *addr = (const void *)(start & ~(lsize - 1));                           \
		const void *aend = (const void *)((end - 1) & ~(lsize - 1));                       \
		const unsigned int cache_ops[] = {ops};                                            \
		unsigned int i;                                                                    \
                                                                                                   \
		if (!lsize)                                                                        \
			break;                                                                     \
                                                                                                   \
		for (; addr <= aend; addr += lsize) {                                              \
			for (i = 0; i < ARRAY_SIZE(cache_ops); i++)                                \
				la_cache(cache_ops[i], addr);                                      \
		}                                                                                  \
	} while (0)

static inline unsigned long icache_line_size(void) {
	return 4;
}

static inline unsigned long dcache_line_size(void) {
	return 4;
}

void flush_cache(unsigned long start_addr, unsigned long size);
void flush_dcache_range(unsigned long start_addr, unsigned long stop);
void invalidate_dcache_range(unsigned long start_addr, unsigned long stop);

#endif // !_CACHE_H_
