#
# Makefile for Magick++ library
#
# Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
#
AUTOMAKE_OPTIONS = 1.7 nostdinc

SUBDIRS = Magick++

INCLUDES = -I$(srcdir) -I../.. -I$(top_srcdir)

includedir = $(prefix)/include
pkgdatadir = $(datadir)/Magick++
pkglibdir = $(libdir)/Magick++
pkgincludedir = $(includedir)/Magick++

lib_LTLIBRARIES    = libMagick++.la
MAJOR_VERSION      = @MAJOR_VERSION@
MINOR_VERSION      = @MINOR_VERSION@
MICRO_VERSION      = @MICRO_VERSION@

libMagick___la_SOURCES = \
	Blob.cpp BlobRef.cpp CoderInfo.cpp Color.cpp Drawable.cpp \
	Exception.cpp Functions.cpp Geometry.cpp Image.cpp ImageRef.cpp \
	Montage.cpp Options.cpp Pixels.cpp STL.cpp Thread.cpp TypeMetric.cpp

# Add -no-undefined once Cygwin supports throwing C++ exceptions from DLLs.
libMagick___la_LDFLAGS = \
	-release $(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)

libMagick___la_LIBADD = ../../magick/libMagick.la

include_HEADERS = Magick++.h

EXTRA_DIST =

AM_LDFLAGS = @LDFLAGS@ $(LIBSTDCLDFLAGS)
