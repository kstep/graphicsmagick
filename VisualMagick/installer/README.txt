[ The files in this directory are to support the GraphicsMagick
  setup.exe-style installer package. The following are the
  instructions for how to build a Q:8 (or Q:16) DLL-based distribution
  installer package using Visual C++ 7.0 through at least Visual
  Studio Professional 2008. ]

	   Steps for building VisualMagick Distribution Package

0. Install prerequisite software

  a. Download and install Inno Setup 5 from
      "http://www.jrsoftware.org/isinfo.php".
  b. Download and install ActiveState ActivePerl from
     "http://www.activestate.com/activeperl/downloads/".

1. Build VisualMagick\configure\configure.exe (if missing/outdated)

   Open workspace VisualMagick\configure\configure.dsp by double-clicking
   from Windows Explorer.  Visual Studio should come up and offer to upgrade
   the project.

   a. Upgrade Visual Studio project (as required)
   b. Select "Rebuild All" to build configure.exe
   c. Close the project.

2. Configure GraphicsMagick Visual Studio Workspace
   a. Use Windows Explorer to navigate to VisualMagick\configure.
   b. Double-click on configure.exe to run configure program
   c. Select DLL build
   d. Check "Build demo and test programs"
   e. Click on Edit "magick_config.h" and ensure that UseInstalledGraphicsMagick
       and ProvideDllMain are defined.
   f. Finish remaining configure wizard screens to complete.
   g. File -> "Close Workspace"

3. Open Workspace, set build configuration, build GraphicsMagick software.

   Open workspace VisualMagick\VisualDynamicMT.sln by double-clicking from
   Windows Explorer or opening workpace via Visual C++ dialog.

   a. Build -> "Set Active Configuration" -> "All - Win32 Release" -> OK
   b. Open CORE_magick\magick\magick_config.h and edit any configuration
      settings you would like to change.
   c. Optionally enable OpenMP by selecting CORE_magick and adjusting
      its language settings to include OpenMP.  OpenMP requires Visual
      Studio Professional 2005 or later (i.e. not free version).
   d. Build -> "Rebuild All"
   e. Close workspace

4. Build ImageMagickObject

   a. cd PROJECT\contrib\win32\ATL7\ImageMagickObject
   b. BuildImageMagickObject clean
   c. BuildImageMagickObject release
   d. cd ..\..\..\..

5. Build PerlMagick

   a. Open Windows Command Shell Window
   b. cd PROJECT\PerlMagick
   c. nmake clean (only if this is a rebuild)
   d. perl Makefile.nt
   e. nmake release

   NOTE: access to nmake requires that there be a path to it.  Visual
   Studio provides one or more "Visual Studio XXXX Command Prompt"
   options which will open a Command Tool with the correct path
   settings for that version of Visual Studio.

6. Open VisualMagick\installer\gm-win32-Q8-dll-full.iss (or
   gm-win32-Q16-dll-full.iss if you set QuantumDepth=16) by
   double-clicking from Windows Explorer.

   a. File -> Compile
   b. Test install by clicking on green triangle
   c. Quit Inno Setup.

7.
   
   a. cd PROJECT\PerlMagick
   b. nmake test
      All tests must pass!

8. 
   a. cd PROJECT\VisualMagick\tests
   b. run_rwfile.bat
      All tests must pass!
   c. run_rwblob.bat
      All tests must pass!

9.
   a. cd Magick++/tests
   b. run_tests.bat
      All tests must pass!

10.
   a. cd Magick++/demo
   b. run_demos.bat
   c. Use 'gmdisplay' to visually inspect all output files.

11.
   Distribution package is available named similarly to
     VisualMagick\bin\GraphicsMagick-3.0-Q8-dll.exe

