SUBDIRS = extra api ipa . convert

CFLAGS = @CFLAGS@ @WMF_CFLAGS@

LDFLAGS = @WMF_LDFLAGS@ @LDFLAGS@

DEFS = @DEFS@ @WMF_DEFS@ \
	-DPACKAGE=\"@PACKAGE@\" \
	-DVERSION=\"@VERSION@\"

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_builddir)

lib_LTLIBRARIES = libwmf.la

libwmf_la_SOURCES = player.c

if LIBWMF_OPT_TRIO
LIBTRIO = extra/trio/libtrio.la
else
LIBTRIO = 
endif

if LIBWMF_OPT_SYS_GD
LIBGD = 
else
LIBGD = extra/gd/libgd.la
endif

libwmf_la_LIBADD = api/libapi.la ipa/libipa.la $(LIBGD) $(LIBTRIO)

libwmf_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-release $(LT_RELEASE) -export-dynamic

PLAYERHEADERS = \
	player/clip.h \
	player/color.h \
	player/coord.h \
	player/dc.h \
	player/defaults.h \
	player/meta.h \
	player/record.h \
	player/region.h

EXTRA_DIST = wmfdefs.h player.h $(PLAYERHEADERS)
