#
# Makefile for Magick++ tests
#
# Copyright Bob Friesenhahn, 1999
#
AUTOMAKE_OPTIONS = 1.2 

INCLUDES = -I../lib

#
# Execute 'make check' to compile and run a test program
#
check_PROGRAMS = color attributes appendImages averageImages coalesceImages \
	montageImages morphImages readWriteImages readWriteBlob

LDADD = ../lib/libMagick++.la

appendImages_SOURCES	= appendImages.cpp
attributes_SOURCES	= attributes.cpp
averageImages_SOURCES	= averageImages.cpp
coalesceImages_SOURCES	= coalesceImages.cpp
color_SOURCES		= color.cpp
montageImages_SOURCES	= montageImages.cpp
morphImages_SOURCES	= morphImages.cpp
readWriteImages_SOURCES = readWriteImages.cpp
readWriteBlob_SOURCES	= readWriteBlob.cpp

EXTRA_DIST = \
	Generic.ttf test_image.miff test_image_anim.gif \
	appendImages.dsp attributes.dsp averageImages.dsp coalesceImages.dsp \
	color.dsp montageImages.dsp morphImages.dsp readWriteImages.dsp \
	readWriteBlob.dsp

check:
	./color
	./attributes
	./appendImages
	-./averageImages
	./coalesceImages
	./montageImages
	./morphImages
#	./readWriteImages
	./readWriteBlob test_image.miff AVS
	./readWriteBlob test_image.miff BMP
	./readWriteBlob test_image.miff CMYK
	./readWriteBlob test_image.miff DIB
	./readWriteBlob test_image.miff FAX
	./readWriteBlob test_image.miff FITS
	./readWriteBlob test_image.miff GIF
	./readWriteBlob test_image.miff GRAY
	./readWriteBlob test_image.miff HDF
	./readWriteBlob test_image.miff ICB
	./readWriteBlob test_image.miff JBIG
	./readWriteBlob test_image.miff JPEG
#	./readWriteBlob test_image.miff MIFF
	./readWriteBlob test_image.miff MNG
	./readWriteBlob test_image.miff MONO
	./readWriteBlob test_image.miff MTV
	./readWriteBlob test_image.miff P7
	./readWriteBlob test_image.miff PBM
	./readWriteBlob test_image.miff PCD
	./readWriteBlob test_image.miff PCDS
	./readWriteBlob test_image.miff PCX
#	./readWriteBlob test_image.miff PICT
	./readWriteBlob test_image.miff PGM
	./readWriteBlob test_image.miff PNG
	./readWriteBlob test_image.miff PNM
	./readWriteBlob test_image.miff PPM
	./readWriteBlob test_image.miff PSD
#	./readWriteBlob test_image.miff PTIF # "Deflate compression support is not configured"
	./readWriteBlob test_image.miff RAS
	./readWriteBlob test_image.miff RGB
	./readWriteBlob test_image.miff RGBA
	./readWriteBlob test_image.miff SGI
	./readWriteBlob test_image.miff SUN
	./readWriteBlob test_image.miff TGA
#	./readWriteBlob test_image.miff TIFF
#	./readWriteBlob test_image.miff UYVY
	./readWriteBlob test_image.miff VDA
	./readWriteBlob test_image.miff VICAR
	./readWriteBlob test_image.miff VIFF
	./readWriteBlob test_image.miff VST
	./readWriteBlob test_image.miff XBM
	./readWriteBlob test_image.miff XPM
	./readWriteBlob test_image.miff XWD
#	./readWriteBlob test_image.miff YUV

