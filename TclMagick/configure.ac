#!/bin/bash -norc
dnl	This file is an input file used by the GNU "autoconf" program to
dnl	generate the file "configure", which is run during Tcl installation
dnl	to configure the system for the local environment.
#
# RCS: @(#) $Id$

AC_INIT([TclMagick], [0.3])
AM_INIT_AUTOMAKE(TclMagick, 0.3)

AC_PROG_CC
AC_PROG_LIBTOOL
AC_ISC_POSIX
AC_HEADER_STDC([])

TEA_INIT([3.0])

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

AC_MSG_CHECKING(pre-processor flags)
# If -DHAVE_CONFIG_H is not specified, then QuantumLeap doesn't work.
GM_CPPFLAGS=`GraphicsMagickWand-config --cppflags`
AC_SUBST(GM_CPPFLAGS)
AC_MSG_RESULT("$GM_CPPFLAGS")

AC_MSG_CHECKING(linker flags)
GM_LDFLAGS=`GraphicsMagickWand-config --ldflags`
AC_SUBST(GM_LDFLAGS)
AC_MSG_RESULT("$GM_LDFLAGS")

AC_MSG_CHECKING(libs)
GM_LIBS=`GraphicsMagickWand-config --libs`
AC_SUBST(GM_LIBS)
AC_MSG_RESULT("$GM_LIBS")

#AC_CONFIG_FILES([Makefile generic/Makefile])
AC_OUTPUT([
Makefile
generic/Makefile
])
