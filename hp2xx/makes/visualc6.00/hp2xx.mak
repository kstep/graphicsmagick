# Microsoft Developer Studio Generated NMAKE File, Based on hp2xx.dsp
!IF "$(CFG)" == ""
CFG=hp2xx - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hp2xx - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hp2xx - Win32 Release" && "$(CFG)" != "hp2xx - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "hp2xx - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\hp2xx.exe"


CLEAN :
	-@erase "$(INTDIR)\bresnham.obj"
	-@erase "$(INTDIR)\chardraw.obj"
	-@erase "$(INTDIR)\clip.obj"
	-@erase "$(INTDIR)\fillpoly.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt1.obj"
	-@erase "$(INTDIR)\hp2xx.obj"
	-@erase "$(INTDIR)\hpgl.obj"
	-@erase "$(INTDIR)\lindef.obj"
	-@erase "$(INTDIR)\no_prev.obj"
	-@erase "$(INTDIR)\pendef.obj"
	-@erase "$(INTDIR)\picbuf.obj"
	-@erase "$(INTDIR)\std_main.obj"
	-@erase "$(INTDIR)\to_emf.obj"
	-@erase "$(INTDIR)\to_eps.obj"
	-@erase "$(INTDIR)\to_escp2.obj"
	-@erase "$(INTDIR)\to_fig.obj"
	-@erase "$(INTDIR)\to_ilbm.obj"
	-@erase "$(INTDIR)\to_img.obj"
	-@erase "$(INTDIR)\to_pac.obj"
	-@erase "$(INTDIR)\to_pbm.obj"
	-@erase "$(INTDIR)\to_pcl.obj"
	-@erase "$(INTDIR)\to_pcx.obj"
	-@erase "$(INTDIR)\to_pic.obj"
	-@erase "$(INTDIR)\to_rgip.obj"
	-@erase "$(INTDIR)\to_vec.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hp2xx.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "EMF" /D "_NO_VCL" /D "NORINT" /Fp"$(INTDIR)\hp2xx.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hp2xx.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\hp2xx.pdb" /machine:I386 /out:"$(OUTDIR)\hp2xx.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bresnham.obj" \
	"$(INTDIR)\chardraw.obj" \
	"$(INTDIR)\clip.obj" \
	"$(INTDIR)\fillpoly.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\getopt1.obj" \
	"$(INTDIR)\hp2xx.obj" \
	"$(INTDIR)\hpgl.obj" \
	"$(INTDIR)\lindef.obj" \
	"$(INTDIR)\no_prev.obj" \
	"$(INTDIR)\pendef.obj" \
	"$(INTDIR)\picbuf.obj" \
	"$(INTDIR)\std_main.obj" \
	"$(INTDIR)\to_emf.obj" \
	"$(INTDIR)\to_eps.obj" \
	"$(INTDIR)\to_escp2.obj" \
	"$(INTDIR)\to_fig.obj" \
	"$(INTDIR)\to_ilbm.obj" \
	"$(INTDIR)\to_img.obj" \
	"$(INTDIR)\to_pac.obj" \
	"$(INTDIR)\to_pbm.obj" \
	"$(INTDIR)\to_pcl.obj" \
	"$(INTDIR)\to_pcx.obj" \
	"$(INTDIR)\to_pic.obj" \
	"$(INTDIR)\to_rgip.obj" \
	"$(INTDIR)\to_vec.obj"

"$(OUTDIR)\hp2xx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hp2xx - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\hp2xx.exe"


CLEAN :
	-@erase "$(INTDIR)\bresnham.obj"
	-@erase "$(INTDIR)\chardraw.obj"
	-@erase "$(INTDIR)\clip.obj"
	-@erase "$(INTDIR)\fillpoly.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt1.obj"
	-@erase "$(INTDIR)\hp2xx.obj"
	-@erase "$(INTDIR)\hpgl.obj"
	-@erase "$(INTDIR)\lindef.obj"
	-@erase "$(INTDIR)\no_prev.obj"
	-@erase "$(INTDIR)\pendef.obj"
	-@erase "$(INTDIR)\picbuf.obj"
	-@erase "$(INTDIR)\std_main.obj"
	-@erase "$(INTDIR)\to_emf.obj"
	-@erase "$(INTDIR)\to_eps.obj"
	-@erase "$(INTDIR)\to_escp2.obj"
	-@erase "$(INTDIR)\to_fig.obj"
	-@erase "$(INTDIR)\to_ilbm.obj"
	-@erase "$(INTDIR)\to_img.obj"
	-@erase "$(INTDIR)\to_pac.obj"
	-@erase "$(INTDIR)\to_pbm.obj"
	-@erase "$(INTDIR)\to_pcl.obj"
	-@erase "$(INTDIR)\to_pcx.obj"
	-@erase "$(INTDIR)\to_pic.obj"
	-@erase "$(INTDIR)\to_rgip.obj"
	-@erase "$(INTDIR)\to_vec.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\hp2xx.exe"
	-@erase "$(OUTDIR)\hp2xx.ilk"
	-@erase "$(OUTDIR)\hp2xx.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_NO_VCL" /D "NORINT" /D "EMF" /Fp"$(INTDIR)\hp2xx.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hp2xx.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\hp2xx.pdb" /debug /machine:I386 /out:"$(OUTDIR)\hp2xx.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\bresnham.obj" \
	"$(INTDIR)\chardraw.obj" \
	"$(INTDIR)\clip.obj" \
	"$(INTDIR)\fillpoly.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\getopt1.obj" \
	"$(INTDIR)\hp2xx.obj" \
	"$(INTDIR)\hpgl.obj" \
	"$(INTDIR)\lindef.obj" \
	"$(INTDIR)\no_prev.obj" \
	"$(INTDIR)\pendef.obj" \
	"$(INTDIR)\picbuf.obj" \
	"$(INTDIR)\std_main.obj" \
	"$(INTDIR)\to_emf.obj" \
	"$(INTDIR)\to_eps.obj" \
	"$(INTDIR)\to_escp2.obj" \
	"$(INTDIR)\to_fig.obj" \
	"$(INTDIR)\to_ilbm.obj" \
	"$(INTDIR)\to_img.obj" \
	"$(INTDIR)\to_pac.obj" \
	"$(INTDIR)\to_pbm.obj" \
	"$(INTDIR)\to_pcl.obj" \
	"$(INTDIR)\to_pcx.obj" \
	"$(INTDIR)\to_pic.obj" \
	"$(INTDIR)\to_rgip.obj" \
	"$(INTDIR)\to_vec.obj"

"$(OUTDIR)\hp2xx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("hp2xx.dep")
!INCLUDE "hp2xx.dep"
!ELSE 
!MESSAGE Warning: cannot find "hp2xx.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hp2xx - Win32 Release" || "$(CFG)" == "hp2xx - Win32 Debug"
SOURCE=.\bresnham.c

"$(INTDIR)\bresnham.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\chardraw.c

"$(INTDIR)\chardraw.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\clip.c

"$(INTDIR)\clip.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\fillpoly.c

"$(INTDIR)\fillpoly.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\getopt.c

"$(INTDIR)\getopt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\getopt1.c

"$(INTDIR)\getopt1.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hp2xx.c

"$(INTDIR)\hp2xx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hpgl.c

"$(INTDIR)\hpgl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lindef.c

"$(INTDIR)\lindef.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\no_prev.c

"$(INTDIR)\no_prev.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pendef.c

"$(INTDIR)\pendef.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\picbuf.c

"$(INTDIR)\picbuf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\std_main.c

"$(INTDIR)\std_main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_emf.c

"$(INTDIR)\to_emf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_eps.c

"$(INTDIR)\to_eps.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_escp2.c

"$(INTDIR)\to_escp2.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_fig.c

"$(INTDIR)\to_fig.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_ilbm.c

"$(INTDIR)\to_ilbm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_img.c

"$(INTDIR)\to_img.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_pac.c

"$(INTDIR)\to_pac.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_pbm.c

"$(INTDIR)\to_pbm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_pcl.c

"$(INTDIR)\to_pcl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_pcx.c

"$(INTDIR)\to_pcx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_pic.c

"$(INTDIR)\to_pic.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_rgip.c

"$(INTDIR)\to_rgip.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\to_vec.c

"$(INTDIR)\to_vec.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

