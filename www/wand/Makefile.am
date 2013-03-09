# Copyright (C) 2009 - 2012 GraphicsMagick Group

# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building Wand API HTML files
#

wwwwandapidir = $(htmldir)/wand

WWWWANDAPIDIR = $(top_srcdir)/www/wand

WWWWANDAPI_HTML_TARGETS = \
	$(WWWWANDAPIDIR)/drawing_wand.html \
	$(WWWWANDAPIDIR)/magick_wand.html \
	$(WWWWANDAPIDIR)/pixel_wand.html


WWWWANDAPI_FILES = \
	www/wand/drawing_wand.html \
	www/wand/magick_wand.html \
	www/wand/pixel_wand.html

WWWWANDAPI_EXTRA_DIST = \
	www/wand/wand.rst \
	$(WWWWANDAPI_FILES)

WWWWANDAPI_MAINTAINER_TARGETS = $(WWWWANDAPI_HTML_TARGETS)

if MAINTAINER_MODE

if HasRST2HTML
#APIWHATIS = $(top_srcdir)/scripts/whatis.txt
#FORMATCAPI = $(top_srcdir)/scripts/format_c_api_doc.py

#APIRST2HTML_COMMAND = $(RST2HTMLDECO) \
  --link-stylesheet=../docutils-articles.css`

$(WWWWANDAPI_HTML_TARGETS) : $(FORMATCAPI) \
	$(RST2HTMLDECO) \
	$(APIWHATIS) \
	$(top_srcdir)/scripts/html_fragments.py \
	$(top_srcdir)/www/wand/Makefile.am \
	$(top_srcdir)/www/api/api_hyperlinks.rst

$(WWWWANDAPIDIR)/%.apirst : $(top_srcdir)/wand/%.c
	$(FORMATCAPI) --whatis-file $(APIWHATIS) \
	--include-rst ../api/api_hyperlinks.rst $< $@

$(WWWWANDAPIDIR)/%.html : $(WWWWANDAPIDIR)/%.apirst
	$(RST2HTMLDECO) --link-stylesheet=`$(RELPATH) $(top_srcdir)/www $@`docutils-api.css \
	--url-prefix=`$(RELPATH) $(top_srcdir)/www $@` $< $@

endif # HasRST2HTML

endif # MAINTAINER_MODE
