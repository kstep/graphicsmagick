
AUTOMAKE_OPTIONS = 1.6 dist-zip dist-bzip2 foreign

# Directories to include in distribution
DIST_SUBDIRS = basics fpx jpeg ole oless ri_image

# Directories to build
SUBDIRS = oless jpeg basics ole ri_image fpx

# Where to look for include files
INCLUDES = -I$(top_srcdir)

# Headers which are installed
include_HEADERS = fpxlib.h

# Shared libraries built in this directory
lib_LTLIBRARIES = libfpx.la

# Library ld flags
libfpx_la_LDFLAGS = -version-info $(MAJOR_VERSION):$(MINOR_VERSION)$(MICRO_VERSION):0

# Sources
libfpx_la_SOURCES =

# Convenience libraries
libfpx_la_LIBADD = \
  basics/libBasics.la fpx/libfpx.la jpeg/libJpeg.la ole/libole.la \
  ri_image/libRiImage.la oless/libOless.la

MAJOR_VERSION=@MAJOR_VERSION@
MINOR_VERSION=@MINOR_VERSION@
MICRO_VERSION=@MICRO_VERSION@

# Additional files to distribute
EXTRA_DIST = README _mac.sea ltcf-c.sh ltcf-cxx.sh

# Headers which are not installed but which are distributed
noinst_HEADERS = flashpix.h

# Non-Automake subdirectories to distribute
DISTDIRS = _win doc macdeps
dist-hook:
	( \
	  builddir=`pwd` ; \
	  cd $(srcdir) && \
	  ( \
	    for dir in $(DISTDIRS) ; do \
	      find $$dir -depth -print | egrep -v '(~$$)|(/CVS)|(/\.#)|(/\.deps)' | cpio -pdum $$builddir/$(distdir) 2> /dev/null ; \
	    done \
	  ) \
	)

