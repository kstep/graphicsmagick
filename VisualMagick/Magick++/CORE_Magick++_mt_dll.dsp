# Microsoft Developer Studio Project File - Name="CORE_Magick++" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CORE_Magick++ - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CORE_Magick++.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CORE_Magick++.mak" CFG="CORE_Magick++ - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CORE_Magick++ - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CORE_Magick++ - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CORE_Magick++ - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Release\CORE_Magick++"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "..\.." /I "..\..\magick" /I "..\..\xlib" /I "..\..\Magick++\lib" /I "..\..\MagickArgs" /I "..\..\zlib" /I "..\..\Magick++" /I "..\..\Magick++\lib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_DLL" /D "_MAGICKMOD_" /D "_MAGICKLIB_" /YX /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /machine:I386  /out:"..\lib\CORE_Magick++_.lib"

!ELSEIF  "$(CFG)" == "CORE_Magick++ - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "..\Debug\CORE_Magick++"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "." /I "..\.." /I "..\..\magick" /I "..\..\xlib" /I "..\..\Magick++\lib" /I "..\..\MagickArgs" /I "..\..\zlib" /I "..\..\Magick++" /I "..\..\Magick++\lib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_VISUALC_" /D "NeedFunctionPrototypes" /D "_DLL" /D "_MAGICKMOD_" /D "_MAGICKLIB_" /YX /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /machine:I386  /out:"..\lib\CORE_DB_Magick++_.lib"
!ENDIF 

# Begin Target

# Name "CORE_Magick++ - Win32 Release"
# Name "CORE_Magick++ - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Magick++\lib\Blob.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Color.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Drawable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Exception.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Functions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Montage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Options.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Pixels.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\STL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Magick++\lib\Thread.cpp
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Magick++\lib\Magick++.h
# End Source File
# End Group
# End Target
# End Project
