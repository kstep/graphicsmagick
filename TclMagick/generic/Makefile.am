# Makefile for creating libTclMagick and libTkMagick

# Copyright David N. Welton <davidw@dedasys.com>, 2004, with many
# thanks to Bob Friesenhahn for all the help.

AM_CFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@

lib_LTLIBRARIES = libTclMagick.la libTkMagick.la
libTclMagick_la_LDFLAGS = -no-undefined -module
libTkMagick_la_LDFLAGS = -no-undefined -module

libTclMagick_la_LIBADD = @TCL_LIBS@ @GM_LIBS@
libTclMagick_la_SOURCES = TclMagick.c

libTkMagick_la_LIBADD = @TCL_LIBS@ @TK_LIBS@ @GM_LIBS@
libTkMagick_la_CFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@ @TK_INCLUDES@
libTkMagick_la_SOURCES = TkMagick.c

