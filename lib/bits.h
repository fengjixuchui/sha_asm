#ifndef BITS_H
#define BITS_H

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LITTLE_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define BIG_ENDIAN
#else
#error Byte order not supported
#endif

#define bswap32(x) \
(x >> 24 | \
 x >> 8  & 0xff00 | \
 x << 8  & 0xff0000 | \
 x << 24 & 0xff000000)

#define bswap64(x) \
(x >> 56 | \
 x >> 40 & 0xff00 | \
 x >> 24 & 0xff0000 | \
 x >> 8  & 0xff000000 | \
 x << 8  & 0xff00000000 | \
 x << 24 & 0xff0000000000 | \
 x << 40 & 0xff000000000000 | \
 x << 56 & 0xff00000000000000)

/* rotate a 32-bit val by n bits left */
#define rotl32(val, n) (val << n | val >> 32 - n)

#endif
