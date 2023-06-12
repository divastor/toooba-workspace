#ifndef PWNTOOLS_C
#define PWNTOOLS_C
/**
 * reads in inArray array (and corresponding size) and outIdxArrays top two idx's (and their
 * corresponding values) in the inArray array that has the highest values.
 *
 * @input inArray array of values to find the top two maxs
 * @input inArraySize size of the inArray array in entries
 * @inout outIdxArray array holding the idxs of the top two values
 *        ([0] idx has the larger value in inArray array)
 * @inout outValArray array holding the top two values ([0] has the larger value)
 */
void topTwoIdx(uint64_t* inArray, uint64_t inArraySize, uint8_t* outIdxArray, uint64_t* outValArray){
    outValArray[0] = 0;
    outValArray[1] = 0;

    for (uint64_t i = 0; i < inArraySize; ++i){
        if (inArray[i] > outValArray[0]){
            outValArray[1] = outValArray[0];
            outValArray[0] = inArray[i];
            outIdxArray[1] = outIdxArray[0];
            outIdxArray[0] = i;
        }
        else if (inArray[i] > outValArray[1]){
            outValArray[1] = inArray[i];
            outIdxArray[1] = i;
        }
    }
}
void printIndexStats(uint64_t* inArray, uint64_t inArraySize) {
    uint64_t outValArray[inArraySize];

    char bar[ATTACK_SAME_ROUNDS+1];
    for (uint64_t j = 0; j < ATTACK_SAME_ROUNDS+1; ++j)
        bar[j]=0;

    for (uint64_t i = 0; i < inArraySize; ++i){
        if (inArray[i]==0)
            continue;

        uint64_t j = 0;
        for (; j < inArray[i]; ++j)
            bar[j] = '|';
        for (; j < ATTACK_SAME_ROUNDS; ++j)
            bar[j] = ' ';

        bar[ATTACK_SAME_ROUNDS] = 0;
        char* tw;
        if (i>=100){
            tw = "";
        } else if (i>=10) {
            tw = " ";
        } else {
            tw = "  ";
        }
        printf("%d%s: %s (%d)\n", i, tw, bar, inArray[i]);
    }
}

#endif