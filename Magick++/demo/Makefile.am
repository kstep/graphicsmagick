#
# Makefile for Magick++ demo programs
#
# Copyright Bob Friesenhahn, 1999, 2000
#
AUTOMAKE_OPTIONS = 1.4

INCLUDES = -I$(srcdir)/../lib -I../.. -I$(top_srcdir)

check_PROGRAMS = button demo detrans flip gravity piddle shapes

LDADD = ../lib/libMagick++.la ../../magick/libMagick.la

button_SOURCES  = button.cpp
demo_SOURCES    = demo.cpp
detrans_SOURCES = detrans.cpp
flip_SOURCES    = flip.cpp
gravity_SOURCES = gravity.cpp
piddle_SOURCES  = piddle.cpp
shapes_SOURCES  = shapes.cpp

EXTRA_DIST = Generic.ttf model.miff smile.miff smile_anim.miff tile.miff

CLEANFILES = *_out.miff

check:
	env srcdir=$(srcdir) ./button
	env srcdir=$(srcdir) ./demo
	env srcdir=$(srcdir) ./flip
	env srcdir=$(srcdir) ./gravity
	env srcdir=$(srcdir) ./piddle
	env srcdir=$(srcdir) ./shapes

