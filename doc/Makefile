# Copyright (C) 2003 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building GraphicsMagick documentation.
# Written by Bob Friesenhahn based on previous work by Glenn Randers-Pehrson
#

UTILITIES = \
	animate composite conjure convert display identify import \
	mogrify montage GraphicsMagick

UTILSUBDIR=expanded/$(UTILITY)
UTILBASE=$(UTILITY)
UTILSUBWILDCARD=expanded/*
INSTALLWWW=../www
INSTALLUTILITIES=../utilities
GMSUBDIR=Gm

all:	gm.1 gm.html gm.tex

man:	gm.1

html:	gm.html

tex:	gm.tex

install: targets-install $(INSTALLWWW)/gm.html $(INSTALLUTILITIES)/gm.1

clean: targets-clean
	rm -f gm.1 gm.html gm.tex
	rm -f $(GMSUBDIR)/*
	if test -d $(GMSUBDIR) ; then rmdir $(GMSUBDIR) ; fi
	if test -d expanded; then rmdir expanded; fi

gm.1: targets-man $(GMSUBDIR)/man.imdoc
	./imdoc2man gm $(GMSUBDIR)/man.imdoc > $@

gm.html: targets-html $(GMSUBDIR)/html.imdoc
	./gmdoc2html gm $(GMSUBDIR)/man.imdoc > $@

gm.tex: targets-tex $(GMSUBDIR)/tex.imdoc
	./imdoc2tex < $(GMSUBDIR)/man.imdoc > $@

brief_options.imdoc:	options.imdoc
	imdoc_select_brief options.imdoc brief_options.imdoc

$(INSTALLWWW)/gm.html: gm.html
	cp gm.html $@

$(INSTALLUTILITIES)/gm.1: gm.1
	cp gm.1 $@

$(GMSUBDIR):
	mkdir -p $@

$(GMSUBDIR)/man.imdoc: targets-man $(GMSUBDIR)
	cat $(UTILSUBWILDCARD)/man.imdoc > $@

$(GMSUBDIR)/html.imdoc: targets-html $(GMSUBDIR)
	cat $(UTILSUBWILDCARD)/html.imdoc > $@

$(GMSUBDIR)/tex.imdoc: targets-tex $(GMSUBDIR)
	cat $(UTILSUBWILDCARD)/tex.imdoc > $@

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

#
# Per utility targets
#
utility: utility-man utility-html utility-tex

utility-man:	$(UTILBASE).1

utility-html:	$(UTILBASE).html

utility-tex:	$(UTILBASE).tex

utility-install: $(INSTALLWWW)/$(UTILBASE).html $(INSTALLUTILITIES)/$(UTILBASE).1

$(UTILSUBDIR):
	mkdir -p $(UTILSUBDIR)

$(UTILSUBDIR)/Expanded.imdoc: $(UTILSUBDIR) $(UTILITY).imdoc brief_options.imdoc copyright.imdoc
	imdocinclude $(UTILITY).imdoc > $@

$(UTILSUBDIR)/man.imdoc: $(UTILSUBDIR) $(UTILSUBDIR)/Expanded.imdoc
	gmdocselect $(UTILITY) man $(UTILSUBDIR)/Expanded.imdoc > $@

$(UTILSUBDIR)/html.imdoc: $(UTILSUBDIR) $(UTILSUBDIR)/Expanded.imdoc
	gmdocselect $(UTILITY) html $(UTILSUBDIR)/Expanded.imdoc > $@

$(UTILSUBDIR)/tex.imdoc: $(UTILSUBDIR) $(UTILSUBDIR)/Expanded.imdoc
	gmdocselect $(UTILITY) tex $(UTILSUBDIR)/Expanded.imdoc > $@

$(UTILBASE).1:  $(UTILSUBDIR) $(UTILSUBDIR)/man.imdoc
	imdoc2man $(UTILITY) $(UTILSUBDIR)/man.imdoc > $@

$(UTILBASE).html:  $(UTILSUBDIR) $(UTILSUBDIR)/html.imdoc
	gmdoc2html $(UTILITY) $(UTILSUBDIR)/html.imdoc > $@

$(UTILBASE).tex:  $(UTILSUBDIR) $(UTILSUBDIR)/tex.imdoc
	imdoc2tex < $(UTILSUBDIR)/tex.imdoc > $@

$(INSTALLWWW)/$(UTILITY).html: $(UTILBASE).html
	cp $(UTILBASE).html $@

$(INSTALLUTILITIES)/$(UTILITY).1: $(UTILBASE).1
	cp $(UTILBASE).1 $@

utility-clean:
	/bin/rm -f $(UTILBASE).1 $(UTILBASE).html $(UTILBASE).tex $(UTILSUBDIR)/*
	if test -d $(UTILSUBDIR); then rmdir $(UTILSUBDIR); fi
