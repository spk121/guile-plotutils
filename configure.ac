dnl Process this file with autoconf to produce a configure script

#    Copyright (C) 2018, 2019, 2020, 2021 Michael L. Gran

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

################
# Package Info

AC_INIT([Guile Plotutils], [1.0.1], [spk121@yahoo.com], [guile_plotutils])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/guile_plot.c])
AM_INIT_AUTOMAKE(foreign)

AC_PREREQ([2.63])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])


################
# Libtool versioning

LIBGUILE_PLOTUTILS_INTERFACE="1:0:0"
AC_SUBST([LIBGUILE_PLOTUTILS_INTERFACE])

################
# Check for programs

PKG_PROG_PKG_CONFIG
AC_PROG_CC
AC_CANONICAL_HOST
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AM_PROG_AR

dnl AC_USE_SYSTEM_EXTENSIONS
dnl AC_HEADER_STDC
dnl AC_PROG_SED

# from guile.m4
GUILE_PKG([3.0 2.2])
GUILE_PROGS
GUILE_FLAGS
GUILE_SITE_DIR

# Installed .scm scheme libraries go here
AC_MSG_CHECKING([Guile site directory])
guilesitedir="\${datadir}/guile/site/$GUILE_EFFECTIVE_VERSION"
AC_MSG_RESULT([$guilesitedir])
AC_SUBST(guilesitedir)

# Compiled libguile-*.so extension libraries go here
AC_MSG_CHECKING([Guile extension directory])
guileextensiondir="\${libdir}/guile/$GUILE_EFFECTIVE_VERSION/extensions"
AC_MSG_RESULT([$guileextensiondir])
AC_SUBST(guileextensiondir)

# Compiled .go scheme libraries go here
AC_MSG_CHECKING([Guile object directory])
guileobjectdir="\${libdir}/guile/$GUILE_EFFECTIVE_VERSION/site-ccache"
AC_MSG_RESULT([$guileobjectdir])
AC_SUBST(guileobjectdir)

################
# Check for Libtool

LT_PREREQ([2.2])
LT_INIT([disable-fast-install disable-static win32-dll])

##################
# Initial guess of platform specific options

AS_CASE([$host_os],
        [mingw*], [building_dll=yes;dll_version_info=no;makeinfo_css=no],
        [cygwin*], [building_dll=yes;dll_version_info=yes;makeinfo_css=yes],
        [pw32*], [building_dll=yes;dll_version_info=yes;makeinfo_css=no],
        [cegcc*], [building_dll=yes;dll_version_info=yes;makeinfo_css=no],
        [darwin*], [building_dll=no;dll_version_info=yes;makeinfo_css=no],
        [building_dll=no;dll_version_info=yes;makeinfo_css=yes])

AM_CONDITIONAL([BUILDING_DLL], [test "$building_dll" = yes])
AM_CONDITIONAL([DLL_VERSION_INFO], [test "$dll_version_info" = yes])

# FIXME: a hack to workaround an apparent lack of the --css-ref flag
# in some MacOS makeinfo executables
AM_CONDITIONAL([MAKEINFO_CSS], [test "$makeinfo_css" = yes])

################
# Check for libraries

AC_SEARCH_LIBS([pl_newpl_r],
               [plot], ,
               # if GNU Plotutils's libplot is missing
               [AC_MSG_ERROR([libplot not found])])


################
# Check for header files

AC_CHECK_HEADER([plot.h], [AC_DEFINE([HAVE_PLOT_H], 1,
      [Define if you have GNU Plotutils's <plot.h> header file.])])

################
# Check for types, structures, compiler characteristics
m4_define([compiler_flags_common],[ dnl
 -D_FORTIFY_SOURCE=2 dnl
 -DSCM_DEBUG_TYPING_STRICTNESS=2 dnl
 -O2 dnl
 -pipe dnl
 -g dnl
 -grecord-gcc-switches dnl
 -Wall dnl
 -Werror=format-security dnl
 -Werror=implicit-function-declaration dnl
 -Werror=shadow dnl
 -fno-omit-frame-pointer dnl
 -fasynchronous-unwind-tables dnl
 -fexceptions dnl
 -fvar-tracking dnl
 -fstack-protector-strong dnl
 -fstack-clash-protection dnl
 -fcf-protection dnl
 -fplugin=annobin dnl
 -Wl,--dynamic-list-data dnl
 -Wl,-z,defs dnl
 -Wl,-z,now dnl
 -Wl,-z,relro dnl
 -Wsuggest-attribute=format dnl
 -Wsuggest-attribute=malloc dnl
 -Wsuggest-attribute=noreturn dnl
 -Wsuggest-attribute=pure dnl
 -fdiagnostics-color=auto dnl
])

AC_ARG_ENABLE([hardening],
              [AS_HELP_STRING([--enable-hardening],
                              [compile with common hardening flags])])
AS_IF([test "x$enable_hardening" = xyes],
      [CC_CHECK_CFLAGS_APPEND([compiler_flags_common])])

AC_ARG_ENABLE([guild],
              [AS_HELP_STRING([--disable-guild],
                              [don't compile guile sources])],
              [AS_CASE(["${enableval}"],
                       [yes], [guild=true],
                       [no], [guild=false],
                       [AC_MSG_ERROR([bad value ${enableval} for --enable-guild])])],
              [guild=true])
AM_CONDITIONAL([GUILD], [test x$guild = xtrue])

################
# Check for C library functions

################
# Check for system services

################
# Output

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([docs/ex-sine-wave.scm], [chmod +x docs/ex-sine-wave.scm])
AC_CONFIG_FILES([docs/ex-sine-wave2.scm], [chmod +x docs/ex-sine-wave2.scm])
AC_CONFIG_FILES([docs/ex-plot-curve.scm], [chmod +x docs/ex-plot-curve.scm])
AC_CONFIG_FILES([tools/uninstalled-env], [chmod +x tools/uninstalled-env])
AC_CONFIG_FILES([tools/uninstalled-test-env], [chmod +x tools/uninstalled-test-env])
AC_CONFIG_FILES([tools/run-guile], [chmod +x tools/run-guile])
AC_CONFIG_FILES([tools/run-test], [chmod +x tools/run-test])
AC_CONFIG_FILES([tools/gdb-guile], [chmod +x tools/gdb-guile])
AC_CONFIG_FILES([tools/gdb-test], [chmod +x tools/gdb-test])

AC_REQUIRE_AUX_FILE([test-driver])
AC_REQUIRE_AUX_FILE([test-driver.scm])

AC_OUTPUT
