CFLAGS = @CFLAGS@ @WMF_CFLAGS@

LDFLAGS = @LDFLAGS@

DEFS = @DEFS@ @WMF_DEFS@ \
	-DPACKAGE=\"@PACKAGE@\" \
	-DVERSION=\"@VERSION@\" \
	-DWMF_FONTDIR=\"@WMF_FONTDIR@\" \
	-DWMF_GS_FONTDIR=\"@WMF_GS_FONTDIR@\"

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_builddir)

noinst_LTLIBRARIES = libapi.la

libapi_la_SOURCES = api.c bbuf.c meta.c stream.c

libapi_la_LDFLAGS = 

EXTRA_DIST = api.h bbuf.h stream.h
