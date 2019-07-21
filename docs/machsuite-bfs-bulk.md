# ORIGINAL
Resides in original/bfs/bulk

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for `level`.
Add tripcount to `loop_neighbors` for estimation.
BRAM resource directives commented out as they break the flow. #35
A larger struct `edge_t` with a redundant field is used to avoid HLS error during SDSoC synthesis in generated CPP source in #39. 
Data copy pragma used for `nodes` to avoid #34.

# REWRITE
For `bit<8>` and `ubit<64>`, `ap_int<8>` and `unsigned int` are manually changed to `char` and `uint64_t` to avoid interface conflicts in software.

Added 
```
#include <inttypes.h>
```

In `bfs.h`, for the BFS node/edge `struct`s, the "names" of the `struct`s are deleted. In other words, `typedef struct X { ... } X_t;` is changed to `typedef struct { ... } X_t;` This is so the Fuse Vivado backend can generate record types that match this; the Vivado backend won't generate C++ code that has "named" `struct`s." (Fuse record types are just compiled to definitions of the form `typedef struct { ... } X_t;`.)

Internal `for` loop changed to `while` because starting point is dynamically determined.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
