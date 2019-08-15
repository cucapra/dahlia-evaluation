#include "ap_int.h"


#pragma SDS data copy(m1[0:64][0:64])
void gemm(double m1[64][64], double m2[64][64], double prod[64][64]) {
  #pragma HLS INTERFACE s_axilite port=m1
  #pragma HLS INTERFACE s_axilite port=m2
  #pragma HLS INTERFACE s_axilite port=prod
  
  #pragma HLS ARRAY_PARTITION variable=m1 cyclic factor=4 dim=2
  
  #pragma HLS ARRAY_PARTITION variable=m2 cyclic factor=4 dim=1
  #pragma HLS ARRAY_PARTITION variable=m2 cyclic factor=4 dim=2
  
  #pragma HLS ARRAY_PARTITION variable=prod cyclic factor=4 dim=2
  
  
  
  for(int i = 0; i < 64; i++) {
    for(int j = 0; j < 64; j++) {
      #pragma HLS pipeline
      double sum = 0.0;
      
      for(int k = 0; k < 64; k++) {
        double mult = (m1[i][k] * m2[k][j]);
        
        // combiner:
        sum += mult;
      }
      prod[i][j] = sum;
    }
  }
}