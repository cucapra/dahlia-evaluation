/*
Based on algorithm described here:
http://www.cs.berkeley.edu/~mhoemmen/matrix-seminar/slides/UCB_sparse_tutorial_1.pdf
*/

#include "func.h"

void spmv(TYPE nzval[N*L], int32_t cols[N*L], TYPE vec[N], TYPE out[N])
{
#pragma HLS INTERFACE s_axilite port=nzval
#pragma HLS INTERFACE s_axilite port=cols
#pragma HLS INTERFACE s_axilite port=vec
#pragma HLS INTERFACE s_axilite port=out

    int i, j;
    TYPE Si;

    ellpack_1 : for (i=0; i<N; i++) {
        TYPE sum = out[i];
        ellpack_2 : for (j=0; j<L; j++) {
                Si = nzval[j + i*L] * vec[cols[j + i*L]];
                sum += Si;
        }
        out[i] = sum;
    }
}
