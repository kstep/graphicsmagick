#
#  Makefile for building ImageMagick utilities
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
#  This file is currently maintained by Bob Friesenhahn,
#  bfriesen@simple.dallas.tx.us. Please report any bugs via the ImageMagick
#  bug tracking system at http://www.simplesystems.org/ImageMagick/bugs.

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

MAGICK_LIB	= ../magick/libMagick.la # libtool library name
INCLUDES	= -I../ -I$(top_srcdir) $(INCLTDL) $(X_CFLAGS)

STDMANPAGES = ImageMagick.1  combine.1 convert.1  identify.1 \
   miff.4 mogrify.1 montage.1 quantize.5
X11MANPAGES = animate.1 display.1 import.1
ALLMANPAGES = $(STDMANPAGES) $(X11MANPAGES)

# Executables to build
if HasX11
X11PROGRAMS = animate display import
MANPAGES = $(ALLMANPAGES)
else
X11PROGRAMS =
MANPAGES = $(STDMANPAGES)
endif
bin_PROGRAMS = combine convert identify mogrify montage $(X11PROGRAMS)

# Per-executable sources
display_SOURCES  = display.c
animate_SOURCES  = animate.c
import_SOURCES   = import.c
montage_SOURCES  = montage.c
convert_SOURCES  = convert.c
mogrify_SOURCES  = mogrify.c
identify_SOURCES = identify.c
combine_SOURCES  = combine.c

# Temporary for magic testing
#noinst_PROGRAMS  = magic
#magic_SOURCES    = magic.c

# Libtool libraries, dependencies, and linker flags used by all executables
if WITH_MODULES
LDADD		 = $(MAGICK_LIB) $(top_builddir)/ltdl/libltdlc.la
LDFLAGS          = @LDFLAGS@ @DLLDFLAGS@
else
LDADD		 = $(MAGICK_LIB)
LDFLAGS	         = @LDFLAGS@
endif # WITH_MODULES

# Manual pages to install
man_MANS = $(MANPAGES)

# Additional files to distribute
EXTRA_DIST = $(ALLMANPAGES)
