# require autoconf and autoconf-archive
# first run aclocal
# then run autoconf
# then test ./configure

# Generated using Autoscan, added C++11 check and AC_LANG manually
AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src/array.h])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs
AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Check for C++11 support
AX_CXX_COMPILE_STDCXX_11

# Checks for header files.
AC_CHECK_HEADERS([float.h stddef.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([floor pow sqrt])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
