# Copyright (C) 2008 GraphicsMagick Group

# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building HTML files
#

WWWDIR=$(top_srcdir)/www

# Install HTML files
DOCDIRS = www www/api www/images www/Magick++
HTML_INSTALL_DATA_TARGETS = install-data-html
#	$(INSTALL_DATA) $(srcdir)/index.html $(DESTDIR)$(htmldir)
install-data-html:
	$(mkinstalldirs) $(DESTDIR)$(htmldir)
	@for dir in $(DOCDIRS) ; do \
	  $(mkinstalldirs) $(DESTDIR)$(htmldir)/$$dir && \
	  for file in $(srcdir)/$$dir/*.* ; do \
	    echo "$(INSTALL_DATA) $$file $(DESTDIR)$(htmldir)/$$dir" ; \
	    $(INSTALL_DATA) "$$file" $(DESTDIR)$(htmldir)/$$dir ; \
	  done ; \
	done

# Uninstall HTML files
HTML_UNINSTALL_DATA_TARGETS = uninstall-data-html
#	rm -f $(DESTDIR)$(htmldir)/index.html
uninstall-data-html:
	for dir in $(DOCDIRS) ; do \
	  rm -f -r $(DESTDIR)$(htmldir)/$$dir ; \
	done

RST2HTML_TARGETS = \
	$(WWWDIR)/Magick++/index.html \
	$(WWWDIR)/Magick++/Documentation.html \
	$(WWWDIR)/api/api.html \
	$(WWWDIR)/CVS.html \
	$(WWWDIR)/Copyright.html \
	$(WWWDIR)/FAQ.html \
	$(WWWDIR)/INSTALL-unix.html \
	$(WWWDIR)/INSTALL-windows.html \
	$(WWWDIR)/NEWS.html \
	$(WWWDIR)/OpenMP.html \
	$(WWWDIR)/README.html \
	$(WWWDIR)/authors.html \
	$(WWWDIR)/benchmarks.html \
	$(WWWDIR)/benchmarks-1.2.html \
	$(WWWDIR)/bugs.html \
	$(WWWDIR)/contribute.html \
	$(WWWDIR)/download.html \
	$(WWWDIR)/formats.html \
	$(WWWDIR)/ImageMagickObject.html \
	$(WWWDIR)/index.html \
	$(WWWDIR)/links.html \
	$(WWWDIR)/miff.html \
	$(WWWDIR)/mission.html \
	$(WWWDIR)/motion-picture.html \
	$(WWWDIR)/perl.html \
	$(WWWDIR)/platforms.html \
	$(WWWDIR)/process.html \
	$(WWWDIR)/programming.html \
	$(WWWDIR)/quantize.html \
	$(WWWDIR)/tools.html \
	$(WWWDIR)/utilities.html

WWW_RST_FILES = \
	www/CVS.rst \
	www/FAQ.rst \
	www/OpenMP.rst \
	www/Magick++/index.rst \
	www/Magick++/Documentation.rst \
	www/api/api.rst \
	www/authors.rst \
	www/benchmarks.rst \
	www/benchmarks-1.2.rst \
	www/bugs.rst \
	www/contribute.rst \
	www/download.rst \
	www/formats.rst \
	www/ImageMagickObject.rst \
	www/index.rst \
	www/links.rst \
	www/miff.rst \
	www/mission.rst \
	www/motion-picture.rst \
	www/perl.rst \
	www/platforms.rst \
	www/process.rst \
	www/programming.rst \
	www/quantize.rst \
	www/tools.rst \
	www/utilities.rst

WWW_HTML_FILES = \
	www/CVS.html \
	www/Changelog.html \
	www/Copyright.html \
	www/FAQ.html \
	www/GraphicsMagick.html \
	www/INSTALL-unix.html \
	www/INSTALL-windows.html \
	www/ImageMagickObject.html \
	www/ImageMagickObject.rst \
	www/Magick++/Documentation.html \
	www/Magick++/Documentation.rst \
	www/Magick++/index.html \
	www/NEWS.html \
	www/OpenMP.html \
	www/README.html \
	www/animate.html \
	www/api/api.html \
	www/authors.html \
	www/benchmarks.html \
	www/bugs.html \
	www/color.html \
	www/compare.html \
	www/composite.html \
	www/conjure.html \
	www/contribute.html \
	www/convert.html \
	www/display.html \
	www/docutils-api.css \
	www/docutils-articles.css \
	www/download.html \
	www/formats.html \
	www/gm.html \
	www/identify.html \
	www/import.html \
	www/index.html \
	www/links.html \
	www/miff.html \
	www/mission.html \
	www/mogrify.html \
	www/montage.html \
	www/motion-picture.html \
	www/perl.html \
	www/platforms.html \
	www/process.html \
	www/programming.html \
	www/quantize.html \
	www/tools.html \
	www/utilities.html

WWWW_EXTRA_DIST=$(WWW_HTML_FILES) $(WWW_RST_FILES)

WWW_MAINTAINER_TARGETS=$(RST2HTML_TARGETS) $(TXT2HTML_TARGETS) $(WWWDIR)/color.html

if MAINTAINER_MODE

NAMED_COLORS=$(top_srcdir)/scripts/named_colors.py
RST2HTMLDECO=$(top_srcdir)/scripts/rst2htmldeco.py
RELPATH=$(top_srcdir)/scripts/relpath.py
OMP_DECIMAL_ALIGN=$(top_srcdir)/scripts/omp_decimal_align.py
RST2HTML_COMMAND = $(RST2HTMLDECO) --link-stylesheet=docutils-articles.css

if WITH_PERL
# txt2html requires perl.
TXT2HTML_TARGETS = \
	$(WWWDIR)/Changelog.html
endif # WITH_PERL

$(RST2HTML_TARGETS) : \
	$(top_srcdir)/scripts/html_fragments.py \
	$(top_srcdir)/scripts/rst2htmldeco.py \
	$(WWWDIR)/Makefile.am

# Build HTML version of RST file
$(top_srcdir)/www/%.html: $(top_srcdir)/%.txt
	$(RST2HTMLDECO) --link-stylesheet=`$(RELPATH) $(top_srcdir)/www $@`docutils-articles.css \
	--url-prefix=`$(RELPATH) $(top_srcdir)/www $@` $< $@

$(top_srcdir)/%.html: $(top_srcdir)/%.rst
	$(RST2HTMLDECO)  --link-stylesheet=`$(RELPATH) $(top_srcdir)/www $@`docutils-articles.css \
	--url-prefix=`$(RELPATH) $(top_srcdir)/www $@` $< | $(OMP_DECIMAL_ALIGN) > $@

$(top_srcdir)/www/%.html: $(top_srcdir)/www/%.rst
	$(RST2HTMLDECO)  --link-stylesheet=`$(RELPATH) $(top_srcdir)/www $@`docutils-articles.css \
	--url-prefix=`$(RELPATH) $(top_srcdir)/www $@` $< | $(OMP_DECIMAL_ALIGN) > $@

# Build HTML version of ChangeLog
$(top_srcdir)/www/Changelog.html: $(top_srcdir)/ChangeLog $(top_srcdir)/scripts/txt2html
	@PERL@ $(top_srcdir)/scripts/txt2html  -t $(PACKAGE_NAME)' '$(PACKAGE_VERSION)' ChangeLog' \
	< $(top_srcdir)/ChangeLog > \
	$(top_srcdir)/www/Changelog.html

# Build color.html
$(WWWDIR)/color.html : $(NAMED_COLORS)
	$(NAMED_COLORS) > $@

endif # MAINTAINER_MODE
