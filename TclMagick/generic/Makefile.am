# Makefile for creating libTclMagick and libTkMagick

# David N. Welton <davidw@dedasys.com>, 2004, with many thanks to Bob
# Friesenhahn for all the help.

# $(TCL_CFLAGS_OPTIMIZE) $(TCL_CFLAGS_WARNING) $(TCL_EXTRA_CFLAGS) 

AM_CFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@

lib_LTLIBRARIES = libTclMagick.la libTkMagick.la

libTclMagick_la_LIBADD = @TCL_LIBS@
libTclMagick_la_SOURCES = TclMagick.c

libTkMagick_la_LIBADD = @TCL_LIBS@ @TK_LIBS@
libTkMagick_la_CFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@ @TK_INCLUDES@
libTkMagick_la_SOURCES = TkMagick.c

