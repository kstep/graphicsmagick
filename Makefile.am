# Copyright (C) 2004 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Top-Level Makefile for building GraphicsMagick
#
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.9 subdir-objects dist-zip dist-bzip2 foreign

topincludedir = @includedir@/GraphicsMagick

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/Magick++/lib

# It seems that AC_CONFIG_MACRO_DIR([m4]) in configure.ac should cause
# Automake to pass aclocal the right flags, but here is some more
# positive enforcement.
ACLOCAL_AMFLAGS = -I .

MODULECOMMONFLAGS = -no-undefined -module -avoid-version

# Options to pass when running configure in the distcheck target.
#
# We want to preserve user-provided option variables so the same
# compiler, headers, and libraries are used as for a normal build.
DISTCHECK_CONFIGURE_FLAGS=$(DISTCHECK_CONFIG_FLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = \
	$(LTDL_CLEANFILES) \
	$(MAGICKPP_CLEANFILES) \
	$(UTILITIES_CLEANFILES) \
	$(TESTS_CLEANFILES) \
	$(WAND_CLEANFILES)

BUILT_SOURCES = \
	$(MAGICK_BUILT_SRCS)

bin_PROGRAMS = \
	$(UTILITIES_PGMS)

# Binary scripts
bin_SCRIPTS = \
	$(MAGICK_BIN_SCRPTS) \
	$(MAGICKPP_SCRPTS) \
	$(WAND_BIN_SCRPTS)

include_HEADERS =
	$(LTDL_INSTALLHEADERS)

# Headers which are not installed but which are distributed
noinst_HEADERS = \
	$(LIBLTDL_NOINST_HDRS) \
	$(MAGICK_NOINST_HDRS)

if WIN32_NATIVE_BUILD
SRCDIR='$(shell @WinPathScript@ $(srcdir)/)'
else
SRCDIR="$(srcdir)/"
endif

RUNENV = $(SHELL) `pwd`/rungm.sh

# Environment parameters to be used during tests
TESTS_ENVIRONMENT = \
        RUNENV="$(RUNENV)" \
	SRCDIR=$(SRCDIR) \
	GM="$(RUNENV) `pwd`/utilities/gm"

# Tests to run
TESTS = \
	$(TESTS_TESTS) \
	$(MAGICKPP_TESTS) \
	$(UTILITIES_TESTS)

# Tests which are expected to fail
XFAIL_TESTS = \
	$(TESTS_XFAIL_TESTS)

TOP_EXTRA_DIST = \
	ChangeLog \
	Copyright.txt \
	INSTALL-mac.txt \
	INSTALL-unix.txt \
	INSTALL-vms.txt \
	INSTALL-windows.txt \
	Magickshr.opt \
	Make.com \
	NEWS \
	PLATFORMS.txt \
	README.txt \
	TODO.txt \
	index.html \
	lndir.sh \
	version.sh \
	winpath.sh


# Additional files to distribute
EXTRA_DIST = \
	$(TOP_EXTRA_DIST) \
	$(CONFIG_EXTRA_DIST) \
	$(MAGICK_EXTRA_DIST) \
	$(MAGICKPP_EXTRA_DIST) \
	$(TESTS_EXTRA_DIST) \
	$(UTILITIES_EXTRA_DIST) \
	$(WAND_EXTRA_DIST)


lib_LTLIBRARIES = $(LIBLTDL) $(LIBMAGICK) $(LIBMAGICKPP) $(LIBWAND)

check_PROGRAMS = \
	$(TESTS_CHECK_PGRMS) \
	$(MAGICKPP_CHECK_PGRMS) \
	$(WAND_CHECK_PGRMS)

include $(srcdir)/config/Makefile.am
include $(srcdir)/ltdl/Makefile.am
include $(srcdir)/coders/Makefile.am
include $(srcdir)/magick/Makefile.am
include $(srcdir)/filters/Makefile.am
include $(srcdir)/Magick++/Makefile.am
include $(srcdir)/tests/Makefile.am
include $(srcdir)/wand/Makefile.am
include $(srcdir)/utilities/Makefile.am
include $(srcdir)/PerlMagick/Makefile.am

# Pkgconfig directory
pkgconfigdir = $(libdir)/pkgconfig

# Files to install in Pkgconfig directory
pkgconfig_DATA = \
	$(MAGICK_PKGCONFIG) \
	$(MAGICKPP_PKGCONFIG) \
	$(WAND_PKGCONFIG)

# Manual pages to install
man_MANS = \
	$(MAGICK_MANS) \
	$(MAGICKPP_MANS) \
	$(UTILITIES_MANS) \
	$(WAND_MANS)

# Architecture independent data files installed in the package's data directory
docdir = $(MagickSharePath)
doc_DATA = \
	Copyright.txt \
	ChangeLog \
	NEWS

if MAINTAINER_MODE
MAINTAINER_TARGETS = magick-version $(TXT2HTML_TARGETS)
endif

all-local: $(MAGICKPP_LOCAL_TARGETS) $(PERLMAGICK_ALL_LOCAL_TARGETS) $(MAINTAINER_TARGETS)

install-exec-local: $(PERLMAGICK_INSTALL_EXEC_LOCAL_TARGETS)

install-data-local: $(MAGICK_INSTALL_DATA_LOCAL_TARGETS) $(PERLMAGICK_INSTALL_DATA_LOCAL_TARGETS) $(HTML_INSTALL_DATA_TARGETS)

uninstall-local: $(MAGICK_UNINSTALL_LOCAL_TARGETS) $(PERLMAGICK_UNINSTALL_LOCAL_TARGETS) $(HTML_UNINSTALL_DATA_TARGETS)

clean-local: $(PERLMAGICK_CLEAN_LOCAL_TARGETS)

distclean-local: $(PERLMAGICK_DISTCLEAN_LOCAL_TARGETS)

maintainer-clean-local: $(PERLMAGICK_MAINTAINER_CLEAN_LOCAL_TARGETS)

check-local: $(PERLMAGICK_CHECK_LOCAL_TARGETS)

# Non-Automake subdirectories to distribute
DISTDIRS = images locale scenes scripts www PerlMagick TclMagick
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
pkgdocdir = $(MagickSharePath)
DOCDIRS = images www www/api www/Magick++
HTML_INSTALL_DATA_TARGETS = install-data-html
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
HTML_UNINSTALL_DATA_TARGETS = uninstall-data-html
uninstall-data-html:
	rm -f $(DESTDIR)$(pkgdocdir)/index.html
	for dir in $(DOCDIRS) ; do \
	  rm -f -r $(DESTDIR)$(pkgdocdir)/$$dir ; \
	done

# Ensure that version.h at $(srcdir)/magick/version.h is kept up to date.
magick-version: magick/version.h
	@if test -f "$(srcdir)/VisualMagick/installer/inc/version.isx.in" ; then \
	  ./config.status --file="$(srcdir)/VisualMagick/installer/inc/version.isx" 2> /dev/null ; \
	fi
	@if test -n "$(VPATH)" ; then \
	  cmp magick/version.h $(srcdir)/magick/version.h > /dev/null ; \
          if test $$? -eq 1 ; then \
	    echo "Updating $(srcdir)/magick/version.h ..."; \
	    cp magick/version.h $(srcdir)/magick/version.h ; \
	  fi ; \
	fi ; \
	touch magick-version

${srcdir}/configure: ${srcdir}/version.sh

if WITH_PERL
# txt2html requires perl.
TXT2HTML_TARGETS = \
	$(top_srcdir)/www/AUTHORS.html \
	$(top_srcdir)/www/Changelog.html \
	$(top_srcdir)/www/INSTALL-mac.html \
	$(top_srcdir)/www/INSTALL-unix.html \
	$(top_srcdir)/www/INSTALL-vms.html \
	$(top_srcdir)/www/INSTALL-windows.html \
	$(top_srcdir)/www/NEWS.html \
	$(top_srcdir)/www/README.html
endif # WITH_PERL

# Automatically reconfigure libtool
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

# Build HTML version of AUTHORS
$(top_srcdir)/www/AUTHORS.html: $(top_srcdir)/AUTHORS
	@PERL@ $(top_srcdir)/scripts/txt2html -p 4 -pb 1 -t $(PACKAGE_NAME)' Authors' < $(top_srcdir)/AUTHORS > $(top_srcdir)/www/AUTHORS.html

# Build HTML version of ChangeLog
$(top_srcdir)/www/Changelog.html: $(top_srcdir)/ChangeLog
	@PERL@ $(top_srcdir)/scripts/txt2html  -t $(PACKAGE_NAME)' '$(PACKAGE_VERSION)' ChangeLog' < $(top_srcdir)/ChangeLog > $(top_srcdir)/www/Changelog.html

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

# Build HTML version of news
$(top_srcdir)/www/NEWS.html: $(top_srcdir)/NEWS
	@PERL@ $(top_srcdir)/scripts/txt2html -t $(PACKAGE_NAME)' '$(PACKAGE_VERSION)' News' < $(top_srcdir)/NEWS  > $(top_srcdir)/www/NEWS.html

# Build HTML version of README
$(top_srcdir)/www/README.html: $(top_srcdir)/README.txt
	@PERL@ $(top_srcdir)/scripts/txt2html -p 4 -pb 1 -t $(PACKAGE_NAME)' Install README' < $(top_srcdir)/README.txt > $(top_srcdir)/www/README.html

# Format C API documentation
format_c_api_docs:
	@PERL@ $(top_srcdir)/scripts/format_c_api_docs -format html --srcdir $(top_srcdir)/magick --outdir $(top_srcdir)/www/api

#
# Build a Windows source Zip-ball
#
DIST_WINDOWS_SRC_ZIP=$(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-windows.zip
$(DIST_WINDOWS_SRC_ZIP) windows-dist:
	if test -d $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM) ; then \
	  chmod -R u+w $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM) ; \
	  rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM) ; \
	fi
	rm -f $(DIST_WINDOWS_SRC_ZIP)
	cvs -d ':ext:anoncvs@cvs.graphicsmagick.org:/GraphicsMagick' export -r $(CVS_BRANCH_TAG) -d $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM) GraphicsMagick-NT
	zip -r -9 -q  $(DIST_WINDOWS_SRC_ZIP) $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)
	rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)

#
# RPM build support
#
if HAS_RPM

DIST_ARCHIVE_SRPM=$(distdir)-1.src.rpm
PHONY: srpm
$(DIST_ARCHIVE_SRPM) srpm: $(distdir).tar.bz2
	rm -f $(DIST_ARCHIVE_SRPM)
	$(RPM) --define="_sourcedir `pwd`" --define="_srcrpmdir `pwd`" --nodeps -bs GraphicsMagick.spec
	@echo ==============================================================
	@echo $(DIST_ARCHIVE_SRPM) is ready for distribution.
	@echo ==============================================================

RPMDIR=redhat
RPMARCH=i386

DIST_ARCHIVE_RPM= \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-c++-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-c++-devel-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-devel-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-perl-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm

PHONY: rpm
rpm:
	rm -rf $(RPMDIR)
	mkdir $(RPMDIR)
	mkdir $(RPMDIR)/BUILD
	mkdir $(RPMDIR)/RPMS
	$(MAKE) distcheck
	$(RPM) --define="_sourcedir `pwd`" --define="_rpmdir `pwd`/$(RPMDIR)/RPMS" --define="_builddir `pwd`/$(RPMDIR)/BUILD" --nodeps -bb GraphicsMagick.spec
	rm -rf rpmbuild
	@echo ==============================================================
	@echo $(DIST_ARCHIVE_RPM) is ready for distribution.
	@echo ==============================================================

else
DIST_ARCHIVE_RPM=
endif # HAS_RPM

#
# Build a validated snapshot release and move to the snapshots directory.
#
snapshot: distcheck
	$(MAKE) $(DIST_ARCHIVE_SRPM)
	$(MAKE) $(DIST_WINDOWS_SRC_ZIP)
	mv $(DIST_ARCHIVES) $(DIST_WINDOWS_SRC_ZIP) $(DIST_ARCHIVE_SRPM) /ftp/pub/GraphicsMagick/snapshots/
