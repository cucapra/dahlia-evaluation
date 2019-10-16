#include "md.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((TYPE)1.0e-6)

/* Input format:
%% Section 1
TYPE[NATOMS]: x positions
%% Section 2
TYPE[NATOMS]: y positions
%% Section 3
TYPE[NATOMS]: z positions
%% Section 4
int32_t[NATOMS*MAXNEIGHBORS]: neighbor list
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->position_x, NATOMS);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->position_y, NATOMS);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->position_z, NATOMS);

  s = find_section_start(p,4);
  parse_int32_t_array(s, data->NL, NATOMS*MAXNEIGHBORS);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_x, NATOMS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_y, NATOMS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_z, NATOMS);

  write_section_header(fd);
  write_int32_t_array(fd, data->NL, NATOMS*MAXNEIGHBORS);

}

/* Output format:
%% Section 1
TYPE[NATOMS]: new x force
%% Section 2
TYPE[NATOMS]: new y force
%% Section 3
TYPE[NATOMS]: new z force
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->force_x, NATOMS);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->force_y, NATOMS);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->force_z, NATOMS);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_x, NATOMS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_y, NATOMS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_z, NATOMS);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;
  TYPE diff_x, diff_y, diff_z;

  for( i=0; i<NATOMS; i++ ) {
    diff_x = data->force_x[i] - ref->force_x[i];
    diff_y = data->force_y[i] - ref->force_y[i];
    diff_z = data->force_z[i] - ref->force_z[i];
    has_errors |= (diff_x<-EPSILON) || (EPSILON<diff_x);
    has_errors |= (diff_y<-EPSILON) || (EPSILON<diff_y);
    has_errors |= (diff_z<-EPSILON) || (EPSILON<diff_z);
  }

  // Return true if it's correct.
  return !has_errors;
}
