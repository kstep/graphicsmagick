# Microsoft Developer Studio Project File - Name="LIBR_PNG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LIBR_PNG - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_PNG_st_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_PNG_st_lib.mak" CFG="LIBR_PNG - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIBR_PNG - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LIBR_PNG - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIBR_PNG - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__FLAT__" /D "_VISUALC_" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\LIBR_PNG.lib"

!ELSEIF  "$(CFG)" == "LIBR_PNG - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\sDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__FLAT__" /D "_VISUALC_" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\LIBR_DB_PNG.lib"

!ENDIF 

# Begin Target

# Name "LIBR_PNG - Win32 Release"
# Name "LIBR_PNG - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\png\png.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngget.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngread.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngset.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\..\png\pngwutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\png\png.h
# End Source File
# Begin Source File

SOURCE=..\..\png\pngconf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\version.rc
# End Source File
# End Group
# End Target
# End Project
