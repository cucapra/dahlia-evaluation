// #if defined(__SDSCC__) || defined(VHLS) || defined(SDACCEL)

// In "hardware" mode (i.e., SDSoC compilation), use the real/provided ap_int
// library.

// #include <ap_int.h>

// #else

// In "software" mode (i.e., a normal C++ compiler), use standard int types.
// Thanks to Matthew Milano for his help figuring out how to do this.

#include <stdint.h>

// ap_int

template <int N>
struct _int_hack {};

template <>
struct _int_hack<1> { using t = signed char; };

template <>
struct _int_hack<2> { using t = int8_t; };
template <>
struct _int_hack<3> { using t = int8_t; };
template <>
struct _int_hack<4> { using t = int8_t; };
template <>
struct _int_hack<5> { using t = int8_t; };
template <>
struct _int_hack<6> { using t = int8_t; };
template <>
struct _int_hack<7> { using t = int8_t; };
template <>
struct _int_hack<8> { using t = int8_t; };

template <>
struct _int_hack<16> { using t = int16_t; };

template <>
struct _int_hack<32> { using t = int32_t; };

template <>
struct _int_hack<64> { using t = int64_t; };

template <int N>
using ap_int = typename _int_hack<N>::t;

// ap_uint

template <int N>
struct _uint_hack {};

template <>
struct _uint_hack<1> { using t = unsigned char; };

template <>
struct _uint_hack<8> { using t = uint8_t; };

template <>
struct _uint_hack<16> { using t = uint16_t; };

template <>
struct _uint_hack<32> { using t = uint32_t; };

template <>
struct _uint_hack<64> { using t = uint64_t; };

template <int N>
using ap_uint = typename _uint_hack<N>::t;

// #endif
