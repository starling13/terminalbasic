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

/**
 * @file config.hpp
 * @brief Configuration parameters, common among versions
 */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <stdint.h>

namespace BASIC
{

/*
 * Allow '_' symbol in identifiers
 */
#define ALLOW_UNDERSCORE_ID 0

/*
 * DELAY command, suspends execution for N ms
 */
#define USE_DELAY    1

/*
 * Real arithmetics
 * 
 * Support of operations with real numbers.
 * When enabled, all variables and arrays, which names are not ending with "$ ! %"
 * are treated as reals. Mathematical functions support depend on this option
 */
#define USE_REALS               1
#if USE_REALS
	/*
	 * Mathematical functions support
	 */
	#define USEMATH  1
	#if USEMATH
		/*
		 * SIN COS TAN COT
		 */
		#define M_TRIGONOMETRIC         1
		/*
		 * ACS ASN ATN
		 */
		#define M_REVERSE_TRIGONOMETRIC 0
		/*
		 * CBR (cubic root) ...
		 */
		#define M_ADDITIONAL            0
	#endif // USEMATH
#endif // USE_REALS

// Use string functions and operations
#define USE_STRINGOPS      1
#if USE_STRINGOPS
	// GET$ function, returns string from last pressed key symbol
	#define USE_GET            1
	// CHR$ function, returns string from the parameter ASCII code
	#define USE_CHR            1
	// ASC function, returns code of the first symbol in a string
	#define USE_ASC            1
	// LEN function, returns length of the string
	#define USE_LEN            1
#endif // USE_STRINGOPS
/**
 * Allow GO TO OPERATOR in addition to GOTO
 */
#define CONF_SEPARATE_GO_TO     1
/*
 * Use >< as not-equals operator (with default <>)
 */
#define CONF_USE_ALTERNATIVE_NE 1
/*
 * Support of 4-byte integer datatype
 * Functions, variables and arrays of long integer type ends with double % mark
 */
#define USE_LONGINT          0
/*
 * Support of integer division and modulo operation
 */
#define USE_INTEGER_DIV      1
#if USE_INTEGER_DIV
	/*
	 * Use DIV keyword for integer division in addition to \ operation
	 */
	#define USE_DIV_KW           0
#endif // USE_INTEGER_DIV
/**
 * DUMP command support
 * This command can be used to see BASIC memory image, variables and arrays list
 */
#define USE_DUMP             0
/*
 * Clear program memory on NEW command
 */
#define CLEAR_PROGRAM_MEMORY 1
/*
 * RANDOMIZE command and RND() function support
 */
#define USE_RANDOM           1
/*
 * Support of Darthmouth BASIX-style matrix operations
 */
#define USE_MATRIX           0
/**
 * Support of DATA/READ statements
 */
#define USE_DATA             1
/*
 * Support of DEF FN construct
 */
#define USE_DEFFN            0
/*
 * Use vt100 text attributes
 */
#define USE_TEXTATTRIBUTES   1
#if USE_TEXTATTRIBUTES
	/*
	 * Use ANSI color attributes
	 */
	#define USE_COLORATTRIBUTES  1
	/*
	 * Support of SPC(N) print command
	 */
	#define CONF_USE_SPC_PRINT_COM  1
	/*
	 * Set print zones width (tab spacing)
	 */
	#define SET_PRINTZNES  1
	#if SET_PRINTZNES
		#define PRINT_ZONE_WIDTH 16
		#define PRINT_ZONES_NUMBER 5
	#endif // SET_PRINTZNES
#endif // USE_TEXTATTRIBUTES
/*
 * SAVE, LOAD and CHAIN commands support
 */
#define USE_SAVE_LOAD        1
#if USE_SAVE_LOAD
	// Compute checksums while SAVE, LOAD and CHAIN
	#define SAVE_LOAD_CHECKSUM   1
#endif // USE_SAVE_LOAD
/*
 * STOP and CONTINUE commands support
 */
#define USESTOPCONT       1
// Convert all input to upper register
#define AUTOCAPITALIZE    0

/*
 * C++ level code optimisation mode
 */
#define OPT_SPEED     1 // Extensive use of switch/case constructs
#define OPT_SIZE      2 // Use cascade of if/else if instead of switch/case
#define OPT           OPT_SIZE // Selected mode

/*
 * SDcard module
 */
#define USESD         0

#define CONF_ERROR_STRINGS 1
#if CONF_ERROR_STRINGS
	#define ERROR_STRING_SIZE 48
#endif

// Arduino IO module
#define CONF_MODULE_ARDUINOIO      1
#if CONF_MODULE_ARDUINOIO
	// TONE command support
	#define CONF_MODULE_ARDUINOIO_TONE 0
#endif // CONF_MODULE_ARDUINOIO

// External EEPROM functions module
#define USE_EXTEEPROM    0
#if USE_EXTEEPROM
	/*
	 * Size in bytes
	 */
	#define EXTEEPROM_SIZE   32768
#endif // USE_EXTEEPROM

/*
 * Structured loop support
 */
#define USE_DOLOOP       0
/*
 * Indention of the loop bodies
 */
#define LOOP_INDENT      1
/*
 * Indention of the line numbers in LIST output
 */
#define LINE_NUM_INDENT  1
/*
 * GFX module
 */
#define USE_GFX          0
/*
 * Prompt message
 */
#define CLI_PROMPT       "READY"
#define CLI_PROMPT_NELINE 1
/*
 * LF character processing
 */
#define LF_NONE    0 // Not handled
#define LF_EAT     1 // Silently eat it
#define LF_NEWLINE 2 // Use as new line
#define PROCESS_LF LF_NEWLINE

/*
 * Input and output for single terminal mode
 */

// Input variants
#define SERIAL_I    1  // Serial input
#define SERIAL1_I   2  // Serial1 input
#define SERIALL_I   3  // SerialL input (non-buffering, interrupt-free)
#define SERIALL1_I  4  // SerialL1 input (non-buffering, interrupt-free)
#define SERIALL2_I  5  // SerialL2 input (non-buffering, interrupt-free)
#define SERIALL3_I  6  // SerialL3 input (non-buffering, interrupt-free)
	#define SERIAL_I_BR 115200
#define PS2UARTKB_I 7  // PS/2 keyboard through USART
#define SDL_I       8  // SDL input on PC

// Output variants
#define SERIAL_O   1 // Serial output
#define SERIAL1_O  2 // Serial1 output
#define SERIALL_O  3 // SerialL output (non-buffering, interrupt-free)
#define SERIALL1_O 4 // SerialL1 output (non-buffering, interrupt-free)
#define SERIALL2_O 5 // SerialL2 output (non-buffering, interrupt-free)
#define SERIALL3_O 6 // SerialL3 output (non-buffering, interrupt-free)
	#define SERIAL_O_BR 115200
#define UTFT_O     7 // UTFT library output
#define TVOUT_O    8 // TVoutEx library output
	#define TVOUT_HORIZ 240
	#define TVOUT_VERT 192
#define LIQCR_O    9 // LiquidCrystal library output
	#define LIQCR_HORIZ 20
	#define LIQCR_VERT 4
	#define LIQCR_RS 12
	#define LIQCR_E 11
	#define LIQCR_D0 5
	#define LIQCR_D1 4
	#define LIQCR_D2 3
	#define LIQCR_D3 2

// Input select
#define S_INPUT SERIAL_I

// Output select
#define S_OUTPUT SERIAL_O

#if USE_EXTEEPROM
	#define USE_WIRE 1
#else
	#define USE_WIRE 0
#endif

/*
 * Max size of the program line
 */
const uint8_t PROGSTRINGSIZE = 75;

// Max size of the string constants/variables
const uint8_t STRINGSIZE = 75;

// Number of characters in variable name
const uint8_t VARSIZE = 5;

} // namespace BASIC

#endif // CONFIG_HPP
