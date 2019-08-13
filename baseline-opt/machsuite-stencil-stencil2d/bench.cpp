#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  stencil( args->orig, args->sol, args->filter );
}
