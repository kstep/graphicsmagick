#
# Makefile for Magick++ demo programs
#
# Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
#
AUTOMAKE_OPTIONS = 1.6

INCLUDES = -I$(srcdir)/../lib -I../.. -I$(top_srcdir)

check_PROGRAMS = button demo detrans flip gravity piddle shapes zoom

LDADD = ../lib/libMagick++.la ../../magick/libMagick.la

button_SOURCES  = button.cpp
demo_SOURCES    = demo.cpp
detrans_SOURCES = detrans.cpp
flip_SOURCES    = flip.cpp
gravity_SOURCES = gravity.cpp
piddle_SOURCES  = piddle.cpp
shapes_SOURCES  = shapes.cpp
zoom_SOURCES	= zoom.cpp

EXTRA_DIST = model.miff smile.miff smile_anim.miff tile.miff

CLEANFILES = *_out.miff

ZOOM_FILTERS = point box triangle hermite hanning hamming blackman gaussian \
	       quadratic cubic catrom mitchell lanczos bessel sinc
ZOOM_GEOMETRY = 600x600

check:
	env srcdir=$(srcdir) ./button
	env srcdir=$(srcdir) ./demo
	env srcdir=$(srcdir) ./flip
	env srcdir=$(srcdir) ./gravity
	env srcdir=$(srcdir) ./piddle
	env srcdir=$(srcdir) ./shapes
	for filter in $(ZOOM_FILTERS) ; do  \
	  echo "$$filter ..." ; \
	  env srcdir=$(srcdir) ./zoom $$filter $(ZOOM_GEOMETRY) $(srcdir)/model.miff zoom_$${filter}_out.miff ; \
	done

