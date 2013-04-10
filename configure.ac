dnl Process this file with autoconf to produce a configure script.
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; version 2 or later of the License.   
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
dnl 02110-1301, USA.


dnl Initializations:
AC_INIT(xsc.C)
AM_INIT_AUTOMAKE(xstarcastle, 1.6)
AM_CONFIG_HEADER(config.h)

dnl Set the language to C++
AC_LANG_CPLUSPLUS

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

dnl Checks for simple libraries.
AC_CHECK_LIB(m, sin)

dnl Checks for X11 include and library directories.
AC_PATH_X
AC_PATH_XTRA

CPPFLAGS="$CPPFLAGS $X_CFLAGS"
LDFLAGS="$LDFLAGS $X_LDFLAGS $X_LIBS"

if test "x$no_x" = "xyes"; then 
    AC_MSG_ERROR([
*** X libraries or include files not found. Check 'config.log' for 
*** more details.])
fi

PKG_CHECK_MODULES(SDL, [sdl >= 1.2.1])
AC_SUBST(SDL_LIBS)
AC_SUBST(SDL_CFLAGS)

eval XSTARCASTLE_DATADIR="$datadir/xstarcastle"
AC_SUBST(XSTARCASTLE_DATADIR)

AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio, , 
    AC_MSG_ERROR([*** libSDL_mixer not found. Check 'config.log' for more details.]),

    $X_EXTRA_LIBS)
AC_CHECK_LIB(X11, XOpenDisplay, , 
    AC_MSG_ERROR([*** libX11 not found. Check 'config.log' for more details.]),
    $X_EXTRA_LIBS)

AC_CHECK_LIB(GL, glXChooseVisual, , 
    AC_MSG_ERROR([*** libGL not found. Check 'config.log' for more details.]),
    $X_EXTRA_LIBS)

dnl Checks for header files.
AC_CHECK_HEADERS(time.h)
AC_CHECK_HEADERS(sys/time.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_TIME

dnl Checks for library functions.
AC_TYPE_SIGNAL

AC_CHECK_FUNCS(gettimeofday, true, AC_MSG_ERROR([
*** gettimeofday() not found.  Check 'config.log' for more details.
*** If it really doesn't exist you'll need to find another high resolution
*** clock function and modify timing.h use it.]))

AC_CHECK_FUNCS(usleep, true, AC_MSG_ERROR([
*** usleep() not found.  Check 'config.log' for more details.
*** If it really doesn't exist you'll need to find another high resolution
*** sleep function and modify timing.h to use it.]))

dnl fall back on rand, srand if these don't exist (or configure screws up)
AC_CHECK_FUNCS(random srandom, true, false)

dnl Output:
AC_OUTPUT(Makefile sounds/Makefile)
