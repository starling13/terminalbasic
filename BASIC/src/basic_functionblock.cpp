/*
 * ucBASIC is a lightweight BASIC-like language interpreter
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

#include "basic_functionblock.hpp"
#include "basic_parser.hpp"
#include "basic_parser_value.hpp"
#include "basic_interpreter.hpp"

namespace BASIC
{

FunctionBlock::FunctionBlock(FunctionBlock *next) :
commandTokens(NULL), commands(NULL),
functionTokens(NULL), functions(NULL), _next(next)
{
}

void
FunctionBlock::init()
{
	this->_init();
	if (_next != NULL)
		_next->init();
}

void
FunctionBlock::setNext(FunctionBlock *next)
{
	if (_next != NULL)
		_next->setNext(next);
	else
		_next = next;
}

FunctionBlock::function
FunctionBlock::getFunction(const char *name) const
{
	function result;
	if (((result = _getFunction(name)) == NULL) &&
	    _next != NULL)
		result = _next->getFunction(name);
	return (result);
}

FunctionBlock::command
FunctionBlock::getCommand(const char *name) const
{
	command result;
	if (((result = _getCommand(name)) == NULL) &&
	    _next != NULL)
		result = _next->getCommand(name);
	return (result);
}

FunctionBlock::function
FunctionBlock::_getFunction(const char *name) const
{
	function result = NULL;
	
	if (functionTokens != NULL) {
		uint8_t index;
		if (scanTable((const uint8_t*)name, functionTokens, index)) {
			result = reinterpret_cast<FunctionBlock::function>(
			    pgm_read_ptr(&functions[index]));
		}
	}

	return (result);
}

FunctionBlock::command
FunctionBlock::_getCommand(const char *name) const
{
	if (commandTokens == NULL)
		return (NULL);
	
	uint8_t index;
	if (scanTable((const uint8_t*)name, commandTokens, index))
		return (reinterpret_cast<FunctionBlock::command>(
		    pgm_read_ptr(&commands[index])));

	return (NULL);
}

#if USE_REALS
bool
FunctionBlock::general_func(Interpreter &i, _funcReal f)
{
	Parser::Value v(Real(0));
	i.popValue(v);
	if (v.type == Parser::Value::INTEGER ||
#if USE_LONGINT
	    v.type == Parser::Value::LONG_INTEGER ||
#endif
	    v.type == Parser::Value::REAL) {
		v = (*f)(Real(v));
		i.pushValue(v);
		return (true);
	} else
		return (false);
}
#endif

bool
FunctionBlock::general_func(Interpreter &i, _funcInteger f)
{
	Parser::Value v(Integer(0));
	i.popValue(v);
	if (v.type == Parser::Value::INTEGER
#if USE_LONGINT
	 || v.type == Parser::Value::LONG_INTEGER
#endif
#if USE_REALS
	 || v.type == Parser::Value::REAL
#endif
	) {
#if USE_LONGINT
		v = (*f)(LongInteger(v));
#else
		v = (*f)(Integer(v));
#endif
		i.pushValue(v);
		return (true);
	} else
		return (false);
}

}
