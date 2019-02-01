#ifndef TOOLS_H
#define TOOLS_H

#include "sys/cdefs.h"
#include <stdint.h>

__BEGIN_DECLS

void ftoa(float, char*);

void dtoa(double, char*);

void readU16(uint16_t*, const uint8_t*);

void writeU16(uint16_t, uint8_t*);

void readU32(uint32_t*, const uint8_t*);

void writeU32(uint32_t, uint8_t*);

void writeR32(float, uint8_t*);

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define LOW_BYTE_U16(w) ((uint8_t)w)

#define HIGH_BYTE_U16(w) ((uint8_t)((w) >> 8))

typedef uint8_t BOOLEAN;

#ifndef TRUE
#define TRUE ((BOOLEAN)1)
#endif

#ifndef FALSE
#define FALSE ((BOOLEAN)0)
#endif

__END_DECLS

#endif /* TOOLS_H */

