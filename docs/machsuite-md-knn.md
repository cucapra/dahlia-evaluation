# ORIGINAL
Resides in original/MD/KNN

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `md_kernel` to `md` in all three source files.

# BASELINE
No change made

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

`bit<32>`s are manually changed to `int`s.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
