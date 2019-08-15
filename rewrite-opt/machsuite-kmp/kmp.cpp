#include "ap_int.h"

void cpf(ap_int<8> pattern[4], ap_int<32> kmpNext[4]) {
  
  
  ap_int<32> k = 0;
  
  kmpNext[0] = 0;
  //---
  for(int q = 1; q < 4; q++) {
    #pragma HLS loop_tripcount max=4 min=0
    ap_int<8> k_val = pattern[k];
    
    //---
    ap_int<8> q_val = pattern[q];
    
    //---
    while(((k > 0) && (k_val != q_val))) {
      #pragma HLS loop_tripcount max=4 min=0
      k = kmpNext[q];
      k_val = pattern[k];
      //---
      q_val = pattern[q];
    }
    //---
    if((k_val == q_val)){
      k = (k + 1);
    } else{
      
    }
    kmpNext[q] = k;
  }
}
#pragma SDS data copy(pattern[0:PATTERN_SIZE])
#pragma SDS data zero_copy(kmpNext[0:PATTERN_SIZE], n_matches)
#pragma SDS data zero_copy(input[0:STRING_SIZE])
void kmp(ap_int<8> pattern[4], ap_int<8> input[32411], ap_int<32> kmpNext[4], ap_int<32> n_matches[1]) {
  #pragma HLS INTERFACE s_axilite port=pattern
  #pragma HLS INTERFACE s_axilite port=input
  #pragma HLS INTERFACE s_axilite port=kmpNext
  #pragma HLS INTERFACE s_axilite port=n_matches
  
  n_matches[0] = 0;
  cpf(pattern, kmpNext);
  //---
  ap_int<32> q = 0;
  
  for(int i = 0; i < 32411; i++) {
    #pragma HLS loop_tripcount max=32500 min=0
    while(((q > 0) && (pattern[q] != input[i]))) {
      #pragma HLS loop_tripcount max=32500 min=0
      #pragma HLS pipeline
      q = kmpNext[q];
    }
    //---
    if((pattern[q] == input[i])){
      q = (q + 1);
    } else{
      
    }
    if((q >= 4)){
      ap_int<32> temp = n_matches[0];
      
      //---
      n_matches[0] = (temp + 1);
      q = kmpNext[(q - 1)];
    } else{
      
    }
  }
}