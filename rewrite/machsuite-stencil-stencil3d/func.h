#include "stencil.h"

#pragma SDS data copy(C[0:2])
void stencil( TYPE C[2], TYPE orig[height_size][col_size][row_size], TYPE sol[height_size][col_size][row_size] );
