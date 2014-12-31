# Copyright (C) 2008 - 2014 GraphicsMagick Group

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

# These files are built from reStructuredText
RST2HTML_TARGETS = \
	$(WWWDIR)/ChangeLog-2001.html \
	$(WWWDIR)/ChangeLog-2002.html \
	$(WWWDIR)/ChangeLog-2003.html \
	$(WWWDIR)/ChangeLog-2004.html \
	$(WWWDIR)/ChangeLog-2005.html \
	$(WWWDIR)/ChangeLog-2006.html \
	$(WWWDIR)/ChangeLog-2007.html \
	$(WWWDIR)/ChangeLog-2008.html \
	$(WWWDIR)/ChangeLog-2009.html \
	$(WWWDIR)/ChangeLog-2010.html \
	$(WWWDIR)/ChangeLog-2011.html \
	$(WWWDIR)/ChangeLog-2012.html \
	$(WWWDIR)/ChangeLog-2013.html \
	$(WWWDIR)/ChangeLog-2014.html \
	$(WWWDIR)/Changelog.html \
	$(WWWDIR)/Changes.html \
	$(WWWDIR)/Copyright.html \
	$(WWWDIR)/FAQ.html \
	$(WWWDIR)/Hg.html \
	$(WWWDIR)/INSTALL-unix.html \
	$(WWWDIR)/INSTALL-windows.html \
	$(WWWDIR)/ImageMagickObject.html \
	$(WWWDIR)/Magick++/Blob.html \
	$(WWWDIR)/Magick++/ChangeLog.html \
	$(WWWDIR)/Magick++/CoderInfo.html \
	$(WWWDIR)/Magick++/Color.html \
	$(WWWDIR)/Magick++/Drawable.html \
	$(WWWDIR)/Magick++/Enumerations.html \
	$(WWWDIR)/Magick++/Exception.html \
	$(WWWDIR)/Magick++/FormatCharacters.html \
	$(WWWDIR)/Magick++/Geometry.html \
	$(WWWDIR)/Magick++/Image.html \
	$(WWWDIR)/Magick++/ImageDesign.html \
	$(WWWDIR)/Magick++/Montage.html \
	$(WWWDIR)/Magick++/PixelPacket.html \
	$(WWWDIR)/Magick++/Pixels.html \
	$(WWWDIR)/Magick++/TypeMetric.html \
	$(WWWDIR)/Magick++/index.html \
	$(WWWDIR)/NEWS.html \
	$(WWWDIR)/OpenMP.html \
	$(WWWDIR)/README.html \
	$(WWWDIR)/api/api.html \
	$(WWWDIR)/authors.html \
	$(WWWDIR)/benchmarks.html \
	$(WWWDIR)/bugs.html \
	$(WWWDIR)/contribute.html \
	$(WWWDIR)/download.html \
	$(WWWDIR)/formats.html \
	$(WWWDIR)/index.html \
	$(WWWDIR)/links.html \
	$(WWWDIR)/miff.html \
	$(WWWDIR)/mission.html \
	$(WWWDIR)/motion-picture.html \
	$(WWWDIR)/perl.html \
	$(WWWDIR)/process.html \
	$(WWWDIR)/programming.html \
	$(WWWDIR)/project.html \
	$(WWWDIR)/reference.html \
	$(WWWDIR)/quantize.html \
	$(WWWDIR)/tools.html \
	$(WWWDIR)/utilities.html \
	$(WWWDIR)/wand/wand.html

# These files are existing reStructuredText files
WWW_RST_FILES = \
	$(WWWDIR)/Changes.rst \
	$(WWWDIR)/FAQ.rst \
	$(WWWDIR)/Hg.rst \
	$(WWWDIR)/ImageMagickObject.rst \
	$(WWWDIR)/Magick++/Blob.rst \
	$(WWWDIR)/Magick++/CoderInfo.rst \
	$(WWWDIR)/Magick++/Color.rst \
	$(WWWDIR)/Magick++/Drawable.rst \
	$(WWWDIR)/Magick++/Enumerations.rst \
	$(WWWDIR)/Magick++/Exception.rst \
	$(WWWDIR)/Magick++/FormatCharacters.rst \
	$(WWWDIR)/Magick++/Geometry.rst \
	$(WWWDIR)/Magick++/Image.rst \
	$(WWWDIR)/Magick++/ImageDesign.rst \
	$(WWWDIR)/Magick++/Montage.rst \
	$(WWWDIR)/Magick++/PixelPacket.rst \
	$(WWWDIR)/Magick++/Pixels.rst \
	$(WWWDIR)/Magick++/TypeMetric.rst \
	$(WWWDIR)/Magick++/index.rst \
	$(WWWDIR)/OpenMP.rst \
	$(WWWDIR)/api/api.rst \
	$(WWWDIR)/authors.rst \
	$(WWWDIR)/benchmarks.rst \
	$(WWWDIR)/bugs.rst \
	$(WWWDIR)/contribute.rst \
	$(WWWDIR)/download.rst \
	$(WWWDIR)/formats.rst \
	$(WWWDIR)/index.rst \
	$(WWWDIR)/links.rst \
	$(WWWDIR)/miff.rst \
	$(WWWDIR)/mission.rst \
	$(WWWDIR)/motion-picture.rst \
	$(WWWDIR)/perl.rst \
	$(WWWDIR)/process.rst \
	$(WWWDIR)/programming.rst \
	$(WWWDIR)/project.rst \
	$(WWWDIR)/reference.rst \
	$(WWWDIR)/quantize.rst \
	$(WWWDIR)/tools.rst \
	$(WWWDIR)/utilities.rst

# These are the HTML files which are packaged
WWW_HTML_FILES = \
	$(WWWDIR)/ChangeLog-2001.html \
	$(WWWDIR)/ChangeLog-2002.html \
	$(WWWDIR)/ChangeLog-2003.html \
	$(WWWDIR)/ChangeLog-2004.html \
	$(WWWDIR)/ChangeLog-2005.html \
	$(WWWDIR)/ChangeLog-2006.html \
	$(WWWDIR)/ChangeLog-2007.html \
	$(WWWDIR)/ChangeLog-2008.html \
	$(WWWDIR)/ChangeLog-2009.html \
	$(WWWDIR)/ChangeLog-2010.html \
	$(WWWDIR)/ChangeLog-2011.html \
	$(WWWDIR)/ChangeLog-2012.html \
	$(WWWDIR)/ChangeLog-2013.html \
	$(WWWDIR)/ChangeLog-2014.html \
	$(WWWDIR)/Changelog.html \
	$(WWWDIR)/Changes.html \
	$(WWWDIR)/Copyright.html \
	$(WWWDIR)/FAQ.html \
	$(WWWDIR)/GraphicsMagick.html \
	$(WWWDIR)/Hg.html \
	$(WWWDIR)/INSTALL-unix.html \
	$(WWWDIR)/INSTALL-windows.html \
	$(WWWDIR)/ImageMagickObject.html \
	$(WWWDIR)/ImageMagickObject.rst \
	$(WWWDIR)/Magick++/Blob.html \
	$(WWWDIR)/Magick++/ChangeLog.html \
	$(WWWDIR)/Magick++/CoderInfo.html \
	$(WWWDIR)/Magick++/Color.html \
	$(WWWDIR)/Magick++/Drawable.html \
	$(WWWDIR)/Magick++/Enumerations.html \
	$(WWWDIR)/Magick++/Exception.html \
	$(WWWDIR)/Magick++/FormatCharacters.html \
	$(WWWDIR)/Magick++/Geometry.html \
	$(WWWDIR)/Magick++/Image.html \
	$(WWWDIR)/Magick++/ImageDesign.html \
	$(WWWDIR)/Magick++/Montage.html \
	$(WWWDIR)/Magick++/PixelPacket.html \
	$(WWWDIR)/Magick++/Pixels.html \
	$(WWWDIR)/Magick++/TypeMetric.html \
	$(WWWDIR)/Magick++/index.html \
	$(WWWDIR)/NEWS.html \
	$(WWWDIR)/OpenMP.html \
	$(WWWDIR)/README.html \
	$(WWWDIR)/animate.html \
	$(WWWDIR)/api/api.html \
	$(WWWDIR)/authors.html \
	$(WWWDIR)/benchmarks.html \
	$(WWWDIR)/bugs.html \
	$(WWWDIR)/color.html \
	$(WWWDIR)/compare.html \
	$(WWWDIR)/composite.html \
	$(WWWDIR)/conjure.html \
	$(WWWDIR)/contribute.html \
	$(WWWDIR)/convert.html \
	$(WWWDIR)/display.html \
	$(WWWDIR)/docutils-api.css \
	$(WWWDIR)/docutils-articles.css \
	$(WWWDIR)/download.html \
	$(WWWDIR)/formats.html \
	$(WWWDIR)/gm.html \
	$(WWWDIR)/identify.html \
	$(WWWDIR)/import.html \
	$(WWWDIR)/index.html \
	$(WWWDIR)/links.html \
	$(WWWDIR)/miff.html \
	$(WWWDIR)/mission.html \
	$(WWWDIR)/mogrify.html \
	$(WWWDIR)/montage.html \
	$(WWWDIR)/motion-picture.html \
	$(WWWDIR)/perl.html \
	$(WWWDIR)/process.html \
	$(WWWDIR)/project.html \
	$(WWWDIR)/reference.html \
	$(WWWDIR)/programming.html \
	$(WWWDIR)/quantize.html \
	$(WWWDIR)/tools.html \
	$(WWWDIR)/utilities.html\
	$(WWWDIR)/wand/wand.html

WWWW_EXTRA_DIST=$(WWW_HTML_FILES) $(WWW_RST_FILES)

WWW_MAINTAINER_TARGETS=$(WWW_HTML_FILES) $(WWWDIR)/color.html

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

#
# Build HTML version of ChangeLogs
#
CHANGELOG2RST=$(top_srcdir)/scripts/changelog2rst.sh

$(top_srcdir)/www/Changelog.rst: $(top_srcdir)/ChangeLog
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2014.rst: $(top_srcdir)/ChangeLog.2014
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2013.rst: $(top_srcdir)/ChangeLog.2013
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2012.rst: $(top_srcdir)/ChangeLog.2012
	$(CHANGELOG2RST) < $^ > $@

$(top_srcdir)/www/ChangeLog-2011.rst: $(top_srcdir)/ChangeLog.2011
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

$(top_srcdir)/www/Magick++/ChangeLog.rst: $(top_srcdir)/Magick++/ChangeLog
	$(CHANGELOG2RST) < $^ > $@

# Build color.html
$(WWWDIR)/color.html : $(NAMED_COLORS)
	$(NAMED_COLORS) > $@

endif # MAINTAINER_MODE
