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
void bfs(
node_t nodes[256], 
edge_t edges[4096], node_index_t starting_node, 
level_t level[256], 
edge_index_t level_counts[10]) {
  
  int8_t MAX_LEVEL = 127;
  level[starting_node] = 0;
  level_counts[0] = 1;
  //---
  for(int horizon = 0; horizon < 10; horizon++) {
    #pragma HLS loop_tripcount max=10 min=1
    uint64_t cnt = 0;
    for(int n = 0; n < 256; n++) {
      int8_t tmp_l = level[n];
      //---
      if((tmp_l == horizon)){
        uint64_t e = nodes[n].edge_begin;
        //---
        uint64_t tmp_end = nodes[n].edge_end;
        //---
        while((e < tmp_end)) {
          #pragma HLS loop_tripcount avg=12
          uint64_t tmp_dst = edges[e].dst;
          int8_t tmp_level = level[tmp_dst];
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
