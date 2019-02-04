#include "basic_lexer.h"

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "avr/pgmspace.h"
#include "tools.h"

extern const uint8_t _basic_lexer_tokenTable[] PROGMEM;

const char sSTAR[] PROGMEM = "*";
const char sSLASH[] PROGMEM = "/";
#if USE_REALS && USE_INTEGER_DIV
const char sBACK_SLASH[] PROGMEM = "\\";
#endif
const char sPLUS[] PROGMEM = "+";
const char sMINUS[] PROGMEM = "-";
const char sEQUALS[] PROGMEM = "=";
const char sCOLON[] PROGMEM = ":";
const char sSEMI[] PROGMEM = ";";
const char sLT[] PROGMEM = "<";
const char sGT[] PROGMEM = ">";
const char sLTE[] PROGMEM = "<=";
const char sGTE[] PROGMEM = ">=";
const char sNE[] PROGMEM = "<>";
#if CONF_USE_ALTERNATIVE_NE
const char sNEA[] PROGMEM = "><";
#endif
const char sCOMMA[] PROGMEM = ",";
const char sPOW[] PROGMEM = "^";
const char sLPAREN[] PROGMEM = "(";
const char sRPAREN[] PROGMEM = ")";

PGM_P const _basic_lexer_tokenStrings[] PROGMEM = {
	sSTAR,
	sSLASH,
#if USE_REALS && USE_INTEGER_DIV
	sBACK_SLASH,
#endif
	sPLUS, sMINUS,

	sEQUALS,
	sCOLON, sSEMI,
	sLT, sGT,
	sLTE, sGTE,
	sNE,
#if CONF_USE_ALTERNATIVE_NE
	sNEA,
#endif
	sCOMMA,
	sPOW,
	sLPAREN, sRPAREN
};

#if USE_REALS
static void
_basic_lexer_decimalreal(basic_lexer_context_t*);
#endif // USE_REALS

void
basic_lexer_init(basic_lexer_context_t *self, const uint8_t *str)
{
	assert(str != NULL);

	self->string_pointer = 0;
	self->string_to_parse = str;
}

#define SYM ((uint8_t)(self->string_to_parse[self->string_pointer]))

static void
_basic_lexer_firstLT(basic_lexer_context_t *self)
{
	if (SYM == '=')
		self->token = BASIC_TOKEN_LTE;
	else if (SYM == '>')
		self->token = BASIC_TOKEN_NE;
	else {
		self->token = BASIC_TOKEN_LT;
		return;
	}
	++self->string_pointer;
}

static void
_basic_lexer_firstGT(basic_lexer_context_t *self)
{
	if (SYM == '=')
		self->token = BASIC_TOKEN_GTE;
#if CONF_USE_ALTERNATIVE_NE
	else if (SYM == '<')
		self->token = BASIC_TOKEN_NEA;
#endif // CONF_USE_ALTERNATIVE_NE
	else {
		self->token = BASIC_TOKEN_GT;
		return;
	}
	++self->string_pointer;
}

static void
_basic_lexer_pushSym(basic_lexer_context_t *self)
{
	if (self->_value_pointer < (STRING_SIZE - 1))
		self->_id[self->_value_pointer++] = SYM;
	else
		self->_error = BASIC_LEXER_ERROR_STRING_OVERFLOW;
	++self->string_pointer;
}

#if USE_REALS

static BOOLEAN
lexer_number_scale(basic_lexer_context_t *self)
{
	integer_t scale = 0;
	BOOLEAN sign = TRUE;

	++self->string_pointer;
	if (SYM == '-') {
		sign = FALSE;
		++self->string_pointer;
	} else if (SYM == '+')
		++self->string_pointer;

	if (isdigit(SYM)) {
		scale += SYM - '0';
		++self->string_pointer;
	} else
		return FALSE;

	while (TRUE) {
		if (isdigit(SYM)) {
			scale *= (integer_t) (10);
			scale += SYM - '0';
			++self->string_pointer;
			continue;
		} else {
			if (!sign)
				scale = -scale;
			real_t pw = mf_pow((real_t) (10), (real_t) scale);
			basic_value_t pwv = basic_value_from_real(pw);
			basic_value_multeq(&self->value, &pwv);
			return TRUE;
		}
	}
}
#endif // USE_REALS

#if USE_LONGINT
static void
_basic_lexer_decimallongint(basic_lexer_context_t *self)
{
	long_integer_t val = self->value.body.long_integer;
	while (TRUE) {
		if (isdigit(SYM)) {
			const long_integer_t d = (long_integer_t)(SYM - '0');
			const long_integer_t v = val*(long_integer_t)10;
#if USE_REALS
			if ((val > MAX_LONG_INTEGER/(integer_t)(10)) ||
			    (v > MAX_LONG_INTEGER-d)) {
				self->value.body.real = d + v;
				++self->string_pointer;
				_basic_lexer_decimalreal(self);
				return;
			}
#endif // USE_REALS
			val *= 10;
			val += d;
		}
#if USE_REALS
		else if (SYM == '.') {
			self->value.body.real = val;
			_basic_lexer_decimalreal(self);
			return;
		}
#endif // USE_REALS
		else {
			self->value.body.long_integer = val;
			self->value.type = BASIC_VALUE_TYPE_LONG_INTEGER;
			self->token = BASIC_TOKEN_C_LONG_INTEGER;
			return;
		}
	}
}
#endif // USE_LONGINT

#if USE_REALS
static void
_basic_lexer_decimalreal(basic_lexer_context_t *self)
{
	real_t val = self->value.body.real;
	while (TRUE) {
		if (isdigit(SYM)) {
			val *= (real_t)10;
			val += (real_t)(SYM - '0');
			++self->string_pointer;
		} else if (SYM == '.') {
			real_t d = 1;
			while (TRUE) {
				++self->string_pointer;
				if (isdigit(SYM)) {
					d /= (real_t)10;
					val += (real_t)(SYM - '0') * d;
				} else if (SYM == 'E' || SYM == 'e') {
					if (!lexer_number_scale(self))
						self->token = BASIC_TOKEN_NOTOKEN;
					else {
						self->token = BASIC_TOKEN_C_REAL;
						return;
					}
				} else
					goto final;
			}
		} else
			break;
	}
final:
	self->value = basic_value_from_real(val);
	self->token = BASIC_TOKEN_C_REAL;
}
#endif // USE_REALS

static void
_basic_lexer_decimalint(basic_lexer_context_t *self)
{
	integer_t val = 0;
	while (TRUE) {
		if (isdigit(SYM)) {
			const integer_t d = (integer_t)(SYM - '0');
			const integer_t v = val*(integer_t)10;
#if USE_LONGINT
			if ((val > MAX_INTEGER/(integer_t)(10)) ||
			    (v > MAX_INTEGER-d)) {
				self->value.body.long_integer = val;
				_basic_lexer_decimallongint(self);
				return;
			}
			val = d + v;
			++self->string_pointer;
			continue;
#elif USE_REALS
			if ((val > MAX_INTEGER/(integer_t)(10)) ||
			    (v > MAX_INTEGER-d)) {
				self->value.body.real = val;
				_basic_lexer_decimalreal(self);
				return;
			}
#endif // USE_REALS
			val *= (integer_t)10;
			val += d;
			++self->string_pointer;
		}
#if USE_REALS
		else if (SYM == '.' || SYM == 'E' || SYM == 'e') {
			self->value.body.real = val;
			_basic_lexer_decimalreal(self);
			return;
		}
#endif // USE_REALS
		else {
			self->value.body.integer = val;
			self->value.type = BASIC_VALUE_TYPE_INTEGER;
			self->token = BASIC_TOKEN_C_INTEGER;
			return;
		}
	}
}

static void
_basic_lexer_decimal(basic_lexer_context_t *self)
{
#if USE_LONGINT
	self->value.type = BASIC_VALUE_TYPE_LONG_INTEGER;
	long_integer_t *val = &self->value.body.long_integer;
#else
	self->value.type = BASIC_VALUE_TYPE_INTEGER;
	integer_t *val = &self->value.body.integer;
#endif // USE_LONGINT
#if USE_REALS
	/* Number, starting with explicit decimal point - zero whole part of
	 * mantissa */
	if (SYM == '.')
		*val = 0;
	else
#endif
		*val = SYM - '0';
	while (SYM > 0) {
#if USE_REALS
		if (self->value.type == BASIC_VALUE_TYPE_REAL) {
			++self->string_pointer;
			if (isdigit(SYM)) {
				self->value.body.real *= (real_t) (10);
				self->value.body.real += (real_t) (SYM - '0');
				continue;
			}
		} else if (SYM != '.') {

#endif // USE_REALS
			++self->string_pointer;
			if (isdigit(SYM)) {
#if USE_REALS
				if (*val > MAXINT / (INT) (10)) {
					self->value.type = BASIC_VALUE_TYPE_REAL;
					self->value.body.real = (real_t) (*val);
					self->value.body.real *= (real_t) (10);
					self->value.body.real += SYM - '0';
				} else {
#endif
					*val *= (INT) (10);
					*val += SYM - '0';
#if USE_REALS
				}
#endif
				continue;
			}
#if USE_REALS
		}
#endif

		switch (SYM) {
#if USE_REALS
		case '.':
		{
			if (self->value.type != BASIC_VALUE_TYPE_REAL) {
				self->value.type = BASIC_VALUE_TYPE_REAL;
				self->value.body.real = (real_t) (*val);
			}
			real_t d = 1;
			while (TRUE) {
				++self->string_pointer;
				if (isdigit(SYM)) {
					d /= 10.f;
					self->value.body.real +=
					    (real_t) (SYM - '0') * d;
					continue;
				} else if (SYM == 0) {
					self->token = BASIC_TOKEN_C_REAL;
					return;
				} else if (SYM == 'E' || SYM == 'e') {
					if (!lexer_number_scale(self))
						self->token = BASIC_TOKEN_NOTOKEN;
					else
						self->token = BASIC_TOKEN_C_REAL;
					return;
				} else {
					self->token = BASIC_TOKEN_C_REAL;
					return;
				}
			}
		}
			break;
		case 'E':
		case 'e':
		{
			if (self->value.type == BASIC_VALUE_TYPE_INTEGER
#if USE_LONGINT
			    || self->value.type == BASIC_VALUE_TYPE_LONG_INTEGER
#endif
			    )
				self->value.body.real = basic_value_to_real(
				    &self->value);
			if (!lexer_number_scale(self)) {
				self->token = BASIC_TOKEN_NOTOKEN;
				return;
			}
		}
#endif
		default:
			if (self->value.type == BASIC_VALUE_TYPE_INTEGER
#if USE_LONGINT
			    || self->value.type == BASIC_VALUE_TYPE_LONG_INTEGER
#endif
			    )
				self->token = BASIC_TOKEN_C_INTEGER;
#if USE_REALS
			else
				self->token = BASIC_TOKEN_C_REAL;
#endif
			return;
		}
	}
}

static void
_basic_lexer_stringConst(basic_lexer_context_t *self)
{
	while (SYM != 0) {
		if (SYM == '"') {
			self->token = BASIC_TOKEN_STRING_IDENT;
			self->_id[self->_value_pointer] = 0;
			return;
		}
		_basic_lexer_pushSym(self);
	}
}

static void
_basic_lexer_ident(basic_lexer_context_t *self)
{
	while (isalnum(SYM)
#if ALLOW_UNDERSCORE_ID
	    || (SYM == '_')
#endif
	    ) {
		_basic_lexer_pushSym(self);
	}
	if (SYM == '%') {
		_basic_lexer_pushSym(self);
#if USE_LONGINT
		if (SYM == '%') {
			_basic_lexer_pushSym(self);
			self->token = BASIC_TOKEN_LONGINT_IDENT;
		} else
#endif
			self->token = BASIC_TOKEN_INTEGER_IDENT;
	} else if (SYM == '$') {
		_basic_lexer_pushSym(self);
		self->token = BASIC_TOKEN_STRING_IDENT;
	} else if (SYM == '!') {
		_basic_lexer_pushSym(self);
		self->token = BASIC_TOKEN_BOOL_IDENT;
	} else
		self->token = BASIC_TOKEN_REAL_IDENT;
	self->value.type = BASIC_VALUE_TYPE_STRING;
	self->_id[self->_value_pointer] = 0;
}

BOOLEAN
basic_lexer_getnextPlain(basic_lexer_context_t *self)
{
	self->token = BASIC_TOKEN_NOTOKEN;
	/*_error = NO_ERROR;*/
	self->_value_pointer = 0;
	/* Iterate until end of input string */
	while (SYM > 0) {
		switch (SYM) {
		case '=':
			self->token = BASIC_TOKEN_EQUALS;
			goto token_found;
		case ';':
			self->token = BASIC_TOKEN_SEMI;
			goto token_found;
#if USE_REALS
		case '.':
			_basic_lexer_decimal(self);
			return TRUE;
#endif // USE_REALS
		case ',':
			self->token = BASIC_TOKEN_COMMA;
			goto token_found;
		case ':':
			self->token = BASIC_TOKEN_COLON;
			goto token_found;
		case '<':
			++self->string_pointer;
			_basic_lexer_firstLT(self);
			return TRUE;
		case '>':
			++self->string_pointer;
			_basic_lexer_firstGT(self);
			return TRUE;
		case '(':
			self->token = BASIC_TOKEN_LPAREN;
			goto token_found;
		case ')':
			self->token = BASIC_TOKEN_RPAREN;
			goto token_found;
		case '+':
			self->token = BASIC_TOKEN_PLUS;
			goto token_found;
		case '-':
			self->token = BASIC_TOKEN_MINUS;
			goto token_found;
		case '*':
			self->token = BASIC_TOKEN_STAR;
			goto token_found;
		case '/':
			self->token = BASIC_TOKEN_SLASH;
			goto token_found;

#if USE_REALS && USE_INTEGER_DIV
		case '\\':
			self->token = BASIC_TOKEN_BACK_SLASH;
			goto token_found;
#endif
		case '^':
			self->token = BASIC_TOKEN_POW;
			goto token_found;
		case '"':
			++self->string_pointer;
			_basic_lexer_stringConst(self);
			goto token_found;
		case ' ':
		case '\t':
			++self->string_pointer;
			break;
		default:
			if (isdigit(SYM)) {
				_basic_lexer_decimalint(self);
				return TRUE;
			} else if (isalpha(SYM)) {
				uint8_t index;
				uint8_t *pos =
				    (uint8_t*) self->string_to_parse +
				    self->string_pointer;
				if ((pos = scanTable(pos, _basic_lexer_tokenTable,
						&index)) != NULL) {
					self->token = (basic_token_t)index;
					if (self->token == BASIC_TOKEN_KW_TRUE) {
						self->value.body.logical = TRUE;
						self->token = BASIC_TOKEN_C_BOOLEAN;
					} else if (self->token == BASIC_TOKEN_KW_FALSE) {
						self->value.body.logical = FALSE;
						self->token = BASIC_TOKEN_C_BOOLEAN;
					}
					self->string_pointer += (uint8_t) (pos - ((uint8_t*) self->string_to_parse +
					    self->string_pointer));
					return TRUE;
				} else {
					_basic_lexer_pushSym(self);
					_basic_lexer_ident(self);
					return TRUE;
				}
			}
			goto token_found; /* ? */
		}
	}

	return FALSE;
token_found:
	++self->string_pointer;
	return TRUE;
}

static BOOLEAN
_basic_lexer_tokenizedNext(basic_lexer_context_t *self)
{
	if (SYM != ASCII_NUL) {
		self->token = SYM;
		++self->string_pointer;
		switch (self->token) {
		case BASIC_TOKEN_C_INTEGER :
#if USE_LONGINT
			self->value.type = BASIC_VALUE_TYPE_LONG_INTEGER;
			readU32((uint32_t*)&self->value.body.long_integer,
			     self->string_to_parse+self->string_pointer);
#else
			self->value.type = BASIC_VALUE_TYPE_INTEGER;
			readU16((uint16_t*)&self->value.body.integer,
			     self->string_to_parse+self->string_pointer);
#endif
			self->string_pointer += sizeof (INT);
			break;
		default:
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOLEAN
basic_lexer_getnextTokenized(basic_lexer_context_t *self)
{
	self->token = BASIC_TOKEN_NOTOKEN;
	/*_error = NO_ERROR;*/
	self->_value_pointer = 0;
	/* Iterate until end of input string */
	while (SYM > ASCII_NUL) {
		switch (SYM) {
		case ASCII_DLE:
			++self->string_pointer;
			return _basic_lexer_tokenizedNext(self);
		case ' ':
		case '\t':
			++self->string_pointer;
			break;
		default:
			if (isalpha(SYM)) {
				_basic_lexer_pushSym(self);
				_basic_lexer_ident(self);
				return TRUE;
			}
			goto token_found; /* ? */
		}
	}

	return FALSE;
token_found:
	++self->string_pointer;
	return TRUE;
}

void
basic_lexer_tokenString(basic_token_t t, uint8_t *buf)
{
	if (t < BASIC_TOKEN_STAR) {
		const uint8_t *result = _basic_lexer_tokenTable, *pointer = result;
		uint8_t c;
		uint8_t index = 0;

		do {
			c = pgm_read_byte(pointer++);
			if (c & 0x80) {
				if (index++ == (uint8_t) (t)) {
					pointer = result;
					result = buf;
					while (((c = pgm_read_byte(pointer++)) & 0x80) == 0)
						*(buf++) = c;
					*(buf++) = c & ~0x80;
					*buf = 0;
				} else
					result = pointer;
			}
		} while (c != 0);
	} else if (t < BASIC_TOKEN_INTEGER_IDENT)
		strcpy_P((char*) buf,
			(PGM_P) pgm_read_ptr(&(_basic_lexer_tokenStrings[
					     (uint8_t) (t)-(uint8_t) (BASIC_TOKEN_STAR)])));
	else
		*buf = '\0';
}

uint8_t
basic_lexer_tokenize(basic_lexer_context_t *self, uint8_t *dst, uint8_t dstlen,
		     const uint8_t *src)
{
	basic_lexer_init(self, src);

	uint8_t position = 0;
	uint8_t lexerPosition = 0;

	while (basic_lexer_getnextPlain(self)) {
		/* The destinaton buffer is over */
		if (position >= (dstlen - 1)) {
			position = dstlen-1;
			break;
		}
		const basic_token_t tok = self->token;
		if (tok <= BASIC_TOKEN_RPAREN) {
			/* One byte tokens need space of 2 bytes - DLE and token */
			if (position+2 >= dstlen)
				break;
			dst[position++] = ASCII_DLE;
			dst[position++] = tok;
			lexerPosition = self->string_pointer;
			/* Save rem text as is */
			if (tok == BASIC_TOKEN_KW_REM) {
				/* Skip blank */
				while ((self->string_to_parse[lexerPosition] == ' ')
				|| (self->string_to_parse[lexerPosition] == '\t'))
					++lexerPosition;
				const uint8_t remaining =
				    strlen((char*) src + lexerPosition);
				if (remaining + position >= dstlen)
					break;
				memcpy(dst + position, src + lexerPosition,
				    remaining);
				position += remaining;
				break;
			}
			continue;
		} else if (tok == BASIC_TOKEN_C_INTEGER) {
			if (position+2+sizeof(integer_t) >= dstlen)
				break;
			dst[position++] = ASCII_DLE;
			dst[position++] = tok;
			const integer_t v = self->value.body.integer;
			writeU16((uint16_t)v, dst + position);
			position += sizeof (integer_t);
		}
#if USE_LONGINT
		else if (tok == BASIC_TOKEN_C_LONG_INTEGER) {
			if (position+2+sizeof(long_integer_t) >= dstlen)
				break;
			dst[position++] = ASCII_DLE;
			dst[position++] = tok;
			const long_integer_t v = self->value.body.long_integer;
			writeU32((uint32_t)v, dst + position);
			position += sizeof (long_integer_t);
		}
#endif // USE_LONGINT
#if USE_REALS
		else if (tok == BASIC_TOKEN_C_REAL) {
			if (position+2+sizeof(real_t) >= dstlen)
				break;
			dst[position++] = ASCII_DLE;
			dst[position++] = tok;
			
			const real_t v = self->value.body.real;
			writeR32((real_t)v, dst + position);
			position += sizeof (real_t);
		}
#endif // USE_REALS
		else { // Other tokens
			dst[position++] = ' ';
			while (src[lexerPosition] == ' ' ||
			src[lexerPosition] == '\t')
				++lexerPosition;
			const uint8_t siz = self->string_pointer - lexerPosition;
			if ((position + siz) >= dstlen)
				break;
			memcpy(dst + position, src + lexerPosition, siz);
			position += siz;
		}
		lexerPosition = self->string_pointer;
	}
	dst[position] = '\0';
	return position;
}
