# Copyright (C) 2004 GraphicsMagick Group

# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building GraphicsMagick library
#
#

magickincdir = $(topincludedir)/magick

# Headers which are installed
magickinc_HEADERS = \
	$(MAGICK_INCLUDE_HDRS)

MAGICK_BIN_SCRPTS = \
        magick/GraphicsMagick-config

MAGICK_PKGCONFIG = \
        magick/GraphicsMagick.pc

MAGICK_MANS = \
        magick/GraphicsMagick-config.1

MAGICK_BUILT_SRCS = \
	magick/magick_config.h \
	magick/magick_config_api.h

LIBMAGICK=magick/libGraphicsMagick.la

if WITH_MODULES
magick_libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS)
magick_libGraphicsMagick_la_LIBADD = $(MAGICK_DEP_LIBS) $(LIBLTDL)
else
magick_libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS) $(MAGICK_CODER_SRCS) $(MAGICK_FILTER_SRCS)
magick_libGraphicsMagick_la_LIBADD = $(MAGICK_DEP_LIBS) $(LIBLTDL)
endif # WITH_MODULES
magick_libGraphicsMagick_la_CPPFLAGS = -I$(top_builddir)/ltdl -I$(top_srcdir)/ltdl $(LIBRARY_EXTRA_CPPFLAGS)
magick_libGraphicsMagick_la_LDFLAGS = -no-undefined \
	-version-info $(MAGICK_LIBRARY_CURRENT):$(MAGICK_LIBRARY_REVISION):$(MAGICK_LIBRARY_AGE)

# Library base sources
MAGICK_BASE_SRCS = \
	magick/alpha_composite.h \
	magick/animate.c \
	magick/animate.h \
	magick/annotate.c \
	magick/api.h \
	magick/attribute.c \
	magick/attribute.h \
	magick/bit_stream.h \
	magick/blob.c \
	magick/blob.h \
	magick/cache.c \
	magick/cache.h \
	magick/cache_view.c \
	magick/cache_view.h \
	magick/channel.c \
	magick/channel.h \
	magick/color.c \
	magick/color.h \
	magick/colorspace.c \
	magick/colorspace.h \
	magick/command.c \
	magick/command.h \
	magick/composite.c \
	magick/composite.h \
	magick/compress.c \
	magick/compress.h \
	magick/constitute.c \
	magick/constitute.h \
	magick/decorate.c \
	magick/decorate.h \
	magick/delegate.c \
	magick/delegate.h \
	magick/deprecate.c \
	magick/deprecate.h \
	magick/display.c \
	magick/display.h \
	magick/draw.c \
	magick/draw.h \
	magick/effect.c \
	magick/effect.h \
	magick/endian.c \
	magick/endian.h \
	magick/enhance.c \
	magick/enhance.h \
	magick/error.c \
	magick/error.h \
	magick/fx.c \
	magick/fx.h \
	magick/gem.c \
	magick/gem.h \
	magick/image.c \
	magick/image.h \
	magick/list.c \
	magick/list.h \
	magick/locale.c \
	magick/locale_c.h \
	magick/log.c \
	magick/log.h \
	magick/magic.c \
	magick/magic.h \
	magick/magick.c \
	magick/magick.h \
	magick/map.c \
	magick/map.h \
	magick/memory.c \
	magick/module.c \
	magick/module.h \
	magick/monitor.c \
	magick/monitor.h \
	magick/montage.c \
	magick/montage.h \
	magick/operator.c \
	magick/operator.h \
	magick/paint.c \
	magick/paint.h \
	magick/pixel_iterator.c \
	magick/pixel_iterator.h \
	magick/PreRvIcccm.c \
	magick/PreRvIcccm.h \
	magick/profile.c \
	magick/profile.h \
	magick/quantize.c \
	magick/quantize.h \
	magick/registry.c \
	magick/registry.h \
	magick/render.c \
	magick/render.h \
	magick/resize.c \
	magick/resize.h \
	magick/resource.c \
	magick/resource.h \
	magick/segment.c \
	magick/semaphore.c \
	magick/semaphore.h \
	magick/shear.c \
	magick/shear.h \
	magick/signature.c \
	magick/signature.h \
	magick/spinlock.h \
	magick/static.c \
	magick/static.h \
	magick/stream.c \
	magick/stream.h \
	magick/studio.h \
	magick/symbols.h \
	magick/tempfile.c \
	magick/tempfile.h \
	magick/timer.c \
	magick/timer.h \
	magick/transform.c \
	magick/transform.h \
	magick/tsd.c \
	magick/tsd.h \
	magick/type.c \
	magick/utility.c \
	magick/utility.h \
	magick/version.c \
	magick/version.h \
	magick/widget.c \
	magick/widget.h \
	magick/xwindow.c \
	magick/xwindow.h

if WIN32_NATIVE_BUILD
MAGICK_PLATFORM_SRCS = 	\
	magick/nt_base.c \
	magick/nt_base.h \
	magick/nt_feature.c \
	magick/nt_feature.h
else
if CYGWIN_BUILD
MAGICK_PLATFORM_SRCS = \
	magick/nt_feature.c \
	magick/nt_feature.h
else
MAGICK_PLATFORM_SRCS =
endif # if CYGWIN_BUILD
endif # if WIN32_NATIVE_BUILD

MAGICK_INCLUDE_HDRS = \
	magick/PreRvIcccm.h \
	magick/api.h \
	magick/attribute.h \
	magick/blob.h \
	magick/cache.h \
	magick/cache.h \
	magick/cache_view.h \
	magick/channel.h \
	magick/color.h \
	magick/colorspace.h \
	magick/command.h \
	magick/command.h \
	magick/composite.h \
	magick/compress.h \
	magick/constitute.h \
	magick/decorate.h \
	magick/delegate.h \
	magick/deprecate.h \
	magick/draw.h \
	magick/effect.h \
	magick/enhance.h \
	magick/error.h \
	magick/fx.h \
	magick/gem.h \
	magick/image.h \
	magick/list.h \
	magick/log.h \
	magick/magic.h \
	magick/magick.h \
	magick/magick_types.h \
	magick/module.h \
	magick/monitor.h \
	magick/montage.h \
	magick/operator.h \
	magick/paint.h \
	magick/profile.h \
	magick/quantize.h \
	magick/registry.h \
	magick/render.h \
	magick/resize.h \
	magick/resource.h \
	magick/shear.h \
	magick/signature.h \
	magick/stream.h \
	magick/symbols.h \
	magick/timer.h \
	magick/transform.h \
	magick/utility.h \
	magick/version.h \
	magick/widget.h \
	magick/xwindow.h

MAGICK_NOINST_HDRS = \
	magick/animate.h \
	magick/bit_stream.h \
	magick/display.h \
	magick/locale_c.h \
	magick/mac.h \
	magick/map.h \
	magick/nt_base.h \
	magick/nt_feature.h \
	magick/pixel_iterator.h \
	magick/semaphore.h \
	magick/spinlock.h \
	magick/static.h \
	magick/studio.h \
	magick/tempfile.h \
	magick/vms.h

MAGICK_EXTRA_DIST = \
	magick/GraphicsMagick-config.in \
	$(MAGICK_MANS) \
	magick/GraphicsMagick.pc.in \
	magick/Make.com \
	magick/mac.c \
	magick/nt_base.c \
	magick/nt_feature.c \
	magick/vms.c

# Install magick_config_api.h as magick_config.h
MAGICK_INSTALL_DATA_LOCAL_TARGETS = magick-install-data-local
magick-install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(magickincdir)
	$(INSTALL_HEADER) magick/magick_config_api.h $(DESTDIR)$(magickincdir)/magick_config.h

# Uninstall magick_config.h
MAGICK_UNINSTALL_LOCAL_TARGETS = magick-uninstall-local
magick-uninstall-local:
	rm -f $(DESTDIR)$(magickincdir)/magick_config.h

