# Copyright (C) 2004 GraphicsMagick Group

# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for Magick library configuration files
#
#

# Where architecture-independent configuration files get installed
# (share/GraphicsMagick-version)
configsharedir = $(MagickShareConfigPath)
configshare_DATA = \
	config/colors.mgk \
	config/log.mgk \
	config/modules.mgk

# Where architecture-dependent configuration files get installed
# (lib/GraphicsMagick-version)
configlibdir =  $(MagickLibConfigPath)
configlib_DATA = \
	config/delegates.mgk \
	config/type-ghostscript.mgk \
	config/type.mgk \
	config/type-solaris.mgk \
	config/type-windows.mgk

CONFIG_EXTRA_DIST = \
        config/colors.mgk \
	config/delegates.mgk.in \
        config/log.mgk \
	config/modules.mgk \
	config/type-ghostscript.mgk.in \
	config/type-solaris.mgk.in \
	config/type-windows.mgk.in \
	config/type.mgk.in

