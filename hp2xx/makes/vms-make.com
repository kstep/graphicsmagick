$!###########################################################################
$!#    Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.
$!#    Distributed by Free Software Foundation, Inc.
$!#
$!# This file is part of HP2xx.
$!#
$!# HP2xx is distributed in the hope that it will be useful, but
$!# WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
$!# to anyone for the consequences of using it or for whether it serves any
$!# particular purpose or works at all, unless he says so in writing.  Refer
$!# to the GNU General Public License, Version 2 or later, for full details
$!#
$!# Everyone is granted permission to copy, modify and redistribute
$!# HP2xx, but only under the conditions described in the GNU General Public
$!# License.  A copy of this license is supposed to have been
$!# given to you along with HP2xx so you can know your rights and
$!# responsibilities.  It should be in a file named COPYING.  Among other
$!# things, the copyright notice and this notice must be preserved on all
$!# copies.
$!###########################################################################
$!#
$!# Command file for VMS version of hp2xx
$!#
$!# Created automatically by $ MMS; modifications added.
$!#
$!###########################################################################
$       Verify = F$VERIFY(1)
$!
$       CC     = "GCC/Opt=2"
$       Option = "GNU.OPT"
$!
$! Uncomment the following lines if you want to use VMS-CC instead of GNU-CC.
$!      CC     = "CC"
$!      Option = "VAX.OPT"
$!
$       IF ( P1 .EQS. "" ) THEN GOTO ALL
$       GOTO P1
$!###########################################################################
$INSTALL:
$ALL:
$       WRITE SYS$OUTPUT "      MAKE of Hp2xx"
$       'CC'/NOLIST/OBJECT=HP2XX.OBJ   /DEFINE=("VAX") HP2XX.C
$       'CC'/NOLIST/OBJECT=HPGL.OBJ    /DEFINE=("VAX") HPGL.C
$       'CC'/NOLIST/OBJECT=PICBUF.OBJ  /DEFINE=("VAX") PICBUF.C
$       'CC'/NOLIST/OBJECT=BRESNHAM.OBJ/DEFINE=("VAX") BRESNHAM.C
$       'CC'/NOLIST/OBJECT=CHARDRAW.OBJ/DEFINE=("VAX") CHARDRAW.C
$       'CC'/NOLIST/OBJECT=GETOPT.OBJ  /DEFINE=("VAX") GETOPT.C
$       'CC'/NOLIST/OBJECT=GETOPT1.OBJ /DEFINE=("VAX") GETOPT1.C
$       'CC'/NOLIST/OBJECT=STD_MAIN.OBJ/DEFINE=("VAX") STD_MAIN.C
$       'CC'/NOLIST/OBJECT=TO_PCX.OBJ  /DEFINE=("VAX") TO_PCX.C
$       'CC'/NOLIST/OBJECT=TO_PCL.OBJ  /DEFINE=("VAX") TO_PCL.C
$       'CC'/NOLIST/OBJECT=TO_EPS.OBJ  /DEFINE=("VAX") TO_EPS.C
$       'CC'/NOLIST/OBJECT=TO_IMG.OBJ  /DEFINE=("VAX") TO_IMG.C
$       'CC'/NOLIST/OBJECT=TO_PBM.OBJ  /DEFINE=("VAX") TO_PBM.C
$       'CC'/NOLIST/OBJECT=TO_RGIP.OBJ /DEFINE=("VAX") TO_RGIP.C
$       'CC'/NOLIST/OBJECT=TO_VEC.OBJ  /DEFINE=("VAX") TO_VEC.C
$       'CC'/NOLIST/OBJECT=TO_UIS.OBJ  /DEFINE=("VAX") TO_UIS.C
$       'CC'/NOLIST/OBJECT=TO_FIG.OBJ  /DEFINE=("VAX") TO_FIG.C
$       'CC'/NOLIST/OBJECT=CLIP.OBJ  /DEFINE=("VAX") CLIP.C
$	'CC'/NOLIST/OBJECT=FILLPOLY.OBJ /DEFINE=("VAX") FILLPOLY.C
$	'CC'/NOLIST/OBJECT=PENDEF.OBJ /DEFINE=("VAX") PENDEF.C
$       LINK /TRACE/NOMAP/EXEC=[-]HP2XX.EXE hp2xx.obj,hpgl.obj,picbuf.obj,bresnham.obj, -
                                      chardraw.obj,getopt.obj,getopt1.obj,std_main.obj, -
                                      to_pcx.obj,to_pcl.obj,to_eps.obj,                 -
                                      to_img.obj,to_pbm.obj,to_rgip.obj,                -
                                      to_fig.obj,clip.obj,fillpoly.obj,                 -
				      pendef.obj,to_vec.obj,to_uis.obj,lindef.obj,                 -
				      'Option'/opt
$       WRITE SYS$OUTPUT "      Hp2xx complete"
$       IF ( F$EDIT(P1,"UPCASE") .EQS. "INSTALL" ) THEN GOTO Install_Help
$       GOTO End
$!###########################################################################
$Check:
$       DefDir = F$ENVIRONMENT("DEFAULT")
$       ExeDir = DefDir - ".SOURCES]" + "]"
$       HP2XX  == "$''ExeDir'HP2XX"
$       TestDir   = DefDir - "SOURCES]" + "HP-TESTS]"
$       ASSIGN/NoLog SYS$COMMAND SYS$INPUT              ! read <RETURN> from keyboard
$       HP2XX -mpre 'TestDir'inter.hp
$       GOTO End
$!###########################################################################
$RealClean:
$       DELETE/NoConfirm/Log [-]HP2XX.EXE;*
$       LIBRARY/HELP/REMOVE=(HP2XX) [-]HP2XX.HLB
$Clean:
$MostlyClean:
$       DELETE/NoConfirm/Log *.OBJ;*
$       PURGE [-]HP2XX.EXE
$       GOTO End
$!###########################################################################
$DistClean:
$       WRITE SYS$OUTPUT " DistClean -- not supported yet."
$       GOTO End
$!###########################################################################
$Dist:
$       WRITE SYS$OUTPUT " Dist -- not supported yet."
$       GOTO End
$!###########################################################################
$TAGS:
$       WRITE SYS$OUTPUT " TAGS -- not supported yet."
$       GOTO End
$!###########################################################################
$Install_Help:
$       GOTO End
$       If "''F$Search("[-]HP2XX.HLB")'" .EQS. "" Then  -
                LIBRARY/Create/Help [-]HP2XX.HLB
$       LIBRARY/REPLACE [-]HP2XX.HLB [-.DOC]HP2XX.HLP
$! Add a line like the following to your personal login file or to SYLOGIN.COM,
$! respectively, for use of the help file.
$! $ SiteH*elp == "HELP/LIBRARY=SYS$HELP:SITESPEC/NoLibList/NoPage"
$       GOTO End
$!###########################################################################
$End:
$       Verify = F$VERIFY('Verify')
$       EXIT

