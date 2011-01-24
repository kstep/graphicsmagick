# Copyright (C) 2008 - 2011 GraphicsMagick Group

# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building HTML files
#

WWWDIR=$(top_srcdir)/www

# Install HTML files
DOCDIRS = www www/api www/images www/Magick++ www/wand
HTML_INSTALL_DATA_TARGETS = install-data-html
install-data-html:
	$(mkinstalldirs) $(DESTDIR)$(htmldir)
	@for dir in $(DOCDIRS) ; do \
	  $(mkinstalldirs) $(DESTDIR)$(htmldir)/$$dir && \
	  for file in $(srcdir)/$$dir/*.* ; do \
            case $$file in \
              *.c | *.css | *.gif | *.html | *.ico | *.jpg | *.png ) \
	        echo "$(INSTALL_DATA) $$file $(DESTDIR)$(htmldir)/$$dir" ; \
	        $(INSTALL_DATA) "$$file" $(DESTDIR)$(htmldir)/$$dir ; \
                ;; \
            esac \
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
	$(WWWDIR)/Magick++/Image.html \
	$(WWWDIR)/Magick++/index.html \
	$(WWWDIR)/Magick++/Documentation.html \
	$(WWWDIR)/api/api.html \
	$(WWWDIR)/wand/wand.html \
	$(WWWDIR)/CVS.html \
	$(WWWDIR)/Changes.html \
	$(WWWDIR)/Changelog.html \
	$(WWWDIR)/ChangeLog-2010.html \
	$(WWWDIR)/ChangeLog-2009.html \
	$(WWWDIR)/ChangeLog-2008.html \
	$(WWWDIR)/ChangeLog-2007.html \
	$(WWWDIR)/ChangeLog-2006.html \
	$(WWWDIR)/ChangeLog-2005.html \
	$(WWWDIR)/ChangeLog-2004.html \
	$(WWWDIR)/ChangeLog-2003.html \
	$(WWWDIR)/ChangeLog-2002.html \
	$(WWWDIR)/ChangeLog-2001.html
	$(WWWDIR)/Copyright.html \
	$(WWWDIR)/FAQ.html \
	$(WWWDIR)/INSTALL-unix.html \
	$(WWWDIR)/INSTALL-windows.html \
	$(WWWDIR)/NEWS.html \
	$(WWWDIR)/OpenMP.html \
	$(WWWDIR)/README.html \
	$(WWWDIR)/authors.html \
	$(WWWDIR)/benchmarks.html \
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
	www/Changes.rst \
	www/FAQ.rst \
	www/OpenMP.rst \
	www/Magick++/Image.rst \
	www/Magick++/index.rst \
	www/Magick++/Documentation.rst \
	www/api/api.rst \
	www/authors.rst \
	www/benchmarks.rst \
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
	www/Changes.html \
	www/Changelog.html \
	www/ChangeLog-2009.html \
	www/ChangeLog-2008.html \
	www/ChangeLog-2007.html \
	www/ChangeLog-2006.html \
	www/ChangeLog-2005.html \
	www/ChangeLog-2004.html \
	www/ChangeLog-2003.html \
	www/ChangeLog-2002.html \
	www/ChangeLog-2001.html \
	www/Copyright.html \
	www/FAQ.html \
	www/GraphicsMagick.html \
	www/INSTALL-unix.html \
	www/INSTALL-windows.html \
	www/ImageMagickObject.html \
	www/ImageMagickObject.rst \
	www/Magick++/Documentation.html \
	www/Magick++/Documentation.rst \
	www/Magick++/Image.html \
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

WWW_MAINTAINER_TARGETS=$(RST2HTML_TARGETS) $(WWWDIR)/color.html

if MAINTAINER_MODE

NAMED_COLORS=$(top_srcdir)/scripts/named_colors.py
RST2HTMLDECO=$(top_srcdir)/scripts/rst2htmldeco.py
RELPATH=$(top_srcdir)/scripts/relpath.py
OMP_DECIMAL_ALIGN=$(top_srcdir)/scripts/omp_decimal_align.py
RST2HTML_COMMAND = $(RST2HTMLDECO) --cloak-email-addresses --link-stylesheet=docutils-articles.css

$(RST2HTML_TARGETS) : \
	$(top_srcdir)/scripts/html_fragments.py \
	$(top_srcdir)/scripts/rst2htmldeco.py \
	$(top_srcdir)/scripts/omp_decimal_align.py \
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

#
# Build HTML version of ChangeLogs
#
CHANGELOG2RST=$(top_srcdir)/scripts/changelog2rst.sh

$(top_srcdir)/www/Changelog.rst: $(top_srcdir)/ChangeLog
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2010.rst: $(top_srcdir)/ChangeLog.2010
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2009.rst: $(top_srcdir)/ChangeLog.2009
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2008.rst: $(top_srcdir)/ChangeLog.2008
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2007.rst: $(top_srcdir)/ChangeLog.2007
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2006.rst: $(top_srcdir)/ChangeLog.2006
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2005.rst: $(top_srcdir)/ChangeLog.2005
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2004.rst: $(top_srcdir)/ChangeLog.2004
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2003.rst: $(top_srcdir)/ChangeLog.2003
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2002.rst: $(top_srcdir)/ChangeLog.2002
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2001.rst: $(top_srcdir)/ChangeLog.2001
	$(CHANGELOG2RST) < $^ > $@

# Build color.html
$(WWWDIR)/color.html : $(NAMED_COLORS)
	$(NAMED_COLORS) > $@

endif # MAINTAINER_MODE
