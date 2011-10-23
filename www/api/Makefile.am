# Copyright (C) 2008-2011 GraphicsMagick Group

# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building API HTML files
#

wwwapidir = $(htmldir)/api

WWWAPIDIR = $(top_srcdir)/www/api

WWWAPI_HTML_TARGETS = \
	$(WWWAPIDIR)/animate.html \
	$(WWWAPIDIR)/annotate.html \
	$(WWWAPIDIR)/attribute.html \
	$(WWWAPIDIR)/average.html \
	$(WWWAPIDIR)/blob.html \
	$(WWWAPIDIR)/cdl.html \
	$(WWWAPIDIR)/channel.html \
	$(WWWAPIDIR)/color.html \
	$(WWWAPIDIR)/colormap.html \
	$(WWWAPIDIR)/compare.html \
	$(WWWAPIDIR)/composite.html \
	$(WWWAPIDIR)/confirm_access.html \
	$(WWWAPIDIR)/constitute.html \
	$(WWWAPIDIR)/decorate.html \
	$(WWWAPIDIR)/deprecate.html \
	$(WWWAPIDIR)/describe.html \
	$(WWWAPIDIR)/display.html \
	$(WWWAPIDIR)/draw.html \
	$(WWWAPIDIR)/effect.html \
	$(WWWAPIDIR)/enhance.html \
	$(WWWAPIDIR)/error.html \
	$(WWWAPIDIR)/export.html \
	$(WWWAPIDIR)/fx.html \
	$(WWWAPIDIR)/hclut.html \
	$(WWWAPIDIR)/image.html \
	$(WWWAPIDIR)/import.html \
	$(WWWAPIDIR)/list.html \
	$(WWWAPIDIR)/magick.html \
	$(WWWAPIDIR)/memory.html \
	$(WWWAPIDIR)/monitor.html \
	$(WWWAPIDIR)/montage.html \
	$(WWWAPIDIR)/operator.html \
	$(WWWAPIDIR)/paint.html \
	$(WWWAPIDIR)/pixel_cache.html \
	$(WWWAPIDIR)/pixel_iterator.html \
	$(WWWAPIDIR)/plasma.html \
	$(WWWAPIDIR)/profile.html \
	$(WWWAPIDIR)/quantize.html \
	$(WWWAPIDIR)/registry.html \
	$(WWWAPIDIR)/render.html \
	$(WWWAPIDIR)/resize.html \
	$(WWWAPIDIR)/resource.html \
	$(WWWAPIDIR)/segment.html \
	$(WWWAPIDIR)/shear.html \
	$(WWWAPIDIR)/signature.html \
	$(WWWAPIDIR)/statistics.html \
	$(WWWAPIDIR)/texture.html \
	$(WWWAPIDIR)/transform.html \
	$(WWWAPIDIR)/types.html \
	$(WWWAPIDIR)/widget.html

WWWWAPI_FILES = \
	www/api/animate.html \
	www/api/annotate.html \
	www/api/api_hyperlinks.rst \
	www/api/attribute.html \
	www/api/average.html \
	www/api/blob.html \
	www/api/cdl.html \
	www/api/channel.html \
	www/api/color.html \
	www/api/colormap.html \
	www/api/compare.html \
	www/api/composite.html \
	www/api/confirm_access.html \
	www/api/constitute.html \
	www/api/decorate.html \
	www/api/deprecate.html \
	www/api/describe.html \
	www/api/display.html \
	www/api/draw.html \
	www/api/effect.html \
	www/api/export.html \
	www/api/enhance.html \
	www/api/error.html \
	www/api/fx.html \
	www/api/hclut.html \
	www/api/image.html \
	www/api/import.html \
	www/api/list.html \
	www/api/magick.html \
	www/api/memory.html \
	www/api/monitor.html \
	www/api/montage.html \
	www/api/operator.html \
	www/api/paint.html \
	www/api/pixel_cache.html \
	www/api/pixel_iterator.html \
	www/api/plasma.html \
	www/api/profile.html \
	www/api/quantize.html \
	www/api/registry.html \
	www/api/render.html \
	www/api/resize.html \
	www/api/resource.html \
	www/api/segment.html \
	www/api/shear.html \
	www/api/signature.html \
	www/api/statistics.html \
	www/api/texture.html \
	www/api/transform.html \
	www/api/types.html \
	www/api/widget.html

WWWAPI_EXTRA_DIST = \
	$(WWWWAPI_FILES)

WWWAPI_MAINTAINER_TARGETS = $(WWWAPI_HTML_TARGETS)

if MAINTAINER_MODE

if HasRST2HTML
APIWHATIS = $(top_srcdir)/scripts/whatis.txt
FORMATCAPI = $(top_srcdir)/scripts/format_c_api_doc.py

APIRST2HTML_COMMAND = $(RST2HTMLDECO) \
  --link-stylesheet=../docutils-articles.css`

$(WWWAPI_HTML_TARGETS) : $(FORMATCAPI) $(RST2HTMLDECO) $(APIWHATIS) $(top_srcdir)/scripts/html_fragments.py $(top_srcdir)/www/api/Makefile.am $(top_srcdir)/www/api/api_hyperlinks.rst

$(WWWAPIDIR)/%.apirst : $(top_srcdir)/magick/%.c
	$(FORMATCAPI) --whatis-file $(APIWHATIS) $< $@

$(WWWAPIDIR)/%.html : $(WWWAPIDIR)/%.apirst
	$(RST2HTMLDECO) --link-stylesheet=`$(RELPATH) $(top_srcdir)/www $@`docutils-api.css \
	--url-prefix=`$(RELPATH) $(top_srcdir)/www $@` $< $@

endif # HasRST2HTML

endif # MAINTAINER_MODE
