#include "gemm.h"
extern "C" {
void gemm( TYPE m1[N], TYPE m2[N], TYPE prod[N] ){
#pragma HLS INTERFACE m_axi port = m1 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = m2 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = prod offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = m1 bundle = control
#pragma HLS INTERFACE s_axilite port = m2 bundle = control
#pragma HLS INTERFACE s_axilite port = prod bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
    int i, j, k;
    int k_col, i_col;
    TYPE mult;

    outer:for(i=0;i<row_size;i++) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=64
        middle:for(j=0;j<col_size;j++) {
             #pragma HLS LOOP_TRIPCOUNT min=1 max=64
            i_col = i * col_size;
            TYPE sum = 0;
            inner:for(k=0;k<row_size;k++) {
                #pragma HLS LOOP_TRIPCOUNT min=1 max=64
                k_col = k * col_size;
                mult = m1[i_col + k] * m2[k_col + j];
                sum += mult;
            }
            prod[i_col + j]  = sum;
        }
    }
}
}