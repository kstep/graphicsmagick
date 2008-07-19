#!/bin/bash -norc
dnl	This file is an input file used by the GNU "autoconf" program to
dnl	generate the file "configure", which is run during Tcl installation
dnl	to configure the system for the local environment.
#
AC_INIT(TclMagick, 0.45)

# Specify directory where m4 macros may be found.
AC_CONFIG_MACRO_DIR([unix/m4])

# Directory where autotools helper scripts lives.
AC_CONFIG_AUX_DIR([unix/config])

AM_INIT_AUTOMAKE(TclMagick, 0.45)

# Add configure option --enable-maintainer-mode which enables dependency
# checking and generation useful to package maintainers.  This is made an
# option to avoid confusing end users.
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_LIBTOOL
AC_ISC_POSIX
AC_HEADER_STDC([])


dnl This is required, because we need this information!
AC_ARG_WITH(magick, [  --with-magick              location of Graphics/ImageMagickWand-config script], with_magick=${withval})
AC_MSG_CHECKING([for Graphics/ImageMagick configuration script])

dnl First check to see if --with-magick-config was specified.
GM_CPPFLAGS=''
GM_LDFLAGS=''
GM_LIBS=''
if test x"${with_magick}" != x ; then
    if test -x "${with_magick}" ; then
	dnl It actually calls another script, so the PATH needs to be set.
	export PATH=$PATH:`dirname ${with_magick}`
	AC_MSG_RESULT("Using ${with_magick}")
	GM_CPPFLAGS=`${with_magick} --cppflags`
	AC_SUBST(GM_CPPFLAGS)
	GM_LDFLAGS=`${with_magick} --ldflags`
	AC_SUBST(GM_LDFLAGS)
	GM_LIBS=`${with_magick} --libs`
	AC_SUBST(GM_LIBS)
    else
	AC_MSG_ERROR([${with_magick} is not an executable file])
    fi
else
    AC_MSG_ERROR([You must specify the Graphics/ImageMagick-config script with --with-magick!])
fi

CPPFLAGS_SAVE=$CPPFLAGS
LDFLAGS_SAVE=$LDFLAGS
LIBS_SAVE=$LIBS
CPPFLAGS="$CPPFLAGS $GM_CPPFLAGS"
LDFLAGS="$LDFLAGS $GM_LDFLAGS"
LIBS="$GM_LIBS $LIBS"
AC_CHECK_FUNCS([MagickSetImageFormat])
CPPFLAGS=$CPPFLAGS_SAVE
LDFLAGS=$LDFLAGS_SAVE
LIBS=$LIBS_SAVE

TEA_INIT([3.6])

TEA_PATH_TCLCONFIG
TEA_LOAD_TCLCONFIG

TEA_PATH_TKCONFIG
TEA_LOAD_TKCONFIG

TEA_PREFIX

TEA_SETUP_COMPILER

TEA_ADD_HEADERS([])
TEA_ADD_INCLUDES([])
TEA_ADD_LIBS([])
TEA_ADD_CFLAGS([])
TEA_ADD_STUB_SOURCES([])
TEA_ADD_TCL_SOURCES([])

TEA_PUBLIC_TCL_HEADERS
TEA_PUBLIC_TK_HEADERS

TEA_ENABLE_THREADS

TEA_ENABLE_SHARED

TEA_CONFIG_CFLAGS

AC_DEFINE(USE_TCL_STUBS)
AC_DEFINE(USE_TK_STUBS)

TEA_MAKE_LIB

TEA_PROG_TCLSH
TEA_PROG_WISH


AC_OUTPUT([
Makefile
generic/Makefile
])
