
# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
	-I$(top_srcdir)/oless/h

# A convenience library
noinst_LTLIBRARIES = libole.la

# Sources to compile into library
libole_la_SOURCES = \
  gen_guid.cpp olecore.cpp olefiles.cpp olehstrm.cpp oleprop.cpp \
  oleprops.cpp olestats.cpp olestorg.cpp olestrm.cpp oleclink.cpp

# Additional files to distribute
EXTRA_DIST = 

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  gen_guid.h oleclink.h olecomm.h olecore.h olefiles.h olehstrm.h \
  oleprop.h oleprops.h olestats.h olestorg.h olestrm.h str8_16.h
