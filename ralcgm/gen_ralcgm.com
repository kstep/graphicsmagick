$!============================================================================
$!
$! DCL procedure to build the RALCGM package on VAX/VMS
$!
$! Nick Hill
$!
$! Computer Graphics Group,
$! Central Computing Department,
$! Rutherford Appleton Laboratory,
$! Chilton, Didcot, Oxfordshire
$! OX11 0QX, ENGLAND.
$!
$! NMH1@UK.AC.RL.VE
$!
$! Mods
$! 28-8-91  NMH  Added CGMOVGA to list of files to ignore on compilation
$! 24-9-91  NMH  Added setting 13 or 35 PS font set for font system
$! 24-9-91  NMH  Added better checking for definition of edit
$! 01-1-92  NMH  Added renaming of object library if RALCGM.OLB already exists
$! 8-12-92  NMH  Added TEK4200 and HPGL options
$!============================================================================
$!
$X_WIN ="N"
$TEK4200 = "N"
$PS = "N"
$PS_OPT ="N"
$HPGL = "N"
$HER = "N"
$BEZ = "N"
$ed_type= ""
$isalpha = "N"
$!
$! Set cc command for ALPHA
$if f$search("sys$common:[sysexe]vmb.exe") .eqs. ""
$then
$  cc := cc/standard=vaxc
$  isalpha = "Y"
$endif
$!set noon
$save_dir = f$env("default")
$on CONTROL_C then $ goto stop
$on CONTROL_Y then $goto stop
$on ERROR then $ goto error
$!=============================================
$! check on defininition of edit -- we need edt
$!=============================================
$if f$type(edit).nes."" then $ ed_type = edit
$edit :== edit/edt
$!==========================
$! determine directory paths
$!==========================
$master = f$env("default")
$define/nolog mast_dir  'master
$source=master-"]"+".SRC]"
$define/nolog source_dir 'source
$inclu=master-"]"+".INCLUDE]"
$define/nolog inc_dir 'inclu
$data=master-"]"+".DATA]"
$define/nolog data_dir 'data
$bin=master-"]"+".BIN]"
$define/nolog bin_dir 'bin
$docs=master-"]"+".DOCS]"
$define/nolog docs_dir 'docs
$!=============================================
$! build names for library and executable files
$!=============================================
$lib_name= "bin_dir:ralcgm.olb"
$exe_name= "bin_dir:ralcgm"
$!====================
$! check for link only
$!====================
$if p1.nes."" then $goto link
$!============================================
$! if the library doesn't exist then create it
$!============================================
$file=f$search(lib_name)
$if file.eqs.""
$then
$  library/create 'lib_name
$else
$  write sys$output "A RALCGM object library already exists - using RALCGM3.OLB "
$  lib_name= "bin_dir:ralcgm3.olb"
$  library/create 'lib_name
$endif
$!=======================================================
$! make sure we have the correct mach.h file
$!=======================================================
$copy/nolog sys$input: sys$output:






                          ****************************
                          *   RALCGM  Installation   *
                          ****************************

This procedure will generate RALCGM for VAX/VMS from the general distribution.

RALCGM can be used to view CGM files using an X windows display. Do you require
this option? (DECwindows MUST be installed on your VAX) 
$inquire/nopunct X_WIN "Enter Y/N [N] ->"
$if X_WIN
$then
$  write sys$output " "
$  write sys$output "RALCGM X windows viewer will be included. X will be the default output...."
$  copy/nolog inc_dir:mc_vms.h inc_dir:mach.h
$else
$  write sys$output " "
$  write sys$output "RALCGM X windows viewer will not be included."
$  define/nolog/user sys$output nl:
$  edit/output=inc_dir:mach.h inc_dir:mc_vms.h
s n |#define XW|/*#define XW */|
s n |#define DEFOUT  'X'|/*#define DEFOUT  'X' */|
exit
$endif
$!==================
$! check for TEK4200
$!==================
$copy/nolog sys$input: sys$output:

Do you require the Tektronix 4200 output option?
$inquire/nopunct TEK4200 "Enter Y/N [N] ->"
$if TEK4200
$then
$  write sys$output " "
$  write sys$output "Tektronix output will be included...."
$else
$write sys$output " "
$write sys$output "Tektronix  output will not be available..."
$  define/nolog/user sys$output nl:
$  edit/output=inc_dir:mach.h inc_dir:mach.h
s n |#define TEK4200|/*#define TEK4200 */|
exit
$endif
$!=====================
$! check for PostScript
$!=====================
$copy/nolog sys$input: sys$output:


Do you require the PostScript output option?
$inquire/nopunct PS "Enter Y/N [N] ->"
$if PS
$then
$  write sys$output " "
$  write sys$output "PostScript output will be included...."
$  copy/nolog sys$input: sys$output:

Do you want to use the 35 PostScript font set of the LaserWriter II?
(N gives the 13 PostScript font set of the LaserWriter)
$  inquire/nopunct PS_OPT "Enter Y/N [N] ->"
$  if PS_OPT
$  then
$    write sys$output " "
$    write sys$output "35 PostScript font set will be used...
$    copy/nolog  data_dir:m_ps35.fnt data_dir:m_ps.fnt
$  else
$    write sys$output " "
$    write sys$output "13 PostScript font set will be used...
$    copy/nolog  data_dir:m_ps13.fnt data_dir:m_ps.fnt
$  endif
$else
$  write sys$output " "
$  write sys$output "PostScript output will not be included..."
$  define/nolog/user sys$output nl:
$  edit/output=inc_dir:mach.h inc_dir:mach.h
s n |#define POSTSCRIPT|/*#define POSTSCRIPT */|
exit
$endif
$!==================
$! check for HPGL
$!==================
$copy/nolog sys$input: sys$output:

Do you require the HPGL output option?
$inquire/nopunct HPGL "Enter Y/N [N] ->"
$if HPGL
$then
$  write sys$output " "
$  write sys$output "HPGL output will be included...."
$else
$write sys$output " "
$write sys$output "HPGL output will not be available..."
$  define/nolog/user sys$output nl:
$  edit/output=inc_dir:mach.h inc_dir:mach.h
s n |#define HPGL|/*#define HPGL */|
exit
$endif
$!==================
$! check for HERSHEY
$!==================
$copy/nolog sys$input: sys$output:


Do you require the HERSHEY fonts (advisable if PostScript or X selected)
$inquire/nopunct HER "Enter Y/N [N] ->"
$if HER
$then
$write sys$output " "
$write sys$output "HERSHEY fonts will be available...."
$else
$write sys$output " "
$write sys$output "HERSHEY fonts will not be available..."
$  define/nolog/user sys$output nl:
$  edit/output=inc_dir:mach.h inc_dir:mach.h
s n |#define HERSHEY|/*#define HERSHEY */|
exit
$endif
$!==================
$! check for BEZIER
$!==================
$copy/nolog sys$input: sys$output:


Do you require the BEZIER fonts (advisable if PostScript or X selected)
$inquire/nopunct BEZ "Enter Y/N [N] ->"
$if BEZ
$then
$write sys$output " "
$write sys$output "BEZIER fonts will be available...."
$else
$write sys$output " "
$write sys$output "BEZIER fonts will not be available..."
$  define/nolog/user sys$output nl:
$  edit/output=inc_dir:mach.h inc_dir:mach.h
s n |#define BEZIER|/*#define BEZIER */|
exit
$endif
$!==================================================================
$! compile each of the source files for VMS and place in the library
$!==================================================================
$if X_WIN
$then
$  ignore_files="CGMOIGL,CGMSKEL,CGMTEST,CGMOTPZ,CGMLIB,CGMOVGA"
$else
$  ignore_files="CGMOIGL,CGMSKEL,CGMTEST,CGMOTPZ,CGMLIB,CGMGDP,CGMOXW,CGMFILL,CGMOVGA"
$endif
$length_ignore=f$length(ignore_files)
$write sys$output "Compiling RALCGM source files......"
$loop:
$file =f$search("source_dir:cgm*.c")
$if file.eqs."" then $ goto loop_end
$file_name=f$parse(file,,,"name")
$if f$locate(file_name,ignore_files).ne.length_ignore then $ goto loop
$write sys$output "Compiling file:''file_name'"
$cc/include=inc_dir 'file
$if $status
$then
$!write sys$output "Placing ''file_name' module in object library"
$  obj_file="''file_name'.obj;"
$  lib/nolog/replace 'lib_name 'obj_file
$  del/nolog 'obj_file
$endif
$goto loop
$loop_end:
$!=========================================================
$! Link the interpreter, decw$xlibshr required for X viewer
$!=========================================================
$link:
$if f$edit(p2,"UPCASE").eqs."X" then $X_WIN="Y"
$if p1.eqs."LD"
$then
$  write sys$output "Linking RALCGM system with DEBUG...."
$  if X_WIN
$  then
$link/debug/exe='exe_name 'lib_name/inc=cgmmain,'lib_name/lib,sys$input:/options
sys$library:decw$xlibshr/share
$  else
$link/debug/exe='exe_name 'lib_name/inc=cgmmain,'lib_name/lib,sys$input:/options
sys$library:vaxcrtl.exe/share
$  endif
$else
$  write sys$output "Linking RALCGM system...."
$  if X_WIN
$  then
$link/exe='exe_name 'lib_name/inc=cgmmain,'lib_name/lib,sys$input:/options
sys$library:decw$xlibshr/share
$  else
$link/exe='exe_name 'lib_name/inc=cgmmain,'lib_name/lib,sys$input:/options
sys$library:vaxcrtl.exe/share
$  endif
$endif
$!=======================================================
$! create the startup file to do the required assignments
$!=======================================================
$write sys$output "Creating RALCGM startup file...."
$open/write out ralcgm_startup.com
$write out "$ralcgm :==$''bin'ralcgm.exe"
$write out "$define/nolog cgmmenufont ""-Adobe-ITC Lubalin Graph-Book-R-Normal--12-120-75-75-P-70-ISO8859-1""
$write out "$define/nolog CGMDATA ''data'"
$close out
$!=====================
$! create the help file
$!=====================
$if p1.nes."" then $goto mess
$write sys$output "Creating the RALCGM help file...."
$runoff/output=docs_dir:ralcgm.hlp docs_dir:ralcgm.rnh
$write sys$output "Helpfile ''docs'RALCGM.HLP created"
$!==============================================================
$! define the logical CGMDATA before running the font generators
$!==============================================================
$gens:
$define/nolog CGMDATA 'data
$if (X_WIN.or.PS)
$then
$!========================
$!  build the bezier fonts
$!========================
$  if p1.nes."" then $goto mess
$  if BEZ
$  then
$    write sys$output "Generating BEZIER font files ...."
$    if f$search("bin_dir:genbez.exe").eqs.""
$    then
$      cc/include=inc_dir source_dir:genbez
$      link/exe=bin_dir:genbez.exe genbez,'lib_name/lib,sys$input:/options
sys$library:vaxcrtl.exe/share
$      delete/nolog genbez.obj;
$    endif
$    run bin_dir:genbez
$  endif
$!=========================
$!  build the hershey fonts
$!=========================
$  if p1.nes."" then $goto mess
$  if HER
$  then
$    write sys$output "Generating HERSHEY font files...."
$    if f$search("bin_dir:genher.exe").eqs.""
$    then
$      cc/include=inc_dir source_dir:genher
$      link/exe=bin_dir:genher genher,'lib_name/lib,sys$input:/options
sys$library:vaxcrtl/share
$      delete/nolog genher.obj;
$    endif
$    run bin_dir:genher
$  endif
$else
$  write sys$output " "
$  write sys$output "Font systems were not built as neither X or PostScript selected"
$  write sys$output " "
$endif
$!===============
$! output message
$!===============
$mess:
$copy/nolog sys$input: sys$output:


The RALCGM system has been created. You will need to invoke the procedure:
$write sys$output "''master'RALCGM_STARTUP.COM "
$copy/nolog sys$input: sys$output:
to define the relevent symbols and logical names required. 

The same distribution can be used for generating RALCGM for UNIX machines
using the makefiles provided, see the README file for more information!

$goto stop
$error:
$write sys$output " "
$write sys$output "ERROR generating RALCGM system "
$write sys$output " "
$stop:
$!==========================
$! restore the edit symbol
$!==========================
$if ed_type.nes.""
$then
$  edit :== 'ed_type
$else
$  delete/symbol/global/nolog edit
$endif
$set default 'save_dir
$exit
