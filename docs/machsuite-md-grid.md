# ORIGINAL
Resides in original/md/grid

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
#Data zero copy pragma used for `force`, will give better results or avoid timing failures.
Add tripcount to `grid1_x,y,z`,`loop_p` and `loop_q` for estimation.
force_local local array is created to avoid #37.
Data copy pragma used for `n_points` to avoid #34.
Hardware function arguments are unpacked, `bench_args_t` struct is unpacked to avoid #43. 

# REWRITE
No change made

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
