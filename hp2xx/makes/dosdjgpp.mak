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
# Makefile for DJ Delorie's GO32 DOS extender + GCC version of hp2xx
#
CC	= gcc
OPTIONS	= -g -O2 -ansi -fstrength-reduce -finline-functions -Wall
LIBS	= -lgr -lm

# There are two versions available which you select by un-commenting
# three lines and commenting out another three lines just below this
# text. Remember to EITHER comment out the standard version OR the other.

# Standard version:

CFLAGS	= -c -DDOS -DGNU -DHAS_DOS_DJGR
EX_SRC	=
EX_OBJ	=

# Extended version, including modes PIC and PAC:
# Note: You'll need files to_pic.c and to_pac.c from ../extras

# CFLAGS	= -c -DDOS -DGNU -DHAS_DOS_DJGR -DPIC_PAC
# EX_SRC	= to_pic.c to_pac.c
# EX_OBJ	= pic.o pac.o


# No user-serviceable part below!
#############################################################################

SRCS	= hp2xx.c hpgl.c picbuf.c bresnham.c chardraw.c getopt.c $(INCS) \
	to_vec.c to_pcx.c to_pcl.c to_eps.c to_img.c to_dj_gr.c to_pbm.c \
	to_rgip.c std_main.c to_fig.c clip.c fillpoly.c pendef.c lindef.c \
	$(EX_SRC)


OBJS	= hp2xx.o hpgl.o pbuf.o bham.o cdrw.o opt.o rgip.o opt1.o \
	vec.o pcx.o pcl.o eps.o img.o djgr.o pbm.o std.o to_fig.o clip.o \
	fillpoly.o pendef.o lindef.o $(EX_OBJS)

INCS	= hp2xx.h bresnham.h chardraw.h charset0.h charset1.h charset2.h \
	charset3.h charset4.h charset5.h charset6.h charset7.h clip.h \
	 getopt.h pendef.h lindef.h

PROGRAM	= hp2xx


#########################################################################
#									#
# 			Implicit Rules					#
#									#
#########################################################################

all:	$(PROGRAM)386.exe

.c.o:	#$<.c $(INCS)
	$(CC) $(CFLAGS) $(OPTIONS) $<

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

$(PROGRAM)386.exe: $(PROGRAM)
	strip $(PROGRAM)
	copy /b c:\djgpp\bin\stub.exe+$(PROGRAM) $(PROGRAM)386.exe


# The following statements merely help to reduce characters within the
# DOS command line to avoid DOS's stupid 128 char limit:
#

pbuf.o:	picbuf.o
	copy picbuf.o pbuf.o

bham.o:	bresnham.o
	copy bresnham.o bham.o

cdrw.o:	chardraw.o
	copy chardraw.o cdrw.o

opt.o:	getopt.o
	copy getopt.o opt.o

opt1.o:	getopt1.o
	copy getopt1.o opt1.o

std.o:	std_main.o
	copy std_main.o std.o

vec.o:	to_vec.o
	copy to_vec.o vec.o

pcx.o:	to_pcx.o
	copy to_pcx.o pcx.o

pcl.o:	to_pcl.o
	copy to_pcl.o pcl.o

eps.o:	to_eps.o
	copy to_eps.o eps.o

img.o:	to_img.o
	copy to_img.o img.o

rgip.o:	to_rgip.o
	copy to_rgip.o rgip.o

djgr.o:	to_dj_gr.o
	copy to_dj_gr.o djgr.o

pbm.o:	to_pbm.o
	copy to_pbm.o pbm.o

pic.o:	to_pic.o
	copy to_pic.o pic.o

pac.o:	to_pac.o
	copy to_pac.o pac.o

#########################################################################

clean:		
		rm -f *.o hp2xx core a.out


