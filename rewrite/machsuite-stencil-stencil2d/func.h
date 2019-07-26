#include "stencil.h"

#pragma SDS data copy(orig[0:128][0:64])
void stencil( TYPE orig[row_size][col_size],
        TYPE sol[row_size][col_size],
        TYPE filter[3][3] );
