# Copyright (C) 2003 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building Magick Wand library
#
#
# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

includedir = ${prefix}/include/GraphicsMagick
pkgincludedir = $(includedir)/wand

lib_LTLIBRARIES = libGraphicsMagickWand.la

LIBRARY_CURRENT    = @MAGICK_WAND_LIBRARY_CURRENT@
LIBRARY_REVISION   = @MAGICK_WAND_LIBRARY_REVISION@
LIBRARY_AGE        = @MAGICK_WAND_LIBRARY_AGE@

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)

libGraphicsMagickWand_la_SOURCES = drawing_wand.c magick_wand.c pixel_wand.c \
	magick_compat.c

libGraphicsMagickWand_la_LDFLAGS = -no-undefined \
	-version-info $(LIBRARY_CURRENT):$(LIBRARY_REVISION):$(LIBRARY_AGE)

libGraphicsMagickWand_la_LIBADD = $(top_builddir)/magick/libGraphicsMagick.la

check_PROGRAMS = drawtest wandtest

drawtest_LDADD     = libGraphicsMagickWand.la
drawtest_LDFLAGS   = @LDFLAGS@
drawtest_SOURCES   = drawtest.c

wandtest_LDADD     = libGraphicsMagickWand.la
wandtest_LDFLAGS   = @LDFLAGS@
wandtest_SOURCES   = wandtest.c

# Binary scripts
bin_SCRIPTS = GraphicsMagickWand-config

# Headers which are installed
pkginclude_HEADERS = drawing_wand.h magick_wand.h pixel_wand.h wand_api.h

# Headers which are not installed but which are distributed
noinst_HEADERS = magick_compat.h

# Pkgconfig directory
pkgconfigdir = $(libdir)/pkgconfig

# Files to install in Pkgconfig directory
pkgconfig_DATA = GraphicsMagickWand.pc

EXTRA_DIST = GraphicsMagickWand-config.1 input_256c.miff input_bilevel.miff \
	input_gray.miff input_truecolor.miff sequence.miff

# Manual pages to install
man_MANS = GraphicsMagickWand-config.1

if WIN32_NATIVE_BUILD
SRCDIR='$(shell @WinPathScript@ $(srcdir)/)'
else
SRCDIR='$(srcdir)/'
endif

RUNENV = $(SHELL) $(top_builddir)/rungm.sh

check:
	-@$(RUNENV) ./drawtest drawtest_out.miff
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./wandtest
