/*
Implementations based on:
Harish and Narayanan. "Accelerating large graph algorithms on the GPU using CUDA." HiPC, 2007.
Hong, Oguntebi, Olukotun. "Efficient Parallel Graph Exploration on Multi-Core CPU and GPU." PACT, 2011.
*/

#include "bfs.h"
#include <iostream>
extern "C"
{
  void bfs(
      edge_index_t nodes_edge_begin[N_NODES],
      edge_index_t nodes_edge_end[N_NODES],

      node_index_t edges_src[N_EDGES],
      node_index_t edges_dst[N_EDGES],

      node_index_t starting_node,
      level_t level[N_NODES],
      edge_index_t level_counts[N_LEVELS])
  {
#pragma HLS INTERFACE m_axi port = nodes_edge_begin offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = nodes_edge_end offset = slave bundle = gmem

#pragma HLS INTERFACE m_axi port = edges_src offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = edges_dst offset = slave bundle = gmem

#pragma HLS INTERFACE m_axi port = level offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = level_counts offset = slave bundle = gmem

#pragma HLS INTERFACE s_axilite port = nodes_edge_begin bundle = control
#pragma HLS INTERFACE s_axilite port = nodes_edge_end bundle = control

#pragma HLS INTERFACE s_axilite port = edges_src bundle = control
#pragma HLS INTERFACE s_axilite port = edges_dst bundle = control

#pragma HLS INTERFACE s_axilite port = level bundle = control
#pragma HLS INTERFACE s_axilite port = starting_node bundle = control
#pragma HLS INTERFACE s_axilite port = level_counts bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    node_index_t n;
    edge_index_t e;
    level_t horizon;
    edge_index_t cnt;

    level[starting_node] = 0;
    level_counts[0] = 1;

    for (auto k = 0; k < N_NODES; k++) {
      std::cout << (int)level[k] << " ";
    }
    std::cout << "---------" << std::endl;

    for (horizon = 0; horizon < N_LEVELS; horizon++) {
      cnt = 0;
     //Add unmarked neighbors of the current horizon to the next horizon
      for (n = 0; n < N_NODES; n++) {
        if (level[n] == horizon) {
          edge_index_t tmp_begin = nodes_edge_begin[n];
          edge_index_t tmp_end = nodes_edge_end[n];
          for (e = tmp_begin; e < tmp_end; e++) {
            node_index_t tmp_dst = edges_dst[n];
            level_t tmp_level = level[tmp_dst];
            std::cout  << "N_NODES: " << ","
              << "tmp_dst: " << tmp_dst << ", "
              << "tmp_level: " << (int)tmp_level << ", "
              << "MAX_LEVEL: " << MAX_LEVEL << ", "
              << "cnt: " << cnt << std::endl;

            if (tmp_level == MAX_LEVEL) {
              level[tmp_dst] = horizon + 1;
              ++cnt;
            }
          }
        }
      }
      level_counts[horizon + 1] = cnt;
      if (cnt == 0) {
        break;
      }
    }
  }
}
