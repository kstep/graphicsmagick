# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Makefile for building ImageMagick library
#
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

includedir = ${prefix}/include
pkgincludedir = $(includedir)/GraphicsMagick/magick

# Shared libraries built in this directory
lib_LTLIBRARIES = libGraphicsMagick.la

LIBRARY_CURRENT    = @LIBRARY_CURRENT@
LIBRARY_REVISION   = @LIBRARY_REVISION@
LIBRARY_AGE        = @LIBRARY_AGE@

BUILT_SOURCES = magick_config.h magick_config_api.h

INCLUDES = -I$(top_srcdir) $(INCLTDL)

ALLMANPAGES = GraphicsMagick-config.1

libGraphicsMagick_la_LDFLAGS = -no-undefined \
	-version-info $(LIBRARY_CURRENT):$(LIBRARY_REVISION):$(LIBRARY_AGE)

#AM_CFLAGS = @LIBRARY_EXTRA_CPPFLAGS@
AM_CPPFLAGS = @LIBRARY_EXTRA_CPPFLAGS@

# Library base sources
MAGICK_BASE_SRCS = memory.c blob.c cache.c locale.c error.c monitor.c \
	constitute.c image.c composite.c list.c color.c quantize.c segment.c \
	resize.c transform.c shear.c enhance.c effect.c fx.c decorate.c \
	annotate.c type.c draw.c render.c paint.c montage.c semaphore.c attribute.c \
	signature.c compress.c command.c gem.c registry.c magic.c magick.c \
	delegate.c module.c static.c resource.c log.c timer.c utility.c cache_view.c \
	stream.c version.c animate.c display.c widget.c xwindow.c PreRvIcccm.c \
	deprecate.c tempfile.c

# Platform specific source files that may appear in the library
EXTRA_libGraphicsMagick_la_SOURCES = nt_base.c nt_feature.c

if WIN32_NATIVE_BUILD
MAGICK_PLATFORM_SRCS = nt_base.c nt_feature.c
else
if CYGWIN_BUILD
MAGICK_PLATFORM_SRCS = nt_feature.c
else
MAGICK_PLATFORM_SRCS =
endif # if CYGWIN_BUILD
endif # if WIN32_NATIVE_BUILD

#
# Library sources
#
if WITH_LTDL
LIB_LTDL=$(LIBLTDL)
else
LIB_LTDL=
endif

if WITH_MODULES
libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS)
libGraphicsMagick_la_LIBADD = @MAGICK_DEP_LIBS@ $(LIB_LTDL)
else
libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS)
libGraphicsMagick_la_LIBADD = ../coders/libCoders.la ../filters/libFilters.la @MAGICK_DEP_LIBS@ $(LIB_LTDL)
endif # WITH_MODULES

# Binary scripts
bin_SCRIPTS = GraphicsMagick-config

# Headers which are installed
pkginclude_HEADERS = \
	PreRvIcccm.h \
	api.h \
	attribute.h \
	blob.h \
	cache.h \
	cache.h \
	cache_view.h \
	color.h \
	command.h \
	command.h \
	composite.h \
	compress.h \
	constitute.h \
	decorate.h \
	delegate.h \
	deprecate.h \
	draw.h \
	effect.h \
	enhance.h \
	error.h \
	fx.h \
	gem.h \
	image.h \
	list.h \
	log.h \
	magic.h \
	magick.h \
	magick.h \
	module.h \
	monitor.h \
	montage.h \
	paint.h \
	quantize.h \
	registry.h \
	render.h \
	resize.h \
	resource.h \
	semaphore.h \
	shear.h \
	signature.h \
	stream.h \
	timer.h \
	transform.h \
	utility.h \
	version.h \
	widget.h \
	xwindow.h


# Headers which are not installed but which are distributed
noinst_HEADERS = \
	animate.h \
	display.h \
	mac.h \
	nt_base.h \
	nt_feature.h \
	static.h \
	studio.h \
	tempfile.h \
	vms.h

# Install magick_config_api.h as magick_config.h
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
	$(INSTALL_HEADER) magick_config_api.h $(DESTDIR)$(pkgincludedir)/magick_config.h

uninstall-hook:
	rm -f $(DESTDIR)$(pkgincludedir)/magick_config.h

# Additional files to distribute
EXTRA_DIST = $(ALLMANPAGES) colors.mgk log.mgk magic.mgk GraphicsMagick-config.in \
  GraphicsMagick.pc.in Make.com mac.c nt_base.c nt_feature.c vms.c $(MAGICK_BASE_SRCS)

# Where configuration files get installed
pkgdir = @MagickLibPath@

# Architecture-dependent data files installed into ImageMagick/lib
pkg_DATA = colors.mgk log.mgk magic.mgk

# Architecture independent data files installed into ImageMagick/share
pkgdata_DATA = 

# Pkgconfig directory
pkgconfigdir = $(libdir)/pkgconfig

# Files to install in Pkgconfig directory
pkgconfig_DATA = GraphicsMagick.pc

# Manual pages to install
man_MANS = $(ALLMANPAGES)

# Ensure that configuration header is not distributed
dist-hook:
	-rm $(distdir)/magick_config.h
	-rm $(distdir)/magick_config_api.h

DEFS = @DEFS@ -I../
