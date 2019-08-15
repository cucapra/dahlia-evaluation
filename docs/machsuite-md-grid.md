# ORIGINAL
Resides in original/md/grid

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
#Data zero copy pragma used for `force`, will give better results or avoid timing failures.
Add tripcount to `grid1_x,y,z`,`loop_p` and `loop_q` for estimation.
x max: 3, min: 0, total: 160, number: 64, avg: 2
p max: 8, min: 0, total: 3964, number: 1000, avg: 3
q max: 8, min: 0, total: 15722, number: 3964, avg: 3
force_local local array is created to avoid #37.
Data copy pragma used for `n_points` to avoid #34.
Hardware function arguments are unpacked, `bench_args_t` struct is unpacked to avoid #43. 

# REWRITE
Comments correspond to those in baseline for the sake of readability.
Program uses a series of if statements instead of the `max` and `min` functions to better fit the style of fuse
Vector b0 is instead three integers to allow for easier access and rewriting of the x, y, and z values, so `b0.x` in the original is `b0x` in the rewrite.
Uses while loops for grid1 `for` loops as bounds are determined at runtime. 

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
