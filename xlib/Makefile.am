# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Automake Makefile for the X11 stubs library
#
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

lib_LTLIBRARIES = @X11STUBSLANAME@

# Any library that is auto-configured into lib_LTLIBRARIES must be listed
# in EXTRA_LTLIBRARIES so that Automake can generate rules to build it.
# Furthermore, Automake does not assume a -rpath argument for such libraries
# so it must be added explicitly.
EXTRA_LTLIBRARIES = libX11_stubs.la
libX11_stubs_la_SOURCES = X11_stubs.c
libX11_stubs_la_LDFLAGS = -rpath $(libdir)

DISTDIRS = X11 unix
dist-hook:
	( \
	  builddir=`pwd` ; \
	  cd $(srcdir) && \
	  ( \
	    for dir in $(DISTDIRS) ; do \
	      find $$dir -depth -print | egrep -v '(~$$)' | cpio -pdum $$builddir/$(distdir) 2> /dev/null ; \
	    done \
	  ) \
	)

# Since we are building a library, no need for LIBS
LIBS =

