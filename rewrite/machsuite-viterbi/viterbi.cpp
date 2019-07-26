// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = uint8_t;
#endif
#include <inttypes.h>

typedef uint8_t tok_t;
typedef double prob_t;
typedef uint8_t state_t;
#pragma SDS data copy(obs[0:140])
#pragma SDS data zero_copy(path[0:140])
void viterbi(
tok_t obs[140], 
prob_t init[64], 
prob_t transition[64][64], 
prob_t emission[64][64], 
state_t path[140]) {
  
  
  double llike[140][64];
  double min_p = 0.0;
  double p = 0.0;
  uint8_t min_s = 0;
  for(int s = 0; s < 64; s++) {
    llike[0][s] = (init[s] + emission[s][obs[0]]);
  }
  //---
  for(int t = 1; t < 140; t++) {
    #pragma HLS loop_tripcount avg=139
    for(int curr = 0; curr < 64; curr++) {
      #pragma HLS loop_tripcount avg=64
      min_p = (llike[(t - 1)][0] + (transition[0][curr] + emission[curr][obs[t]]));
      //---
      for(int prev = 1; prev < 64; prev++) {
        #pragma HLS loop_tripcount avg=63
        p = (llike[(t - 1)][prev] + (transition[prev][curr] + emission[curr][obs[t]]));
        if((p < min_p)){
          min_p = p;
        } else{
          
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
    #pragma HLS loop_tripcount avg=63
    p = llike[139][s];
    if((p < min_p)){
      min_p = p;
      min_s = s;
    } else{
      
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
    min_p = (llike[t][min_s] + transition[min_s][path[(t + 1)]]);
    //---
    for(int s = 1; s < 64; s++) {
      #pragma HLS loop_tripcount avg=63
      p = (llike[t][s] + transition[s][path[(t + 1)]]);
      if((p < min_p)){
        min_p = p;
        min_s = s;
      } else{
        
      }
    }
    //---
    path[t] = min_s;
    t = (temp_t - 1);
  }
}
