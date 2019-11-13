// git.status = clean, build.date = Wed Nov 13 17:14:51 EST 2019, git.hash = bac29f5
#include <ap_int.h>
extern "C" {
  void nw(ap_int<8> SEQA_interface[128], ap_int<8> SEQB_interface[128], ap_int<8> alignedA[256], ap_int<8> alignedB[256], ap_int<32> M_interface[16641], ap_int<8> ptr_interface[16641]) {
    #pragma HLS INTERFACE m_axi port=SEQA_interface offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=SEQA_interface bundle=control
    #pragma HLS INTERFACE m_axi port=SEQB_interface offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=SEQB_interface bundle=control
    #pragma HLS INTERFACE m_axi port=alignedA offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=alignedA bundle=control
    #pragma HLS INTERFACE m_axi port=alignedB offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=alignedB bundle=control
    #pragma HLS INTERFACE m_axi port=M_interface offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=M_interface bundle=control
    #pragma HLS INTERFACE m_axi port=ptr_interface offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=ptr_interface bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    ap_int<8> SEQA[128];
    #pragma HLS resource variable=SEQA core=RAM_1P_BRAM
    #pragma HLS ARRAY_PARTITION variable=SEQA cyclic factor=2 dim=1
    ap_int<8> SEQB[128];
    #pragma HLS resource variable=SEQB core=RAM_1P_BRAM
    #pragma HLS ARRAY_PARTITION variable=SEQB cyclic factor=2 dim=1
    ap_int<32> M[130][130];
    #pragma HLS resource variable=M core=RAM_1P_BRAM
    #pragma HLS ARRAY_PARTITION variable=M cyclic factor=2 dim=1
    #pragma HLS ARRAY_PARTITION variable=M cyclic factor=2 dim=2
    ap_int<8> ptr[130][130];
    #pragma HLS resource variable=ptr core=RAM_1P_BRAM
    #pragma HLS ARRAY_PARTITION variable=ptr cyclic factor=2 dim=1
    #pragma HLS ARRAY_PARTITION variable=ptr cyclic factor=2 dim=2
    ap_int<32> MATCH_SCORE = 1;
    ap_int<32> MISMATCH_SCORE = (0 - 1);
    ap_int<32> GAP_SCORE = (0 - 1);
    ap_int<8> ALIGN = 92;
    ap_int<8> SKIPA = 94;
    ap_int<8> SKIPB = 60;
    ap_int<8> DASH = 45;
    ap_int<8> UNDERSCORE = 95;
    
    
    
    
    for(int i = 0; i < 128; i++) {
      SEQA[i] = SEQA_interface[i];
      SEQB[i] = SEQB_interface[i];
    }
    for(int i = 0; i < 129; i++) {
      for(int j = 0; j < 129; j++) {
        M[i][j] = M_interface[((i * 129) + j)];
        ptr[i][j] = ptr_interface[((i * 129) + j)];
      }
    }
    //---
    for(int a_idx = 0; a_idx < 130; a_idx++) {
      #pragma HLS UNROLL factor=2 skip_exit_check
      
      #pragma HLS loop_tripcount max=128 min=0
      M[0][a_idx] = (a_idx * GAP_SCORE);
    }
    //---
    for(int b_idx = 0; b_idx < 130; b_idx++) {
      #pragma HLS UNROLL factor=2 skip_exit_check
      
      #pragma HLS loop_tripcount max=128 min=0
      M[b_idx][0] = (b_idx * GAP_SCORE);
    }
    //---
    
    
    
    
    
    for(int b_idx = 1; b_idx < 129; b_idx++) {
      #pragma HLS UNROLL factor=2 skip_exit_check
      
      #pragma HLS loop_tripcount max=128 min=0
      for(int a_idx = 1; a_idx < 129; a_idx++) {
        #pragma HLS UNROLL factor=2 skip_exit_check
        
        #pragma HLS loop_tripcount max=128 min=0
        ap_int<32> score = 0;
        if ((SEQA[((1 * -1) + a_idx)] == SEQB[((1 * -1) + b_idx)])) {
          score = MATCH_SCORE;
        } else {
          score = MISMATCH_SCORE;
        }
        ap_int<32> up_left = (M[(-1 + b_idx)][(-1 + a_idx)] + score);
        //---
        ap_int<32> up = (M[(-1 + b_idx)][a_idx] + GAP_SCORE);
        //---
        ap_int<32> left = (M[b_idx][((1 * -1) + a_idx)] + GAP_SCORE);
        ap_int<32> max = 0;
        if (((up_left >= up) && (up_left >= left))) {
          max = up_left;
        } else {
          if ((up >= left)) {
            max = up;
          } else {
            max = left;
          }
        }
        //---
        M[b_idx][a_idx] = max;
        if ((max == left)) {
          ptr[b_idx][a_idx] = SKIPB;
        } else {
          if ((max == up)) {
            ptr[b_idx][a_idx] = SKIPA;
          } else {
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
      #pragma HLS loop_tripcount max=151 min=0
      ap_int<32> r = (b_idx * 129);
      if ((ptr[b_idx][a_idx] == ALIGN)) {
        alignedA[a_str_idx] = SEQA[(a_idx - 1)];
        alignedB[b_str_idx] = SEQB[(b_idx - 1)];
        a_idx = (a_idx - 1);
        b_idx = (b_idx - 1);
      } else {
        if ((ptr[b_idx][a_idx] == SKIPB)) {
          alignedA[a_str_idx] = SEQA[(a_idx - 1)];
          alignedB[b_str_idx] = DASH;
          a_idx = (a_idx - 1);
        } else {
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
      #pragma HLS loop_tripcount max=105 min=0
      alignedA[a_str_idx] = UNDERSCORE;
      a_str_idx = (a_str_idx + 1);
    }
    while((b_str_idx < 256)) {
      #pragma HLS loop_tripcount max=105 min=0
      alignedB[b_str_idx] = UNDERSCORE;
      b_str_idx = (b_str_idx + 1);
    }
  }
}