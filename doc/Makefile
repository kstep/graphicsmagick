
UTILITIES = \
	animate composite conjure convert display identify import \
	mogrify montage GraphicsMagick

all:	gm.1 gm.html gm.tex

man:	gm.1

html:	gm.html

tex:	gm.tex

clean: targets-clean
	rm -f gm.1 gm.html gm.tex
	rm -f gm-Gm/*
	if test -d 'gm-Gm' ; then rmdir 'gm-Gm' ; fi

gm.1: targets-man
	if test ! -d 'gm-Gm'; then mkdir 'gm-Gm'; fi
	cat gm-*/man.imdoc > gm-Gm/man.imdoc
	./imdoc2man gm gm-Gm/man.imdoc > gm.1

gm.html: targets-html
	if test ! -d 'gm-Gm'; then mkdir 'gm-Gm'; fi
	cat gm-*/html.imdoc > gm-Gm/html.imdoc
	./gmdoc2html gm gm-Gm/html.imdoc > gm.html

gm.tex: targets-tex
	if test ! -d 'gm-Gm'; then mkdir 'gm-Gm'; fi
	cat gm-*/tex.imdoc > gm-Gm/tex.imdoc
	./imdoc2tex < gm-Gm/tex.imdoc > gm.tex

brief_options.imdoc:	options.imdoc
	imdoc_select_brief options.imdoc brief_options.imdoc

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

targets-install:
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

utility-man:	gm-$(UTILITY).1

utility-html:	gm-$(UTILITY).html

utility-tex:	gm-$(UTILITY).tex

gm-$(UTILITY):
	mkdir gm-$(UTILITY)

gm-$(UTILITY)/Expanded.imdoc: gm-$(UTILITY) $(UTILITY).imdoc brief_options.imdoc copyright.imdoc
	imdocinclude $(UTILITY).imdoc > gm-$(UTILITY)/Expanded.imdoc

gm-$(UTILITY)/man.imdoc: gm-$(UTILITY) gm-$(UTILITY)/Expanded.imdoc
	gmdocselect $(UTILITY) man gm-$(UTILITY)/Expanded.imdoc > \
	gm-$(UTILITY)/man.imdoc

gm-$(UTILITY)/html.imdoc: gm-$(UTILITY) gm-$(UTILITY)/Expanded.imdoc
	gmdocselect $(UTILITY) html gm-$(UTILITY)/Expanded.imdoc > \
	gm-$(UTILITY)/html.imdoc

gm-$(UTILITY)/tex.imdoc: gm-$(UTILITY) gm-$(UTILITY)/Expanded.imdoc
	gmdocselect $(UTILITY) tex gm-$(UTILITY)/Expanded.imdoc > \
	gm-$(UTILITY)/tex.imdoc

gm-$(UTILITY).1:  gm-$(UTILITY) gm-$(UTILITY)/man.imdoc
	imdoc2man $(UTILITY) gm-$(UTILITY)/man.imdoc > gm-$(UTILITY).1

gm-$(UTILITY).html:  gm-$(UTILITY) gm-$(UTILITY)/html.imdoc
	gmdoc2html $(UTILITY) gm-$(UTILITY)/html.imdoc > gm-$(UTILITY).html

gm-$(UTILITY).tex:  gm-$(UTILITY) gm-$(UTILITY)/tex.imdoc
	imdoc2tex < gm-$(UTILITY)/tex.imdoc > gm-$(UTILITY).tex

utility-install: www/gm-$(UTILITY).html targets/gm-$(UTILITY).1

www/gm-$(UTILITY).html: gm-$(UTILITY).html
	cp gm-$(UTILITY).html gm-www/$(UTILITY).html

utilities/gm-$(UTILITY).1: gm-$(UTILITY).1
	cp gm-$(UTILITY).1 utilities/gm-$(UTILITY).1

utility-clean:
	/bin/rm -f gm-$(UTILITY).1 gm-$(UTILITY).html gm-$(UTILITY).tex gm-$(UTILITY)/*
	if test -d gm-$(UTILITY); then rmdir gm-$(UTILITY); fi
