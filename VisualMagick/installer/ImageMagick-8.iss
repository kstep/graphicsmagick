;
; Configuration file for Inno Setup Compiler 2.0.19 to construct GraphicsMagick
; installation package.  This file is originally authored by Bob Friesenhahn
; <bfriesen@simple.dallas.tx.us>.
;
; Inno Setup is freely available from "http://www.jrsoftware.org/isinfo.php".
;
; Copyright (C) 2002 GraphicsMagick Group, a non-profit organization dedicated
; to making software imaging solutions freely available.
;
; Permission is hereby granted, free of charge, to any person obtaining a
; copy of this software and associated documentation files ("GraphicsMagick"),
; to deal in GraphicsMagick without restriction, including without limitation
; the rights to use, copy, modify, merge, publish, distribute, sublicense,
; and/or sell copies of GraphicsMagick, and to permit persons to whom the
; GraphicsMagick is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of GraphicsMagick.
;
; The software is provided "as is", without warranty of any kind, express or
; implied, including but not limited to the warranties of merchantability,
; fitness for a particular purpose and noninfringement.  In no event shall
; GraphicsMagick Group be liable for any claim, damages or other liability,
; whether in an action of contract, tort or otherwise, arising from, out of
; or in connection with GraphicsMagick or the use or other dealings in
; GraphicsMagick.
;
; Except as contained in this notice, the name of the GraphicsMagick Group
; shall not be used in advertising or otherwise to promote the sale, use or
; other dealings in GraphicsMagick without prior written authorization from the
; GraphicsMagick Group.
;
;
; Notes to whoever builds an installer from this file.  Certain things
; may need to be adjusted before you run the compiler.  Please remember
; to update the following if needed:
;
;   o GraphicsMagick release version (needs to be globally substituted)
;   o Release date in AppVerName should reflect the release date.
;   o Ensure that the word "Beta" occurs in AppVerName before the date
;     if this is a beta release snapshot.  For example:
;     "GraphicsMagick 1.0.0 Q:8 Beta (August 26, 2002)"
;   o Ensure that the quantum depth indication ("Q:8", "Q:8", or "Q:32")
;     is correct.  The normal release build should be Q:8.
;
; Eventually I hope to create an m4 macro package that makes producing this
; file easier, and perhaps even running the compiler automatically as a
; final build step.

[Setup]
AppName=GraphicsMagick 1.0.0 Q:8
; When updating the version string, remember to also update the version in
; the [Registry] section
AppVerName=GraphicsMagick 1.0.0 Q:8 Beta (December 13, 2002)
AppVersion=1.0.0
AppCopyright=Copyright (C) 2002 GraphicsMagick Group
AppPublisher=GraphicsMagick Group
AppPublisherURL=http://www.GraphicsMagick.org/
AppSupportURL=http://www.GraphicsMagick.org/www/magick-list.html
AppUpdatesURL=http://www.GraphicsMagick.org/www/archives.html
; Default install is under "C:\Program Files\GraphicsMagick-1.0.0-Q8":
DefaultDirName={pf}\GraphicsMagick-1.0.0-Q8
DefaultGroupName=GraphicsMagick 1.0.0 Q8
Compression=zip/9
ChangesAssociations=1
AllowRootDirectory=no
InfoBeforeFile=before.txt
InfoAfterFile=after.txt
UninstallDisplayIcon={app}\ImageMagick.ico
OutputDir=..\bin
OutputBaseFilename=GraphicsMagick-1.0.0-Q8-windows
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

; Optional Tasks
[Tasks]
Name: desktop_icon; Description: "Create a &desktop icon"; MinVersion: 4,4
Name: update_path; Description: "Update executable search path"
Name: associate_extensions; Description: "Associate supported file extensions with GraphicsMagick"; Flags: unchecked
Name: install_PerlMagick; Description: "Install PerlMagick for ActiveState Perl v5.8.0 build 804"; Flags: unchecked
;Name: install_Magick_DLL; Description: "Install ImageMagickObject OLE Control for VBscript, Visual Basic, and WSH"; Flags: unchecked

; Windows registry settings
[Registry]
;
; Record installation paths
;
Root: HKLM; Subkey: "Software\GraphicsMagick"; ValueType: none; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\GraphicsMagick\1.0.0"; ValueType: none; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\GraphicsMagick\1.0.0\Q:8"; ValueType: none; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\GraphicsMagick\1.0.0\Q:8"; ValueType: string; ValueName: "BinPath"; ValueData: "{app}"
Root: HKLM; Subkey: "Software\GraphicsMagick\1.0.0\Q:8"; ValueType: string; ValueName: "ConfigurePath"; ValueData: "{app}\config"
Root: HKLM; Subkey: "Software\GraphicsMagick\1.0.0\Q:8"; ValueType: string; ValueName: "LibPath"; ValueData: "{app}"
Root: HKLM; Subkey: "Software\GraphicsMagick\1.0.0\Q:8"; ValueType: string; ValueName: "ModulesPath"; ValueData: "{app}\modules"

Root: HKLM; Subkey: "Software\GraphicsMagick\Current"; ValueType: none; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\GraphicsMagick\Current"; ValueType: string; ValueName: "Version"; ValueData: "1.0.0"
Root: HKLM; Subkey: "Software\GraphicsMagick\Current"; ValueType: dword; ValueName: "QuantumDepth"; ValueData: 8

Root: HKLM; Subkey: "Software\GraphicsMagick\Current"; ValueType: string; ValueName: "BinPath"; ValueData: "{app}"
Root: HKLM; Subkey: "Software\GraphicsMagick\Current"; ValueType: string; ValueName: "ConfigurePath"; ValueData: "{app}\config"
Root: HKLM; Subkey: "Software\GraphicsMagick\Current"; ValueType: string; ValueName: "LibPath"; ValueData: "{app}"
Root: HKLM; Subkey: "Software\GraphicsMagick\Current"; ValueType: string; ValueName: "ModulesPath"; ValueData: "{app}\modules"

;
; Tell windows to use our convert.exe
;
;Root: HKLM; Subkey: "Software\Classes\Applications"; ValueType: string; ValueName: "AutoRun"; ValueData: "{app}\convert.exe $*"; Flags: uninsdeletevalue

;
; Create file associations
;

; AVS (.AVS)

Root: HKCR; Subkey: ".avs"; ValueType: string; ValueName: ""; ValueData: "Magick.AVSFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".avs\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".avs\OpenWithProgids\Magick.AVSFile"; ValueType: none; Flags: uninsdeletekey ; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.AVSFile"; ValueType: string; ValueName: ""; ValueData: "AVS X Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.AVSFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.AVSFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.AVSFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.AVSFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.AVSFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; BMP (.bmp)
Root: HKCR; Subkey: ".bmp"; ValueType: string; ValueName: ""; ValueData: "Magick.BMPFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".bmp\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".bmp\OpenWithProgids\Magick.BMPFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.BMPFile"; ValueType: string; ValueName: ""; ValueData: "Windows Bitmap Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.BMPFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.BMPFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.BMPFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.BMPFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.BMPFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; CGM (.cgm)
Root: HKCR; Subkey: ".cgm"; ValueType: string; ValueName: ""; ValueData: "Magick.CGMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".cgm\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".cgm\OpenWithProgids\Magick.CGMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.CGMFile"; ValueType: string; ValueName: ""; ValueData: "Computer Graphics Metafile"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.CGMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.CGMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.CGMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; CUT (.cut)
Root: HKCR; Subkey: ".cut"; ValueType: string; ValueName: ""; ValueData: "Magick.CUTFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".cut\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".cut\OpenWithProgids\Magick.CUTFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.CUTFile"; ValueType: string; ValueName: ""; ValueData: "DR Halo Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.CUTFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.CUTFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.CUTFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; DCX (.DCX)
Root: HKCR; Subkey: ".dcx"; ValueType: string; ValueName: ""; ValueData: "Magick.DCXFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dcx\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dcx\OpenWithProgids\Magick.DCXFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DCXFile"; ValueType: string; ValueName: ""; ValueData: "ZSoft Paintbrush Multi-Page Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DCXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DCXFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DCXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DCXFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DCXFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; DIB (.dib)
Root: HKCR; Subkey: ".dib"; ValueType: string; ValueName: ""; ValueData: "Magick.DIBFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dib\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dib\OpenWithProgids\Magick.DIBFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DIBFile"; ValueType: string; ValueName: ""; ValueData: "Windows Device Independent Bitmap"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DIBFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DIBFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DIBFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DIBFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DIBFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; DICOM (.dcm, .dicom)
Root: HKCR; Subkey: ".dcm"; ValueType: string; ValueName: ""; ValueData: "Magick.DICOMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dcm\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dcm\OpenWithProgids\Magick.DICOMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".dicom"; ValueType: string; ValueName: ""; ValueData: "Magick.DICOMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dicom\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dicom\OpenWithProgids\Magick.DICOMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DICOMFile"; ValueType: string; ValueName: ""; ValueData: "DICOM Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DICOMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DICOMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DICOMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; DPX (.dpx)
Root: HKCR; Subkey: ".dpx"; ValueType: string; ValueName: ""; ValueData: "Magick.DPXFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dpx\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".dpx\OpenWithProgids\Magick.DPXFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DPXFile"; ValueType: string; ValueName: ""; ValueData: "Digital Moving Picture Exchange"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DPXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DPXFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DPXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DPXFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.DPXFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; EMF (.emf)
Root: HKCR; Subkey: ".emf"; ValueType: string; ValueName: ""; ValueData: "Magick.EMFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".emf\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".emf\OpenWithProgids\Magick.EMFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EMFFile"; ValueType: string; ValueName: ""; ValueData: "Windows Ehanced Meta File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EMFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EMFFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EMFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; EPS (.eps)
Root: HKCR; Subkey: ".eps"; ValueType: string; ValueName: ""; ValueData: "Magick.EPSFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".eps\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".eps\OpenWithProgids\Magick.EPSFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EPSFile"; ValueType: string; ValueName: ""; ValueData: "Encapsulated Postscript File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EPSFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EPSFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EPSFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EPSFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.EPSFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; FlashPIX (.fpx)
Root: HKCR; Subkey: ".fpx"; ValueType: string; ValueName: ""; ValueData: "Magick.FPXFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".fpx\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".fpx\OpenWithProgids\Magick.FPXFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FPXFile"; ValueType: string; ValueName: ""; ValueData: "FlashPIX Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FPXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FPXFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FPXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FPXFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FPXFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; GIF (.gif)
Root: HKCR; Subkey: ".gif"; ValueType: string; ValueName: ""; ValueData: "Magick.GIFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".gif\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".gif\OpenWithProgids\Magick.GIFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.GIFFile"; ValueType: string; ValueName: ""; ValueData: "Compuserv GIF Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.GIFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.GIFFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.GIFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.GIFFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.GIFFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; HPGL (.hp, .hpgl)
Root: HKCR; Subkey: ".hp"; ValueType: string; ValueName: ""; ValueData: "Magick.HPGLFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".hp\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".hp\OpenWithProgids\Magick.HPGLFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".hpgl"; ValueType: string; ValueName: ""; ValueData: "Magick.HPGLFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".hpgl\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".hpgl\OpenWithProgids\Magick.HPGLFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.HPGLFile"; ValueType: string; ValueName: ""; ValueData: "HP-GL Plotter File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.HPGLFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.HPGLFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.HPGLFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; JBIG (.bie, .jbg, .jbig)
Root: HKCR; Subkey: ".bie"; ValueType: string; ValueName: ""; ValueData: "Magick.JBIGFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".bie\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".bie\OpenWithProgids\Magick.JBIGFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".jbg"; ValueType: string; ValueName: ""; ValueData: "Magick.JBIGFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".jbg\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".jbg\OpenWithProgids\Magick.JBIGFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".jbig"; ValueType: string; ValueName: ""; ValueData: "Magick.JBIGFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".jbig\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".jbig\OpenWithProgids\Magick.JBIGFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JBIGFile"; ValueType: string; ValueName: ""; ValueData: "JBIG Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JBIGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JBIGFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JBIGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JBIGFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JBIGFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; JPEG (.jpg, .jpeg)
Root: HKCR; Subkey: ".jpg"; ValueType: string; ValueName: ""; ValueData: "Magick.JPEGFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".jpg\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".jpg\OpenWithProgids\Magick.JPEGFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".jpeg"; ValueType: string; ValueName: ""; ValueData: "Magick.JPEGFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".jpeg\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".jpeg\OpenWithProgids\Magick.JPEGFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JPEGFile"; ValueType: string; ValueName: ""; ValueData: "JPEG Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JPEGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JPEGFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JPEGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JPEGFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.JPEGFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; KHOROS (.viff, .xv)
Root: HKCR; Subkey: ".viff"; ValueType: string; ValueName: ""; ValueData: "Magick.KHOROSFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".viff\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".viff\OpenWithProgids\Magick.KHOROSFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".xv"; ValueType: string; ValueName: ""; ValueData: "Magick.KHOROSFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xv\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xv\OpenWithProgids\Magick.KHOROSFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.KHOROSFile"; ValueType: string; ValueName: ""; ValueData: "Khoros Visualization Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.KHOROSFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.KHOROSFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.KHOROSFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.KHOROSFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.KHOROSFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; MNG (.mng)
Root: HKCR; Subkey: ".mng"; ValueType: string; ValueName: ""; ValueData: "Magick.MNGFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".mng\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".mng\OpenWithProgids\Magick.MNGFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MNGFile"; ValueType: string; ValueName: ""; ValueData: "Multiple-image Network Graphics Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MNGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MNGFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MNGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MNGFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MNGFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; MPC (.mpc)
Root: HKCR; Subkey: ".mpc"; ValueType: string; ValueName: ""; ValueData: "Magick.MPCFile"
Root: HKCR; Subkey: ".mpc\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty
Root: HKCR; Subkey: ".mpc\OpenWithProgids\Magick.MPCFile"; ValueType: none; Flags: uninsdeletekey
Root: HKCR; Subkey: "Magick.MPCFile"; ValueType: string; ValueName: ""; ValueData: "Magick Persistent Cache Image"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Magick.MPCFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "Magick.MPCFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "Magick.MPCFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MIFF (.miff)
Root: HKCR; Subkey: ".miff"; ValueType: string; ValueName: ""; ValueData: "Magick.MIFFFile"
Root: HKCR; Subkey: ".miff\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty
Root: HKCR; Subkey: ".miff\OpenWithProgids\Magick.MIFFFile"; ValueType: none; Flags: uninsdeletekey
Root: HKCR; Subkey: "Magick.MIFFFile"; ValueType: string; ValueName: ""; ValueData: "Magick Image File Image"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Magick.MIFFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "Magick.MIFFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "Magick.MIFFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""
Root: HKCR; Subkey: "Magick.MIFFFile\shell\edit"; ValueType: none
Root: HKCR; Subkey: "Magick.MIFFFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MSL (.msl)
Root: HKCR; Subkey: ".msl"; ValueType: string; ValueName: ""; ValueData: "Magick.MSLFile"
Root: HKCR; Subkey: ".msl\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty
Root: HKCR; Subkey: ".msl\OpenWithProgids\Magick.MSLFile"; ValueType: none; Flags: uninsdeletekey
Root: HKCR; Subkey: "Magick.MSLFile"; ValueType: string; ValueName: ""; ValueData: "Magick Scripting Language Script"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Magick.MSLFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "Magick.MSLFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "Magick.MSLFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MVG (.mvg)
Root: HKCR; Subkey: ".mvg"; ValueType: string; ValueName: ""; ValueData: "Magick.MVGFile"
Root: HKCR; Subkey: ".mvg\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty
Root: HKCR; Subkey: ".mvg\OpenWithProgids\Magick.MVGFile"; ValueType: none; Flags: uninsdeletekey
Root: HKCR; Subkey: "Magick.MVGFile"; ValueType: string; ValueName: ""; ValueData: "Magick Vector Graphics File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Magick.MVGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "Magick.MVGFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "Magick.MVGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PBM (.pbm)
Root: HKCR; Subkey: ".pbm"; ValueType: string; ValueName: ""; ValueData: "Magick.PBMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pbm\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pbm\OpenWithProgids\Magick.PBMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PBMFile"; ValueType: string; ValueName: ""; ValueData: "Portable Bitmap File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PBMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PBMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PBMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PBMFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PBMFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PGM (.pgm)
Root: HKCR; Subkey: ".pgm"; ValueType: string; ValueName: ""; ValueData: "Magick.PGMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pgm\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pgm\OpenWithProgids\Magick.PGMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PGMFile"; ValueType: string; ValueName: ""; ValueData: "Portable Graymap File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PGMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PGMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PGMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PGMFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PGMFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PPM (.ppm)
Root: HKCR; Subkey: ".ppm"; ValueType: string; ValueName: ""; ValueData: "Magick.PPMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ppm\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ppm\OpenWithProgids\Magick.PPMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PPMFile"; ValueType: string; ValueName: ""; ValueData: "Portable Pixmap File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PPMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PPMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PPMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PPMFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PPMFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PNG (.png)
Root: HKCR; Subkey: ".png"; ValueType: string; ValueName: ""; ValueData: "Magick.PNGFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".png\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".png\OpenWithProgids\Magick.PNGFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PNGFile"; ValueType: string; ValueName: ""; ValueData: "Portable Network Graphics Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PNGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PNGFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PNGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PNGFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PNGFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PTIF (.ptif)
Root: HKCR; Subkey: ".ptif"; ValueType: string; ValueName: ""; ValueData: "Magick.PTIFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ptif\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ptif\OpenWithProgids\Magick.PTIFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PTIFFile"; ValueType: string; ValueName: ""; ValueData: "Pyramid encoded TIFF Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PTIFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PTIFFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PTIFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PTIFFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PTIFFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; TIFF (.fax, .g3, .tif, .tiff)
Root: HKCR; Subkey: ".fax"; ValueType: string; ValueName: ""; ValueData: "Magick.TIFFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".fax\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".fax\OpenWithProgids\Magick.TIFFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".g3"; ValueType: string; ValueName: ""; ValueData: "Magick.TIFFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".g3\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".g3\OpenWithProgids\Magick.TIFFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".tif"; ValueType: string; ValueName: ""; ValueData: "Magick.TIFFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".tif\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".tif\OpenWithProgids\Magick.TIFFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".tiff"; ValueType: string; ValueName: ""; ValueData: "Magick.TIFFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".tiff\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".tiff\OpenWithProgids\Magick.TIFFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIFFFile"; ValueType: string; ValueName: ""; ValueData: "Tagged Image File Format"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIFFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIFFFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIFFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIFFFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIFFFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PCX (.pcx)
Root: HKCR; Subkey: ".pcx"; ValueType: string; ValueName: ""; ValueData: "Magick.PCXFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pcx\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pcx\OpenWithProgids\Magick.PCXFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCXFile"; ValueType: string; ValueName: ""; ValueData: "ZSoft Paintbrush Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCXFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCXFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCXFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; FITS (.fits)
Root: HKCR; Subkey: ".fits"; ValueType: string; ValueName: ""; ValueData: "Magick.FITSFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".fits\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".fits\OpenWithProgids\Magick.FITSFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FITSFile"; ValueType: string; ValueName: ""; ValueData: "Flexible Image Transport System Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FITSFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FITSFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FITSFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FITSFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.FITSFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; TARGA (.icb, .tga, .vda, .vst)
Root: HKCR; Subkey: ".icb"; ValueType: string; ValueName: ""; ValueData: "Magick.TARGAFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".icb\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".icb\OpenWithProgids\Magick.TARGAFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".tga"; ValueType: string; ValueName: ""; ValueData: "Magick.TARGAFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".tga\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".tga\OpenWithProgids\Magick.TARGAFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".vda"; ValueType: string; ValueName: ""; ValueData: "Magick.TARGAFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".vda\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".vda\OpenWithProgids\Magick.TARGAFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".vst"; ValueType: string; ValueName: ""; ValueData: "Magick.TARGAFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".vst\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".vst\OpenWithProgids\Magick.TARGAFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TARGAFile"; ValueType: string; ValueName: ""; ValueData: "Truevision Targa Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TARGAFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TARGAFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TARGAFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TARGAFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TARGAFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; ICON (.ico, .icon)
Root: HKCR; Subkey: ".ico"; ValueType: string; ValueName: ""; ValueData: "Magick.ICONFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ico\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ico\OpenWithProgids\Magick.ICONFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".icon"; ValueType: string; ValueName: ""; ValueData: "Magick.ICONFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".icon\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".icon\OpenWithProgids\Magick.ICONFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.ICONFile"; ValueType: string; ValueName: ""; ValueData: "Microsoft Windows Icon"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.ICONFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.ICONFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.ICONFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; MATLAB (.mat)
Root: HKCR; Subkey: ".mat"; ValueType: string; ValueName: ""; ValueData: "Magick.MATLABFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".mat\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".mat\OpenWithProgids\Magick.MATLABFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MATLABFile"; ValueType: string; ValueName: ""; ValueData: "MATLAB Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MATLABFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MATLABFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MATLABFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; MTV (.mtv)
Root: HKCR; Subkey: ".mtv"; ValueType: string; ValueName: ""; ValueData: "Magick.MTVFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".mtv\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".mtv\OpenWithProgids\Magick.MTVFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MTVFile"; ValueType: string; ValueName: ""; ValueData: "MTV Raytrace Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MTVFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MTVFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MTVFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MTVFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.MTVFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; OTB (.otb)
Root: HKCR; Subkey: ".otb"; ValueType: string; ValueName: ""; ValueData: "Magick.OTBFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".otb\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".otb\OpenWithProgids\Magick.OTBFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.OTBFile"; ValueType: string; ValueName: ""; ValueData: "On The Air Bitmap Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.OTBFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.OTBFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.OTBFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.OTBFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.OTBFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PALM (.palm)
Root: HKCR; Subkey: ".palm"; ValueType: string; ValueName: ""; ValueData: "Magick.PALMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".palm\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".palm\OpenWithProgids\Magick.PALMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PALMFile"; ValueType: string; ValueName: ""; ValueData: "Palm Pixmap Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PALMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PALMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PALMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PALMFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PALMFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PCD (.pcd)
Root: HKCR; Subkey: ".pcd"; ValueType: string; ValueName: ""; ValueData: "Magick.PCDFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pcd\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pcd\OpenWithProgids\Magick.PCDFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCDFile"; ValueType: string; ValueName: ""; ValueData: "Photo CD Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCDFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCDFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCDFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCDFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PCDFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PDF (.pdf)
Root: HKCR; Subkey: ".pdf"; ValueType: string; ValueName: ""; ValueData: "Magick.PDFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pdf\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pdf\OpenWithProgids\Magick.PDFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PDFFile"; ValueType: string; ValueName: ""; ValueData: "Portable Document Format File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PDFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PDFFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PDFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PDFFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PDFFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PICT (.pct, .pict )
Root: HKCR; Subkey: ".pct"; ValueType: string; ValueName: ""; ValueData: "Magick.PICTFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pct\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pct\OpenWithProgids\Magick.PICTFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".pict"; ValueType: string; ValueName: ""; ValueData: "Magick.PICTFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pict\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pict\OpenWithProgids\Magick.PICTFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICTFile"; ValueType: string; ValueName: ""; ValueData: "Apple Mac QuickDraw/PICT Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICTFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICTFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICTFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICTFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICTFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PICON (.picon)
Root: HKCR; Subkey: ".picon"; ValueType: string; ValueName: ""; ValueData: "Magick.PICONFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".picon\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".picon\OpenWithProgids\Magick.PICONFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICONFile"; ValueType: string; ValueName: ""; ValueData: "Personal Icon"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICONFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICONFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICONFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICONFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PICONFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PIX (.pix, .rla)
Root: HKCR; Subkey: ".pix"; ValueType: string; ValueName: ""; ValueData: "Magick.PIXFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pix\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pix\OpenWithProgids\Magick.PIXFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".rla"; ValueType: string; ValueName: ""; ValueData: "Magick.PIXFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".rla\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".rla\OpenWithProgids\Magick.PIXFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PIXFile"; ValueType: string; ValueName: ""; ValueData: "Alias/Wavefront RLE Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PIXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PIXFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PIXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PSD (.psd)
Root: HKCR; Subkey: ".psd"; ValueType: string; ValueName: ""; ValueData: "Magick.PSDFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".psd\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".psd\OpenWithProgids\Magick.PSDFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSDFile"; ValueType: string; ValueName: ""; ValueData: "Adobe Photoshop Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSDFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSDFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSDFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSDFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSDFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; Postscript (.ps)
Root: HKCR; Subkey: ".ps"; ValueType: string; ValueName: ""; ValueData: "Magick.PostscriptFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ps\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ps\OpenWithProgids\Magick.PostscriptFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PostscriptFile"; ValueType: string; ValueName: ""; ValueData: "Postscript File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PostscriptFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PostscriptFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PostscriptFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PostscriptFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PostscriptFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; PSFONT (.pfa, .pfb)
Root: HKCR; Subkey: ".pfa"; ValueType: string; ValueName: ""; ValueData: "Magick.PSFONTFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pfa\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pfa\OpenWithProgids\Magick.PSFONTFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".pfb"; ValueType: string; ValueName: ""; ValueData: "Magick.PSFONTFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pfb\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pfb\OpenWithProgids\Magick.PSFONTFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSFONTFile"; ValueType: string; ValueName: ""; ValueData: "Postscript Font"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSFONTFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSFONTFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PSFONTFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; SFW (.pwp, .sfw)
Root: HKCR; Subkey: ".pwp"; ValueType: string; ValueName: ""; ValueData: "Magick.PWPFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pwp\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".pwp\OpenWithProgids\Magick.PWPFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: ".sfw"; ValueType: string; ValueName: ""; ValueData: "Magick.PWPFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".sfw\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".sfw\OpenWithProgids\Magick.PWPFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PWPFile"; ValueType: string; ValueName: ""; ValueData: "Seattle File Works Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PWPFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PWPFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.PWPFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; TIM (.tim)
Root: HKCR; Subkey: ".tim"; ValueType: string; ValueName: ""; ValueData: "Magick.TIMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".tim\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".tim\OpenWithProgids\Magick.TIMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIMFile"; ValueType: string; ValueName: ""; ValueData: "PSX TIM Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TIMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; TTF (.ttf)
Root: HKCR; Subkey: ".ttf"; ValueType: string; ValueName: ""; ValueData: "Magick.TTFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ttf\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".ttf\OpenWithProgids\Magick.TTFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TTFFile"; ValueType: string; ValueName: ""; ValueData: "TrueType Font"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TTFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TTFFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.TTFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; VICAR (.vicar)
Root: HKCR; Subkey: ".vicar"; ValueType: string; ValueName: ""; ValueData: "Magick.VICARFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".vicar\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".vicar\OpenWithProgids\Magick.VICARFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.VICARFile"; ValueType: string; ValueName: ""; ValueData: "VICAR Rasterfile Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.VICARFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.VICARFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.VICARFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.VICARFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.VICARFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; WBMP (.wbmp)
Root: HKCR; Subkey: ".wbmp"; ValueType: string; ValueName: ""; ValueData: "Magick.WBMPFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".wbmp\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".wbmp\OpenWithProgids\Magick.WBMPFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WBMPFile"; ValueType: string; ValueName: ""; ValueData: "Wireless Bitmap (Level 0) Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WBMPFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WBMPFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WBMPFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WBMPFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WBMPFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; WMF (.wmf)
Root: HKCR; Subkey: ".wmf"; ValueType: string; ValueName: ""; ValueData: "Magick.WMFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".wmf\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".wmf\OpenWithProgids\Magick.WMFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WMFFile"; ValueType: string; ValueName: ""; ValueData: "Windows Meta File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WMFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WMFFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.WMFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; XBM (.xbm)
Root: HKCR; Subkey: ".xbm"; ValueType: string; ValueName: ""; ValueData: "Magick.XBMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xbm\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xbm\OpenWithProgids\Magick.XBMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XBMFile"; ValueType: string; ValueName: ""; ValueData: "X Windows Bitmap Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XBMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XBMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XBMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XBMFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XBMFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; XCF (.xcf)
Root: HKCR; Subkey: ".xcf"; ValueType: string; ValueName: ""; ValueData: "Magick.XCFFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xcf\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xcf\OpenWithProgids\Magick.XCFFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XCFFile"; ValueType: string; ValueName: ""; ValueData: "GIMP Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XCFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XCFFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XCFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; XFIG (.fig)
Root: HKCR; Subkey: ".fig"; ValueType: string; ValueName: ""; ValueData: "Magick.XFIGFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".fig\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".fig\OpenWithProgids\Magick.XFIGFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XFIGFile"; ValueType: string; ValueName: ""; ValueData: "Xfig File"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XFIGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XFIGFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XFIGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; XPM (.xpm)
Root: HKCR; Subkey: ".xpm"; ValueType: string; ValueName: ""; ValueData: "Magick.XPMFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xpm\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xmp\OpenWithProgids\Magick.XPMFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XPMFile"; ValueType: string; ValueName: ""; ValueData: "X Windows Pixmap Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XPMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XPMFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XPMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XPMFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XPMFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

; XWD (.xwd)
Root: HKCR; Subkey: ".xwd"; ValueType: string; ValueName: ""; ValueData: "Magick.XWDFile"; Flags: uninsdeletevalue uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xwd\OpenWithProgids"; ValueType: none; Flags: uninsdeletekeyifempty; Tasks: associate_extensions
Root: HKCR; Subkey: ".xwd\OpenWithProgids\Magick.XWDFile"; ValueType: none; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XWDFile"; ValueType: string; ValueName: ""; ValueData: "X Window Dump Image"; Flags: uninsdeletekey; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XWDFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XWDFile\shell\open"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XWDFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XWDFile\shell\edit"; ValueType: none; Tasks: associate_extensions
Root: HKCR; Subkey: "Magick.XWDFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""; Tasks: associate_extensions

[Files]
; Compiler-dependent DLLs
Source: "..\bin\msvcr70.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\msvcp70.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\mfc70.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; Executables
Source: "..\bin\IMDisplay.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\animate.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\composite.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\conjure.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\convert.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\display.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\gm.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\hp2xx.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\identify.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\import.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\mogrify.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\montage.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\mpeg2dec.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\mpeg2enc.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; DLLs
Source: "..\bin\CORE_RL*.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\IM_MOD_RL*.dll"; DestDir: "{app}\modules"; CopyMode: alwaysoverwrite
Source: "..\bin\X11.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\bin\Xext.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; Configuration files
Source: "..\..\magick\colors.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "..\..\magick\log.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "..\bin\delegates.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "..\..\magick\magic.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "..\..\coders\modules.mgk"; DestDir: "{app}\modules"; CopyMode: alwaysoverwrite
Source: "..\bin\type-ghostscript.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "..\bin\type.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
; Release documentation files
Source: "..\..\ChangeLog"; DestDir: "{app}"; CopyMode: alwaysoverwrite; DestName: "ChangeLog.txt"
Source: "..\..\Copyright.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\..\QuickStart.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\..\README.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; Web pages
Source: "..\..\index.html"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: isreadme
Source: "..\..\images\*.jpg"; DestDir: "{app}\images"; CopyMode: alwaysoverwrite
Source: "..\..\images\*.png"; DestDir: "{app}\images"; CopyMode: alwaysoverwrite
Source: "..\..\www\*.c"; DestDir: "{app}\www"; CopyMode: alwaysoverwrite
Source: "..\..\www\*.css"; DestDir: "{app}\www"; CopyMode: alwaysoverwrite
Source: "..\..\www\*.html"; DestDir: "{app}\www"; CopyMode: alwaysoverwrite
Source: "..\..\www\Magick++\*.html"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\www\Magick++\*.jpg"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\www\Magick++\*.png"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\www\Magick++\COPYING"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite; DestName: "COPYING.txt"
Source: "..\..\www\Magick++\README.txt"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\www\api\*.html"; DestDir: "{app}\www\api"; CopyMode: alwaysoverwrite
Source: "..\..\www\api\*.html"; DestDir: "{app}\www\api"; CopyMode: alwaysoverwrite
Source: "..\..\www\api\types\*.html"; DestDir: "{app}\www\api\types"; CopyMode: alwaysoverwrite
; Delegate library licenses
Source: "..\..\licenses\*.txt"; DestDir: "{app}\licenses"; CopyMode: alwaysoverwrite
; Icon file
Source: "..\..\magick\ImageMagick.ico"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; Path Tool
Source: "..\..\contrib\win32\PathTool\PathTool.exe"; DestDir: "{app}\uninstall"; CopyMode: alwaysoverwrite
Source: "..\..\contrib\win32\PathTool\PathTool.pdf"; DestDir: "{app}\uninstall"; CopyMode: alwaysoverwrite
; PerlMagick Perl module
Source: "..\..\PerlMagick\Image-Magick.ppd"; DestDir: "{app}\PerlMagick"; CopyMode: alwaysoverwrite
Source: "..\..\PerlMagick\Image-Magick.tar.gz"; DestDir: "{app}\PerlMagick\x86"; CopyMode: alwaysoverwrite
Source: "..\..\PerlMagick\demo\*"; DestDir: "{app}\PerlMagick\demo"; CopyMode: alwaysoverwrite
Source: "PerlMagick-PPM.txt"; DestDir: "{app}\PerlMagick"; CopyMode: alwaysoverwrite; DestName: "README.txt"
; ImageMagickObject DLL (usable from Visual Basic and WSH)
;Source: "..\bin\ImageMagickObject.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
;Source: "..\..\contrib\win32\ATL\ImageMagickObject\README.txt"; DestDir: "{app}\ImageMagickObject"; CopyMode: alwaysoverwrite
;Source: "..\..\contrib\win32\ATL\ImageMagickObject\Tests\VBTest\*"; DestDir: "{app}\ImageMagickObject\VBExample"; CopyMode: alwaysoverwrite
;Source: "..\..\contrib\win32\ATL\ImageMagickObject\Tests\WSHTest\*"; DestDir: "{app}\ImageMagickObject\WSHExample"; CopyMode: alwaysoverwrite
;  Development libraries
Source: "..\lib\CORE_RL_Magick++_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_bzlib_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_fpx_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_jbig_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_jp2_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_jpeg_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_lcms_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_libxml_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
Source: "..\lib\CORE_RL_magick_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_png_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_tiff_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_ttf_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_wmf_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "..\lib\CORE_RL_zlib_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
Source: "..\lib\X11.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;  Development headers
Source: "..\..\magick\api.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\attribute.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\blob.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\cache.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\cache_view.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\command.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\compress.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\delegate.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\deprecate.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\draw.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\error.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\gem.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\integral_types.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\image.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\list.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\log.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\magic.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\magick_config.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\magick.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\module.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\monitor.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\PreRvIcccm.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\paint.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\quantize.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\registry.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\render.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\resource.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\random.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\semaphore.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\signature.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\timer.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\utility.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\version.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\widget.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\..\magick\xwindow.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
; Magick++ headers
Source: "..\..\Magick++\lib\Magick++.h"; DestDir: "{app}\include"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Blob.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\CoderInfo.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Color.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Drawable.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Exception.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Geometry.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Image.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Include.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Montage.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\Pixels.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\STL.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\lib\Magick++\TypeMetric.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite
; Magick++ demos
Source: "Magick++_Demo\*.dsw"; DestDir: "{app}\Magick++_Demo"; CopyMode: alwaysoverwrite
Source: "Magick++_Demo\*.dsp"; DestDir: "{app}\Magick++_Demo"; CopyMode: alwaysoverwrite
Source: "Magick++_Demo\*.bat"; DestDir: "{app}\Magick++_Demo"; CopyMode: alwaysoverwrite
Source: "Magick++_Demo\README.txt"; DestDir: "{app}\Magick++_Demo"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\demo\*.cpp"; DestDir: "{app}\Magick++_Demo"; CopyMode: alwaysoverwrite
Source: "..\..\Magick++\demo\*.miff"; DestDir: "{app}\Magick++_Demo"; CopyMode: alwaysoverwrite

[Icons]
Name: "{group}\GraphicsMagick Web Pages"; Filename: "{app}\index.html"
Name: "{group}\GraphicsMagick Windows Setup"; Filename: "{app}\www\windows.html"
; Name: "{group}\GraphicsMagick Readme"; Filename: "{app}\README.txt"
; Name: "{group}\GraphicsMagick Quick Start"; Filename: "{app}\QuickStart.txt"
; Name: "{group}\GraphicsMagick License"; Filename: "{app}\Copyright.txt"
Name: "{group}\GraphicsMagick Display"; Filename: "{app}\IMDisplay.exe"; IconFilename: "{app}\ImageMagick.ico"
Name: "{userdesktop}\GraphicsMagick Display"; Filename: "{app}\IMDisplay.exe"; IconFilename: "{app}\ImageMagick.ico"; Tasks: desktop_icon; MinVersion: 4,4

[Run]
; Add -debug to parameters to enable debugging
Filename: "{app}\uninstall\PathTool.exe"; Parameters: "-silent -a:""{app}"""; StatusMsg: "Updating environment variables..."; Tasks: update_path
Filename: "ppm"; Parameters: "install Image-Magick.ppd"; WorkingDir: "{app}\PerlMagick"; StatusMsg: "Installing PerlMagick..."; Tasks: install_PerlMagick; Flags: shellexec
; Add /s to regsvr32 for silent operation
;Filename: "regsvr32"; Parameters: "/c /s""{app}\ImageMagickObject.dll"""; StatusMsg: "Registering ImageMagickObject DLL..."; Tasks: install_Magick_DLL

[UninstallRun]
; Add -debug to parameters to enable debugging
Filename: "{app}\uninstall\PathTool.exe"; Parameters: "-silent -r:""{app}"""; StatusMsg: "Restoring environment variables..."; Tasks: update_path
Filename: "ppm"; Parameters: "remove Image-Magick"; StatusMsg: "Uninstalling PerlMagick..."; Tasks: install_PerlMagick; Flags: shellexec
; Add /s to regsvr32 for silent operation
;Filename: "regsvr32"; Parameters: "/u /s""{app}\ImageMagickObject.dll"""; StatusMsg: "Unregistering ImageMagickObject DLL..."; Tasks: install_Magick_DLL

