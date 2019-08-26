/*
Based on algorithm described here:
http://www.cs.berkeley.edu/~mhoemmen/matrix-seminar/slides/UCB_sparse_tutorial_1.pdf
*/

#include "spmv.h"

extern "C"{
void spmv(TYPE nzval[N * L], int32_t cols[N * L], TYPE vec[N], TYPE out[N])
    {
#pragma HLS INTERFACE m_axi port = nzval offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = cols offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = vec offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port=nzval bundle = control
#pragma HLS INTERFACE s_axilite port=cols bundle = control
#pragma HLS INTERFACE s_axilite port=vec bundle = control
#pragma HLS INTERFACE s_axilite port=out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

        int i, j;
        TYPE Si;

    ellpack_1:
        for (i = 0; i < N; i++)
        {
            TYPE sum = out[i];
        ellpack_2:
            for (j = 0; j < L; j++)
            {
                Si = nzval[j + i * L] * vec[cols[j + i * L]];
                sum += Si;
            }
            out[i] = sum;
        }
    }
}