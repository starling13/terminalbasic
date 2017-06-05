/*
 * Terminal-BASIC is a lightweight BASIC-like language interpreter
 * Copyright (C) 2016, 2017 Andrey V. Skvortsov <starling13@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "basic_parser_value.hpp"

#include "math.hpp"
#include "basic_lexer.hpp"

namespace BASIC
{

Parser::Value::Value() :
type(INTEGER)
{
	value.integer = 0;
}

#if USE_LONGINT

Parser::Value::Value(LongInteger v) :
type(LONG_INTEGER)
{
	value.longInteger = v;
}
#endif // USE_LONGINT

Parser::Value::Value(Integer v) :
type(INTEGER)
{
	value.integer = v;
}

#if USE_REALS

Parser::Value::Value(Real v) :
type(REAL)
{
	value.real = v;
}
#endif // USE_REALS

Parser::Value::Value(bool v) :
type(BOOLEAN)
{
	value.boolean = v;
}

#if USE_REALS

Parser::Value::operator Real() const
{
	switch (type) {
#if USE_LONGINT
	case LONG_INTEGER:
		return (Real(value.longInteger));
#endif // USE_LONGINT
	case INTEGER:
		return (Real(value.integer));
	case REAL:
		return (value.real);
	case BOOLEAN:
		return (Real(value.boolean));
	default:
		return (Real(NAN));
	}
}
#endif // USE_REALS

Parser::Value::operator bool() const
{
	switch (type) {
#if USE_LONGINT
	case LONG_INTEGER:
		return (bool(value.longInteger));
#endif // USE_LONGINT
	case INTEGER:
		return (bool(value.integer));
#if USE_REALS
	case REAL:
		return (bool(value.real));
#endif // USE_REALS
	case BOOLEAN:
		return (value.boolean);
	default:
		return (false);
	}
}

#if USE_LONGINT

Parser::Value::operator LongInteger() const
{
	switch (type) {
	case LONG_INTEGER:
		return value.longInteger;
	case INTEGER:
		return LongInteger(value.integer);
#if USE_REALS
	case REAL:
		return LongInteger(value.real);
#endif // USE_REALS
	case BOOLEAN:
		return LongInteger(value.boolean);
	default:
		return LongInteger(0);
	}
}
#endif // USE_LONGINT

Parser::Value::operator Integer() const
{
	switch (type) {
#if USE_LONGINT
	case LONG_INTEGER:
		return Integer(value.longInteger);
#endif // USE_LONGINT
	case INTEGER:
		return value.integer;
#if USE_REALS
	case REAL:
		return Integer(value.real);
#endif // USE_REALS
	case BOOLEAN:
		return Integer(value.boolean);
	default:
		return Integer(0);
	}
}

Parser::Value &
Parser::Value::operator-()
{
	switch (type) {
#if USE_LONGINT
	case LONG_INTEGER:
		value.longInteger = -value.longInteger;
		break;
#endif // USE_LONGINT
	case INTEGER:
		value.integer = -value.integer;
		break;
#if USE_REALS
	case REAL:
		value.real = -value.real;
		break;
#endif // USE_LONGINT
	case BOOLEAN:
		value.boolean = !value.boolean;
		break;
	default:
		// undefined
		break;
	}
	return *this;
}

bool
Parser::Value::operator<(const Value &rhs) const
{
	return !((*this > rhs) || (*this == rhs));
}

bool
Parser::Value::operator==(const Value &rhs) const
{
#if USE_REALS
	if (rhs.type == REAL)
		return math<Real>::almost_equal(Real(*this), Real(rhs), 2);
	else
#endif
	switch (this->type) {
#if USE_REALS
	case REAL:
		return math<Real>::almost_equal(this->value.real, Real(rhs), 2);
#endif
#if USE_LONGINT
	case LONG_INTEGER:
		return this->value.longInteger == LongInteger(rhs);
#endif
	case INTEGER:
		return this->value.integer == Integer(rhs);
	default:
		break;
	}
}

bool
Parser::Value::operator>(const Value &rhs) const
{
#if USE_REALS
	if (rhs.type == REAL)
		return Real(*this) > Real(rhs);
	else
#endif
	switch (this->type) {
#if USE_REALS
	case REAL:
		return this->value.real > Real(rhs);
#endif
#if USE_LONGINT
	case LONG_INTEGER:
		return this->value.longInteger > LongInteger(rhs);
#endif
	case INTEGER:
		return this->value.integer > Integer(rhs);
	default:
		break;
	}
}

bool
operator>=(const Parser::Value &l, const Parser::Value &r)
{
	return (l.operator>(r) || l.operator==(r));
}

bool
operator<=(const Parser::Value &l, const Parser::Value &r)
{
	return (l.operator<(r) || l.operator==(r));
}

Parser::Value &
Parser::Value::operator+=(const Value &rhs)
{
#if USE_REALS
	if (rhs.type == REAL)
		*this = Real(*this) + Real(rhs);
	else
#endif
	switch (this->type) {
#if USE_REALS
	case REAL: this->value.real += Real(rhs);
		break;
#endif
#if USE_LONGINT
	case LONG_INTEGER: this->value.longInteger += LongInteger(rhs);
		break;
#endif
	case INTEGER: this->value.integer += Integer(rhs);
		break;
	default: break;
	}

	return (*this);
}

Parser::Value &
Parser::Value::operator-=(const Value &rhs)
{
#if USE_REALS
	if (rhs.type == REAL)
		*this = Real(*this) - Real(rhs);
	else
#endif
	switch (this->type) {
#if USE_REALS
	case REAL: this->value.real -= Real(rhs);
		break;
#endif
#if USE_LONGINT
	case LONG_INTEGER: this->value.longInteger -= LongInteger(rhs);
		break;
#endif
	case INTEGER: this->value.integer -= Integer(rhs);
		break;
	default: break;
	}

	return (*this);
}

Parser::Value &
Parser::Value::operator*=(const Value &rhs)
{
#if USE_REALS
	if (rhs.type == REAL)
		*this = Real(*this) * Real(rhs);
	else
#endif
	switch (this->type) {
#if USE_REALS
	case REAL: this->value.real *= Real(rhs);
		break;
#endif
#if USE_LONGINT
	case LONG_INTEGER: this->value.longInteger *= LongInteger(rhs);
		break;
#endif
	case INTEGER: this->value.integer *= Integer(rhs);
		break;
	default: break;
	}

	return (*this);
}

// '/' operation always return REAL if real numbers support used
Parser::Value &
Parser::Value::operator/=(const Value &rhs)
{
#if USE_REALS
	value.real = Real(*this) / Real(rhs);
	type = Value::REAL;
#elif USE_LONGINT
	value.longInteger = LongInteger(*this) / LongInteger(rhs);
	type = Value::LONG_INTEGER;
#else
	value.integer = Integer(*this) / Integer(rhs);
	type = Value::INTEGER;
#endif
	return (*this);
}

void
Parser::Value::powerMatchValue(const Value &rhs)
{
#if USE_LONGINT
	if (rhs.type == LONG_INTEGER
#if USE_REALS
	    && type != REAL
#endif
	    )
		*this = LongInteger(*this);
#endif
#if USE_REALS
	if (rhs.type == REAL || rhs < Integer(0))
		*this = Real(*this);
#endif
}

Parser::Value &
Parser::Value::operator^=(const Value &rhs)
{
	powerMatchValue(rhs);
	switch (type) {
	case INTEGER:
	{
		Integer r = 1;
		for (Integer i = 0; i < Integer(rhs); ++i)
			r *= value.integer;
		value.integer = r;
	}
		break;
#if USE_LONGINT
	case LONG_INTEGER:
	{
		LongInteger r = 1;
		for (LongInteger i = 0; i < LongInteger(rhs); ++i)
			r *= value.longInteger;
		value.longInteger = r;
	}
		break;
#endif
#if USE_REALS
	case REAL:
		value.real = pow(value.real, Real(rhs));
		break;
#endif
	}
	return (*this);
}

void
Parser::Value::switchSign()
{
	switch (type) {
	case INTEGER:
		value.integer = -value.integer;
		break;
#if USE_LONGINT
	case LONG_INTEGER:
		value.longInteger = -value.longInteger;
		break;
#endif
#if USE_REALS
	case REAL:
		value.real = -value.real;
		break;
#endif
	case BOOLEAN:
		value.integer = -Integer(value.boolean);
		type = INTEGER;
		break;
	default:
		break;
	}
}

void
Parser::Value::notOp()
{
	switch (type) {
	case INTEGER:
		this->value.integer = ~this->value.integer;
		break;
	case BOOLEAN:
		this->value.boolean = !this->value.boolean;
		break;
	default:
		break;
	}
}

size_t
Parser::Value::size(Type t)
{
	switch (t) {
	case INTEGER:
		return sizeof(Integer);
#if USE_LONGINT
	case LONG_INTEGER:
		return sizeof(LongInteger);	
#endif
#if USE_REALS
	case REAL:
		return sizeof(Real);	
#endif
	case BOOLEAN:
		return sizeof(bool);
	case STRING:
		return 0;
	}
}

Parser::Value &
Parser::Value::operator|=(const Value &v)
{
	switch (type) {
	case INTEGER:
		this->value.integer |= Integer(v);
		break;
	case BOOLEAN:
		this->value.boolean |= bool(v);
		break;
	default:
		break;
	}
	return (*this);
}

Parser::Value &
Parser::Value::operator&=(const Value &v)
{
	switch (type) {
	case INTEGER:
		this->value.integer &= Integer(v);
		break;
	case BOOLEAN:
		this->value.boolean &= bool(v);
		break;
	default:
		break;
	}
	return (*this);
}

size_t
Parser::Value::printTo(Print& p) const
{
	switch (type) {
	case BOOLEAN:
	{
		char buf[6]; // Size, sufficient to store both 'TRUE' and 'FALSE
		Token t;
		if (value.boolean)
			t = Token::KW_TRUE;
		else
			t = Token::KW_FALSE;
		strcpy_P(buf, (PGM_P)pgm_read_word(&(Lexer::
		    tokenStrings[uint8_t(t)])));
		return p.print(buf);
	}
		break;
#if USE_REALS
	case REAL:
	{
		char buf[15];
#ifdef __AVR_ARCH__
		uint8_t decWhole = 1;
		Real n = math<Real>::abs(value.real);
		while (n >= Real(10)) {
			n /= Real(10);
			++decWhole;
		}
		if (math<Real>::abs(value.real) >= Real(0.1) && decWhole < 4)
			::dtostrf(value.real, 10, 8 - decWhole, buf);
		else
			::dtostre(value.real, buf, 7, DTOSTR_ALWAYS_SIGN);
#else
		::sprintf(buf, "%- 10.7G", value.real);
#endif // ARDUINO
		return p.print(buf);
	}
		break;
#endif
#if USE_LONGINT
	case Parser::Value::LONG_INTEGER:
		if (value.longInteger >= LongInteger(0))
			p.write(' ');
		return p.print(value.longInteger) + 1;
#endif // USE_LONGINT
	case Parser::Value::INTEGER:
		if (value.integer >= Integer(0))
			p.write(' ');
		return p.print(value.integer) + 1;
	default:
		return p.print('?');
	}
}

}
