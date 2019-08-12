/*
Based on algorithm described here:
http://www.cs.berkeley.edu/~mhoemmen/matrix-seminar/slides/UCB_sparse_tutorial_1.pdf
*/

#include "spmv.h"

extern"C" {
void spmv(TYPE val[NNZ], int32_t cols[NNZ], int32_t rowDelimiters[N+1], TYPE vec[N], TYPE out[N]){
#pragma HLS INTERFACE m_axi port = val offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = cols offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = rowDelimiters offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = vec offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port=val bundle = control
#pragma HLS INTERFACE s_axilite port=cols bundle = control
#pragma HLS INTERFACE s_axilite port=rowDelimiters bundle = control
#pragma HLS INTERFACE s_axilite port=vec bundle = control
#pragma HLS INTERFACE s_axilite port=out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control


    int i, j;
    TYPE sum, Si;

    spmv_1 : for(i = 0; i < N; i++){
        #pragma HLS LOOP_TRIPCOUNT min=1 max=494
        sum = 0; Si = 0;
        int tmp_begin = rowDelimiters[i];
        int tmp_end = rowDelimiters[i+1];
        spmv_2 : for (j = tmp_begin; j < tmp_end; j++){
             #pragma HLS LOOP_TRIPCOUNT min=1 max=494
            Si = val[j] * vec[cols[j]];
            sum = sum + Si;
        }
        out[i] = sum;
    }
}
}

