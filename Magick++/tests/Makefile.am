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
check_PROGRAMS =  \
	appendImages \
	attributes \
	averageImages \
	coalesceImages \
	coderInfo \
	color \
	montageImages \
	morphImages \
	readWriteBlob \
	readWriteImages

LDADD = ../lib/libMagick++.la ../../magick/libMagick.la

appendImages_SOURCES	= appendImages.cpp
attributes_SOURCES	= attributes.cpp
averageImages_SOURCES	= averageImages.cpp
coalesceImages_SOURCES	= coalesceImages.cpp
coderInfo_SOURCES	= coderInfo.cpp
color_SOURCES		= color.cpp
montageImages_SOURCES	= montageImages.cpp
morphImages_SOURCES	= morphImages.cpp
readWriteBlob_SOURCES	= readWriteBlob.cpp
readWriteImages_SOURCES = readWriteImages.cpp

EXTRA_DIST = \
	Generic.ttf test_image.miff test_image_anim.miff

CLEANFILES = testmagick_anim_out.miff

check:
	env srcdir=$(srcdir) ./appendImages
	env srcdir=$(srcdir) ./attributes
	env srcdir=$(srcdir) ./averageImages
	env srcdir=$(srcdir) ./coalesceImages
	env srcdir=$(srcdir) ./coderInfo
	env srcdir=$(srcdir) ./color
	env srcdir=$(srcdir) ./montageImages
	env srcdir=$(srcdir) ./morphImages
	env srcdir=$(srcdir) ./readWriteBlob
	env srcdir=$(srcdir) ./readWriteImages

