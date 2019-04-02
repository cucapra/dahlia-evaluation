#ifndef LOCAL_SUPPORT_H
#define LOCAL_SUPPORT_H

#include <string.h>

#include "common/support.h"
#include "stencil_3d.h"

#define INPUT_SIZE sizeof(struct bench_args_t)

#define EPSILON (1.0e-6)

void run_benchmark(bench_args_t *args);
void input_to_data(int fd, bench_args_t *vdata);
void data_to_input(int fd, void *vdata);

void output_to_data(int fd, bench_args_t *data);
void data_to_output(int fd, void *vdata);

int check_data(bench_args_t *vdata, bench_args_t *vref);
#endif
