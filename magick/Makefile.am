# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building GraphicsMagick library
#
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

includedir = ${prefix}/include
pkgincludedir = $(includedir)/GraphicsMagick/magick

# Shared libraries built in this directory
lib_LTLIBRARIES = libGraphicsMagick.la

LIBRARY_CURRENT    = $(MAGICK_LIBRARY_CURRENT)
LIBRARY_REVISION   = $(MAGICK_LIBRARY_REVISION)
LIBRARY_AGE        = $(MAGICK_LIBRARY_AGE)

BUILT_SOURCES = magick_config.h magick_config_api.h

ALLMANPAGES = GraphicsMagick-config.1

libGraphicsMagick_la_LDFLAGS = -no-undefined \
	-version-info $(LIBRARY_CURRENT):$(LIBRARY_REVISION):$(LIBRARY_AGE)

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) $(INCLTDL) $(LIBRARY_EXTRA_CPPFLAGS)

# Library base sources
MAGICK_BASE_SRCS = \
	animate.c \
	annotate.c \
	attribute.c \
	blob.c \
	cache.c \
	cache_view.c \
	color.c \
	command.c \
	composite.c \
	compress.c \
	constitute.c \
	decorate.c \
	delegate.c \
	deprecate.c \
	display.c \
	draw.c \
	effect.c \
	enhance.c \
	error.c \
	fx.c \
	gem.c \
	image.c \
	list.c \
	locale.c \
	log.c \
	magic.c \
	magick.c \
	map.c \
	memory.c \
	module.c \
	monitor.c \
	montage.c \
	paint.c \
	PreRvIcccm.c \
	quantize.c \
	registry.c \
	render.c \
	resize.c \
	resource.c \
	segment.c \
	semaphore.c \
	shear.c \
	signature.c \
	static.c \
	stream.c \
	tempfile.c \
	timer.c \
	transform.c \
	type.c \
	utility.c \
	version.c \
	widget.c \
	xwindow.c

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
libGraphicsMagick_la_LIBADD = $(MAGICK_DEP_LIBS) $(LIB_LTDL)
else
libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS)
libGraphicsMagick_la_LIBADD = ../coders/libCoders.la ../filters/libFilters.la $(MAGICK_DEP_LIBS) $(LIB_LTDL)
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
	magick_types.h \
	module.h \
	monitor.h \
	montage.h \
	paint.h \
	quantize.h \
	registry.h \
	render.h \
	resize.h \
	resource.h \
	shear.h \
	signature.h \
	stream.h \
	symbols.h \
	timer.h \
	transform.h \
	utility.h \
	version.h \
	widget.h \
	xwindow.h


# Headers which are not installed but which are distributed
noinst_HEADERS = \
	animate.h \
	bit_stream.h \
	display.h \
	locale_c.h \
	mac.h \
	map.h \
	nt_base.h \
	nt_feature.h \
	semaphore.h \
	spinlock.h \
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
EXTRA_DIST = \
	$(ALLMANPAGES) \
	GraphicsMagick-config.in \
	GraphicsMagick.pc.in \
	Make.com mac.c \
	nt_base.c \
	nt_feature.c \
	vms.c \
	$(MAGICK_BASE_SRCS)

# Where configuration files get installed
pkgdir = $(MagickLibPath)

# Architecture-dependent data files installed into GraphicsMagick/lib
pkg_DATA = 

# Architecture independent data files installed into GraphicsMagick/share
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
