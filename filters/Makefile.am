#  Makefile for building GraphicsMagick filter modules
#
# Copyright (C) 2002 GraphicsMagick Group, an organization dedicated
# to making software imaging solutions freely available.
#
# Copyright (C) 2002 GraphicsMagick Group, an organization dedicated
# to making software imaging solutions freely available.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# ("GraphicsMagick"), to deal in GraphicsMagick without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of GraphicsMagick,
# and to permit persons to whom GraphicsMagick is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of GraphicsMagick.
#
# The software is provided "as is", without warranty of any kind, express
# or implied, including but not limited to the warranties of
# merchantability, fitness for a particular purpose and noninfringement.
# In no event shall GraphicsMagick Group be liable for any claim,
# damages or other liability, whether in an action of contract, tort or
# otherwise, arising from, out of or in connection with GraphicsMagick
# or the use or other dealings in GraphicsMagick.
#
# Except as contained in this notice, the name of the GraphicsMagick
# Group shall not be used in advertising or otherwise to promote the
# sale, use or other dealings in GraphicsMagick without prior written
# authorization from the GraphicsMagick Group.
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

INCLUDES = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/magick

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

#AM_CFLAGS = @MODULE_EXTRA_CPPFLAGS@
AM_CPPFLAGS = @MODULE_EXTRA_CPPFLAGS@

# Analyze module
analyze_la_SOURCES      = analyze.c
analyze_la_LDFLAGS      = $(MODULECOMMONFLAGS)
analyze_la_LIBADD       = $(LIBMAGICK)

DEFS = @DEFS@ -I../
