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

#include <avr/io.h>

#include "seriallight.hpp"

#include "wiring_private.h"

SerialLight SerialL(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0);

SerialLight::SerialLight(
   volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
   volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
   volatile uint8_t *ucsrc, volatile uint8_t *udr) :
_ubrrh(ubrrh), _ubrrl(ubrrl),
_ucsra(ucsra), _ucsrb(ucsrb), _ucsrc(ucsrc),
_udr(udr), _hasByte(false)
{
}

void
SerialLight::begin(unsigned long baud, uint8_t config)
{
	// Try u2x mode first
	uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
	*_ucsra = 1 << U2X0;

	// hardcoded exception for 57600 for compatibility with the bootloader
	// shipped with the Duemilanove and previous boards and the firmware
	// on the 8U2 on the Uno and Mega 2560. Also, The baud_setting cannot
	// be > 4095, so switch back to non-u2x mode if the baud rate is too
	// low.
	if (((F_CPU == 16000000UL) && (baud == 57600)) || (baud_setting > 4095)) {
		*_ucsra = 0;
		baud_setting = (F_CPU / 8 / baud - 1) / 2;
	}

	// assign the baud_setting, a.k.a. ubrr (USART Baud Rate Register)
	*_ubrrh = baud_setting >> 8;
	*_ubrrl = baud_setting;

	//set the data bits, parity, and stop bits
#if defined(__AVR_ATmega8__)
	config |= 0x80; // select UCSRC register (shared with UBRRH)
#endif
	*_ucsrc = config;

	sbi(*_ucsrb, RXEN0);
	sbi(*_ucsrb, TXEN0);
	sbi(*_ucsrb, RXCIE0);
	cbi(*_ucsrb, UDRIE0);
}

int
SerialLight::available()
{
	if (*_ucsra & (1<<RXC0))
		return (1);
	else
		return (0);
}

void
SerialLight::flush()
{
}

int
SerialLight::peek()
{
	if (_hasByte)
		return (_byte);
	else if (*_ucsra & (1<<RXC0)) {
		_byte = *_udr;
		_hasByte = true;
	} else
		return (-1);
}

int
SerialLight::read()
{
	if (_hasByte) {
		_hasByte = false;
		return (_byte);
	} else if (*_ucsra & (1<<RXC0))
		return (*_udr);
	else
		return (-1);
}

size_t
SerialLight::write(uint8_t c)
{
	/* Wait for empty transmit buffer */
	while ( !(*_ucsra & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	*_udr = c;
}

