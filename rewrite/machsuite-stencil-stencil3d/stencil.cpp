// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = unsigned int;
#endif


#pragma SDS data copy(C[0:2])
void stencil(
ap_int<32> C[2], 
ap_int<32> orig[32][32][16], 
ap_int<32> sol[32][32][16]) {
  
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
  for(int i = 1; i < 31; i++) {
    for(int j = 1; j < 31; j++) {
      for(int k = 1; k < 15; k++) {
        ap_int<32> temp1 = (orig[i][j][k] * C[0]);
        //---
        ap_int<32> temp2 = ((orig[(i + 1)][j][k] * C[1]) + temp1);
        //---
        ap_int<32> temp3 = ((orig[(i - 1)][j][k] * C[1]) + temp2);
        //---
        ap_int<32> temp4 = ((orig[i][(j + 1)][k] * C[1]) + temp3);
        //---
        ap_int<32> temp5 = ((orig[i][(j - 1)][k] * C[1]) + temp4);
        //---
        ap_int<32> temp6 = ((orig[i][j][(k + 1)] * C[1]) + temp5);
        //---
        sol[i][j][k] = ((orig[i][j][(k - 1)] * C[1]) + temp6);
      }
    }
  }
}