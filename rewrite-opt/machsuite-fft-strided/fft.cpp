#include "ap_int.h"


#pragma SDS data zero_copy(real[0:1024], img[0:1024])
void fft(double real[1024], double img[1024], double real_twid[512], double img_twid[512]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=real
  #pragma HLS INTERFACE s_axilite port=img
  #pragma HLS INTERFACE s_axilite port=real_twid
  #pragma HLS INTERFACE s_axilite port=img_twid
  
  ap_int<32> span = (1024 >> 1);
  
  ap_int<32> log = 0;
  
  while((span != 0)) {
    ap_int<32> odd = span;
    
    while((odd < 1024)) {
      #pragma HLS loop_tripcount max=512 min=512
      odd = (odd | span);
      ap_int<32> even = (odd ^ span);
      
      double real_odd = real[odd];
      
      double img_odd = img[odd];
      
      //---
      double real_even = real[even];
      
      double img_even = img[even];
      
      double temp_r = (real_even + real_odd);
      
      double temp_i = (img_even + img_odd);
      
      //---
      real[odd] = (real_even - real_odd);
      img[odd] = (img_even - img_odd);
      //---
      real[even] = temp_r;
      img[even] = temp_i;
      ap_int<32> rootindex = ((even << log) & (1024 - 1));
      
      //---
      if((rootindex != 0)){
        double temp = ((real_twid[rootindex] * real[odd]) - (img_twid[rootindex] * img[odd]));
        
        img_odd = img[odd];
        //---
        img[odd] = ((real_twid[rootindex] * img_odd) + (img_twid[rootindex] * real[odd]));
        //---
        real[odd] = temp;
      } else{
        
      }
      odd = (odd + 1);
    }
    span = (span >> 1);
    log = (log + 1);
  }
}