/*
 * Terminal-BASIC is a lightweight BASIC-like language interpreter
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

#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include "cps.hpp"

template <typename T, size_t size>
class RingQueue
{
	CPS_NOTCOPYABLE(RingQueue)
public:
	RingQueue() :
	_last(0), _length(0)
	{
	}
	
	~RingQueue() = default;
	
	constexpr size_t getSize() const { return size; }
	
	void insert(T element)
	{
		_data[_last] = element;
		_last = (_last+1)%size;
		if (_length < size)
			++_length;
	}
	
	unsigned length() const { return _length; }
	
	unsigned last() const { return _last; }
	
	unsigned first() const
	{
		unsigned result;
		
		if (_last >= _length)
			result = _last-_length;
		else
			result = size - _length + _last;
		
		return result;
	}
	
	T *data() { return _data; }
private:
	
	unsigned _last, _length;
	T _data[size];
};

#endif
