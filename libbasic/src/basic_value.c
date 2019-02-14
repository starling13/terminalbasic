#include "basic_value.h"

#include <math.h>

#if USE_REALS
real_t
basic_value_toReal(const basic_value_t *self)
{
	switch (self->type) {
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		return (real_t)(self->body.long_integer);
#endif // USE_LONGINT
	case BASIC_VALUE_TYPE_INTEGER:
		return (real_t)(self->body.integer);
	case BASIC_VALUE_TYPE_REAL:
		return self->body.real;
	case BASIC_VALUE_TYPE_LOGICAL:
		return (real_t)(self->body.logical);
	default:
		return (real_t)(NAN);
	}
}

basic_value_t
basic_value_from_real(real_t other)
{
	basic_value_t result;
	
	basic_value_setFromReal(&result, other);
	
	return result;
}

void
basic_value_setFromReal(basic_value_t* self, real_t val)
{
	self->type = BASIC_VALUE_TYPE_REAL;
	self->body.real = val;
}
#endif // USE_REALS

#if USE_LONGINT
long_integer_t
basic_value_toLongInteger(const basic_value_t* self)
{
	switch (self->type) {
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		return self->body.long_integer;
	case BASIC_VALUE_TYPE_INTEGER:
		return (long_integer_t)self->body.integer;
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL:
		return (long_integer_t)self->body.real;
#endif // USE_REALS
	case BASIC_VALUE_TYPE_LOGICAL:
		return (BOOLEAN)self->body.logical;
	default:
		return (long_integer_t)0;
	}
}

basic_value_t
basic_value_fromLongInteger(long_integer_t other)
{
	basic_value_t result;
	
	basic_value_setFromLongInteger(&result, other);
	
	return result;
}

void
basic_value_setFromLongInteger(basic_value_t* self, long_integer_t other)
{
	self->type = BASIC_VALUE_TYPE_LONG_INTEGER;
	self->body.long_integer = other;
}
#endif // USE_LONGINT

integer_t
basic_value_toInteger(const basic_value_t* self)
{
	switch (self->type) {
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		return (integer_t)(self->body.long_integer);
#endif // USE_LONGINT
	case BASIC_VALUE_TYPE_INTEGER:
		return self->body.integer;
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL:
		return (integer_t)(self->body.real);
#endif // USE_REALS
	case BASIC_VALUE_TYPE_LOGICAL:
		return (integer_t)(self->body.logical);
	default:
		return (integer_t)(0);
	}
}

basic_value_t
basic_value_fromInteger(integer_t other)
{
	basic_value_t result;
	
	basic_value_setFromInteger(&result, other);
	
	return result;
}

void
basic_value_setFromInteger(basic_value_t* self, integer_t val)
{
	self->type = BASIC_VALUE_TYPE_INTEGER;
	self->body.integer = val;
}

BOOLEAN
basic_value_toLogical(const basic_value_t* self)
{
	switch (self->type) {
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		return (BOOLEAN)(self->body.long_integer);
#endif // USE_LONGINT
	case BASIC_VALUE_TYPE_INTEGER:
		return (BOOLEAN)(self->body.integer);
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL:
		return (BOOLEAN)(self->body.real);
#endif // USE_REALS
	case BASIC_VALUE_TYPE_LOGICAL:
		return self->body.logical;
	default:
		return FALSE;
	}
}

void
basic_value_setFromLogical(basic_value_t* self, BOOLEAN val)
{
	self->type = BASIC_VALUE_TYPE_LOGICAL;
	self->body.logical = val;
}

basic_value_t
basic_value_fromLogical(BOOLEAN val)
{
	basic_value_t result;
	
	basic_value_setFromLogical(&result, val);
	
	return result;
}

void
basic_value_minuseq(basic_value_t *self, const basic_value_t *rhs)
{
#if USE_REALS
	if (rhs->type == BASIC_VALUE_TYPE_REAL)
		basic_value_setFromReal(self, basic_value_toReal(self)-
		    basic_value_toReal(rhs));
	else
#endif
	switch (self->type) {
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL: self->body.real -=
	    basic_value_toReal(rhs);
		break;
#endif
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER: self->body.long_integer -=
	     basic_value_toLongInteger(rhs);
		break;
#endif
	case BASIC_VALUE_TYPE_INTEGER: self->body.integer -=
	    basic_value_toInteger(rhs);
		break;
	default:
		break;
	}

}

void
basic_value_pluseq(basic_value_t *self, const basic_value_t *rhs)
{
#if USE_REALS
	if (rhs->type == BASIC_VALUE_TYPE_REAL)
		basic_value_setFromReal(self, basic_value_toReal(self)+
		    rhs->body.real);
	else
#endif
	switch (self->type) {
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL:
		self->body.real += basic_value_toReal(rhs);
		break;
#endif
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		self->body.long_integer += basic_value_toLongInteger(rhs);
		break;
#endif
	case BASIC_VALUE_TYPE_INTEGER:
		self->body.integer += basic_value_toInteger(rhs);
		break;
	default:
		break;
	}
}

void
basic_value_multeq(basic_value_t *self, const basic_value_t *rhs)
{
#if USE_REALS
	if (rhs->type == BASIC_VALUE_TYPE_REAL) {
		self->type = BASIC_VALUE_TYPE_REAL;
		self->body.real = basic_value_toReal(self) * rhs->body.real;
	} else
#endif
	switch (self->type) {
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL :
		self->body.real *= basic_value_toReal(rhs);
		break;
#endif
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER :
		self->body.long_integer *= basic_value_toInteger(rhs);
		break;
#endif
	case BASIC_VALUE_TYPE_INTEGER :
		self->body.integer *= basic_value_toInteger(rhs);
		break;
	default:
		break;
	}
}

void
basic_value_diveq(basic_value_t *self, const basic_value_t *rhs)
{
#if USE_REALS
	basic_value_setFromReal(self, basic_value_toReal(self) /
	    basic_value_toReal(rhs));
#elif USE_LONGINT
	basic_value_setFromLongInteger(self, basic_value_toLongInteger()self) /
	    basic_value_toLongInteger(rhs));
#else
	basic_value_setFromInteger(self, basic_value_toInteger()self) /
	    basic_value_toInteger(rhs));
#endif
}

void
basic_value_switchSign(basic_value_t* self)
{
#if OPT == OPT_SPEED
	switch (self->type) {
	case BASIC_VALUE_TYPE_INTEGER:
		self->body.integer = -self->body.integer;
		break;
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		self->body.long_integer = -self->body.long_integer;
		break;
#endif // USE_LONGINT
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL:
		self->body.real = -self->body.real;
		break;
#endif // USE_LONGINT
	case BASIC_VALUE_TYPE_LOGICAL:
		self->body.logical = !self->body.logical;
		break;
	default:
		// undefined
		break;
	}
#else // OPT == OPT_SIZE
	if (self->type == BASIC_VALUE_TYPE_INTEGER)
		self->body.integer = -self->body.integer;
#if USE_LONGINT
	else if (self->type == BASIC_VALUE_TYPE_LONG_INTEGER)
		self->body.long_integer = -self->body.long_integer;
#endif // USE_LONGINT
#if USE_REALS
	else if (self->type == BASIC_VALUE_TYPE_REAL)
		self->body.real = -self->body.real;
#endif // USE_REALS
	else if (self->type == BASIC_VALUE_TYPE_LOGICAL)
		self->body.logical = !self->body.logical;
#endif // OPT
}

BOOLEAN
basic_value_equals(const basic_value_t* lhs, const basic_value_t* rhs)
{
#if USE_REALS
	if (rhs->type == BASIC_VALUE_TYPE_REAL)
		return basic_value_toReal(lhs) == rhs->body.real;
	else
#endif
	switch (lhs->type) {
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL:
		return lhs->body.real == basic_value_toReal(rhs);
#endif
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		return lhs->body.long_integer == basic_value_toLongInteger(rhs);
#endif
	case BASIC_VALUE_TYPE_INTEGER:
		return lhs->body.integer == basic_value_toInteger(rhs);
	case BASIC_VALUE_TYPE_LOGICAL:
		return lhs->body.logical == basic_value_toLogical(rhs);
	default:
        	return FALSE;
	}
}

BOOLEAN
basic_value_greater(const basic_value_t* lhs, const basic_value_t* rhs)
{
#if USE_REALS
	if (rhs->type == BASIC_VALUE_TYPE_REAL)
		return basic_value_toReal(lhs) > rhs->body.real;
	else
#endif
	switch (lhs->type) {
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL:
		return lhs->body.real == basic_value_toReal(rhs);
#endif
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		return lhs->body.long_integer == basic_value_toLongInteger(rhs);
#endif
	case BASIC_VALUE_TYPE_INTEGER:
		return lhs->body.integer == basic_value_toInteger(rhs);
	case BASIC_VALUE_TYPE_LOGICAL:
		return lhs->body.logical == basic_value_toLogical(rhs);
	default:
        	return FALSE;
	}
}

static void
_basic_value_powerMatch(basic_value_t *self, const basic_value_t *rhs)
{
#if USE_LONGINT
	if (rhs->type == BASIC_VALUE_TYPE_LONG_INTEGER
#if USE_REALS
	    && self->type != BASIC_VALUE_TYPE_REAL
#endif
	    )
		basic_value_setFromLongInteger(self,
		    basic_value_toLongInteger(self));
#endif
#if USE_REALS
	basic_value_t zer = basic_value_fromInteger(-1);
	if (rhs->type == BASIC_VALUE_TYPE_REAL ||
	    !basic_value_greater(rhs, &zer))
		basic_value_setFromReal(self, basic_value_toReal(self));
#endif
}

void
basic_value_poweq(basic_value_t *self, const basic_value_t *rhs)
{
	_basic_value_powerMatch(self, rhs);
	switch (self->type) {
	case BASIC_VALUE_TYPE_INTEGER:
	{
		integer_t r = 1;
		integer_t i;
		for (i = basic_value_toInteger(rhs); i>0; --i)
			r *= self->body.integer;
		self->body.integer = r;
	}
		break;
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
	{
		long_integer_t r = 1;
		long_integer_t i;
		for (i = basic_value_toLongInteger(rhs); i>0; --i)
			r *= self->body.long_integer;
		self->body.long_integer = r;
	}
		break;
#endif
#if USE_REALS
	case BASIC_VALUE_TYPE_REAL:
		self->body.real = pow(self->body.real, basic_value_toReal(rhs));
		break;
#endif
	default:
		break;
	}
}

void
basic_value_oreq(basic_value_t* self, const basic_value_t* rhs)
{
	switch (self->type) {
	case BASIC_VALUE_TYPE_INTEGER:
		self->body.integer |= basic_value_toInteger(rhs);
		break;
	case BASIC_VALUE_TYPE_LOGICAL:
		self->body.logical |= basic_value_toLogical(rhs);
		break;
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		self->body.long_integer |= basic_value_toLongInteger(rhs);
		break;
#endif
	default:
		break;
	}
}

void
basic_value_andeq(basic_value_t* self, const basic_value_t* rhs)
{
	switch (self->type) {
	case BASIC_VALUE_TYPE_INTEGER:
		self->body.integer &= basic_value_toInteger(rhs);
		break;
	case BASIC_VALUE_TYPE_LOGICAL:
		self->body.logical &= basic_value_toLogical(rhs);
		break;
#if USE_LONGINT
	case BASIC_VALUE_TYPE_LONG_INTEGER:
		self->body.long_integer &= basic_value_toLongInteger(rhs);
		break;
#endif
	default:
		break;
	}
}
