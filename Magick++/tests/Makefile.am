#
# Makefile for Magick++ tests
#
# Copyright Bob Friesenhahn, 1999
#
AUTOMAKE_OPTIONS = 1.2 

INCLUDES = -I../lib

#
# Execute 'make check' to compile and run a test program
#
check_PROGRAMS = attributes manipulate

attributes_SOURCES = attributes.cpp
attributes_LDADD = ../lib/libMagick++.a

manipulate_SOURCES = manipulate.cpp
manipulate_LDADD = ../lib/libMagick++.a

EXTRA_DIST = Generic.ttf test_image.jpg test_image_anim.gif attributes.dsp manipulate.dsp

check:
	./attributes
	./manipulate
