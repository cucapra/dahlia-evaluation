#include "md.h"

#define MIN(x,y) ( (x)<(y) ? (x) : (y) )
#define MAX(x,y) ( (x)>(y) ? (x) : (y) )

void md( int32_t n_points[blockSide][blockSide][blockSide],
         TYPE force_x[blockSide][blockSide][blockSide][densityFactor],
         TYPE force_y[blockSide][blockSide][blockSide][densityFactor],
         TYPE force_z[blockSide][blockSide][blockSide][densityFactor],
         TYPE position_x[blockSide][blockSide][blockSide][densityFactor],
         TYPE position_y[blockSide][blockSide][blockSide][densityFactor],
         TYPE position_z[blockSide][blockSide][blockSide][densityFactor] )
{
#pragma HLS INTERFACE s_axilite port=n_points
#pragma HLS INTERFACE s_axilite port=force_x
#pragma HLS INTERFACE s_axilite port=force_y
#pragma HLS INTERFACE s_axilite port=force_z
#pragma HLS INTERFACE s_axilite port=position_x
#pragma HLS INTERFACE s_axilite port=position_y
#pragma HLS INTERFACE s_axilite port=position_z

  ivector_t b0, b1; // b0 is the current block, b1 is b0 or a neighboring block
  dvector_t p, q; // p is a point in b0, q is a point in either b0 or b1
  int32_t p_idx, q_idx;
  TYPE dx, dy, dz, r2inv, r6inv, potential, f;

  dvector_t force_local[blockSide][blockSide][blockSide][densityFactor];

  loop_init_x: for( b0.x=0; b0.x<blockSide; b0.x++ ) {
  loop_init_y: for( b0.y=0; b0.y<blockSide; b0.y++ ) {
  loop_init_z: for( b0.z=0; b0.z<blockSide; b0.z++ ) {
    loop_init_p: for( p_idx=0; p_idx<densityFactor; p_idx++ ) {
      force_local[b0.x][b0.y][b0.z][p_idx].x = 0;
      force_local[b0.x][b0.y][b0.z][p_idx].y = 0;
      force_local[b0.x][b0.y][b0.z][p_idx].z = 0;
    }
  }}}

  // Iterate over the grid, block by block
  loop_grid0_x: for( b0.x=0; b0.x<blockSide; b0.x++ ) {
  loop_grid0_y: for( b0.y=0; b0.y<blockSide; b0.y++ ) {
  loop_grid0_z: for( b0.z=0; b0.z<blockSide; b0.z++ ) {
  // Iterate over the 3x3x3 (modulo boundary conditions) cube of blocks around b0
  loop_grid1_x: for( b1.x=MAX(0,b0.x-1); b1.x<MIN(blockSide,b0.x+2); b1.x++ ) {
  loop_grid1_y: for( b1.y=MAX(0,b0.y-1); b1.y<MIN(blockSide,b0.y+2); b1.y++ ) {
  loop_grid1_z: for( b1.z=MAX(0,b0.z-1); b1.z<MIN(blockSide,b0.z+2); b1.z++ ) {
    // For all points in b0
    TYPE *base_q_x = position_x[b1.x][b1.y][b1.z];
    TYPE *base_q_y = position_y[b1.x][b1.y][b1.z];
    TYPE *base_q_z = position_z[b1.x][b1.y][b1.z];
    int q_idx_range = n_points[b1.x][b1.y][b1.z];
    loop_p: for( p_idx=0; p_idx<n_points[b0.x][b0.y][b0.z]; p_idx++ ) {
      p.x = position_x[b0.x][b0.y][b0.z][p_idx];
      p.y = position_y[b0.x][b0.y][b0.z][p_idx];
      p.z = position_z[b0.x][b0.y][b0.z][p_idx];
      TYPE sum_x = force_local[b0.x][b0.y][b0.z][p_idx].x;
      TYPE sum_y = force_local[b0.x][b0.y][b0.z][p_idx].y;
      TYPE sum_z = force_local[b0.x][b0.y][b0.z][p_idx].z;
      // For all points in b1
      loop_q: for( q_idx=0; q_idx< q_idx_range ; q_idx++ ) {
        q.x = *(base_q_x + q_idx);
        q.y = *(base_q_y + q_idx);
        q.z = *(base_q_z + q_idx);

        // Don't compute our own
        if( q.x!=p.x || q.y!=p.y || q.z!=p.z ) {
          // Compute the LJ-potential
          dx = p.x - q.x;
          dy = p.y - q.y;
          dz = p.z - q.z;
          r2inv = 1.0/( dx*dx + dy*dy + dz*dz );
          r6inv = r2inv*r2inv*r2inv;
          potential = r6inv*(lj1*r6inv - lj2);
          // Update forces
          f = r2inv*potential;
          sum_x += f*dx;
          sum_y += f*dy;
          sum_z += f*dz;
        }
      } // loop_q
      force_local[b0.x][b0.y][b0.z][p_idx].x = sum_x ;
      force_local[b0.x][b0.y][b0.z][p_idx].y = sum_y ;
      force_local[b0.x][b0.y][b0.z][p_idx].z = sum_z ;
    } // loop_p
  }}} // loop_grid1_*
  }}} // loop_grid0_*


  loop_copy_x: for( b0.x=0; b0.x<blockSide; b0.x++ ) {
  loop_copy_y: for( b0.y=0; b0.y<blockSide; b0.y++ ) {
  loop_copy_z: for( b0.z=0; b0.z<blockSide; b0.z++ ) {
    loop_copy_p: for( p_idx=0; p_idx<densityFactor; p_idx++ ) {
      force_x[b0.x][b0.y][b0.z][p_idx] = force_local[b0.x][b0.y][b0.z][p_idx].x ;
      force_y[b0.x][b0.y][b0.z][p_idx] = force_local[b0.x][b0.y][b0.z][p_idx].y ;
      force_z[b0.x][b0.y][b0.z][p_idx] = force_local[b0.x][b0.y][b0.z][p_idx].z ;
    }
  }}}
}
