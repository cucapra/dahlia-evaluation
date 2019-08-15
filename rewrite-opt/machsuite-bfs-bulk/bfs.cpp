#include "ap_int.h"

typedef struct {
  ap_uint<64> src;
  ap_uint<64> dst;
} edge_t;
typedef struct {
  ap_uint<64> edge_begin;
  ap_uint<64> edge_end;
} node_t;
#pragma SDS data copy(nodes[0:256])
#pragma SDS data zero_copy(level[0:256])
void bfs(node_t nodes[256], edge_t edges[4096], ap_uint<64> starting_node, ap_int<8> level[256], ap_uint<64> level_counts[10]) {
  #pragma HLS INTERFACE s_axilite port=nodes
  #pragma HLS INTERFACE s_axilite port=edges
  
  #pragma HLS INTERFACE s_axilite port=level
  #pragma HLS INTERFACE s_axilite port=level_counts
  
  ap_int<8> MAX_LEVEL = 127;
  
  level[starting_node] = 0;
  level_counts[0] = 1;
  //---
  for(int horizon = 0; horizon < 10; horizon++) {
    #pragma HLS loop_tripcount max=10 min=1
    ap_uint<64> cnt = 0;
    
    for(int n = 0; n < 256; n++) {
      ap_int<8> tmp_l = level[n];
      
      //---
      if((tmp_l == horizon)){
        ap_uint<64> e = nodes[n].edge_begin;
        
        //---
        ap_uint<64> tmp_end = nodes[n].edge_end;
        
        //---
        while((e < tmp_end)) {
          #pragma HLS loop_tripcount avg=17
          ap_uint<64> tmp_dst = edges[e].dst;
          
          ap_int<8> tmp_level = level[tmp_dst];
          
          //---
          if((tmp_level == MAX_LEVEL)){
            level[tmp_dst] = (horizon + 1);
            //---
            cnt = (cnt + 1);
          } else{
            
          }
          e = (e + 1);
        }
      } else{
        
      }
    }
    level_counts[(horizon + 1)] = cnt;
    if((cnt == 0)){
      horizon = 10;
    } else{
      
    }
  }
}