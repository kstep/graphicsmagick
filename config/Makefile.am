# Copyright (C) 2003 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building and installing GraphicsMagick run-time
# configuration files.
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

# Additional files to distribute
EXTRA_DIST = \
	colors.mgk \
	delegates.mgk.in \
	log.mgk \
	modules.mgk \
	magic.mgk \
	type-ghostscript.mgk.in \
	type.mgk.in \
	type-solaris.mgk.in \
	type-windows.mgk.in

# Where package files get installed (lib/GraphicsMagick-version)
pkgdir = $(MagickLibConfigPath)

# Where architecture-independent files get installed (share/GraphicsMagick-version)
pkgdatadir = $(MagickShareConfigPath)

# Architecture-dependent data files installed into pkgdir
pkg_DATA = \
	colors.mgk \
	delegates.mgk \
	type-ghostscript.mgk \
	type.mgk \
	type-solaris.mgk \
	type-windows.mgk

# Architecture independent data files installed into pkgdatadir
pkgdata_DATA =	\
	log.mgk \
	modules.mgk \
	magic.mgk
