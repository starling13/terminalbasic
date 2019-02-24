#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/basic_arduinoio.o \
	${OBJECTDIR}/src/basic_common.o \
	${OBJECTDIR}/src/basic_dataparser.o \
	${OBJECTDIR}/src/basic_exteeprom.o \
	${OBJECTDIR}/src/basic_functionblock.o \
	${OBJECTDIR}/src/basic_gfx.o \
	${OBJECTDIR}/src/basic_gfx_serial.o \
	${OBJECTDIR}/src/basic_gfx_tvout.o \
	${OBJECTDIR}/src/basic_gfx_utft.o \
	${OBJECTDIR}/src/basic_internalfuncs.o \
	${OBJECTDIR}/src/basic_interpreter.o \
	${OBJECTDIR}/src/basic_interpreter_matrix.o \
	${OBJECTDIR}/src/basic_lexer.o \
	${OBJECTDIR}/src/basic_math.o \
	${OBJECTDIR}/src/basic_parser.o \
	${OBJECTDIR}/src/basic_parser_value.o \
	${OBJECTDIR}/src/basic_program.o \
	${OBJECTDIR}/src/basic_sdfs.o \
	${OBJECTDIR}/src/ucbasic_main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=`pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` 
CXXFLAGS=`pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../../tvoutex/livbtvoutex/dist/Release_PC/GNU-Linux/liblivbtvoutex.a ../../libarduinoemulator/dist/Release/GNU-Linux/liblibarduinoemulator.a ../../libarduinoext/dist/Release/GNU-Linux/liblibarduinoext.a `pkg-config --libs posixcpp` `pkg-config --libs sdl2`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/terminal-basic

${CND_DISTDIR}/${CND_CONF}/terminal-basic: ../../tvoutex/livbtvoutex/dist/Release_PC/GNU-Linux/liblivbtvoutex.a

${CND_DISTDIR}/${CND_CONF}/terminal-basic: ../../libarduinoemulator/dist/Release/GNU-Linux/liblibarduinoemulator.a

${CND_DISTDIR}/${CND_CONF}/terminal-basic: ../../libarduinoext/dist/Release/GNU-Linux/liblibarduinoext.a

${CND_DISTDIR}/${CND_CONF}/terminal-basic: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/terminal-basic ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/basic_arduinoio.o: src/basic_arduinoio.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_arduinoio.o src/basic_arduinoio.cpp

${OBJECTDIR}/src/basic_common.o: src/basic_common.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_common.o src/basic_common.cpp

${OBJECTDIR}/src/basic_dataparser.o: src/basic_dataparser.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_dataparser.o src/basic_dataparser.cpp

${OBJECTDIR}/src/basic_exteeprom.o: src/basic_exteeprom.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_exteeprom.o src/basic_exteeprom.cpp

${OBJECTDIR}/src/basic_functionblock.o: src/basic_functionblock.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_functionblock.o src/basic_functionblock.cpp

${OBJECTDIR}/src/basic_gfx.o: src/basic_gfx.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_gfx.o src/basic_gfx.cpp

${OBJECTDIR}/src/basic_gfx_serial.o: src/basic_gfx_serial.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_gfx_serial.o src/basic_gfx_serial.cpp

${OBJECTDIR}/src/basic_gfx_tvout.o: src/basic_gfx_tvout.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_gfx_tvout.o src/basic_gfx_tvout.cpp

${OBJECTDIR}/src/basic_gfx_utft.o: src/basic_gfx_utft.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_gfx_utft.o src/basic_gfx_utft.cpp

${OBJECTDIR}/src/basic_internalfuncs.o: src/basic_internalfuncs.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_internalfuncs.o src/basic_internalfuncs.cpp

${OBJECTDIR}/src/basic_interpreter.o: src/basic_interpreter.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_interpreter.o src/basic_interpreter.cpp

${OBJECTDIR}/src/basic_interpreter_matrix.o: src/basic_interpreter_matrix.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_interpreter_matrix.o src/basic_interpreter_matrix.cpp

${OBJECTDIR}/src/basic_lexer.o: src/basic_lexer.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_lexer.o src/basic_lexer.cpp

${OBJECTDIR}/src/basic_math.o: src/basic_math.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_math.o src/basic_math.cpp

${OBJECTDIR}/src/basic_parser.o: src/basic_parser.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_parser.o src/basic_parser.cpp

${OBJECTDIR}/src/basic_parser_value.o: src/basic_parser_value.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_parser_value.o src/basic_parser_value.cpp

${OBJECTDIR}/src/basic_program.o: src/basic_program.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_program.o src/basic_program.cpp

${OBJECTDIR}/src/basic_sdfs.o: src/basic_sdfs.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_sdfs.o src/basic_sdfs.cpp

${OBJECTDIR}/src/ucbasic_main.o: src/ucbasic_main.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -Wall -s -Iinclude -I../../libarduinoext/include -I../../libarduinoemulator/include -I${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino -I../../tvoutex/TVoutEx `pkg-config --cflags posixcpp` `pkg-config --cflags sdl2` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ucbasic_main.o src/ucbasic_main.cpp

# Subprojects
.build-subprojects:
	cd ../../tvoutex/livbtvoutex && ${MAKE}  -f Makefile CONF=Release_PC
	cd ../../libarduinoemulator && ${MAKE}  -f Makefile CONF=Release
	cd ../../libarduinoext && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../../tvoutex/livbtvoutex && ${MAKE}  -f Makefile CONF=Release_PC clean
	cd ../../libarduinoemulator && ${MAKE}  -f Makefile CONF=Release clean
	cd ../../libarduinoext && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
