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

#ifndef LEXER_HPP
#define LEXER_HPP

#include <inttypes.h>
#include <stdlib.h>

#include "arduino_logger.hpp"
#include "basic_parser.hpp"
#include "basic_parser_value.hpp"
#include "helper.hpp"

namespace BASIC
{

#if ARDUINO_LOG
Logger&
operator<<(Logger &logger, Token tok);
#endif

/**
 * @brief Lexical analyzer class
 */
class Lexer
{
public:
	/**
	 * @brief lexical analyses stage errors
	 */
	enum Error : uint8_t
	{
		NO_ERROR = 0,
		STRING_OVERFLOW = 1
	};
	/**
	 * @brief initialize lexer session
	 * @param str null-terminating string to extract tokens from
	 */
	void init(const char*);
	/**
	 * @brief continue lexical analyze for next token
	 * @return string end flag
	 */
	bool getNext();
	/**
	 * @brief get last extracted token
	 * @return last token
	 */
	Token getToken() const { return _token; }
	/**
	 * @brief get last lexer error
	 * @return error code
	 */
	Error getError() const { return _error; }
	/**
	 * @brief get current value (numberm boolean...)
	 * @return value, extracted from string
	 */
	const Parser::Value &getValue() const { return _value; }
	/**
	 * @brief get current string (identifier)
	 * @return identifier string
	 */
	const char *id() const { return _id; }
	/**
	 * @brief get analised string position
	 * @return string position index
	 */
	uint8_t getPointer() const { return _pointer; }
	/**
	 * @brief token strings array
	 */
	static PGM_P const tokenStrings[];
	/**
	 * @brief Get null-terminated token string representation
	 * @param token Token code
	 * @param buf String buffer to copy to
	 * @return buffer pointer or nullptr if error
	 */
	static const uint8_t *getTokenString(Token, uint8_t*);
private:

	void pushSYM();
	void next();

	void fitst_LT();
	void fitst_GT();
	
	// Parse decimal number
	void decimalNumber();
	// Parse Binary number
	void binaryInteger();
#if USE_REALS
	bool numberScale();
#endif
	void ident();
	void stringConst();
	
	// analyzed string
	const char *_string;
	// analysed string position
	uint8_t _pointer;
	// current value
	Parser::Value _value;
	// current identifier string
	char _id[STRINGSIZE];
	// identifier string pointer
	uint8_t _valuePointer;
	Token _token;
	Error	_error;
};

} // namespace BASIC

#endif
