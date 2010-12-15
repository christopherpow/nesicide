# work in progress
# this doenst work at all yet

AC_PREREQ([2.65])
AC_INIT([nesicide-pasm], [2.0.0], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([pasm_types.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

#AM_PROG_LEX
#AC_PROG_YACC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strcasecmp strdup strstr strtoul])

AC_OUTPUT(makefile)
