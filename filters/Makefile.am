# Copyright (C) 2003 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Makefile for building GraphicsMagick filter modules
#
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.8 foreign

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) @MODULE_EXTRA_CPPFLAGS@

MAGICK_FILTER_SRCS = \
  analyze.c

if WITH_MODULES
noinst_LTLIBRARIES =
else
noinst_LTLIBRARIES = libFilters.la
libFilters_la_SOURCES = $(MAGICK_FILTER_SRCS)
EXTRA_libFilters_la_SOURCES =  # Platform dependent modules
endif # WITH_MODULES

# Additional files to distribute
EXTRA_DIST = 

# Headers which are not installed but which are distributed
noinst_HEADERS = 

# Location of libGraphicsMagick.la
LIBMAGICK = ../magick/libGraphicsMagick.la

# Where filter modules are installed
pkgdir = @MagickFilterModulesPath@

# Where config files get installed
pkglibdir = @MagickLibPath@

# Architecture-dependent data files installed with modules
pkg_DATA = 

# Architecture independent data files installed into GraphicsMagick/share
pkgdata_DATA =

if WITH_MODULES

# Modules which are to be built
pkg_LTLIBRARIES = \
  analyze.la
MODULECOMMONFLAGS = -no-undefined -module -avoid-version
else
pkg_LTLIBRARIES =
MODULECOMMONFLAGS     =
endif # WITH_MODULES

# Analyze module
analyze_la_SOURCES      = analyze.c
analyze_la_LDFLAGS      = $(MODULECOMMONFLAGS)
analyze_la_LIBADD       = $(LIBMAGICK)

DEFS = @DEFS@ -I../
