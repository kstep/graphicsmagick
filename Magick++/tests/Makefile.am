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
check_PROGRAMS = color attributes appendImages averageImages coalesceImages \
	montageImages morphImages readWriteImages readWriteBlob

LDADD = ../lib/libMagick++.la

appendImages_SOURCES	= appendImages.cpp
attributes_SOURCES	= attributes.cpp
averageImages_SOURCES	= averageImages.cpp
coalesceImages_SOURCES	= coalesceImages.cpp
color_SOURCES		= color.cpp
montageImages_SOURCES	= montageImages.cpp
morphImages_SOURCES	= morphImages.cpp
readWriteImages_SOURCES = readWriteImages.cpp
readWriteBlob_SOURCES	= readWriteBlob.cpp

EXTRA_DIST = \
	Generic.ttf test_image.miff test_image_anim.miff \
	appendImages.dsp attributes.dsp averageImages.dsp coalesceImages.dsp \
	color.dsp montageImages.dsp morphImages.dsp readWriteImages.dsp \
	readWriteBlob.dsp

check:
	./color
	./attributes
	./appendImages
	./averageImages
	./coalesceImages
	./montageImages
	./morphImages
	./readWriteImages
	./readWriteBlob

