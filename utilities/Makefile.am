#
#  Makefile for building ImageMagick utilities
#
# Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated
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
AUTOMAKE_OPTIONS = 1.4 foreign

MAGICK_LIB	= ../magick/libMagick.la # libtool library name
INCLUDES	= -I../ -I$(top_srcdir) $(INCLTDL) $(X_CFLAGS)

STDMANPAGES = ImageMagick.1  composite.1 conjure.1 convert.1  identify.1 \
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

if HasXML
XMLPROGRAMS = conjure
else
XMLPROGRAMS = 
endif

bin_PROGRAMS = composite $(XMLPROGRAMS) convert identify mogrify montage $(X11PROGRAMS)

# Libtool libraries, dependencies, and linker flags used by most executables
if WITH_MODULES
COMMONLDADD	= $(MAGICK_LIB) $(top_builddir)/ltdl/libltdlc.la
COMMONLDFLAGS	= @LDFLAGS@ @DLLDFLAGS@
else
COMMONLDADD	= $(MAGICK_LIB)
COMMONLDFLAGS	= @LDFLAGS@
endif # WITH_MODULES

animate_LDADD     = $(COMMONLDADD)
animate_LDFLAGS   = $(COMMONLDFLAGS)
animate_SOURCES   = animate.c

composite_LDADD     = $(COMMONLDADD)
composite_LDFLAGS   = $(COMMONLDFLAGS)
composite_SOURCES   = composite.c

conjure_LDADD     = $(COMMONLDADD) @LIB_XML@
conjure_LDFLAGS   = $(COMMONLDFLAGS)
conjure_SOURCES   = conjure.c

convert_LDADD     = $(COMMONLDADD)
convert_LDFLAGS   = $(COMMONLDFLAGS)
convert_SOURCES   = convert.c

display_LDADD     = $(COMMONLDADD)
display_LDFLAGS   = $(COMMONLDFLAGS)
display_SOURCES   = display.c

identify_LDADD    = $(COMMONLDADD)
identify_LDFLAGS  = $(COMMONLDFLAGS)
identify_SOURCES  = identify.c

import_LDADD      = $(COMMONLDADD)
import_LDFLAGS    = $(COMMONLDFLAGS)
import_SOURCES    = import.c

mogrify_LDADD     = $(COMMONLDADD)
mogrify_LDFLAGS   = $(COMMONLDFLAGS)
mogrify_SOURCES   = mogrify.c

montage_LDADD     = $(COMMONLDADD)
montage_LDFLAGS   = $(COMMONLDFLAGS)
montage_SOURCES   = montage.c

# Manual pages to install
man_MANS = $(MANPAGES)

# Additional files to distribute
EXTRA_DIST = $(ALLMANPAGES)
