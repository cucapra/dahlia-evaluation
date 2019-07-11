# ORIGINAL
Resides in original/spmv/crs

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Add tripcount to `spmv_2` for estimation.
BRAM resource directives commented out as they break the flow. #35
Data copy pragma used for `val` to avoid #34.

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
