# Unix makefile for JBIG-KIT
# $Id$

# Select an ANSI/ISO C compiler here, GNU gcc is recommended
CC = gcc

# Options for the compiler: A high optimization level is suggested
CCFLAGS = -O2 -W
#CCFLAGS = -O -g -W -Wall -ansi -pedantic #-DDEBUG  # developer only

CFLAGS = $(CCFLAGS) -I../libjbig

VERSION=1.2

all: lib pbm
	@echo "Enter 'make test' in order to start some automatic tests."

lib:
	(cd libjbig;  make "CC=$(CC)" "CFLAGS=$(CFLAGS)")

pbm: lib
	(cd pbmtools; make "CC=$(CC)" "CFLAGS=$(CFLAGS)")

test: lib pbm
	(cd libjbig;  make "CC=$(CC)" "CFLAGS=$(CFLAGS)" test)
	(cd pbmtools; make "CC=$(CC)" "CFLAGS=$(CFLAGS)" test)

clean:
	rm -f *~ core
	(cd libjbig; make clean)
	(cd pbmtools; make clean)

DISTHOST = faui01c.informatik.uni-erlangen.de
DISTUID  = mskuhn
DISTPATH = ftp/ISO/JBIG

distribution: clean
	rm -f libjbig/libjbig.a
	(cd ..; tar -c -v --exclude RCS -f jbigkit-$(VERSION).tar jbigkit ; \
	  gzip -9f jbigkit-$(VERSION).tar )
	ssh -l $(DISTUID) $(DISTHOST) rm -f \
	  $(DISTPATH)/jbigkit-$(VERSION).tar.gz \
	  $(DISTPATH)/jbigkit-$(VERSION)-announce.txt
	scp ../jbigkit-$(VERSION).tar.gz $(DISTUID)@$(DISTHOST):$(DISTPATH)/
	scp ANNOUNCE \
	  $(DISTUID)@$(DISTHOST):$(DISTPATH)/jbigkit-$(VERSION)-announce.txt
	mv ../jbigkit-$(VERSION).tar.gz $(HOME)/.www/download/
