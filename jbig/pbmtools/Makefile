# Unix makefile for the JBIG-KIT PBM tools
# $Id$

# Select an ANSI/ISO C compiler here, e.g. GNU gcc is recommended
CC = gcc

# Options for the compiler
CFLAGS = -g -Wall -ansi -pedantic -I../libjbig

.SUFFIXES: .1 .5 .txt $(SUFFIXES)

all: pbmtojbg jbgtopbm pbmtojbg.txt jbgtopbm.txt pbm.txt pgm.txt

pbmtojbg: pbmtojbg.o ../libjbig/libjbig.a
	$(CC) $(CFLAGS) -o pbmtojbg pbmtojbg.o -L../libjbig -ljbig

jbgtopbm: jbgtopbm.o ../libjbig/libjbig.a
	$(CC) $(CFLAGS) -o jbgtopbm jbgtopbm.o -L../libjbig -ljbig

jbgtopbm.o: jbgtopbm.c ../libjbig/jbig.h
pbmtojbg.o: pbmtojbg.c ../libjbig/jbig.h

../libjbig/libjbig.a:
	cd ../libjbig ; make libjbig.a

test: pbmtojbg jbgtopbm
	make IMG=ccitt1  OPTIONSJ=      OPTIONSP=      dotest1
	make IMG=ccitt2  OPTIONSJ=      OPTIONSP=      dotest1
	make IMG=ccitt3  OPTIONSJ=      OPTIONSP=      dotest1
	make IMG=xvlogo  OPTIONSJ=     "OPTIONSP=-d 3" dotest1
	make IMG=sandra  OPTIONSP=      OPTIONSJ=      dotest2
	make IMG=sandra  OPTIONSP=-b    OPTIONSJ=-b    dotest2
	make IMG=sandra  OPTIONSP=-q    OPTIONSJ=      dotest2
	make IMG=sandra "OPTIONSP=-o 0" OPTIONSJ=      dotest2
	make IMG=sandra "OPTIONSP=-o 2" OPTIONSJ=      dotest2
	rm -f test-*.jbg test-*.pbm test-*.pgm
	@echo
	@echo "The pbmtools have PASSED the functional tests. Good!"
	@echo

dotest1:
	./jbgtopbm $(OPTIONSJ) ../examples/$(IMG).jbg test-$(IMG).pbm
	./pbmtojbg $(OPTIONSP) test-$(IMG).pbm test-$(IMG).jbg
	cmp test-$(IMG).jbg ../examples/$(IMG).jbg

dotest2:
	./pbmtojbg $(OPTIONSP) ../examples/$(IMG).pgm test-$(IMG).jbg
	./jbgtopbm $(OPTIONSJ) test-$(IMG).jbg test-$(IMG).pgm
	cmp test-$(IMG).pgm ../examples/$(IMG).pgm

.1.txt:
	gtroff -man -Tascii $< | grotty - -u -b >$@

.5.txt:
	gtroff -man -Tascii $< | grotty - -u -b >$@

clean:
	rm -f *.o *~ core pbmtojbg jbgtopbm
