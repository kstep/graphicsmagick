# Microsoft Developer Studio Project File - Name="IMagick" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IMagick - Win32 Debug static
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IMagick.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IMagick.mak" CFG="IMagick - Win32 Debug static"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IMagick - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IMagick - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IMagick - Win32 Release static" (based on "Win32 (x86) Static Library")
!MESSAGE "IMagick - Win32 Debug static" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "IMagick - Win32 Release"

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
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\xlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo".\Release\version.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\lib\MagickJPEG.lib ..\lib\MagickTIFF.lib ..\lib\MagickPNG.lib ..\lib\MagickZIP.lib ..\lib\MagickJBIG.lib ..\lib\Magickttf.lib ..\lib\MagickFPX.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:"..\bin\IMagick.pdb" /machine:I386 /out:"..\bin\IMagick.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IMagick - Win32 Debug"

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
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\xlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo".\Debug\version.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\lib\MagickJPEGDB.lib ..\lib\MagickTIFFDB.lib ..\lib\MagickPNGDB.lib ..\lib\MagickZIPDB.lib ..\lib\MagickJBIGDB.lib ..\lib\MagickttfDB.lib ..\lib\MagickFPXDB.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"..\bin\IMagickDB.pdb" /debug /machine:I386 /out:"..\bin\IMagickDB.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IMagick - Win32 Release static"

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
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\xlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_LIB" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo".\sRelease\version.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\IMagickstatic.lib"

!ELSEIF  "$(CFG)" == "IMagick - Win32 Debug static"

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
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\xlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_LIB" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo".\sDebug\version.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\IMagickstaticDB.lib"

!ENDIF 

# Begin Target

# Name "IMagick - Win32 Release"
# Name "IMagick - Win32 Debug"
# Name "IMagick - Win32 Release static"
# Name "IMagick - Win32 Debug static"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\magick\8BIM.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\animate.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\annotate.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\avs.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\blob.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\bmp.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\cache.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\cache_io.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\cmyk.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\colors.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\compress.c
# ADD CPP /I "..\..\tiff\libtiff" /I "..\..\zlib"
# End Source File
# Begin Source File

SOURCE=..\..\magick\dcm.c
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

SOURCE=..\..\magick\dps.c
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

SOURCE=..\..\magick\ept.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\error.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\fax.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\fits.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\fpx.c
# ADD CPP /I "..\..\fpx\\"
# End Source File
# Begin Source File

SOURCE=..\..\magick\gems.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\gif.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\gradation.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\gray.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\hdf.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\histogram.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\html.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\icc.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\icon.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\image.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\iptc.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\jbig.c
# ADD CPP /I "..\..\jbig\libjbig"
# End Source File
# Begin Source File

SOURCE=..\..\magick\jpeg.c
# ADD CPP /I "..\..\jpeg"
# End Source File
# Begin Source File

SOURCE=..\..\magick\label.c
# ADD CPP /I "..\..\ttf\lib"
# End Source File
# Begin Source File

SOURCE=..\..\magick\logo.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\mac.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\magick.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\map.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\matte.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\memory.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\miff.c
# ADD CPP /I "..\..\zlib"
# End Source File
# Begin Source File

SOURCE=..\..\magick\monitor.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\mono.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\montage.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\mtv.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\nt.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\null.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\pcd.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\pcl.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\pcx.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\pdf.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\pict.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\pix.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\plasma.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\png.c
# ADD CPP /I "..\..\png" /I "..\..\zlib"
# End Source File
# Begin Source File

SOURCE=..\..\magick\pnm.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\PreRvIcccm.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\preview.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\ps.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\ps2.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\ps3.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\psd.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\pwp.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\quantize.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\rgb.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\rla.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\rle.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\sct.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\segment.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\sfw.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\sgi.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\shear.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\signature.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\stegano.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\sun.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\tga.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\tiff.c
# ADD CPP /I "..\..\tiff\libtiff"
# End Source File
# Begin Source File

SOURCE=..\..\magick\tile.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\tim.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\timer.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\transform.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\ttf.c
# ADD CPP /I "..\..\ttf\lib"
# End Source File
# Begin Source File

SOURCE=..\..\magick\txt.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\uil.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\utility.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\uyvy.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\vicar.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\vid.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\viff.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\vms.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\widget.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\X.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\xbm.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\xc.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\xpm.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\xwd.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\xwindows.c
# End Source File
# Begin Source File

SOURCE=..\..\magick\yuv.c
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

SOURCE=..\..\magick\blob.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\cache.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\cache_io.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\classify.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\Colorlist.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\compress.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\config.h
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

SOURCE=..\..\magick\formats.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\gems.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\image.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\logo.h
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

SOURCE=..\..\magick\monitor.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\nt.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\PreRvIcccm.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\proxy.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\quantize.h
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

SOURCE=..\..\magick\XWDFile.h
# End Source File
# Begin Source File

SOURCE=..\..\magick\xwindows.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\README.txt
# End Source File
# Begin Source File

SOURCE=.\Xmd.nth

!IF  "$(CFG)" == "IMagick - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\Xmd.nth

"..\..\xlib\X11\Xmd.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\Xmd.nth ..\..\xlib\X11\Xmd.h

# End Custom Build

!ELSEIF  "$(CFG)" == "IMagick - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\Xmd.nth

"..\..\xlib\X11\Xmd.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\Xmd.nth ..\..\xlib\X11\Xmd.h

# End Custom Build

!ELSEIF  "$(CFG)" == "IMagick - Win32 Release static"

# Begin Custom Build
ProjDir=.
InputPath=.\Xmd.nth

"..\..\xlib\X11\Xmd.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\Xmd.nth ..\..\xlib\X11\Xmd.h

# End Custom Build

!ELSEIF  "$(CFG)" == "IMagick - Win32 Debug static"

# Begin Custom Build
ProjDir=.
InputPath=.\Xmd.nth

"..\..\xlib\X11\Xmd.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(ProjDir)\Xmd.nth ..\..\xlib\X11\Xmd.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\X11.lib
# End Source File
# End Target
# End Project
