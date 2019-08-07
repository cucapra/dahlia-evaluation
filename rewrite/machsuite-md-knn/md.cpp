#include "ap_int.h"



void md(double force_x[256], double force_y[256], double force_z[256], double position_x[256], double position_y[256], double position_z[256], ap_int<32> nl[256][16]) {
  #pragma HLS INTERFACE s_axilite port=force_x
  #pragma HLS INTERFACE s_axilite port=force_y
  #pragma HLS INTERFACE s_axilite port=force_z
  #pragma HLS INTERFACE s_axilite port=position_x
  #pragma HLS INTERFACE s_axilite port=position_y
  #pragma HLS INTERFACE s_axilite port=position_z
  #pragma HLS INTERFACE s_axilite port=nl
  
  double lj1 = 1.5;
  
  double lj2 = 2.0;
  
  for(int i = 0; i < 256; i++) {
    double ix = position_x[i];
    
    double iy = position_y[i];
    
    double iz = position_z[i];
    
    //---
    double fx = 0.0;
    
    double fy = 0.0;
    
    double fz = 0.0;
    
    for(int j = 0; j < 16; j++) {
      ap_int<32> j_idx = nl[i][j];
      
      double jx = position_x[j_idx];
      
      double jy = position_y[j_idx];
      
      double jz = position_z[j_idx];
      
      double delx = (ix - jx);
      
      double dely = (iy - jy);
      
      double delz = (iz - jz);
      
      double r2inv = (1.0 / ((delx * delx) + ((dely * dely) + (delz * delz))));
      
      double r6inv = (r2inv * (r2inv * r2inv));
      
      double potential = (r6inv * ((lj1 * r6inv) - lj2));
      
      double force = (r2inv * potential);
      
      // combiner:
      fx += (delx * force);
      fy += (dely * force);
      fz += (delz * force);
    }
    force_x[i] = fx;
    force_y[i] = fy;
    force_z[i] = fz;
  }
}