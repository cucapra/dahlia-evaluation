# ORIGINAL
Resides in original/viterbi/viterbi

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for path argument.
Data copy pragma used for `obs` to avoid #34.

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

Added 
```
#include <inttypes.h>
```
to use `uint8_t` in interface

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
