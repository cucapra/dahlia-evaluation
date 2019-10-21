#include "gemm.h"
extern "C" {
  void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N], TYPE out[1], int x ){

    #pragma HLS INTERFACE m_axi port = m1 offset = slave bundle = gmem
    #pragma HLS INTERFACE m_axi port = m2 offset = slave bundle = gmem
    #pragma HLS INTERFACE m_axi port = prod offset = slave bundle = gmem
    #pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem

    #pragma HLS INTERFACE s_axilite port = m1 bundle = control
    #pragma HLS INTERFACE s_axilite port = m2 bundle = control
    #pragma HLS INTERFACE s_axilite port = prod bundle = control
    #pragma HLS INTERFACE s_axilite port = out bundle = control

    #pragma HLS INTERFACE s_axilite port = x bundle = control

    #pragma HLS INTERFACE s_axilite port = return bundle = control

    // Local partitioned buffers
    TYPE m1_local[N];
    #pragma HLS ARRAY_PARTITION variable = m1_local cyclic factor = ::PARTITION:: dim = 1
    #pragma HLS resource variable=m1_local core = RAM_1P_BRAM
    TYPE m2_local[N];
    #pragma HLS ARRAY_PARTITION variable = m2_local cyclic factor = ::PARTITION:: dim = 1
    #pragma HLS resource variable=m2_local core = RAM_1P_BRAM
    TYPE prod_local[N];
    #pragma HLS ARRAY_PARTITION variable = prod_local cyclic factor = ::PARTITION:: dim = 1
    #pragma HLS resource variable=prod_local core = RAM_1P_BRAM

    // Copy data to local buffers
    for(int l = 0; l < N; l++) {
      m1_local[l] = m1[l];
      m2_local[l] = m2[l];
    }

    // Compute GeMM
    TYPE sum;
    int store = 0;
    for(int i=0; i < ROW_SIZE; i++) {
      for(int j=0; j < COL_SIZE; j++) {
        #pragma HLS unroll factor = ::UNROLL2::
        sum = 0;
        for(int k=0; k<ROW_SIZE; k++) {
          #pragma HLS unroll factor = ::UNROLL1::
          sum += m1_local[i * COL_SIZE + k] * m2_local[k * COL_SIZE + j];
          store += ((i * COL_SIZE + k) ^ x) + ((k * COL_SIZE + j) ^ x);
        }
        prod_local[i * COL_SIZE + j]  = sum;
        store += ((i * COL_SIZE + j) ^ x);
      }
    }
    out[0] = store;

    // Write back to output buffer
    for(int l = 0; l < N; l++) {
      prod[l] = prod_local[l];
    }
  }
}
