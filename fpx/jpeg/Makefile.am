
# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
	-I$(top_srcdir)/oless/h

# A convenience library
noinst_LTLIBRARIES = libJpeg.la

# Sources to compile into library
libJpeg_la_SOURCES = \
  chen_dct.c dbuffer.c decoder.c dectile.c dhuff.c djpeg.c dllmain.c \
  dparser.c dscan.c ebuffer.c ejpeg.c enctile.c eparser.c fpxmem.c \
  huffman.c mcu2rast.c process.c public.c tables.c win_dct.c winograd.c

# Additional files to distribute
EXTRA_DIST = makefile.gcc

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  chen_dct.h dbuffer.h dct.h decoder.h dectile.h dhuff.h djpeg.h \
  dparser.h ebuffer.h ejpeg.h encoder.h enctile.h eparser.h fpxjpeg.h \
  fpxmem.h huffman.h jpegconf.h process.h public.h tables.h win_dct.h \
  winograd.h
