#
# Makefile for Magick++ library
#
# Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002, 2003
#
AUTOMAKE_OPTIONS = 1.7 nostdinc

SUBDIRS = Magick++

INCLUDES = -I$(srcdir) -I../.. -I$(top_srcdir)

includedir = $(prefix)/include/GraphicsMagick
pkgdatadir = $(datadir)/Magick++
pkglibdir = $(libdir)/Magick++
pkgincludedir = $(includedir)/Magick++

lib_LTLIBRARIES    = libGraphicsMagick++.la

LIBRARY_CURRENT    = @LIBRARY_CURRENT@
LIBRARY_REVISION   = @LIBRARY_REVISION@
LIBRARY_AGE        = @LIBRARY_AGE@

libGraphicsMagick___la_SOURCES = \
	Blob.cpp BlobRef.cpp CoderInfo.cpp Color.cpp Drawable.cpp \
	Exception.cpp Functions.cpp Geometry.cpp Image.cpp ImageRef.cpp \
	Montage.cpp Options.cpp Pixels.cpp STL.cpp Thread.cpp TypeMetric.cpp

# Add -no-undefined once Cygwin supports throwing C++ exceptions from DLLs.
libGraphicsMagick___la_LDFLAGS = \
	-version-info $(LIBRARY_CURRENT):$(LIBRARY_REVISION):$(LIBRARY_AGE)

libGraphicsMagick___la_LIBADD = ../../magick/libGraphicsMagick.la

include_HEADERS = Magick++.h

EXTRA_DIST =

AM_LDFLAGS = @LDFLAGS@ $(LIBSTDCLDFLAGS)
