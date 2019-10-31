#include <ap_int.h>
#pragma SDS data copy(obs[0:140])
#pragma SDS data zero_copy(path[0:140])
extern "C" {
  void viterbi(ap_int<8> obs[140], double init[64], double transition[64][64], double emission[64][64], ap_int<8> path[140]) {
    #pragma HLS INTERFACE m_axi port=obs offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=obs bundle=control
    #pragma HLS INTERFACE m_axi port=init offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=init bundle=control
    #pragma HLS INTERFACE m_axi port=transition offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=transition bundle=control
    #pragma HLS INTERFACE m_axi port=emission offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=emission bundle=control
    #pragma HLS INTERFACE m_axi port=path offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=path bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    double llike[140][64];
    double min_p = 0.0;
    double p = 0.0;
    ap_int<8> min_s = 0;
    for(int s = 0; s < 64; s++) {
      llike[0][s] = (init[s] + emission[s][obs[0]]);
    }
    //---
    for(int t = 1; t < 140; t++) {
      #pragma HLS loop_tripcount avg=139
      for(int curr = 0; curr < 64; curr++) {
        #pragma HLS loop_tripcount avg=64
        min_p = ((llike[(t - 1)][0] + transition[0][curr]) + emission[curr][obs[t]]);
        //---
        for(int prev = 1; prev < 64; prev++) {
          #pragma HLS loop_tripcount avg=63
          p = ((llike[(t - 1)][prev] + transition[prev][curr]) + emission[curr][obs[t]]);
          if ((p < min_p)) {
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
      #pragma HLS loop_tripcount avg=63
      p = llike[139][s];
      if ((p < min_p)) {
        min_p = p;
        min_s = s;
      }
    }
    path[139] = min_s;
    //---
    ap_int<8> t = 138;
    ap_int<8> temp_t = 138;
    while((t >= 0)) {
      #pragma HLS loop_tripcount avg=138
      min_s = 0;
      temp_t = t;
      min_p = (llike[t][min_s] + transition[min_s][path[(t + 1)]]);
      //---
      for(int s = 1; s < 64; s++) {
        #pragma HLS loop_tripcount avg=63
        p = (llike[t][s] + transition[s][path[(t + 1)]]);
        if ((p < min_p)) {
          min_p = p;
          min_s = s;
        }
      }
      //---
      path[t] = min_s;
      t = (temp_t - 1);
    }
  }
}