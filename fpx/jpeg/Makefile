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

ACLOCAL = ${SHELL} /home/bfriesen/src/graphics/fpx_port/fpx/missing --run aclocal
AUTOCONF = ${SHELL} /home/bfriesen/src/graphics/fpx_port/fpx/missing --run autoconf
AUTOMAKE = ${SHELL} /home/bfriesen/src/graphics/fpx_port/fpx/missing --run automake
AUTOHEADER = ${SHELL} /home/bfriesen/src/graphics/fpx_port/fpx/missing --run autoheader

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
MAKEINFO = ${SHELL} /home/bfriesen/src/graphics/fpx_port/fpx/missing --run makeinfo
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

INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
	-I$(top_srcdir)/oless/h


# A convenience library
noinst_LTLIBRARIES = libJpeg.la

# Sources to compile into library
libJpeg_la_SOURCES = \
  chen_dct.c dbuffer.c decoder.c dectile.c dhuff.c djpeg.c dllmain.c \
  dparser.c dscan.c ebuffer.c ejpeg.c enctile.c eparser.c fpxmem.c \
  huffman.c mcu2rast.c process.c public.c tables.c win_dct.c winograd.c


# Additional files to distribute
EXTRA_DIST = 

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  chen_dct.h dbuffer.h dct.h decoder.h dectile.h dhuff.h djpeg.h \
  dparser.h ebuffer.h ejpeg.h encoder.h enctile.h eparser.h fpxjpeg.h \
  fpxmem.h huffman.h jpegconf.h process.h public.h tables.h win_dct.h \
  winograd.h

subdir = jpeg
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(noinst_LTLIBRARIES)


DEFS =  -DWORDS_BIGENDIAN=1  -I. -I$(srcdir)
CPPFLAGS = -D_UNIX 
LDFLAGS = -R/usr/local/lib
LIBS = 
libJpeg_la_LDFLAGS = 
libJpeg_la_LIBADD = 
am_libJpeg_la_OBJECTS =  chen_dct.lo dbuffer.lo decoder.lo dectile.lo \
dhuff.lo djpeg.lo dllmain.lo dparser.lo dscan.lo ebuffer.lo ejpeg.lo \
enctile.lo eparser.lo fpxmem.lo huffman.lo mcu2rast.lo process.lo \
public.lo tables.lo win_dct.lo winograd.lo
libJpeg_la_OBJECTS =  $(am_libJpeg_la_OBJECTS)
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CFLAGS = -g -O2
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
DIST_SOURCES =  $(libJpeg_la_SOURCES)
HEADERS =  $(noinst_HEADERS)

depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES =   $(DEPDIR)/chen_dct.Plo $(DEPDIR)/dbuffer.Plo \
$(DEPDIR)/decoder.Plo $(DEPDIR)/dectile.Plo $(DEPDIR)/dhuff.Plo \
$(DEPDIR)/djpeg.Plo $(DEPDIR)/dllmain.Plo $(DEPDIR)/dparser.Plo \
$(DEPDIR)/dscan.Plo $(DEPDIR)/ebuffer.Plo $(DEPDIR)/ejpeg.Plo \
$(DEPDIR)/enctile.Plo $(DEPDIR)/eparser.Plo $(DEPDIR)/fpxmem.Plo \
$(DEPDIR)/huffman.Plo $(DEPDIR)/mcu2rast.Plo $(DEPDIR)/process.Plo \
$(DEPDIR)/public.Plo $(DEPDIR)/tables.Plo $(DEPDIR)/win_dct.Plo \
$(DEPDIR)/winograd.Plo
DIST_COMMON =  $(noinst_HEADERS) Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

GZIP_ENV = --best
SOURCES = $(libJpeg_la_SOURCES)
OBJECTS = $(am_libJpeg_la_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .c .lo .o .obj
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --foreign jpeg/Makefile

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

libJpeg.la: $(libJpeg_la_OBJECTS) $(libJpeg_la_DEPENDENCIES)
	$(LINK)  $(libJpeg_la_LDFLAGS) $(libJpeg_la_OBJECTS) $(libJpeg_la_LIBADD) $(LIBS)

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

include $(DEPDIR)/chen_dct.Plo
include $(DEPDIR)/dbuffer.Plo
include $(DEPDIR)/decoder.Plo
include $(DEPDIR)/dectile.Plo
include $(DEPDIR)/dhuff.Plo
include $(DEPDIR)/djpeg.Plo
include $(DEPDIR)/dllmain.Plo
include $(DEPDIR)/dparser.Plo
include $(DEPDIR)/dscan.Plo
include $(DEPDIR)/ebuffer.Plo
include $(DEPDIR)/ejpeg.Plo
include $(DEPDIR)/enctile.Plo
include $(DEPDIR)/eparser.Plo
include $(DEPDIR)/fpxmem.Plo
include $(DEPDIR)/huffman.Plo
include $(DEPDIR)/mcu2rast.Plo
include $(DEPDIR)/process.Plo
include $(DEPDIR)/public.Plo
include $(DEPDIR)/tables.Plo
include $(DEPDIR)/win_dct.Plo
include $(DEPDIR)/winograd.Plo

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


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
