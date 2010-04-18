# Copyright (C) 2004-2010 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Makefile for building PerlMagick
#
#

# If source files are missing, see if they can be obtained via VPATH
# and create a linked tree.
perl-sources:
	@if test -n "$(VPATH)" ; then \
	  echo "Linking PerlMagick Sources ..." ; \
	  imagemagick=`(cd $(VPATH) ; pwd)` && \
	  ( cd $(PERLMAGICK) && \
	    sh $$imagemagick/lndir.sh $$imagemagick/$(PERLMAGICK) ) \
	fi ; \
	touch perl-sources

# The substituted file Magick.pm needs to be copied to the PerlMagick
# source directory since it becomes part of the source distribution.
$(top_srcdir)/$(PERLMAGICK)/Magick.pm : $(PERLMAGICK)/Magick.pm
	@if test -n "$(VPATH)" ; then \
	    echo "Updating  $(PERLMAGICK)/Magick.pm ..." ; \
	    cp $(PERLMAGICK)/Magick.pm $(top_srcdir)/$(PERLMAGICK)/Magick.pm ; \
	fi


PERLMAGICK=PerlMagick
PERLMAGICK_MAINTAINER_TARGETS=$(top_srcdir)/$(PERLMAGICK)/Magick.pm

if WITH_PERL

PERLMAKEMAKER=$(PERLMAGICK)/Makefile.PL
PERLMAKEFILE=$(PERLMAGICK)/Makefile

PERLMAGICK_ALL_LOCAL_TARGETS = all-perl
#PERLMAGICK_INSTALL_EXEC_LOCAL_TARGETS = install-exec-perl
PERLMAGICK_INSTALL_DATA_LOCAL_TARGETS = 
#PERLMAGICK_UNINSTALL_LOCAL_TARGETS = uninstall-exec-perl
PERLMAGICK_CLEAN_LOCAL_TARGETS = clean-perl
PERLMAGICK_DISTCLEAN_LOCAL_TARGETS = clean-perl
PERLMAGICK_MAINTAINER_CLEAN_LOCAL_TARGETS = distclean-local
#PERLMAGICK_CHECK_LOCAL_TARGETS = check-perl

PERLMAGICK_TESTS = PerlMagick/PerlMagickCheck.sh

#PERLMAGICK_CHECKSCRIPTS = perl-build

if WITH_PERL_DYNAMIC

PERLMAGICK_CHECKSCRIPTS =

$(PERLMAKEFILE): perl-sources $(LIBMAGICK) $(PERLMAKEMAKER)
	cd $(PERLMAGICK) && @PERL@ Makefile.PL $(PERL_MAKE_OPTIONS)

install-exec-perl: $(PERLMAKEFILE)
	( cd $(PERLMAGICK) && $(MAKE) CC='@CC@' && \
	$(MAKE) CC='@CC@' install )

all-perl: perl-sources

uninstall-exec-perl: $(PERLMAKEFILE)
	echo "Uninstall not supported for PerlMagick"

check-perl: $(PERLMAKEFILE)
	cd $(PERLMAGICK) && $(abs_top_builddir)/rungm.sh $(MAKE) CC='@CC@' test

perl-build: $(PERLMAKEFILE)
	( cd $(PERLMAGICK) && $(MAKE) CC='@CC@' )

else
if WITH_PERL_STATIC

PERLSTATICNAME=PerlMagick

PERLMAGICK_CHECKSCRIPTS = perl-build

$(PERLMAKEFILE): perl-sources $(LIBMAGICK) $(PERLMAKEMAKER)
	cd $(PERLMAGICK) && @PERL@ Makefile.PL MAP_TARGET=$(PERLSTATICNAME) $(PERL_MAKE_OPTIONS) && $(MAKE) Makefile ; $(MAKE) Makefile

$(PERLMAGICK)/$(PERLSTATICNAME): $(LIBMAGICK) $(PERLMAKEFILE)
	( rm -f $(PERLMAGICK)/$(PERLSTATICNAME) ; cd $(PERLMAGICK) && $(MAKE) CC='@CC@' $(PERLSTATICNAME) ; $(MAKE) CC='@CC@' $(PERLSTATICNAME) )

all-perl: $(PERLMAGICK)/$(PERLSTATICNAME)

install-exec-perl: $(PERLMAGICK)/$(PERLSTATICNAME)
	rm -f "$(DESTDIR)$(BIN_DIR)/$(PERLSTATICNAME)"
	if test "x$(DESTDIR)" = "x" -o "$(PERL_SUPPORTS_DESTDIR)" = 'yes' ; then \
	  ( cd $(PERLMAGICK) && \
	    $(MAKE) -f Makefile.aperl CC='@CC@' inst_perl MAP_TARGET=$(PERLSTATICNAME) \
	    INSTALLBIN="$(BIN_DIR)" \
	  ) ; \
	else \
	  ( cd $(PERLMAGICK) && \
	    $(MAKE) -f Makefile.aperl CC='@CC@' inst_perl MAP_TARGET=$(PERLSTATICNAME) \
	    INSTALLBIN="$(DESTDIR)$(BIN_DIR)" PREFIX="$(DESTDIR)$(prefix)" \
	  ) ; \
	fi

uninstall-exec-perl:
	rm -f '$(DESTDIR)$(BIN_DIR)/$(PERLSTATICNAME)'

check-perl: $(PERLMAGICK)/$(PERLSTATICNAME)
	cd $(PERLMAGICK) && $(abs_top_builddir)/rungm.sh $(MAKE) -f Makefile.aperl CC='@CC@' test

perl-build: $(PERLMAGICK)/$(PERLSTATICNAME)

endif # WITH_PERL_STATIC
endif # WTIH_PERL_DYNAMIC


clean-perl:
	(cd $(PERLMAGICK) && \
	( if test -f Makefile.old ; then $(MAKE) -f Makefile.old CC='@CC@' clean ; fi ) ; \
	( if test -f Makefile ; then $(MAKE) CC='@CC@' clean ; fi ) ; \
	( if test -f Makefile ; then $(MAKE) CC='@CC@' clean ; fi ) ; \
	rm -f Makefile.old PerlMagick ; \
	rm -f t/output* t/jng/*_tmp.jng t/*/output* )
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
