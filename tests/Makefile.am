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

AUTOMAKE_OPTIONS = 1.2 foreign

INCLUDES = -I../ -I$(top_srcdir)

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
	rm -f out.*

CHECK_FILES = \
	'LOGO:' \
	'../PerlMagick/t/input_p4.pbm'  \
	'../PerlMagick/t/input_p5.pgm' \
	'../PerlMagick/t/input_p6.ppm'

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

CHECK_FORMATS = \
	AVS \
	BMP \
	CMYK \
	DIB \
	FAX \
	FITS \
	GIF \
	GRAY \
	$(HDF) \
	ICB \
	$(JBIG) \
	$(JPEG) \
	MIFF \
	$(MNG) \
	MONO \
	MTV \
	P7 \
	PBM \
	PCD \
	PCDS \
	PCX \
	PGM \
	PICT \
	$(PNG) \
	PNM \
	PPM \
	PSD \
	$(PTIF) \
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
	XBM \
	XPM \
	$(XWD) \
	YUV

check-rwblob:
	status=0; \
	for format in $(CHECK_FORMATS) ; \
	do \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if DELEGATE_PATH=../delegates ./rwblob $$file $$format ; \
	    then \
	      : ; \
	    else \
	      status=1 ; \
	    fi \
	  done \
	done ; \
	exit $$status


check-rwfile:
	status=0; \
	for format in $(CHECK_FORMATS) ; \
	do \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if DELEGATE_PATH=../delegates ./rwfile $$file $$format ; \
	    then \
	      : ; \
	    else \
	      status=1 ; \
	    fi \
	  done \
	done ; \
	exit $$status

check: check-rwfile check-rwblob

