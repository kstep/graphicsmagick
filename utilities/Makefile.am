#
#  Makefile for building GraphicsMagick utilities
#
# Copyright (C) 2002 GraphicsMagick Group, a non-profit organization dedicated
# to making software imaging solutions freely available.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files ("GraphicsMagick"),
# to deal in GraphicsMagick without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense
# and/or sell copies of GraphicsMagick, and to permit persons to whom the
# GraphicsMagick is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of GraphicsMagick.
#
# The software is provided "as is", without warranty of any kind, express or
# implied, including but not limited to the warranties of merchantability,
# fitness for a particular purpose and noninfringement.  In no event shall
# GraphicsMagick Group be liable for any claim, damages or other liability,
# whether in an action of contract, tort or otherwise, arising from, out of
# or in connection with GraphicsMagick or the use or other dealings in
# GraphicsMagick.
#
# Except as contained in this notice, the name of the GraphicsMagick Group
# shall not be used in advertising or otherwise to promote the sale, use or
# other dealings in GraphicsMagick without prior written authorization from the
# GraphicsMagick Group.
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

MAGICK_LIB	= ../magick/libGraphicsMagick.la # libtool library name
INCLUDES	=  -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/magick

STDMANPAGES = gm.1 miff.4 quantize.5
X11MANPAGES = 
ALLMANPAGES = $(STDMANPAGES) $(X11MANPAGES)


MANPAGES = $(ALLMANPAGES)

bin_PROGRAMS = gm

# Libtool libraries, dependencies, and linker flags used by most executables
if WITH_LTDL
COMMONLDADD	= $(MAGICK_LIB) $(top_builddir)/ltdl/libltdlc.la
COMMONLDFLAGS	= @LDFLAGS@ @DLLDFLAGS@
else
COMMONLDADD	= $(MAGICK_LIB)
COMMONLDFLAGS	= @LDFLAGS@
endif # WITH_LTDL

gm_LDADD           = $(COMMONLDADD)
gm_LDFLAGS         = $(COMMONLDFLAGS)
gm_SOURCES         = gm.c

# Manual pages to install
man_MANS = $(MANPAGES)

# Additional files to distribute
EXTRA_DIST = $(ALLMANPAGES)
