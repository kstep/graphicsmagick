# Copyright (C) 2004-2011 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for GraphicsMagick tests invoked via the C API
#

TESTS_CHECK_PGRMS = \
	tests/bitstream \
        tests/constitute \
        tests/drawtest \
        tests/maptest \
        tests/rwblob \
        tests/rwfile

tests_bitstream_SOURCES = tests/bitstream.c
tests_bitstream_LDADD = $(LIBMAGICK)
tests_bitstream_CPPFLAGS = $(AM_CPPFLAGS)

tests_constitute_SOURCES = tests/constitute.c
tests_constitute_CPPFLAGS = $(AM_CPPFLAGS)
tests_constitute_LDADD = $(LIBMAGICK)

tests_maptest_SOURCES = tests/maptest.c
tests_maptest_CPPFLAGS = $(AM_CPPFLAGS)
tests_maptest_LDADD = $(LIBMAGICK)

tests_rwblob_SOURCES = tests/rwblob.c
tests_rwblob_CPPFLAGS = $(AM_CPPFLAGS)
tests_rwblob_LDADD = $(LIBMAGICK)

tests_rwfile_SOURCES = tests/rwfile.c
tests_rwfile_CPPFLAGS = $(AM_CPPFLAGS)
tests_rwfile_LDADD = $(LIBMAGICK)

tests_drawtest_SOURCES = tests/drawtest.c
tests_drawtest_CPPFLAGS = $(AM_CPPFLAGS)
tests_drawtest_LDADD = $(LIBMAGICK)

if !HasFPX
TESTS_FPX_XFAIL_TESTS = \
	tests/rwblob_FPX_bilevel.sh \
	tests/rwblob_FPX_gray.sh \
	tests/rwblob_FPX_pallette.sh \
	tests/rwblob_FPX_truecolor.sh \
	tests/rwfile_FPX_bilevel.sh \
	tests/rwfile_FPX_gray.sh \
	tests/rwfile_FPX_pallette.sh \
	tests/rwfile_FPX_truecolor.sh \
	tests/rwfile_FPX_truecolor10.sh \
	tests/rwfile_FPX_truecolor12.sh \
	tests/rwfile_FPX_truecolor16.sh
endif

if !HasJBIG
TESTS_JBIG_XFAIL_TESTS = \
	tests/rwblob_JBIG_bilevel.sh \
	tests/rwblob_JBIG_gray.sh \
	tests/rwblob_JBIG_pallette.sh \
	tests/rwblob_JBIG_truecolor.sh \
	tests/rwfile_JBIG_bilevel.sh \
	tests/rwfile_JBIG_gray.sh \
	tests/rwfile_JBIG_pallette.sh \
	tests/rwfile_JBIG_truecolor.sh \
	tests/rwfile_JBIG_truecolor10.sh \
	tests/rwfile_JBIG_truecolor12.sh \
	tests/rwfile_JBIG_truecolor16.sh
endif

if !HasJPEG
TESTS_JPEG_XFAIL_TESTS = \
	tests/rwblob_JPEG_bilevel.sh \
	tests/rwblob_JPEG_gray.sh \
	tests/rwblob_JPEG_pallette.sh \
	tests/rwblob_JPEG_truecolor.sh \
	tests/rwfile_JPEG_bilevel.sh \
	tests/rwfile_JPEG_gray.sh \
	tests/rwfile_JPEG_pallette.sh \
	tests/rwfile_JPEG_truecolor.sh \
	tests/rwfile_JPEG_truecolor10.sh \
	tests/rwfile_JPEG_truecolor12.sh \
	tests/rwfile_JPEG_truecolor16.sh
endif

if !HasJP2
TESTS_JP2_XFAIL_TESTS = \
	tests/rwblob_JP2_bilevel.sh \
	tests/rwblob_JP2_gray.sh \
	tests/rwblob_JP2_pallette.sh \
	tests/rwblob_JP2_truecolor.sh \
	tests/rwfile_JP2_bilevel.sh \
	tests/rwfile_JP2_gray.sh \
	tests/rwfile_JP2_pallette.sh \
	tests/rwfile_JP2_truecolor.sh \
	tests/rwfile_JP2_truecolor10.sh \
	tests/rwfile_JP2_truecolor12.sh \
	tests/rwfile_JP2_truecolor16.sh
endif

if !WIN32_NATIVE_BUILD
# For a native WWIN32 build, we assume that the Ghostcript library is
# dynamically loaded.  We don't have a good way to know if the
# Ghostscript library is available so we will assume that Postscript
# is supported, and allow obvious test errors to occur if it is not.
if !HasPSDelegate
TESTS_PS_XFAIL_TESTS = \
	tests/rwblob_EPDF_bilevel.sh \
	tests/rwblob_EPDF_gray.sh \
	tests/rwblob_EPDF_pallette.sh \
	tests/rwblob_EPDF_truecolor.sh \
	tests/rwblob_EPT_bilevel.sh \
	tests/rwblob_EPT_gray.sh \
	tests/rwblob_EPT_pallette.sh \
	tests/rwblob_EPT_truecolor.sh \
	tests/rwfile_EPDF_bilevel.sh \
	tests/rwfile_EPDF_gray.sh \
	tests/rwfile_EPDF_pallette.sh \
	tests/rwfile_EPDF_truecolor.sh \
	tests/rwfile_EPDF_truecolor10.sh \
	tests/rwfile_EPDF_truecolor12.sh \
	tests/rwfile_EPDF_truecolor16.sh \
	tests/rwfile_EPI_bilevel.sh \
	tests/rwfile_EPI_gray.sh \
	tests/rwfile_EPI_pallette.sh \
	tests/rwfile_EPI_truecolor.sh \
	tests/rwfile_EPI_truecolor10.sh \
	tests/rwfile_EPI_truecolor12.sh \
	tests/rwfile_EPI_truecolor16.sh \
	tests/rwfile_EPSF_bilevel.sh \
	tests/rwfile_EPSF_gray.sh \
	tests/rwfile_EPSF_pallette.sh \
	tests/rwfile_EPSF_truecolor.sh \
	tests/rwfile_EPSF_truecolor10.sh \
	tests/rwfile_EPSF_truecolor12.sh \
	tests/rwfile_EPSF_truecolor16.sh \
	tests/rwfile_EPSI_bilevel.sh \
	tests/rwfile_EPSI_gray.sh \
	tests/rwfile_EPSI_pallette.sh \
	tests/rwfile_EPSI_truecolor.sh \
	tests/rwfile_EPSI_truecolor10.sh \
	tests/rwfile_EPSI_truecolor12.sh \
	tests/rwfile_EPSI_truecolor16.sh \
	tests/rwfile_EPS_bilevel.sh \
	tests/rwfile_EPS_gray.sh \
	tests/rwfile_EPS_pallette.sh \
	tests/rwfile_EPS_truecolor.sh \
	tests/rwfile_EPS_truecolor10.sh \
	tests/rwfile_EPS_truecolor12.sh \
	tests/rwfile_EPS_truecolor16.sh \
	tests/rwfile_EPT_bilevel.sh \
	tests/rwfile_EPT_gray.sh \
	tests/rwfile_EPT_pallette.sh \
	tests/rwfile_EPT_truecolor.sh \
	tests/rwfile_EPT_truecolor10.sh \
	tests/rwfile_EPT_truecolor12.sh \
	tests/rwfile_EPT_truecolor16.sh \
	tests/rwfile_PDF_bilevel.sh \
	tests/rwfile_PDF_bilevel_fax.sh \
	tests/rwfile_PDF_bilevel_jpeg.sh \
	tests/rwfile_PDF_bilevel_lzw.sh \
	tests/rwfile_PDF_bilevel_none.sh \
	tests/rwfile_PDF_bilevel_rle.sh \
	tests/rwfile_PDF_bilevel_zip.sh \
	tests/rwfile_PDF_gray.sh \
	tests/rwfile_PDF_gray_fax.sh \
	tests/rwfile_PDF_gray_jpeg.sh \
	tests/rwfile_PDF_gray_lzw.sh \
	tests/rwfile_PDF_gray_none.sh \
	tests/rwfile_PDF_gray_rle.sh \
	tests/rwfile_PDF_gray_zip.sh \
	tests/rwfile_PDF_pallette.sh \
	tests/rwfile_PDF_pallette_fax.sh \
	tests/rwfile_PDF_pallette_jpeg.sh \
	tests/rwfile_PDF_pallette_lzw.sh \
	tests/rwfile_PDF_pallette_none.sh \
	tests/rwfile_PDF_pallette_rle.sh \
	tests/rwfile_PDF_pallette_zip.sh \
	tests/rwfile_PDF_truecolor.sh \
	tests/rwfile_PDF_truecolor_fax.sh \
	tests/rwfile_PDF_truecolor_jpeg.sh \
	tests/rwfile_PDF_truecolor_lzw.sh \
	tests/rwfile_PDF_truecolor_none.sh \
	tests/rwfile_PDF_truecolor_rle.sh \
	tests/rwfile_PDF_truecolor_zip.sh \
	tests/rwfile_PS_bilevel.sh \
	tests/rwfile_PS_gray.sh \
	tests/rwfile_PS_pallette.sh \
	tests/rwfile_PS_truecolor.sh \
	tests/rwfile_PS_truecolor10.sh \
	tests/rwfile_PS_truecolor12.sh \
	tests/rwfile_PS_truecolor16.sh
endif
endif

if !HasPNG
TESTS_PNG_XFAIL_TESTS = \
	tests/rwblob_MNG_bilevel.sh \
	tests/rwblob_MNG_gray.sh \
	tests/rwblob_MNG_pallette.sh \
	tests/rwblob_MNG_truecolor.sh \
	tests/rwblob_PNG_bilevel.sh \
	tests/rwblob_PNG_gray.sh \
	tests/rwblob_PNG_pallette.sh \
	tests/rwblob_PNG_truecolor.sh \
	tests/rwfile_MNG_bilevel.sh \
	tests/rwfile_MNG_gray.sh \
	tests/rwfile_MNG_pallette.sh \
	tests/rwfile_MNG_truecolor.sh \
	tests/rwfile_MNG_truecolor10.sh \
	tests/rwfile_MNG_truecolor12.sh \
	tests/rwfile_MNG_truecolor16.sh \
	tests/rwfile_PNG_bilevel.sh \
	tests/rwfile_PNG_gray.sh \
	tests/rwfile_PNG_pallette.sh \
	tests/rwfile_PNG_truecolor.sh \
	tests/rwfile_PNG_truecolor10.sh \
	tests/rwfile_PNG_truecolor12.sh \
	tests/rwfile_PNG_truecolor16.sh
endif

if !HasTIFF
TESTS_TIFF_XFAIL_TESTS = \
	tests/rwblob_CALS_bilevel.sh \
	tests/rwblob_CALS_gray.sh \
	tests/rwblob_CALS_pallette.sh \
	tests/rwblob_CALS_truecolor.sh \
	tests/rwblob_EPT_bilevel.sh \
	tests/rwblob_EPT_gray.sh \
	tests/rwblob_EPT_pallette.sh \
	tests/rwblob_EPT_truecolor.sh \
	tests/rwblob_TIFF_bilevel.sh \
	tests/rwblob_TIFF_gray.sh \
	tests/rwblob_TIFF_pallette.sh \
	tests/rwblob_TIFF_truecolor.sh \
	tests/rwfile_CALS_bilevel.sh \
	tests/rwfile_CALS_gray.sh \
	tests/rwfile_CALS_pallette.sh \
	tests/rwfile_CALS_truecolor.sh \
	tests/rwfile_CALS_truecolor10.sh \
	tests/rwfile_CALS_truecolor12.sh \
	tests/rwfile_CALS_truecolor16.sh \
	tests/rwfile_EPT_bilevel.sh \
	tests/rwfile_EPT_gray.sh \
	tests/rwfile_EPT_pallette.sh \
	tests/rwfile_EPT_truecolor.sh \
	tests/rwfile_EPT_truecolor10.sh \
	tests/rwfile_EPT_truecolor12.sh \
	tests/rwfile_EPT_truecolor16.sh \
	tests/rwfile_PTIF_bilevel.sh \
	tests/rwfile_PTIF_gray.sh \
	tests/rwfile_PTIF_pallette.sh \
	tests/rwfile_PTIF_truecolor.sh \
	tests/rwfile_PTIF_truecolor10.sh \
	tests/rwfile_PTIF_truecolor12.sh \
	tests/rwfile_PTIF_truecolor16.sh \
	tests/rwfile_TIFF_bilevel.sh \
	tests/rwfile_TIFF_gray.sh \
	tests/rwfile_TIFF_pallette.sh \
	tests/rwfile_TIFF_truecolor.sh \
	tests/rwfile_TIFF_truecolor10.sh \
	tests/rwfile_TIFF_truecolor12.sh \
	tests/rwfile_TIFF_truecolor16.sh
endif

if !HasX11
TESTS_X11_XFAIL_TESTS = \
	tests/rwblob_XWD_bilevel.sh \
	tests/rwblob_XWD_gray.sh \
	tests/rwblob_XWD_pallette.sh \
	tests/rwblob_XWD_truecolor.sh \
	tests/rwfile_XWD_bilevel.sh \
	tests/rwfile_XWD_gray.sh \
	tests/rwfile_XWD_pallette.sh \
	tests/rwfile_XWD_truecolor.sh \
	tests/rwfile_XWD_truecolor10.sh \
	tests/rwfile_XWD_truecolor12.sh \
	tests/rwfile_XWD_truecolor16.sh
endif

# These tests do not currently actually fail so they are not included
# in TESTS_XFAIL_TESTS
if !HasBZLIB
TESTS_BZIP_XFAIL_TESTS = \
	tests/rwfile_MIFF_bilevel_bzip.sh \
	tests/rwfile_MIFF_gray_bzip.sh \
	tests/rwfile_MIFF_pallette_bzip.sh \
	tests/rwfile_MIFF_truecolor_bzip.sh
endif

# These tests do not currently actually fail so they are not included
# in TESTS_XFAIL_TESTS
if !HasZLIB
TESTS_ZIP_XFAIL_TESTS = \
	tests/rwfile_MIFF_bilevel_zip.sh \
	tests/rwfile_MIFF_gray_zip.sh \
	tests/rwfile_MIFF_pallette_zip.sh \
	tests/rwfile_MIFF_truecolor_zip.sh
endif

TESTS_XFAIL_TESTS = \
	$(TESTS_FPX_XFAIL_TESTS) \
	$(TESTS_JBIG_XFAIL_TESTS) \
	$(TESTS_JP2_XFAIL_TESTS) \
	$(TESTS_JPEG_XFAIL_TESTS) \
	$(TESTS_PNG_XFAIL_TESTS) \
	$(TESTS_PS_XFAIL_TESTS) \
	$(TESTS_TIFF_XFAIL_TESTS) \
	$(TESTS_X11_XFAIL_TESTS)

TESTS_TESTS = \
	tests/constitute.tap \
	tests/drawtests.tap \
	tests/rwblob.tap \
	tests/rwblob_sized.tap \
	tests/rwfile.tap \
	tests/rwfile_sized.tap \
	tests/rwfile_miff.tap \
	tests/rwfile_pdf.tap \
	tests/rwfile_deep.tap

TESTS_EXTRA_DIST = \
        tests/common.shi \
        tests/input_bilevel.miff \
        tests/input_gray.miff \
        tests/input_pallette.miff \
	tests/input_truecolor_70x46.miff \
        tests/input_truecolor.miff \
	tests/input_truecolor10.dpx \
	tests/input_truecolor12.dpx \
	tests/input_truecolor16.dpx \
        $(TESTS_TESTS)

TESTS_CLEANFILES = \
        tests/*out*.*


CHECK_TYPES = bilevel gray pallette truecolor

CHECK_DEEP_TYPES = truecolor10 truecolor12 truecolor16

# Blob formats to test which don't require that size be specified
CHECK_BLOB_FORMATS = \
	AVS \
	BMP \
	CALS \
	CIN \
	DIB \
	DPX \
	EPDF \
	EPT \
	FAX \
	FPX \
	G3 \
	FITS \
	GIF \
	GIF87 \
	JBIG \
	JPEG \
	JP2 \
	MAT \
	MIFF \
	MNG \
	MTV \
	P7 \
	PBM \
	PCD \
	PCDS \
	PCX \
	PGM \
	PICON \
	PICT \
	PNG \
	PPM \
	RAS \
	SGI \
	SUN \
	TGA \
	TIFF \
	TXT \
	VDA \
	VICAR \
	VIFF \
	VST \
	WBMP \
	XBM \
	XPM \
	XWD

# File formats to test which don't require that size be specified
CHECK_FILE_FORMATS = \
	AVS \
	BMP \
	CALS \
	CIN \
	DCX \
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
	FPX \
	GIF \
	GRAY \
	ICB \
	JBIG \
	JPEG \
	JP2 \
	MAT \
	MIFF \
	MNG \
	MPR \
	MTV \
	P7 \
	PBM \
	PCD \
	PCDS \
	PCX \
	PDF \
	PGM \
	PICON \
	PICT \
	PNG \
	PNM \
	PPM \
	PS \
	PSD \
	PTIF \
	RAS \
	SGI \
	SUN \
	TGA \
	TXT \
	TIFF \
	VDA \
	VICAR \
	VIFF \
	VST \
	WBMP \
	XBM \
	XPM \
	XWD

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

# File formats which support more than 8 bits per sample
CHECK_DEEP_FILE_FORMATS = \
	CIN \
	DPX \
	FITS \
	JP2 \
	MAT \
	MIFF \
	MNG \
	PNG \
	PPM \
	PTIF \
	SGI \
	TIFF \
	TGA \
	TXT

CHECK_MIFF_FILE_COMPRESS = \
	none \
	bzip \
	zip \
	rle

CHECK_PDF_FILE_COMPRESS = \
	none \
	fax \
	jpeg \
	lzw \
	rle \
	zip

generate-check-blob-scripts:
	sh_file="$(srcdir)/tests/rwblob.tap" ; \
	( \
	  total_tests=0 ; \
	  for format in $(CHECK_BLOB_FORMATS) ; \
	  do \
	    for type in  $(CHECK_TYPES) ; \
	    do \
	     total_tests=$$(($$total_tests + 1)) ; \
	    done ; \
	  done ; \
	  echo "#!/bin/sh" ; \
	  echo "# Copyright (C) 2004-2012 GraphicsMagick Group" ; \
	  echo ". \$${srcdir}/tests/common.shi" ; \
	  echo ; \
	  echo "# Test program" ; \
	  echo "rwblob=./rwblob" ; \
	  echo ; \
	  echo "# Types we will test" ; \
	  echo "check_types='$(CHECK_TYPES)'" ; \
	  echo ; \
	  echo "# Number of tests we plan to run" ; \
	  echo "test_plan_fn $$total_tests" ; \
	  for format in $(CHECK_BLOB_FORMATS) ; \
	    do \
	    echo ; \
	    echo "# $$format format" ; \
	    echo 'for type in $${check_types}' ; \
	    echo 'do' ; \
	    echo "  test_command_fn \"$$format \$${type}\" \$${MEMCHECK} \$${rwblob} \"\$${SRCDIR}/input_\$${type}.miff\" $$format" ; \
	    echo 'done' ; \
	  done ; \
	  echo ; \
	  echo ':' ; \
	) > "$$sh_file" ; \
	chmod +x "$$sh_file"

generate-check-file-scripts:
	sh_file="$(srcdir)/tests/rwfile.tap" ; \
	( \
	  total_tests=0 ; \
	  for format in $(CHECK_FILE_FORMATS) ; \
	  do \
	    for type in  $(CHECK_TYPES) ; \
	    do \
	     total_tests=$$(($$total_tests + 1)) ; \
	    done ; \
	  done ; \
	  echo "#!/bin/sh" ; \
	  echo "# Copyright (C) 2004-2012 GraphicsMagick Group" ; \
	  echo ". \$${srcdir}/tests/common.shi" ; \
	  echo ; \
	  echo "# Test program" ; \
	  echo "rwfile=./rwfile" ; \
	  echo ; \
	  echo "# Types we will test" ; \
	  echo "check_types='$(CHECK_TYPES)'" ; \
	  echo ; \
	  echo "# Number of tests we plan to run" ; \
	  echo "test_plan_fn $$total_tests" ; \
	  for format in $(CHECK_FILE_FORMATS) ; \
	    do \
	    echo ; \
	    echo "# $$format format" ; \
	    echo 'for type in $${check_types}' ; \
	    echo 'do' ; \
	    echo "  test_command_fn \"$$format \$${type}\" \$${MEMCHECK} \$${rwfile} -filespec \"out_\$${type}_%d\" \"\$${SRCDIR}/input_\$${type}.miff\" $$format" ; \
	    echo 'done' ; \
	  done ; \
	  echo ; \
	  echo ':' ; \
	) > "$$sh_file" ; \
	chmod +x "$$sh_file"

generate-check-sized-blob-scripts:
	sh_file="$(srcdir)/tests/rwblob_sized.tap" ; \
	( \
	  total_tests=0 ; \
	  for format in $(CHECK_SIZED_BLOB_FORMATS) ; \
	  do \
	    for type in truecolor_70x46 ; \
	    do \
	     total_tests=$$(($$total_tests + 1)) ; \
	    done ; \
	  done ; \
	  echo "#!/bin/sh" ; \
	  echo "# Copyright (C) 2004-2012 GraphicsMagick Group" ; \
	  echo ". \$${srcdir}/tests/common.shi" ; \
	  echo ; \
	  echo "# Test program" ; \
	  echo "rwblob=./rwblob" ; \
	  echo ; \
	  echo "# Types we will test" ; \
	  echo "check_types='truecolor_70x46'" ; \
	  echo ; \
	  echo "# Number of tests we plan to run" ; \
	  echo "test_plan_fn $$total_tests" ; \
	  for format in $(CHECK_SIZED_BLOB_FORMATS) ; \
	    do \
	    echo ; \
	    echo "# $$format format" ; \
	    echo 'for type in $${check_types}' ; \
	    echo 'do' ; \
	    echo "  test_command_fn \"$$format \$${type}\" \$${MEMCHECK} \$${rwblob} -size 70x46 \"\$${SRCDIR}/input_\$${type}.miff\" $$format" ; \
	    echo 'done' ; \
	  done ; \
	  echo ; \
	  echo ':' ; \
	) > "$$sh_file" ; \
	chmod +x "$$sh_file"

generate-check-sized-file-scripts:
	sh_file="$(srcdir)/tests/rwfile_sized.tap" ; \
	( \
	  total_tests=0 ; \
	  for format in $(CHECK_SIZED_FILE_FORMATS) ; \
	  do \
	    for type in truecolor_70x46 ; \
	    do \
	     total_tests=$$(($$total_tests + 1)) ; \
	    done ; \
	  done ; \
	  echo "#!/bin/sh" ; \
	  echo "# Copyright (C) 2004-2012 GraphicsMagick Group" ; \
	  echo ". \$${srcdir}/tests/common.shi" ; \
	  echo ; \
	  echo "# Test program" ; \
	  echo "rwfile=./rwfile" ; \
	  echo ; \
	  echo "# Types we will test" ; \
	  echo "check_types='truecolor_70x46'" ; \
	  echo ; \
	  echo "# Number of tests we plan to run" ; \
	  echo "test_plan_fn $$total_tests" ; \
	  for format in $(CHECK_SIZED_FILE_FORMATS) ; \
	    do \
	    echo ; \
	    echo "# $$format format" ; \
	    echo 'for type in $${check_types}' ; \
	    echo 'do' ; \
	    echo "  test_command_fn \"$$format \$${type}\" \$${MEMCHECK} \$${rwfile} -filespec \"out_\$${type}_%d\" -size 70x46 \"\$${SRCDIR}/input_\$${type}.miff\" $$format" ; \
	    echo 'done' ; \
	  done ; \
	  echo ; \
	  echo ':' ; \
	) > "$$sh_file" ; \
	chmod +x "$$sh_file"

generate-check-miff-compressed-scripts:
	sh_file="$(srcdir)/tests/rwfile_miff.tap" ; \
	( \
	  total_tests=0 ; \
	  for compress in $(CHECK_MIFF_FILE_COMPRESS) ; \
	  do \
	    for type in  $(CHECK_TYPES) ; \
	    do \
	     total_tests=$$(($$total_tests + 1)) ; \
	    done ; \
	  done ; \
	  echo "#!/bin/sh" ; \
	  echo "# Copyright (C) 2004-2012 GraphicsMagick Group" ; \
	  echo "# Test MIFF format with various storage and compression types" ; \
	  echo ". \$${srcdir}/tests/common.shi" ; \
	  echo ; \
	  echo "# Test program" ; \
	  echo "rwfile=./rwfile" ; \
	  echo ; \
	  echo "# Storage types we will test" ; \
	  echo "check_types='$(CHECK_TYPES)'" ; \
	  echo ; \
	  echo "# Comression types we will test" ; \
	  echo "compress_types='$(CHECK_MIFF_FILE_COMPRESS)'" ; \
	  echo ; \
	  echo "# Number of tests we plan to run" ; \
	  echo "test_plan_fn $$total_tests" ; \
	  echo ; \
	  echo "for compress in \$${compress_types}" ; \
	  echo "do" ; \
	  echo '  for type in $${check_types}' ; \
	  echo '  do' ; \
	  echo "    test_command_fn \"MIFF \$${compress} \$${type}\" \$${MEMCHECK} \$${rwfile} -filespec \"out_\$${type}_\$${compress}_%d\" -compress \$$compress \"\$${SRCDIR}/input_\$${type}.miff\" MIFF" ; \
	  echo '  done' ; \
	  echo 'done' ; \
	  echo ; \
	  echo ':' ; \
	) > "$$sh_file" ; \
	chmod +x "$$sh_file"

generate-check-pdf-compressed-scripts:
	sh_file="$(srcdir)/tests/rwfile_pdf.tap" ; \
	( \
	  total_tests=0 ; \
	  for compress in $(CHECK_PDF_FILE_COMPRESS) ; \
	  do \
	    for type in  $(CHECK_TYPES) ; \
	    do \
	     total_tests=$$(($$total_tests + 1)) ; \
	    done ; \
	  done ; \
	  echo "#!/bin/sh" ; \
	  echo "# Copyright (C) 2004-2012 GraphicsMagick Group" ; \
	  echo "# Test PDF format with various storage and compression types" ; \
	  echo ". \$${srcdir}/tests/common.shi" ; \
	  echo ; \
	  echo "# Test program" ; \
	  echo "rwfile=./rwfile" ; \
	  echo ; \
	  echo "# Storage types we will test" ; \
	  echo "check_types='$(CHECK_TYPES)'" ; \
	  echo ; \
	  echo "# Comression types we will test" ; \
	  echo "compress_types='$(CHECK_PDF_FILE_COMPRESS)'" ; \
	  echo ; \
	  echo "# Number of tests we plan to run" ; \
	  echo "test_plan_fn $$total_tests" ; \
	  echo ; \
	  echo "for compress in \$${compress_types}" ; \
	  echo "do" ; \
	  echo '  for type in $${check_types}' ; \
	  echo '  do' ; \
	  echo "    test_command_fn \"PDF \$${type} \$${compress}\" \$${MEMCHECK} \$${rwfile} -filespec \"out_\$${type}_\$${compress}_%d\" -compress \$$compress \"\$${SRCDIR}/input_\$${type}.miff\" PDF" ; \
	  echo '  done' ; \
	  echo 'done' ; \
	  echo ; \
	  echo ':' ; \
	) > "$$sh_file" ; \
	chmod +x "$$sh_file"


generate-check-deep-scripts:
	sh_file="$(srcdir)/tests/rwfile_deep.tap" ; \
	( \
	  total_tests=0 ; \
	  for format in $(CHECK_DEEP_FILE_FORMATS) ; \
	  do \
	    for type in  $(CHECK_DEEP_TYPES) ; \
	    do \
	     total_tests=$$(($$total_tests + 1)) ; \
	    done ; \
	  done ; \
	  echo "#!/bin/sh" ; \
	  echo "# Copyright (C) 2004-2012 GraphicsMagick Group" ; \
	  echo ". \$${srcdir}/tests/common.shi" ; \
	  echo ; \
	  echo "# Test program" ; \
	  echo "rwfile=./rwfile" ; \
	  echo ; \
	  echo "# Types we will test" ; \
	  echo "check_types='$(CHECK_DEEP_TYPES)'" ; \
	  echo ; \
	  echo "# Number of tests we plan to run" ; \
	  echo "test_plan_fn $$total_tests" ; \
	  for format in $(CHECK_DEEP_FILE_FORMATS) ; \
	    do \
	    echo ; \
	    echo "# $$format format" ; \
	    echo 'for type in $${check_types}' ; \
	    echo 'do' ; \
	    echo "  test_command_fn \"$$format \$${type}\" \$${MEMCHECK} \$${rwfile} -filespec \"out_\$${type}_%d\" \"\$${SRCDIR}/input_\$${type}.dpx\" $$format" ; \
	    echo 'done' ; \
	  done ; \
	  echo ; \
	  echo ':' ; \
	) > "$$sh_file" ; \
	chmod +x "$$sh_file"

GENERATE_TESTS_SCRIPTS_DEPS = \
	generate-check-blob-scripts \
	generate-check-file-scripts \
	generate-check-sized-blob-scripts \
	generate-check-sized-file-scripts \
	generate-check-miff-compressed-scripts \
	generate-check-pdf-compressed-scripts \
	generate-check-deep-scripts

generate-tests-scripts: $(GENERATE_TESTS_SCRIPTS_DEPS)
