#
# Makefile for ImageMagick tests invoked via the C API
#
# Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated
# to making software imaging solutions freely available.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files ("ImageMagick"),
# to deal in ImageMagick without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense
# and/or sell copies of ImageMagick, and to permit persons to whom the
# ImageMagick is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of ImageMagick.
#
# The software is provided "as is", without warranty of any kind, express or
# implied, including but not limited to the warranties of merchantability,
# fitness for a particular purpose and noninfringement.  In no event shall
# ImageMagick Studio be liable for any claim, damages or other liability,
# whether in an action of contract, tort or otherwise, arising from, out of
# or in connection with ImageMagick or the use or other dealings in
# ImageMagick.
#
# Except as contained in this notice, the name of the ImageMagick Studio
# shall not be used in advertising or otherwise to promote the sale, use or
# other dealings in ImageMagick without prior written authorization from the
# ImageMagick Studio.
#

AUTOMAKE_OPTIONS = 1.6 foreign

INCLUDES = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/magick
#INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/magick
# ../magick/config.h is needed.

#
# Execute 'make check' to compile and run a test program
#
check_PROGRAMS = drawtest rwblob rwfile

LDADD = ../magick/libGraphicsMagick.la
LDFLAGS	= @LDFLAGS@

rwblob_SOURCES = rwblob.c

rwfile_SOURCES = rwfile.c

drawtest_SOURCES = drawtest.c

EXTRA_DIST = input_256c.miff input_bilevel.miff input_gray.miff \
	input_truecolor.miff

clean-local:
	rm -f *out*.*

CHECK_FILES = \
	'$(srcdir)/input_bilevel.miff' \
	'$(srcdir)/input_gray.miff'  \
	'$(srcdir)/input_256c.miff' \
	'$(srcdir)/input_truecolor.miff'

if HasFPX
FPX=FPX
else
FPX=
endif

if HasJBIG
JBIG=JBIG
else
JBIG=
endif

if HasJPEG
JPEG=JPEG
else
JPEG=
endif

if HasJP2
JP2=JP2
else
JP2=
endif

if HasPNG
MNG=MNG
PNG=PNG
else
MNG=
PNG=
endif

if HasTIFF
TIFF=TIFF
PTIF=PTIF
else
TIFF=
PTIF=
endif

if HasX11
XWD=XWD
else
XWD=
endif

BILEVEL_FORMATS = \
	$(JBIG) \
	BMP \
	DIB \
	FAX \
	GIF \
	MONO \
	OTB \
	SUN \
	MIFF \
	WBMP \
	XBM

COLORMAPPED_FORMATS = \
	ART \
	BMP \
	DIB \
	GIF \
	SUN \
	MIFF \
	$(MNG) \
	PALM \
	PDB \
	PCX \
	PICT \
	$(PNG) \
	$(TIFF) \
	VIFF \
	XPM \
	$(XWD)

GRAYSCALE_FORMATS = \
	FITS \
	MIFF \
	$(MNG) \
	$(JPEG) \
	$(JP2) \
	PDB \
	$(PNG)

TRUECOLOR_FORMATS = \
	AVS \
	BMP \
	DCX \
	DIB \
	$(FPX) \
	$(JPEG) \
	$(JP2) \
	$(MNG) \
	PCD \
	PCDS \
	$(PNG) \
	SUN \
	MIFF \
	MTV \
	P7 \
	PCX \
	PICT \
	PSD \
	SGI \
	TGA \
	$(TIFF) \
	$(XWD)

# Blob formats to test that don't require that size be specified
CHECK_BLOB_FORMATS = \
	AVS \
	BMP \
	DIB \
	DPX \
	EPDF \
	EPT \
	FAX \
	$(FPX) \
	G3 \
	FITS \
	GIF \
	GIF87 \
	$(JBIG) \
	$(JPEG) \
	$(JP2) \
	MIFF \
	$(MNG) \
	MTV \
	P7 \
	PBM \
	PCD \
	PCDS \
	PCX \
	PGM \
	PICON \
	PICT \
	$(PNG) \
	PNM \
	PPM \
	PSD \
	RAS \
	SGI \
	SUN \
	TGA \
	$(TIFF) \
	VDA \
	VICAR \
	VIFF \
	VST \
	WBMP \
	XBM \
	XPM \
	$(XWD)

# File formats to test that don't require that size be specified
CHECK_FILE_FORMATS = \
	AVS \
	BMP \
	DIB \
	DPX \
	EPDF \
	EPSF \
	EPSI \
	EPI \
	EPS \
	EPT \
	FAX \
	FITS \
	$(FPX) \
	GIF \
	GRAY \
	ICB \
	$(JBIG) \
	$(JPEG) \
	$(JP2) \
	MIFF \
	$(MNG) \
	MTV \
	P7 \
	PBM \
	PCD \
	PCDS \
	PCX \
	PGM \
	PICON \
	PICT \
	$(PNG) \
	PNM \
	PPM \
	PS \
	PSD \
	RAS \
	SGI \
	SUN \
	TGA \
	$(TIFF) \
	VDA \
	VICAR \
	VIFF \
	VST \
	WBMP \
	XBM \
	XPM \
	$(XWD)

# Blob formats to test where size must be specified
CHECK_SIZED_BLOB_FORMATS = \
	CMYK \
	GRAY \
	MONO \
	PAL \
	RGB \
	RGBA \
	UYVY \
	YUV

# File formats to test where size must be specified
CHECK_SIZED_FILE_FORMATS = \
	CMYK \
	GRAY \
	MONO \
	PAL \
	RGB \
	RGBA \
	UYVY \
	YUV

check-rwblob:
	-@status=0 ; \
	format='' ; \
	for format in $(CHECK_BLOB_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if env MAGICK_MODULE_PATH=`pwd`/../coders/.libs ./rwblob $$file $$format ; \
	    then \
	      : ; \
	    else \
	      status=1 ; \
	      format_failed=1 ; \
	    fi \
	  done ; \
	  if test $$format_failed -gt 0 ; \
	  then \
	    failed="$$failed$$format " ; \
	  fi ; \
	done ; \
	if test -n "$$failed" ; \
	then \
	  echo "Failed rwblob for formats: $$failed" ; \
	fi ; \
	exit $$status

check-rwfile:
	-@status=0 ; \
	format='' ; \
	for format in $(CHECK_FILE_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in  $(CHECK_FILES) ; \
	  do \
	    if env MAGICK_MODULE_PATH=`pwd`/../coders/.libs ./rwfile $$file $$format ; \
	    then \
	      : ; \
	    else \
	      status=1 ; \
	      format_failed=1 ; \
	    fi \
	  done ; \
	  if test $$format_failed -gt 0 ; \
	  then \
	    failed="$$failed$$format " ; \
	  fi ; \
	done ; \
	if test -n "$$failed" ; \
	then \
	  echo "Failed rwfile for formats: $$failed" ; \
	fi ; \
	exit $$status

check-rwblob-sized:
	-@status=0 ; \
	format='' ; \
	for format in $(CHECK_SIZED_BLOB_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if env MAGICK_MODULE_PATH=`pwd`/../coders/.libs ./rwblob -size 70x46 $$file $$format ; \
	    then \
	      : ; \
	    else \
	      status=1 ; \
	      format_failed=1 ; \
	    fi \
	  done ; \
	  if test $$format_failed -gt 0 ; \
	  then \
	    failed="$$failed$$format " ; \
	  fi ; \
	done ; \
	if test -n "$$failed" ; \
	then \
	  echo "Failed rwblob for formats: $$failed" ; \
	fi ; \
	exit $$status

check-rwfile-sized:
	-@status=0 ; \
	format='' ; \
	for format in $(CHECK_SIZED_FILE_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if env MAGICK_MODULE_PATH=`pwd`/../coders/.libs ./rwfile -size 70x46 $$file $$format ; \
	    then \
	      : ; \
	    else \
	      status=1 ; \
	      format_failed=1 ; \
	    fi \
	  done ; \
	  if test $$format_failed -gt 0 ; \
	  then \
	    failed="$$failed$$format " ; \
	  fi ; \
	done ; \
	if test -n "$$failed" ; \
	then \
	  echo "Failed rwfile for formats: $$failed" ; \
	fi ; \
	exit $$status

check-drawtest:
	-./drawtest drawtest_out.miff
	-./drawtest drawtest_out.mvg
	-./drawtest drawtest_out.svg

check: check-drawtest check-rwfile check-rwblob check-rwblob-sized check-rwfile-sized 
