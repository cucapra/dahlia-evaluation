
#include "xcl2.hpp"
#include <vector>
#include "bfs.h"
#include <string.h>
#include <unistd.h>

int INPUT_SIZE = sizeof(struct bench_args_t);
void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  cl_int err;
  // Copy the test data 
  std::vector<node_t,aligned_allocator<node_t>> nodes(N_NODES);
  std::vector<edge_t,aligned_allocator<edge_t>> edges(N_EDGES);
  std::vector<level_t,aligned_allocator<level_t>> level(N_NODES);
  std::vector<edge_index_t,aligned_allocator<edge_index_t>> level_counts(N_LEVELS);
  
  node_index_t starting_node = args->starting_node;

  for (int i=0; i < N_NODES; i++) {
          nodes[i]  = args->nodes[i];
          level[i]  = args->level[i];
        }
  for (int i=0; i<  N_EDGES; i++) edges[i] = args-> edges[i];

  // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err, std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err)); 

    // find_binary_file() is a utility API which will search the xclbin file for
    // targeted mode (sw_emu/hw_emu/hw) and for targeted platforms.
    std::string binaryFile = xcl::find_binary_file(device_name,"bfs");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_bfs_bulk(program,"bfs", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and 
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer nodes_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    N_NODES*sizeof(node_t),
                                    nodes.data(),
                                    &err));  
    OCL_CHECK(err,
              cl::Buffer edges_buffer(context, 
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    N_EDGES*sizeof(edge_t),
                                    edges.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer level_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    N_NODES*sizeof(level_t),
                                    level.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer level_counts_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    N_LEVELS*sizeof(edge_index_t),
                                    level_counts.data(),
                                    &err));
    
   
    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({nodes_buffer, edges_buffer, level_buffer},0/* 0 means from host*/));

    int size = 4096;
    OCL_CHECK(err, err = krnl_bfs_bulk.setArg(0, nodes_buffer));
    OCL_CHECK(err, err = krnl_bfs_bulk.setArg(1, edges_buffer));
    OCL_CHECK(err, err = krnl_bfs_bulk.setArg(2, starting_node));
    OCL_CHECK(err, err = krnl_bfs_bulk.setArg(3, level_buffer));
    OCL_CHECK(err, err = krnl_bfs_bulk.setArg(4, level_counts_buffer));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_bfs_bulk));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({level_counts_buffer},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  for (int i=0; i < N_LEVELS; i++) args->level_counts[i] = level_counts[i];
              
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
