// git.status = clean, build.date = Sat Nov 09 13:00:37 EST 2019, git.hash = 75aa1ab
#include <ap_int.h>
extern "C" {
  void gemm(double m1[64][64], double m2[64][64], double prod[64][64]) {
    #pragma HLS INTERFACE m_axi port=m1 offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=m1 bundle=control
    #pragma HLS INTERFACE m_axi port=m2 offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=m2 bundle=control
    #pragma HLS INTERFACE m_axi port=prod offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=prod bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    for(int i = 0; i < 64; i++) {
      for(int j = 0; j < 64; j++) {
        prod[i][j] = 0.0;
      }
    }
    //---
    for(int jj = 0; jj < 8; jj++) {
      for(int kk = 0; kk < 8; kk++) {
        for(int i = 0; i < 64; i++) {
          for(int j = 0; j < 8; j++) {
            for(int k = 0; k < 8; k++) {
              double temp_x = m1[i][((kk * 8) + k)];
              double mul = (temp_x * m2[((kk * 8) + k)][((jj * 8) + j)]);
              // combiner:
              prod[i][((jj * 8) + j)] += mul;
            }
          }
        }
      }
    }
  }
}