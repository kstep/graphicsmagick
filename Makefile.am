# Copyright (C) 2004-2014 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Top-Level Makefile for building GraphicsMagick
#
#

# Don't require all the GNU mandated files
# Use XZ_OPT (a environment variable) to tune xz compression levels (e.g. XZ_OPT=-8e)
# nostdinc 
AUTOMAKE_OPTIONS = 1.12 subdir-objects nostdinc color-tests parallel-tests dist-bzip2 dist-xz foreign dist-lzip

topincludedir = @includedir@/GraphicsMagick

# CPPFLAGS required by all C/C++ code
AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)

ACLOCAL_AMFLAGS = -I m4

MODULECOMMONFLAGS = -no-undefined -export-symbols-regex ".*" -shared -module -avoid-version
MODULECOMMONCPPFLAGS = $(AM_CPPFLAGS)

# Options to pass when running configure in the distcheck target.
#
# We want to preserve user-provided option variables so the same
# compiler, headers, and libraries are used as for a normal build.
DISTCHECK_CONFIGURE_FLAGS=$(DISTCHECK_CONFIG_FLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = \
	$(MAGICKPP_CLEANFILES) \
	$(UTILITIES_CLEANFILES) \
	$(TESTS_CLEANFILES) \
	$(WAND_CLEANFILES)

bin_PROGRAMS = \
	$(UTILITIES_PGMS)

# Binary scripts
bin_SCRIPTS = \
	$(MAGICK_BIN_SCRPTS) \
	$(MAGICKPP_SCRPTS) \
	$(WAND_BIN_SCRPTS)

include_HEADERS =

# Headers which are not installed but which are distributed
noinst_HEADERS = \
	$(MAGICK_NOINST_HDRS)

if WIN32_NATIVE_BUILD
SRCDIR='$(shell @WinPathScript@ $(srcdir)/)'
else
SRCDIR="$(srcdir)/"
endif

# Run the tests with a proper shell detected at configure time.
LOG_COMPILER = $(SHELL)

SH_LOG_COMPILER = $(LOG_COMPILER)
TAP_LOG_COMPILER = $(LOG_COMPILER)

# Test extensions
TEST_EXTENSIONS = .sh .tap

# Tests with .tap extensions use the TAP protocol and TAP driver
TAP_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
	$(top_srcdir)/scripts/tap-driver.sh

# Environment parameters to be used during tests
TESTS_ENVIRONMENT = \
	MAKE="$(MAKE)" \
	MAKEFLAGS="$(MAKEFLAGS)" \
	MEMCHECK="$(MEMCHECK)"

# Tests to run
TESTS = \
	$(PERLMAGICK_TESTS) \
	$(TESTS_TESTS) \
	$(MAGICKPP_TESTS) \
	$(WAND_TESTS) \
	$(UTILITIES_TESTS)

# Tests which are expected to fail
XFAIL_TESTS = \
	$(TESTS_XFAIL_TESTS) \
	$(MAGICKPP_XFAIL_TESTS) \
	$(UTILITIES_XFAIL_TESTS)

# Be sure to list CHANGELOGS in reverse chronological order in case we
# should want to concatenate them.
CHANGELOGS = \
	ChangeLog \
	ChangeLog.2013 \
	ChangeLog.2012 \
	ChangeLog.2011 \
	ChangeLog.2010 \
	ChangeLog.2009 \
	ChangeLog.2008 \
	ChangeLog.2007 \
	ChangeLog.2006 \
	ChangeLog.2005 \
	ChangeLog.2004 \
	ChangeLog.2003 \
	ChangeLog.2002 \
	ChangeLog.2001

TOP_EXTRA_DIST = \
	$(CHANGELOGS) \
	Copyright.txt \
	NEWS.txt \
	README.txt \
	TODO.txt \
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
	$(WAND_EXTRA_DIST) \
	$(WWWW_EXTRA_DIST) \
	$(WWWAPI_EXTRA_DIST) \
	$(WWWWANDAPI_EXTRA_DIST)

lib_LTLIBRARIES = $(LIBMAGICK) $(LIBMAGICKPP) $(LIBWAND)

check_PROGRAMS = \
	$(TESTS_CHECK_PGRMS) \
	$(MAGICKPP_CHECK_PGRMS) \
	$(WAND_CHECK_PGRMS)

check_SCRIPTS = \
	$(PERLMAGICK_CHECKSCRIPTS)

AM_LDFLAGS =
noinst_LTLIBRARIES =
EXTRA_LTLIBRARIES =
BUILT_SOURCES =
MOSTLYCLEANFILES =

include config/Makefile.am
include coders/Makefile.am
include magick/Makefile.am
include filters/Makefile.am
include Magick++/Makefile.am
include tests/Makefile.am
include wand/Makefile.am
include www/Makefile.am
include www/api/Makefile.am
include www/wand/Makefile.am
include utilities/Makefile.am
include PerlMagick/Makefile.am

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

# Files to install in the package's doc directory
doc_DATA = \
	Copyright.txt \
	$(CHANGELOGS) \
	NEWS.txt

if MAINTAINER_MODE
MAINTAINER_TARGETS = \
	magick-version \
	GraphicsMagick.spec \
	$(PERLMAGICK_MAINTAINER_TARGETS) \
	$(WWW_MAINTAINER_TARGETS) \
	$(WWWAPI_MAINTAINER_TARGETS) \
	$(WWWWANDAPI_MAINTAINER_TARGETS)

# Target to build just the web pages.
.PHONY: www
www : $(WWW_MAINTAINER_TARGETS) $(WWWAPI_MAINTAINER_TARGETS) $(WWWWANDAPI_MAINTAINER_TARGETS)

endif

all-local: $(MAGICKPP_LOCAL_TARGETS) \
	$(PERLMAGICK_ALL_LOCAL_TARGETS) $(MAINTAINER_TARGETS)

install-exec-local: $(PERLMAGICK_INSTALL_EXEC_LOCAL_TARGETS) \
	$(UTILITIES_INSTALL_EXEC_LOCAL_TARGETS)

install-data-local: $(MAGICK_INSTALL_DATA_LOCAL_TARGETS) \
	$(PERLMAGICK_INSTALL_DATA_LOCAL_TARGETS) \
	$(HTML_INSTALL_DATA_TARGETS)

uninstall-local: $(MAGICK_UNINSTALL_LOCAL_TARGETS) \
	$(PERLMAGICK_UNINSTALL_LOCAL_TARGETS) \
	$(HTML_UNINSTALL_DATA_TARGETS) \
	$(UTILITIES_UNINSTALL_LOCAL_TARGETS)

clean-local: $(PERLMAGICK_CLEAN_LOCAL_TARGETS)

distclean-local: $(PERLMAGICK_DISTCLEAN_LOCAL_TARGETS)

maintainer-clean-local: $(PERLMAGICK_MAINTAINER_CLEAN_LOCAL_TARGETS)

check-local: $(PERLMAGICK_CHECK_LOCAL_TARGETS)

# drd: valgrind's newer thread error detector
drd:
	$(MAKE) MEMCHECK='valgrind --tool=drd --check-stack-var=yes --var-info=yes \
	  --quiet $(VALGRIND_EXTRA_OPTS)' check

# helgrind: valgrind's older thread error detector
helgrind:
	$(MAKE) MEMCHECK='valgrind --tool=helgrind --error-exitcode=2 --quiet \
	$(VALGRIND_EXTRA_OPTS)' check

# memcheck: valgrind's memory access checker.
# The suppressions which come with valgrind are usually insufficient
# to handle certain pthread library aspects and gomp and so errors
# will be reported which are unrelated to GraphicsMagick.  When first
# starting with a new system (or after a major system update), it is
# good to execute the test suite like 'make memcheck
# VALGRIND_EXTRA_OPTS=--gen-suppressions=all' to create valgrind
# suppression entries in the test log.  Tell valgrind about the
# suppressions by creating a .valgrindrc file with content like:
#   --memcheck:suppressions=mysupp.supp
memcheck:
	$(MAKE) MEMCHECK='valgrind --tool=memcheck --leak-check=full --read-var-info=yes \
	--error-exitcode=2 --track-origins=yes --num-callers=12 \
	--quiet $(VALGRIND_EXTRA_OPTS)' check

# ptrcheck: valgrind's experimental pointer checking tool.
ptrcheck:
	$(MAKE) MEMCHECK='valgrind --tool=exp-ptrcheck --quiet $(VALGRIND_EXTRA_OPTS)' check

# Non-Automake subdirectories to distribute
DISTDIRS = locale scripts www PerlMagick TclMagick
dist-hook:
	( \
	  builddir=`pwd` ; \
	  cd $(srcdir) && \
	  ( \
	    for dir in $(DISTDIRS) ; do \
	      find $$dir -depth -print | egrep -v '(~$$)|(/\.hg)|(/\.#)|(/\.deps)|(\.pyc)' \
	        | cpio -pdum $$builddir/$(distdir) 2> /dev/null ; \
	    done \
	  ) \
	)

#
# Additional install rules
#

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

magick/version.h: $(top_srcdir)/ChangeLog $(top_srcdir)/version.sh


# Automatically reconfigure libtool
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

#
# Build Windows source Zip and 7Zip balls
#
if HAS_ZIP
DIST_WINDOWS_SRC_ZIP=$(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-windows.zip
else
DIST_WINDOWS_SRC_ZIP=
endif
if HAS_P7ZIP
DIST_WINDOWS_SRC_7ZIP=$(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-windows.7z
else
DIST_WINDOWS_SRC_7ZIP=
endif
$(DIST_WINDOWS_SRC_7ZIP) windows-dist:
	if test -d $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM) ; then \
	  chmod -R u+w $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM) ; \
	  rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM) ; \
	fi
	hg --repository $(top_srcdir) archive --rev $(HG_BRANCH_TAG) $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)
if HAS_P7ZIP
	rm -f $(DIST_WINDOWS_SRC_7ZIP)
	$(P7ZIP) a -t7z -mx=9 $(DIST_WINDOWS_SRC_7ZIP) $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)
	chmod 644 $(DIST_WINDOWS_SRC_7ZIP)
endif # HAS_P7ZIP
	rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)

#
# RPM build support
#
if HAS_RPM

DIST_ARCHIVE_SRPM=$(distdir)-1.src.rpm
.PHONY: srpm
$(DIST_ARCHIVE_SRPM) srpm: $(distdir).tar.bz2
	rm -f $(DIST_ARCHIVE_SRPM)
	$(RPM) --define="_sourcedir `pwd`" --define="_srcrpmdir `pwd`" --nodeps -bs GraphicsMagick.spec
	@echo ==============================================================
	@echo $(DIST_ARCHIVE_SRPM) is ready for distribution.
	@echo ==============================================================

RPMDIR=rpmbuild
RPMARCH=$(MAGICK_TARGET_CPU)

DIST_ARCHIVE_RPM= \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-c++-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-c++-devel-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-devel-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm \
	$(RPMDIR)/$(RPMARCH)/$(PACKAGE_NAME)-perl-$(PACKAGE_VERSION)$(PACKAGE_VERSION_ADDENDUM)-1.$(RPMARCH).rpm

.PHONY: rpm
rpm:
	rm -rf $(RPMDIR)
	$(MKDIR_P) $(RPMDIR)
	$(MKDIR_P) $(RPMDIR)/BUILD
	$(MKDIR_P) $(RPMDIR)/RPMS
	$(RPM) --define="_sourcedir `pwd`" --define="_rpmdir `pwd`/$(RPMDIR)/RPMS" \
	  --define="_builddir `pwd`/$(RPMDIR)/BUILD" --define='quant $(QuantumDepth)' \
	  --nodeps -bb GraphicsMagick.spec
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
SNAPSHOT_DIRECTORY=/ftp/pub/GraphicsMagick/snapshots
snapshot: distcheck
	$(MAKE) $(DIST_ARCHIVE_SRPM)
	$(MAKE) $(DIST_WINDOWS_SRC_7ZIP)
	mv $(DIST_ARCHIVES) $(DIST_WINDOWS_SRC_7ZIP) $(DIST_ARCHIVE_SRPM) $(SNAPSHOT_DIRECTORY)/
	cp $(top_srcdir)/ChangeLog $(SNAPSHOT_DIRECTORY)/ChangeLog.txt
	cp $(top_srcdir)/www/Changelog.html $(SNAPSHOT_DIRECTORY)/ChangeLog.html

COVERITY_EMAIL=bfriesen@simple.dallas.tx.us
COVERITY_TARBALL=GraphicsMagick.xz
COVERITY_VERSION=snapshot-@PACKAGE_CHANGE_DATE@
COVERITY_DESCRIPTION='Ubuntu Q@QuantumDepth@'
coverity:
	$(MAKE) clean
	cov-build --dir cov-int $(MAKE)
	tar caf $(COVERITY_TARBALL) cov-int
	curl --limit-rate 50K \
	--form token=`cat $$HOME/.coverity_key_gm` \
	--form email=$(COVERITY_EMAIL) \
	--form file=@$(COVERITY_TARBALL) \
	--form version=$(COVERITY_VERSION) \
	--form description=$(COVERITY_DESCRIPTION) \
	https://scan.coverity.com/builds?project=GraphicsMagick
	$(RM) $(COVERITY_TARBALL)

# Rules for a profiled build (according to Paolo Bonzini <bonzini@gnu.org>)
#.PHONY: profile
#profile:
#	$(MAKE) clean
#	$(MAKE) CFLAGS="$(CFLAGS) -fprofile-generate"
#	$(MAKE) check
#	$(MAKE) mostlyclean
#	$(MAKE) CFLAGS="$(CFLAGS) -fprofile-use"
#
#CLEANFILES = *.gcda *.gcno
