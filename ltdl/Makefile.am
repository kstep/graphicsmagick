# Copyright (C) 2004 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building libltdl
#
#

LTDL_HDRS = \
        ltdl/ltdl.h

LTDL_SRCS = \
        ltdl/ltdl.c \
        ltdl/ltdl.h

#if WITH_LTDL
#LIB_LTDL=$(LIBLTDL)
#else
#LIB_LTDL=
#endif

if WITH_LTDL
 LIBLTDL=ltdl/libltdl.la
if INSTALL_LTDL
  LTDL_INSTALLHEADERS =  $(LTDL_HDRS)
else
  LTDL_NOINST_HDRS = $(LTDL_HDRS)
endif # INSTALL_LTDL
else
 LIBLTDL=
endif # WITH_LTDL


LTDL_CLEANFILES = \
        ltdl/libltdl.la

ltdl_libltdl_la_SOURCES = $(LTDL_SRCS)
ltdl_libltdl_la_LDFLAGS = -no-undefined -version-info 4:0:1
ltdl_libltdl_la_LIBADD = $(LIBADD_DL)

