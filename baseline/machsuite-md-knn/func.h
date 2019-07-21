#include "md.h"

void md(TYPE force_x[nAtoms],
        TYPE force_y[nAtoms],
        TYPE force_z[nAtoms],
        TYPE position_x[nAtoms],
        TYPE position_y[nAtoms],
        TYPE position_z[nAtoms],
        int32_t NL[nAtoms*maxNeighbors]);
