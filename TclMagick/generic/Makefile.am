# Makefile for creating libTclMagick and libTkMagick

# David N. Welton <davidw@dedasys.com>, 2004, with many thanks to Bob
# Friesenhahn for all the help.

# $(TCL_CFLAGS_OPTIMIZE) $(TCL_CFLAGS_WARNING) $(TCL_EXTRA_CFLAGS) 

AM_CFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@

lib_LTLIBRARIES = libTclMagick.la

libTclMagick_la_LIBADD = @TCL_LIBS@
libTclMagick_la_SOURCES = TclMagick.c
#pkg_LTLIBRARIES = libTclMagick.la libTkMagick.la

#pkg_la_LDFLAGS = $(TCL_LIB_SPEC) $(TCL_LIBS)

#_lcms_la_LIBADD = $(top_builddir)/src/liblcms.la -lpython$(PYTHON_VERSION)

#tclmagick_la_SOURCES = ../generic/TclMagick.c
#tkmagick_la_SOURCES = ../generic/TkMagick.c

#tclmagick_la_CFLAGS = $(AM_CFLAGS) -I$(TK_INC_DIR)

#_lcms_la_DEPENDENCIES = lcms.i

#BUILT_SOURCES = $(LCMSBUILTSOURCES)

#EXTRA_DIST = lcms.i testbed

#pkg_DATA = $(LCMSWRAPPER)

