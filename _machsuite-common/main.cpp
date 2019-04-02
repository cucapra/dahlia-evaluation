#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "local_support.h"
#ifdef __SDSCC__
#include "sds_lib.h"
#include "ap_int.h"
#else
#define sds_alloc(x) (malloc(x))
#define sds_free(x) (free(x))
#endif

#ifndef NUM_TESTS
#define NUM_TESTS 1024
#endif

int main(int argc, char **argv) {
  char *in_file;
  char *check_file;

  assert(argc == 3 && "Usage ./benchmark <input_file> <check_file>");
  in_file = argv[1];
  check_file = argv[2];

  // Load input data
  int in_fd;
  bench_args_t *data;
  data = (bench_args_t *) sds_alloc(INPUT_SIZE);
  assert (data != NULL && "Out of memory. Could not alloc space for input");

  in_fd = open(in_file, O_RDONLY);
  assert(in_fd > 0 && "Couldn't open input data file");
  input_to_data(in_fd, data);

  // Unpack and call hardware function
  run_benchmark(data);

  // Check the results
  int check_fd;
  bench_args_t *ref;
  ref = (bench_args_t *) malloc(INPUT_SIZE);
  assert (ref != NULL && "Out of memory. Could alloc space for input");

  check_fd = open(check_file, O_RDONLY);
  assert(check_fd > 0 && "Couldn't open input data file");
  output_to_data(check_fd, ref);

  int res = check_data(data, ref);
  if (res) {
    printf("Success.\n");
  } else {
    printf("Failed.\n");
  }

  sds_free(data);
  free(ref);

  return 0;
}
