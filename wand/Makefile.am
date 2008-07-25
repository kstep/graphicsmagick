# Copyright (C) 2004 GraphicsMagick Group

# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building Wand library
#

wandincdir = $(topincludedir)/wand

WAND_SOURCES = \
        wand/drawing_wand.c \
	wand/drawing_wand.h \
	wand/magick_compat.c \
	wand/magick_compat.h \
	wand/magick_wand.c \
	wand/magick_wand.h \
	wand/pixel_wand.c \
	wand/pixel_wand.h \
	wand/wand_api.h

WAND_INCLUDE_HDRS = \
	wand/drawing_wand.h \
	wand/magick_wand.h \
	wand/pixel_wand.h \
	wand/wand_api.h

# Headers which are installed
wandinc_HEADERS = \
	$(WAND_INCLUDE_HDRS)

WAND_BIN_SCRPTS = \
        wand/GraphicsMagickWand-config

WAND_PKGCONFIG = \
        wand/GraphicsMagickWand.pc

WAND_MANS = \
        wand/GraphicsMagickWand-config.1

LIBWAND=wand/libGraphicsMagickWand.la

wand_libGraphicsMagickWand_la_SOURCES = $(WAND_SOURCES)
wand_libGraphicsMagickWand_la_LDFLAGS = -no-undefined -export-symbols-regex ".*" \
	-version-info $(MAGICK_WAND_LIBRARY_CURRENT):$(MAGICK_WAND_LIBRARY_REVISION):$(MAGICK_WAND_LIBRARY_AGE)
wand_libGraphicsMagickWand_la_LIBADD = $(LIBMAGICK) $(LIB_MATH)

WAND_EXTRA_DIST = \
	wand/GraphicsMagickWand-config.1 \
	wand/input_256c.miff \
	wand/input_bilevel.miff \
	wand/input_gray.miff \
	wand/input_truecolor.miff \
	wand/sequence.miff \
	wand/common.sh \
	$(WAND_TESTS)

wand_drawtest_LDADD        = $(LIBWAND)
wand_drawtest_LDFLAGS      = $(LDFLAGS)
wand_drawtest_SOURCES      = wand/drawtest.c

wand_wandtest_LDADD        = $(LIBWAND)
wand_wandtest_LDFLAGS      = $(LDFLAGS)
wand_wandtest_SOURCES      = wand/wandtest.c

WAND_CHECK_PGRMS = \
	wand/drawtest \
	wand/wandtest

WAND_TESTS = \
	wand/drawtest.sh \
	wand/wandtest.sh

WAND_CLEANFILES = \
        wand/*_out.*
