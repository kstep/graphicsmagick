# Makefile.in generated automatically by automake 1.4a from Makefile.am

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

pkgdatadir = $(datadir)/libfpx
pkglibdir = $(libdir)/libfpx
pkgincludedir = $(includedir)/libfpx

top_builddir = ..

ACLOCAL = ${SHELL} /tmp/ImageMagick/fpx/missing --run aclocal
AUTOCONF = ${SHELL} /tmp/ImageMagick/fpx/missing --run autoconf
AUTOMAKE = ${SHELL} /tmp/ImageMagick/fpx/missing --run automake
AUTOHEADER = ${SHELL} /tmp/ImageMagick/fpx/missing --run autoheader

INSTALL = /usr/local/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_FLAG =
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :


build_alias = @build_alias@
build_triplet = sparc-sun-solaris2.6
host_alias = @host_alias@
host_triplet = sparc-sun-solaris2.6
target_alias = @target_alias@
target_triplet = sparc-sun-solaris2.6
AMDEP = 
AMTAR = gtar
AS = @AS@
AWK = gawk
CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
DEPDIR = .deps
DLLTOOL = @DLLTOOL@
EXEEXT = 
GCJ = @GCJ@
GCJFLAGS = @GCJFLAGS@
INCLTDL = -I${top_srcdir}/libltdl
LD = /usr/ccs/bin/ld
LIBLTDL = ${top_builddir}/libltdl/libltdlc.la
LIBSTDC_LDFLAGS = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAJOR_VERSION = 1
MAKEINFO = ${SHELL} /tmp/ImageMagick/fpx/missing --run makeinfo
MICRO_VERSION = 0
MINOR_VERSION = 2
OBJDUMP = @OBJDUMP@
OBJEXT = o
PACKAGE = libfpx
RANLIB = ranlib
STRIP = strip
VERSION = 1.2.0
install_sh = ./install-sh

# Don't require all the GNU mandated files


AUTOMAKE_OPTIONS = 1.4 foreign

#INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
#	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
#	-I$(top_srcdir)/oless/h
INCLUDES = \
	-I$(top_srcdir)/oless -I$(top_srcdir)/oless/h


# A convenience library
noinst_LTLIBRARIES = libOless.la

# Sources to compile into library
libOless_la_SOURCES = \
  ascii.cxx cdocfile.cxx chinst.cxx dffuncs.cxx dfiter.cxx dfstream.cxx \
  difat.cxx dir.cxx dirp.cxx docfile.cxx entry.cxx expdf.cxx expiter.cxx \
  expst.cxx fat.cxx funcs.cxx header.cxx iter.cxx mem.cxx msf.cxx \
  msfiter.cxx mstream.cxx page.cxx refilb.cxx rexpdf.cxx sstream.cxx \
  storage.cxx time.cxx vect.cxx wchar.c


# Additional files to distribute
EXTRA_DIST = dfhead.cxx exphead.cxx msfhead.cxx

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  ascii.hxx byteordr.hxx dbg.hxx dfbasis.hxx expdf.hxx expiter.hxx \
  expst.hxx iter.hxx logfile.hxx mread.hxx time.hxx


# Non-Automake subdirectories to distribute
DISTDIRS = doc h props test
subdir = oless
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(noinst_LTLIBRARIES)


DEFS =  -DWORDS_BIGENDIAN=1  -I. -I$(srcdir)
CPPFLAGS = -D_UNIX -D_POSIX_C_SOURCE=3
LDFLAGS = -R/usr/local/lib
LIBS = 
libOless_la_LDFLAGS = 
libOless_la_LIBADD = 
am_libOless_la_OBJECTS =  ascii.lo cdocfile.lo chinst.lo dffuncs.lo \
dfiter.lo dfstream.lo difat.lo dir.lo dirp.lo docfile.lo entry.lo \
expdf.lo expiter.lo expst.lo fat.lo funcs.lo header.lo iter.lo mem.lo \
msf.lo msfiter.lo mstream.lo page.lo refilb.lo rexpdf.lo sstream.lo \
storage.lo time.lo vect.lo wchar.lo
libOless_la_OBJECTS =  $(am_libOless_la_OBJECTS)
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CFLAGS = -g -O2
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
CXXFLAGS = -O0 -g -ansi
CXXLD = $(CXX)
CXXLINK = $(LIBTOOL) --mode=link $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
DIST_SOURCES =  $(libOless_la_SOURCES)
HEADERS =  $(noinst_HEADERS)

depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES =   $(DEPDIR)/ascii.Plo $(DEPDIR)/cdocfile.Plo \
$(DEPDIR)/chinst.Plo $(DEPDIR)/dffuncs.Plo $(DEPDIR)/dfiter.Plo \
$(DEPDIR)/dfstream.Plo $(DEPDIR)/difat.Plo $(DEPDIR)/dir.Plo \
$(DEPDIR)/dirp.Plo $(DEPDIR)/docfile.Plo $(DEPDIR)/entry.Plo \
$(DEPDIR)/expdf.Plo $(DEPDIR)/expiter.Plo $(DEPDIR)/expst.Plo \
$(DEPDIR)/fat.Plo $(DEPDIR)/funcs.Plo $(DEPDIR)/header.Plo \
$(DEPDIR)/iter.Plo $(DEPDIR)/mem.Plo $(DEPDIR)/msf.Plo \
$(DEPDIR)/msfiter.Plo $(DEPDIR)/mstream.Plo $(DEPDIR)/page.Plo \
$(DEPDIR)/refilb.Plo $(DEPDIR)/rexpdf.Plo $(DEPDIR)/sstream.Plo \
$(DEPDIR)/storage.Plo $(DEPDIR)/time.Plo $(DEPDIR)/vect.Plo \
$(DEPDIR)/wchar.Plo
DIST_COMMON =  $(noinst_HEADERS) Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

GZIP_ENV = --best
SOURCES = $(libOless_la_SOURCES)
OBJECTS = $(am_libOless_la_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .c .cxx .lo .o .obj
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --foreign oless/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-noinstLTLIBRARIES:

clean-noinstLTLIBRARIES:
	-test -z "$(noinst_LTLIBRARIES)" || rm -f $(noinst_LTLIBRARIES)

distclean-noinstLTLIBRARIES:

maintainer-clean-noinstLTLIBRARIES:

mostlyclean-compile:
	-rm -f *.o core *.core
	-rm -f *.$(OBJEXT)

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

libOless.la: $(libOless_la_OBJECTS) $(libOless_la_DEPENDENCIES)
	$(CXXLINK)  $(libOless_la_LDFLAGS) $(libOless_la_OBJECTS) $(libOless_la_LIBADD) $(LIBS)

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || etags $(ETAGS_ARGS) $$tags  $$unique $(LISP)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

include $(DEPDIR)/ascii.Plo
include $(DEPDIR)/cdocfile.Plo
include $(DEPDIR)/chinst.Plo
include $(DEPDIR)/dffuncs.Plo
include $(DEPDIR)/dfiter.Plo
include $(DEPDIR)/dfstream.Plo
include $(DEPDIR)/difat.Plo
include $(DEPDIR)/dir.Plo
include $(DEPDIR)/dirp.Plo
include $(DEPDIR)/docfile.Plo
include $(DEPDIR)/entry.Plo
include $(DEPDIR)/expdf.Plo
include $(DEPDIR)/expiter.Plo
include $(DEPDIR)/expst.Plo
include $(DEPDIR)/fat.Plo
include $(DEPDIR)/funcs.Plo
include $(DEPDIR)/header.Plo
include $(DEPDIR)/iter.Plo
include $(DEPDIR)/mem.Plo
include $(DEPDIR)/msf.Plo
include $(DEPDIR)/msfiter.Plo
include $(DEPDIR)/mstream.Plo
include $(DEPDIR)/page.Plo
include $(DEPDIR)/refilb.Plo
include $(DEPDIR)/rexpdf.Plo
include $(DEPDIR)/sstream.Plo
include $(DEPDIR)/storage.Plo
include $(DEPDIR)/time.Plo
include $(DEPDIR)/vect.Plo
include $(DEPDIR)/wchar.Plo

mostlyclean-depend:

clean-depend:

distclean-depend:
	-rm -rf $(DEPDIR)

maintainer-clean-depend:

CCDEPMODE = depmode=gcc

.c.o:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c -o $@ `test -f $< || echo '$(srcdir)/'`$<

.c.lo:
	source='$<' object='$@' libtool=yes \
	depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LTCOMPILE) -c -o $@ `test -f $< || echo '$(srcdir)/'`$<

.c.obj:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c -o $@ `cygpath -w $<`


CXXDEPMODE = depmode=gcc

.cxx.o:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) -c -o $@ `test -f $< || echo '$(srcdir)/'`$<

.cxx.lo:
	source='$<' object='$@' libtool=yes \
	depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' \
	$(CXXDEPMODE) $(depcomp) \
	$(LTCXXCOMPILE) -c -o $@ `test -f $< || echo '$(srcdir)/'`$<

.cxx.obj:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CXXDEPMODE) $(depcomp) \
	$(CXXCOMPILE) -c -o $@ `cygpath -w $<`


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pR $$d/$$file $(distdir) \
	    || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
	$(MAKE) $(AM_MAKEFLAGS) top_distdir="$(top_distdir)" distdir="$(distdir)" dist-hook
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am:
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am:
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES) $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_STRIP_FLAG=-s install
installdirs:


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
	-rm -f Makefile.in
mostlyclean-am:  mostlyclean-noinstLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags mostlyclean-depend \
		mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-noinstLTLIBRARIES clean-compile clean-libtool \
		clean-tags clean-depend clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-noinstLTLIBRARIES distclean-compile \
		distclean-libtool distclean-tags distclean-depend \
		distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-noinstLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-depend \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-noinstLTLIBRARIES distclean-noinstLTLIBRARIES \
clean-noinstLTLIBRARIES maintainer-clean-noinstLTLIBRARIES \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool tags mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags mostlyclean-depend \
distclean-depend clean-depend maintainer-clean-depend distdir info-am \
info dvi-am dvi check check-am installcheck-am installcheck \
install-exec-am install-exec install-data-am install-data install-am \
install uninstall-am uninstall all-redirect all-am all install-strip \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean

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

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
