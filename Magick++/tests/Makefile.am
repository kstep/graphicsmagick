#
# Makefile for Magick++ tests
#
# Copyright Bob Friesenhahn, 1999, 2000
#
AUTOMAKE_OPTIONS = 1.4

INCLUDES = -I$(srcdir)/../lib -I../.. -I$(top_srcdir)

#
# Execute 'make check' to compile and run a test program
#
check_PROGRAMS = color attributes appendImages averageImages coalesceImages \
	montageImages morphImages readWriteImages readWriteBlob

LDADD = ../lib/libMagick++.la ../../magick/libMagick.la

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
	Generic.ttf test_image.miff test_image_anim.miff

CLEANFILES = testmagick_anim_out.miff

check:
	env srcdir=$(srcdir) ./color
	env srcdir=$(srcdir) ./attributes
	env srcdir=$(srcdir) ./appendImages
	env srcdir=$(srcdir) ./averageImages
	env srcdir=$(srcdir) ./coalesceImages
	env srcdir=$(srcdir) ./montageImages
	env srcdir=$(srcdir) ./morphImages
	env srcdir=$(srcdir) ./readWriteImages
	env srcdir=$(srcdir) ./readWriteBlob

