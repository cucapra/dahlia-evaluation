#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  viterbi( args->obs, args->init, args->transition, args->emission, args->path );
}
