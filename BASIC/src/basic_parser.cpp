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

#include <math.h>
#include <string.h>
#include <signal.h>

#include "basic_parser.hpp"
#include "basic_interpreter.hpp"
#include "basic_program.hpp"
#include "ascii.hpp"

/*
 * TEXT = OPERATORS | C_INTEGER OPERATORS
 * OPERATORS = OPERATOR | OPERATOR COLON OPERATORS
 * OPERATOR =
 *	KW_DIM ARRAYS_LIST |
 *      KW_END |
 *	KW_FOR FOR_CONDS |
 *	KW_GOSUB EXPRESSION |
 *	KW_IF EXPRESSION IF_STATEMENT |
 *	KW_INPUT VAR_LIST |
 *	KW_LET IMPLICIT_ASSIGNMENT |
 *	KW_NEXT IDENT |
 *	KW_PRINT | KW_PRINT PRINT_LIST |
 *	KW_REM TEXT |
 *	KW_RETURN |
 *	KW_RANDOMIZE |
 *	GOTO_STATEMENT |
 *	COMMAND |
 *	KW_MAT MATRIX_OPERATION
 * COMMAND = COM_DUMP | COM_DUMP KW_VARS | COM_DUMP KW_ARRAYS
 *	COM_LIST | COM_NEW | COM_RUN | COM_SAVE | COM_LOAD
 * ASSIGNMENT = KW_LET IMPLICIT_ASSIGNMENT | IMPLICIT_ASSIGNMENT
 * IMPLICIT_ASSIGNMENT = VAR EQUALS EXPRESSION | VAR ARRAY EQUALS EXPRESSION
 * EXPRESSION = SIMPLE_EXPRESSION | SIMPLE_EXPRESSION REL SIMPLE_EXPRESSION
 * REL = LT | LTE | EQUALS | GT | GTE | NE | NEA
 * SIMPLE_EXPRESSION = TERM | TERM ADD TERM
 * ADD = PLUS MINUS
 * TERM = FACTOR | FACTOR MUL FACTOR
 * MUL = STAR | SLASH | DIV | MOD
 * FACTOR = FINAL | FINAL POW FINAL
 * FINAL = C_INTEGER | C_REAL | C_STRING | VAR | VAR ARRAY |
 *	LPAREN EXPRESSION RPAREN | MINUS FINAL
 * VAR = REAL_IDENT | INTEGER_IDENT | STRING_IDENT
 * VAR_LIST = VAR | VAR VAR_LIST
 * PRINT_LIST = EXPRESSION | EXPRESSION COMMA PRINT_LIST
 * IF_STATEMENT = GOTO_STATEMEMNT | KW_THEN OPERATORS
 * GOTO_STATEMENT = KW_GOTO C_INTEGER
 * FOR_CONDS = IMPLICIT_ASSIGNMENT KW_TO EXPRESSION |
 *	IMPLICIT_ASSIGNMENT KW_TO EXPRESSION KW_STEP EXPRESSION
 * ARRAYS_LIST = VAR ARRAY | VAR ARRAY ARRAYS_LIST
 * ARRAY = LPAREN DIMENSIONS RPAREN
 * DIMENSIONS = C_INTEGER | C_INTEGER COMMA DIMENSIONS
 * MATRIX_OPERATION =
 *      PRINT MATRIX_PRINT |
 *	DET VAR
 *      VAR EQUALS MATRIX_EXPRESSION
 */

namespace BASIC
{

#if CONF_ERROR_STRINGS

#if (LANG == LANG_RU)
#include "strings_ru_koi8r.hpp"
#elif (LANG == LANG_EN)
#include "strings_en.hpp"
#endif

static const char noerror[] PROGMEM = STR_NO_ERROR;
static const char operexp[] PROGMEM = STR_OPERATOR_EXPECTED;
static const char exprexp[] PROGMEM = STR_EXPRESSION_EXPECTED;
static const char intexp[] PROGMEM = STR_INTEGER_CONSTANT_EXPECTED;
static const char thengtexp[] PROGMEM = STR_THEN_OR_GOTO_EXPECTED;
static const char invdata[] PROGMEM = STR_INVALID_DATA_EXPRESSION;
static const char invread[] PROGMEM = STR_INVALID_READ_EXPRESSION;
static const char varlistexp[] PROGMEM = STR_VARIABLES_LIST_EXPECTED;
static const char stringovf[] PROGMEM = STR_STRING_OVERFLOW;

PGM_P const Parser::errorStrings[] PROGMEM = {
	noerror,
	operexp,
	exprexp,
	intexp,
	thengtexp,
	invdata,
	invread,
	varlistexp,
	stringovf
};
#endif // CONF_ERROR_STRINGS

Parser::Parser(Lexer &l, Interpreter &i) :
_lexer(l), _interpreter(i), _mode(EXECUTE)
{
}

void Parser::init()
{
	_mode = EXECUTE;
	_internal.init();
}

void Parser::addModule(FunctionBlock *module)
{
	_internal.setNext(module);
}

void
Parser::stop()
{
	_stopParse = true;
}

bool
Parser::parse(const char *s, bool &ok)
{
	LOG_TRACE;

	_lexer.init(s);
	_stopParse = false;
	_error = NO_ERROR;
	
	if (_lexer.getNext())
		return fOperators(ok);
	else {
		ok = true;
		return false;
	}
}

/*
 * OPERATORS = OPERATOR | OPERATOR COLON OPERATORS
 */
bool
Parser::fOperators(bool &ok)
{
	if (!fOperator()) {
		if (_error == NO_ERROR)
			_error = OPERATOR_EXPECTED;
		ok = false;
		return true;
	}
	if (_stopParse) {
		ok = true;
		return false;
	}
	
	const Token t = _lexer.getToken();
	if (t == Token::COLON) {
		ok = true;
		return true;
	} else if (t == Token::NOTOKENS) {
		ok = true;
		return false;
	} else {
		ok = false;
		return true;
	}
}

bool
Parser::testExpression(Value &v)
{
	if (!_lexer.getNext() || !fExpression(v)) {
		_error = EXPRESSION_EXPECTED;
		return false;
	}
	return true;
}

/*
 * OPERATOR =
 *	KW_DIM ARRAYS_LIST |
 *	KW_DATA DATA_STATEMENT |
 *      KW_END |
 *	KW_STOP |
 *	KW_FOR FOR_CONDS |
 *	KW_GOSUB EXPRESSION |
 *	KW_IF EXPRESSION IF_STATEMENT |
 *	KW_INPUT VAR_LIST |
 *	KW_LET IMPLICIT_ASSIGNMENT |
 *	KW_NEXT IDENT |
 *	KW_PRINT | KW_PRINT PRINT_LIST |
 *	KW_REM TEXT |
 *	KW_RETURN |
 *	KW_RANDOMIZE |
 *	GOTO_STATEMENT |
 *	COMMAND |
 *      KW_MAT MATRIX_OPERATION
 */
bool
Parser::fOperator()
{
	LOG_TRACE;

	const Token t = _lexer.getToken();
	LOG(t);
	switch (t) {
	case Token::KW_DIM:
		if (_lexer.getNext())
			return fArrayList();
		return false;
#if USE_DEFFN
	case Token::KW_DEF:
		if (_lexer.getNext())
			return fDefStatement();
		return false;
#endif // USE_DEFFN
#if USE_DATA
	case Token::KW_DATA: {
		if (_mode == EXECUTE)
			_mode = SCAN;
		if (!_lexer.getNext())
			return false;
		bool res = fDataStatement();
		if (!res)
			_error = INVALID_DATA_EXPR;
		if (_mode == SCAN)
			_mode = EXECUTE;
		return res;
	}
#endif // USE_DATA
	case Token::KW_END:
		_interpreter._program.reset();
#if USESTOPCONT
	case Token::KW_STOP:
#endif
		if (_mode == EXECUTE)
			_interpreter.end();
		_stopParse = true;
		_lexer.getNext();
		break;
	case Token::KW_FOR:
		if (_lexer.getNext())
			return fForConds();
		return false;
	case Token::KW_ON: {
		Value v;
		if (!_lexer.getNext() || !fExpression(v)) {
			_error = EXPRESSION_EXPECTED;
			return false;
		}
		bool res = fOnStatement(uint8_t(INT(v)));
		_stopParse = true;
		return res;
	}
	case Token::KW_GOSUB: {
		Value v;
		if (!_lexer.getNext() || !fExpression(v)) {
			_error = EXPRESSION_EXPECTED;
			return false;
		}
		if (_mode == EXECUTE) {
			_interpreter.pushReturnAddress();
			_interpreter.gotoLine(v);
		}
		_stopParse = true;
		break;
	}
	case Token::KW_IF: {
		Value v;
		if (!_lexer.getNext() || !fExpression(v)) {
			_error = EXPRESSION_EXPECTED;
			return false;
		}
		bool res;
		if (_mode == EXECUTE) {
			if (!bool(v))
				_mode = SCAN;
			res = fIfStatement();
			_mode = EXECUTE;
		} else
			res = fIfStatement();
		if (!res)
			_error = THEN_OR_GOTO_EXPECTED;
		return res;
	}
	case Token::KW_INPUT:
		_lexer.getNext();
#if INPUT_WITH_TEXT
		if (_lexer.getToken() == Token::C_STRING) {
		    _interpreter.print(_lexer.id());
			if (!_lexer.getNext() || _lexer.getToken() !=
			    Token::SEMI || !_lexer.getNext()) {
				_error = VARIABLES_LIST_EXPECTED;
				return false;
			}
		}
#endif // INPUT_WITH_TEXT
		if (!fVarList()) {
			_error = VARIABLES_LIST_EXPECTED;
			return false;
		} else if (_mode == EXECUTE)
			_interpreter.input();
		break;
	case Token::KW_LET: {
		char vName[IDSIZE];
		if (!_lexer.getNext() || !fImplicitAssignment(vName))
			return false;
	}
		break;
#if USE_MATRIX
	case Token::KW_MAT:
		if (!_lexer.getNext() || !fMatrixOperation())
			return false;
		break;
#endif
	case Token::KW_NEXT: {
		char vName[IDSIZE];
		if (!_lexer.getNext() || !fIdentifier(vName))
			return false;
		if (_mode == EXECUTE) {
			vName[VARSIZE-1] = '\0';
			_stopParse = !_interpreter.next(vName);
		} else
			_mode = EXECUTE;
		if (!_stopParse)
			_lexer.getNext();
	}
		break;
	case Token::KW_PRINT:
		if (_lexer.getNext()) {
			if (!fPrintList())
				return false;
		} else if (_mode == EXECUTE)
			_interpreter.newline();
		break;
#if USE_RANDOM
	case Token::KW_RANDOMIZE:
		if (_mode == EXECUTE)
			_interpreter.randomize();
		_lexer.getNext();
		break;
#endif // USE_RANDOM
#if USE_DATA
	case Token::KW_READ : {
		if (!_lexer.getNext())
			return false;
		bool res = fReadStatement();
		if (!res)
			_error = INVALID_READ_EXPR;
		return res;
	}
#endif // USE_DATA
	case Token::KW_REM:
		while (_lexer.getNext());
		break;
#if USE_DATA
	case Token::KW_RESTORE:
		if (_mode == EXECUTE)
			_interpreter.restore();
		_lexer.getNext();
		break;
#endif // USE_DATA
	case Token::KW_RETURN:
		if (_mode == EXECUTE) {
			_interpreter.returnFromSub();
			_stopParse = true;
		}
		_lexer.getNext();
		break;
	default:
		if (fCommand() || fGotoStatement())
			break;
		{
			char vName[IDSIZE];
			if (fImplicitAssignment(vName))
				break;
			else
				return false;
		}
		_error = OPERATOR_EXPECTED;
		return false;
	}
//#if USE_DEFFN
//	if (t == TToken::KW_DEF) {
//		if (_lexer.getNext())
//			return fDefStatement();
//		return false;
//	} else
//#endif // USE_DEFFN
//	if (t == Token::KW_DIM) {
//		if (_lexer.getNext())
//			return fArrayList();
//		return false;
//	} else if (t == Token::KW_END) {
//		if (_mode == EXECUTE) {
//			_interpreter._program.reset();
//			_interpreter.end();
//		}
//		_stopParse = true;
//		_lexer.getNext();
//	} else if (t == Token::KW_STOP) {
//		if (_mode == EXECUTE)
//			_interpreter.end();
//		_stopParse = true;
//		_lexer.getNext();
//	} else if (t == Token::KW_FOR) {
//		if (_lexer.getNext())
//			return fForConds();
//		return false;
//	} else if (t == Token::KW_GOSUB) {
//		Value v;
//		if (!_lexer.getNext() || !fExpression(v)) {
//			_error = EXPRESSION_EXPECTED;
//			return false;
//		}
//		if (_mode == EXECUTE) {
//			_interpreter.pushReturnAddress(_lexer.getPointer());
//			_interpreter.gotoLine(v);
//		}
//		_stopParse = true;
//	} else if (t == Token::KW_IF) {
//		Value v;
//		if (!_lexer.getNext() || !fExpression(v)) {
//			_error = EXPRESSION_EXPECTED;
//			return false;
//		}
//		bool res;
//		if (!bool(v))
//			_mode = SCAN;
//		if (fIfStatement())
//			res = true;
//		else {
//			_error = THEN_OR_GOTO_EXPECTED;
//			res = false;
//		}
//		_mode = EXECUTE;
//		return res;
//	} else if (t == Token::KW_INPUT) {
//		_lexer.getNext();
//#if INPUT_WITH_TEXT
//		if (_lexer.getToken() == Token::C_STRING) {
//		    _interpreter.print(_lexer.id());
//			if (!_lexer.getNext() || _lexer.getToken() !=
//			    Token::SEMI || !_lexer.getNext()) {
//				_error = VARIABLES_LIST_EXPECTED;
//				return false;
//			}
//		}
//#endif // INPUT_WITH_TEXT
//		if (!fVarList()) {
//			_error = VARIABLES_LIST_EXPECTED;
//			return false;
//		} else if (_mode == EXECUTE)
//			_interpreter.input();
//	} else if (t == Token::KW_LET) {
//		char vName[IDSIZE];
//		if (!_lexer.getNext() || !fImplicitAssignment(vName))
//			return false;
//	}
//#if USE_MATRIX
//	 else if (t == Token::KW_MAT)
//		return _lexer.getNext() && fMatrixOperation();
//#endif
//	else if (t == Token::KW_NEXT) {
//		char vName[IDSIZE];
//		if (!_lexer.getNext() || !fVar(vName))
//			return false;
//		if (_mode == EXECUTE)
//			_stopParse = !_interpreter.next(_lexer.id());
//		if (!_stopParse)
//			_lexer.getNext();
//	} else if (t == Token::KW_PRINT) {
//		if (_lexer.getNext())
//			return fPrintList();
//		else
//			_interpreter.newline();
//	}
//#if USE_RANDOM
//	else if (t == Token::KW_RANDOMIZE) {
//		if (_mode == EXECUTE)
//			_interpreter.randomize();
//		_lexer.getNext();
//	}
//#endif
//	else if (t == Token::KW_REM)
//		while (_lexer.getNext());
//	else if (t == Token::KW_RETURN) {
//		if (_mode == EXECUTE) {
//			_interpreter.returnFromSub();
//			_stopParse = true;
//		}
//		_lexer.getNext();
//	} else {
//		if (fGotoStatement() || fCommand())
//			return true;
//		{
//			char vName[IDSIZE];
//			if (fImplicitAssignment(vName))
//				return true;
//		}
//		_error = OPERATOR_EXPECTED;
//		return false;
//	}
	return true;
}

bool
Parser::fOnStatement(uint8_t index)
{
 	if (_lexer.getToken() == Token::KW_GOTO) {
		while (index-- > 0) {
			if (_lexer.getNext() &&
			    _lexer.getToken() == Token::C_INTEGER) {
				if (index == 0) {
					_interpreter.gotoLine(_lexer.getValue());
					return true;
				} else if (index > 0 && _lexer.getNext() &&
					_lexer.getToken() == Token::COMMA)
					continue;
			} else
				break;
		}
	}
	return false;
}

#if USE_DATA
bool
Parser::fDataStatement()
{
	Token t;
	while (true) {
		t = _lexer.getToken();
		if (t == Token::MINUS) {
			_lexer.getNext();
			t = _lexer.getToken();
		}
		if ((t >= Token::C_INTEGER && t <= Token::C_STRING)
		 || (t == Token::KW_TRUE)
		 || (t == Token::KW_FALSE)) {
			if (_lexer.getNext()) {
				t = _lexer.getToken();
				if (t == Token::COLON)
					break;
				else if (t == Token::COMMA) {
					if (_lexer.getNext())
						continue;
				}
			} else
				break;
		}
		return false;
	}
	return true;
}

bool
Parser::fReadStatement()
{
	Token t;
	while (true) {
		char varName[IDSIZE];
		if (fIdentifier(varName)) {
			uint8_t dimensions;
			bool array;
                        _lexer.getNext();
			if (_lexer.getToken() == Token::LPAREN) {
				if (fArray(dimensions))
					array = true;
				else
					return false;
			} else
				array = false;
			if (_mode == EXECUTE) {
				varName[VARSIZE-1] = '\0';
				Value v;
				const bool res = _interpreter.read(v);
				if (!res)
					return false;
				
				if (array)
					_interpreter.setArrayElement(varName, v);
				else
					_interpreter.setVariable(varName, v);
			}

			t = _lexer.getToken();
			if (t == Token::COLON)
				break;
			else if (t == Token::COMMA) {
				if (_lexer.getNext())
					continue;
			}
			if (!_lexer.getNext())
				break;
		}
		return false;
	}
	return true;
}
#endif // USE_DATA

#if USE_DEFFN
bool
Parser::fDefStatement()
{
	// nex keyword must be FN
	if (_lexer.getToken() == Token::KW_FN && _lexer.getNext()) {
		char buf[IDSIZE];
		if ((fIdentifier(buf)) && _lexer.getNext() &&
		    (_lexer.getToken() == Token::LPAREN) && _lexer.getNext() &&
		    (_lexer.getToken() == Token::RPAREN) && _lexer.getNext() &&
		    (_lexer.getToken() == Token::EQUALS)) {
			
			if (_mode == EXECUTE) {
				_mode = SCAN;
				_interpreter.newFunction(buf);
				if (!_lexer.getNext())
					return false;
				Value v;
				const bool res = fExpression(v);
				_mode = EXECUTE;
				return res;
			}
			return true;
		}
	}
	return false;
}
#endif // USE_DEFFN

/*
 * IMPLICIT_ASSIGNMENT =
 * VAR EQUALS EXPRESSION |
 * VAR ARRAY EQUALS EXPRESSION
 */
bool
Parser::fImplicitAssignment(char *varName)
{
	LOG_TRACE;

	if (fIdentifier(varName) && _lexer.getNext()) {
		bool array;
		if (_lexer.getToken() == Token::LPAREN) {
			uint8_t dimensions;
			if (fArray(dimensions))
				array = true;
			else
				return false;
		} else
			array = false;
		
		Value v;
		if (_lexer.getToken() == Token::EQUALS) {
			if (_lexer.getNext() && fExpression(v)) {
				if (_mode == EXECUTE) {
					varName[VARSIZE-1] = '\0';
					if (array)
						_interpreter.setArrayElement(
						    varName, v);
					else
						_interpreter.setVariable(
						    varName, v);
				}
				return true;
			} else
				_error = EXPRESSION_EXPECTED;
		}
	}
	return false;
}

/*
 * PRINT_LIST =
 * PRINT_ITEM |
 * PRINT_ITEM COMMA PRINT_LIST |
 * PRINT_ITEM SEMI PRINT_LIST
 */
bool
Parser::fPrintList()
{
	LOG_TRACE;
	
	if (!fPrintItem()) 
		return false;
	
	while (true) {
		const Token t = _lexer.getToken();
		switch (t) {
		case Token::COMMA:
			if (_mode == EXECUTE)
				_interpreter.print(char(ASCII::HT));
			_lexer.getNext();
			if (!fPrintItem())
				return false;
			break;
		case Token::SEMI:
			if (_lexer.getNext() && _lexer.getToken() != Token::COLON) {
				if (!fPrintItem())
					return false;
			} else
				return true;
			break;
		default:
			if (_mode == EXECUTE)
				_interpreter.newline();
			return true;
		}
	}
}

/*
 * PRINT_ITEM =
 * KW_TAB LPAREN EXPRESSION RPAREN |
 * EXPRESSION
 */
bool
Parser::fPrintItem()
{
	const Token t = _lexer.getToken();
	if (t != Token::NOTOKENS &&t != Token::COMMA && t != Token::COLON) { // printable tokens
		Value v;
#if USE_TEXTATTRIBUTES
		if (t == Token::KW_TAB
#if CONF_USE_SPC_PRINT_COM
		    || t == Token::KW_SPC
#endif
		    ) {
			const bool flag = (t == Token::KW_TAB);
			if (_lexer.getNext() && _lexer.getToken() == Token::LPAREN &&
			    _lexer.getNext() && fExpression(v) &&
			    _lexer.getToken() == Token::RPAREN) {
				if (_mode == EXECUTE)
					_interpreter.printTab(v, flag);
				_lexer.getNext();
			} else
				return false;
		} else
#endif // USE_TEXTATTRIBUTES
		{
			if (!fExpression(v)) {
				if (_error == NO_ERROR)
					_error = EXPRESSION_EXPECTED;
				return false;
			}

			if (_mode == EXECUTE)
				_interpreter.print(v);
		}
	}
	return true;
}

/*
 * EXPRESSION =
 *	OP_NOT EXPRESSION |
 *	LOGICAL_ADD_EXPRESSION |
 *	LOGICAL_ADD_EXPRESSION OP_OR LOGICAL_ADD_EXPRESSION
 */
bool
Parser::fExpression(Value &v)
{
	LOG_TRACE;
	
	if (_lexer.getToken() == Token::OP_NOT) {
		if (!_lexer.getNext() || !fExpression(v))
			return false;
		if (_mode == EXECUTE)
			v.notOp();
		return true;
	}
	
	if (!fLogicalAdd(v))
		return false;

	while (true) {
		const Token t = _lexer.getToken();
		Value v2;
		if (t == Token::OP_OR) {
			if (!_lexer.getNext() || !fLogicalAdd(v2))
				return false;
			
			if (_mode != Mode::EXECUTE)
				continue;
			v |= v2;
		} else
			return true;
	}
}

/*
 * LOGICAL_ADD_EXPRESSION =
 *	LOGICAL_FINAL_EXPRESSION |
 *	LOGICAL_FINAL_EXPRESSION OP_AND LOGICAL_FINAL_EXPRESSION
 */
bool
Parser::fLogicalAdd(Value &v)
{
	LOG_TRACE;
	
	if (!fLogicalFinal(v))
		return false;

	while (true) {
		const Token t = _lexer.getToken();
		Value v2;
		if (t == Token::OP_AND) {
			if (!_lexer.getNext() || !fLogicalFinal(v2))
				return false;
			
			if (_mode != Mode::EXECUTE)
				continue;
			v &= v2;
		} else
			return true;
	}
}

/*
 * LOGICAL_FINAL_EXPRESSION =
 *	SIMPLE_EXPRESSION |
 *	SIMPLE_EXPRESSION REL SIMPLE_EXPRESSION
 */
bool
Parser::fLogicalFinal(Value &v)
{
	LOG_TRACE;
	
	if (!fSimpleExpression(v))
		return false;

	while (true) {
		const Token t = _lexer.getToken();
		Value v2;
#if OPT == OPT_SPEED
		switch (t) {
		case Token::LT:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				if (_mode == Mode::EXECUTE)
					v = v < v2;
				continue;
			} else
				return false;
		case Token::LTE:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				if (_mode == Mode::EXECUTE)
					v = (v < v2) || (v == v2);
				continue;
			} else
				return false;
		case Token::GT:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				if (_mode == Mode::EXECUTE)
					v = v > v2;
				continue;
			} else
				return false;
		case Token::GTE:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				if (_mode == Mode::EXECUTE)
					v = (v > v2) || (v == v2);
				continue;
			} else
				return false;
		case Token::EQUALS:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				if (_mode == Mode::EXECUTE) {
#if USE_STRINGOPS
					if (v.type == Value::STRING &&
					   v2.type == Value::STRING)
						v = _interpreter.strCmp();
					else
#endif
						v = v == v2;
				}
				continue;
			} else
				return false;
		case Token::NE:
#if CONF_USE_ALTERNATIVE_NE
		case Token::NEA:
#endif
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				if (_mode == Mode::EXECUTE)
					v = !(v == v2);
				continue;
			} else
				return false;
		default:
			return true;
		}
#else
		if (t == Token::LT || t == Token::LTE || t == Token::GT
		 || t == Token::GTE || t == Token::EQUALS || t == Token::NE
#if CONF_USE_ALTERNATIVE_NE
		 || t == Token::NEA
#endif
		    ) {
			if (!_lexer.getNext() || !fSimpleExpression(v2))
				return false;
			
			if (_mode != Mode::EXECUTE)
				continue;
			
			if (t == Token::LT)
				v = v < v2;
			else if (t == Token::LTE)
				v = (v < v2) || (v == v2);
			else if (t == Token::GT)
				v = v > v2;
			else if (t ==Token::GTE)
				v = (v > v2) || (v == v2);
			else if (t == Token::EQUALS) {
#if USE_STRINGOPS
				if (v.type == Value::STRING &&
				    v2.type == Value::STRING)
					v = _interpreter.strCmp();
				else
#endif // USE_STRINGOPS
					v = v == v2;
			} else if (t == Token::NE
#if CONF_USE_ALTERNATIVE_NE
				|| t == Token::NEA
#endif
			    ) {
#if USE_STRINGOPS
				if (v.type == Value::STRING &&
				    v2.type == Value::STRING)
					v = !_interpreter.strCmp();
				else
#endif // USE_STRINGOPS
					v = !(v == v2);
			}
		} else
			return true;
#endif
	}
}

/*
 * SIMPLE_EXPRESSION =
 *	TERM |
 *	TERM ADD TERM
 */
bool
Parser::fSimpleExpression(Value &v)
{
	LOG_TRACE;

	if (!fTerm(v))
		return false;

	while (true) {
		const Token t = _lexer.getToken();
		LOG(t);
		Value v2;
#if OPT == OPT_SPEED
		switch (t) {
		case Token::PLUS:
			if (_lexer.getNext() && fTerm(v2)) {
				if (_mode == Mode::EXECUTE) {
#if USE_STRINGOPS
					if (v.type == Value::STRING &&
						v2.type == Value::STRING)
						_interpreter.strConcat();
					else
#endif // USE_STRINGOPS
						v += v2;
				}
					continue;
			} else
				return false;
		case Token::MINUS:
			if (_lexer.getNext() && fTerm(v2)) {
				if (_mode == Mode::EXECUTE)
					v -= v2;
				continue;
			} else
				return false;
		default:
			return true;
		}
#else
		if (t == Token::PLUS || t == Token::MINUS) {
			if (!_lexer.getNext() || !fTerm(v2))
				return false;
			if (_mode != Mode::EXECUTE)
				continue;
			if ((t == Token::PLUS)) {
#if USE_STRINGOPS
				if (v.type == Value::STRING &&
				    v2.type == Value::STRING)
					_interpreter.strConcat();
				else
#endif // USE_STRINGOPS
					v += v2;
			} else if (t == Token::MINUS)
				v -= v2;
		} else
			return true;
#endif // OPT == OPT_SPEED
	}
}

/*
 * TERM =
 *     FACTOR |
 *     FACTOR MUL FACTOR
 */
bool
Parser::fTerm(Value &v)
{
	LOG_TRACE;

	if (!fFactor(v))
		return false;

	while (true) {
		const Token t = _lexer.getToken();
		LOG(t);
		Value v2;
#if OPT == OPT_SPEED
		switch (t) {
		case Token::STAR:
			if (_lexer.getNext() && fFactor(v2)) {
				v *= v2;
				continue;
			} else
				return false;
		case Token::SLASH:
			if (_lexer.getNext() && fFactor(v2)) {
				v /= v2;
				continue;
			} else
				return false;
#if USE_INTEGER_DIV
#if USE_REALS
		case Token::BACK_SLASH:
#if USE_DIV_KW
		case Token::KW_DIV:
#endif // USE_DIV_KW
			if (_lexer.getNext() && fFactor(v2)) {
				v.divEquals(v2);
				continue;
			} else
				return false;
#endif // USE_REALS
		case Token::KW_MOD:
			if (_lexer.getNext() && fFactor(v2)) {
				v.modEquals(v2);
				continue;
			} else
				return false;
#endif // USE_INTEGER_DIV
		default:
			return true;
		}
#else
		if (t == Token::STAR || t == Token::SLASH
#if USE_INTEGER_DIV
#if USE_REALS
		 || t == Token::BACK_SLASH
#if USE_DIV_KW
		 || t == Token::KW_DIV
#endif // USE_DIV_KW
#endif // USE_REALS
		 || t == Token::KW_MOD
#endif // USE_INTEGER_DIV
		   ) {
			if (!_lexer.getNext() || !fFactor(v2))
				return false;
			
			if (_mode != Mode::EXECUTE)
				continue;
			
			if (t == Token::STAR)
				v *= v2;
			else if (t == Token::SLASH)
				v /= v2;
#if USE_INTEGER_DIV
#if USE_REALS
			else if (t == Token::BACK_SLASH
#if USE_DIV_KW
			      || t == Token::KW_DIV
#endif // USE_DIV_KW
			)
				v.divEquals(v2);
#endif // USE_REALS
			else if (t == Token::KW_MOD)
				v.modEquals(v2);
#endif // USE_INTEGER_DIV
		} else
			return true;
#endif // OPT == OPT_SPEED
	}
}

/*
 * FACTOR =
 *     ADD FACTOR |
 *     FINAL |
 *     FINAL POW FINAL
 */
bool
Parser::fFactor(Value &v)
{
	LOG_TRACE;
	
	const Token t = _lexer.getToken();
	if (t == Token::PLUS) { // Unary plus, ignored
		return _lexer.getNext() && fFactor(v);
	} else if (t == Token::MINUS) { // Unary minus, switch sign
		if (!_lexer.getNext() || !fFactor(v))
			return false;
		if (_mode == EXECUTE)
			v.switchSign();
		return true;
	}

	if (!fFinal(v))
		return false;

	while (true) {
		const Token t = _lexer.getToken();
		LOG(t);
		if (t == Token::POW) {
			Value v2;
			if (_lexer.getNext() && fFinal(v2)) {
				if (_mode == Mode::EXECUTE)
					v ^= v2;
			} else
				return false;
		} else
			return true;
	}
}

/* FINAL =
 *     C_INTEGER | C_REAL | C_STRING | VAR | VAR ARRAY |
 *     LPAREN EXPRESSION RPAREN
 */
bool
Parser::fFinal(Value &v)
{
	LOG_TRACE;

	const Token t = _lexer.getToken();
	LOG(t);

	while (true) {
#if OPT == OPT_SPEED
		switch (t) {
		case Token::PLUS:
			return _lexer.getNext() && fFinal(v);
		case Token::MINUS:
			if (!_lexer.getNext() || !fFinal(v))
				return false;
			if (_mode == EXECUTE)
				v.switchSign();
			return true;
		case Token::C_INTEGER:
		case Token::C_REAL:
		case Token::C_BOOLEAN:
			if (_mode == EXECUTE)
				v = _lexer.getValue();
			_lexer.getNext();
			return true;
		case Token::C_STRING:
			if (_lexer.getError() == Lexer::STRING_OVERFLOW) {
				_error = STRING_OVERFLOW;
				_lexer.getNext();
				return false;
			}
			if (_mode == EXECUTE) {
				_interpreter.pushString(_lexer.id());
				v.type = Value::Type::STRING;
			}
			_lexer.getNext();
			return true;
		case Token::LPAREN:
			if (!_lexer.getNext() || !fExpression(v))
				return false;
			if (_lexer.getToken() != Token::RPAREN)
				return false;
			else {
				_lexer.getNext();
				return true;
			}
		default:
		{
			char varName[IDSIZE];
			if (fIdentifier(varName))
				return fIdentifierExpr(varName, v);
		}
			return false;
		}
#else
		if (t == Token::C_INTEGER || t == Token::C_REAL ||
		    t == Token::C_BOOLEAN) {
			if (_mode == EXECUTE)
				v = _lexer.getValue();
			_lexer.getNext();
			return true;
		} else if (t == Token::C_STRING) {
			if (_lexer.getError() == Lexer::STRING_OVERFLOW) {
				_error = STRING_OVERFLOW;
				_lexer.getNext();
				return false;
			}
			if (_mode == EXECUTE) {
				_interpreter.pushString(_lexer.id());
				v.type = Value::Type::STRING;
			}
			_lexer.getNext();
			return true;
		} else if (t == Token::LPAREN) {
			if (!_lexer.getNext() || !fExpression(v))
				return false;
			if (_lexer.getToken() != Token::RPAREN) {
				_error = MISSING_RPAREN;
				return false;
			} else {
				_lexer.getNext();
				return true;
			}
		}
#if USE_DEFFN
		else if (t == Token::KW_FN) {
			char varName[IDSIZE];
			if (_lexer.getNext() && fIdentifier(varName) &&
			    _lexer.getNext() && (_lexer.getToken() == Token::LPAREN) &&
			    _lexer.getNext() && (_lexer.getToken() == Token::RPAREN)) {
				if (_mode == EXECUTE) {
					m_definedFunctionExecute = true;
					_interpreter.pushReturnAddress();
					_interpreter.execFn(varName);
					_lexer.getNext();
					fExpression(v);
					_interpreter.returnFromFn();
					_lexer.getNext();
				}
				_stopParse = true;
				return true;
			}
			return false;
		}
#endif // USE_DEFFN
		 else {
			char varName[IDSIZE];
			if (fIdentifier(varName))
				return fIdentifierExpr(varName, v);
			return false;
		}
#endif
	}
}

bool
Parser::fIfStatement()
{
	const Token t = _lexer.getToken();
	LOG(t);
	if (t == Token::KW_THEN) {
		if (_lexer.getNext()) {
			if (_lexer.getToken() == Token::C_INTEGER) {
				if (_mode == EXECUTE)
					_interpreter.gotoLine(_lexer.getValue());
				_lexer.getNext();
				return true;
			} else {
				bool res;
				while (fOperators(res)) {
					if (!res)
						return false;
					else
						_lexer.getNext();
				}
				return true;
			}
		}
	} else if (fGotoStatement())
		return true;
	
	return false;
}

bool
Parser::fGotoStatement()
{
	Token t = _lexer.getToken();
	LOG(t);
	
#if CONF_SEPARATE_GO_TO
	if (t == Token::KW_GO) {
		_lexer.getNext();
		t = _lexer.getToken();
	}
#endif
	
	if (t == Token::KW_GOTO
#if CONF_SEPARATE_GO_TO
	 || t == Token::KW_TO
#endif
	    ) {
		Value v;
		if (!_lexer.getNext() || !fExpression(v)) {
			_error = EXPRESSION_EXPECTED;
			return false;
		}
		if (_mode == EXECUTE)
			_interpreter.gotoLine(v.value.integer);
		return true;
	} else
		return false;
}

bool
Parser::fCommand()
{
	const Token t = _lexer.getToken();
	LOG(t);
	typedef void (Interpreter::*func)();
	func f = nullptr;
	switch (t) {
#if USE_SAVE_LOAD
	case Token::COM_CHAIN:
		f = &Interpreter::chain;
		break;
#endif
#if USE_TEXTATTRIBUTES
	case Token::COM_CLS:
		f = &Interpreter::cls;
		break;
#endif
#if USESTOPCONT
	case Token::COM_CONT:
		f = &Interpreter::cont;
		break;
#endif
#if USE_DUMP
	case Token::COM_DUMP:
	{
		Interpreter::DumpMode mode = Interpreter::MEMORY;
		if (_lexer.getNext()) {
			if (_lexer.getToken() == Token::KW_VARS) {
				mode = Interpreter::VARS;
				_lexer.getNext();
			} else if (_lexer.getToken() == Token::KW_ARRAYS) {
				mode = Interpreter::ARRAYS;
				_lexer.getNext();
			}
		}
		if (_mode == EXECUTE)
			_interpreter.dump(mode);
		return true;
	}
#endif
#if USE_DELAY
	case Token::COM_DELAY: {
		Parser::Value v;
		if (_lexer.getNext() && fExpression(v)) {
			if (_mode == EXECUTE)
				_interpreter.delay(Integer(v));
			return true;
		} else
			return false;
	}
#endif
	case Token::COM_LIST:
	{
		Integer start = 1, stop = 0;
		_lexer.getNext(); // Result is not valuable
		if (_lexer.getToken() == Token::C_INTEGER) {
			start = Integer(_lexer.getValue());
			stop = start;
			_lexer.getNext();
		}
		if (_lexer.getToken() == Token::MINUS) {
			if (!_lexer.getNext() || _lexer.getToken() !=
			    Token::C_INTEGER) {
				_error = INTEGER_CONSTANT_EXPECTED;
				return false;
			}
			stop = Integer(_lexer.getValue());
			_lexer.getNext();
		}
		if (_mode == EXECUTE)
			_interpreter.list(start, stop);
	}
		return true;
#if USE_SAVE_LOAD
	case Token::COM_LOAD:
		f = &Interpreter::load;
		break;
#endif
#if USE_TEXTATTRIBUTES
	case Token::COM_LOCATE: {
		Value v1,v2;
		if (_lexer.getNext() && fExpression(v1) &&
		    _lexer.getToken() == Token::COMMA && _lexer.getNext() &&
		    fExpression(v2)) {
			if (_mode == EXECUTE)
				_interpreter.locate(Integer(v1), Integer(v2));
			return true;
		} else
			return false;
	}
#endif
	case Token::COM_NEW:
		f = &Interpreter::newProgram;
		break;
	case Token::COM_RUN:
		f = &Interpreter::run;
		break;
#if USE_SAVE_LOAD
	case Token::COM_SAVE:
		f = &Interpreter::save;
		break;
#endif
	case Token::REAL_IDENT:
	case Token::INTEGER_IDENT:
		FunctionBlock::command c;
		if ((c=_internal.getCommand(_lexer.id())) != nullptr) {
			while (_lexer.getNext()) {
				Value v;
				// String value already on stack after fExpression
				if (fExpression(v)) {
					if (v.type != Value::STRING)
						_interpreter.pushValue(v);
				} else
					break;
				
				if (_lexer.getToken() == Token::COMMA)
					continue;
				else
					break;
			}
			_interpreter.execCommand(c);
			return true;
		}
	default:
		break;
	}
	if (f != nullptr) {
		if (_mode == EXECUTE)
			(_interpreter.*f)();
		_lexer.getNext();
		return true;
	}
	return false;
}

/*
 * FOR_CONDS =
 *     IMPLICIT_ASSIGNMENT KW_TO EXPRESSION |
 *     IMPLICIT_ASSIGNMENT KW_TO EXPRESSION KW_STEP EXPRESSION
 */
bool
Parser::fForConds()
{
	Value v;
	char vName[IDSIZE];
	if (!fImplicitAssignment(vName) ||
	    _lexer.getToken()!=Token::KW_TO || !_lexer.getNext() ||
	    !fExpression(v))
		return false;
	
	Value vStep(Integer(1));
	if (_lexer.getToken() == Token::KW_STEP && (!_lexer.getNext() ||
	    !fExpression(vStep)))
		return false;
	
	if (_mode == EXECUTE) {
		Program::StackFrame *f = _interpreter.pushForLoop(vName,
		    _lexer.getPointer(), v, vStep);
		if (f != nullptr) {
			if (_interpreter.testFor(*f))
				_mode = SCAN;
			else
				_stopParse = true;
		}
	}
	
	return true;
}

bool
Parser::fVarList()
{
	Token t;
	char varName[IDSIZE];
	do {
		if (!fIdentifier(varName))
			return false;
		if (_mode == EXECUTE) {
			varName[VARSIZE-1] = '\0';
			_interpreter.pushInputObject(varName);
		}
		if (!_lexer.getNext())
			return true;
		t = _lexer.getToken();
		_lexer.getNext();
	} while (t == Token::COMMA);
	return true;
}

bool
Parser::fIdentifier(char *idName)
{
	if ((_lexer.getToken() >= Token::INTEGER_IDENT) &&
	    (_lexer.getToken() <= Token::BOOL_IDENT)) {
		strncpy(idName, _lexer.id(), IDSIZE);
		idName[IDSIZE-1] = '\0';
		return true;
	} else
		return false;
}

bool
Parser::fArrayList()
{
	Token t;
	char arrName[IDSIZE];
	uint8_t dimensions;
	do {
		if (!fIdentifier(arrName) ||
		    !_lexer.getNext() || !fArray(dimensions))
			return false;
		if (_mode == Mode::EXECUTE) {
			_interpreter.pushDimensions(dimensions);
			arrName[VARSIZE-1] = '\0';
			_interpreter.newArray(arrName);
		}
		t = _lexer.getToken();
		if (t != Token::COMMA)
			return true;
		else
			if (!_lexer.getNext())
				return false;
	} while (true);
}

bool
Parser::fArray(uint8_t &dimensions)
{
	if (_lexer.getToken() != Token::LPAREN ||
	    !fDimensions(dimensions) || _lexer.getToken() != Token::RPAREN)
		return false;

	_lexer.getNext();
	return true;
}

bool
Parser::fDimensions(uint8_t &dimensions)
{
	Parser::Value v;
	dimensions = 0;
	do {
		if (!_lexer.getNext() || !fExpression(v))
			return false;
		if (_mode == Mode::EXECUTE)
			_interpreter.pushDimension(Integer(v));
		++dimensions;
	} while (_lexer.getToken() == Token::COMMA);
	return true;
}

bool
Parser::fIdentifierExpr(char *varName, Value &v)
{
	// Identifier, var or func or array ?
	if (_lexer.getNext() && _lexer.getToken()==
	    Token::LPAREN) { // ( - array or function
		FunctionBlock::function f;
		if ((f=_internal.getFunction(varName)) != nullptr) {
			// function
			Value arg;
			do {
				if (!_lexer.getNext())
					return false;
				else if (_lexer.getToken() == Token::RPAREN) {
					break;
				} else {
					if (!fExpression(arg))
						return false;
					if (_mode == Mode::EXECUTE)
						_interpreter.pushValue(arg);
				}
			} while (_lexer.getToken() == Token::COMMA);
			_lexer.getNext();
			if (_mode == EXECUTE) {
				bool result = true;
				result = ((*f)(_interpreter));
				if (!result || !_interpreter.popValue(v))
					return false;
			}
		} else { // No such function, array variable
			uint8_t dim;
			if (fArray(dim)) {
				if (_mode == EXECUTE) {
					varName[VARSIZE-1] = '\0';
					return _interpreter.valueFromArray(v,
					    varName);
				}
			} else
				return false;
		}
	} else // variable
		if (_mode == EXECUTE) {
			varName[VARSIZE-1] = '\0';
			_interpreter.valueFromVar(v, varName);
		}
	
	return true;
}

#if USE_MATRIX
/*
 * MATRIX_OPERATION =
 *     PRINT MATRIX_PRINT |
 *     DET VAR |
 *     VAR EQUALS MATRIX_EXPRESSION
 */
bool
Parser::fMatrixOperation()
{
	char buf[IDSIZE];
	if (fIdentifier(buf)) {
		if (!_lexer.getNext())
			return false;
		if (_lexer.getToken() == Token::EQUALS) {
			if (_lexer.getNext() && fMatrixExpression(buf)) {
				_lexer.getNext();
				return true;
			} else
				return false;
		} else
			return false;
	} else if (_lexer.getToken() == Token::KW_PRINT) {
		if (_lexer.getNext() && fMatrixPrint()) {
			_lexer.getNext();
			return true;
		}
	} else if (_lexer.getToken() == Token::KW_DET) {
		if (_lexer.getNext() && fIdentifier(buf)) {
			if (_mode == Mode::EXECUTE)
				_interpreter.matrixDet(buf);
			_lexer.getNext();
			return true;
		}
	}
#if USE_DATA
	else if (_lexer.getToken() == Token::KW_READ) {
		if (_lexer.getNext() && fIdentifier(buf)) {
			if (_mode == Mode::EXECUTE)
				_interpreter.matrixRead(buf);
			_lexer.getNext();
			return true;
		}
	}
#endif
	return false;
}

bool
Parser::fMatrixPrint()
{
	char buf[IDSIZE];
	if (fIdentifier(buf)) {
		if (_mode == Mode::EXECUTE) {
			buf[VARSIZE-1] = '\0';
			_interpreter.printMatrix(buf);
		}
		return true;
	} else
		return false;
}

bool
Parser::fMatrixExpression(const char *buf)
{
	Interpreter::MatrixOperation_t mo;
	
	const Token t = _lexer.getToken();
	
	switch (t) {
	case Token::KW_ZER: // Zero matrix
		if (_mode == Mode::EXECUTE)
			_interpreter.zeroMatrix(buf);
		return true;
	case Token::KW_CON: // Ones matrix
		if (_mode == Mode::EXECUTE)
			_interpreter.onesMatrix(buf);
		return true;
	case Token::KW_IDN: // Identity matrix
		if (_mode == Mode::EXECUTE)
			_interpreter.identMatrix(buf);
		return true;
	case Token::LPAREN: { // Scalar
		Value v;
		char first[IDSIZE];
		if (_lexer.getNext() && fExpression(v) &&
		    _lexer.getToken() == Token::RPAREN &&
		    _lexer.getNext() && _lexer.getToken() == Token::STAR &&
		    _lexer.getNext() && fIdentifier(first)) {
			if (_mode == Mode::EXECUTE) {
				_interpreter.pushValue(v);
				_interpreter.assignMatrix(buf, first, nullptr,
				    Interpreter::MO_SCALE);
			}
		} else
			return false;
	}
		break;
	case Token::KW_TRN:
		mo = Interpreter::MO_TRANSPOSE;
	case Token::KW_INV:
		mo = Interpreter::MO_INVERT;
	{
		char first[IDSIZE];
		if (_lexer.getNext() && _lexer.getToken() == Token::LPAREN &&
		    _lexer.getNext() && fIdentifier(first) &&
		    _lexer.getNext() && _lexer.getToken() == Token::RPAREN) {
			if (_mode == Mode::EXECUTE)
				_interpreter.assignMatrix(buf, first, nullptr,
				    mo);
			return true;
		} else
			return false;
	}
		break;
	default:
		break;
	}
	
	char first[IDSIZE];
	if (fIdentifier(first)) { // Matrix expression
		if (_lexer.getNext()) {
			switch (_lexer.getToken()) {
			case Token::PLUS:
				mo = Interpreter::MO_SUM;
				break;
			case Token::MINUS:
				mo = Interpreter::MO_SUB;
				break;
			case Token::STAR:
				mo = Interpreter::MO_MUL;
				break;
			default:
				return false;
			}
			char second[VARSIZE];
			if (_lexer.getNext() && fIdentifier(second)) {
				if (_mode == Mode::EXECUTE)
					_interpreter.assignMatrix(buf, first,
					    second, mo);
				return true;
			} else
				return false;
		}
		if (_mode == Mode::EXECUTE)
			_interpreter.assignMatrix(buf, first);
		return true;
	}
	return false;
}

#endif // USE_MATRIX

} // namespace BASIC
