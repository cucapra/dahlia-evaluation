# ORIGINAL
Resides in original/sort/radix

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `ss_sort` to `sort` in all three source files.

# BASELINE
Data zero copy pragma used for all 4 arguments.

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

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
