#include "md.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((TYPE)1.0e-6)

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  md( args->n_points, args->force_x, args->force_y, args->force_z, args->position_x, args->position_y, args->position_z );
}

/* Input format:
%% Section 1
int32_t[blockSide^3]: grid populations
%% Section 2
TYPE[blockSide^3*densityFactor]: positions
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  dvector_t *position;

  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_int32_t_array(s, (int32_t *)(data->n_points), blockSide*blockSide*blockSide);

  s = find_section_start(p,2);
  position = (dvector_t *)malloc(nBlocks*densityFactor*sizeof(dvector_t));
  STAC(parse_,TYPE,_array)(s, (double *)(position), 3*blockSide*blockSide*blockSide*densityFactor);
  for(int i = 0; i < blockSide; i++) {
  for(int j = 0; j < blockSide; j++) {
  for(int k = 0; k < blockSide; k++) {
    for(int l = 0; l < densityFactor; l++) {
        data->position_x[i][j][k][l] = position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].x;
        data->position_y[i][j][k][l] = position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].y;
        data->position_z[i][j][k][l] = position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].z;
    }
  }}}
  free(position);
  //STAC(parse_,TYPE,_array)(s, (double *)(data->position), 3*blockSide*blockSide*blockSide*densityFactor);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  dvector_t *position;

  write_section_header(fd);
  write_int32_t_array(fd, (int32_t *)(data->n_points), blockSide*blockSide*blockSide);

  write_section_header(fd);
  position = (dvector_t *)malloc(nBlocks*densityFactor*sizeof(dvector_t));
  for(int i = 0; i < blockSide; i++) {
  for(int j = 0; j < blockSide; j++) {
  for(int k = 0; k < blockSide; k++) {
    for(int l = 0; l < densityFactor; l++) {
        position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].x = data->position_x[i][j][k][l];
        position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].y = data->position_y[i][j][k][l];
        position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].z = data->position_z[i][j][k][l];
    }
  }}}
  STAC(write_,TYPE,_array)(fd, (double *)(position), 3*blockSide*blockSide*blockSide*densityFactor);
//  STAC(write_,TYPE,_array)(fd, (double *)(data->position), 3*blockSide*blockSide*blockSide*densityFactor);
  free(position);

}

/* Output format:
%% Section 1
TYPE[blockSide^3*densityFactor]: force
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  dvector_t *force;

  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  force = (dvector_t *)malloc(nBlocks*densityFactor*sizeof(dvector_t));
  STAC(parse_,TYPE,_array)(s, (double *)force, 3*blockSide*blockSide*blockSide*densityFactor);
//  STAC(parse_,TYPE,_array)(s, (double *)data->force, 3*blockSide*blockSide*blockSide*densityFactor);
  for(int i = 0; i < blockSide; i++) {
  for(int j = 0; j < blockSide; j++) {
  for(int k = 0; k < blockSide; k++) {
    for(int l = 0; l < densityFactor; l++) {
        data->force_x[i][j][k][l] = force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].x; 
        data->force_y[i][j][k][l] = force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].y; 
        data->force_z[i][j][k][l] = force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].z; 
    }
  }}}
  free(force);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  dvector_t *force;

  write_section_header(fd);
  force = (dvector_t *)malloc(nBlocks*densityFactor*sizeof(dvector_t));
  for(int i = 0; i < blockSide; i++) {
  for(int j = 0; j < blockSide; j++) {
  for(int k = 0; k < blockSide; k++) {
    for(int l = 0; l < densityFactor; l++) {
        force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].x = data->force_x[i][j][k][l]; 
        force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].y = data->force_y[i][j][k][l]; 
        force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].z = data->force_z[i][j][k][l]; 
    }
  }}}
  STAC(write_,TYPE,_array)(fd, (double *)force, 3*blockSide*blockSide*blockSide*densityFactor);
  free(force);
  //STAC(write_,TYPE,_array)(fd, (double *)data->force, 3*blockSide*blockSide*blockSide*densityFactor);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i, j, k, d;
  TYPE diff_x, diff_y, diff_z;

  for(i=0; i<blockSide; i++) {
    for(j=0; j<blockSide; j++) {
      for(k=0; k<blockSide; k++) {
        for(d=0; d<densityFactor; d++) {
          diff_x = data->force_x[i][j][k][d] - ref->force_x[i][j][k][d];
          diff_y = data->force_y[i][j][k][d] - ref->force_y[i][j][k][d];
          diff_z = data->force_z[i][j][k][d] - ref->force_z[i][j][k][d];
          has_errors |= (diff_x<-EPSILON) || (EPSILON<diff_x);
          has_errors |= (diff_y<-EPSILON) || (EPSILON<diff_y);
          has_errors |= (diff_z<-EPSILON) || (EPSILON<diff_z);
        }
      }
    }
  }

  // Return true if it's correct.
  return !has_errors;
}
