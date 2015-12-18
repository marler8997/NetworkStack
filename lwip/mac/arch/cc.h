#ifndef __ARCH_CC_H_
#define __ARCH_CC_H_

#include <stdio.h>
#include <inttypes.h>
#include <errno.h>

#define BYTE_ORDER LITTLE_ENDIAN

typedef intptr_t mem_ptr_t;

typedef uint8_t  u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;

typedef int8_t   s8_t;
typedef int16_t  s16_t;
typedef int32_t  s32_t;

/*
NOT WORKING FOR SOME REASON
#define S16_F PRId16
#define S32_F PRId32
#define U16_F PRIu16
#define U32_F PRIu32
#define X16_F PRIx16
#define X32_F PRIx32
#define SZT_F "zu"
*/
#define S16_F "d"
#define S32_F "d"
#define U16_F "u"
#define U32_F "u"
#define X16_F "x"
#define X32_F "x"
#define SZT_F "zu"

// Normally defined in stdlib.h, but that header
// file has too many things we don't want
void abort(void);

#define LWIP_PLATFORM_DIAG(x) do { printf x ; } while(0)

#define LWIP_PLATFORM_ASSERT(x) do { printf("Assertion %s failed at line %d in %s\n", x, __LINE__, __FILE__); fflush(NULL); abort(); } while(0)

#define LWIP_RAND() ((u32_t)rand())

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define ERR_UNKNOWN -17

#endif
