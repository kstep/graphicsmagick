
# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
	-I$(top_srcdir)/oless/h

# A convenience library
noinst_LTLIBRARIES = libRiImage.la

# Sources to compile into library
libRiImage_la_SOURCES = \
  compress.cpp corr_lut.cpp cp32to24.cpp cp_jpeg.cpp fn_srch.cpp matrix.cpp \
  ph_image.cpp pimgfile.cpp pr_level.cpp priimage.cpp ptile.cpp ri_page.cpp \
  ri_sys.cpp viewimg.cpp

# Additional files to distribute
EXTRA_DIST = 

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  fpxcompress.h corr_lut.h cp32to24.h cp_jpeg.h cpqtime.h fn_srch.h \
  matrix.h ph_image.h pimgfile.h pr_level.h priimage.h ptile.h \
  ri_imgvw.h ri_lib.h ri_libio.h ri_page.h ri_sys.h viewimg.h

