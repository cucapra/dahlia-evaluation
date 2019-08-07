#include "ap_int.h"


#pragma SDS data copy(m1[0:64][0:64])
void gemm(double m1[64][64], double m2[64][64], double prod[64][64]) {
  #pragma HLS INTERFACE s_axilite port=m1
  #pragma HLS INTERFACE s_axilite port=m2
  #pragma HLS INTERFACE s_axilite port=prod
  
  for(int i = 0; i < 64; i++) {
    for(int j = 0; j < 64; j++) {
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