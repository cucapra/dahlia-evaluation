// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = unsigned int;
#endif


#pragma SDS data copy(nzval[0:494][0:10])
#pragma SDS data zero_copy(out[0:494])
void spmv(
double nzval[494][10], 
ap_int<32> cols[494][10], 
double vec[494], 
double out[494]) {
  
  for(int i = 0; i < 494; i++) {
    for(int j = 0; j < 10; j++) {
      double si = (nzval[i][j] * vec[cols[i][j]]);
      // combiner:
      out[i] += si;
    }
  }
}
