/*
   Implemenataion based on:
   A. Danalis, G. Marin, C. McCurdy, J. S. Meredith, P. C. Roth, K. Spafford, V. Tipparaju, and J. S. Vetter.
   The scalable heterogeneous computing (shoc) benchmark suite.
   In Proceedings of the 3rd Workshop on General-Purpose Computation on Graphics Processing Units, 2010.
   */

#include "md.h"
extern "C" {
  void md(TYPE force_x[NATOMS],
      TYPE force_y[NATOMS],
      TYPE force_z[NATOMS],
      TYPE position_x[NATOMS],
      TYPE position_y[NATOMS],
      TYPE position_z[NATOMS],
      int32_t NL[NATOMS*MAXNEIGHBORS]) {

    #pragma HLS INTERFACE m_axi port=force_x offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi port=force_y offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi port=force_z offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi port=position_x offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi port=position_y offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi port=position_z offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi port=NL offset=slave bundle=gmem1
    #pragma HLS INTERFACE s_axilite port=force_x bundle=control
    #pragma HLS INTERFACE s_axilite port=force_y bundle=control
    #pragma HLS INTERFACE s_axilite port=force_z bundle=control
    #pragma HLS INTERFACE s_axilite port=position_x bundle=control
    #pragma HLS INTERFACE s_axilite port=position_y bundle=control
    #pragma HLS INTERFACE s_axilite port=position_z bundle=control
    #pragma HLS INTERFACE s_axilite port=NL bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    TYPE delx, dely, delz, r2inv;
    TYPE r6inv, potential, force, j_x, j_y, j_z;
    TYPE i_x, i_y, i_z, fx, fy, fz;

    int32_t i, j, jidx;

    for (i = 0; i < NATOMS; i++) {
      i_x = position_x[i];
      i_y = position_y[i];
      i_z = position_z[i];
      fx = 0; fy = 0; fz = 0;

      for( j = 0; j < MAXNEIGHBORS; j++) {
        // Get neighbor
        jidx = NL[i*MAXNEIGHBORS + j];
        // Look up x,y,z positions
        j_x = position_x[jidx];
        j_y = position_y[jidx];
        j_z = position_z[jidx];
        // Calc distance
        delx = i_x - j_x;
        dely = i_y - j_y;
        delz = i_z - j_z;
        r2inv = 1.0/( delx*delx + dely*dely + delz*delz );
        // Assume no cutoff and aways account for all nodes in area
        r6inv = r2inv * r2inv * r2inv;
        potential = r6inv*(LJ1*r6inv - LJ2);
        // Sum changes in force
        force = r2inv*potential;
        fx += delx * force;
        fy += dely * force;
        fz += delz * force;
      }
      //Update forces after all neighbors accounted for.
      force_x[i] = fx;
      force_y[i] = fy;
      force_z[i] = fz;
      //printf("dF=%lf,%lf,%lf\n", fx, fy, fz);
    }
  }
}
