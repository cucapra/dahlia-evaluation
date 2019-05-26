
typedef struct {
  ap_int<32> dst;
} edge_t;
typedef struct {
  ap_int<32> edge_begin;
  ap_int<32> edge_end;
} node_t;
void bfs(node_t nodes[50], edge_t edges[50], ap_int<32> starting_node, ap_int<32> level[50], ap_int<32> level_counts[50]) {
  
  ap_int<1> cnt = 0;
  ap_int<32> tmp_begin = 0;
  ap_int<32> tmp_end = 0;
  ap_int<32> tmp_dst = 0;
  ap_int<32> tmp_level = 0;
  ap_int<5> MAX_LEVEL = 10;
  ap_int<32> tmp_l = 0;
  for(int horizon = 0; horizon < 50; horizon++) {
    cnt = 0;
    for(int n = 0; n < 50; n++) {
      tmp_l = level[n];
      //---
      if((tmp_l == horizon)){
        tmp_begin = nodes[n].edge_begin;
        //---
        tmp_end = nodes[n].edge_end;
        //---
        ap_int<32> e = tmp_begin;
        while((e < tmp_end)) {
          tmp_dst = edges[e].dst;
          tmp_level = level[tmp_dst];
          //---
          if((tmp_level == MAX_LEVEL)){
            level[tmp_dst] = (horizon + 1);
            cnt = (cnt + 1);
          } else{
            
          }
        }
      } else{
        
      }
    }
  }
}