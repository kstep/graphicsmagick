# Makefile for creating libTclMagick and libTkMagick

# Copyright David N. Welton <davidw@dedasys.com>, 2004, with many
# thanks to Bob Friesenhahn for all the help.

# AM_CFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@

# Put this in its own package directory.
libdir = @libdir@/TclMagick@VERSION@
lib_LTLIBRARIES = libTclMagick.la libTkMagick.la

libTclMagick_la_SOURCES = TclMagick.c TclMagick.h
libTkMagick_la_SOURCES = TkMagick.c TclMagick.h

libTclMagick_la_LDFLAGS = @GM_LDFLAGS@ -no-undefined -module
libTclMagick_la_LIBADD = @TCL_LIBS@ @GM_LIBS@
libTclMagick_la_CPPFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@

libTkMagick_la_LDFLAGS = @GM_LDFLAGS@ -no-undefined -module
libTkMagick_la_LIBADD = @TCL_LIBS@ @TK_LIBS@ @GM_LIBS@
libTkMagick_la_CPPFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@ @TK_INCLUDES@ @TK_XINCLUDES@
