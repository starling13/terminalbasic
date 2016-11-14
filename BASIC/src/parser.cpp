#include <math.h>
#include <string.h>

#include "parser.hpp"
#include "basic_interpreter.hpp"
#include "basic_interpreter_program.hpp"

/*
 * TEXT = OPERATORS | C_INTEGER OPERATORS
 * OPERATORS = OPERATOR | OPERATOR COLON OPERATORS
 * OPERATOR = KW_END |
 *	KW_RUN |
 *	KW_LIST |
 *	KW_NEW |
 *	KW_DUMP |
 *	KW_RETURN |
 *	KW_PRINT PRINT_LIST |
 *	KW_IF EXPRESSION IF_STATEMENT |
 *	KW_FOR IMPLICIT_ASSIGNMENT KW_TO EXPRESSION |
 *	KW_NEXT IDENT |
 *	ASSIGNMENT |TURN |
 *	GOTO_STATEMENT |
 *	KW_GOSUB INTEGER
 * ASSIGNMENT = KW_LET IMPLICIT_ASSIGNMENT | IMPLICIT_ASSIGNMENT
 * IMPLICIT_ASSIGNMENT = IDENTIFIER EQUALS EXPRESSION
 * EXPRESSION = SIMPLE_EXPRESSION | SIMPLE_EXPRESSION REL SIMPLE_EXPRESSION
 * REL = LT | LTE | EQUALS | GT | GTE | NE
 * SIMPLE_EXPRESSION = TERM | TERM ADD TERM
 * ADD = PLUS MINUS KW_OR
 * TERM = FACTOR | FACTOR MUL FACTOR
 * MUL = STAR | SLASH | DIV | MOD | KW_AND
 * FACTOR = FINAL | FINAL POW FINAL
 * FINAL = IDENT | INTEGER | REAL | LPAREN EXPRESSION RPAREN | MINUS FINAL
 * PRINT_LIST = EXPRESSION | EXPRESSION COMMA PRINT_LIST
 * IF_STATEMENT = GOTO_STATEMEMNT | KW_THEN OPERATORS
 * GOTO_STATEMENT = KW_GOTO C_INTEGER
 */

namespace BASIC
{

Parser::Parser(Lexer &l, Interpreter &i) :
_lexer(l), _interpreter(i), _mode(EXECUTE)
{
}

bool
Parser::parse(const char *s)
{
	LOG_TRACE;

	_lexer.init(s);
	_error = NO_ERROR;
	
	return fOperators();
}

bool
Parser::fOperators()
{
	Token t;
	do {
		if (!_lexer.getNext() || !fOperator()) {
			if (_error == NO_ERROR)
				_error = OPERATOR_EXPECTED;
			return false;
		}
		t = _lexer.getToken();
	} while (t == COLON);
	return true;
}

bool
Parser::fOperator()
{
	LOG_TRACE;

	Token t = _lexer.getToken();
	LOG(t);
	switch (t) {
	case KW_DUMP:
		if (_mode == EXECUTE)
			_interpreter.dump();
		return true;
	case KW_END:
		if (_mode == EXECUTE)
			_interpreter.end();
		return true;
	case KW_FOR:
	{
		Value v;
		char vName[4];
		if (!_lexer.getNext() || !fImplicitAssignment(vName)) {
			return false;
		}
		if (_lexer.getToken()!=KW_TO) {
			return false;
		}
		if (!_lexer.getNext() || !fExpression(v)) {
			return false;
		}
		if (_mode == EXECUTE)
			_interpreter.pushForLoop(vName, v);
		return true;
	}
	case KW_GOSUB:
	{
		Value v;
		if (!_lexer.getNext() || !fExpression(v) || (v.type !=
		    Value::INTEGER)) {
			_error = INTEGER_EXPRESSION_EXPECTED;
			return false;
		}
		if (_mode == EXECUTE) {
			_interpreter.pushReturnAddress();
			_interpreter.gotoLine(v.value.integer);
		}
		return true;
	}
	case KW_IF:
	{
		Value v;
		if (!_lexer.getNext() || !fExpression(v)) {
			_error = EXPRESSION_EXPECTED;
			return false;
		}
		bool res;
		if (!bool(v))
			_mode = SCAN;
		if (fIfStatement())
			res = true;
		else {
			_error = THEN_OR_GOTO_EXPECTED;
			res = false;
		}
		_mode = EXECUTE;
		return res;
	}
	case KW_LET:
	{
		char vName[VARSIZE];
		if (!_lexer.getNext() || !fImplicitAssignment(vName)) {
			return false;
		} else
			return true;
	}
	case KW_LIST:
		if (_mode == EXECUTE)
			_interpreter.list();
		return true;
	case KW_NEW:
		if (_mode == EXECUTE)
			_interpreter.newProgram();
		return true;
	case KW_NEXT:
		char vName[VARSIZE];
		if (!_lexer.getNext() || _lexer.getToken() != IDENT)
			return false;
		if (_mode == EXECUTE)
			_interpreter.next(_lexer.id());
		return true;
	case KW_PRINT:
		if (_lexer.getNext())
			if (!fPrintList())
				return false;
		if (_mode == EXECUTE)
			_interpreter.print('\n');
		return true;
	case KW_RETURN:
		if (_mode == EXECUTE)
			_interpreter.returnFromSub();
		return true;
	case KW_RUN:
		if (_mode == EXECUTE)
			_interpreter.run();
		return true;
	default:
		if (fGotoStatement())
			return true;
		_error = OPERATOR_EXPECTED;
		return false;
	}
}

bool
Parser::fImplicitAssignment(char *varName)
{
	LOG_TRACE;

	if (_lexer.getToken() != IDENT)
		return false;

	strcpy(varName, _lexer.id());
	Value v;
	if (_lexer.getNext() && (_lexer.getToken() == EQUALS) &&
	    _lexer.getNext() && fExpression(v)) {
		if (_mode == EXECUTE)
			_interpreter.setVariable(varName, v);
		return true;
	} else {
		_error = EXPRESSION_EXPECTED;
		return false;
	}
}

bool
Parser::fPrintList()
{
	LOG_TRACE;

	Value v;
	if (!fExpression(v)) {
		_error = EXPRESSION_EXPECTED;
		return false;
	}

	if (_mode == EXECUTE)
		_interpreter.print(v);
	while (true) {
		Token t = _lexer.getToken();
		switch (t) {
		case COMMA:
			if (!_lexer.getNext() || !fExpression(v)) {
				_error = EXPRESSION_EXPECTED;
				return false;
			}
			if (_mode == EXECUTE) {
				_interpreter.print('\t');
				_interpreter.print(v);
			}
			break;
		default:
			return true;
		}
	}
}

bool
Parser::fExpression(Value &v)
{
	LOG_TRACE;

	if (!fSimpleExpression(v))
		return false;

	while (true) {
		Token t = _lexer.getToken();
		LOG(t);
		Value v2;
		switch (t) {
		case LT:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				v.value.boolean = v < v2;
				v.type = Value::BOOLEAN;
				continue;
			} else
				return false;
		case LTE:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				v.value.boolean = (v < v2) || (v == v2);
				v.type = Value::BOOLEAN;
				continue;
			} else
				return false;
		case GT:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				v.value.boolean = v > v2;
				v.type = Value::BOOLEAN;
				continue;
			} else
				return false;
		case GTE:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				v.value.boolean = (v > v2) || (v == v2);
				v.type = Value::BOOLEAN;
				continue;
			} else
				return false;
		case EQUALS:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				v.value.boolean = v == v2;
				v.type = Value::BOOLEAN;
				continue;
			} else
				return false;
		case NE:
			if (_lexer.getNext() && fSimpleExpression(v2)) {
				v.value.boolean = !(v == v2);
				v.type = Value::BOOLEAN;
				continue;
			} else
				return false;
		default:
			return true;
		}
		v.type = Value::BOOLEAN;
	}
}

bool
Parser::fSimpleExpression(Value &v)
{
	LOG_TRACE;

	if (!fTerm(v))
		return false;

	while (true) {
		Token t = _lexer.getToken();
		LOG(t);
		Value v2;
		switch (t) {
		case PLUS:
			if (_lexer.getNext() && fTerm(v2)) {
				v += v2;
				continue;
			} else
				return false;
		case MINUS:
			if (_lexer.getNext() && fTerm(v2)) {
				v -= v2;
				continue;
			} else
				return false;
		default:
			return true;
		}
	}
}

bool
Parser::fTerm(Value &v)
{
	LOG_TRACE;

	if (!fFactor(v))
		return false;

	while (true) {
		Token t = _lexer.getToken();
		LOG(t);
		Value v2;
		switch (t) {
		case STAR:
			if (_lexer.getNext() && fTerm(v2)) {
				v *= v2;
				continue;
			} else
				return false;
		case SLASH:
			if (_lexer.getNext() && fTerm(v2)) {
				v /= v2;
				continue;
			} else
				return false;
		default:
			return true;
		}
	}
}

bool
Parser::fFactor(Value &v)
{
	LOG_TRACE;

	if (!fFinal(v))
		return false;

	while (true) {
		Token t = _lexer.getToken();
		LOG(t);
		Value v2;
		switch (t) {
		case POW:
			if (_lexer.getNext() && fFactor(v2)) {
				v ^= v2;
				continue;
			} else
				return false;
		default:
			return true;
		}
	}
}

bool
Parser::fFinal(Value &v)
{
	LOG_TRACE;

	Token t = _lexer.getToken();
	LOG(t);
	while (true) {
		switch (t) {
		case MINUS:
			if (!_lexer.getNext() || !fFinal(v))
				return false;
			if (_mode == EXECUTE)
				v.switchSign();
			_lexer.getNext();
			return true;
		case C_INTEGER:
		case C_REAL:
			if (_mode == EXECUTE)
				v = _lexer.getValue();
			_lexer.getNext();
			return true;
		case IDENT:
			if (_mode == EXECUTE) {
				Interpreter::valueFromFrame(v,
				    _interpreter.getVariable(_lexer.id()));
			}
			_lexer.getNext();
			return true;
		case LPAREN:
			if (!_lexer.getNext() || !fExpression(v))
				return false;
			if (_lexer.getToken() != RPAREN)
				return false;
			else {
				_lexer.getNext();
				return true;
			}
		default:
			return false;
		}
	}
}

bool
Parser::fIfStatement()
{
	Token t = _lexer.getToken();
	LOG(t);
	switch (t) {
	case KW_THEN:
		if (fOperators())
			return true;
		break;
	default:
		if (_lexer.getNext() && fGotoStatement())
			return true;
		break;
	}
	return false;
}

bool
Parser::fGotoStatement()
{
	Token t = _lexer.getToken();
	LOG(t);
	if (t == KW_GOTO) {
		Value v;
		if (!_lexer.getNext() || !fExpression(v) || (v.type != Value::INTEGER)) {
			_error = INTEGER_EXPRESSION_EXPECTED;
			return false;
		}
		if (_mode == EXECUTE)
			_interpreter.gotoLine(v.value.integer);
		return true;
	} else
		return false;
}

}
