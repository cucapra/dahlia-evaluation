#include "func.h"

void gemm( TYPE m1[N], TYPE m2[N], TYPE prod[N] ){
#pragma HLS INTERFACE s_axilite port=m1
#pragma HLS INTERFACE s_axilite port=m2
#pragma HLS INTERFACE s_axilite port=prod

    int i, j, k;
    int k_col, i_col;
    TYPE mult;

    outer:for(i=0;i<row_size;i++) {
        middle:for(j=0;j<col_size;j++) {
            i_col = i * col_size;
            TYPE sum = 0;
            inner:for(k=0;k<row_size;k++) {
                k_col = k * col_size;
                mult = m1[i_col + k] * m2[k_col + j];
                sum += mult;
            }
            prod[i_col + j]  = sum;
        }
    }
}
