#
# Makefile for Magick++ library
#
# Copyright Bob Friesenhahn, 1999, 2000
#
AUTOMAKE_OPTIONS = 1.7

INCLUDES = -I../.. -I$(top_srcdir)

includedir = ${prefix}/include
pkgdatadir = $(datadir)/Magick++
pkglibdir = $(libdir)/Magick++
pkgincludedir = $(includedir)/Magick++

MAJOR_VERSION      = @MAJOR_VERSION@
MINOR_VERSION      = @MINOR_VERSION@
MICRO_VERSION      = @MICRO_VERSION@

# Binary scripts
bin_SCRIPTS = GraphicsMagick++-config

# Manual pages
ALLMANPAGES = GraphicsMagick++-config.1

# Manual pages to install
man_MANS = $(ALLMANPAGES)

EXTRA_DIST = $(ALLMANPAGES) GraphicsMagick++-config.in
