[ The files in this directory are to support the GraphicsMagick setup.exe-style
  installer package. The following are the instructions for how to build
  a Q:8 DLL-based distribution installer package using Visual C++ 7.0. ]

	   Steps for building VisualMagick Distribution Package

0) Install prerequisite software

  0a) Download and install the Inno Setup Compiler from
      "http://www.jrsoftware.org/isinfo.php".
  0b) Download and install ActiveState ActivePerl from
     "http://www.activestate.com/Products/ActivePerl/Download.html".

1) Open workspace VisualMagick\configure\configure.sln by double-clicking
   from Windows Explorer.

   1a) Select "Rebuild All"
   1b) Click on '!' icon to run configure program
   1c) Select DLL build
   1d) Uncheck "Use X11 Stubs" and check "Build demo and test programs"
   1e) Click on Edit "magick_config.h" and ensure that UseInstalledGraphicsMagick
       and ProvideDllMain are defined.
   1f) Finish remaining configure wizard screens to complete.
   1g) File -> "Close Workspace"

2) Open workspace VisualMagick\VisualDynamicMT.sln by double-clicking from
   Windows Explorer or opening workpace via Visual C++ dialog.

   2a) Build -> "Set Active Configuration" -> "All - Win32 Release" -> OK
   2b) Build -> "Rebuild All"

3) Build ImageMagickObject

   3a) cd contrib\win32\ATL7\ImageMagickObject
   3b) BuildImageMagickObject release
   3c) cd ..\..\..\..

4) Open Windows Command Shell Window

   4a) cd PerlMagick
   4b) nmake clean (only if this is a rebuild)
   4c) perl Makefile.nt
   4d) nmake release

NOTE: access to nmake requires that there be a path to it. Depending n how the install
of Visual Studio was done, this may not be the case. Visual Studio provides a batch
script in VC98\Bin called VCVARS32.BAT that can be used to do this manually after you
open up a command prompt.

5) Open VisualMagick\installer\gm-dll-8.iss by double-clicking from Windows Explorer.

   5a) File -> Compile
   5b) Test install by clicking on green triangle

6)
   6a) cd PerlMagick
   6b) nmake test
      All tests must pass!

7) 
   7a) cd VisualMagick\tests
   7b) run_rwfile.bat
      All tests must pass!
   7c) run_rwblob.bat
      All tests must pass!

8)
   8a) cd Magick++/tests
   8b) run_tests.bat
      All tests must pass!

9)
   9a) cd Magick++/demo
   9b) run_demos.bat
   9c) Use 'gmdisplay' to visually inspect all output files.

10)
   Distribution package is available as
     VisualMagick\bin\GraphicsMagick-1.0-Q8-dll.exe

