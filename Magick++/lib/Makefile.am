#
# Makefile for Magick++ library
#
# Copyright Bob Friesenhahn, 1999, 2000
#
AUTOMAKE_OPTIONS = 1.4

SUBDIRS = Magick++

lib_LTLIBRARIES    = libMagick++.la
MAJOR_VERSION      = @MAJOR_VERSION@
MINOR_VERSION      = @MINOR_VERSION@
MICRO_VERSION      = @MICRO_VERSION@

libMagick___la_SOURCES = \
	Blob.cpp Color.cpp Drawable.cpp Exception.cpp \
	Functions.cpp Geometry.cpp Options.cpp Thread.cpp \
	Image.cpp Montage.cpp Pixels.cpp

libMagick___la_LDFLAGS = -version-info $(MAJOR_VERSION):$(MINOR_VERSION)$(MICRO_VERSION):0

include_HEADERS = Magick++.h

EXTRA_DIST =
