#include <stdio.h>
// N_NODES: 256
#include <inttypes.h>
// N_EDGES: 4096
// N_LEVELS: 10
// MAX_LEVEL: INT8_MAX
template < int N >
using ap_int = int;

typedef struct {
  uint64_t dst;
} edge_t;
typedef struct {
  uint64_t edge_begin;
  uint64_t edge_end;
} node_t;

void bfs(node_t nodes[256], edge_t edges[4096], uint64_t starting_node, signed char level[256], uint64_t level_counts[10]) {

  ap_int<1> cnt = 0;
  uint64_t tmp_begin = 0;
  uint64_t tmp_end = 0;
  uint64_t tmp_dst = 0;
  signed char tmp_level = 0;
  ap_int<32> MAX_LEVEL = INT8_MAX;
  ap_int<32> tmp_l = 0;
	level[starting_node] = 0;
	level_counts[0] = 1;
  for(int horizon = 0; horizon < 10; horizon++) {
    cnt = 0;
    for(int n = 0; n < 256; n++) {
      tmp_l = level[n];
      //---
      if((tmp_l == horizon)){
        tmp_begin = nodes[n].edge_begin;
        //---
        tmp_end = nodes[n].edge_end;
        //---
        uint64_t e = tmp_begin;
        while((e < tmp_end)) {
          tmp_dst = edges[e].dst;
          tmp_level = level[tmp_dst];
          //---
          if((tmp_level == MAX_LEVEL)){
            level[tmp_dst] = (horizon + 1);
            cnt = (cnt + 1);
          } else{

          }
					e++;
        }
      } else{

      }
    }
		if (cnt == 0) break;
		level_counts[horizon+1] = cnt;
  }
}
