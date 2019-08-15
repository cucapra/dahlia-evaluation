/*
Implementation based on http://www-igm.univ-mlv.fr/~lecroq/string/node8.html
*/

#include "kmp.h"

void CPF(char pattern[PATTERN_SIZE], int32_t kmpNext[PATTERN_SIZE]) {
    int32_t k, q;
    k = 0;
    kmpNext[0] = 0;

  int max =0; int min = 0; int number = 0; int total = 0;
    c1 : for(q = 1; q < PATTERN_SIZE; q++){
        int count = 0;
        number += 1;
        c2 : while(k > 0 && pattern[k] != pattern[q]){
          count += 1;
            k = kmpNext[q];
        }
        total += count;
        if (count >= max)
            max = count;
        if (count <= min)
            min = count;
        if(pattern[k] == pattern[q]){
            k++;
        }
        kmpNext[q] = k;
    }
        printf("CPF max: %d, min: %d, total: %d, number: %d, avg: %d \n",max, min, total, number, total/number);
}


void kmp(char pattern[PATTERN_SIZE], char input[STRING_SIZE], int32_t kmpNext[PATTERN_SIZE], int32_t n_matches[1]) {
#pragma HLS INTERFACE s_axilite port=pattern
#pragma HLS INTERFACE s_axilite port=input
#pragma HLS INTERFACE s_axilite port=kmpNext
#pragma HLS INTERFACE s_axilite port=n_matches

    int32_t i, q;
    n_matches[0] = 0;

    CPF(pattern, kmpNext);

  int max =0; int min = 0; int number = 0; int total = 0;
    q = 0;
    k1 : for(i = 0; i < STRING_SIZE; i++){
        int count = 0;
        number += 1;
        k2 : while (q > 0 && pattern[q] != input[i]){
          count += 1;
            q = kmpNext[q];
        }
        total += count;
        if (count >= max)
            max = count;
        if (count <= min)
            min = count;
        if (pattern[q] == input[i]){
            q++;
        }
        if (q >= PATTERN_SIZE){
            n_matches[0]++;
            q = kmpNext[q - 1];
        }
    }
        printf("kmp max: %d, min: %d, total: %d, number: %d, avg: %d \n",max, min, total, number, total/number);
}
