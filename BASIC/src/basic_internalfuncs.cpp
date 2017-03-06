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

#include "basic_internalfuncs.hpp"
#include "basic_interpreter.hpp"
#include "Arduino.h"

#include <assert.h>

namespace BASIC
{

InternalFunctions::InternalFunctions(FunctionBlock *first) :
FunctionBlock(first)
{
}

FunctionBlock::function
InternalFunctions::_getFunction(const char *name) const
{
	assert(name != NULL);
	uint8_t position = 0;
	char c = name[position];
	if (c != 0) {
		switch (c) {
		case 'R':
			++position;
			if (name[position] == 'N') {
				++position;
				if (name[position] == 'D')
					return (func_rnd);
			}
			break;
		case 'T':
			++position;
			if (name[position] == 'I') {
				++position;
				if (name[position] == 'M') {
					++position;
					if (name[position] == 'E')
						return (func_tim);
				}
			}
			break;
		}
	}
	return (NULL);
}

bool
InternalFunctions::func_rnd(Interpreter &i)
{
#if USE_REALS
	Parser::Value v(Real(random()) / Real(RANDOM_MAX));
#else
	Parser::Value v(Integer(random()));
#endif
	i.pushValue(v);
	return (true);
}

bool
InternalFunctions::func_tim(Interpreter &i)
{
#if USE_REALS
	Real time = Real(millis()) / Real(1000);
#else
	Integer time = millis() / 1000;
#endif
	Parser::Value v(time);
	i.pushValue(v);
	return (true);
}

}
