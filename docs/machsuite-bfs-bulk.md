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
After compiling the `.fuse` file
```
#ifdef __SDSCC__
#include "ap_int.h"
#else
template < int N >
using ap_int = int;
#endif
```
has to be added to the top of the generated cpp file for `ap_int` to be recognized.

Add interface pragmas and loop count pragmas which are not supported by code generation.

- `struct` fields must be `uint64_t`; the data alignment doesn't seem to work unless this is the case.

Added 
```
#include <inttypes.h>
```
to use `uint64_t` in interface

- In `bfs.h`, for the BFS node/edge `struct`s, the "names" of the `struct`s are deleted. In other words, `typedef struct X { ... } X_t;` is changed to `typedef struct { ... } X_t;` This is so the Fuse Vivado backend can generate record types that match this; the Vivado backend won't generate C++ code that has "named" `struct`s." (Fuse record types are just compiled to definitions of the form `typedef struct { ... } X_t;`.)

- `bit<32>`s are manually replaced with `int`s.
- I manually changed some type annotations to `unsigned long long` and `signed char` to get the types to match with `bfs.h`.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
