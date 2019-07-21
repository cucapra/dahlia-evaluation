#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  md( args->force_x, args->force_y, args->force_z,
      args->position_x, args->position_y, args->position_z,
      args->NL );
}
