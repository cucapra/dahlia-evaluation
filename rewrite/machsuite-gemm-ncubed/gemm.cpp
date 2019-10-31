#include <ap_int.h>



extern "C" {
  void gemm(double m1[64][64], double m2[64][64], double prod[64][64]) {
    #pragma HLS INLINE
    #pragma HLS INTERFACE m_axi port=m1 offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=m1 bundle=control
    #pragma HLS INTERFACE m_axi port=m2 offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=m2 bundle=control
    #pragma HLS INTERFACE m_axi port=prod offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=prod bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
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
}