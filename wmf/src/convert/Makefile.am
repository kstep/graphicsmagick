CFLAGS = @CFLAGS@ @WMF_CFLAGS@

LDFLAGS = -L$(top_builddir)/src/.libs @WMF_LDFLAGS@ @LDFLAGS@

DEFS = @DEFS@ @WMF_DEFS@ \
	-DPACKAGE=\"@PACKAGE@\" \
	-DVERSION=\"@VERSION@\"

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_builddir)

if LIBWMF_OPT_X
WMF2X = wmf2x
else
WMF2X = 
endif

if LIBWMF_OPT_PLOT
WMF2PLOT = wmf2plot
else
WMF2PLOT = 
endif

bin_PROGRAMS = wmf2eps wmf2fig wmf2svg wmf2gd wmf2magick $(WMF2X) $(WMF2PLOT)

wmf2eps_LDADD = ../libwmf.la

wmf2fig_LDADD = ../libwmf.la

wmf2svg_LDADD = ../libwmf.la

wmf2gd_LDADD = ../libwmf.la

wmf2x_LDADD = ../libwmf.la

wmf2plot_LDADD = ../libwmf.la

wmf2magick_LDADD = ../libwmf.la
