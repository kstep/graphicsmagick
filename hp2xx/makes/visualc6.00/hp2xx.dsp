# Microsoft Developer Studio Project File - Name="hp2xx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=hp2xx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hp2xx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hp2xx.mak" CFG="hp2xx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hp2xx - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "hp2xx - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hp2xx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "EMF" /D "_NO_VCL" /D "NORINT" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "hp2xx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_NO_VCL" /D "NORINT" /D "EMF" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "hp2xx - Win32 Release"
# Name "hp2xx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bresnham.c
# End Source File
# Begin Source File

SOURCE=.\chardraw.c
# End Source File
# Begin Source File

SOURCE=.\clip.c
# End Source File
# Begin Source File

SOURCE=.\fillpoly.c
# End Source File
# Begin Source File

SOURCE=.\getopt.c
# End Source File
# Begin Source File

SOURCE=.\getopt1.c
# End Source File
# Begin Source File

SOURCE=.\hp2xx.c
# End Source File
# Begin Source File

SOURCE=.\hpgl.c
# End Source File
# Begin Source File

SOURCE=.\lindef.c
# End Source File
# Begin Source File

SOURCE=.\no_prev.c
# End Source File
# Begin Source File

SOURCE=.\pendef.c
# End Source File
# Begin Source File

SOURCE=.\picbuf.c
# End Source File
# Begin Source File

SOURCE=.\std_main.c
# End Source File
# Begin Source File

SOURCE=.\to_emf.c
# End Source File
# Begin Source File

SOURCE=.\to_eps.c
# End Source File
# Begin Source File

SOURCE=.\to_escp2.c
# End Source File
# Begin Source File

SOURCE=.\to_fig.c
# End Source File
# Begin Source File

SOURCE=.\to_ilbm.c
# End Source File
# Begin Source File

SOURCE=.\to_img.c
# End Source File
# Begin Source File

SOURCE=.\to_pac.c
# End Source File
# Begin Source File

SOURCE=.\to_pbm.c
# End Source File
# Begin Source File

SOURCE=.\to_pcl.c
# End Source File
# Begin Source File

SOURCE=.\to_pcx.c
# End Source File
# Begin Source File

SOURCE=.\to_pic.c
# End Source File
# Begin Source File

SOURCE=.\to_rgip.c

!IF  "$(CFG)" == "hp2xx - Win32 Release"

!ELSEIF  "$(CFG)" == "hp2xx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\to_vec.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
