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
AppVerName=ImageMagick version 5.4.7
AppCopyright=Copyright (C) 2002 ImageMagick Studio
DefaultDirName={pf}\ImageMagick
DefaultGroupName=ImageMagick
UninstallDisplayIcon={app}\IMDisplay.exe
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Files]
; Executables
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
Source: "bin\mpeg2dec.exe"; DestDir: "{app}"
Source: "bin\mpeg2enc.exe"; DestDir: "{app}"
; DLLs
Source: "bin\CORE_RL*.dll"; DestDir: "{app}"
Source: "bin\IM_MOD_RL*.dll"; DestDir: "{app}"
Source: "bin\X11.dll"; DestDir: "{app}"
Source: "bin\Xext.dll"; DestDir: "{app}"
; Configuration files
Source: "bin\colors.mgk"; DestDir: "{app}"
Source: "bin\delegates.mgk"; DestDir: "{app}"
Source: "bin\fonts.mgk"; DestDir: "{app}"
Source: "bin\magic.mgk"; DestDir: "{app}"
Source: "bin\modules.mgk"; DestDir: "{app}"
Source: "bin\type-ghostscript.mgk"; DestDir: "{app}"
Source: "bin\type-windows.mgk"; DestDir: "{app}"
Source: "bin\type.mgk"; DestDir: "{app}"
; Release documentation files
Source: "..\ChangeLog"; DestDir: "{app}"; DestName: "ChangeLog.txt"
Source: "..\Copyright.txt"; DestDir: "{app}"
Source: "..\QuickStart.txt"; DestDir: "{app}"
Source: "..\README.txt"; DestDir: "{app}"
; Web pages
Source: "..\ImageMagick.html"; DestDir: "{app}"; Flags: isreadme
Source: "..\images\*.jpg"; DestDir: "{app}\images"
Source: "..\images\*.png"; DestDir: "{app}\images"
Source: "..\www\*.c"; DestDir: "{app}\www"
Source: "..\www\*.css"; DestDir: "{app}\www"
Source: "..\www\*.html"; DestDir: "{app}\www"
Source: "..\www\Magick++\*.html"; DestDir: "{app}\www\Magick++"
Source: "..\www\Magick++\*.jpg"; DestDir: "{app}\www\Magick++"
Source: "..\www\Magick++\*.png"; DestDir: "{app}\www\Magick++"
Source: "..\www\Magick++\COPYING"; DestDir: "{app}\www\Magick++"
Source: "..\www\Magick++\README.txt"; DestDir: "{app}\www\Magick++"
Source: "..\www\api\*.html"; DestDir: "{app}\www\api"
Source: "..\www\api\*.html"; DestDir: "{app}\www\api"
Source: "..\www\api\types\*.html"; DestDir: "{app}\www\api\types"
; Delegate library licenses
Source: "..\licenses\*.txt"; DestDir: "{app}\licenses"
; Icon file
Source: "..\magick\ImageMagick.ico"; DestDir: "{app}"

[Icons]
Name: "{group}\ImageMagick Web Pages"; Filename: "{app}\ImageMagick.html"
; Name: "{group}\ImageMagick Readme"; Filename: "{app}\README.txt"
; Name: "{group}\ImageMagick Quick Start"; Filename: "{app}\QuickStart.txt"
; Name: "{group}\ImageMagick License"; Filename: "{app}\Copyright.txt"
Name: "{group}\ImageMagick Display"; Filename: "{app}\IMDisplay.exe"; IconFilename: "{app}\ImageMagick.ico"
