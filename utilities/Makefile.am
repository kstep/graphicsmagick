# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Makefile for building GraphicsMagick utilities
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
