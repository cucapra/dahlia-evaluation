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
#pragma SDS data zero_copy(level_counts[0:10])
void bfs(node_t nodes[256], edge_t edges[4096], ap_uint<64> starting_node, ap_int<8> level[256], ap_uint<64> level_counts[10]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=nodes
  #pragma HLS INTERFACE s_axilite port=edges
  
  #pragma HLS INTERFACE s_axilite port=level
  #pragma HLS INTERFACE s_axilite port=level_counts
  
  ap_uint<64> queue[256];
  
  ap_int<8> MAX_LEVEL = 127;
  
  ap_uint<64> q_in = 1;
  
  ap_uint<64> q_out = 0;
  
  level[starting_node] = 0;
  level_counts[0] = 1;
  if((q_in == 0)){
    queue[255] = starting_node;
  } else{
    queue[(q_in - 1)] = starting_node;
  }
  ap_uint<64> temp_q_in = ((q_in + 1) % 256);
  
  q_in = temp_q_in;
  //---
  for(int dummy = 0; dummy < 256; dummy++) {
    #pragma HLS loop_tripcount max=256 min=1
    int q_empty = 0;
    
    if((q_in > q_out)){
      q_empty = (q_in == (q_out + 1));
    } else{
      q_empty = ((q_in == 0) && (q_out == 255));
    }
    if(q_empty){
      dummy = 256;
    } else{
      
    }
    ap_uint<64> n = queue[q_out];
    
    q_out = ((q_out + 1) % 256);
    ap_uint<64> e = nodes[n].edge_begin;
    
    ap_uint<64> tmp_end = nodes[n].edge_end;
    
    //---
    while((e < tmp_end)) {
      #pragma HLS loop_tripcount avg=17
      ap_uint<64> tmp_dst = edges[e].dst;
      
      ap_int<8> tmp_level = level[tmp_dst];
      
      //---
      if((tmp_level == MAX_LEVEL)){
        tmp_level = (level[n] + 1);
        ap_uint<64> tmp_level_counts = level_counts[tmp_level];
        
        //---
        level[tmp_dst] = tmp_level;
        level_counts[tmp_level] = (tmp_level_counts + 1);
        if((q_in == 0)){
          queue[255] = tmp_dst;
        } else{
          queue[(q_in - 1)] = tmp_dst;
        }
        temp_q_in = ((q_in + 1) % 256);
        q_in = temp_q_in;
      } else{
        
      }
      e = (e + 1);
    }
  }
}