#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  TYPE (*orig_l)[col_size][row_size] = (TYPE (*)[col_size][row_size])&args->orig[0];
  TYPE (*sol_l )[col_size][row_size] = (TYPE (*)[col_size][row_size])&args->sol[0];
  stencil( args->C, orig_l, sol_l );
}

