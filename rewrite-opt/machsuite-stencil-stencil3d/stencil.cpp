#include "ap_int.h"


#pragma SDS data copy(C[0:2])
void stencil(ap_int<32> C[2], ap_int<32> orig[33][33][18], ap_int<32> sol[32][32][16]) {
  #pragma HLS INTERFACE s_axilite port=C
  #pragma HLS INTERFACE s_axilite port=orig
  #pragma HLS INTERFACE s_axilite port=sol
  
  #pragma HLS ARRAY_PARTITION variable=C cyclic factor=2 dim=1
  
  #pragma HLS ARRAY_PARTITION variable=orig cyclic factor=3 dim=1
  #pragma HLS ARRAY_PARTITION variable=orig cyclic factor=3 dim=2
  #pragma HLS ARRAY_PARTITION variable=orig cyclic factor=3 dim=3
  
  for(int j = 0; j < 32; j++) {
    for(int k = 0; k < 16; k++) {
      sol[0][j][k] = orig[0][j][k];
      //---
      sol[31][j][k] = orig[31][j][k];
    }
  }
  //---
  
  for(int i = 1; i < 31; i++) {
    for(int k = 0; k < 16; k++) {
      sol[i][0][k] = orig[i][0][k];
      //---
      sol[i][31][k] = orig[i][31][k];
    }
  }
  //---
  
  for(int i = 1; i < 31; i++) {
    for(int j = 1; j < 31; j++) {
      sol[i][j][0] = orig[i][j][0];
      //---
      sol[i][j][15] = orig[i][j][15];
    }
  }
  //---
  for(int i = 0; i < 30; i++) {
    for(int j = 0; j < 30; j++) {
      #pragma HLS pipeline
      for(int k = 0; k < 14; k++) {
        #pragma HLS pipeline
        
        
        ap_int<32> temp_1 = 0;
        
        for(int l1 = 0; l1 < 3; l1++) {
          #pragma HLS UNROLL factor=3 skip_exit_check
          
          ap_int<32> temp_2 = 0;
          
          for(int l2 = 0; l2 < 3; l2++) {
            #pragma HLS UNROLL factor=3 skip_exit_check
            
            ap_int<32> temp_3 = 0;
            
            for(int l3 = 0; l3 < 3; l3++) {
              #pragma HLS UNROLL factor=3 skip_exit_check
              
              ap_int<32> multiply = 0;
              
              if(((l1 == 1) && ((l2 == 1) && (l3 == 1)))){
                multiply = C[0];
              } else{
                if((((l1 == 1) && (l2 == 1)) || (((l2 == 1) && (l3 == 1)) || ((l3 == 1) && (l1 == 1))))){
                  multiply = C[1];
                } else{
                  multiply = 0;
                }
              }
              ap_int<32> mul = (orig[(i + l1)][(j + l2)][(k + l3)] * multiply);
              
              // combiner:
              temp_3 += mul;
            }
            // combiner:
            temp_2 += temp_3;
          }
          // combiner:
          temp_1 += temp_2;
        }
        sol[(1 + i)][(1 + j)][(1 + k)] = temp_1;
      }
    }
  }
}