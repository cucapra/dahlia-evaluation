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
    #pragma HLS ARRAY_PARTITION variable = m1_local cyclic factor = ::PARTITION:: dim = 2
    #pragma HLS resource variable=m1_local core = RAM_1P_BRAM
    TYPE m2_local[ROW_SIZE][COL_SIZE];
    #pragma HLS ARRAY_PARTITION variable = m2_local cyclic factor = ::PARTITION:: dim = 1
    #pragma HLS resource variable=m2_local core = RAM_1P_BRAM
    TYPE prod_local[ROW_SIZE][COL_SIZE];
    #pragma HLS ARRAY_PARTITION variable = prod_local cyclic factor = ::PARTITION:: dim = 1
    #pragma HLS resource variable=prod_local core = RAM_1P_BRAM

    // Copy data to local buffers
    for (int r = 0; r < ROW_SIZE; r++) {
      for (int c = 0; c < COL_SIZE; c++) {
        m1_local[r][c] = m1[r * COL_SIZE + c];
        m2_local[r][c] = m2[r * COL_SIZE + c];
      }
    }

    // Compute GeMM
    TYPE sum;
    for(int i=0; i < ROW_SIZE; i++) {
      for(int j=0; j < COL_SIZE; j++) {
        sum = 0;
        for(int k=0; k<ROW_SIZE; k++) {
          #pragma HLS unroll factor = 8
          sum += m1_local[i][k] * m2_local[k][j];
        }
        prod_local[i][j]  = sum;
      }
    }

    // Write back to output buffer
    for (int r = 0; r < ROW_SIZE; r++) {
      for (int c = 0; c < COL_SIZE; c++) {
        prod[r * COL_SIZE + c] = prod_local[r][c];
      }
    }
  }
}
