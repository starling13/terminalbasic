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

#include "basic_arduinoio.hpp"

#if CONF_MODULE_ARDUINOIO

#include "Arduino.h"
#include <assert.h>

namespace BASIC
{

static const uint8_t arduinoIOFuncs[] PROGMEM = {
	'A', 'R', 'E', 'A', 'D', '%'+0x80,
#if USE_REALS
	'A', 'R', 'E', 'A', 'D'+0x80,
#endif
	'D', 'R', 'E', 'A', 'D'+0x80,
	0
};

const FunctionBlock::function ArduinoIO::_funcs[] PROGMEM = {
	ArduinoIO::func_aread_int,
#if USE_REALS
	ArduinoIO::func_aread,
#endif
	ArduinoIO::func_dread
};

static const uint8_t arduinoIOCommands[] PROGMEM = {
	'A', 'W', 'R', 'I', 'T', 'E'+0x80,
#if CONF_BEEP
	'B', 'E', 'E', 'P'+0x80,
#endif
#if CONF_MODULE_ARDUINOIO_TONE
	'D', 'N', 'O', 'T', 'O', 'N', 'E'+0x80,
	'D', 'T', 'O', 'N', 'E'+0x80,
#endif
	'D', 'W', 'R', 'I', 'T', 'E'+0x80,
	0
};

const FunctionBlock::command  ArduinoIO::_commands[] PROGMEM = {
	ArduinoIO::comm_awrite,
#if CONF_BEEP
	ArduinoIO::comm_beep,
#endif
#if CONF_MODULE_ARDUINOIO_TONE
	ArduinoIO::comm_notone,
	ArduinoIO::comm_tone,
#endif
	ArduinoIO::comm_dwrite
};

ArduinoIO::ArduinoIO()
{
	commands = _commands;
	commandTokens = arduinoIOCommands;
	functions = _funcs;
	functionTokens = arduinoIOFuncs;
}

#if USE_REALS
bool
ArduinoIO::func_aread(Interpreter &i)
{
	return general_func(i, aread_r);
}
#endif

bool
ArduinoIO::func_aread_int(Interpreter &i)
{
	return general_func(i, aread_i);
}

bool
ArduinoIO::func_dread(Interpreter &i)
{
	INT v;
	if (getIntegerFromStack(i, v)) {
		pinMode(v, INPUT);
		i.pushValue(bool(digitalRead(v)));
		return true;
	} else
		return false;
}

bool
ArduinoIO::comm_awrite(Interpreter &i)
{
	INT v;
	if (getIntegerFromStack(i, v)) {
		INT v2;
		if (getIntegerFromStack(i, v2)) {
			pinMode(v2, OUTPUT);
			analogWrite(v2, v);
			return true;
		}
	}

	return false;
}

bool
ArduinoIO::comm_dwrite(Interpreter &i)
{
	Parser::Value v(false);
	if (i.popValue(v)) {
		if (v.type == Parser::Value::BOOLEAN) {
			INT v2;
			if (getIntegerFromStack(i, v2)) {
				pinMode(v2, OUTPUT);
				digitalWrite(v2, bool(v));
				return true;
			}
		}
	}

	return false;
}

#if CONF_MODULE_ARDUINOIO_TONE
bool
ArduinoIO::comm_tone(Interpreter &i)
{
	INT pin, freq, dur;
	Parser::Value on(false);
	if (getIntegerFromStack(i, dur)) {
		if (getIntegerFromStack(i, freq)) {
			if (getIntegerFromStack(i, pin)) {
				tone(pin, freq, dur);
				return true;
			}
		}
	}
	return false;
}

bool
ArduinoIO::comm_notone(Interpreter &i)
{
	INT pin;
	if (getIntegerFromStack(i, pin)) {
		noTone(pin);
		return true;
	}
	return false;
}

#endif

#if USE_REALS
Real
ArduinoIO::aread_r(Real v)
{
	pinMode(v, INPUT);

	return Real(analogRead(v)) / Real(1023) * Real(5.0);
}
#endif

#if USE_LONGINT
#define INT LongInteger
#else
#define INT Integer
#endif // USE_LONGINT
INT
ArduinoIO::aread_i(INT v)
{
	return analogRead(v);
}
#undef INT

#if CONF_BEEP
bool
ArduinoIO::comm_beep(Interpreter &i)
{
    pinMode(BEEP_PIN, OUTPUT);
    tone(BEEP_PIN, 440, 333);
    return true;
}
#endif // CONF_BEEP

} // namespace BASIC

#endif // CONF_MODULE_ARDUINOIO
