# Copyright (C) 2003-2013 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/Copyright.html.
#
# Makefile for building GraphicsMagick documentation.
# Written by Bob Friesenhahn based on previous work by Glenn Randers-Pehrson
#

UTILITIES = \
	animate \
	batch \
	benchmark \
	compare \
	composite \
	conjure \
	convert \
	display \
	identify \
	import \
	mogrify \
	montage \
	time \
	version \
	GraphicsMagick

UTILSUBDIR       = expanded/$(UTILITY)
UTILBASE         = $(UTILITY)
UTILSUBWILDCARD  = expanded/*
INSTALLWWW       = ../www
INSTALLUTILITIES = ../utilities
GMSUBDIR=Gm

DOC2MAN	         = ./imdoc2man
DOC2HTML         = ./gmdoc2html
DOC2TEX          = ./imdoc2tex
DOCSELECTCOMBINED = ./gmdocselect
DOCSELECTSEPARATE = ./imdocselect
DOCINCLUDE       = ./imdocinclude

# You might need to change this to /usr/bin/sed
SED = /bin/sed
export SED

HTML_TARGETS = \
	GraphicsMagick.html \
	animate.html \
	batch.html \
	benchmark.html \
	compare.html \
	composite.html \
	conjure.html \
	convert.html \
	display.html \
	gm.html \
	identify.html \
	import.html \
	mogrify.html \
	montage.html \
	time.html \
	version.html

MAN_TARGETS       = gm.1

all:	gm.1 targets-html # gm.html # gm.tex

man:	gm.1

html:	gm.html

tex:	gm.tex

install: targets-install $(INSTALLWWW)/gm.html $(INSTALLUTILITIES)/gm.1

clean: targets-clean
	rm -f gm.1 gm.html gm.tex *.tmp
	rm -f $(GMSUBDIR)/*
	if test -d $(GMSUBDIR) ; then rmdir $(GMSUBDIR) ; fi
	if test -d expanded; then rmdir expanded; fi

gm.1: targets-man $(GMSUBDIR)/man.imdoc $(DOC2MAN)
	-rm -f $@ $@.tmp
	$(DOC2MAN) gm $(GMSUBDIR)/man.imdoc > $@.tmp && \
	mv $@.tmp $@

gm.html: targets-html $(GMSUBDIR)/html.imdoc $(DOC2HTML)
	-rm -f $@ $@.tmp
	$(DOC2HTML) gm $(GMSUBDIR)/man.imdoc > $@.tmp && \
	mv $@.tmp $@

gm.tex: targets-tex $(GMSUBDIR)/tex.imdoc $(DOC2TEX)
	-rm -f $@ $@.tmp
	$(DOC2TEX) < $(GMSUBDIR)/man.imdoc > $@.tmp && \
	mv $@.tmp $@

brief_options.imdoc:	options.imdoc
	imdoc_select_brief options.imdoc brief_options.imdoc

$(INSTALLWWW)/gm.html: gm.html
	cp -p $< $@

$(INSTALLUTILITIES)/gm.1: gm.1
	cp -p $< $@

$(GMSUBDIR)/man.imdoc: targets-man
	@if test ! -d $(GMSUBDIR); then mkdir -p $(GMSUBDIR); fi
	-rm -f $@ $@.tmp
	cat $(UTILSUBWILDCARD)/man.imdoc > $@.tmp && \
	mv $@.tmp $@

$(GMSUBDIR)/html.imdoc: targets-html
	@if test ! -d $(GMSUBDIR); then mkdir -p $(GMSUBDIR); fi
	-rm -f $@ $@.tmp
	cat $(UTILSUBWILDCARD)/html.imdoc > $@.tmp && \
	mv $@.tmp $@

$(GMSUBDIR)/tex.imdoc: targets-tex
	@if test ! -d $(GMSUBDIR); then mkdir -p $(GMSUBDIR); fi
	-rm -f $@ $@.tmp
	cat $(UTILSUBWILDCARD)/tex.imdoc > $@.tmp && \
	mv $@.tmp $@

#
# Invoke per-utility targets
#
targets:
	for utility in $(UTILITIES) ; \
	do \
	  $(MAKE) UTILITY=$$utility utility; \
	done

targets-man:
	for utility in $(UTILITIES) ; \
	do \
	  $(MAKE) UTILITY=$$utility utility-man; \
	done

targets-html:
	for utility in $(UTILITIES) ; \
	do \
	  $(MAKE) UTILITY=$$utility utility-html; \
	done

targets-tex:
	for utility in $(UTILITIES) ; \
	do \
	  $(MAKE) UTILITY=$$utility utility-tex; \
	done

targets-install: targets-man targets-html
	for utility in $(UTILITIES) ; \
	do \
	  $(MAKE) UTILITY=$$utility utility-install; \
	done

targets-clean:
	for utility in $(UTILITIES) ; \
	do \
	  $(MAKE) UTILITY=$$utility utility-clean; \
	done
	rm -f brief_options.imdoc

#
# Per utility targets
#
utility: utility-man utility-html utility-tex

utility-man:	$(UTILSUBDIR)/man.imdoc

utility-html:	$(UTILBASE).html

utility-tex:	$(UTILBASE).tex

utility-install: $(UTILBASE).html # $(INSTALLUTILITIES)/$(UTILBASE).1
	cp -p $(UTILBASE).html $(INSTALLWWW)/$(UTILBASE).html

$(UTILSUBDIR)/Expanded.imdoc: $(UTILITY).imdoc brief_options.imdoc config_files.imdoc copyright.imdoc environment.imdoc $(DOCINCLUDE)
	@if test ! -d $(UTILSUBDIR); then mkdir -p $(UTILSUBDIR); fi
	-rm -f $@ $@.tmp
	$(DOCINCLUDE) $< > $@.tmp && \
	mv $@.tmp $@

$(UTILSUBDIR)/man.imdoc: $(UTILSUBDIR)/Expanded.imdoc $(DOCSELECTCOMBINED)
	-rm -f $@ $@.tmp
	$(DOCSELECTCOMBINED) $(UTILITY) man $< > $@.tmp && \
	mv $@.tmp $@

$(UTILSUBDIR)/html.imdoc: $(UTILSUBDIR)/Expanded.imdoc $(DOCSELECTSEPARATE)
	-rm -f $@ $@.tmp
	$(DOCSELECTSEPARATE) $(UTILITY) html $< > $@.tmp && \
	mv $@.tmp $@

$(UTILSUBDIR)/tex.imdoc: $(UTILSUBDIR)/Expanded.imdoc $(DOCSELECTCOMBINED)
	-rm -f $@ $@.tmp
	$(DOCSELECTCOMBINED) $(UTILITY) tex $< > $@.tmp && \
	mv $@.tmp $@

#$(UTILBASE).1:  $(UTILSUBDIR)/man.imdoc $(DOC2MAN)
#	-rm -f $@ $@.tmp
#	$(DOC2MAN) $(UTILITY) $< > $@.tmp && \
#	mv $@.tmp $@

$(UTILBASE).html:  $(UTILSUBDIR)/html.imdoc $(DOC2HTML)
	-rm -f $@ $@.tmp
	$(DOC2HTML) $(UTILITY) $< > $@.tmp && \
	mv $@.tmp $@

$(UTILBASE).tex:  $(UTILSUBDIR)/tex.imdoc $(DOC2TEX)
	-rm -f $@ $@.tmp
	$(DOC2TEX) < $< > $@.tmp && \
	mv $@.tmp $@

$(INSTALLWWW)/$(UTILITY).html: $(UTILBASE).html
	cp -p $< $@

$(INSTALLUTILITIES)/$(UTILITY).1: $(UTILBASE).1
	cp -p $< $@

utility-clean:
	rm -f $(UTILBASE).1 $(UTILBASE).html $(UTILBASE).tex $(UTILSUBDIR)/*
	if test -d $(UTILSUBDIR); then rmdir $(UTILSUBDIR); fi
