#include <inttypes.h>
#include <stdio.h>
#ifdef __SDSCC__
#include "ap_int.h"
#else
template < int N >
using ap_int = int;
#endif

#pragma SDS data copy(obs[0:N_OBS])
#pragma SDS data zero_copy(path[0:N_OBS])
void viterbi(uint8_t obs[140], double init[64], double transition[4096], double emission[4096], uint8_t path[140]) {
  
  double llike[140][64];
  double min_p = 0.0;
  double p = 0.0;
  ap_int<8> min_s = 0;
  for(int s = 0; s < 64; s++) {
    llike[0][s] = (init[s] + emission[((s * 64) + obs[0])]);
  }
  //---
  for(int t = 1; t < 140; t++) {
#pragma HLS loop_tripcount avg=140
    for(int curr = 0; curr < 64; curr++) {
#pragma HLS loop_tripcount avg=64
      min_p = (llike[(t - 1)][0] + (transition[((0 * 64) + curr)] + emission[((curr * 64) + obs[t])]));
      //---
      for(int prev = 1; prev < 64; prev++) {
#pragma HLS loop_tripcount avg=64
        p = (llike[(t - 1)][prev] + (transition[((prev * 64) + curr)] + emission[((curr * 64) + obs[t])]));
        if((p < min_p)){
          min_p = p;
        }
      }
      //---
      llike[t][curr] = min_p;
    }
  }
  //---
  min_p = llike[139][min_s];
  //---
  for(int s = 1; s < 64; s++) {
#pragma HLS loop_tripcount avg=4
    p = llike[139][s];
    if((p < min_p)){
      min_p = p;
      min_s = s;
    }
  }
  path[139] = min_s;
  //---
  ap_int<9> t = 138;
  ap_int<9> temp_t = 138;
  while((t >= 0)) {
#pragma HLS loop_tripcount avg=138
    min_s = 0;
    temp_t = t;
    min_p = (llike[t][min_s] + transition[((min_s * 64) + path[(t + 1)])]);
    //---
    for(int s = 1; s < 64; s++) {
#pragma HLS loop_tripcount avg=64
      p = (llike[t][s] + transition[((s * 64) + path[(t + 1)])]);
      if((p < min_p)){
        min_p = p;
        min_s = s;
      }
    }
    //---
    path[t] = min_s;
    t = temp_t - 1;
  }
}
