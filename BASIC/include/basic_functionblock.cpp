/*
 * ucBASIC is a lightweight BASIC-like language interpreter
 * Copyright (C) 2016  Andrey V. Skvortsov <starling13@mail.ru>
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

#include "basic_functionblock.hpp"
#include "basic_parser.hpp"
#include "basic_parser_value.hpp"
#include "basic_interpreter.hpp"

namespace BASIC
{

FunctionBlock::FunctionBlock(FunctionBlock *next) :
	_next(next)
{
}

FunctionBlock::function
FunctionBlock::getFunction(const char *name) const
{
	function result;
	if (((result = _getFunction(name)) == NULL) &&
	    _next != NULL)
		result = _next->getFunction(name);
	return result;
}

bool
FunctionBlock::general_func(Interpreter &i, _funcReal f)
{
	Parser::Value v(Real(0));
	i.popValue(v);
	if (v.type == Parser::Value::INTEGER || v.type == Parser::Value::REAL) {
		v = (*f)(Real(v));
		i.pushValue(v);
		return true;
	} else
		return false;
}

bool
FunctionBlock::general_func(Interpreter &i, _funcInteger f)
{
	Parser::Value v(Integer(0));
	i.popValue(v);
	if (v.type == Parser::Value::INTEGER || v.type == Parser::Value::REAL) {
		v = (*f)(Integer(v));
		i.pushValue(v);
		return true;
	} else
		return false;
}

}
