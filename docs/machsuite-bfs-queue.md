# ORIGINAL
Resides in original/bfs/queue

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for `level` and `level_counts`.
Add tripcount to `loop_neighbors` for estimation.
BRAM resource directives commented out as they break the flow. #35
A larger struct `edge_t` with a redundant field is used to avoid HLS error during SDSoC synthesis in generated CPP source in #39. 
Data copy pragma used for `nodes` to avoid #34.

# REWRITE
No change made

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
