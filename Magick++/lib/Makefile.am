#
# Makefile for Magick++ library
#
# Copyright Bob Friesenhahn, 1999
#
AUTOMAKE_OPTIONS = 1.2

lib_LTLIBRARIES    = libMagick++.la
INTERFACE_CURRENT  = @INTERFACE_CURRENT@
INTERFACE_REVISION = @INTERFACE_REVISION@
INTERFACE_AGE      = @INTERFACE_AGE@

libMagick___la_SOURCES = MagickBlob.cpp MagickColor.cpp MagickDrawable.cpp \
	MagickException.cpp MagickFunctions.cpp MagickGeometry.cpp \
	MagickImage.cpp MagickLastError.cpp MagickMontage.cpp \
	MagickOptions.cpp

libMagick___la_LDFLAGS = -version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE)

include_HEADERS = Magick++.h MagickBlob.h MagickColor.h MagickDrawable.h \
	MagickException.h MagickFunctions.h MagickGeometry.h MagickImage.h \
	MagickInclude.h MagickLastError.h MagickMontage.h MagickOptions.h \
	MagickSTL.h

EXTRA_DIST = Magick++.dsp
