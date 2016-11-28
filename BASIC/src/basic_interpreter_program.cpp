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

#include "basic_interpreter_program.hpp"
#include <assert.h>

namespace BASIC
{

Interpreter::Program::Program()
{
	newProg();
}

Interpreter::Program::String*
Interpreter::Program::getString()
{
	if (_jumpFlag) {
		_current = _jump;
		_jumpFlag = false;
		return current();
	}
	if (_current >= _textEnd)
		return NULL;
	else {
		Program::String *result = current();
		_current += result->size;
		_textPosition = 0;
		return result;
	}
}

Interpreter::Program::String*
Interpreter::Program::current() const
{
	return (stringByIndex(_current));
}

Interpreter::Program::String*
Interpreter::Program::first() const
{
	return (stringByIndex(0));
}

Interpreter::Program::String*
Interpreter::Program::last() const
{
	return (stringByIndex(_textEnd));
}

Interpreter::Program::String*
Interpreter::Program::stringByIndex(uint16_t index) const
{
	return (const_cast<String*> (reinterpret_cast<const String*> (
	    _text + index)));
}

Interpreter::Program::String*
Interpreter::Program::stringByNumber(uint16_t number, size_t index)
{
	Program::String *result = NULL;

	if (index <= _textEnd) {
		_current = index;
		for (String *cur = getString(); cur != NULL;
		    cur = getString()) {
			if (cur->number == number) {
				result = cur;
				break;
			}
		}
	}
	return (result);
}

uint8_t
Interpreter::Program::StackFrame::size(Type t)
{
	switch (t) {
	case SUBPROGRAM_RETURN:
		return sizeof (Type) + sizeof (GosubReturn);
	case FOR_NEXT:
		return sizeof (Type) + sizeof (ForBody);
	case STRING:
		return sizeof (Type) + STRINGSIZE;
	case ARRAY_DIMENSION:
		return sizeof (Type) + sizeof (uint16_t);
	case ARRAY_DIMENSIONS:
		return sizeof (Type) + sizeof (uint8_t);
	default:
		return 0;
	}
}

void
Interpreter::Program::newProg()
{
	_textEnd = _current = _variablesEnd = _arraysEnd = _jump = 0;
	_jumpFlag = false;
	_textPosition = 0;
	_sp = PROGSIZE;
	memset(_text, 0xFF, PROGSIZE);
}

Interpreter::VariableFrame*
Interpreter::Program::variableByName(const char *name)
{
	uint16_t index = _textEnd;

	for (VariableFrame *f = variableByIndex(index); f != NULL;
	    f = variableByIndex(index)) {
		int8_t res = strcmp(name, f->name);
		if (res == 0) {
			return f;
		} else if (res < 0)
			break;
		index += f->size();
	}
	return NULL;
}

uint16_t
Interpreter::Program::stringIndex(const String *s) const
{
	return (((char*) s) - _text);
}

uint16_t
Interpreter::Program::variableIndex(VariableFrame *f) const
{
	return (((char*)f) - _text);
}

uint16_t
Interpreter::Program::arrayIndex(ArrayFrame *f) const
{
	return (((char*)f) - _text);
}

Interpreter::Program::StackFrame*
Interpreter::Program::push(StackFrame::Type t)
{
	_sp -= StackFrame::size(t);
	StackFrame *f = stackFrameByIndex(_sp);
	if (f != NULL)
		f->_type = t;
	return f;
}

void
Interpreter::Program::pop()
{
	StackFrame *f = stackFrameByIndex(_sp);
	if (f != NULL)
		_sp += StackFrame::size(f->_type);
}

Interpreter::Program::StackFrame*
Interpreter::Program::stackFrameByIndex(uint16_t index)
{
	if ((index > 0) && (index < PROGSIZE))
		return (reinterpret_cast<StackFrame*> (_text + index));
	else
		return (NULL);
}

Interpreter::Program::StackFrame*
Interpreter::Program::currentStackFrame()
{
	return stackFrameByIndex(_sp);
}

Interpreter::ArrayFrame*
Interpreter::Program::arrayByName(const char *name)
{
	uint16_t index = _variablesEnd;

	for (ArrayFrame *f = arrayByIndex(index); f != NULL; index += f->size(),
	    f = arrayByIndex(index)) {
		int8_t res = strcmp(name, f->name);
		if (res == 0) {
			return f;
		} else if (res < 0)
			break;
	}
	return NULL;
}

Interpreter::VariableFrame*
Interpreter::Program::variableByIndex(uint16_t index)
{
	if (index < _variablesEnd)
		return (reinterpret_cast<VariableFrame*> (_text + index));
	else
		return NULL;
}

Interpreter::ArrayFrame*
Interpreter::Program::arrayByIndex(uint16_t index)
{
	return (reinterpret_cast<ArrayFrame*> (_text + index));
}

bool
Interpreter::Program::addLine(uint16_t num, const char *text)
{
	reset();

	const uint16_t strLen = sizeof (String) + strlen(text) + 1;
	
	if (_textEnd == 0) // First string insertion
		return insert(num, text);
	
	// Iterate over
	String *cur;
	for (cur = current(); _current<_textEnd; cur = current()) {
		if (num < cur->number) {
			break;
		} else if (num == cur->number) { // Replace string
			uint16_t newSize = strLen;
			uint16_t curSize = cur->size;
			int16_t dist = int16_t(newSize) - curSize;
			uint16_t bytes2copy = _arraysEnd -
				    (_current + curSize);
			if ((_arraysEnd+dist) >= _sp)
				return false;
			memmove(_text + _current + newSize,
			    _text + _current + curSize, bytes2copy);
			cur->number = num;
			cur->size = strLen;
			strcpy(cur->text, text);
			_textEnd += dist, _variablesEnd += dist,
			    _arraysEnd += dist;
			return true;
		}
		_current += cur->size;
	}
	return insert(num, text);
}

bool
Interpreter::Program::insert(uint16_t num, const char *text)
{
	const uint8_t sl = strlen(text);
	assert(sl < PROGSTRINGSIZE);
	const uint16_t strLen = sizeof (String) + strlen(text) + 1;

	if (_arraysEnd+strLen>=_sp)
		return false;
	
	memmove(_text+_current+strLen, _text + _current,
	    _arraysEnd-_current);

	String *cur = current();
	cur->number = num;
	cur->size = strLen;
	strcpy(cur->text, text);
	_textEnd+=strLen, _variablesEnd+=strLen, _arraysEnd+=strLen;
	return true;
}

void Interpreter::Program::reset()
{
	_current = 0;
}

}
