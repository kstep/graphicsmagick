# Microsoft Developer Studio Project File - Name="Magick++" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Magick++ - Win32 Debug static
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Magick++.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Magick++.mak" CFG="Magick++ - Win32 Debug static"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Magick++ - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Magick++ - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Magick++ - Win32 Release static" (based on "Win32 (x86) Static Library")
!MESSAGE "Magick++ - Win32 Debug static" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Magick++ - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir ".\dRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\..\\" /I "..\..\Magick++\lib" /I "..\..\magick" /I "..\..\jpeg" /I "..\..\mpeg" /I "..\..\tiff" /I "..\..\ttf" /I "..\..\png" /I "..\..\zlib" /I "..\..\jbig" /I "..\..\fpx" /I "..\..\xlib\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Magick++.lib"

!ELSEIF  "$(CFG)" == "Magick++ - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir ".\dDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\\" /I "..\..\\" /I "..\..\Magick++\lib" /I "..\..\magick" /I "..\..\jpeg" /I "..\..\mpeg" /I "..\..\tiff" /I "..\..\ttf" /I "..\..\png" /I "..\..\zlib" /I "..\..\jbig" /I "..\..\fpx" /I "..\..\xlib\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Magick++DB.lib"

!ELSEIF  "$(CFG)" == "Magick++ - Win32 Release static"

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
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I "..\..\\" /I "..\..\Magick++\lib" /I "..\..\magick" /I "..\..\jpeg" /I "..\..\mpeg" /I "..\..\tiff" /I "..\..\ttf" /I "..\..\png" /I "..\..\zlib" /I "..\..\jbig" /I "..\..\fpx" /I "..\..\xlib\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Magick++Static.lib"

!ELSEIF  "$(CFG)" == "Magick++ - Win32 Debug static"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I ".\\" /I "..\..\\" /I "..\..\Magick++\lib" /I "..\..\magick" /I "..\..\jpeg" /I "..\..\mpeg" /I "..\..\tiff" /I "..\..\ttf" /I "..\..\png" /I "..\..\zlib" /I "..\..\jbig" /I "..\..\fpx" /I "..\..\xlib\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Magick++StaticDB.lib"

!ENDIF 

# Begin Target

# Name "Magick++ - Win32 Release"
# Name "Magick++ - Win32 Debug"
# Name "Magick++ - Win32 Release static"
# Name "Magick++ - Win32 Debug static"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE="..\..\Magick++\lib\Blob.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Color.cpp"

!IF  "$(CFG)" == "Magick++ - Win32 Release"

!ELSEIF  "$(CFG)" == "Magick++ - Win32 Debug"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "Magick++ - Win32 Release static"

!ELSEIF  "$(CFG)" == "Magick++ - Win32 Debug static"

# ADD BASE CPP /I "."
# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Drawable.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Exception.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Functions.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Geometry.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Image.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Montage.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Options.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Pixels.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Thread.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Color.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Drawable.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Exception.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Functions.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Geometry.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Image.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Include.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Montage.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Options.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Pixels.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\STL.h"
# End Source File
# Begin Source File

SOURCE="..\..\Magick++\lib\Magick++\Thread.h"
# End Source File
# End Group
# End Target
# End Project
