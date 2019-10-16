/*
Implemenataion based on:
A. Danalis, G. Marin, C. McCurdy, J. S. Meredith, P. C. Roth, K. Spafford, V. Tipparaju, and J. S. Vetter.
The scalable heterogeneous computing (shoc) benchmark suite.
In Proceedings of the 3rd Workshop on General-Purpose Computation on Graphics Processing Units, 2010.
*/

#include <stdlib.h>
#include <stdio.h>
#include "support.h"

#define TYPE double

// Problem Constants
#define NATOMS        256
#define MAXNEIGHBORS  16
// LJ coefficients
#define LJ1           1.5
#define LJ2           2.0

////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
  TYPE force_x[NATOMS];
  TYPE force_y[NATOMS];
  TYPE force_z[NATOMS];
  TYPE position_x[NATOMS];
  TYPE position_y[NATOMS];
  TYPE position_z[NATOMS];
  int32_t NL[NATOMS*MAXNEIGHBORS];
};
