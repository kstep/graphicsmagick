# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for ImageMagick tests invoked via the C API
#

AUTOMAKE_OPTIONS = 1.8 foreign

INCLUDES = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/magick
#INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/magick
# ../magick/config.h is needed.

RUNENV = $(SHELL) $(top_builddir)/rungm.sh

#
# Execute 'make check' to compile and run a test program
#
check_PROGRAMS = constitute drawtest maptest rwblob rwfile

LDADD = ../magick/libGraphicsMagick.la
LDFLAGS	= @LDFLAGS@

constitute_SOURCES = constitute.c

maptest_SOURCES = maptest.c

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

# Optional compression formats.
if HasLZW
LZWCOMPRESS=lzw
else
LZWCOMPRESS=
endif

if HasZLIB
ZIPCOMPRESS=zip
else
ZIPCOMPRESS=
endif

if HasBZLIB
BZIPCOMPRESS=bzip
else
BZIPCOMPRESS=
endif

RLECOMPRESS=rle

NOCOMPRESS=none

# Blob formats to test which don't require that size be specified
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

# File formats to test which don't require that size be specified
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

CHECK_CONSTITUTE_MAPS = \
	RGB \
	rgb \
	rgba \
	cmy \
	cmyk \
	bGr \
	Gbr \
	i

CHECK_CONSTITUTE_STORAGETYPES = \
	char \
	short \
	integer \
	long \
	float \
	double

check-constitute: constitute$(EXEEXT)
	@status=0 ; \
	storage_type='' ; \
	for storage_type in $(CHECK_CONSTITUTE_STORAGETYPES) ; \
	do \
	  storage_type_failed=0 ; \
	  for map in $(CHECK_CONSTITUTE_MAPS) ; \
	  do \
	    if $(RUNENV) ./constitute -storagetype $$storage_type $(srcdir)/input_truecolor.miff $$map ; \
	    then \
	      : ; \
	    else \
	      status=1 ; \
	      storage_type_failed=1 ; \
	    fi \
	  done ; \
	  if test $$storage_type_failed -gt 0 ; \
	  then \
	    failed="$$failed$$storage_type:$$map " ; \
	  fi ; \
	done ; \
	if test -n "$$failed" ; \
	then \
	  echo "Failed constitute for tests: $$failed" ; \
	fi ; \
	exit $$status

check-rwblob: rwblob$(EXEEXT)
	-@status=0 ; \
	format='' ; \
	for format in $(CHECK_BLOB_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if $(RUNENV) ./rwblob $$file $$format ; \
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

check-rwfile: rwfile$(EXEEXT)
	-@status=0 ; \
	format='' ; \
	for format in $(CHECK_FILE_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in  $(CHECK_FILES) ; \
	  do \
	    if $(RUNENV) ./rwfile $$file $$format ; \
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

check-rwblob-sized: rwblob$(EXEEXT)
	-@status=0 ; \
	format='' ; \
	for format in $(CHECK_SIZED_BLOB_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if $(RUNENV) ./rwblob -size 70x46 $$file $$format ; \
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

check-rwfile-sized: rwfile$(EXEEXT)
	-@status=0 ; \
	format='' ; \
	for format in $(CHECK_SIZED_FILE_FORMATS) ; \
	do \
	  format_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if $(RUNENV) ./rwfile -size 70x46 $$file $$format ; \
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

check-drawtest: drawtest$(EXEEXT)
	$(RUNENV) ./drawtest drawtest_out.miff
	$(RUNENV) ./drawtest rawtest_out.mvg

CHECK_MIFF_FILE_COMPRESS= \
	$(NOCOMPRESS) \
	$(LZWCOMPRESS) \
	$(BZIPCOMPRESS) \
	$(RLECOMPRESS)

check-miff: rwfile$(EXEEXT)
	@status=0 ; \
	compress='' ; \
	for compress in $(CHECK_MIFF_FILE_COMPRESS) ; \
	do \
	  test_failed=0 ; \
	  for file in $(CHECK_FILES) ; \
	  do \
	    if $(RUNENV) ./rwfile -compress $$compress $$file MIFF ; \
	    then \
	      : ; \
	    else \
	      status=1 ; \
	      test_failed=1 ; \
	    fi \
	  done ; \
	  if test $$test_failed -gt 0 ; \
	  then \
	    failed="$$failed$$compress " ; \
	  fi ; \
	done ; \
	if test -n "$$failed" ; \
	then \
	  echo "Failed rwfile MIFF for compression: $$failed" ; \
	fi ; \
	exit $$status

check:	check-constitute \
	check-drawtest \
	check-rwfile \
	check-miff \
	check-rwblob \
	check-rwblob-sized \
	check-rwfile-sized 
