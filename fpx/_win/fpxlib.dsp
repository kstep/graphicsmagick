# Microsoft Developer Studio Project File - Name="fpxlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=fpxlib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fpxlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fpxlib.mak" CFG="fpxlib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fpxlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fpxlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fpxlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\fpx_rls"
# PROP Intermediate_Dir ".\fpx_rls"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /w /W0 /GX /O2 /I "..\basics" /I "..\fpx" /I "..\jpeg" /I "..\ole" /I "..\ri_image" /I "..\macdeps" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "TOOLKIT" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "DOS" /D "USE_OLE" /D NO_FW=0 /D LP_FW=1 /D XVT_FW=2 /D FRAMEWORK=NO_FW /D "TOOLKIT_FlashPix" /D "TOOLKIT_FLASHPIX" /D "USE_UNICODE" /D "_UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib .\jpeg_rls\jpeglib.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "fpxlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\fpx_dbg"
# PROP Intermediate_Dir ".\fpx_dbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /w /W0 /Gm /GX /Zi /Od /I "..\basics" /I "..\fpx" /I "..\jpeg" /I "..\ole" /I "..\ri_image" /I "..\macdeps" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "TOOLKIT" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "DOS" /D "USE_OLE" /D NO_FW=0 /D LP_FW=1 /D XVT_FW=2 /D FRAMEWORK=NO_FW /D "TOOLKIT_FlashPix" /D "TOOLKIT_FLASHPIX" /D "USE_UNICODE" /D "_UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib odbc32.lib odbccp32.lib .\jpeg_dbg\jpeglib.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /dll /debug /machine:I386

!ENDIF 

# Begin Target

# Name "fpxlib - Win32 Release"
# Name "fpxlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Group "Basics"

# PROP Default_Filter "*.cpp;*.cxx;*.c"
# Begin Source File

SOURCE=..\basics\a_file.cpp
# End Source File
# Begin Source File

SOURCE=..\basics\b_string.cpp
# End Source File
# Begin Source File

SOURCE=..\basics\color.cpp
# End Source File
# Begin Source File

SOURCE=..\basics\common_b.cpp
# End Source File
# Begin Source File

SOURCE=..\basics\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\basics\filename.cpp
# End Source File
# Begin Source File

SOURCE=..\basics\geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\basics\graphic.cpp
# End Source File
# End Group
# Begin Group "Ri_image"

# PROP Default_Filter "*.cpp;*.cxx;*.c"
# Begin Source File

SOURCE=..\ri_image\compress.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\corr_lut.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\cp32to24.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\cp_jpeg.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\fn_srch.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\ph_image.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\pimgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\pr_level.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\priimage.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\ptile.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\ri_page.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\ri_sys.cpp
# End Source File
# Begin Source File

SOURCE=..\ri_image\viewimg.cpp
# End Source File
# End Group
# Begin Group "Fpx"

# PROP Default_Filter "*.cpp;*.cxx;*.c"
# Begin Source File

SOURCE=..\fpx\buffdesc.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\coltwist.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\f_fpxio.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\f_fpxvw.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\filter.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\fpxformt.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\fpximgvw.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\fpxlib.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\fpxlibio.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\fpxutils.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\imginfio.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\pres_fpx.cpp
# End Source File
# Begin Source File

SOURCE=..\fpx\ptil_fpx.cpp
# End Source File
# End Group
# Begin Group "Ole"

# PROP Default_Filter "*.cpp;*.cxx;*.c"
# Begin Source File

SOURCE=..\ole\gen_guid.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\oleclink.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\olecore.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\olefiles.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\olehstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\oleprop.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\oleprops.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\olestats.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\olestorg.cpp
# End Source File
# Begin Source File

SOURCE=..\ole\olestrm.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "Basics_h"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\basics\a_file.h
# End Source File
# Begin Source File

SOURCE=..\BASICS\B_ERROR.H
# End Source File
# Begin Source File

SOURCE=..\basics\b_memory.h
# End Source File
# Begin Source File

SOURCE=..\basics\b_string.h
# End Source File
# Begin Source File

SOURCE=..\BASICS\B_TYPES.H
# End Source File
# Begin Source File

SOURCE=..\basics\color.h
# End Source File
# Begin Source File

SOURCE=..\BASICS\COMMON.H
# End Source File
# Begin Source File

SOURCE=..\BASICS\COMMON_A.H
# End Source File
# Begin Source File

SOURCE=..\basics\common_b.h
# End Source File
# Begin Source File

SOURCE=..\basics\debug.h
# End Source File
# Begin Source File

SOURCE=..\basics\filename.h
# End Source File
# Begin Source File

SOURCE=..\BASICS\FILEUTIL.H
# End Source File
# Begin Source File

SOURCE=..\basics\geometry.h
# End Source File
# Begin Source File

SOURCE=..\basics\graphic.h
# End Source File
# Begin Source File

SOURCE=..\BASICS\MAC_COMP.H
# End Source File
# Begin Source File

SOURCE=..\BASICS\NPIXEL.H
# End Source File
# Begin Source File

SOURCE=..\BASICS\NUMBERS.H
# End Source File
# Begin Source File

SOURCE=..\BASICS\P_DEFINE.H
# End Source File
# Begin Source File

SOURCE=..\BASICS\PASCONV.H
# End Source File
# Begin Source File

SOURCE=..\BASICS\PIXEL.H
# End Source File
# Begin Source File

SOURCE=..\BASICS\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=..\BASICS\SWAPBYTE.H
# End Source File
# End Group
# Begin Group "Ole_h"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\ole\gen_guid.h
# End Source File
# Begin Source File

SOURCE=..\ole\oleclink.h
# End Source File
# Begin Source File

SOURCE=..\ole\olecore.h
# End Source File
# Begin Source File

SOURCE=..\ole\olefiles.h
# End Source File
# Begin Source File

SOURCE=..\ole\olehstrm.h
# End Source File
# Begin Source File

SOURCE=..\ole\oleprop.h
# End Source File
# Begin Source File

SOURCE=..\ole\oleprops.h
# End Source File
# Begin Source File

SOURCE=..\ole\olestats.h
# End Source File
# Begin Source File

SOURCE=..\ole\olestorg.h
# End Source File
# Begin Source File

SOURCE=..\ole\olestrm.h
# End Source File
# End Group
# Begin Group "Fpx_h"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\fpx\buffdesc.h
# End Source File
# Begin Source File

SOURCE=..\fpx\coltwist.h
# End Source File
# Begin Source File

SOURCE=..\fpx\f_fpxio.h
# End Source File
# Begin Source File

SOURCE=..\fpx\f_fpxvw.h
# End Source File
# Begin Source File

SOURCE=..\fpx\fpxformt.h
# End Source File
# Begin Source File

SOURCE=..\fpx\fpximgvw.h
# End Source File
# Begin Source File

SOURCE=..\fpx\fpxlib.h
# End Source File
# Begin Source File

SOURCE=..\fpx\fpxlibio.h
# End Source File
# Begin Source File

SOURCE=..\fpx\fpxutils.h
# End Source File
# Begin Source File

SOURCE=..\fpx\imginfio.h
# End Source File
# Begin Source File

SOURCE=..\fpx\pres_fpx.h
# End Source File
# Begin Source File

SOURCE=..\fpx\ptil_fpx.h
# End Source File
# Begin Source File

SOURCE=..\FPX\THMBNAIL.H
# End Source File
# End Group
# Begin Group "Ri_image_h"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\ri_image\compress.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\corr_lut.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\cp32to24.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\cp_jpeg.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\fn_srch.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\matrix.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\ph_image.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\pimgfile.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\pr_level.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\priimage.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\ptile.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\ri_page.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\ri_sys.h
# End Source File
# Begin Source File

SOURCE=..\ri_image\viewimg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\fpxlib.rc
# End Source File
# End Group
# End Target
# End Project
