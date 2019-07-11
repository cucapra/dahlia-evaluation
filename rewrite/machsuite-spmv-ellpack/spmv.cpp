#ifdef __SDSCC__
#include "ap_int.h"
#else
template < int N >
using ap_int = int;
#endif

#pragma SDS data copy(nzval[0:N*L])
#pragma SDS data zero_copy(out[0:N])
void spmv(double nzval[4940], ap_int<32> cols[4940], double vec[494], double out[494]) {

  double si = 0.0;
  for(int i = 0; i < 494; i++) {
    double sum = out[i];
    for(int j = 0; j < 10; j++) {
      si = (nzval[(j + (i * 10))] * vec[cols[(j + (i * 10))]]);
      sum = (sum + si);
    }
    //---
    out[i] = sum;
  }
}
