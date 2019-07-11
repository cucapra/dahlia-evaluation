# ORIGINAL
Resides in original/nw/nw

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `needwun` to `nw` in all three source files.

# BASELINE
Data zero copy pragma used for `M, ptr` arguments.
Add tripcount to all 8 loops naively for estimation (only adding for `trace` didn't work).
BRAM resource directives commented out as they break the flow. #35
Data copy pragma used for `SEQA` to avoid #34.

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

`ap_int<8>`s in the interface changed to `char`.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
