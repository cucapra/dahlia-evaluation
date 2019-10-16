/*
Implementation based on algorithm described in:
The cache performance and optimizations of blocked algorithms
M. D. Lam, E. E. Rothberg, and M. E. Wolf
ASPLOS 1991
*/

#include "gemm.h"

extern "C" {
  void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]){
  #pragma HLS INTERFACE m_axi port = m1 offset = slave bundle = gmem
  #pragma HLS INTERFACE m_axi port = m2 offset = slave bundle = gmem
  #pragma HLS INTERFACE m_axi port = prod offset = slave bundle = gmem
  #pragma HLS INTERFACE s_axilite port = m1 bundle = control
  #pragma HLS INTERFACE s_axilite port = m2 bundle = control
  #pragma HLS INTERFACE s_axilite port = prod bundle = control
  #pragma HLS INTERFACE s_axilite port = return bundle = control

      int i, k, j, jj, kk;
      int i_row, k_row;
      TYPE temp_x, mul;

      for(int l = 0; l < N; l++) {
        prod[l] = 0;
      }

      loopjj:for (jj = 0; jj < row_size; jj += block_size){
           #pragma HLS LOOP_TRIPCOUNT min=1 max=64
          loopkk:for (kk = 0; kk < row_size; kk += block_size){
               #pragma HLS LOOP_TRIPCOUNT min=1 max=64
              loopi:for ( i = 0; i < row_size; ++i){
                   #pragma HLS LOOP_TRIPCOUNT min=1 max=64
                  loopk:for (k = 0; k < block_size; ++k){
                       #pragma HLS LOOP_TRIPCOUNT min=1 max=64
                      i_row = i * row_size;
                      k_row = (k  + kk) * row_size;
                      temp_x = m1[i_row + k + kk];
                      loopj:for (j = 0; j < block_size; ++j){
                           #pragma HLS LOOP_TRIPCOUNT min=1 max=64
                          mul = temp_x * m2[k_row + j + jj];
                          prod[i_row + j + jj] += mul;
                      }
                  }
              }
          }
      }
  }
}
