AC_INIT([ucbasic],[1.1-b1],[starling13@gmail.com],[ucbasic],[https://bitbucket.org/starling13/ucbasic])
AM_INIT_AUTOMAKE([foreign])
AC_PREREQ(2.57)
AC_CONFIG_HEADER(config.h)
AC_LANG(C++)

#############################################################################
#
# Default prefix directory for installation by OS
#
AC_PREFIX_DEFAULT(/usr/local)

#############################################################################
#
# Top and bottom of config.h
#

AH_TOP([#ifndef __config_h
#define __config_h
#include "options.h"])
AH_BOTTOM([

#if TKGATE_NOYYRESTART
#define yyrestart(f) { extern FILE *yyin; yyin = f; }
#endif

#if TKGATE_SIGSET
#define signal(a,b)	sigset(a,b)
#endif

#if !HAVE_PW_GECOS
#define pe_gecos pw_name
#endif

#endif /* __config_h */
])

#------------------------------------------------------------------------
# Standard compiler checks
#------------------------------------------------------------------------

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_CPP
AC_PROG_INSTALL

# Checks for header files.
AC_PATH_XTRA
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([limits.h malloc.h stdlib.h string.h sys/param.h sys/time.h unistd.h stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_INTPTR_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_TYPES(unsigned long long)

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getcwd gettimeofday memmove memset putenv select strcasecmp strchr strcspn strdup strncasecmp strrchr strspn strstr strcasestr])
AC_CHECK_LIB(m, floor)

#############################################################################
#
# Check for word size and Endianess
#
AC_C_BIGENDIAN

#############################################################################
#
# Substitution variables
#
AC_CONFIG_FILES([Makefile terminal-basic/Makefile])
AC_OUTPUT