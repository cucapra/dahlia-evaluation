// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = unsigned int;
#endif


#pragma SDS data copy(m1[0:64][0:64])
#pragma SDS data zero_copy(prod[0:64][0:64])
void gemm(
double m1[64][64], 
double m2[64][64], 
double prod[64][64]) {
  
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