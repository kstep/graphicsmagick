# Microsoft Developer Studio Project File - Name="CORE_magick" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CORE_magick - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CORE_magick.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CORE_magick.mak" CFG="CORE_magick - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CORE_magick - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CORE_magick - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CORE_magick - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Release\CORE_magick"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "..\.." /I "..\..\magick" /I "..\..\xlib" /I "..\..\Magick++\lib" /I "..\..\MagickArgs" /I "..\..\zlib" /I "..\..\magick" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_LIB" /D "_MAGICKLIB_" /YX /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /machine:I386  /out:"..\lib\CORE_magick_.lib"

!ELSEIF  "$(CFG)" == "CORE_magick - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Debug\CORE_magick"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "." /I "..\.." /I "..\..\magick" /I "..\..\xlib" /I "..\..\Magick++\lib" /I "..\..\MagickArgs" /I "..\..\zlib" /I "..\..\magick" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_LIB" /D "_MAGICKLIB_" /YX /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /machine:I386  /out:"..\lib\CORE_DB_magick_.lib"
!ENDIF 

# Begin Target

# Name "CORE_magick - Win32 Release"
# Name "CORE_magick - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\magick\animate.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\annotate.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\attributes.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\blob.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\cache_view.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\colors.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\constitute.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\decorate.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\delegates.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\display.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\draw.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\effects.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\enhance.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\error.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\gems.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\image.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\mac.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\magic.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\magick.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\memory.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\modules.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\monitor.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\montage.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\nt.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\PreRvIcccm.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\quantize.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\segment.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\shear.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\signature.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\stash.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\stream.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\timer.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\transform.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\utility.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\vms.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\widget.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\xwindows.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\zoom.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\magick\acconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\animate.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\api.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\attributes.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\blob.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\cache_view.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\classify.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\compress.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\defines.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\delegates.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\display.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\error.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\gems.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\image.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\mac.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\magic.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\magick.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\memory.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\modules.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\monitor.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\nt.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\PreRvIcccm.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\quantize.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\stash.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\utility.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\version.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\vms.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\widget.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\xwindows.h
# End Source File
# End Group
# End Target
# End Project
