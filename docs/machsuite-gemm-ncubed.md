# ORIGINAL
Resides in original/gemm/ncubed

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data copy pragma used for `m1` to avoid #34.

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

`bit<32>`s are replaced with `int`s.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
