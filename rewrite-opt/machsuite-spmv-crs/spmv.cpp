#include "ap_int.h"


#pragma SDS data copy(val[0:NNZ])
void spmv(double val[1672], ap_int<32> cols[1672], ap_int<32> row_delimiters[496], double vec[496], double out[496]) {
  #pragma HLS INTERFACE s_axilite port=val
  #pragma HLS INTERFACE s_axilite port=cols
  #pragma HLS INTERFACE s_axilite port=row_delimiters
  #pragma HLS INTERFACE s_axilite port=vec
  #pragma HLS INTERFACE s_axilite port=out
  
  #pragma HLS ARRAY_PARTITION variable=row_delimiters cyclic factor=8 dim=1
  
  #pragma HLS ARRAY_PARTITION variable=out cyclic factor=8 dim=1
  double sum = 0.0;
  
  double si = 0.0;
  
  ap_int<32> tmp_begin = 0;
  
  ap_int<32> tmp_end = 0;
  
  ap_int<32> j = 0;
  
  
  for(int i = 0; i < 496; i++) {
    #pragma HLS UNROLL factor=8 skip_exit_check
    
    sum = 0.0;
    si = 0.0;
    tmp_begin = row_delimiters[i];
    //---
    tmp_end = row_delimiters[(1 + i)];
    j = tmp_begin;
    while((j < tmp_end)) {
      #pragma HLS loop_tripcount avg=3
      #pragma HLS pipeline
      si = (val[j] * vec[cols[j]]);
      sum = (sum + si);
      j = (j + 1);
    }
    out[i] = sum;
  }
}