# Copyright (C) 2004-2012 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building GraphicsMagick utilities
#
#

UTILITIES_PGMS = \
	utilities/gm

utilities_gm_LDADD	= $(LIBMAGICK)
utilities_gm_LDFLAGS	= $(LDFLAGS)
utilities_gm_SOURCES	= utilities/gm.c


UTILITIES_XFAIL_TESTS = 

# Tests to run
UTILITIES_TESTS = \
	utilities/tests/effects.tap \
	utilities/tests/pipe.tap \
	utilities/tests/hald-clut.tap \
	utilities/tests/help.tap \
	utilities/tests/icc-transform.tap \
	utilities/tests/identify.tap \
	utilities/tests/list.tap \
	utilities/tests/montage.tap \
	utilities/tests/msl_composite.tap \
	utilities/tests/preview.tap

utilities/tests/montage.log : \
	utilities/tests/effects.tap

UTILITIES_MANS = \
	utilities/gm.1 \
	utilities/miff.4 \
	utilities/quantize.5

UTILITIES_EXTRA_DIST = \
	$(UTILITIES_MANS) \
	$(UTILITIES_TESTS) \
	utilities/tests/BetaRGB.icc \
	utilities/tests/common.sh \
	utilities/tests/sunrise.jpg \
	utilities/tests/sunrise.miff

UTILITIES_CLEANFILES = \
	utilities/tests/*_out.miff \
	utilities/tests/*_out.pnm \
	utilities/tests/*_out.txt \
	utilities/tests/demo*.miff \
	utilities/tests/gm.core \
	utilities/tests/core

# Install/Uninstall ImageMagick compatibility links
if MAGICK_COMPAT
MAGICKPROGRAMS=animate compare composite conjure convert display identify import mogrify montage
UTILITIES_INSTALL_EXEC_LOCAL_TARGETS=install-exec-local-utilities
install-exec-local-utilities:
	$(mkdir_p) $(DESTDIR)$(bindir)
	cd $(DESTDIR)$(bindir) ; \
	gm=`echo "gm" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	for name in $(MAGICKPROGRAMS) ; \
	do \
	  target=`echo "$$name" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  rm -f $$target ; \
	  $(LN_S) $$gm $$target ; \
	done

UTILITIES_UNINSTALL_LOCAL_TARGETS=uninstall-local-utilities
uninstall-local-utilities:
	cd $(DESTDIR)$(bindir) ; \
	for name in $(MAGICKPROGRAMS) ; \
	do \
	  target=`echo "$$name" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  rm -f $$target ; \
	done
endif # MAGICK_COMPAT
