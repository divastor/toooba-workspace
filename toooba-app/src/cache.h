#ifndef CACHE_H
#define CACHE_H

// #include "../src/printf.c"

// cache values
// -> Values taken from paper on SweRV par. C and 
// https://en.wikipedia.org/wiki/CPU_cache#Cache_entry_structure
#define L1_SETS 64 // (L1_SZ_BYTES/L1_BLOCK_SZ_BYTES)/L1_WAYS = 32KiB/64/8 = 512/8 = 64
#define L1_SET_BITS 6 // log2(64) = 6 -- note: this is log2Ceil(L1_SETS)
#define L1_WAYS 8 //from src_Core/RISCY_OOO/procs/RV64G_OOO/ProcConfig.bsv -- note: this looks like there are 8 ways
#define L1_BLOCK_SZ_BYTES 64 // from src_Core/RISCY_OOO/procs/lib/CacheUtils.bsv
#define L1_BLOCK_BITS 6 // log2(64) = 6 -- note: this is log2Ceil(L1_BLOCK_SZ_BYTES)
#define L1_SZ_BYTES (L1_SETS*L1_WAYS*L1_BLOCK_SZ_BYTES) // = 32KiB
#define FULL_MASK 0xFFFFFFFFFFFFFFFF
#define OFF_MASK (~(FULL_MASK << L1_BLOCK_BITS)) // 0x3F or 0b111111
#define TAG_MASK (FULL_MASK << (L1_SET_BITS + L1_BLOCK_BITS)) // 0xF...FF000 or 0b1...1000000000000
#define SET_MASK (~(TAG_MASK | OFF_MASK)) // 0xFC0 or 0b111111000000

/* ----------------------------------
 * |                  Cache address |
 * ----------------------------------
 * |       tag |      idx |  offset |
 * ----------------------------------
 * | 63 <-> 12 | 11 <-> 6 | 5 <-> 0 |
 * ----------------------------------
 */

// setup array size of cache to "put" in the cache on $ flush
// guarantees contiguous set of addrs that is at least the sz of cache
// 5 so that you can hit more
uint8_t dummyMem[5 * L1_SZ_BYTES];
uint32_t current;

/**
 * Flush the cache of the address given since RV64 does not have a
 * clflush type of instruction. Clears any set that has the same idx bits
 * as the address input range.
 *
 * Note: This does not work if you are trying to flush dummyMem out of the
 * cache.
 *
 * @param addr starting address to clear the cache
 * @param sz size of the data to remove in bytes
 */
void flushCache(uint64_t addr, uint64_t sz) {
    // printf("--- Flushing cache: addr:0x%x, sz = %d\n", addr, sz);
    uint8_t dummy = 0;
    uint64_t alignedMem = (((uint64_t)&dummyMem) + L1_SZ_BYTES) & TAG_MASK;

    uint64_t numSetsClear = sz >> L1_BLOCK_BITS;
    if ((sz & OFF_MASK) != 0){
        numSetsClear += 1;
    }
    if (numSetsClear > L1_SETS){
        // flush entire cache with no rollover (makes the function finish faster) 
        numSetsClear = L1_SETS;
    }
    // printf("numSetsClear = %d\n", numSetsClear);

    for (uint64_t i = 0; i < numSetsClear; ++i) {
        // offset to move across the sets that you want to flush
        uint64_t setOffset = (((addr & SET_MASK) >> L1_BLOCK_BITS) + i) << L1_BLOCK_BITS;
        //printf("setOffset(0x%x)\n", setOffset);
        for(int j = 0; j < 4+L1_WAYS; j++) {
            // each new tag will take place in a different way of the set
            uint64_t tagOffset = j << (L1_BLOCK_BITS + L1_SET_BITS);
            dummy = *((uint8_t*)(alignedMem + tagOffset + setOffset));
        }
    }
    // printf("--- ---\n\n");
}

#endif
