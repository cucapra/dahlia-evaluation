#include <ap_int.h>
extern "C" {
  void bfs(ap_uint<64> nodes_edge_begin[256], ap_uint<64> nodes_edge_end[256], ap_uint<64> edges_src[4096], ap_uint<64> edges_dst[4096], ap_uint<64> starting_node, ap_int<8> level[256], ap_uint<64> level_counts[10]) {
    #pragma HLS INTERFACE m_axi port=nodes_edge_begin offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=nodes_edge_begin bundle=control
    #pragma HLS INTERFACE m_axi port=nodes_edge_end offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=nodes_edge_end bundle=control
    #pragma HLS INTERFACE m_axi port=edges_src offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=edges_src bundle=control
    #pragma HLS INTERFACE m_axi port=edges_dst offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=edges_dst bundle=control
    #pragma HLS INTERFACE s_axilite port=starting_node bundle=control
    #pragma HLS INTERFACE m_axi port=level offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=level bundle=control
    #pragma HLS INTERFACE m_axi port=level_counts offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=level_counts bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
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
        if ((tmp_l == horizon)) {
          ap_uint<64> e = nodes_edge_begin[n];
          //---
          ap_uint<64> tmp_end = nodes_edge_end[n];
          //---
          while((e < tmp_end)) {
            #pragma HLS loop_tripcount avg=17
            ap_uint<64> tmp_dst = edges_dst[e];
            ap_int<8> tmp_level = level[tmp_dst];
            //---
            if ((tmp_level == MAX_LEVEL)) {
              level[tmp_dst] = (horizon + 1);
              //---
              cnt = (cnt + 1);
            }
            e = (e + 1);
          }
        }
      }
      level_counts[(horizon + 1)] = cnt;
      if ((cnt == 0)) {
        horizon = 10;
      }
    }
  }
}