#
#  Top-Level Makefile for building ImageMagick
#
# Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated
# to making software imaging solutions freely available.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files ("ImageMagick"),
# to deal in ImageMagick without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense
# and/or sell copies of ImageMagick, and to permit persons to whom the
# ImageMagick is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of ImageMagick.
#
# The software is provided "as is", without warranty of any kind, express or
# implied, including but not limited to the warranties of merchantability,
# fitness for a particular purpose and noninfringement.  In no event shall
# ImageMagick Studio be liable for any claim, damages or other liability,
# whether in an action of contract, tort or otherwise, arising from, out of
# or in connection with ImageMagick or the use or other dealings in
# ImageMagick.
#
# Except as contained in this notice, the name of the ImageMagick Studio
# shall not be used in advertising or otherwise to promote the sale, use or
# other dealings in ImageMagick without prior written authorization from the
# ImageMagick Studio.
#
#  This file is currently maintained by Bob Friesenhahn,
#  bfriesen@simple.dallas.tx.us. Please report any bugs via the ImageMagick
#  bug tracking system at http://www.imagemagick.org/ImageMagick/bugs.

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 dist-zip dist-bzip2 foreign

# Subdirectories to build
if WITH_MAGICK_PLUS_PLUS
MAGICK_PLUS_PLUS=Magick++
endif # WITH_MAGICK_PLUS_PLUS

# Directories to include in distribution
DIST_SUBDIRS=ltdl coders magick utilities tests Magick++

if WITH_MODULES
SUBDIRS = ltdl magick coders utilities tests $(MAGICK_PLUS_PLUS)
else
SUBDIRS = coders magick utilities tests $(MAGICK_PLUS_PLUS)
endif # WITH_MODULES

# Architecture independent data files installed in the package's data directory
pkgdata_DATA = Copyright.txt QuickStart.txt

# Additional files to distribute
EXTRA_DIST = Copyright.txt TODO.txt ImageMagick.html Magickshr.opt Make.com \
	Manifest.ps PLATFORMS.txt QuickStart.txt README.txt acconfig.h \
	lndir.sh ltcf-c.sh ltcf-cxx.sh ltcf-gcj.sh ltconfig

# Non-Automake subdirectories to distribute
DISTDIRS = contrib guide images scenes scripts www PerlMagick
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

#
# Additional install rules
#

# Install HTML files
pkgdocdir = @MagickSharePath@
DOCDIRS = images www www/api www/api/types www/Magick++
install-data-html:
	$(mkinstalldirs) $(pkgdocdir)
	$(INSTALL_DATA) $(srcdir)/ImageMagick.html $(pkgdocdir)
	for dir in $(DOCDIRS) ; do \
	  $(mkinstalldirs) $(pkgdocdir)/$$dir && \
	  $(INSTALL_DATA) $(srcdir)/$$dir/*.* $(pkgdocdir)/$$dir ; \
	done

# Uninstall HTML files
uninstall-data-html:
	rm -f $(pkgdocdir)/ImageMagick.html
	for dir in $(DOCDIRS) ; do \
	  rm -rf $(pkgdocdir)/$$dir ; \
	done

if WITH_PERL

PERLMAGICK=PerlMagick
PERLMAKEMAKER=$(PERLMAGICK)/Makefile.PL
PERLMAKEFILE=$(PERLMAGICK)/Makefile

# If source files missing, see if they can be obtained via VPATH
perl-sources:
	if test -n "$(VPATH)" ; then \
	  imagemagick=`(cd $(VPATH) ; pwd)` && \
	  ( cd $(PERLMAGICK) && sh $$imagemagick/lndir.sh $$imagemagick/$(PERLMAGICK) ) \
	fi
	touch perl-sources

if WITH_PERL_DYNAMIC

$(PERLMAKEFILE): perl-sources $(PERLMAKEMAKER)
	cd $(PERLMAGICK) && @PERL@ Makefile.PL $(PERL_MAKE_OPTIONS)

install-exec-perl: $(PERLMAKEFILE)
	( cd $(PERLMAGICK) && $(MAKE) CC='@CC@' && \
	$(MAKE) CC='@CC@' install && \
	$(MAKE) clean && rm -f  Makefile.old )

check-perl: $(PERLMAKEFILE)
	cd $(PERLMAGICK) && $(MAKE) CC='@CC@' test

else
if WITH_PERL_STATIC

PERLSTATICNAME=PerlMagick

$(PERLMAKEFILE): perl-sources $(PERLMAKEMAKER)
	cd $(PERLMAGICK) && @PERL@ Makefile.PL MAP_TARGET=$(PERLSTATICNAME) $(PERL_MAKE_OPTIONS)

$(PERLMAGICK)/$(PERLMAGICK): $(PERLMAKEFILE)
	( cd $(PERLMAGICK) && $(MAKE) CC='@CC@' $(PERLSTATICNAME) )

install-exec-perl: $(PERLMAGICK)/$(PERLMAGICK)
	( cd $(PERLMAGICK) && \
	$(MAKE) -f Makefile.aperl CC='@CC@' inst_perl MAP_TARGET=$(PERLSTATICNAME) && \
	$(MAKE) -f Makefile.aperl map_clean && \
	$(MAKE) clean && rm -f  Makefile.old )

check-perl: $(PERLMAGICK)/$(PERLMAGICK)
	cd $(PERLMAGICK) && $(MAKE) -f Makefile.aperl CC='@CC@' test

endif # WITH_PERL_STATIC
endif # WTIH_PERL_DYNAMIC


clean-perl: $(PERLMAKEFILE)
	cd $(PERLMAGICK) && $(MAKE) CC='@CC@' clean && rm -f Makefile.old PerlMagick
	rm -f perl-sources

distclean-perl: clean-perl

else
# Satisfy makefile requirements if not building PERL
all-perl:
install-exec-perl:
check-perl:
clean-perl:
distclean-perl:
endif # WITH_PERL

#all-local: all-perl

# Install PerlMagick
install-exec-local: install-exec-perl

# Install arch-independent package data
install-data-local: install-data-html

# Uninstall arch-independent package data
uninstall-local: uninstall-data-html

# Do a make clean in PerlMagick directory 
clean-local: clean-perl

# Do a make distclean in PerlMagick directory 
distclean-local: distclean-perl

# Maintainer clean is just like 'distclean' for PerlMagick
maintainer-clean-local: distclean-local

# Run tests on PerlMagick
check-local: check-perl
