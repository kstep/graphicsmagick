# Microsoft Developer Studio Project File - Name="LIBR_FPX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LIBR_FPX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_FPX_st_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_FPX_st_lib.mak" CFG="LIBR_FPX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIBR_FPX - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LIBR_FPX - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIBR_FPX - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\fpx\basics" /I "..\..\fpx\fpx" /I "..\..\fpx\jpeg" /I "..\..\fpx\ole" /I "..\..\fpx\ri_image" /I "..\..\fpx\macdeps" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "TOOLKIT" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "DOS" /D "USE_OLE" /D NO_FW=0 /D LP_FW=1 /D XVT_FW=2 /D FRAMEWORK=NO_FW /D "TOOLKIT_FlashPix" /D "TOOLKIT_FLASHPIX" /D "USE_UNICODE" /D "_UNICODE" /D "_VISUALC_" /D "_IMAGE_MAGICK_FPX_" /D "_FPXLIB_" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\LIBR_RL_FPX_.lib"

!ELSEIF  "$(CFG)" == "LIBR_FPX - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\fpx\basics" /I "..\..\fpx\fpx" /I "..\..\fpx\jpeg" /I "..\..\fpx\ole" /I "..\..\fpx\ri_image" /I "..\..\fpx\macdeps" /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "TOOLKIT" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "DOS" /D "USE_OLE" /D NO_FW=0 /D LP_FW=1 /D XVT_FW=2 /D FRAMEWORK=NO_FW /D "TOOLKIT_FlashPix" /D "TOOLKIT_FLASHPIX" /D "USE_UNICODE" /D "_UNICODE" /D "_VISUALC_" /D "_IMAGE_MAGICK_FPX_" /D "_FPXLIB_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\LIBR_DB_FPX_.lib"

!ENDIF 

# Begin Target

# Name "LIBR_FPX - Win32 Release"
# Name "LIBR_FPX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Group "Basics"

# PROP Default_Filter "*.cpp;*.cxx;*.c"
# Begin Source File

SOURCE=..\..\fpx\basics\a_file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\b_string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\color.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\common_b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\filename.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\graphic.cpp
# End Source File
# End Group
# Begin Group "Ri_image"

# PROP Default_Filter "*.cpp;*.cxx;*.c"
# Begin Source File

SOURCE=..\..\fpx\ri_image\compress.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\corr_lut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\cp32to24.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\cp_jpeg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\fn_srch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\ph_image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\pimgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\pr_level.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\priimage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\ptile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\ri_page.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\ri_sys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\viewimg.cpp
# End Source File
# End Group
# Begin Group "Fpx"

# PROP Default_Filter "*.cpp;*.cxx;*.c"
# Begin Source File

SOURCE=..\..\fpx\fpx\buffdesc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\coltwist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\f_fpxio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\f_fpxvw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpxformt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpximgvw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpxlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpxlibio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpxutils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\imginfio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\pres_fpx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\ptil_fpx.cpp
# End Source File
# End Group
# Begin Group "Ole"

# PROP Default_Filter "*.cpp;*.cxx;*.c"
# Begin Source File

SOURCE=..\..\fpx\ole\gen_guid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\oleclink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olecore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olefiles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olehstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\oleprop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\oleprops.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olestats.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olestorg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olestrm.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "Basics_h"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\..\fpx\basics\a_file.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\B_ERROR.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\b_memory.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\b_string.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\B_TYPES.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\color.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\COMMON.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\COMMON_A.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\common_b.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\filename.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\FILEUTIL.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\basics\graphic.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\MAC_COMP.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\NPIXEL.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\NUMBERS.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\P_DEFINE.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\PASCONV.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\PIXEL.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\BASICS\SWAPBYTE.H
# End Source File
# End Group
# Begin Group "Ole_h"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\..\fpx\ole\gen_guid.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\oleclink.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olecore.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olefiles.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olehstrm.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\oleprop.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\oleprops.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olestats.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olestorg.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ole\olestrm.h
# End Source File
# End Group
# Begin Group "Fpx_h"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\..\fpx\fpx\buffdesc.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\coltwist.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\f_fpxio.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\f_fpxvw.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpxformt.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpximgvw.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpxlib.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpxlibio.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\fpxutils.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\imginfio.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\pres_fpx.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\fpx\ptil_fpx.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\FPX\THMBNAIL.H
# End Source File
# End Group
# Begin Group "Ri_image_h"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\..\fpx\ri_image\compress.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\corr_lut.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\cp32to24.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\cp_jpeg.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\fn_srch.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\ph_image.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\pimgfile.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\pr_level.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\priimage.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\ptile.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\ri_page.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\ri_sys.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\ri_image\viewimg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\LIBR_FPX.rc
# End Source File
# End Group
# End Target
# End Project
