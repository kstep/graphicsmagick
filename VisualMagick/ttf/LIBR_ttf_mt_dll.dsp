# Microsoft Developer Studio Project File - Name="LIBR_ttf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LIBR_ttf - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_ttf_mt_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_ttf_mt_dll.mak" CFG="LIBR_ttf - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIBR_ttf - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LIBR_ttf - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIBR_ttf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\dRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "TTF_DLL" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "_TTFLIB_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x66600000" /subsystem:windows /dll /machine:I386 /out:"..\bin\LIBR_ttf.dll"

!ELSEIF  "$(CFG)" == "LIBR_ttf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\dDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "TTF_DLL" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "_TTFLIB_" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x66600000" /subsystem:windows /dll /incremental:no /pdb:"..\bin\LIBR_DB_ttf.pdb" /debug /machine:I386 /out:"..\bin\LIBR_DB_ttf.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LIBR_ttf - Win32 Release"
# Name "LIBR_ttf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=..\..\ttf\lib\ttapi.c

!IF  "$(CFG)" == "LIBR_ttf - Win32 Release"

# ADD CPP /Ot /Oi /Oy

!ELSEIF  "$(CFG)" == "LIBR_ttf - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttcache.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttcalc.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttcmap.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttdebug.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttextend.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttfile.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttgload.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttinterp.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttload.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttmemory.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttmutex.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttobjs.c
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttraster.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\ttf\lib\freetype.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ft_conf.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\header.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttcache.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttcalc.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttcmap.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttcommon.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttdebug.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttengine.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttextend.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttfile.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttgload.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttinterp.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttload.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttmutex.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttobjs.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\ttraster.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\tttables.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\tttags.h
# End Source File
# Begin Source File

SOURCE=..\..\ttf\lib\tttypes.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "*.rc"
# Begin Source File

SOURCE=.\version.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\freetype.nth

!IF  "$(CFG)" == "LIBR_ttf - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\freetype.nth

"..\..\ttf\lib\freetype.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\freetype.nth ..\..\ttf\lib\freetype.h

# End Custom Build

!ELSEIF  "$(CFG)" == "LIBR_ttf - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\freetype.nth

"..\..\ttf\lib\freetype.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\freetype.nth ..\..\ttf\lib\freetype.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ft_conf.nth

!IF  "$(CFG)" == "LIBR_ttf - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\ft_conf.nth

"..\..\ttf\lib\ft_conf.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\ft_conf.nth ..\..\ttf\lib\ft_conf.h

# End Custom Build

!ELSEIF  "$(CFG)" == "LIBR_ttf - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\ft_conf.nth

"..\..\ttf\lib\ft_conf.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\ft_conf.nth ..\..\ttf\lib\ft_conf.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ttconfig.nth

!IF  "$(CFG)" == "LIBR_ttf - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\ttconfig.nth

"..\..\ttf\lib\ttconfig.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\ttconfig.nth ..\..\ttf\lib\ttconfig.h

# End Custom Build

!ELSEIF  "$(CFG)" == "LIBR_ttf - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\ttconfig.nth

"..\..\ttf\lib\ttconfig.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\ttconfig.nth ..\..\ttf\lib\ttconfig.h

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
