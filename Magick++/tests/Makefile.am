#
# Makefile for Magick++ tests
#
# Copyright Bob Friesenhahn, 1999, 2000, 2002
#
AUTOMAKE_OPTIONS = 1.7 foreign

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)/Magick++/lib -I$(top_srcdir)

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
	colorHistogram \
	exceptions \
	montageImages \
	morphImages \
	readWriteBlob \
	readWriteImages

LDADD = ../lib/libGraphicsMagick++.la

appendImages_SOURCES	= appendImages.cpp
attributes_SOURCES	= attributes.cpp
averageImages_SOURCES	= averageImages.cpp
coalesceImages_SOURCES	= coalesceImages.cpp
coderInfo_SOURCES	= coderInfo.cpp
color_SOURCES		= color.cpp
colorHistogram_SOURCES  = colorHistogram.cpp
exceptions_SOURCES	= exceptions.cpp
montageImages_SOURCES	= montageImages.cpp
morphImages_SOURCES	= morphImages.cpp
readWriteBlob_SOURCES	= readWriteBlob.cpp
readWriteImages_SOURCES = readWriteImages.cpp

EXTRA_DIST = \
	test_image.miff test_image_anim.miff

CLEANFILES = colorHistogram.txt testmagick_anim_out.miff

if WIN32_NATIVE_BUILD
SRCDIR='$(shell @WinPathScript@ $(srcdir)/)'
else
SRCDIR='$(srcdir)/'
endif

# FIXME: MinGW needs Windows path form of srcdir here (or pass as command argument)
check:
	-env SRCDIR=$(SRCDIR) ./exceptions
	-env SRCDIR=$(SRCDIR) ./attributes
	-env SRCDIR=$(SRCDIR) ./appendImages
	-env SRCDIR=$(SRCDIR) ./averageImages
	-env SRCDIR=$(SRCDIR) ./coalesceImages
	-env SRCDIR=$(SRCDIR) ./coderInfo
	-env SRCDIR=$(SRCDIR) ./color
	-env SRCDIR=$(SRCDIR) ./colorHistogram > colorHistogram.txt
	-env SRCDIR=$(SRCDIR) ./montageImages
	-env SRCDIR=$(SRCDIR) ./morphImages
	-env SRCDIR=$(SRCDIR) ./readWriteBlob
	-env SRCDIR=$(SRCDIR) ./readWriteImages

