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
# Makefile for BORLAND C++ version of hp2xx
#
# Add support of modes PIC and PAC by un-commenting the corresponding
# lines below.
#
# HERCULES (HGC) patches:
#
#   o  Replace all HAS_DOS_VGA with HAS_DOS_HGC
#   o  Replace all to_vga...  with to_hgc...

.AUTODEPEND

#		*Translator Definitions*
CC = bcc +HP2XX.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BC\LIB;C:\BC\LIB\LOCAL
INCLUDEPATH = C:\BC\INCLUDE;C:\BC\INCLUDE\SHARE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 bresnham.obj \
 chardraw.obj \
 getopt.obj \
 getopt1.obj \
 hp2xx.obj \
 hpgl.obj \
 picbuf.obj \
 to_eps.obj \
 to_img.obj \
 to_pbm.obj \
 to_pcl.obj \
 to_pcx.obj \
 to_rgip.obj \
 to_vec.obj \
 to_vga.obj \
 std_main.obj \
 to_fig.obj \
 clip.obj \
 fillpoly.obj \
 pendef.obj \
 lindef.obj
# to_pac.obj \
# to_pic.obj

#		*Explicit Rules*
hp2xx.exe: hp2xx.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
bresnham.obj+
chardraw.obj+
getopt.obj+
getopt1.obj+
hp2xx.obj+
hpgl.obj+
picbuf.obj+
to_eps.obj+
to_img.obj+
to_pbm.obj+
# to_pac.obj+
# to_pic.obj+
to_pcl.obj+
to_pcx.obj+
to_rgip.obj+
to_vec.obj+
to_vga.obj+
to_fig.obj+
clip.obj+
fillpoly.obj+
pendef.obj+
lindef.obj+
std_main.obj
hp2xx
		# no map file
graphics.lib+
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
bresnham.obj: hp2xx.cfg bresnham.c

chardraw.obj: hp2xx.cfg chardraw.c

getopt.obj: hp2xx.cfg getopt.c

getopt1.obj: hp2xx.cfg getopt1.c

hp2xx.obj: hp2xx.cfg hp2xx.c

hpgl.obj: hp2xx.cfg hpgl.c

picbuf.obj: hp2xx.cfg picbuf.c

to_eps.obj: hp2xx.cfg to_eps.c

to_img.obj: hp2xx.cfg to_img.c

to_pbm.obj: hp2xx.cfg to_pbm.c

# to_pac.obj: hp2xx.cfg to_pac.c
#
# to_pic.obj: hp2xx.cfg to_pic.c

to_pcl.obj: hp2xx.cfg to_pcl.c

to_pcx.obj: hp2xx.cfg to_pcx.c

to_rgip.obj: hp2xx.cfg to_rgip.c

to_vec.obj: hp2xx.cfg to_vec.c

to_vga.obj: hp2xx.cfg to_vga.c

std_main.obj: hp2xx.cfg std_main.c

to_fig.obj: hp2xx.cfg to_fig.c

clip.obj: hp2xx.cfg clip.c

fillpoly.obj: hp2xx.cfg fillpoly.c

pendef.obj: hp2xx.cfg pendef.c

lindef.obj: hp2xx.cfg lindef.c

#		*Compiler Configuration File*
hp2xx.cfg: borland.mak
  copy &&|
-ml
-a
-A
-O
-Z
-d
-vi-
-wpin
-wamb
-wamp
-wasm
-wpro
-wcln
-wdef
-wsig
-wnod
-wstv
-wucp
-wuse
-weas
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DDOS
-DHAS_DOS_VGA
-D_LIBC
-D__GNU_LIBRARY__
# -DPIC_PAC
| hp2xx.cfg

