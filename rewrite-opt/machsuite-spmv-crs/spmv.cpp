#include "ap_int.h"


#pragma SDS data copy(val[0:NNZ])
void spmv(double val[1666], ap_int<32> cols[1666], ap_int<32> row_delimiters[495], double vec[494], double out[494]) {
  #pragma HLS INTERFACE s_axilite port=val
  #pragma HLS INTERFACE s_axilite port=cols
  #pragma HLS INTERFACE s_axilite port=row_delimiters
  #pragma HLS INTERFACE s_axilite port=vec
  #pragma HLS INTERFACE s_axilite port=out
  
  double sum = 0.0;
  
  double si = 0.0;
  
  ap_int<32> tmp_begin = 0;
  
  ap_int<32> tmp_end = 0;
  
  ap_int<32> j = 0;
  
  for(int i = 0; i < 494; i++) {
    sum = 0.0;
    si = 0.0;
    tmp_begin = row_delimiters[i];
    //---
    tmp_end = row_delimiters[(i + 1)];
    j = tmp_begin;
    while((j < tmp_end)) {
      #pragma HLS loop_tripcount max=494 min=0
      si = (val[j] * vec[cols[j]]);
      sum = (sum + si);
      j = (j + 1);
    }
    out[i] = sum;
  }
}