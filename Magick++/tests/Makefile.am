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

check:
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./exceptions
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./attributes
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./appendImages
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./averageImages
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./coalesceImages
	-@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./coderInfo # Test fails for moby build since no .so file exists.
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./color
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./colorHistogram > colorHistogram.txt
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./montageImages
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./morphImages
	-@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./readWriteBlob # HP compiler fails on this one due to libxml2!
	@$(RUNENV) SRCDIR=$(SRCDIR) $(MEMCHECK) ./readWriteImages

