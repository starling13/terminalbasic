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

#include "basic.hpp"
#include "basic_exteeprom.hpp"

#if USE_EXTEEPROM

namespace BASIC
{

static const uint8_t extEEPROMCommandTokens[] PROGMEM = {
	'E', 'C', 'H', 'A', 'I', 'N'+0x80,
	'E', 'L', 'O', 'A', 'D'+0x80,
	'E', 'S', 'A', 'V', 'E'+0x80,
	0
};

const FunctionBlock::function ExtEEPROM::_commands[] PROGMEM = {
	ExtEEPROM::com_echain,
	ExtEEPROM::com_eload,
	ExtEEPROM::com_esave
};

ExtEEPROM::ExtEEPROM()
{
	commands = _commands;
	commandTokens = extEEPROMCommandTokens;
}

void
ExtEEPROM::_init()
{
	
}

bool
ExtEEPROM::com_echain(Interpreter &i)
{
	return true;
}

bool
ExtEEPROM::com_eload(Interpreter &i)
{
	return true;
}

bool
ExtEEPROM::com_esave(Interpreter &i)
{
	return true;
}

}

#endif // USE_EXTEEPROM