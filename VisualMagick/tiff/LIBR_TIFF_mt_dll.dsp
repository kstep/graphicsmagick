# Microsoft Developer Studio Project File - Name="LIBR_TIFF" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LIBR_TIFF - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_TIFF_mt_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_TIFF_mt_dll.mak" CFG="LIBR_TIFF - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIBR_TIFF - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LIBR_TIFF - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIBR_TIFF - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\dRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "..\..\jpeg" /I "..\..\zlib" /D "WIN32" /D "__WIN32__" /D "NDEBUG" /D "_WINDOWS" /D "_VISUALC_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x66400000" /subsystem:windows /dll /machine:I386 /def:".\LIBR_RL_TIFF_.def" /out:"..\bin\LIBR_RL_TIFF_.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "LIBR_TIFF - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\dDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "..\..\jpeg" /I "..\..\zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_VISUALC_" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x66400000" /subsystem:windows /dll /incremental:no /pdb:"..\bin\LIBR_DB_TIFF_.pdb" /debug /machine:I386 /def:".\LIBR_DB_TIFF_.def" /out:"..\bin\LIBR_DB_TIFF_.dll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LIBR_TIFF - Win32 Release"
# Name "LIBR_TIFF - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\tiff\libtiff\fax3sm_winnt.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_aux.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_close.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_codec.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_compress.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_dir.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_dirinfo.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_dirread.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_dirwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_dumpmode.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_error.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_fax3.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_flush.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_getimage.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_jpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_luv.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_lzw.c

!IF  "$(CFG)" == "LIBR_TIFF - Win32 Release"

# ADD CPP /Ot /Oi /Oy

!ELSEIF  "$(CFG)" == "LIBR_TIFF - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_next.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_open.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_packbits.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_pixarlog.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_predict.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_print.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_read.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_strip.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_swab.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_thunder.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_tile.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_version.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_warning.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_win32.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_write.c
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_zip.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\tiff\libtiff\port.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\t4.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_dir.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_fax3.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tif_predict.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tiff.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tiffcomp.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tiffconf.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tiffio.h
# End Source File
# Begin Source File

SOURCE=..\..\tiff\libtiff\tiffiop.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\version.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\port.nth

!IF  "$(CFG)" == "LIBR_TIFF - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\port.nth

"..\..\tiff\libtiff\port.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\port.nth ..\..\tiff\libtiff\port.h

# End Custom Build

!ELSEIF  "$(CFG)" == "LIBR_TIFF - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\port.nth

"..\..\tiff\libtiff\port.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\port.nth ..\..\tiff\libtiff\port.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tiffconf.nth

!IF  "$(CFG)" == "LIBR_TIFF - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\tiffconf.nth

"..\..\tiff\libtiff\tiffconf.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\tiffconf.nth ..\..\tiff\libtiff\tiffconf.h

# End Custom Build

!ELSEIF  "$(CFG)" == "LIBR_TIFF - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\tiffconf.nth

"..\..\tiff\libtiff\tiffconf.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\tiffconf.nth ..\..\tiff\libtiff\tiffconf.h

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
