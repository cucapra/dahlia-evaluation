// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = unsigned int;
#endif


#pragma SDS data copy(m1[0:row_size][0:col_size])
void gemm(
double m1[64][64], 
double m2[64][64], 
double prod[64][64]) {
  
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