# Microsoft Developer Studio Project File - Name="CORE_bzlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CORE_bzlib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CORE_bzlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CORE_bzlib.mak" CFG="CORE_bzlib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CORE_bzlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CORE_bzlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CORE_bzlib - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Release\CORE_bzlib"
# PROP Target_Dir ""
# ADD CPP /nologo /ML /W3 /GX /O2 /I "..\.." /I "..\..\magick" /I "..\..\xlib" /I "..\..\Magick++\lib" /I "..\..\zlib" /I "..\..\jpeg" /I "..\..\tiff\libtiff" /I "..\..\lcms\src" /I "..\..\lcms\source" /I "..\..\ttf\include" /I "..\..\libxml\include" /I "..\..\bzlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_LIB" /D "BZ_EXPORT" /D "_WIN32" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /machine:I386  /out:"..\lib\CORE_RL_bzlib_.lib"

!ELSEIF  "$(CFG)" == "CORE_bzlib - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Debug\CORE_bzlib"
# PROP Target_Dir ""
# ADD CPP /nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\.." /I "..\..\magick" /I "..\..\xlib" /I "..\..\Magick++\lib" /I "..\..\zlib" /I "..\..\jpeg" /I "..\..\tiff\libtiff" /I "..\..\lcms\src" /I "..\..\lcms\source" /I "..\..\ttf\include" /I "..\..\libxml\include" /I "..\..\bzlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_LIB" /D "BZ_EXPORT" /D "_WIN32" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /machine:I386  /out:"..\lib\CORE_DB_bzlib_.lib"
!ENDIF 

# Begin Target

# Name "CORE_bzlib - Win32 Release"
# Name "CORE_bzlib - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\bzlib\blocksort.c
# End Source File
# Begin Source File

SOURCE=..\..\bzlib\bzlib.c
# End Source File
# Begin Source File

SOURCE=..\..\bzlib\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\bzlib\crctable.c
# End Source File
# Begin Source File

SOURCE=..\..\bzlib\decompress.c
# End Source File
# Begin Source File

SOURCE=..\..\bzlib\huffman.c
# End Source File
# Begin Source File

SOURCE=..\..\bzlib\randtable.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\bzlib\bzlib.h
# End Source File
# Begin Source File

SOURCE=..\..\bzlib\bzlib_private.h
# End Source File
# End Group
# End Target
# End Project
