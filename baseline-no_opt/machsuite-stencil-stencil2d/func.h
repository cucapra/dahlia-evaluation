#include "stencil.h"

#pragma SDS data copy(orig[0:row_size*col_size])
void stencil( TYPE orig[row_size * col_size],
        TYPE sol[row_size * col_size],
        TYPE filter[f_size] );
