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
# Makefile for OS/2 2.x or later & emx 0.9d version of hp2xx
#
CC	= gcc
OPTIONS	= -O2 -ansi -fstrength-reduce -finline-functions -Wall
EMXBIN	= e:/emx/bin

# There are two versions available which you select by un-commenting
# three lines and commenting out another three lines just below this
# text. Remember to EITHER comment out the standard version OR the other.

# Standard version:

CFLAGS	= -c -DOS2 -DHAS_OS2_PM
EX_SRC	=
EX_OBJ	=

# Extended version, including modes PIC and PAC:
# Note: You'll need files to_pic.c and to_pac.c from ../extras

# CFLAGS	= -c -DOS2 -DPIC_PAC  -DHAS_OS2_PM
# EX_SRC	= to_pic.c to_pac.c
# EX_OBJ	= to_pic.o to_pac.o


# No user-serviceable part below!
#############################################################################

#
# During tests of both PM and full-screen previewer, the lines below
# allow for easy switching:
#

LIBS	= -los2
OS2DISP		= to_pm

# -f (full screen), -p (PM), -w (windowed with stderr output redirection 
# to a separate window)
OS2DISPFLAG	= -w




INCS	= hp2xx.h bresnham.h chardraw.h charset0.h getopt.h pendef.h lindef.h

SRCS	= hp2xx.c hpgl.c picbuf.c bresnham.c chardraw.c getopt.c getopt1.c \
	  $(INCS)\
	  to_vec.c to_pcx.c to_pcl.c to_eps.c to_img.c $(OS2DISP).c to_pbm.c \
	  to_rgip.c std_main.c to_fig.c clip.c fillpoly.c pendef.c lindef.c \
	  $(EX_SRC)

OBJS	= hp2xx.o hpgl.o picbuf.o bresnham.o chardraw.o getopt.o getopt1.o \
	  to_vec.o to_pcx.o to_pcl.o to_eps.o to_img.o $(OS2DISP).o to_pbm.o \
	  to_rgip.o std_main.o to_fig.o clip.o fillpoly.o pendef.o lindef.o \
	  $(EX_OBJ)

PROGRAM	= hp2xx


#########################################################################
#									#
# 			Implicit Rules					#
#									#
#########################################################################

.c.o:	#$<.c $(INCS)
	$(CC) $(CFLAGS) $(OPTIONS) $<

#########################################################################

all:	$(PROGRAM)
	emxbind $(OS2DISPFLAG) $(EMXBIN)/emxl.exe $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:		
	-rm -f *.o hp2xx core a.out

