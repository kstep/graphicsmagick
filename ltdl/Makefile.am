#  Makefile for building ImageMagick library
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

AUTOMAKE_OPTIONS = 1.6.2 foreign

if WITH_LTDL

if INSTALL_LTDL
include_HEADERS = ltdl.h
lib_LTLIBRARIES = libltdl.la
else
noinst_HEADERS = ltdl.h
endif

if CONVENIENCE_LTDL
noinst_LTLIBRARIES = libltdlc.la
endif

endif # WITH_LTDL

EXTRA_DIST = ltdl.h

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libltdl.la libltdlc.la

libltdl_la_SOURCES = ltdl.c
libltdl_la_LDFLAGS = -no-undefined -version-info 4:0:1
libltdl_la_LIBADD = $(LIBADD_DL)

libltdlc_la_SOURCES = ltdl.c
libltdlc_la_LIBADD = $(LIBADD_DL)
