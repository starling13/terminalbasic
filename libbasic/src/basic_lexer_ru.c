#include "avr/pgmspace.h"

const uint8_t _basic_lexer_symbolsShift PROGMEM = (uint8_t)('�')-'A';

static const uint8_t _basic_lexer_tokenTable[] PROGMEM = {
    '�', '�', '�', '�', '�'+0x80,
    '�', '�', '�', '�', '�'+0x80,
    '�', '�', '�', '�', '�'+0x80,
    '�', '�', '�', '�', '�', '�', '�'+0x80,
    '\0'
};
