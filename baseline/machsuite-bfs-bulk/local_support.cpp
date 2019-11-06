#include "xcl2.hpp"
#include <vector>
#include "bfs.h"
#include "helpers.hpp"
#include <string.h>
#include <unistd.h>

int INPUT_SIZE = sizeof(struct bench_args_t);
void run_benchmark( void *vargs, std::ofstream *runtime, int iter ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  cl_int err;

  // Copy the test data
  // OpenCL isn't happy with array of structs. Instead use several arrays.
  std::vector<edge_index_t,aligned_allocator<edge_index_t>> nodes_edge_begin(N_NODES);
  std::vector<edge_index_t,aligned_allocator<edge_index_t>> nodes_edge_end(N_NODES);
  std::vector<node_index_t,aligned_allocator<node_index_t>> edges_src(N_EDGES);
  std::vector<node_index_t,aligned_allocator<node_index_t>> edges_dst(N_EDGES);

  std::vector<level_t,aligned_allocator<level_t>> level(N_NODES);
  std::vector<edge_index_t,aligned_allocator<edge_index_t>> level_counts(N_LEVELS);

  for (int i=0; i < N_NODES; i++) {
    nodes_edge_begin[i]  = args->nodes[i].edge_begin;
    nodes_edge_end[i]  = args->nodes[i].edge_end;
    level[i]  = args->level[i];
  }
  for (int i=0; i<  N_EDGES; i++) {
    edges_src[i] = args->edges[i].src;
    edges_dst[i] = args->edges[i].dst;
  }


  std::vector<cl::Device> devices = xcl::get_xil_devices();
  cl::Device device = devices[0];

  OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
  OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
  cl::Kernel krnl_bfs_bulk = helpers::get_kernel(context, device, "bfs", err);

  OCL_CHECK(err,
            cl::Buffer nodes_edge_begin_buffer(context,
                                               CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                               N_NODES*sizeof(edge_index_t),
                                               nodes_edge_begin.data(),
                                               &err));
  OCL_CHECK(err,
            cl::Buffer nodes_edge_end_buffer(context,
                                             CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                             N_NODES*sizeof(edge_index_t),
                                             nodes_edge_end.data(),
                                             &err));

  OCL_CHECK(err,
            cl::Buffer edges_src_buffer(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                        N_EDGES*sizeof(node_index_t),
                                        edges_src.data(),
                                        &err));
  OCL_CHECK(err,
            cl::Buffer edges_dst_buffer(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                        N_EDGES*sizeof(node_index_t),
                                        edges_dst.data(),
                                        &err));

  OCL_CHECK(err,
            cl::Buffer level_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    N_NODES*sizeof(level_t),
                                    level.data(),
                                    &err));
  OCL_CHECK(err,
            cl::Buffer level_counts_buffer(context,
                                           CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                           N_LEVELS*sizeof(edge_index_t),
                                           level_counts.data(),
                                           &err));


  node_index_t starting_node = args->starting_node;
  // Copy input data to device global memory
  OCL_CHECK(err, err = krnl_bfs_bulk.setArg(0, nodes_edge_begin_buffer));
  OCL_CHECK(err, err = krnl_bfs_bulk.setArg(1, nodes_edge_end_buffer));

  OCL_CHECK(err, err = krnl_bfs_bulk.setArg(2, edges_src_buffer));
  OCL_CHECK(err, err = krnl_bfs_bulk.setArg(3, edges_dst_buffer));

  OCL_CHECK(err, err = krnl_bfs_bulk.setArg(4, starting_node));
  OCL_CHECK(err, err = krnl_bfs_bulk.setArg(5, level_buffer));
  OCL_CHECK(err, err = krnl_bfs_bulk.setArg(6, level_counts_buffer));

  // Init data

  OCL_CHECK(err,
            err = q.enqueueMigrateMemObjects({
                                              nodes_edge_begin_buffer,
                                              nodes_edge_end_buffer,
                                              edges_src_buffer,
                                              edges_dst_buffer,
                                              level_buffer}, 0));

  cl::Event event;
  uint64_t nstimestart, nstimeend;
  OCL_CHECK(err, err = q.enqueueTask(krnl_bfs_bulk, NULL, &event));

  OCL_CHECK(err,
            err = q.enqueueMigrateMemObjects({level_counts_buffer},CL_MIGRATE_MEM_OBJECT_HOST));
  q.finish();

  OCL_CHECK(err,
            err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START, &nstimestart));
  OCL_CHECK(err,
            err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END, &nstimeend));

  auto t = (nstimeend - nstimestart)/1000000.0;
  std::cout << "Iteration: " << iter << ": " << t << " ms." << std::endl;
  *runtime << iter << "," << t << std::endl;

  // Copy results
  for (int i=0; i < N_LEVELS; i++) {
    args->level_counts[i] = level_counts[i];
  }
}


/* Input format:
   %% Section 1
   uint64_t[1]: starting node
   %% Section 2
   uint64_t[N_NODES*2]: node structures (start and end indices of edge lists)
   %% Section 3
   uint64_t[N_EDGES]: edges structures (just destination node id)
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  uint64_t *nodes;
  uint64_t *edges;
  int64_t i;

  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Max-ify levels
  for(i=0; i<N_NODES; i++) {
    data->level[i]=MAX_LEVEL;
  }
  // Load input string
  p = readfile(fd);
  // Section 1: starting node
  s = find_section_start(p,1);
  parse_uint64_t_array(s, &data->starting_node, 1);

  // Section 2: node structures
  s = find_section_start(p,2);
  nodes = (uint64_t *)malloc(N_NODES*2*sizeof(uint64_t));
  parse_uint64_t_array(s, nodes, N_NODES*2);
  for(i=0; i<N_NODES; i++) {
    data->nodes[i].edge_begin = nodes[2*i];
    data->nodes[i].edge_end = nodes[2*i+1];
  }
  free(nodes);
  // Section 3: edge structures
  s = find_section_start(p,3);
  edges = (uint64_t *)malloc(N_EDGES*1*sizeof(uint64_t));
  parse_uint64_t_array(s, edges, N_EDGES*1);
  for(i=0; i<N_EDGES; i++) {
    data->edges[i].src = 0;
    data->edges[i].dst = edges[i];
  }
  free(edges);
  //parse_uint64_t_array(s, (uint64_t *)(data->edges), N_EDGES);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  uint64_t *nodes;
  uint64_t *edges;
  int64_t i;

  struct bench_args_t *data = (struct bench_args_t *)vdata;
  // Section 1: starting node
  write_section_header(fd);
  write_uint64_t_array(fd, &data->starting_node, 1);
  // Section 2: node structures
  write_section_header(fd);
  nodes = (uint64_t *)malloc(N_NODES*2*sizeof(uint64_t));
  for(i=0; i<N_NODES; i++) {
    nodes[2*i]  = data->nodes[i].edge_begin;
    nodes[2*i+1]= data->nodes[i].edge_end;
  }
  write_uint64_t_array(fd, nodes, N_NODES*2);
  free(nodes);
  // Section 3: edge structures
  write_section_header(fd);
  edges = (uint64_t *)malloc(N_EDGES*1*sizeof(uint64_t));
  for(i=0; i<N_EDGES; i++) {
    edges[i]  = data->edges[i].dst;
  }
  write_uint64_t_array(fd, edges, N_NODES*1);
  free(edges);
  //write_uint64_t_array(fd, (uint64_t *)(&data->edges), N_EDGES);
}

/* Output format:
   %% Section 1
   uint64_t[N_LEVELS]: horizon counts
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);
  // Section 1: horizon counts
  s = find_section_start(p,1);
  parse_uint64_t_array(s, data->level_counts, N_LEVELS);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  // Section 1
  write_section_header(fd);
  write_uint64_t_array(fd, data->level_counts, N_LEVELS);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;

  // Check that the horizons have the same number of nodes
  for(i=0; i<N_LEVELS; i++) {
    has_errors |= (data->level_counts[i]!=ref->level_counts[i]);
  }

  // Return true if it's correct.
  return !has_errors;
}
