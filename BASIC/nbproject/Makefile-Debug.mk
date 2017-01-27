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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/basic_arduinoio.o \
	${OBJECTDIR}/src/basic_functionblock.o \
	${OBJECTDIR}/src/basic_interpreter.o \
	${OBJECTDIR}/src/basic_interpreter_program.o \
	${OBJECTDIR}/src/basic_lexer.o \
	${OBJECTDIR}/src/basic_math.o \
	${OBJECTDIR}/src/basic_parser.o \
	${OBJECTDIR}/src/basic_parser_value.o \
	${OBJECTDIR}/src/ucbasic_main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=`pkg-config --cflags posixcpp` 
CXXFLAGS=`pkg-config --cflags posixcpp` 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../arduinoext/dist/Debug/GNU-Linux/libarduinoext.a ../libarduinoemulator/dist/Debug/GNU-Linux/liblibarduinoemulator.a `pkg-config --libs posixcpp`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/basic

${CND_DISTDIR}/${CND_CONF}/basic: ../arduinoext/dist/Debug/GNU-Linux/libarduinoext.a

${CND_DISTDIR}/${CND_CONF}/basic: ../libarduinoemulator/dist/Debug/GNU-Linux/liblibarduinoemulator.a

${CND_DISTDIR}/${CND_CONF}/basic: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/basic ${OBJECTFILES} ${LDLIBSOPTIONS} -static

${OBJECTDIR}/src/basic_arduinoio.o: src/basic_arduinoio.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_arduinoio.o src/basic_arduinoio.cpp

${OBJECTDIR}/src/basic_functionblock.o: src/basic_functionblock.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_functionblock.o src/basic_functionblock.cpp

${OBJECTDIR}/src/basic_interpreter.o: src/basic_interpreter.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_interpreter.o src/basic_interpreter.cpp

${OBJECTDIR}/src/basic_interpreter_program.o: src/basic_interpreter_program.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_interpreter_program.o src/basic_interpreter_program.cpp

${OBJECTDIR}/src/basic_lexer.o: src/basic_lexer.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_lexer.o src/basic_lexer.cpp

${OBJECTDIR}/src/basic_math.o: src/basic_math.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_math.o src/basic_math.cpp

${OBJECTDIR}/src/basic_parser.o: src/basic_parser.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_parser.o src/basic_parser.cpp

${OBJECTDIR}/src/basic_parser_value.o: src/basic_parser_value.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/basic_parser_value.o src/basic_parser_value.cpp

${OBJECTDIR}/src/ucbasic_main.o: src/ucbasic_main.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../arduino -Isrc -I../include -I../arduinoext/include -I../libarduinoemulator/include -Iinclude -I/opt/arduino-1.6.12/hardware/arduino/avr/cores/arduino `pkg-config --cflags posixcpp` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ucbasic_main.o src/ucbasic_main.cpp

# Subprojects
.build-subprojects:
	cd ../arduinoext && ${MAKE}  -f Makefile CONF=Debug
	cd ../libarduinoemulator && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../arduinoext && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../libarduinoemulator && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
