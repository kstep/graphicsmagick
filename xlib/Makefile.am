#
#  Automake Makefile for the X11 stubs library
#
# Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated
# to making software imaging solutions freely available.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files ("ImageMagick"),
# to deal in ImageMagick without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense
# and/or sell copies of ImageMagick, and to permit persons to whom the
# ImageMagick is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of ImageMagick.
#
# The software is provided "as is", without warranty of any kind, express or
# implied, including but not limited to the warranties of merchantability,
# fitness for a particular purpose and noninfringement.  In no event shall
# ImageMagick Studio be liable for any claim, damages or other liability,
# whether in an action of contract, tort or otherwise, arising from, out of
# or in connection with ImageMagick or the use or other dealings in
# ImageMagick.
#
# Except as contained in this notice, the name of the ImageMagick Studio
# shall not be used in advertising or otherwise to promote the sale, use or
# other dealings in ImageMagick without prior written authorization from the
# ImageMagick Studio.
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.6 foreign

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

