;
; Configuration file for Inno Setup Compiler 2.0.19 to construct ImageMagick
; installation package.  This file is originally authored by Bob Friesenhahn
; <bfriesen@simple.dallas.tx.us>.
;
; Inno Setup is freely available from "http://www.jrsoftware.org/isinfo.php".
;
; Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated
; to making software imaging solutions freely available.
;
; Permission is hereby granted, free of charge, to any person obtaining a
; copy of this software and associated documentation files ("ImageMagick"),
; to deal in ImageMagick without restriction, including without limitation
; the rights to use, copy, modify, merge, publish, distribute, sublicense,
; and/or sell copies of ImageMagick, and to permit persons to whom the
; ImageMagick is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of ImageMagick.
;
; The software is provided "as is", without warranty of any kind, express or
; implied, including but not limited to the warranties of merchantability,
; fitness for a particular purpose and noninfringement.  In no event shall
; ImageMagick Studio be liable for any claim, damages or other liability,
; whether in an action of contract, tort or otherwise, arising from, out of
; or in connection with ImageMagick or the use or other dealings in
; ImageMagick.
;
; Except as contained in this notice, the name of the ImageMagick Studio
; shall not be used in advertising or otherwise to promote the sale, use or
; other dealings in ImageMagick without prior written authorization from the
; ImageMagick Studio.
;

[Setup]
AppName=ImageMagick
; When updating the version string, remember to also update the version in
; the [Registry] section
AppVerName=ImageMagick version 5.4.8 Q:16 (Beta 7/26/02)
AppVersion=5.4.8
AppCopyright=Copyright (C) 2002 ImageMagick Studio LLC
AppPublisher=ImageMagick Studio
AppPublisherURL=http://www.imagemagick.org/
AppSupportURL=http://www.imagemagick.org/www/magick-list.html
AppUpdatesURL=http://www.imagemagick.org/www/archives.html
; Default install is under "C:\Program Files\ImageMagick-5.4.8-Q16":
DefaultDirName={pf}\ImageMagick-5.4.8-Q16
DefaultGroupName=ImageMagick 5.4.8
Compression=zip/9
ChangesAssociations=1
AllowRootDirectory=yes
UninstallDisplayIcon={app}\ImageMagick.ico
OutputDir=bin
OutputBaseFilename=ImageMagick-5.4.8-win2k
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

; Optional Tasks
[Tasks]
Name: desktop_icon; Description: "Create a &desktop icon"; MinVersion: 4,4
Name: update_path; Description: "Update executable search path"
Name: install_PerlMagick; Description: "Install PerlMagick for ActiveState Perl v5.6.1 build 633"; Flags: unchecked
Name: install_Magick_DLL; Description: "Install ImageMagickObject OLE Control for Visual Basic and WSH"; Flags: unchecked

; Windows registry settings
[Registry]
;
; Record installation paths
;
Root: HKLM; Subkey: "Software\ImageMagick\5.4.8\Q:16"; ValueType: string; ValueName: "ApplicationDefaultsPath"; ValueData: "{app}\config"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\5.4.8\Q:16"; ValueType: string; ValueName: "BinPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\5.4.8\Q:16"; ValueType: string; ValueName: "LibPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\5.4.8\Q:16"; ValueType: string; ValueName: "ModulesPath"; ValueData: "{app}\modules"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\5.4.8\Q:16"; ValueType: string; ValueName: "SharePath"; ValueData: "{app}\config"; Flags: uninsdeletekey

Root: HKLM; Subkey: "Software\ImageMagick\Current"; ValueType: string; ValueName: "Version"; ValueData: "5.4.8"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\Current"; ValueType: dword; ValueName: "QuantumDepth"; ValueData: 16; Flags: uninsdeletekey

Root: HKLM; Subkey: "Software\ImageMagick\Current"; ValueType: string; ValueName: "ApplicationDefaultsPath"; ValueData: "{app}\config"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\Current"; ValueType: string; ValueName: "BinPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\Current"; ValueType: string; ValueName: "LibPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\Current"; ValueType: string; ValueName: "ModulesPath"; ValueData: "{app}\modules"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ImageMagick\Current"; ValueType: string; ValueName: "SharePath"; ValueData: "{app}\config"; Flags: uninsdeletekey

;
; Create file associations
;

; AVS (.AVS)
Root: HKCR; Subkey: ".avs"; ValueType: string; ValueName: ""; ValueData: "ImageMagickAVSFile"
Root: HKCR; Subkey: "ImageMagickAVSFile"; ValueType: string; ValueName: ""; ValueData: "AVS X Image"
Root: HKCR; Subkey: "ImageMagickAVSFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickAVSFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickAVSFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; BMP (.bmp)
Root: HKCR; Subkey: ".bmp"; ValueType: string; ValueName: ""; ValueData: "ImageMagickBMPFile"
Root: HKCR; Subkey: "ImageMagickBMPFile"; ValueType: string; ValueName: ""; ValueData: "Windows Bitmap"
Root: HKCR; Subkey: "ImageMagickBMPFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickBMPFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickBMPFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; CGM (.cgm)
Root: HKCR; Subkey: ".CGM"; ValueType: string; ValueName: ""; ValueData: "ImageMagickCGMFile"
Root: HKCR; Subkey: "ImageMagickCGMFile"; ValueType: string; ValueName: ""; ValueData: "Computer Graphics Metafile"
Root: HKCR; Subkey: "ImageMagickCGMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickCGMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickCGMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; CUT (.cut)
Root: HKCR; Subkey: ".cut"; ValueType: string; ValueName: ""; ValueData: "ImageMagickCUTFile"
Root: HKCR; Subkey: "ImageMagickCUTFile"; ValueType: string; ValueName: ""; ValueData: "DR Halo File"
Root: HKCR; Subkey: "ImageMagickCUTFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickCUTFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickCUTFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; DIB (.dib)
Root: HKCR; Subkey: ".dib"; ValueType: string; ValueName: ""; ValueData: "ImageMagickDIBFile"
Root: HKCR; Subkey: "ImageMagickDIBFile"; ValueType: string; ValueName: ""; ValueData: "Windows Device Independent Bitmap"
Root: HKCR; Subkey: "ImageMagickDIBFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickDIBFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickDIBFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; DICOM (.dcm, .dicom)
Root: HKCR; Subkey: ".dcm"; ValueType: string; ValueName: ""; ValueData: "ImageMagickDICOMFile"
Root: HKCR; Subkey: ".dicom"; ValueType: string; ValueName: ""; ValueData: "ImageMagickDICOMFile"
Root: HKCR; Subkey: "ImageMagickDICOMFile"; ValueType: string; ValueName: ""; ValueData: "DICOM Image"
Root: HKCR; Subkey: "ImageMagickDICOMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickDICOMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickDICOMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; DPX (.dpx)
Root: HKCR; Subkey: ".DPX"; ValueType: string; ValueName: ""; ValueData: "ImageMagickDPXFile"
Root: HKCR; Subkey: "ImageMagickDPXFile"; ValueType: string; ValueName: ""; ValueData: "Digital Moving Picture Exchange"
Root: HKCR; Subkey: "ImageMagickDPXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickDPXFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickDPXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; EMF (.emf)
Root: HKCR; Subkey: ".emf"; ValueType: string; ValueName: ""; ValueData: "ImageMagickEMFFile"
Root: HKCR; Subkey: "ImageMagickEMFFile"; ValueType: string; ValueName: ""; ValueData: "Windows Ehanced Meta File"
Root: HKCR; Subkey: "ImageMagickEMFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickEMFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickEMFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; EPS (.eps)
Root: HKCR; Subkey: ".eps"; ValueType: string; ValueName: ""; ValueData: "ImageMagickEPSFile"
Root: HKCR; Subkey: "ImageMagickEPSFile"; ValueType: string; ValueName: ""; ValueData: "Encapsulated Postscript File"
Root: HKCR; Subkey: "ImageMagickEPSFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickEPSFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickEPSFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; FlashPIX (.fpx)
Root: HKCR; Subkey: ".fpx"; ValueType: string; ValueName: ""; ValueData: "ImageMagickFPXFile"
Root: HKCR; Subkey: "ImageMagickFPXFile"; ValueType: string; ValueName: ""; ValueData: "FlashPIX File"
Root: HKCR; Subkey: "ImageMagickFPXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickFPXFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickFPXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; GIF (.gif)
Root: HKCR; Subkey: ".gif"; ValueType: string; ValueName: ""; ValueData: "ImageMagickGIFFile"
Root: HKCR; Subkey: "ImageMagickGIFFile"; ValueType: string; ValueName: ""; ValueData: "Compuserv GIF File"
Root: HKCR; Subkey: "ImageMagickGIFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickGIFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickGIFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; HPGL (.hp, .hpgl)
Root: HKCR; Subkey: ".hp"; ValueType: string; ValueName: ""; ValueData: "ImageMagickHPGLFile"
Root: HKCR; Subkey: ".hpgl"; ValueType: string; ValueName: ""; ValueData: "ImageMagickHPGLFile"
Root: HKCR; Subkey: "ImageMagickHPGLFile"; ValueType: string; ValueName: ""; ValueData: "HP-GL Plotter File"
Root: HKCR; Subkey: "ImageMagickHPGLFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickHPGLFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickHPGLFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; JBIG (.bie, .jbg, .jbig)
Root: HKCR; Subkey: ".bie"; ValueType: string; ValueName: ""; ValueData: "ImageMagickJBIGFile"
Root: HKCR; Subkey: ".jbg"; ValueType: string; ValueName: ""; ValueData: "ImageMagickJBIGFile"
Root: HKCR; Subkey: ".jbig"; ValueType: string; ValueName: ""; ValueData: "ImageMagickJBIGFile"
Root: HKCR; Subkey: "ImageMagickJBIGFile"; ValueType: string; ValueName: ""; ValueData: "JBIG File"
Root: HKCR; Subkey: "ImageMagickJBIGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickJBIGFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickJBIGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; JPEG (.jpg, .jpeg)
Root: HKCR; Subkey: ".jpg"; ValueType: string; ValueName: ""; ValueData: "ImageMagickJPEGFile"
Root: HKCR; Subkey: ".jpeg"; ValueType: string; ValueName: ""; ValueData: "ImageMagickJPEGFile"
Root: HKCR; Subkey: "ImageMagickJPEGFile"; ValueType: string; ValueName: ""; ValueData: "JPEG File"
Root: HKCR; Subkey: "ImageMagickJPEGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickJPEGFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickJPEGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; KHOROS (.viff, .xv)
Root: HKCR; Subkey: ".viff"; ValueType: string; ValueName: ""; ValueData: "ImageMagickKHOROSFile"
Root: HKCR; Subkey: ".xv"; ValueType: string; ValueName: ""; ValueData: "ImageMagickKHOROSFile"
Root: HKCR; Subkey: "ImageMagickKHOROSFile"; ValueType: string; ValueName: ""; ValueData: "Khoros Visualization Image"
Root: HKCR; Subkey: "ImageMagickKHOROSFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickKHOROSFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickKHOROSFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MNG (.mng)
Root: HKCR; Subkey: ".mng"; ValueType: string; ValueName: ""; ValueData: "ImageMagickMNGFile"
Root: HKCR; Subkey: "ImageMagickMNGFile"; ValueType: string; ValueName: ""; ValueData: "Multiple-image Network Graphics File"
Root: HKCR; Subkey: "ImageMagickMNGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickMNGFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickMNGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MPC (.mpc)
Root: HKCR; Subkey: ".mpc"; ValueType: string; ValueName: ""; ValueData: "ImageMagickMPCFile"
Root: HKCR; Subkey: "ImageMagickMPCFile"; ValueType: string; ValueName: ""; ValueData: "Magick Persistent Cache"
Root: HKCR; Subkey: "ImageMagickMPCFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickMPCFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickMPCFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MIFF (.miff)
Root: HKCR; Subkey: ".miff"; ValueType: string; ValueName: ""; ValueData: "ImageMagickMIFFFile"
Root: HKCR; Subkey: "ImageMagickMIFFFile"; ValueType: string; ValueName: ""; ValueData: "Magick Image File"
Root: HKCR; Subkey: "ImageMagickMIFFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickMIFFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickMIFFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MSL (.msl)
Root: HKCR; Subkey: ".msl"; ValueType: string; ValueName: ""; ValueData: "ImageMagickMSLFile"
Root: HKCR; Subkey: "ImageMagickMSLFile"; ValueType: string; ValueName: ""; ValueData: "Magick Scripting Language"
Root: HKCR; Subkey: "ImageMagickMSLFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickMSLFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickMSLFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MVG (.mvg)
Root: HKCR; Subkey: ".mvg"; ValueType: string; ValueName: ""; ValueData: "ImageMagickMVGFile"
Root: HKCR; Subkey: "ImageMagickMVGFile"; ValueType: string; ValueName: ""; ValueData: "Magick Vector Graphics"
Root: HKCR; Subkey: "ImageMagickMVGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickMVGFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickMVGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PBM (.pbm)
Root: HKCR; Subkey: ".pbm"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPBMFile"
Root: HKCR; Subkey: "ImageMagickPBMFile"; ValueType: string; ValueName: ""; ValueData: "Portable Bitmap File"
Root: HKCR; Subkey: "ImageMagickPBMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPBMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPBMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PGM (.pgm)
Root: HKCR; Subkey: ".pgm"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPGMFile"
Root: HKCR; Subkey: "ImageMagickPGMFile"; ValueType: string; ValueName: ""; ValueData: "Portable Graymap File"
Root: HKCR; Subkey: "ImageMagickPGMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPGMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPGMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PPM (.ppm)
Root: HKCR; Subkey: ".ppm"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPPMFile"
Root: HKCR; Subkey: "ImageMagickPPMFile"; ValueType: string; ValueName: ""; ValueData: "Portable Pixmap File"
Root: HKCR; Subkey: "ImageMagickPPMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPPMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPPMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PNG (.png)
Root: HKCR; Subkey: ".png"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPNGFile"
Root: HKCR; Subkey: "ImageMagickPNGFile"; ValueType: string; ValueName: ""; ValueData: "Portable Network Graphics File"
Root: HKCR; Subkey: "ImageMagickPNGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPNGFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPNGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PTIF (.ptif)
Root: HKCR; Subkey: ".ptif"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPTIFFile"
Root: HKCR; Subkey: "ImageMagickPTIFFile"; ValueType: string; ValueName: ""; ValueData: "Pyramid encoded TIFF"
Root: HKCR; Subkey: "ImageMagickPTIFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPTIFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPTIFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; TIFF (.fax, .g3, .tif, .tiff)
Root: HKCR; Subkey: ".fax"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTIFFFile"
Root: HKCR; Subkey: ".g3"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTIFFFile"
Root: HKCR; Subkey: ".tif"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTIFFFile"
Root: HKCR; Subkey: ".tiff"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTIFFFile"
Root: HKCR; Subkey: "ImageMagickTIFFFile"; ValueType: string; ValueName: ""; ValueData: "Tagged Image File Format"
Root: HKCR; Subkey: "ImageMagickTIFFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickTIFFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickTIFFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PCX (.pcx)
Root: HKCR; Subkey: ".pcx"; ValueType: string; ValueName: ""; ValueData: "ImageMagickDCXFile"
Root: HKCR; Subkey: "ImageMagickDCXFile"; ValueType: string; ValueName: ""; ValueData: "ZSoft Paintbrush File"
Root: HKCR; Subkey: "ImageMagickDCXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickDCXFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickDCXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; FITS (.fits)
Root: HKCR; Subkey: ".FITS"; ValueType: string; ValueName: ""; ValueData: "ImageMagickFITSFile"
Root: HKCR; Subkey: "ImageMagickFITSFile"; ValueType: string; ValueName: ""; ValueData: "Flexible Image Transport System"
Root: HKCR; Subkey: "ImageMagickFITSFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickFITSFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickFITSFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; TARGA (.icb, .tga, .vda, .vst)
Root: HKCR; Subkey: ".icb"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTARGAFile"
Root: HKCR; Subkey: ".tga"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTARGAFile"
Root: HKCR; Subkey: ".vda"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTARGAFile"
Root: HKCR; Subkey: ".vst"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTARGAFile"
Root: HKCR; Subkey: "ImageMagickTARGAFile"; ValueType: string; ValueName: ""; ValueData: "Truevision Targa Image"
Root: HKCR; Subkey: "ImageMagickTARGAFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickTARGAFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickTARGAFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; ICON (.ico, .icon)
Root: HKCR; Subkey: ".ico"; ValueType: string; ValueName: ""; ValueData: "ImageMagickICONFile"
Root: HKCR; Subkey: ".icon"; ValueType: string; ValueName: ""; ValueData: "ImageMagickICONFile"
Root: HKCR; Subkey: "ImageMagickICONFile"; ValueType: string; ValueName: ""; ValueData: "Microsoft Windows Icon"
Root: HKCR; Subkey: "ImageMagickICONFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickICONFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickICONFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MATLAB (.mat)
Root: HKCR; Subkey: ".mat"; ValueType: string; ValueName: ""; ValueData: "ImageMagickMATLABFile"
Root: HKCR; Subkey: "ImageMagickMATLABFile"; ValueType: string; ValueName: ""; ValueData: "MATLAB Image File"
Root: HKCR; Subkey: "ImageMagickMATLABFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickMATLABFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickMATLABFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; MTV (.mtv)
Root: HKCR; Subkey: ".mtv"; ValueType: string; ValueName: ""; ValueData: "ImageMagickMTVFile"
Root: HKCR; Subkey: "ImageMagickMTVFile"; ValueType: string; ValueName: ""; ValueData: "MTV Raytrace Image File"
Root: HKCR; Subkey: "ImageMagickMTVFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickMTVFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickMTVFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; OTB (.otb)
Root: HKCR; Subkey: ".otb"; ValueType: string; ValueName: ""; ValueData: "ImageMagickOTBFile"
Root: HKCR; Subkey: "ImageMagickOTBFile"; ValueType: string; ValueName: ""; ValueData: "On The Air Bitmap"
Root: HKCR; Subkey: "ImageMagickOTBFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickOTBFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickOTBFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PALM (.palm)
Root: HKCR; Subkey: ".palm"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPALMFile"
Root: HKCR; Subkey: "ImageMagickPALMFile"; ValueType: string; ValueName: ""; ValueData: "Palm Pixmap File"
Root: HKCR; Subkey: "ImageMagickPALMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPALMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPALMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PCD (.pcd)
Root: HKCR; Subkey: ".pcd"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPCDFile"
Root: HKCR; Subkey: "ImageMagickPCDFile"; ValueType: string; ValueName: ""; ValueData: "Photo CD"
Root: HKCR; Subkey: "ImageMagickPCDFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPCDFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPCDFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PDF (.pdf)
Root: HKCR; Subkey: ".pdf"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPDFFile"
Root: HKCR; Subkey: "ImageMagickPDFFile"; ValueType: string; ValueName: ""; ValueData: "Portable Document Format"
Root: HKCR; Subkey: "ImageMagickPDFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPDFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPDFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PICT (.pct, .pict )
Root: HKCR; Subkey: ".pct"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPICTFile"
Root: HKCR; Subkey: ".pict"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPICTFile"
Root: HKCR; Subkey: "ImageMagickPICTFile"; ValueType: string; ValueName: ""; ValueData: "Apple Mac QuickDraw/PICT"
Root: HKCR; Subkey: "ImageMagickPICTFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPICTFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPICTFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PICON (.picon)
Root: HKCR; Subkey: ".picon"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPICONFile"
Root: HKCR; Subkey: "ImageMagickPICONFile"; ValueType: string; ValueName: ""; ValueData: "Personal Icon"
Root: HKCR; Subkey: "ImageMagickPICONFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPICONFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPICONFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PIX (.pix, .rla)
Root: HKCR; Subkey: ".pix"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPIXFile"
Root: HKCR; Subkey: ".rla"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPIXFile"
Root: HKCR; Subkey: "ImageMagickPIXFile"; ValueType: string; ValueName: ""; ValueData: "Alias/Wavefront RLE Image"
Root: HKCR; Subkey: "ImageMagickPIXFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPIXFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPIXFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PSD (.psd)
Root: HKCR; Subkey: ".psd"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPSDFile"
Root: HKCR; Subkey: "ImageMagickPSDFile"; ValueType: string; ValueName: ""; ValueData: "Adobe Photoshop Image"
Root: HKCR; Subkey: "ImageMagickPSDFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPSDFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPSDFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; Postscript (.ps)
Root: HKCR; Subkey: ".ps"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPostscriptFile"
Root: HKCR; Subkey: "ImageMagickPostscriptFile"; ValueType: string; ValueName: ""; ValueData: "Postscript File"
Root: HKCR; Subkey: "ImageMagickPostscriptFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPostscriptFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPostscriptFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; PSFONT (.pfa, .pfb)
Root: HKCR; Subkey: ".pfa"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPSFONTFile"
Root: HKCR; Subkey: ".pfb"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPSFONTFile"
Root: HKCR; Subkey: "ImageMagickPSFONTFile"; ValueType: string; ValueName: ""; ValueData: "Postscript Font"
Root: HKCR; Subkey: "ImageMagickPSFONTFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPSFONTFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPSFONTFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; SFW (.pwp, .sfw)
Root: HKCR; Subkey: ".pwp"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPWPFile"
Root: HKCR; Subkey: ".sfw"; ValueType: string; ValueName: ""; ValueData: "ImageMagickPWPFile"
Root: HKCR; Subkey: "ImageMagickPWPFile"; ValueType: string; ValueName: ""; ValueData: "Seattle File Works Image"
Root: HKCR; Subkey: "ImageMagickPWPFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickPWPFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickPWPFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; TIM (.tim)
Root: HKCR; Subkey: ".tim"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTIMFile"
Root: HKCR; Subkey: "ImageMagickTIMFile"; ValueType: string; ValueName: ""; ValueData: "PSX TIM Image"
Root: HKCR; Subkey: "ImageMagickTIMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickTIMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickTIMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; TTF (.ttf)
Root: HKCR; Subkey: ".ttf"; ValueType: string; ValueName: ""; ValueData: "ImageMagickTTFFile"
Root: HKCR; Subkey: "ImageMagickTTFFile"; ValueType: string; ValueName: ""; ValueData: "TrueType Font"
Root: HKCR; Subkey: "ImageMagickTTFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickTTFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickTTFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; VICAR (.vicar)
Root: HKCR; Subkey: ".vicar"; ValueType: string; ValueName: ""; ValueData: "ImageMagickVICARFile"
Root: HKCR; Subkey: "ImageMagickVICARFile"; ValueType: string; ValueName: ""; ValueData: "VICAR Rasterfile"
Root: HKCR; Subkey: "ImageMagickVICARFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickVICARFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickVICARFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; WBMP (.wbmp)
Root: HKCR; Subkey: ".wbmp"; ValueType: string; ValueName: ""; ValueData: "ImageMagickWBMPFile"
Root: HKCR; Subkey: "ImageMagickWBMPFile"; ValueType: string; ValueName: ""; ValueData: "Wireless Bitmap (Level 0) Image"
Root: HKCR; Subkey: "ImageMagickWBMPFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickWBMPFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickWBMPFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; WMF (.wmf)
Root: HKCR; Subkey: ".wmf"; ValueType: string; ValueName: ""; ValueData: "ImageMagickWMFFile"
Root: HKCR; Subkey: "ImageMagickWMFFile"; ValueType: string; ValueName: ""; ValueData: "Windows Meta File"
Root: HKCR; Subkey: "ImageMagickWMFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickWMFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickWMFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; XBM (.xbm)
Root: HKCR; Subkey: ".xbm"; ValueType: string; ValueName: ""; ValueData: "ImageMagickXBMFile"
Root: HKCR; Subkey: "ImageMagickXBMFile"; ValueType: string; ValueName: ""; ValueData: "X Windows Bitmap"
Root: HKCR; Subkey: "ImageMagickXBMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickXBMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickXBMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; XCF (.xcf)
Root: HKCR; Subkey: ".xcf"; ValueType: string; ValueName: ""; ValueData: "ImageMagickXCFFile"
Root: HKCR; Subkey: "ImageMagickXCFFile"; ValueType: string; ValueName: ""; ValueData: "GIMP Image"
Root: HKCR; Subkey: "ImageMagickXCFFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickXCFFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickXCFFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; XFIG (.fig)
Root: HKCR; Subkey: ".fig"; ValueType: string; ValueName: ""; ValueData: "ImageMagickXFIGFile"
Root: HKCR; Subkey: "ImageMagickXFIGFile"; ValueType: string; ValueName: ""; ValueData: "Xfig File"
Root: HKCR; Subkey: "ImageMagickXFIGFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickXFIGFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickXFIGFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; XPM (.xpm)
Root: HKCR; Subkey: ".xpm"; ValueType: string; ValueName: ""; ValueData: "ImageMagickXPMFile"
Root: HKCR; Subkey: "ImageMagickXPMFile"; ValueType: string; ValueName: ""; ValueData: "X Windows Pixmap"
Root: HKCR; Subkey: "ImageMagickXPMFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickXPMFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickXPMFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

; XWD (.xwd)
Root: HKCR; Subkey: ".xwd"; ValueType: string; ValueName: ""; ValueData: "ImageMagickXWDFile"
Root: HKCR; Subkey: "ImageMagickXWDFile"; ValueType: string; ValueName: ""; ValueData: "X Window Dump"
Root: HKCR; Subkey: "ImageMagickXWDFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ImageMagick.ico,1"
Root: HKCR; Subkey: "ImageMagickXWDFile\shell\open"; ValueType: none
Root: HKCR; Subkey: "ImageMagickXWDFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\IMDisplay.exe"" ""%1"""

[Files]
; Executables
Source: "bin\IMDisplay.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\animate.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\composite.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\conjure.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\convert.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\display.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\hp2xx.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\identify.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\import.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\mogrify.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\montage.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\mpeg2dec.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\mpeg2enc.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; DLLs
Source: "bin\CORE_RL*.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\IM_MOD_RL*.dll"; DestDir: "{app}\modules"; CopyMode: alwaysoverwrite
Source: "bin\X11.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "bin\Xext.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; Configuration files
Source: "..\magick\colors.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "bin\delegates.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "..\magick\magic.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "..\coders\modules.mgk"; DestDir: "{app}\modules"; CopyMode: alwaysoverwrite
Source: "bin\type-ghostscript.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
Source: "bin\type.mgk"; DestDir: "{app}\config"; CopyMode: alwaysoverwrite
; Release documentation files
Source: "..\ChangeLog"; DestDir: "{app}"; CopyMode: alwaysoverwrite; DestName: "ChangeLog.txt"
Source: "..\Copyright.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\QuickStart.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\README.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; Web pages
Source: "..\ImageMagick.html"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: isreadme
Source: "..\images\*.jpg"; DestDir: "{app}\images"; CopyMode: alwaysoverwrite
Source: "..\images\*.png"; DestDir: "{app}\images"; CopyMode: alwaysoverwrite
Source: "..\www\*.c"; DestDir: "{app}\www"; CopyMode: alwaysoverwrite
Source: "..\www\*.css"; DestDir: "{app}\www"; CopyMode: alwaysoverwrite
Source: "..\www\*.html"; DestDir: "{app}\www"; CopyMode: alwaysoverwrite
Source: "..\www\Magick++\*.html"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite
Source: "..\www\Magick++\*.jpg"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite
Source: "..\www\Magick++\*.png"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite
Source: "..\www\Magick++\COPYING"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite; DestName: "COPYING.txt"
Source: "..\www\Magick++\README.txt"; DestDir: "{app}\www\Magick++"; CopyMode: alwaysoverwrite
Source: "..\www\api\*.html"; DestDir: "{app}\www\api"; CopyMode: alwaysoverwrite
Source: "..\www\api\*.html"; DestDir: "{app}\www\api"; CopyMode: alwaysoverwrite
Source: "..\www\api\types\*.html"; DestDir: "{app}\www\api\types"; CopyMode: alwaysoverwrite
; Delegate library licenses
Source: "..\licenses\*.txt"; DestDir: "{app}\licenses"; CopyMode: alwaysoverwrite
; Icon file
Source: "..\magick\ImageMagick.ico"; DestDir: "{app}"; CopyMode: alwaysoverwrite
; Path Tool
Source: "..\contrib\win32\PathTool\PathTool.exe"; DestDir: "{app}\uninstall"; CopyMode: alwaysoverwrite
Source: "..\contrib\win32\PathTool\PathTool.pdf"; DestDir: "{app}\uninstall"; CopyMode: alwaysoverwrite
; PerlMagick Perl module
Source: "..\PerlMagick\Image-Magick.ppd"; DestDir: "{app}\PerlMagick"; CopyMode: alwaysoverwrite
Source: "..\PerlMagick\Image-Magick.tar.gz"; DestDir: "{app}\PerlMagick\x86"; CopyMode: alwaysoverwrite
Source: "..\PerlMagick\demo\*"; DestDir: "{app}\PerlMagick\demo"; CopyMode: alwaysoverwrite
Source: "PerlMagick-PPM.txt"; DestDir: "{app}\PerlMagick"; CopyMode: alwaysoverwrite; DestName: "README.txt"
; ImageMagickObject DLL (usable from Visual Basic and WSH)
Source: "bin\ImageMagickObject.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "..\contrib\win32\ATL\ImageMagickObject\README.txt"; DestDir: "{app}\ImageMagickObject"; CopyMode: alwaysoverwrite
Source: "..\contrib\win32\ATL\ImageMagickObject\Tests\VBTest\*"; DestDir: "{app}\ImageMagickObject\VBExample"; CopyMode: alwaysoverwrite
Source: "..\contrib\win32\ATL\ImageMagickObject\Tests\WSHTest\*"; DestDir: "{app}\ImageMagickObject\WSHExample"; CopyMode: alwaysoverwrite
;  Development libraries
Source: "lib\CORE_RL_Magick++_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_bzlib_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_fpx_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_jbig_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_jp2_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_jpeg_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_lcms_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_libxml_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
Source: "lib\CORE_RL_magick_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_png_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_tiff_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_ttf_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_wmf_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;Source: "lib\CORE_RL_zlib_.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
Source: "lib\X11.lib"; DestDir: "{app}\lib"; CopyMode: alwaysoverwrite
;  Development headers
Source: "..\magick\api.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\attribute.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\blob.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\cache.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\cache_view.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\classify.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\command.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\compress.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\delegate.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\draw.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\error.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\gem.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\image.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\list.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\magic.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\magick_config.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\magick.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\module.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\monitor.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\PreRvIcccm.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\quantize.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\registry.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\render.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\semaphore.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\timer.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\utility.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\version.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\widget.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\magick\xwindow.h"; DestDir: "{app}\include\magick"; CopyMode: alwaysoverwrite
Source: "..\Magick++\lib\Magick++.h"; DestDir: "{app}\include"; CopyMode: alwaysoverwrite
Source: "..\Magick++\lib\Magick++\*.h"; DestDir: "{app}\include\Magick++"; CopyMode: alwaysoverwrite

[Icons]
Name: "{group}\ImageMagick Web Pages"; Filename: "{app}\ImageMagick.html"
; Name: "{group}\ImageMagick Readme"; Filename: "{app}\README.txt"
; Name: "{group}\ImageMagick Quick Start"; Filename: "{app}\QuickStart.txt"
; Name: "{group}\ImageMagick License"; Filename: "{app}\Copyright.txt"
Name: "{group}\ImageMagick Display"; Filename: "{app}\IMDisplay.exe"; IconFilename: "{app}\ImageMagick.ico"
Name: "{userdesktop}\ImageMagick Display"; Filename: "{app}\IMDisplay.exe"; IconFilename: "{app}\ImageMagick.ico"; Tasks: desktop_icon; MinVersion: 4,4

[Run]
; Add -debug to parameters to enable debugging
Filename: "{app}\uninstall\PathTool.exe"; Parameters: "-silent -a:""{app}"""; StatusMsg: "Updating environment variables..."; Tasks: update_path
Filename: "ppm"; Parameters: "install Image-Magick.ppd"; WorkingDir: "{app}\PerlMagick"; StatusMsg: "Installing PerlMagick..."; Tasks: install_PerlMagick; Flags: shellexec
; Add /s to regsvr32 for silent operation
Filename: "regsvr32"; Parameters: "/c ""{app}\ImageMagickObject.dll"""; StatusMsg: "Registering ImageMagickObject DLL..."; Tasks: install_Magick_DLL

[UninstallRun]
; Add -debug to parameters to enable debugging
Filename: "{app}\uninstall\PathTool.exe"; Parameters: "-silent -r:""{app}"""; StatusMsg: "Restoring environment variables..."; Tasks: update_path
Filename: "ppm"; Parameters: "remove Image-Magick"; StatusMsg: "Uninstalling PerlMagick..."; Tasks: install_PerlMagick; Flags: shellexec
; Add /s to regsvr32 for silent operation
Filename: "regsvr32"; Parameters: "/u ""{app}\ImageMagickObject.dll"""; StatusMsg: "Unregistering ImageMagickObject DLL..."; Tasks: install_Magick_DLL

