#include <stdint.h>
#include <string.h>

#include "tools.h"

#define FLOAT_DIGITS10 7

BOOLEAN
tools_isAlpha(uint8_t c)
{
	/* cp866 alpha symbols */
	return (c >= 224 && c <= 247) ||
	       (c >= 128 && c <= 175) ||
	       (c >= 65 && c <= 90) ||
	       (c >= 97 && c <= 122);
}

void
_ftoa(float f, char *buf)
{
	if (f != f) {
		strcpy(buf, "NAN");
		return;
	} else if (f + f == f) {
		if (1.f / f < 0.f)
			strcpy(buf, "-0.0");
		else if (f == 0.0f)
			strcpy(buf, "0.0");
		else if (f > 0.0f)
			strcpy(buf, "INF");
		else
			strcpy(buf, "-INF");
		return;
	}

	if (f < 0.f) {
		f = -f;
		*(buf++) = '-';
	}

	int32_t n = 0;

	if (f < 1e6f)
		do {
			f *= 10.f;
			--n;
		} while (f < (1e6f - 0.5f));
	else if (f >= 1e7f)
		do {
			f /= 10.f;
			++n;
		} while (f >= 1e7f);
	n += FLOAT_DIGITS10 - 1;
	uint32_t fi = f + 0.5f;

	uint8_t i;
	for (i = FLOAT_DIGITS10; i > 1; --i) {
		buf[i] = fi % 10 + '0';
		fi /= 10;
	}
	buf[1] = '.';
	buf[0] = fi % 10 + '0';
	buf += FLOAT_DIGITS10 + 1;
	if (n != 0) {
		*(buf++) = 'e';
		if (n < 0) {
			*(buf++) = '-';
			n = -n;
		}
		if (n >= 10)
			*(buf++) = n / 10 + '0';
		*(buf++) = n % 10 + '0';
	}

	*buf = '\0';
}

union U16 {
	uint16_t num;
	char bytes[sizeof(uint16_t)];
};

union U32 {
	uint32_t num;
	char bytes[sizeof(uint32_t)];
};

union R32 {
	float num;
	char bytes[sizeof(float)];
};

void
readU16(uint16_t *num, const uint8_t *buf)
{
	union U16 s;
	
	s.bytes[1] = *(buf++);
	s.bytes[0] = *buf;
	
	*num = s.num;
}

void
writeU16(uint16_t num, uint8_t *buf)
{
	union U16 s;
	
	s.num = num;
	
	*(buf++) = s.bytes[1];
	*buf = s.bytes[0];
}

void
readU32(uint32_t *num, const uint8_t *buf)
{
	union U32 s;
	
	s.bytes[3] = *(buf++);
	s.bytes[2] = *(buf++);
	s.bytes[1] = *(buf++);
	s.bytes[0] = *buf;
	
	*num = s.num;
}

void
writeU32(uint32_t num, uint8_t *buf)
{
	union U32 s;
	
	s.num = num;
	
	*(buf++) = s.bytes[3];
	*(buf++) = s.bytes[2];
	*(buf++) = s.bytes[1];
	*buf = s.bytes[0];
}


void
readR32(float *num, const const uint8_t *buf)
{
	union R32 s;
	
	s.bytes[3] = *(buf++);
	s.bytes[2] = *(buf++);
	s.bytes[1] = *(buf++);
	s.bytes[0] = *buf;
	
	*num = s.num;
}

void
writeR32(float num, uint8_t *buf)
{
	union R32 s;
	
	s.num = num;
	
	*(buf++) = s.bytes[3];
	*(buf++) = s.bytes[2];
	*(buf++) = s.bytes[1];
	*buf = s.bytes[0];
}