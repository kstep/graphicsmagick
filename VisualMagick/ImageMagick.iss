; -- Sample1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=ImageMagick
AppVerName=ImageMagick version 5.4.7
AppCopyright=Copyright (C) 2002 ImageMagick Studio
DefaultDirName={pf}\ImageMagick
DefaultGroupName=ImageMagick
UninstallDisplayIcon={app}\MyProg.exe
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Files]
Source: "bin\IMDisplay.exe"; DestDir: "{app}"
Source: "bin\animate.exe"; DestDir: "{app}"
Source: "bin\composite.exe"; DestDir: "{app}"
Source: "bin\conjure.exe"; DestDir: "{app}"
Source: "bin\convert.exe"; DestDir: "{app}"
Source: "bin\display.exe"; DestDir: "{app}"
Source: "bin\identify.exe"; DestDir: "{app}"
Source: "bin\import.exe"; DestDir: "{app}"
Source: "bin\mogrify.exe"; DestDir: "{app}"
Source: "bin\montage.exe"; DestDir: "{app}"
Source: "bin\mpeg2dec.exe; DestDir: "{app}"
Source: "bin\mpeg2enc.exe; DestDir: "{app}"
Source: "bin\CORE_*.dll"; DestDir: "{app}"
Source: "bin\IM_MOD_*.dll"; DestDir: "{app}"
Source: "bin\X11.dll"; DestDir: "{app}"
Source: "bin\Xext.dll"; DestDir: "{app}"
Source: "bin\colors.mgk"; DestDir: "{app}"
Source: "bin\delegates.mgk"; DestDir: "{app}"
Source: "bin\fonts.mgk"; DestDir: "{app}"
Source: "bin\magic.mgk"; DestDir: "{app}"
Source: "bin\modules.mgk"; DestDir: "{app}"
Source: "bin\type-ghostscript.mgk"; DestDir: "{app}"
Source: "bin\type-windows.mgk"; DestDir: "{app}"
Source: "bin\type.mgk"; DestDir: "{app}"
Source: "..\ChangeLog"; DestDir: "{app}"
Source: "..\Copyright.txt"; DestDir: "{app}"
Source: "..\QuickStart.txt"; DestDir: "{app}"; Flags: isreadme
Source: "..\README.txt"; DestDir: "{app}"
Source: "..\ImageMagick.html"; DestDir: "{app}"
Source: "..\images\*.jpg"; DestDir: "{app}\images"
Source: "..\images\*.png"; DestDir: "{app}\images"
Source: "..\www\*.html"; DestDir: "{app}\www"
Source: "..\www\*.css"; DestDir: "{app}\www"
Source: "..\www\*.c"; DestDir: "{app}\www"
Source: "..\www\api\*.html"; DestDir: "{app}\www\api"
Source: "..\www\api\*.html"; DestDir: "{app}\www\api"
Source: "..\www\api\types\*.html"; DestDir: "{app}\www\api\types"
Source: "..\www\Magick++\*.html"; DestDir: "{app}\www\Magick++"
Source: "..\www\Magick++\*.png"; DestDir: "{app}\www\Magick++"
Source: "..\www\Magick++\*.jpg"; DestDir: "{app}\www\Magick++"
Source: "..\www\Magick++\COPYING"; DestDir: "{app}\www\Magick++"
Source: "..\www\Magick++\README.txt"; DestDir: "{app}\www\Magick++"
Source: "..\licenses\*.txt"; DestDir: "{app}\licenses"

[Icons]
Name: "{group}\ImageMagick"; Filename: "{app}\IMDisplay.exe"
