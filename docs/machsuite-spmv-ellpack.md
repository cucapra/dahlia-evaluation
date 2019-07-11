# ORIGINAL
Resides in original/spmv/ellpack

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `ellpack` to `spmv` in all three source files.

# BASELINE
Data zero copy pragma used for `out`.
Data copy pragma used for `nzval` to avoid #34.

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

Add interface pragmas which are not supported by code generation.

All `bit<32>`s are manually changed to `int`s.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
