#
# Makefile for ImageMagick read/write tests
#
# Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated
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

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I../ -I$(top_srcdir) -I../magick
# ../magick/config.h is needed.

#
# Execute 'make check' to compile and run a test program
#
check_PROGRAMS = rwblob rwfile

LDADD = ../magick/libMagick.la
LDFLAGS	= @LDFLAGS@

rwblob_SOURCES = rwblob.c

rwfile_SOURCES = rwfile.c

#EXTRA_DIST = 

clean-local:
	rm -f out_*

CHECK_FILES = \
	'LOGO:' \
	'$(top_srcdir)/PerlMagick/t/input_p4.pbm'  \
	'$(top_srcdir)/PerlMagick/t/input_p5.pgm' \
	'$(top_srcdir)/PerlMagick/t/input_p6.ppm'

if HasFPX
FPX=FPX
else
FPX=
endif

if HasHDF
HDF=HDF
else
HDF=
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

# Formats that currently don't work: DPX EPDF EPT $(HDF) $(FPX)
CHECK_BLOB_FORMATS = \
	AVS \
	BMP \
	CMYK \
	DIB \
	FAX \
	G3 \
	FITS \
	GIF \
	GIF87 \
	GRAY \
	$(JBIG) \
	$(JPEG) \
	$(JP2) \
	MIFF \
	$(MNG) \
	MONO \
	MTV \
	P7 \
	PAL \
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
	RGB \
	RGBA \
	SGI \
	SUN \
	TGA \
	$(TIFF) \
	UYVY \
	VDA \
	VICAR \
	VIFF \
	VST \
	WBMP \
	XBM \
	XPM \
	$(XWD) \
	YUV

# Formats that currently don't work: DPX EPDF $(HDF) $(FPX)
CHECK_FILE_FORMATS = \
	AVS \
	BMP \
	CMYK \
	DIB \
	EPSF \
	EPSI \
	EPI \
	EPS \
	EPT \
	FAX \
	FITS \
	GIF \
	GRAY \
	ICB \
	$(JBIG) \
	$(JPEG) \
	$(JP2) \
	MIFF \
	$(MNG) \
	MONO \
	MTV \
	PAL \
	P7 \
	PBM \
	PCD \
	PCDS \
	PCX \
	PGM \
	PICT \
	PICON \
	$(PNG) \
	PNM \
	PPM \
	PS \
	PSD \
	RAS \
	RGB \
	RGBA \
	SGI \
	SUN \
	TGA \
	$(TIFF) \
	UYVY \
	VDA \
	VICAR \
	VIFF \
	VST \
	WBMP \
	XBM \
	XPM \
	$(XWD) \
	YUV

check-rwblob:
	@status=0 ; \
	format='' ; \
	for format in $(CHECK_BLOB_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if MAGICK_DELEGATE_PATH=$(top_srcdir)/delegates ./rwblob $$file $$format ; \
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
	@status=0 ; \
	format='' ; \
	for format in $(CHECK_FILE_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if MAGICK_DELEGATE_PATH=$(top_srcdir)/delegates ./rwfile $$file $$format ; \
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

check: check-rwfile check-rwblob

