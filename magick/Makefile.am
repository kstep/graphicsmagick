#  Makefile for building ImageMagick library
#
#  Copyright 1998 E. I. du Pont de Nemours and Company
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files ("ImageMagick"),
#  to deal in ImageMagick without restriction, including without limitation 
#  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
#  and/or sell copies of ImageMagick, and to permit persons to whom the 
#  ImageMagick is furnished to do so, subject to the following conditions:
#   
#  The above copyright notice and this permission notice shall be included in 
#  all copies or substantial portions of ImageMagick. 
#   
#  The software is provided "as is", without warranty of any kind, express or 
#  implied, including but not limited to the warranties of merchantability, 
#  fitness for a particular purpose and noninfringement.In no event shall 
#  E. I. du Pont de Nemours and Company be liable for any claim, damages or 
#  other liability, whether in an action of contract, tort or otherwise,
#  arising from, out of or in connection with ImageMagick or the use or other 
#  dealings in ImageMagick. 
#   
#  Except as contained in this notice, the name of the E. I. du Pont de 
#  Nemours and Company shall not be used in advertising or otherwise to 
#  promote the sale, use or other dealings in ImageMagick without prior 
#  written authorization from the E. I. du Pont de Nemours and Company. 
#
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.2 foreign

lib_LTLIBRARIES = libMagick.la

MAJOR_VERSION=@MAJOR_VERSION@
MINOR_VERSION=@MINOR_VERSION@
MICRO_VERSION=@MICRO_VERSION@

libMagick_la_LDFLAGS = -version-info $(MAJOR_VERSION):$(MINOR_VERSION)$(MICRO_VERSION):0

# Library sources
libMagick_la_SOURCES = PreRvIcccm.c X.c animate.c colors.c compress.c decode.c \
  delegates.c display.c effects.c encode.c error.c gems.c image.c monitor.c \
  quantize.c segment.c shear.c signature.c utility.c widget.c memory.c

# Source files not used for Unix library but which are distributed
EXTRA_libMagick_la_SOURCES = mac.c nt.c vms.c

# Headers which are installed
include_HEADERS = PreRvIcccm.h X.h compress.h config.h error.h gems.h image.h \
	magick.h monitor.h utility.h widget.h memory.h delegates.h

# Headers which are not installed but which are distributed
noinst_HEADERS = Colorlist.h animate.h dicom.h display.h formats.h logo.h \
	mac.h nt.h pict.h proxy.h version.h vms.h

# Additional files to distribute
EXTRA_DIST = Imakefile Make.com

# Ensure that configuration header is not distributed
dist-hook:
	$(RM) $(distdir)/config.h

INCLUDES = #$(X_CFLAGS)

# Since we are building a library, no need for LIBS
#LIBS =
