#
# Makefile for Magick++ tests
#
# Copyright Bob Friesenhahn, 1999, 2000, 2002
#
AUTOMAKE_OPTIONS = 1.8 foreign

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

CLEANFILES = colorHistogram.txt testmagick_anim_out.miff ir.out # SunWS_cache

if WIN32_NATIVE_BUILD
SRCDIR='$(shell @WinPathScript@ $(srcdir)/)'
else
SRCDIR='$(srcdir)/'
endif

RUNENV = $(SHELL) $(top_builddir)/rungm.sh

# FIXME: MinGW needs Windows path form of srcdir here (or pass as command argument)
check:
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./exceptions
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./attributes
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./appendImages
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./averageImages
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./coalesceImages
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./coderInfo
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./color
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./colorHistogram > colorHistogram.txt
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./montageImages
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./morphImages
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./readWriteBlob
	-@$(RUNENV) SRCDIR=$(SRCDIR) ./readWriteImages

