###########################################################################
#    Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.
#    Distributed by Free Software Foundation, Inc.
#
# This file is part of HP2xx.
#
# HP2xx is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
# to anyone for the consequences of using it or for whether it serves any
# particular purpose or works at all, unless he says so in writing.  Refer
# to the GNU General Public License, Version 2 or later, for full details
#
# Everyone is granted permission to copy, modify and redistribute
# HP2xx, but only under the conditions described in the GNU General Public
# License.  A copy of this license is supposed to have been
# given to you along with HP2xx so you can know your rights and
# responsibilities.  It should be in a file named COPYING.  Among other
# things, the copyright notice and this notice must be preserved on all
# copies.
###########################################################################
#
# Makefile for ATARI versions of hp2xx
#

# CC and OPTIONS set for gcc, but any ANSI-C compiler should work.
# For a non-gcc compiler, simply set OPTIONS = -O (to start with)
CC	= gcc
OPTIONS	= -O2 -fomit-frame-pointer -Wall

LDFLAGS	=			# link flags
INCDIR	=			# additional dir with include files
LIBS	= -lm			# libraries to be linked
GEMLIB	= -lgem			# GEM library.


# There are two versions available which you select by un-commenting
# three lines and commenting out another three lines just below this
# text. Remember to EITHER comment out the standard version OR the other.

# Standard version:

CFLAGS	= -DATARI $(INCDIR) $(OPTIONS)		# compile flags
EX_SRC	=					# extra sources
EX_OBJ	=					# extra objects

# Extended version, including modes PIC and PAC:
# Note: You'll need files to_pic.c and to_pac.c from ../extras

# CFLAGS	= -DATARI -DPIC_PAC $(INCDIR) $(OPTIONS)
# EX_SRC	= to_pic.c to_pac.c
# EX_OBJ	= to_pic.o to_pac.o


# No user-serviceable part below!
#############################################################################

COMMON_INCS	= hp2xx.h bresnham.h
ALL_INCS	= $(COMMON_INCS) chardraw.h charset0.h getopt.h x11.h ilbm.h iff.h


# $(SRCS) NOT USED YET:
SRCS	= hp2xx.c hpgl.c picbuf.c bresnham.c chardraw.c getopt.c getopt1.c \
	  $(ALL_INCS)\
	  to_vec.c to_pcx.c to_pcl.c to_eps.c to_img.c to_pbm.c to_rgip.c \
	  to_atari.c std_main.c fillpoly.c pendef.c lindef.c $(EX_SRC)

OBJS	= hp2xx.o hpgl.o picbuf.o bresnham.o chardraw.o getopt.o getopt1.o \
	  to_vec.o to_pcx.o to_pcl.o to_eps.o to_img.o to_pbm.o to_rgip.o \
	  to_atari.o std_main.o fillpoly.o pendef.o lindef.o $(EX_OBJ)

PROGRAM	= hp2xx.ttp


#########################################################################
#									#
# 			Implicit Rules					#
#									#
#########################################################################

all: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(GEMLIB) $(LIBS) -o $(PROGRAM)


.c.o:	$< $(COMMON_INCS)
	$(CC) -c $(CFLAGS) $<


#########################################################################
#									#
# 			Explicit Rules					#
#									#
#########################################################################

bresnham.o:	$< bresnham.h

chardraw.o:	$< $(COMMON_INCS) chardraw.h charset0.h

getopt.o:	$< getopt.h

getopt1.o:	$< getopt.h

hp2xx.o:	$< $(COMMON_INCS) getopt.h

hpgl.o:		$< $(COMMON_INCS) chardraw.h

ilbm.o:		$< $(COMMON_INCS) ilbm.h iff.h

to_x11.o:	$< $(COMMON_INCS) x11.h

#########################################################################

clean:
		rm -f *.o core a.out

veryclean: clean
		rm -i $(PROGRAM)

# Suggestion for installation:
# Un-comment if desired
#
#install:	hp2xx ../doc/hp2xx.1
#		strip hp2xx
#		install -m 755 hp2xx /usr/local/bin
#		install -m 644 hp2xx.1 /usr/man/local/man1

