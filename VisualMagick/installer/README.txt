[ The files in this directory are to support the ImageMagick setup.exe-style
  installer package. The following are the instructions for how to build
  an installer package using Visual C++ 6.0. ]

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
   1e) Click on Edit "magick_config.h" and ensure that UseInstalledImageMagick
       is defined.
   1f) Finish remaining configure wizard screens to complete.
   1g) File -> "Close Workspace"

2) Open workspace VisualMagick\VisualDynamicMT.dsw by double-clicking from
   Windows Explorer or opening workpace via Visual C++ dialog.

   2a) Build -> "Set Active Configuration" -> "All - Win32 Release" -> OK
   2b) Build -> "Rebuild All"
   2c) Build -> "Set Active Configuration" -> "ImageMagickObject - Win32
       Release" -> OK
   2d) Build -> "Build ImageMagickObject.dll"

3) Open Windows Command Shell Window

   3a) cd PerlMagick
   3b) nmake clean (only if this is a rebuild)
   3c) perl Makefile.nt
   3d) nmake release

NOTE: access to nmake requires that there be a path to it. Depending n how the install
of Visual Studio was done, this may not be the case. Visual Studio provides a batch
script in VC98\Bin called VCVARS32.BAT that can be used to do this manually after you
open up a command prompt.

4) Open VisualMagick\installer\ImageMagick-16.iss by double-clicking from Windows Explorer.

   4a) File -> Compile
   4b) Test install when requested, or by clicking on green triangle in
       Inno Setup Compiler 3.X versions.
   4c) Distribution package is available as
       VisualMagick\bin\ImageMagick-5.5.2-win2k.exe

