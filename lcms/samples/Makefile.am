#
# Makefile for building lcms sample programs
# Written by Bob Friesenhahn, June 2003
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

AM_CPPFLAGS =  -I$(top_builddir)/include -I$(top_srcdir)/include

bin_PROGRAMS = icctrans wtpt icc2ps icclink

icctrans_LDADD = $(top_builddir)/src/liblcms.la 
icctrans_LDFLAGS = @LDFLAGS@
icctrans_SOURCES = icctrans.c xgetopt.c

wtpt_LDADD = $(top_builddir)/src/liblcms.la 
wtpt_LDFLAGS = @LDFLAGS@
wtpt_SOURCES = wtpt.c xgetopt.c

icc2ps_LDADD = $(top_builddir)/src/liblcms.la 
icc2ps_LDFLAGS = @LDFLAGS@
icc2ps_SOURCES = icc2ps.c xgetopt.c

icclink_LDADD = $(top_builddir)/src/liblcms.la 
icclink_LDFLAGS = @LDFLAGS@
icclink_SOURCES = icclink.c xgetopt.c

# EXTRA_DIST =

