#include <inttypes.h>

#ifdef __SDSCC__
#include "ap_int.h"
#else
template < int N >
using ap_int = int;
#endif

#pragma SDS data copy(obs[0:N_OBS])
#pragma SDS data zero_copy(path[0:N_OBS])
void viterbi(uint8_t obs[32], double init[5], double transition[25], double emission[45], uint8_t path[32]) {
  
  ap_int<8> llike[32][5];
  ap_int<8> min_p = 0;
  ap_int<8> p = 0;
  ap_int<8> min_s = 0;
  for(int s = 1; s < 5; s++) {
    llike[0][s] = (init[s] + emission[((s * 9) + obs[0])]);
  }
  //---
  for(int t = 1; t < 32; t++) {
#pragma HLS loop_tripcount avg=31
    for(int curr = 0; curr < 5; curr++) {
#pragma HLS loop_tripcount avg=5
      for(int prev = 0; prev < 5; prev++) {
#pragma HLS loop_tripcount avg=5
        if((prev == 0)){
          min_p = (llike[(t - 1)][prev] + (transition[((prev * 5) + curr)] + emission[((curr * 9) + obs[t])]));
        } else{
          
        }
        //---
        if((prev != 0)){
          p = (llike[(t - 1)][prev] + (transition[((prev * 5) + curr)] + emission[((curr * 9) + obs[t])]));
          if((p < min_p)){
            min_p = p;
          } else{
            
          }
        } else{
          
        }
      }
      //---
      llike[t][curr] = min_p;
    }
  }
  //---
  min_p = llike[31][min_s];
  //---
  for(int s = 1; s < 5; s++) {
#pragma HLS loop_tripcount avg=4
    p = llike[31][s];
    if((p < min_p)){
      min_p = p;
      min_s = s;
    } else{
      
    }
  }
  path[31] = min_s;
  //---
  ap_int<6> t = 30;
  while((t >= 0)) {
#pragma HLS loop_tripcount avg=30
    min_s = 0;
    min_p = (llike[t][min_s] + transition[((min_s * 5) + path[(t + 1)])]);
    //---
    for(int s = 0; s < 5; s++) {
#pragma HLS loop_tripcount avg=5
      p = (llike[t][s] + transition[((s * 5) + path[(t + 1)])]);
      if((p < min_p)){
        min_p = p;
        min_s = s;
      } else{
        
      }
    }
    //---
    path[t] = min_s;
  }
}