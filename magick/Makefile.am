#  Makefile for building ImageMagick library
#
# Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated
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
AUTOMAKE_OPTIONS = 1.7 foreign

includedir = ${prefix}/include
pkgincludedir = $(includedir)/GraphicsMagick/magick

# Shared libraries built in this directory
lib_LTLIBRARIES = libGraphicsMagick.la

MAJOR_VERSION=@MAJOR_VERSION@
MINOR_VERSION=@MINOR_VERSION@
MICRO_VERSION=@MICRO_VERSION@

BUILT_SOURCES = magick_config.h

INCLUDES = -I$(top_srcdir) $(INCLTDL)

ALLMANPAGES = GraphicsMagick-config.1

libGraphicsMagick_la_LDFLAGS = -no-undefined \
	-release $(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION) \
	-version-info 0:0:0

#AM_CFLAGS = @LIBRARY_EXTRA_CPPFLAGS@
AM_CPPFLAGS = @LIBRARY_EXTRA_CPPFLAGS@

# Library base sources
MAGICK_BASE_SRCS = memory.c blob.c cache.c locale.c error.c monitor.c \
	constitute.c image.c composite.c list.c color.c quantize.c segment.c \
	resize.c transform.c shear.c enhance.c effect.c fx.c decorate.c \
	annotate.c type.c draw.c render.c paint.c montage.c semaphore.c attribute.c \
	signature.c random.c compress.c command.c gem.c registry.c magic.c magick.c \
	delegate.c module.c static.c resource.c log.c timer.c utility.c cache_view.c \
	stream.c version.c animate.c display.c widget.c xwindow.c PreRvIcccm.c \
	deprecate.c

# Platform specific source files that may appear in the library
EXTRA_libGraphicsMagick_la_SOURCES = nt_base.c nt_feature.c

if WIN32_NATIVE_BUILD
MAGICK_PLATFORM_SRCS = nt_base.c nt_feature.c
else
if CYGWIN_BUILD
MAGICK_PLATFORM_SRCS = nt_feature.c
else
MAGICK_PLATFORM_SRCS =
endif # if CYGWIN_BUILD
endif # if WIN32_NATIVE_BUILD

#
# Library sources
#
if WITH_LTDL
LIB_LTDL=$(LIBLTDL)
else
LIB_LTDL=
endif

if WITH_MODULES
libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS)
libGraphicsMagick_la_LIBADD = @MAGICK_DEP_LIBS@ $(LIB_LTDL)
else
libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS)
libGraphicsMagick_la_LIBADD = ../coders/libCoders.la @MAGICK_DEP_LIBS@ $(LIB_LTDL)
endif # WITH_MODULES

# Binary scripts
bin_SCRIPTS = GraphicsMagick-config

# Headers which are installed
pkginclude_HEADERS = api.h attribute.h blob.h cache.h \
  cache_view.h command.h compress.h magick_config.h delegate.h \
  draw.h render.h error.h gem.h image.h list.h log.h magic.h magick.h \
  module.h monitor.h quantize.h paint.h random.h resource.h registry.h \
  semaphore.h signature.h timer.h utility.h version.h widget.h xwindow.h \
  PreRvIcccm.h deprecate.h

# Headers which are not installed but which are distributed
noinst_HEADERS = animate.h display.h mac.h nt_base.h nt_feature.h \
  static.h studio.h vms.h

# Additional files to distribute
EXTRA_DIST = $(ALLMANPAGES) colors.mgk log.mgk magic.mgk GraphicsMagick-config.in \
  Make.com mac.c nt_base.c nt_feature.c vms.c $(MAGICK_BASE_SRCS)

# Where configuration files get installed
pkgdir = @MagickLibPath@

# Architecture-dependent data files installed into ImageMagick/lib
pkg_DATA = colors.mgk log.mgk magic.mgk

# Architecture independent data files installed into ImageMagick/share
pkgdata_DATA = 

# Manual pages to install
man_MANS = $(ALLMANPAGES)

# Ensure that configuration header is not distributed
dist-hook:
	-rm $(distdir)/magick_config.h

DEFS = @DEFS@ -I../
