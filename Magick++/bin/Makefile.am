#
# Makefile for Magick++ library
#
# Copyright Bob Friesenhahn, 1999, 2000
#
AUTOMAKE_OPTIONS = 1.6

INCLUDES = -I../.. -I$(top_srcdir)

includedir = ${prefix}/include
pkgdatadir = $(datadir)/Magick++
pkglibdir = $(libdir)/Magick++
pkgincludedir = $(includedir)/Magick++

MAJOR_VERSION      = @MAJOR_VERSION@
MINOR_VERSION      = @MINOR_VERSION@
MICRO_VERSION      = @MICRO_VERSION@

# Binary scripts
bin_SCRIPTS = Magick++-config

# Manual pages
ALLMANPAGES = Magick++-config.1

# Manual pages to install
man_MANS = $(ALLMANPAGES)

EXTRA_DIST = $(ALLMANPAGES) Magick++-config.in
