
# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
	-I$(top_srcdir)/oless/h

# A convenience library
noinst_LTLIBRARIES = libBasics.la

# Sources to compile into library
libBasics_la_SOURCES = \
  a_file.cpp b_string.cpp color.cpp common_b.cpp debug.cpp filename.cpp \
  geometry.cpp graphic.cpp

# Additional files to distribute
EXTRA_DIST =

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  a_file.h b_error.h b_memory.h b_string.h b_types.h fpx_color.h common.h \
  common_a.h common_b.h debug.h filename.h fileutil.h geometry.h graphic.h \
  mac_comp.h npixel.h numbers.h p_define.h pasconv.h pixel.h resource.h \
  swapbyte.h

