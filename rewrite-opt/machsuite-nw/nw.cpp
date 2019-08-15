#include "ap_int.h"


#pragma SDS data copy(SEQA[0:128])
#pragma SDS data zero_copy(M[0:16641], ptr[0:16641])
void nw(ap_int<8> SEQA[128], ap_int<8> SEQB[128], ap_int<8> alignedA[256], ap_int<8> alignedB[256], ap_int<32> M[129][129], ap_int<8> ptr[129][129]) {
  #pragma HLS INTERFACE s_axilite port=SEQA
  #pragma HLS INTERFACE s_axilite port=SEQB
  #pragma HLS INTERFACE s_axilite port=alignedA
  #pragma HLS INTERFACE s_axilite port=alignedB
  #pragma HLS INTERFACE s_axilite port=M
  #pragma HLS INTERFACE s_axilite port=ptr
  
  ap_int<32> MATCH_SCORE = 1;
  
  ap_int<32> MISMATCH_SCORE = (0 - 1);
  
  ap_int<32> GAP_SCORE = (0 - 1);
  
  ap_int<8> ALIGN = 92;
  
  ap_int<8> SKIPA = 94;
  
  ap_int<8> SKIPB = 60;
  
  ap_int<8> DASH = 45;
  
  ap_int<8> UNDERSCORE = 95;
  
  for(int a_idx = 0; a_idx < 129; a_idx++) {
    #pragma HLS loop_tripcount max=100 min=0
    M[0][a_idx] = (a_idx * GAP_SCORE);
  }
  //---
  for(int b_idx = 0; b_idx < 129; b_idx++) {
    #pragma HLS loop_tripcount max=100 min=0
    M[b_idx][0] = (b_idx * GAP_SCORE);
  }
  //---
  for(int b_idx = 1; b_idx < 129; b_idx++) {
    #pragma HLS loop_tripcount max=100 min=0
    for(int a_idx = 1; a_idx < 129; a_idx++) {
      #pragma HLS loop_tripcount max=100 min=0
      ap_int<32> score = 0;
      
      if((SEQA[(a_idx - 1)] == SEQB[(b_idx - 1)])){
        score = MATCH_SCORE;
      } else{
        score = MISMATCH_SCORE;
      }
      ap_int<32> up_left = (M[(b_idx - 1)][(a_idx - 1)] + score);
      
      //---
      ap_int<32> up = (M[(b_idx - 1)][a_idx] + GAP_SCORE);
      
      //---
      ap_int<32> left = (M[b_idx][(a_idx - 1)] + GAP_SCORE);
      
      ap_int<32> max = 0;
      
      if(((up_left >= up) && (up_left >= left))){
        max = up_left;
      } else{
        if((up >= left)){
          max = up;
        } else{
          max = left;
        }
      }
      //---
      M[b_idx][a_idx] = max;
      if((max == left)){
        ptr[b_idx][a_idx] = SKIPB;
      } else{
        if((max == up)){
          ptr[b_idx][a_idx] = SKIPA;
        } else{
          ptr[b_idx][a_idx] = ALIGN;
        }
      }
    }
  }
  //---
  ap_int<32> a_idx = 128;
  
  ap_int<32> b_idx = 128;
  
  ap_int<32> a_str_idx = 0;
  
  ap_int<32> b_str_idx = 0;
  
  while(((a_idx > 0) || (b_idx > 0))) {
    #pragma HLS loop_tripcount max=100 min=0
    ap_int<32> r = (b_idx * 129);
    
    if((ptr[b_idx][a_idx] == ALIGN)){
      alignedA[a_str_idx] = SEQA[(a_idx - 1)];
      alignedB[b_str_idx] = SEQB[(b_idx - 1)];
      a_idx = (a_idx - 1);
      b_idx = (b_idx - 1);
    } else{
      if((ptr[b_idx][a_idx] == SKIPB)){
        alignedA[a_str_idx] = SEQA[(a_idx - 1)];
        alignedB[b_str_idx] = DASH;
        a_idx = (a_idx - 1);
      } else{
        alignedA[a_str_idx] = DASH;
        alignedB[b_str_idx] = SEQB[(b_idx - 1)];
        b_idx = (b_idx - 1);
      }
    }
    a_str_idx = (a_str_idx + 1);
    b_str_idx = (b_str_idx + 1);
  }
  //---
  while((a_str_idx < 256)) {
    #pragma HLS loop_tripcount max=100 min=0
    alignedA[a_str_idx] = UNDERSCORE;
    a_str_idx = (a_str_idx + 1);
  }
  while((b_str_idx < 256)) {
    #pragma HLS loop_tripcount max=100 min=0
    alignedB[b_str_idx] = UNDERSCORE;
    b_str_idx = (b_str_idx + 1);
  }
}