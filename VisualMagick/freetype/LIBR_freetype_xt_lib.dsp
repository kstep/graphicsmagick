# Microsoft Developer Studio Project File - Name="LIBR_freetype" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LIBR_freetype - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_freetype.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_freetype.mak" CFG="LIBR_freetype - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIBR_freetype - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LIBR_freetype - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIBR_freetype - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Release\LIBR_freetype"
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\freetype" /I "..\..\freetype\config" /I "..\..\freetype\include" /I "..\..\freetype\src" /I "..\..\freetype\src\base" /I "..\..\freetype\src\psnames" /I "..\..\freetype\src\sfnt" /I "..\..\freetype\src\shared" /I "..\..\freetype\src\truetype" /I "..\..\freetype\src\type1" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "_LIB" /D "TTF_DLL" /D "_TTFLIB_" /YX /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /machine:I386  /out:"..\lib\LIBR_RL_freetype_.lib"

!ELSEIF  "$(CFG)" == "LIBR_freetype - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Debug\LIBR_freetype"
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\freetype" /I "..\..\freetype\config" /I "..\..\freetype\include" /I "..\..\freetype\src" /I "..\..\freetype\src\base" /I "..\..\freetype\src\psnames" /I "..\..\freetype\src\sfnt" /I "..\..\freetype\src\shared" /I "..\..\freetype\src\truetype" /I "..\..\freetype\src\type1" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "_LIB" /D "TTF_DLL" /D "_TTFLIB_" /YX /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /machine:I386  /out:"..\lib\LIBR_DB_freetype_.lib"
!ENDIF 

# Begin Target

# Name "LIBR_freetype - Win32 Release"
# Name "LIBR_freetype - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\freetype\src\base\ftbase.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftdebug.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftglyph.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftinit.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftraster.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftsystem.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\psnames\psnames.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\sfnt\sfnt.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\truetype.c
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\type1.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\freetype\src\shared\autohint.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\freetype.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\ftbbox.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftcalc.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\config\ftconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftdebug.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\fterrors.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftextend.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\ftglyph.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\ftgrays.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\ftimage.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftlist.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\config\ftmodule.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftobjs.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\config\ftoption.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftoutln.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\ftraster.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\base\ftstream.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\ftsystem.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\psnames\psdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\shared\psnames.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\psnames\pstables.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\sfnt\sfdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\shared\sfnt.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1afm.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1driver.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1errors.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1gload.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1hinter.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1load.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1objs.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1parse.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\type1\t1tokens.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\shared\t1types.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\sfnt\ttcmap.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\ttdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\shared\tterrors.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\tterrors.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\ttgload.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\ttinterp.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\sfnt\ttload.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\include\ttnameid.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\ttnameid.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\ttobjs.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\ttpload.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\sfnt\ttpost.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\sfnt\ttsbit.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\shared\tttags.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\truetype\tttags.h
# End Source File
# Begin Source File

SOURCE=..\..\freetype\src\shared\tttypes.h
# End Source File
# End Group
# End Target
# End Project
