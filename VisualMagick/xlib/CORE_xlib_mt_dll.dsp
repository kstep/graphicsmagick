# Microsoft Developer Studio Project File - Name="CORE_xlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CORE_xlib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CORE_xlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CORE_xlib.mak" CFG="CORE_xlib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CORE_xlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CORE_xlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CORE_xlib - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Release\CORE_xlib"
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "..\.." /I "..\..\magick" /I "..\..\xlib" /I "..\..\Magick++\lib" /I "..\..\MagickArgs" /I "..\..\zlib" /I "..\..\xlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_DLL" /D "_MAGICKMOD_" /D "_MAGICKLIB_" /YX /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /dll /incremental:no /machine:I386 /pdb:"..\bin\CORE_xlib_.pdb" /out:"..\bin\CORE_xlib_.dll"

!ELSEIF  "$(CFG)" == "CORE_xlib - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Debug\CORE_xlib"
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "..\.." /I "..\..\magick" /I "..\..\xlib" /I "..\..\Magick++\lib" /I "..\..\MagickArgs" /I "..\..\zlib" /I "..\..\xlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_DLL" /D "_MAGICKMOD_" /D "_MAGICKLIB_" /YX /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /pdb:"..\bin\CORE_DB_xlib_.pdb" /out:"..\bin\CORE_DB_xlib_.dll"
!ENDIF 

# Begin Target

# Name "CORE_xlib - Win32 Release"
# Name "CORE_xlib - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\xlib\X11_stubs.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
