
# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
	-I$(top_srcdir)/oless/h

# A convenience library
noinst_LTLIBRARIES = libfpx.la

# Sources to compile into library
libfpx_la_SOURCES = \
  buffdesc.cpp coltwist.cpp f_fpxio.cpp f_fpxvw.cpp filter.cpp \
  fpxformt.cpp fpximgvw.cpp fpxlib.cpp fpxlibio.cpp fpxutils.cpp \
  imginfio.cpp pres_fpx.cpp ptil_fpx.cpp


# Additional files to distribute
EXTRA_DIST = 

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  buffdesc.h coltwist.h f_fpxio.h f_fpxvw.h fpxformt.h fpximgvw.h fpxlib.h \
  fpxlibio.h fpxutils.h imginfio.h pres_fpx.h ptil_fpx.h thmbnail.h
