// Avoid using `ap_int` in "software" compilation.
#include <inttypes.h>
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = uint64_t;
#endif

typedef uint64_t edge_index_t;
typedef uint64_t node_index_t;
typedef int8_t level_t;
typedef struct {
  uint64_t src;
  uint64_t dst;
} edge_t;
typedef struct {
  uint64_t edge_begin;
  uint64_t edge_end;
} node_t;
#pragma SDS data copy(nodes[0:256])
#pragma SDS data zero_copy(level[0:256])
#pragma SDS data zero_copy(level_counts[0:10])
void bfs(
node_t nodes[256], 
edge_t edges[4096], node_index_t starting_node, 
level_t level[256], 
edge_index_t level_counts[10]) {
  
  
  uint64_t queue[256];
  int8_t MAX_LEVEL = 127;
  uint64_t q_in = 1;
  uint64_t q_out = 0;
  level[starting_node] = 0;
  level_counts[0] = 1;
  if((q_in == 0)){
    queue[255] = starting_node;
  } else{
    queue[(q_in - 1)] = starting_node;
  }
  uint64_t temp_q_in = ((q_in + 1) % 256);
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
    uint64_t n = queue[q_out];
    q_out = ((q_out + 1) % 256);
    uint64_t e = nodes[n].edge_begin;
    uint64_t tmp_end = nodes[n].edge_end;
    //---
    while((e < tmp_end)) {
      #pragma HLS loop_tripcount avg=12
      uint64_t tmp_dst = edges[e].dst;
      int8_t tmp_level = level[tmp_dst];
      //---
      if((tmp_level == MAX_LEVEL)){
        tmp_level = (level[n] + 1);
        uint64_t tmp_level_counts = level_counts[tmp_level];
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
