# Microsoft Developer Studio Project File - Name="LIBR_JBIG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LIBR_JBIG - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_JBIG_st_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_JBIG_st_lib.mak" CFG="LIBR_JBIG - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIBR_JBIG - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LIBR_JBIG - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIBR_JBIG - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\sRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "_JBIGLIB_" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\LIBR_RL_JBIG_.lib"

!ELSEIF  "$(CFG)" == "LIBR_JBIG - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir ".\sDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\sDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "_JBIGLIB_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\LIBR_DB_JBIG_.lib"

!ENDIF 

# Begin Target

# Name "LIBR_JBIG - Win32 Release"
# Name "LIBR_JBIG - Win32 Debug"
# Begin Source File

SOURCE=..\..\jbig\libjbig\jbig.c
# End Source File
# Begin Source File

SOURCE=..\..\jbig\libjbig\jbig.h
# End Source File
# Begin Source File

SOURCE=.\jbig.nth

!IF  "$(CFG)" == "LIBR_JBIG - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\jbig.nth

"..\..\jbig\libjbig\jbig.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\jbig.nth ..\..\jbig\libjbig\jbig.h

# End Custom Build

!ELSEIF  "$(CFG)" == "LIBR_JBIG - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\jbig.nth

"..\..\jbig\libjbig\jbig.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\jbig.nth ..\..\jbig\libjbig\jbig.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\jbig\libjbig\jbig_tab.c
# End Source File
# Begin Source File

SOURCE=.\version.rc
# End Source File
# End Target
# End Project
