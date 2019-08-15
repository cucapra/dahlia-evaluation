/*
Based on algorithm described here:
http://www.cs.berkeley.edu/~mhoemmen/matrix-seminar/slides/UCB_sparse_tutorial_1.pdf
*/

#include "spmv.h"

void spmv(TYPE val[NNZ], int32_t cols[NNZ], int32_t rowDelimiters[N+1], TYPE vec[N], TYPE out[N]){
#pragma HLS INTERFACE s_axilite port=val
#pragma HLS INTERFACE s_axilite port=cols
#pragma HLS INTERFACE s_axilite port=rowDelimiters
#pragma HLS INTERFACE s_axilite port=vec
#pragma HLS INTERFACE s_axilite port=out

    int i, j;
    TYPE sum, Si;
  int max =0; int min = 0; int number = 0; int total = 0;

    spmv_1 : for(i = 0; i < N; i++){
        sum = 0; Si = 0;
        int tmp_begin = rowDelimiters[i];
        int tmp_end = rowDelimiters[i+1];
        int count = 0;
        number += 1;
        spmv_2 : for (j = tmp_begin; j < tmp_end; j++){
          count += 1;
            Si = val[j] * vec[cols[j]];
            sum = sum + Si;
        }
        total += count;
        if (count >= max)
            max = count;
        if (count <= min)
            min = count;
        out[i] = sum;
    }
        printf("max: %d, min: %d, total: %d, number: %d, avg: %d \n",max, min, total, number, total/number);
}


