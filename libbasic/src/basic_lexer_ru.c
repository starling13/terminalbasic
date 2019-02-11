#include "avr/pgmspace.h"
#include "basic_config.h"
#include "basic.h"

#if CONF_LANG == LANG_RU

const uint8_t _basic_lexer_symbolsShift PROGMEM = (uint8_t)('�')-'A';

const uint8_t _basic_lexer_tokenTable[] PROGMEM = {
	ASCII_NUL,
	'�', '�', '�', '�', ASCII_ETX,
	'�', '�', '�', '�', '�', '�', '�', ASCII_ETX,
	'�', '�', '�', '�', '�', ASCII_ETX,
	'�', '�', '�', '�', ASCII_ETX,
	'�', '�', '�', '�', '�', ASCII_ETX,
	'�', '�', '�', '�', ASCII_ETX,
	'�', '�', '�', '�', '�', ASCII_ETX,
#if USESTOPCONT
	'�', '�', '�', '�', ASCII_NUL,
#endif
	ASCII_NUL
};

#endif 
