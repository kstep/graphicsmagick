# Microsoft Developer Studio Project File - Name="jpeglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=jpeglib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpeglib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeglib.mak" CFG="jpeglib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeglib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "jpeglib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpeglib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\jpeg_rls"
# PROP Intermediate_Dir ".\jpeg_rls"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\jpeg_dbg"
# PROP Intermediate_Dir ".\jpeg_dbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386

!ENDIF 

# Begin Target

# Name "jpeglib - Win32 Release"
# Name "jpeglib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\jpeg\chen_dct.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\dbuffer.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\decoder.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\dectile.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\dhuff.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\djpeg.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\dllmain.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\dparser.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\dscan.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\ebuffer.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\ejpeg.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\enctile.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\eparser.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\fpxmem.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\huffman.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\mcu2rast.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\process.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\public.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\tables.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\win_dct.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\winograd.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\jpeg\chen_dct.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\dbuffer.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\decoder.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\dectile.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\dhuff.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\djpeg.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\dparser.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\ebuffer.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\ejpeg.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\enctile.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\eparser.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\FPXMem.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\huffman.h
# End Source File
# Begin Source File

SOURCE=..\JPEG\jpegconf.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\public.h
# End Source File
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=..\jpeg\tables.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\win_dct.h
# End Source File
# Begin Source File

SOURCE=..\jpeg\winograd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\jpeglib.rc
# End Source File
# End Group
# End Target
# End Project
