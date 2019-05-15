# ORIGINAL
Resides in original/md/grid

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for `force`.
Add tripcount to `grid1_x,y,z`,`loop_p` and `loop_q` for estimation.
force_local local array is created to avoid #37.

Currently the estimation works but HLS error during SDSoC synthesis in #43
```
md.cpp: In function 'void _p0_md_1_noasync(int (*)[4][4], dvector_t (*)[4][4][10], dvector_t (*)[4][4][10])':
md.cpp:97:60: error: request for member 'x' in '* position', which is of non-class type 'dvector_t [4][4][10]'
   cf_send_i(&(_p0_swinst_md_1.position_x_PORTA), position->x, 8388608, &_p0_request_4);
                                                            ^
md.cpp:98:60: error: request for member 'y' in '* position', which is of non-class type 'dvector_t [4][4][10]'
   cf_send_i(&(_p0_swinst_md_1.position_y_PORTA), position->y, 8388608, &_p0_request_5);
                                                            ^
md.cpp:99:60: error: request for member 'z' in '* position', which is of non-class type 'dvector_t [4][4][10]'
   cf_send_i(&(_p0_swinst_md_1.position_z_PORTA), position->z, 8388608, &_p0_request_6);
                                                            ^
md.cpp:101:57: error: request for member 'x' in '* force', which is of non-class type 'dvector_t [4][4][10]'
   cf_receive_i(&(_p0_swinst_md_1.force_x_PORTA), force->x, 8388608, &_p0_md_1_noasync_num_force_x_PORTA, &_p0_request_1);
                                                         ^
md.cpp:102:57: error: request for member 'y' in '* force', which is of non-class type 'dvector_t [4][4][10]'
   cf_receive_i(&(_p0_swinst_md_1.force_y_PORTA), force->y, 8388608, &_p0_md_1_noasync_num_force_y_PORTA, &_p0_request_2);
                                                         ^
md.cpp:103:57: error: request for member 'z' in '* force', which is of non-class type 'dvector_t [4][4][10]'
   cf_receive_i(&(_p0_swinst_md_1.force_z_PORTA), force->z, 8388608, &_p0_md_1_noasync_num_force_z_PORTA, &_p0_request_3);
```

# REWRITE
No change made

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
