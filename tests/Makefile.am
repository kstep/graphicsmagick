#
# Makefile for Magick++ tests
#
# Copyright Bob Friesenhahn, 1999
#
AUTOMAKE_OPTIONS = 1.2 foreign

INCLUDES = -I../ -I$(top_srcdir)

#
# Execute 'make check' to compile and run a test program
#
check_PROGRAMS = rwblob rwfile

LDADD = ../magick/libMagick.la


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

CHECK_FORMATS = \
	AVS \
	BMP \
	CMYK \
	DIB \
	FAX \
	FITS \
	GIF \
	GRAY \
	HDF \
	ICB \
	JBIG \
	JPEG \
	MIFF \
	MNG \
	MONO \
	MTV \
	P7 \
	PBM \
	PCD \
	PCDS \
	PCX \
	PGM \
	PICT \
	PNG \
	PNM \
	PPM \
	PSD \
	PTIF \
	RAS \
	RGB \
	RGBA \
	SGI \
	SUN \
	TGA \
	TIFF \
	UYVY \
	VDA \
	VICAR \
	VIFF \
	VST \
	XBM \
	XPM \
	XWD \
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

