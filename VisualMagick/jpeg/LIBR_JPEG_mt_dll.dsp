# Microsoft Developer Studio Project File - Name="LIBR_JPEG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LIBR_JPEG - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_JPEG_mt_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_JPEG_mt_dll.mak" CFG="LIBR_JPEG - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIBR_JPEG - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LIBR_JPEG - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIBR_JPEG - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "_IMAGE_MAGICK_JPEG_" /D "_JPEGLIB_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x66200000" /subsystem:windows /dll /machine:I386 /out:"..\bin\LIBR_RL_JPEG_.dll"

!ELSEIF  "$(CFG)" == "LIBR_JPEG - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "_IMAGE_MAGICK_JPEG_" /D "_JPEGLIB_" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x66200000" /subsystem:windows /dll /incremental:no /pdb:"..\bin\LIBR_DB_JPEG_.pdb" /debug /machine:I386 /out:"..\bin\LIBR_DB_JPEG_.dll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LIBR_JPEG - Win32 Release"
# Name "LIBR_JPEG - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\jpeg\jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jccolor.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jchuff.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcinit.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcparam.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jcsample.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdatadst.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jerror.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jutils.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\jpeg\jchuff.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdct.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jdhuff.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jerror.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jinclude.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jmemsys.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jpegint.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jpeglib.h
# End Source File
# Begin Source File

SOURCE=..\..\jpeg\jversion.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\version.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\jconfig.nth

!IF  "$(CFG)" == "LIBR_JPEG - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\jconfig.nth

"..\..\jpeg\jconfig.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\jconfig.nth ..\..\jpeg\jconfig.h

# End Custom Build

!ELSEIF  "$(CFG)" == "LIBR_JPEG - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\jconfig.nth

"..\..\jpeg\jconfig.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\jconfig.nth ..\..\jpeg\jconfig.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jmorecfg.nth

!IF  "$(CFG)" == "LIBR_JPEG - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\jmorecfg.nth

"..\..\jpeg\jmorecfg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\jmorecfg.nth ..\..\jpeg\jmorecfg.h

# End Custom Build

!ELSEIF  "$(CFG)" == "LIBR_JPEG - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\jmorecfg.nth

"..\..\jpeg\jmorecfg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\jmorecfg.nth ..\..\jpeg\jmorecfg.h

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
