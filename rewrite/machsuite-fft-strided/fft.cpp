// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = unsigned int;
#endif


#pragma SDS data zero_copy(real[0:FFT_SIZE], img[0:FFT_SIZE])
void fft(
double real[1024], 
double img[1024], 
double real_twid[512], 
double img_twid[512]) {
  
  ap_int<32> even = 0;
  ap_int<32> span = (1024 >> 1);
  ap_int<32> log = 0;
  ap_int<32> rootindex = 0;
  double temp = 0.0;
  double real_odd = 0.0;
  double real_even = 0.0;
  double img_even = 0.0;
  double img_odd = 0.0;
  ap_int<32> odd = 0;
  while((span != 0)) {
    odd = span;
    while((odd < 1024)) {
      #pragma HLS loop_tripcount max=1024 min=512
      odd = (odd | span);
      even = (odd ^ span);
      real_even = real[even];
      //---
      real_odd = real[odd];
      temp = (real_even + real_odd);
      //---
      real[odd] = (real_even - real_odd);
      //---
      real[even] = temp;
      //---
      img_even = img[even];
      //---
      img_odd = img[odd];
      temp = (img_even + img_odd);
      //---
      img[odd] = (img_even - img_odd);
      //---
      img[even] = temp;
      rootindex = ((even << log) & (1024 - 1));
      //---
      if((rootindex != 0)){
        temp = ((real_twid[rootindex] * real[odd]) - (img_twid[rootindex] * img[odd]));
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