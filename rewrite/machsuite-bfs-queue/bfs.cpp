#include <ap_int.h>
extern "C" {
  void bfs(ap_uint<64> nodes_edge_begin[256], ap_uint<64> nodes_edge_end[256], ap_uint<64> edges_src[4096], ap_uint<64> edges_dst[4096], ap_uint<64> starting_node_fake, ap_int<8> level[256], ap_uint<64> level_counts[10]) {
    #pragma HLS INTERFACE m_axi port=nodes_edge_begin offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=nodes_edge_begin bundle=control
    #pragma HLS INTERFACE m_axi port=nodes_edge_end offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=nodes_edge_end bundle=control
    #pragma HLS INTERFACE m_axi port=edges_src offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=edges_src bundle=control
    #pragma HLS INTERFACE m_axi port=edges_dst offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=edges_dst bundle=control
    #pragma HLS INTERFACE s_axilite port=starting_node_fake bundle=control
    #pragma HLS INTERFACE m_axi port=level offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=level bundle=control
    #pragma HLS INTERFACE m_axi port=level_counts offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=level_counts bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    ap_uint<64> queue[256];
    ap_int<64> MAX_LEVEL = 127;
    ap_uint<64> starting_node = 38;
    ap_uint<64> q_in = 1;
    ap_uint<64> q_out = 0;
    level[starting_node] = 0;
    level_counts[0] = 1;
    if ((q_in == 0)) {
      queue[255] = starting_node;
    } else {
      queue[(q_in - 1)] = starting_node;
    }
    ap_uint<64> temp_q_in = ((q_in + 1) % 256);
    q_in = temp_q_in;
    //---
    for(int dummy = 0; dummy < 256; dummy++) {
      #pragma HLS loop_tripcount max=256 min=1
      int q_empty = 0;
      if ((q_in > q_out)) {
        q_empty = (q_in == (q_out + 1));
      } else {
        q_empty = ((q_in == 0) && (q_out == 255));
      }
      if (q_empty) {
        dummy = 256;
      }
      ap_uint<64> n = queue[q_out];
      q_out = ((q_out + 1) % 256);
      ap_uint<64> e = nodes_edge_begin[n];
      ap_uint<64> tmp_end = nodes_edge_end[n];
      //---
      while((e < tmp_end)) {
        #pragma HLS loop_tripcount avg=17
        ap_uint<64> tmp_dst = edges_dst[e];
        ap_int<8> tmp_level = level[tmp_dst];
        //---
        if ((tmp_level == MAX_LEVEL)) {
          tmp_level = (level[n] + 1);
          ap_uint<64> tmp_level_counts = level_counts[tmp_level];
          //---
          level[tmp_dst] = tmp_level;
          level_counts[tmp_level] = (tmp_level_counts + 1);
          if ((q_in == 0)) {
            queue[255] = tmp_dst;
          } else {
            queue[(q_in - 1)] = tmp_dst;
          }
          temp_q_in = ((q_in + 1) % 256);
          q_in = temp_q_in;
        }
        e = (e + 1);
      }
    }
  }
}