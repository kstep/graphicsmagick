
	   Steps for building VisualMagick Distribution Package

0) Install prerequisite software

  0a) Download and install the Inno Setup Compiler 2.0.19 (or later) from
      "http://www.jrsoftware.org/isinfo.php".
  0b) Download and install ActiveState ActivePerl from
     "http://www.activestate.com/Products/ActivePerl/Download.html".

1) Open workspace VisualMagick\configure\configure.dsw by double-clicking
   from Windows Explorer.

   1a) Select "Rebuild All"
   1b) Click on '!' icon to run configure program
   1c) Select DLL build
   1d) Uncheck "Use X11 Stubs" and check "Build demo and test programs"
   1e) Finish remaining configure wizard screens to complete.
   1f) File -> "Close Workspace"

2) Open workspace VisualMagick\VisualDynamicMT.dsw by double-clicking from
   Windows Explorer or opening workpace via Visual C++ dialog.

   2a) Build -> "Set Active Configuration" -> "All - Win32 Release" -> OK
   2b) Build -> "Rebuild All"
   2c) Build -> "Set Active Configuration" -> "ImageMagickObject - Win32
       Release" -> OK
   2d) Build -> "Build ImageMagickObject.dll"

3) Open Windows Command Shell Window

   3a) cd PerlMagick
   3b) nmake clean
   3c) perl Makefile.nt
   3d) nmake release

4) Open VisualMagick\ImageMagick.iss by double-clicking from Windows Explorer.

   4a) File -> Compile
   4b) Test install when requested, or by clicking on green triangle in
       Inno Setup Compiler 3.X versions.
   4c) Distribution package is available as
       VisualMagick\bin\ImageMagick-5.4.8-win2k.exe

