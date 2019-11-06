#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <inttypes.h>
#include <fstream>
#include <iostream>

#define WRITE_OUTPUT
#define CHECK_OUTPUT

#include "support.h"

int main(int argc, char **argv)
{
  // Parse command line.
  char const *in_file;
  #ifdef CHECK_OUTPUT
  char const *check_file;
  #endif
  assert( argc<4 && "Usage: ./benchmark <input_file> <check_file>" );
  in_file = "input.data";
  #ifdef CHECK_OUTPUT
  check_file = "check.data";
  #endif
  if( argc>1 )
    in_file = argv[1];
  #ifdef CHECK_OUTPUT
  if( argc>2 )
    check_file = argv[2];
  #endif

  std::ofstream runtime("runtime.log");
  if (!runtime.is_open()) {
    std::cout << "Error: Failed to open output file.";
    exit(-1);
  }

  // Load input data
  for (auto i = 0; i < 10; i++) {
    int in_fd;
    char *data;
    data = static_cast<char*>(malloc(INPUT_SIZE));
    assert( data!=NULL && "Out of memory" );
    in_fd = open( in_file, O_RDONLY );
    assert( in_fd>0 && "Couldn't open input data file");
    input_to_data(in_fd, data);

    // struct perf_counter_t sw_ctr;
    // perf_reset(&sw_ctr);
    // perf_start(&sw_ctr);
    run_benchmark( data, &runtime, i );
    // perf_stop(&sw_ctr);
    // uint64_t sw_cycles = perf_avg_cpu_cycles(&sw_ctr); // need https://stackoverflow.com/questions/9225567/how-to-print-a-int64-t-type-in-c
    // printf("Average number of CPU cycles running baseline for benchmark: %" PRIu64 "\n", sw_cycles );

#ifdef WRITE_OUTPUT
    int out_fd;
    out_fd = open("output.data", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    assert( out_fd>0 && "Couldn't open output data file" );
    data_to_output(out_fd, data);
    close(out_fd);
#endif

    // Load check data
#ifdef CHECK_OUTPUT
    int check_fd;
    char *ref;
    ref = static_cast<char*>(malloc(INPUT_SIZE));
    assert( ref!=NULL && "Out of memory" );
    check_fd = open( check_file, O_RDONLY );
    assert( check_fd>0 && "Couldn't open check data file");
    output_to_data(check_fd, ref);
#endif

    // Validate benchmark results
#ifdef CHECK_OUTPUT
    if( !check_data(data, ref)) {
      fprintf(stderr, "Benchmark results are incorrect\n");
      return -1;
    }
#endif
    free(data);
    free(ref);
  }

  printf("Success.\n");
  return 0;
}
