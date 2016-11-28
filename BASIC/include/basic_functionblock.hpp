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

#ifndef BASIC_FUNCTIONBLOCK_HPP
#define BASIC_FUNCTIONBLOCK_HPP

#include <stdlib.h>

namespace BASIC
{

class Interpreter;

class FunctionBlock
{
public:

	typedef bool (*function)(Interpreter&);
	
	function getFunction(const char*) const;
	
	FunctionBlock *next() { return _next; }
	
protected:
	
	explicit FunctionBlock(FunctionBlock* =NULL);
	virtual function _getFunction(const char*) const = 0;
	
private:
	FunctionBlock *_next;
};

}

#endif
