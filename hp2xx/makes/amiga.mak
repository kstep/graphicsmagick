# AMIGA Makefile, by Claus H. Langhans (langhans@informatik.uni-frankfurt.de)
#
# This makefile is for AMIGA computers.  GNU-C 2.2.2 and the
# dmake-programm that comes with GNU-C, not the dmake that comes with DICE!

SHELL :=

PROGRAMM 		= hp2xx

HEADERS 		=

COMPILER 		= GCC
CFLAGS 			= -v -c -fstrength-reduce # -Wall -O


LIBS 			= -lm -lc -lamiga13
LINKER 			= $(CC)
LINKEROPTIONS 		= -v

OBJFILES		= hp2xx.o std_main.o hpgl.o chardraw.o picbuf.o  \
 			  bresnham.o to_vec.o to_pcx.o to_pcl.o  \
 			  to_eps.o  to_amiga.o to_pbm.o to_ilbm.o \
 			  to_img.o to_rgip.o getopt.o getopt1.o \
			  to_fig.o clip.o fillpoly.o pendef.o lindef.o


$(PROGRAMM)		:  $(OBJFILES)
			$(LINKER) -o $(PROGRAMM) $(OBJFILES) \
			$(LIBS) $(LINKEROPTIONS)


makepatch		:
			pipe diff -d +context orig "" | tee hp2xx.amiga.patch > NIL:
			
makedist		:  hp2xx
			$(CP) makefile makefile.amiga
			-$(RM) hp2xx3.amiga.zoo
			zoo a hp2xx3.amiga.zoo \
			to_ilbm.c to_amiga.c hp2xx.c \
			makefile.amiga iff.h ilbm.h \
			gcc:libs/ixemul.library gcc:libs/COPYING.LIB \
			gcc:unix/usr/bin/ixconfig \
			COPYING work:readme.1st hp2xx
			zoo v hp2xx3.amiga.zoo
			
# ------- construction command goes here ---------


hp2xx.o			: hp2xx.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) hp2xx.c

std_main.o			: std_main.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) std_main.c

hpgl.o			: hpgl.c bresnham.h hp2xx.h chardraw.h
			$(COMPILER) $(CFLAGS) hpgl.c

chardraw.o		: chardraw.c bresnham.h hp2xx.h chardraw.h charset0.h
			$(COMPILER) $(CFLAGS) chardraw.c

picbuf.o		: picbuf.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) picbuf.c

bresnham.o		: bresnham.c bresnham.h
			$(COMPILER) $(CFLAGS) bresnham.c

to_vec.o			: to_vec.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_vec.c

to_pcx.o		: to_pcx.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_pcx.c

to_pcl.o		: to_pcl.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_pcl.c

to_rgip.o		: to_rgip.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_rgip.c

to_eps.o		: to_eps.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_eps.c

to_getopt.o		: to_getopt.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_eps.c

to_img.o		: to_img.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_img.c

to_amiga.o		: to_amiga.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_amiga.c

to_pbm.o		: to_pbm.c bresnham.h hp2xx.h
			$(COMPILER) $(CFLAGS) to_pbm.c

to_ilbm.o		: to_ilbm.c bresnham.h hp2xx.h iff.h ilbm.h
			$(COMPILER) $(CFLAGS) to_ilbm.c

getopt.o		: getopt.c getopt.h
			$(COMPILER) $(CFLAGS) getopt.c

getopt1.o		: getopt1.c getopt.h
			$(COMPILER) $(CFLAGS) getopt1.c

to_fig.o		: to_fig.c 
			$(COMPILER) $(CFLAGS) to_fig.c

clip.o			: clip.c
			$(COMPILER) $(CFLAGS) clip.c

fillpoly.o		: fillpoly.c
			$(COMPILER) $(CFLAGS) fillpoly.c

pendef.o		: pendef.c
			$(COMPILER) $(CFLAGS) pendef.c

lindef.o		: lindef.c
			$(COMPILER) $(CFLAGS) lindef.c


