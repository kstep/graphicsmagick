#
# Makefile for Magick++ library
#
# Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
#
AUTOMAKE_OPTIONS = 1.6

SUBDIRS = Magick++

INCLUDES = -I../.. -I$(top_srcdir)

includedir = ${prefix}/include
pkgdatadir = $(datadir)/Magick++
pkglibdir = $(libdir)/Magick++
pkgincludedir = $(includedir)/Magick++

lib_LTLIBRARIES    = libMagick++.la
MAJOR_VERSION      = @MAJOR_VERSION@
MINOR_VERSION      = @MINOR_VERSION@
MICRO_VERSION      = @MICRO_VERSION@

libMagick___la_SOURCES = \
	Blob.cpp CoderInfo.cpp Color.cpp Drawable.cpp Exception.cpp \
	Functions.cpp Geometry.cpp Image.cpp Montage.cpp Options.cpp \
	Pixels.cpp STL.cpp Thread.cpp TypeMetric.cpp

libMagick___la_LDFLAGS = -no-undefined \
	-version-info $(MAJOR_VERSION):$(MINOR_VERSION)$(MICRO_VERSION):0

libMagick___la_LIBADD = ../../magick/libMagick.la

include_HEADERS = Magick++.h

EXTRA_DIST =

LDFLAGS = @LDFLAGS@ $(LIBSTDCLDFLAGS)
