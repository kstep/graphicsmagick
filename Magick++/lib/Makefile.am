#
# Makefile for Magick++ library
#
# Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002, 2003, 2004
#
AUTOMAKE_OPTIONS = 1.8 nostdinc

SUBDIRS = Magick++

AM_CPPFLAGS = -I$(srcdir) -I../.. -I$(top_srcdir)

includedir = $(prefix)/include/GraphicsMagick
pkgdatadir = $(datadir)/Magick++
pkglibdir = $(libdir)/Magick++
pkgincludedir = $(includedir)/Magick++

AM_CXXFLAGS=$(LTCXXLIBOPTS)

lib_LTLIBRARIES    = libGraphicsMagick++.la

LIBRARY_CURRENT    = @MAGICK_PLUS_PLUS_LIBRARY_CURRENT@
LIBRARY_REVISION   = @MAGICK_PLUS_PLUS_LIBRARY_REVISION@
LIBRARY_AGE        = @MAGICK_PLUS_PLUS_LIBRARY_AGE@

libGraphicsMagick___la_SOURCES = \
	Blob.cpp BlobRef.cpp CoderInfo.cpp Color.cpp Drawable.cpp \
	Exception.cpp Functions.cpp Geometry.cpp Image.cpp ImageRef.cpp \
	Montage.cpp Options.cpp Pixels.cpp STL.cpp Thread.cpp TypeMetric.cpp

libGraphicsMagick___la_LDFLAGS = \
	-version-info $(LIBRARY_CURRENT):$(LIBRARY_REVISION):$(LIBRARY_AGE)

libGraphicsMagick___la_LIBADD = $(top_builddir)/magick/libGraphicsMagick.la

include_HEADERS = Magick++.h

EXTRA_DIST = GraphicsMagick++.pc.in

AM_LDFLAGS = @LDFLAGS@ $(LIBSTDCLDFLAGS)

# Pkgconfig directory
pkgconfigdir = $(libdir)/pkgconfig

# Files to install in Pkgconfig directory
pkgconfig_DATA = GraphicsMagick++.pc
