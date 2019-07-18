// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = unsigned int;
#endif



void gemm(
double m1[4096], 
double m2[4096], 
double prod[4096]) {
  
  ap_int<32> i_col = 0;
  ap_int<32> k_col = 0;
  double mult = 0.0;
  for(int i = 0; i < 64; i++) {
    for(int j = 0; j < 64; j++) {
      i_col = (i * 64);
      double sum = 0.0;
      for(int k = 0; k < 64; k++) {
        k_col = (k * 64);
        mult = (m1[(i_col + k)] * m2[(k_col + j)]);
        sum = (sum + mult);
      }
      prod[(i_col + j)] = sum;
    }
  }
}