###########################################################################
#    Copyright (c) 1991 - 1993 Heinz W. Werntges.  All rights reserved.
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
# MMS description file for VMS version of hp2xx
#
#       GCC Version 2.4.5
#       *****************
#       HP2XX was successfully compiled with VMS V4.7 or V5.5-1,
#       MMS-V2.2 and GCC-V2.4.5.
#
#       CC Version 2.3
#       **************
#       This file was used successfully with VMS V4.7 or V5.5-1,
#       MMS-V2.2 and CC-V2.3, but you have to do a single modification
#       by hand in GETOPT1.C. This modification is described in the
#       following:
#       Edit GETOPT1.C
#               Goto line 29
#               Remove the backslash at the end of this line and
#               merge line 29 with line 30. Afterwards line 29 should look
#               like the next line:
#                       #if defined(STDC_HEADERS) || defined(__GNU_LIBRARY__) || defined (LIBC) || defined (__BORLANDC__)
#               Exit (with writing) from the editor.
#
#       MMS options
#       ***********
#       This MMS files allows to use several target specifiers; i.e.,
#       you may execute this file by the command
#       $ MMS {target}
#       Target is one of the following:
#               DEFINE          => Define the necessary command to run HP2XX
#               ALL             => Compile and link everything + DEFINE
#               INSTALL_HELP    => Install help file in library
#               INSTALL         => ALL + INSTALL_HELP
#               CHECK           => Run an application in preview mode
#               REALCLEAN       => Produce status quo ante
#               CLEAN or
#                  MOSTLYCLEAN  => Delete all objects
#       The name of the help library should be changed appropriately to
#       your system.
#
#       INSTALLATION
#       ************
#       See the comments below DEFINE and INSTALL_HELP, respectively, for
#       routine use of hp2xx.
#
###########################################################################

.IGNORE                         ! ignore any errors during the make

# Uncomment the following three lines in case you want to compile with GNU-CC
# but comment them in case of VAX-CC
CC              =       GCC
of              =       GNU.OPT
CFLAGS          =       $(CFLAGS)/DEFINE=("VAX")/OPT=2

# Uncomment the following two lines in case you want to compile with VAX-CC
# but comment them in case of GCC
!of             =       VAX.OPT
!CFLAGS         =       $(CFLAGS)/DEFINE=("VAX")

PREVIEWER       =       to_uis.obj

# Change the name of the help library according to your system;
# f.e. the next line may be much more HELPful.
!HelpLib        =       SYS$HELP:SiteSpec.HLB
HelpLib         =       [-]HP2XX.HLB

#############################################################################
#############################################################################
# No user-serviceable parts below!
#############################################################################

.FIRST
        WRITE SYS$OUTPUT "      MAKE of Hp2xx"

.LAST
        WRITE SYS$OUTPUT "      Hp2xx complete"

COMMON_INCS     =       hp2xx.h, bresnham.h, pendef.h lindef.h

OBJS            =       hp2xx.obj,hpgl.obj,picbuf.obj,bresnham.obj,     -
                        chardraw.obj,getopt.obj,getopt1.obj,            -
                        to_vec.obj,to_pcx.obj,to_pcl.obj,to_eps.obj,    -
                        to_img.obj,to_pbm.obj,to_rgip.obj,std_main.obj  -
			to_fig.obj clip.obj fillpoly.obj pendef.obj lindef.obj -
                        $(PREVIEWER)

HP2XX           =       [-]HP2XX.EXE

$(HP2XX)        :       $(OBJS), $(of)
                $(LINK) $(LINKFLAGS) $(OBJS),$(of)/opt

#########################################################################
#                                                                       #
#                       Explicit Rules                                  #
#                                                                       #
#########################################################################

hp2xx.obj       :       hp2xx.c,        $(COMMON_INCS)

std_main.obj    :       std_main.c,     $(COMMON_INCS), getopt.h

hpgl.obj        :       hpgl.c,         $(COMMON_INCS), chardraw.h

picbuf.obj      :       picbuf.c,       $(COMMON_INCS)

bresnham.obj    :       bresnham.c,                     bresnham.h

chardraw.obj    :       chardraw.c,     $(COMMON_INCS), chardraw.h, charset0.h

getopt.obj      :       getopt.c,                       getopt.h

getopt1.obj     :       getopt1.c,                      getopt.h

to_vec.obj      :       to_vec.c,       $(COMMON_INCS)

to_pcx.obj      :       to_pcx.c,       $(COMMON_INCS)

to_pcl.obj      :       to_pcl.c,       $(COMMON_INCS)

to_eps.obj      :       to_eps.c,       $(COMMON_INCS)

to_img.obj      :       to_img.c,       $(COMMON_INCS)

to_pbm.obj      :       to_pbm.c,       $(COMMON_INCS)

to_rgip.obj     :       to_rgip.c,      $(COMMON_INCS)

to_uis.obj      :       to_uis.c,       $(COMMON_INCS)

to_fig.obj      :       to_fig.c,       $(COMMON_INCS)

clip.obj        :       clip.c,       $(COMMON_INCS)

fillpoly.obj	:	fillpoly.c,	$COMMON_INCS)

pendef.obj	:	pendef.c,	$(COMMON_INCS)

lindef.obj	:	lindef.c,	$(COMMON_INCS)

$(HelpLib)      :       [-.DOC]hp2xx.HLP


!no_prev.obj    :       no_prev.c

!to_dj_gr.obj   :       to_dj_gr.c,     $(COMMON_INCS)

!to_ilbm.obj    :       !to_ilbm.c,     $(COMMON_INCS), iff.h, ilbm.h

!to_pac.obj     :       to_pac.c,       $(COMMON_INCS)

!to_pic.obj     :       to_pic.c,       $(COMMON_INCS)

!to_pm.obj      :       to_pm.c,        $(COMMON_INCS)

!to_sunvw.obj   :       to_sunvw.c,     $(COMMON_INCS)

!to_vga.obj     :       to_vga.c,       $(COMMON_INCS)

!to_x11.obj     :       to_x11.c,       $(COMMON_INCS), x11.h

!to_amiga.obj   :       to_amiga.c,     $(COMMON_INCS)

!to_atari.obj   :       to_atari.c,     $(COMMON_INCS)

#########################################################################
#                                                                       #
#                       Targets                                         #
#                                                                       #
#########################################################################

### Compile and link everything
All             :       $(HP2XX), Define

### Define the necessary command to run HP2XX
Define          :
                DefDir = F$ENVIRONMENT("DEFAULT")
                ExeDir = DefDir - ".SOURCES]" + "]"
                HP2XX  == "$''ExeDir'HP2XX"
! A definition like the line above should be included into your personal
! login file or into SYLOGIN.COM, respectively, for routine use of hp2xx.

### Run an application in preview mode
Check           :       $(HP2XX), Define
                TestDir   = DefDir - "SOURCES]" + "HP-TESTS]"
                ASSIGN/NoLog SYS$COMMAND SYS$INPUT      ! read <RETURN> from keyboard
                HP2XX -mpre 'TestDir'inter.hp

###
RealClean       :       Clean
                - DELETE/NoConfirm/Log $(HP2XX);*
                - LIBRARY/HELP/REMOVE=(HP2XX) $(HelpLib)
###
Clean           :       MostlyClean

###
MostlyClean     :
                - DELETE/NoConfirm/Log *.OBJ;*
                - PURGE *.*
                - PURGE $(HP2XX)

###
DistClean       :
                WRITE SYS$OUTPUT " DistClean -- not supported yet."

###
Dist            :
                WRITE SYS$OUTPUT " Dist -- not supported yet."

###
TAGS            :
                WRITE SYS$OUTPUT " TAGS -- not supported yet."

### Install everything
Install         :       ALL, Install-Help

### Install the help file in the library
Install-Help    :       $(HelpLib)
! Add a line like the following to your personal login file or to SYLOGIN.COM,
! respectively, for use of the help file.
! $ SiteH*elp == "HELP/LIBRARY=SYS$HELP:SITESPEC/NoLibList/NoPage"

