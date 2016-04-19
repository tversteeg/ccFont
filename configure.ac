#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ccFont], m4_esyscmd_s([cat VERSION]))

CCFONT_VERSION="$(cat VERSION)"
AC_SUBST(CCFONT_VERSION)

# Set the programming language to 'C'
AC_LANG_C

AM_INIT_AUTOMAKE([foreign subdir-objects nostdinc -Wall -Werror])
AM_MAINTAINER_MODE([disable])
AM_PROG_AR

# Initialize libtool
LT_INIT
AC_ENABLE_STATIC
AC_ENABLE_SHARED

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([m], [cos], [], [echo "Math library is required" && exit -1])
AC_CHECK_LIB([GL], [glXChooseVisual], [], [echo "OpenGL dev library is required" && exit -1])
AC_CHECK_LIB([ccore], [ccFileInfoGet], [], [echo "ccore library is required" && exit -1])

# Checks for the test library
PKG_CHECK_MODULES([CHECK], [check >= 0.8.2], [have_check=yes], [have_check=no])

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime memset select strdup sysinfo])

SUMMARY="Configuration summary\n"
SUMMARY="${SUMMARY}ccFont version: ${CCFONT_VERSION}\n"

CheckX11()
{
	# Check for X11 libraries
	AC_CHECK_LIB([X11], [XOpenDisplay], [], [echo "X11 dev library is required" && exit -1])
	AC_CHECK_LIB([Xrandr], [XRRGetScreenResources], [], [echo "XRandr dev library is required" && exit -1])
	AC_CHECK_LIB([Xinerama], [XineramaQueryExtension], [], [echo "Xinerama dev library is required" && exit -1])
	AC_CHECK_LIB([Xi], [XISelectEvents], [], [echo "XInput dev library is required" && exit -1])
	AC_CHECK_LIB([Xext], [XShmCreateImage], [], [echo "Xshm (Xext) dev library is required" && exit -1])
	AC_PATH_X

	SUMMARY="${SUMMARY}Using X11 to handle window and input functions\n"
}

CheckWindows()
{
	AC_MSG_CHECKING(Windows compiler)
	have_win32_gcc=no
	AC_TRY_COMPILE(["#include <windows.h>"],[],[have_win32_gcc=yes])
	AC_MSG_RESULT($have_win32_gcc)
  if test x$have_win32_gcc != xyes; then
  	AC_MSG_ERROR(["Your compiler ($CC) does not produce Windows executables!"])
	fi
}

# AC_CANONICAL_HOST is needed to acces the 'host_os' variable
AC_CANONICAL_HOST
SUMMARY="${SUMMARY}Target host: ${host_os}\n"
case "${host_os}" in
	linux*)
		# Check Linux headers & libraries
		AC_CHECK_HEADERS([sys/ioctl.h sys/param.h sys/types.h unistd.h])
		AC_CHECK_LIB([pthread], [pthread_create], [], [echo "Pthread dev library is required" && exit -1])

		CheckX11
		;;
	mingw*)
		CheckWindows
		;;
	*)
		AC_MSG_ERROR(["OS $host_os is not supported"])
		;;
esac

# Add the include flag so the header files can read ccFont/*.h

# Pass the building booleans to Makefile.am
AM_CONDITIONAL([BUILD_TESTS], [test "$have_check" = "yes"])

if test x$have_check != xyes; then
	AC_DEFINE([HAVE_CHECK], [1], [Use check])
	SUMMARY="${SUMMARY}Not building tests (test/test.c) because the 'check' package could not be found\n"
	SUMMARY="${SUMMARY}Please install the check package to be able to run the tests\n"
fi

AC_SUBST([AM_CPPFLAGS], "-DCC_USE_ALL")

# Create a custom configuration header
#AC_CONFIG_HEADER([include/ccFont/config.h])
# Check if the source files are in the right location
AC_CONFIG_SRCDIR([src/ccFont/ccFont.c])
AC_CONFIG_FILES([Makefile])
# Setup m4 folder for libtool
AC_CONFIG_MACRO_DIR([m4])

# Display the output summary
AC_CONFIG_COMMANDS([summary], [echo -en "$SUMMARY"], [SUMMARY="$SUMMARY"])

AC_OUTPUT
