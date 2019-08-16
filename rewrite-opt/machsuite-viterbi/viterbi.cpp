#include "ap_int.h"


#pragma SDS data copy(obs[0:140])
#pragma SDS data zero_copy(path[0:140])
void viterbi(ap_int<8> obs[140], double init[64], double transition[64][64], double emission[64][64], ap_int<8> path[140]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=obs
  #pragma HLS INTERFACE s_axilite port=init
  #pragma HLS INTERFACE s_axilite port=transition
  #pragma HLS INTERFACE s_axilite port=emission
  #pragma HLS INTERFACE s_axilite port=path
  
  double llike[140][64];
  
  double min_p = 0.0;
  
  double p = 0.0;
  
  ap_int<8> min_s = 0;
  
  for(int s = 0; s < 64; s++) {
    llike[0][s] = (init[s] + emission[s][obs[0]]);
  }
  //---
  for(int t = 1; t < 140; t++) {
    for(int curr = 0; curr < 64; curr++) {
      min_p = (llike[(t - 1)][0] + (transition[0][curr] + emission[curr][obs[t]]));
      //---
      for(int prev = 1; prev < 64; prev++) {
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
    min_s = 0;
    temp_t = t;
    min_p = (llike[t][min_s] + transition[min_s][path[(t + 1)]]);
    //---
    for(int s = 1; s < 64; s++) {
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