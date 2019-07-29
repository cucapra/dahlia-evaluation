// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = unsigned int;
#endif

typedef struct {
  ap_int<32> x;
  ap_int<32> y;
  ap_int<32> z;
} ivector_t;
typedef struct {
  double x;
  double y;
  double z;
} dvector_t;
#pragma SDS data copy(n_points[0:64])
void md(ap_int<32> n_points[4][4][4]
, double force_x[4][4][4][10]
, double force_y[4][4][4][10]
, double force_z[4][4][4][10]
, double position_x[4][4][4][10]
, double position_y[4][4][4][10]
, double position_z[4][4][4][10]
) {
  
  dvector_t force_local[4][4][4][10]
  ;
  dvector_t empty = {
    .x = 0.0, .y = 0.0, .z = 0.0
  };
  for(int b0x = 0; b0x < 4; b0x++) {
    for(int b0y = 0; b0y < 4; b0y++) {
      for(int b0z = 0; b0z < 4; b0z++) {
        for(int p_idx = 0; p_idx < 10; p_idx++) {
          force_local[b0x][b0y][b0z][p_idx] = empty;
        }
      }
    }
  }
  //---
  for(int b0x = 0; b0x < 4; b0x++) {
    for(int b0y = 0; b0y < 4; b0y++) {
      for(int b0z = 0; b0z < 4; b0z++) {
        ivector_t b1min = {
          .x = 0, .y = 0, .z = 0
        };
        ivector_t b1max = {
          .x = 4, .y = 4, .z = 4
        };
        if(((b0x - 1) > b1min.x)){
          ivector_t newbmin = {
            .x = (b0x - 1), .y = b1min.y, .z = b1min.z
          };
          b1min = newbmin;
        } else{
          
        }
        if(((b0y - 1) > b1min.y)){
          ivector_t newbmin = {
            .x = b1min.x, .y = (b0y - 1), .z = b1min.z
          };
          b1min = newbmin;
        } else{
          
        }
        if(((b0z - 1) > b1min.z)){
          ivector_t newbmin = {
            .x = b1min.x, .y = b1min.y, .z = (b0z - 1)
          };
          b1min = newbmin;
        } else{
          
        }
        if(((b0x + 2) < b1max.x)){
          ivector_t newbmax = {
            .x = (b0x + 2), .y = b1max.y, .z = b1max.z
          };
          b1max = newbmax;
        } else{
          
        }
        if(((b0y + 2) < b1max.y)){
          ivector_t newbmax = {
            .x = b1max.x, .y = (b0y + 2), .z = b1max.z
          };
          b1max = newbmax;
        } else{
          
        }
        if(((b0z + 2) < b1max.z)){
          ivector_t newbmax = {
            .x = b1max.x, .y = b1max.y, .z = (b0z + 2)
          };
          b1max = newbmax;
        } else{
          
        }
        //---
        ap_int<32> b1x = b1min.x;
        while((b1x < b1max.x)) {
          #pragma HLS loop_tripcount max=4 min=0
          ap_int<32> b1y = b1min.y;
          while((b1y < b1max.y)) {
            #pragma HLS loop_tripcount max=4 min=0
            ap_int<32> b1z = b1min.z;
            while((b1z < b1max.z)) {
              #pragma HLS loop_tripcount max=4 min=0
              ap_int<32> q_idx_range = n_points[b1x][b1y][b1z];
              //---
              ap_int<32> p_idx = 0;
              ap_int<32> p_idx_upper = n_points[b0x][b0y][b0z];
              while((p_idx < p_idx_upper)) {
                #pragma HLS loop_tripcount avg=10
                dvector_t p = {
                  .x = position_x[b0x][b0y][b0z][p_idx], .y = position_y[b0x][b0y][b0z][p_idx], .z = position_z[b0x][b0y][b0z][p_idx]
                };
                //---
                double sum_x = force_local[b0x][b0y][b0z][p_idx].x;
                double sum_y = force_local[b0x][b0y][b0z][p_idx].y;
                double sum_z = force_local[b0x][b0y][b0z][p_idx].z;
                ap_int<32> q_idx = 0;
                while((q_idx < q_idx_range)) {
                  #pragma HLS loop_tripcount avg=10
                  dvector_t q = {
                    .x = position_x[b1x][b1y][b1z][q_idx], .y = position_y[b1x][b1y][b1z][q_idx], .z = position_z[b1x][b1y][b1z][q_idx]
                  };
                  if(((q.x != p.x) || ((q.y != p.y) || (q.z != p.z)))){
                    double dx = (p.x - q.x);
                    double dy = (p.y - q.y);
                    double dz = (p.z - q.z);
                    double r2inv = (1.0 / ((dx * dx) + ((dy * dy) + (dz * dz))));
                    double r6inv = (r2inv * (r2inv * r2inv));
                    double potential = (r6inv * ((1.5 * r6inv) - 2.0));
                    double f = (r2inv * potential);
                    sum_x = (sum_x + (f * dx));
                    sum_y = (sum_y + (f * dy));
                    sum_z = (sum_z + (f * dz));
                  } else{
                    
                  }
                  q_idx = (q_idx + 1);
                }
                //---
                dvector_t newforcelocal = {
                  .x = sum_x, .y = sum_y, .z = sum_z
                };
                force_local[b0x][b0y][b0z][p_idx] = newforcelocal;
                p_idx = (p_idx + 1);
              }
              b1z = (b1z + 1);
            }
            b1y = (b1y + 1);
          }
          b1x = (b1x + 1);
        }
      }
    }
  }
  //---
  for(int b0x = 0; b0x < 4; b0x++) {
    for(int b0y = 0; b0y < 4; b0y++) {
      for(int b0z = 0; b0z < 4; b0z++) {
        for(int p_idx = 0; p_idx < 10; p_idx++) {
          force_x[b0x][b0y][b0z][p_idx] = force_local[b0x][b0y][b0z][p_idx].x;
          force_y[b0x][b0y][b0z][p_idx] = force_local[b0x][b0y][b0z][p_idx].y;
          force_z[b0x][b0y][b0z][p_idx] = force_local[b0x][b0y][b0z][p_idx].z;
        }
      }
    }
  }
}
