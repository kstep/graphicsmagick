# Copyright (C) 2004-2012 GraphicsMagick Group
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


TESTS_XFAIL_TESTS =

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
