# Copyright (C) 2004 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building GraphicsMagick filter modules
#
#

# Where filter modules get installed
filtersdir = $(MagickFilterModulesPath)

MAGICK_FILTER_SRCS = \
	filters/analyze.c


if WITH_MODULES
filters_LTLIBRARIES = \
	filters/analyze.la
else
filters_LTLIBRARIES =
endif # WITH_MODULES

# Analyze filter module
filters_analyze_la_SOURCES      = filters/analyze.c
filters_analyze_la_LDFLAGS      = $(MODULECOMMONFLAGS)
filters_analyze_la_LIBADD       = $(LIBMAGICK)
