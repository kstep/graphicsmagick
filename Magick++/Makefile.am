#
# Top Makefile for Magick++
#
# Copyright Bob Friesenhahn, 1999
#
AUTOMAKE_OPTIONS = 1.2 dist-zip #readme-alpha

# Additional files to distribute
EXTRA_DIST = Magick++.dsw ltcf-c.sh ltcf-cxx.sh

SUBDIRS = lib tests demo

# Non-Automake subdirectories to distribute
DISTDIRS = doc
dist-hook:
	( \
	  builddir=`pwd` ; \
	  cd $(srcdir) && \
	  ( \
	    for dir in $(DISTDIRS) ; do \
	      find $$dir -depth -print | egrep -v '(~$$)|(\.bak$$)|(CVS)' | cpio -pdum $$builddir/$(distdir) 2> /dev/null ; \
	    done \
	  ) \
	)
