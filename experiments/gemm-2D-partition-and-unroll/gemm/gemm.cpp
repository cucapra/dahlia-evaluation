#include "gemm.h"
extern "C" {
  void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N] ){

    #pragma HLS INTERFACE m_axi port = m1 offset = slave bundle = gmem
    #pragma HLS INTERFACE m_axi port = m2 offset = slave bundle = gmem
    #pragma HLS INTERFACE m_axi port = prod offset = slave bundle = gmem
    #pragma HLS INTERFACE s_axilite port = m1 bundle = control
    #pragma HLS INTERFACE s_axilite port = m2 bundle = control
    #pragma HLS INTERFACE s_axilite port = prod bundle = control
    #pragma HLS INTERFACE s_axilite port = return bundle = control

    // Local partitioned buffers
    TYPE m1_local[ROW_SIZE][COL_SIZE];
    #pragma HLS ARRAY_PARTITION variable = m1_local cyclic factor = ::PARTITION1:: dim = 1
    #pragma HLS ARRAY_PARTITION variable = m1_local cyclic factor = ::PARTITION2:: dim = 2
    TYPE m2_local[ROW_SIZE][COL_SIZE];
    #pragma HLS ARRAY_PARTITION variable = m2_local cyclic factor = ::PARTITION1:: dim = 1
    #pragma HLS ARRAY_PARTITION variable = m1_local cyclic factor = ::PARTITION2:: dim = 2
    TYPE prod_local[ROW_SIZE][COL_SIZE];
    #pragma HLS ARRAY_PARTITION variable = prod_local cyclic factor = ::PARTITION1:: dim = 1
    #pragma HLS ARRAY_PARTITION variable = m1_local cyclic factor = ::PARTITION2:: dim = 2

    // Copy data to local buffers
    for(int l = 0; l < ROW_SIZE; l++) {
      for(int m = 0; m < COL_SIZE; m++) {
        m1_local[l][m] = m1[l * COL_SIZE + m];
        m2_local[l][m] = m2[l * COL_SIZE + m];
      }
    }

    // Compute GeMM
    TYPE sum;
    for(int i=0; i < ROW_SIZE; i++) {
      for(int j=0; j < COL_SIZE; j++) {
          #pragma HLS unroll factor = ::UNROLL1::
        sum = 0;
        for(int k=0; k<ROW_SIZE; k++) {
          #pragma HLS unroll factor = ::UNROLL2::
          sum += m1_local[i][k] * m2_local[k][j];
        }
        prod_local[i][j] = sum;
      }
    }

    // Write back to output buffer
    for(int l = 0; l < ROW_SIZE; l++) {
      for(int m = 0; m < COL_SIZE; m++) {
        prod[l * COL_SIZE + m] = prod_local[l][m];
      }
    }
  }
}
