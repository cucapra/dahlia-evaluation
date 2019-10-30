/*
Implementation based on:
Hong, Oguntebi, Olukotun. "Efficient Parallel Graph Exploration on Multi-Core CPU and GPU." PACT, 2011.
*/

#include "bfs.h"

#define Q_PUSH(node)                                  \
  {                                                   \
    queue[q_in == 0 ? N_NODES - 1 : q_in - 1] = node; \
    q_in = (q_in + 1) % N_NODES;                      \
  }
#define Q_PEEK() (queue[q_out])
#define Q_POP()                    \
  {                                \
    q_out = (q_out + 1) % N_NODES; \
  }
#define Q_EMPTY() (q_in > q_out ? q_in == q_out + 1 : (q_in == 0) && (q_out == N_NODES - 1))

extern "C"
{
  void bfs(edge_index_t nodes_edge_begin[N_NODES],
           edge_index_t nodes_edge_end[N_NODES],
           // edge_t edges[N_EDGES],
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

    node_index_t queue[N_NODES];
    node_index_t q_in, q_out;
    node_index_t dummy;
    node_index_t n;
    edge_index_t e;

    /*init_levels: for( n=0; n<N_NODES; n++ )*/
    /*level[n] = MAX_LEVEL;*/
    /*init_horizons: for( i=0; i<N_LEVELS; i++ )*/
    /*level_counts[i] = 0;*/

    q_in = 1;
    q_out = 0;
    level[starting_node] = 0;
    level_counts[0] = 1;
    Q_PUSH(starting_node);

  loop_queue:
    for (dummy = 0; dummy < N_NODES; dummy++)
    { // Typically while(not_empty(queue)){
      if (Q_EMPTY())
        break;
      n = Q_PEEK();
      Q_POP();
      edge_index_t tmp_begin = nodes_edge_begin[n];
      edge_index_t tmp_end = nodes_edge_end[n];
    loop_neighbors:
      for (e = tmp_begin; e < tmp_end; e++)
      {
        node_index_t tmp_dst = edges_dst[e];
        level_t tmp_level = level[tmp_dst];

        if (tmp_level == MAX_LEVEL)
        { // Unmarked
          level_t tmp_level = level[n] + 1;
          level[tmp_dst] = tmp_level;
          ++level_counts[tmp_level];
          Q_PUSH(tmp_dst);
        }
      }
    }
  }
}
