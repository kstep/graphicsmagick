#
# Makefile for Magick++ demo programs
#
# Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002, 2003
#
AUTOMAKE_OPTIONS = 1.7 foreign

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)/Magick++/lib -I$(top_srcdir)

check_PROGRAMS = analyze button demo detrans flip gravity piddle shapes zoom

LDADD = ../lib/libGraphicsMagick++.la

analyze_SOURCES = analyze.cpp
button_SOURCES  = button.cpp
demo_SOURCES    = demo.cpp
detrans_SOURCES = detrans.cpp
flip_SOURCES    = flip.cpp
gravity_SOURCES = gravity.cpp
piddle_SOURCES  = piddle.cpp
shapes_SOURCES  = shapes.cpp
zoom_SOURCES	= zoom.cpp

EXTRA_DIST = model.miff smile.miff smile_anim.miff tile.miff

CLEANFILES = *_out.* ir.out # SunWS_cache

ZOOM_FILTERS = bessel blackman box catrom cubic gaussian hamming hanning hermite \
 lanczos mitchell point quadratic sample scale sinc triangle
ZOOM_GEOMETRY = 600x600

if WIN32_NATIVE_BUILD
SRCDIR='$(shell @WinPathScript@ $(srcdir)/)'
else
SRCDIR='$(srcdir)/'
endif

# FIXME: MinGW needs Windows path form of srcdir here (or pass as command argument)
check:
	-env SRCDIR=$(SRCDIR) ./analyze '$(srcdir)/model.miff'
	-env SRCDIR=$(SRCDIR) ./button
	-env SRCDIR=$(SRCDIR) ./demo
	-env SRCDIR=$(SRCDIR) ./flip
	-env SRCDIR=$(SRCDIR) ./gravity
	-env SRCDIR=$(SRCDIR) ./piddle
	-env SRCDIR=$(SRCDIR) ./shapes
	-for filter in $(ZOOM_FILTERS) ; do  \
	  echo "$$filter ..." ; \
	  env SRCDIR=$(SRCDIR) ./zoom -filter $$filter -geometry $(ZOOM_GEOMETRY) '$(srcdir)/model.miff' zoom_$${filter}_out.miff ; \
	done

