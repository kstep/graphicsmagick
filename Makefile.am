# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Top-Level Makefile for building GraphicsMagick
#
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7.2 dist-zip dist-bzip2 foreign

PACKAGE_NAME = @PACKAGE_NAME@
PACKAGE_VERSION = @PACKAGE_VERSION@

# Subdirectories to build
if WITH_MAGICK_PLUS_PLUS
MAGICK_PLUS_PLUS=Magick++
endif # WITH_MAGICK_PLUS_PLUS

# Directories to include in distribution
DIST_SUBDIRS=ltdl coders filters magick utilities tests Magick++

if WITH_LTDL
LTDL=ltdl
else
LTDL=
endif

if WITH_MODULES
SUBDIRS = $(LTDL) magick coders filters $(MAGICK_PLUS_PLUS) utilities tests
else
SUBDIRS = $(LTDL) coders filters magick $(MAGICK_PLUS_PLUS) utilities tests
endif # WITH_MODULES

# Architecture independent data files installed in the package's data directory
pkgdatadir = @MagickSharePath@
pkgdata_DATA = Copyright.txt QuickStart.txt

# Additional files to distribute
EXTRA_DIST = ChangeLog Copyright.txt INSTALL-mac.txt INSTALL-unix.txt \
	INSTALL-vms.txt INSTALL-windows.txt TODO.txt index.html \
	Magickshr.opt Make.com NEWS PLATFORMS.txt QuickStart.txt \
	README.txt lndir.sh version.sh winpath.sh

# Non-Automake subdirectories to distribute
DISTDIRS = guide images locale scenes scripts www PerlMagick
dist-hook:
	( \
	  builddir=`pwd` ; \
	  cd $(srcdir) && \
	  ( \
	    for dir in $(DISTDIRS) ; do \
	      find $$dir -depth -print | egrep -v '(~$$)|(/CVS)|(/\.#)|(/\.deps)' \
	        | cpio -pdum $$builddir/$(distdir) 2> /dev/null ; \
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
	$(mkinstalldirs) $(DESTDIR)$(pkgdocdir)
	$(INSTALL_DATA) $(srcdir)/index.html $(DESTDIR)$(pkgdocdir)
	@for dir in $(DOCDIRS) ; do \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdocdir)/$$dir && \
	  for file in $(srcdir)/$$dir/*.* ; do \
	    echo "$(INSTALL_DATA) $$file $(DESTDIR)$(pkgdocdir)/$$dir" ; \
	    $(INSTALL_DATA) "$$file" $(DESTDIR)$(pkgdocdir)/$$dir ; \
	  done ; \
	done

# Uninstall HTML files
uninstall-data-html:
	rm -f $(DESTDIR)$(pkgdocdir)/index.html
	for dir in $(DOCDIRS) ; do \
	  rm -rf $(DESTDIR)$(pkgdocdir)$$dir ; \
	done

# Ensure that version.h at $(srcdir)/magick/version.h is kept up to date.
magick-version: magick/version.h
	@if test -n "$(VPATH)" ; then \
	  cmp magick/version.h $(srcdir)/magick/version.h > /dev/null ; \
          if test $$? -eq 1 ; then \
	    echo "Updating $(srcdir)/magick/version.h ..."; \
	    cp magick/version.h $(srcdir)/magick/version.h ; \
	  fi ; \
	  cmp VisualMagick/installer/inc/version.isx $(srcdir)/VisualMagick/installer/inc/version.isx > /dev/null ; \
          if test $$? -eq 1 ; then \
	    echo "Updating $(srcdir)/VisualMagick/installer/inc/version.isx ..."; \
	    cp VisualMagick/installer/inc/version.isx $(srcdir)/VisualMagick/installer/inc/version.isx ; \
	  fi ; \
	fi ; \
	touch magick-version

# If source files missing, see if they can be obtained via VPATH
perl-sources:
	@if test -n "$(VPATH)" ; then \
	  echo "Linking PerlMagick Sources ..." ; \
	  imagemagick=`(cd $(VPATH) ; pwd)` && \
	  ( cd $(PERLMAGICK) && \
	    sh $$imagemagick/lndir.sh $$imagemagick/$(PERLMAGICK) ) \
	fi ; \
	touch perl-sources

if WITH_PERL

PERLMAGICK=PerlMagick
PERLMAKEMAKER=$(PERLMAGICK)/Makefile.PL
PERLMAKEFILE=$(PERLMAGICK)/Makefile

if WITH_PERL_DYNAMIC

$(PERLMAKEFILE): perl-sources $(PERLMAKEMAKER)
	cd $(PERLMAGICK) && @PERL@ Makefile.PL $(PERL_MAKE_OPTIONS)

install-exec-perl: $(PERLMAKEFILE)
	( cd $(PERLMAGICK) && $(MAKE) CC='@CC@' && \
	$(MAKE) CC='@CC@' install && \
	$(MAKE) clean && rm -f  Makefile.old )

all-perl:

uninstall-exec-perl: $(PERLMAKEFILE)
	echo "Uninstall not supported for PerlMagick"

check-perl: $(PERLMAKEFILE)
	cd $(PERLMAGICK) && $(MAKE) CC='@CC@' test

else
if WITH_PERL_STATIC

PERLSTATICNAME=PerlMagick

$(PERLMAKEFILE): perl-sources $(PERLMAKEMAKER)
	cd $(PERLMAGICK) && @PERL@ Makefile.PL MAP_TARGET=$(PERLSTATICNAME) $(PERL_MAKE_OPTIONS)

$(PERLMAGICK)/$(PERLSTATICNAME): $(PERLMAKEFILE) magick/libGraphicsMagick.la
	( rm -f $(PERLMAGICK)/$(PERLSTATICNAME) ; cd $(PERLMAGICK) && $(MAKE) CC='@CC@' $(PERLSTATICNAME) )

all-perl: $(PERLMAGICK)/$(PERLSTATICNAME)

install-exec-perl: $(PERLMAGICK)/$(PERLSTATICNAME)
	( cd $(PERLMAGICK) && \
	$(MAKE) -f Makefile.aperl CC='@CC@' inst_perl MAP_TARGET=$(PERLSTATICNAME) )

uninstall-exec-perl:
	rm -f '@BIN_DIR@/$(PERLSTATICNAME)'

check-perl: $(PERLMAGICK)/$(PERLSTATICNAME)
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
uninstall-exec-perl:
check-perl:
clean-perl:
distclean-perl:
endif # WITH_PERL

if WITH_PERL
# txt2html requires perl.
TXT2HTMLTARGETS= \
	$(top_srcdir)/www/Changelog.html \
	$(top_srcdir)/www/INSTALL-mac.html \
	$(top_srcdir)/www/INSTALL-unix.html \
	$(top_srcdir)/www/INSTALL-vms.html \
	$(top_srcdir)/www/INSTALL-windows.html \
	$(top_srcdir)/www/NEWS.html \
	$(top_srcdir)/www/README.html
endif # WITH_PERL

all-local: all-perl @MAINTAINER_MODE_TRUE@ magick-version $(TXT2HTMLTARGETS)

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

# Automatically reconfigure libtool
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

# Build HTML version of ChangeLog
$(top_srcdir)/www/Changelog.html: $(top_srcdir)/ChangeLog
	@PERL@ $(top_srcdir)/scripts/txt2html  -t $(PACKAGE_NAME)' '$(PACKAGE_VERSION)' ChangeLog' < $(top_srcdir)/ChangeLog > $(top_srcdir)/www/Changelog.html

# Build HTML version of news
$(top_srcdir)/www/NEWS.html: $(top_srcdir)/NEWS
	@PERL@ $(top_srcdir)/scripts/txt2html -t $(PACKAGE_NAME)' '$(PACKAGE_VERSION)' News' < $(top_srcdir)/NEWS  > $(top_srcdir)/www/NEWS.html

# Build HTML version of README
$(top_srcdir)/www/README.html: $(top_srcdir)/README.txt
	@PERL@ $(top_srcdir)/scripts/txt2html -p 4 -pb 1 -t $(PACKAGE_NAME)' Install README' < $(top_srcdir)/README.txt > $(top_srcdir)/www/README.html

# Build HTML version of INSTALL-mac.txt
$(top_srcdir)/www/INSTALL-mac.html: $(top_srcdir)/INSTALL-unix.txt
	@PERL@ $(top_srcdir)/scripts/txt2html -s 55 -p 4 -pb 1 -t $(PACKAGE_NAME)' MacOS 9 Installation' < $(top_srcdir)/INSTALL-mac.txt > $(top_srcdir)/www/INSTALL-mac.html

# Build HTML version of INSTALL-unix.txt
$(top_srcdir)/www/INSTALL-unix.html: $(top_srcdir)/INSTALL-unix.txt
	@PERL@ $(top_srcdir)/scripts/txt2html -s 55 -p 4 -pb 1 -t $(PACKAGE_NAME)' Unix Installation' < $(top_srcdir)/INSTALL-unix.txt > $(top_srcdir)/www/INSTALL-unix.html

# Build HTML version of INSTALL-vms.txt
$(top_srcdir)/www/INSTALL-vms.html: $(top_srcdir)/INSTALL-vms.txt
	@PERL@ $(top_srcdir)/scripts/txt2html -s 55 -p 4 -pb 1 -t $(PACKAGE_NAME)' VMS Installation' < $(top_srcdir)/INSTALL-vms.txt > $(top_srcdir)/www/INSTALL-vms.html

# Build HTML version of INSTALL-windows.txt
$(top_srcdir)/www/INSTALL-windows.html: $(top_srcdir)/INSTALL-windows.txt
	@PERL@ $(top_srcdir)/scripts/txt2html -s 55 -p 4 -pb 1 -t $(PACKAGE_NAME)' Windows Installation' < $(top_srcdir)/INSTALL-windows.txt > $(top_srcdir)/www/INSTALL-windows.html

format_c_api_docs:
	@PERL@ $(top_srcdir)/scripts/format_c_api_docs -format html --srcdir $(top_srcdir)/magick --outdir $(top_srcdir)/www/api