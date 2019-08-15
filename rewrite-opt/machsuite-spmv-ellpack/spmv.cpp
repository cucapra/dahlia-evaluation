#include "ap_int.h"


#pragma SDS data copy(nzval[0:494][0:10])
#pragma SDS data zero_copy(out[0:494])
void spmv(double nzval[496][10], ap_int<32> cols[496][10], double vec[496], double out[496]) {
  #pragma HLS INTERFACE s_axilite port=nzval
  #pragma HLS INTERFACE s_axilite port=cols
  #pragma HLS INTERFACE s_axilite port=vec
  #pragma HLS INTERFACE s_axilite port=out
  
  #pragma HLS ARRAY_PARTITION variable=nzval cyclic factor=8 dim=1
  
  #pragma HLS ARRAY_PARTITION variable=cols cyclic factor=8 dim=1
  
  #pragma HLS ARRAY_PARTITION variable=out cyclic factor=8 dim=1
  for(int i = 0; i < 496; i++) {
    #pragma HLS UNROLL factor=8 skip_exit_check
    
    for(int j = 0; j < 10; j++) {
      double si = (nzval[i][j] * vec[cols[i][j]]);
      
      // combiner:
      out[i] += si;
    }
  }
}