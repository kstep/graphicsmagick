#  Makefile for building ImageMagick library
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

# Shared libraries built in this directory
lib_LTLIBRARIES = libMagick.la

MAJOR_VERSION=@MAJOR_VERSION@
MINOR_VERSION=@MINOR_VERSION@
MICRO_VERSION=@MICRO_VERSION@

BUILT_SOURCES = config.h

INCLUDES = -I$(top_srcdir)

ALLMANPAGES = Magick-config.1

libMagick_la_LDFLAGS = -version-info $(MAJOR_VERSION):$(MINOR_VERSION)$(MICRO_VERSION):0

# Library base sources
MAGICK_BASE_SRCS=PreRvIcccm.c animate.c annotate.c attribute.c blob.c \
  cache.c cache_view.c color.c compress.c constitute.c decorate.c \
  delegate.c display.c draw.c effect.c enhance.c error.c gem.c image.c \
  magic.c magick.c memory.c module.c monitor.c montage.c quantize.c \
  segment.c semaphore.c shear.c signature.c static.c stream.c timer.c \
  transform.c type.c utility.c widget.c xwindow.c zoom.c

# Library sources
if WITH_MODULES
libMagick_la_SOURCES = $(MAGICK_BASE_SRCS)
libMagick_la_LIBADD = @MAGICK_DEP_LIBS@ $(top_builddir)/ltdl/libltdlc.la
else
libMagick_la_SOURCES = $(MAGICK_BASE_SRCS)
libMagick_la_LIBADD = ../coders/libCoders.la @MAGICK_DEP_LIBS@
endif # WITH_MODULES

# Binary scripts
bin_SCRIPTS = Magick-config

# Headers which are installed
include_HEADERS = PreRvIcccm.h api.h attribute.h blob.h cache.h cache_view.h \
  classify.h compress.h config.h delegate.h draw.h error.h gem.h \
  image.h magic.h magick.h module.h monitor.h quantize.h semaphore.h \
  timer.h utility.h version.h widget.h xwindow.h

# Headers which are not installed but which are distributed
noinst_HEADERS = animate.h define.h display.h mac.h magic.h nt.h vms.h

# Additional files to distribute
EXTRA_DIST = $(ALLMANPAGES) colors.mgk magic.mgk Magick-config.in \
  Magick-config.in Make.com mac.c nt.c vms.c $(MAGICK_BASE_SRCS)

# Where modules are installed
pkgdir = $(libdir)/ImageMagick/modules/coders

# Architecture-dependent data files installed with modules
pkg_DATA = 

# Architecture independent data files installed in the package's data directory
pkgdata_DATA = colors.mgk magic.mgk

# Manual pages to install
man_MANS = $(ALLMANPAGES)

# Ensure that configuration header is not distributed
dist-hook:
	-rm $(distdir)/config.h

DEFS = @DEFS@ -I../
