# Microsoft Developer Studio Project File - Name="IMagickMOD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IMagickMOD - Win32 Debug static
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IMagickMOD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IMagickMOD.mak" CFG="IMagickMOD - Win32 Debug static"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IMagickMOD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IMagickMOD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IMagickMOD - Win32 Release static" (based on "Win32 (x86) Static Library")
!MESSAGE "IMagickMOD - Win32 Debug static" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "IMagickMOD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\dmRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\xlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_MAGICKLIB_" /D "_MAGICKMOD_" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo".\dmRelease\version.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\lib\MagickJPEG.lib ..\lib\MagickTIFF.lib ..\lib\MagickPNG.lib ..\lib\MagickZIP.lib ..\lib\MagickJBIG.lib ..\lib\Magickttf.lib ..\lib\MagickFPX.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:"..\bin\IMagickMOD.pdb" /machine:I386 /out:"..\bin\IMagickMOD.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IMagickMOD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\dmDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\xlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_MAGICKLIB_" /D "_MAGICKMOD_" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo".\dmDebug\version.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\lib\MagickJPEGDB.lib ..\lib\MagickTIFFDB.lib ..\lib\MagickPNGDB.lib ..\lib\MagickZIPDB.lib ..\lib\MagickJBIGDB.lib ..\lib\MagickttfDB.lib ..\lib\MagickFPXDB.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"..\bin\IMagickMODDB.pdb" /debug /machine:I386 /out:"..\bin\IMagickMODDB.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IMagickMOD - Win32 Release static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\smRelease"
# PROP Target_Dir ""
MTL=midl.exe
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\xlib" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_MAGICKLIB_" /D "_MAGICKMOD_" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo".\smRelease\version.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\IMagickMODstatic.lib"

!ELSEIF  "$(CFG)" == "IMagickMOD - Win32 Debug static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\smDebug"
# PROP Target_Dir ""
MTL=midl.exe
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\xlib" /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_MAGICKLIB_" /D "_MAGICKMOD_" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo".\smDebug\version.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\IMagickMODstaticDB.lib"

!ENDIF 

# Begin Target

# Name "IMagickMOD - Win32 Release"
# Name "IMagickMOD - Win32 Debug"
# Name "IMagickMOD - Win32 Release static"
# Name "IMagickMOD - Win32 Debug static"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
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
# ADD CPP /I "..\..\tiff\libtiff" /I "..\..\zlib"
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
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
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

SOURCE=..\..\magick\config.h.in
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

SOURCE=..\..\magick\magick.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\memory.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\mng.h
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
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\Xmd.nth

!IF  "$(CFG)" == "IMagickMOD - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\Xmd.nth

"..\..\xlib\X11\Xmd.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\Xmd.nth ..\..\xlib\X11\Xmd.h

# End Custom Build

!ELSEIF  "$(CFG)" == "IMagickMOD - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\Xmd.nth

"..\..\xlib\X11\Xmd.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\Xmd.nth ..\..\xlib\X11\Xmd.h

# End Custom Build

!ELSEIF  "$(CFG)" == "IMagickMOD - Win32 Release static"

# Begin Custom Build
ProjDir=.
InputPath=.\Xmd.nth

"..\..\xlib\X11\Xmd.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\Xmd.nth ..\..\xlib\X11\Xmd.h

# End Custom Build

!ELSEIF  "$(CFG)" == "IMagickMOD - Win32 Debug static"

# Begin Custom Build
ProjDir=.
InputPath=.\Xmd.nth

"..\..\xlib\X11\Xmd.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\Xmd.nth ..\..\xlib\X11\Xmd.h

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
