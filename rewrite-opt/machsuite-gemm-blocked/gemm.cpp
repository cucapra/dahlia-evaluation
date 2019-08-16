#include "ap_int.h"


#pragma SDS data copy(m1[0:64][0:64])
#pragma SDS data zero_copy(prod[0:64][0:64])
void gemm(double m1[64][64], double m2[64][64], double prod[64][64]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=m1
  #pragma HLS INTERFACE s_axilite port=m2
  #pragma HLS INTERFACE s_axilite port=prod
  
  #pragma HLS ARRAY_PARTITION variable=m1 cyclic factor=8 dim=2
  
  #pragma HLS ARRAY_PARTITION variable=m2 cyclic factor=8 dim=1
  #pragma HLS ARRAY_PARTITION variable=m2 cyclic factor=4 dim=2
  
  #pragma HLS ARRAY_PARTITION variable=prod cyclic factor=4 dim=2
  for(int jj = 0; jj < 16; jj++) {
    
    for(int kk = 0; kk < 8; kk++) {
      
      
      for(int i = 0; i < 64; i++) {
        for(int j = 0; j < 4; j++) {
          #pragma HLS UNROLL factor=4 skip_exit_check
          
          for(int k = 0; k < 8; k++) {
            #pragma HLS UNROLL factor=8 skip_exit_check
            
            double temp_x = m1[i][((8 * kk) + k)];
            
            double mul = (temp_x * m2[((8 * kk) + k)][((4 * jj) + j)]);
            
            // combiner:
            prod[i][((4 * jj) + j)] += mul;
          }
        }
      }
    }
  }
}