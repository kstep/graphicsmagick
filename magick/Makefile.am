# Copyright (C) 2004-2014 GraphicsMagick Group

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

LIBMAGICK=magick/libGraphicsMagick.la

if WITH_MODULES
magick_libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS)
magick_libGraphicsMagick_la_LIBADD = $(MAGICK_DEP_LIBS)
else
magick_libGraphicsMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_PLATFORM_SRCS) $(MAGICK_CODER_SRCS) $(MAGICK_FILTER_SRCS)
magick_libGraphicsMagick_la_LIBADD = $(MAGICK_DEP_LIBS)
endif # WITH_MODULES
magick_libGraphicsMagick_la_CPPFLAGS = $(AM_CPPFLAGS) $(LIBRARY_EXTRA_CPPFLAGS)
magick_libGraphicsMagick_la_LDFLAGS = \
	-no-undefined -export-symbols-regex ".*" \
	$(MAGICK_LT_RELEASE_OPTS) \
	-version-info $(MAGICK_LIBRARY_CURRENT):$(MAGICK_LIBRARY_REVISION):$(MAGICK_LIBRARY_AGE)
#magick_libGraphicsMagick_la_DEPENDENCIES = 

if HasX11
MAGICK_X11_BASE_SRCS = \
	magick/animate.c \
	magick/animate.h \
	magick/display.c \
	magick/display.h \
	magick/PreRvIcccm.c \
	magick/PreRvIcccm.h \
	magick/widget.c \
	magick/widget.h \
	magick/xwindow.c \
	magick/xwindow.h
endif

# Library base sources
MAGICK_BASE_SRCS = \
	magick/alpha_composite.h \
	magick/analyze.c \
	magick/analyze.h \
	magick/annotate.c \
	magick/api.h \
	magick/attribute.c \
	magick/attribute.h \
	magick/average.c \
	magick/average.h \
	magick/bit_stream.c \
	magick/bit_stream.h \
	magick/blob.c \
	magick/blob.h \
	magick/cdl.c \
	magick/cdl.h \
	magick/channel.c \
	magick/channel.h \
	magick/common.h \
	magick/compare.c \
	magick/compare.h \
	magick/confirm_access.c \
	magick/confirm_access.h \
	magick/color.c \
	magick/color.h \
	magick/color_lookup.c \
	magick/color_lookup.h \
	magick/colormap.c \
	magick/colormap.h \
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
	magick/describe.c \
	magick/describe.h \
	magick/draw.c \
	magick/draw.h \
	magick/effect.c \
	magick/effect.h \
	magick/enhance.c \
	magick/enhance.h \
	magick/enum_strings.c \
	magick/enum_strings.h \
	magick/error.c \
	magick/error.h \
	magick/export.c \
	magick/floats.c \
	magick/floats.h \
	magick/forward.h \
	magick/fx.c \
	magick/fx.h \
	magick/gem.c \
	magick/gem.h \
	magick/gradient.c \
	magick/gradient.h \
	magick/hclut.c \
	magick/hclut.h \
	magick/image.c \
	magick/image.h \
	magick/import.c \
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
	magick/magick_endian.c \
	magick/magick_endian.h \
	magick/map.c \
	magick/map.h \
	magick/memory.c \
	magick/memory.h \
	magick/module.c \
	magick/module.h \
	magick/monitor.c \
	magick/monitor.h \
	magick/montage.c \
	magick/montage.h \
	magick/omp_data_view.c \
	magick/omp_data_view.h \
	magick/operator.c \
	magick/operator.h \
	magick/paint.c \
	magick/paint.h \
	magick/pixel_cache.h \
	magick/pixel_cache.c \
	magick/pixel_iterator.c \
	magick/pixel_iterator.h \
	magick/plasma.c \
	magick/plasma.h \
	magick/prefetch.h \
	magick/profile.c \
	magick/profile.h \
	magick/quantize.c \
	magick/quantize.h \
	magick/registry.c \
	magick/registry.h \
	magick/random.c \
	magick/random.h \
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
	magick/statistics.c \
	magick/statistics.h \
	magick/studio.h \
	magick/symbols.h \
	magick/tempfile.c \
	magick/tempfile.h \
	magick/texture.c \
	magick/texture.h \
	magick/timer.c \
	magick/timer.h \
	magick/transform.c \
	magick/transform.h \
	magick/tsd.c \
	magick/tsd.h \
	magick/type.c \
	magick/type.h \
	magick/unix_port.c \
	magick/utility.c \
	magick/utility.h \
	magick/version.c \
	magick/version.h \
	$(MAGICK_X11_BASE_SRCS)

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
	magick/api.h \
	magick/analyze.h \
	magick/attribute.h \
	magick/average.h \
	magick/blob.h \
	magick/cdl.h \
	magick/channel.h \
	magick/color.h \
	magick/color_lookup.h \
	magick/colormap.h \
	magick/colorspace.h \
	magick/command.h \
	magick/common.h \
	magick/compare.h \
	magick/composite.h \
	magick/compress.h \
	magick/confirm_access.h \
	magick/constitute.h \
	magick/decorate.h \
	magick/delegate.h \
	magick/describe.h \
	magick/deprecate.h \
	magick/draw.h \
	magick/effect.h \
	magick/enhance.h \
	magick/error.h \
	magick/forward.h \
	magick/fx.h \
	magick/gem.h \
	magick/gradient.h \
	magick/hclut.h \
	magick/image.h \
	magick/list.h \
	magick/log.h \
	magick/magic.h \
	magick/magick.h \
	magick/magick_types.h \
	magick/memory.h \
	magick/module.h \
	magick/monitor.h \
	magick/montage.h \
	magick/operator.h \
	magick/paint.h \
	magick/pixel_cache.h \
	magick/pixel_iterator.h \
	magick/plasma.h \
	magick/profile.h \
	magick/quantize.h \
	magick/random.h \
	magick/registry.h \
	magick/render.h \
	magick/resize.h \
	magick/resource.h \
	magick/shear.h \
	magick/signature.h \
	magick/statistics.h \
	magick/symbols.h \
	magick/texture.h \
	magick/timer.h \
	magick/transform.h \
	magick/type.h \
	magick/utility.h \
	magick/version.h

MAGICK_NOINST_HDRS = \
	magick/alpha_composite.h \
	magick/animate.h \
	magick/bit_stream.h \
	magick/display.h \
	magick/floats.h \
	magick/locale_c.h \
	magick/map.h \
	magick/nt_base.h \
	magick/nt_feature.h \
	magick/omp_data_view.h \
	magick/prefetch.h \
	magick/random-private.h \
	magick/semaphore.h \
	magick/spinlock.h \
	magick/static.h \
	magick/studio.h \
	magick/tempfile.h \
	magick/unix_port.h \
	magick/widget.h \
	magick/xwindow.h


MAGICK_EXTRA_DIST = \
	magick/GraphicsMagick-config.in \
	$(MAGICK_MANS) \
	magick/GraphicsMagick.pc.in \
	magick/nt_base.c \
	magick/nt_feature.c

# Install magick_config_api.h as magick_config.h
MAGICK_INSTALL_DATA_LOCAL_TARGETS = magick-install-data-local
magick-install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(magickincdir)
	$(INSTALL_HEADER) magick/magick_config_api.h $(DESTDIR)$(magickincdir)/magick_config.h

# Uninstall magick_config.h
MAGICK_UNINSTALL_LOCAL_TARGETS = magick-uninstall-local
magick-uninstall-local:
	rm -f $(DESTDIR)$(magickincdir)/magick_config.h

