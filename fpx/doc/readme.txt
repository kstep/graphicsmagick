============================================================================
Release Notes for FlashPix Reference Implementation
Filename: READ_REF.REF
Version: 1.2
============================================================================

--------- Table of Contents ----------
1) List of Components
2) Reference Implementation Usage Notes
3) Modifications/Bug Fixes
4) Known problems
5) Build procedures for Windows
6) Build procedures for Macintosh

-------------------------  (1) List of Components  -------------------------

The FlashPix Reference Implementation consists of the following items:

	DOC    - contains the FlashPix Programmers API manual AND
		 architecture Specification
	FPX    - contains the source and build files for the entire RI.

--------------  (2) Reference Implementation Usage Notes  ------------------


1)	If you are planning on building and shipping Reference Implementation
	on Windows you should highly consider renaming FPXLIB.DLL to something
	else in order to avoid conflicts with other products.

2)  The View Layer calls (fpxlib.h) do not update the actual FlashPix file.
	The View Layer calls such as FPX_SetImageInWorldColorTwist() apply
	viewing transform to the current Image in world.  To save View Layer
	updates an application must call an appropriate I/O level call such as
	FPX_SetImageColorTwist() to have changes committed to the file.

3)  FPX_SetDecimationQuality() interface takes an unsigned short as
	the decimationQuality.  If  (decimationQuality >= 4), Gaussian convolution
	will be used to perform decimation, otherwise  Standard Convolution will be
	used.  The Reference Implementation uses Standard Convolution as default when
	performing decimation.  

4) 	The default behavior of Reference Implementation is to use Nearest
	Neighbor when performing interpolation when doing image contruction.  
	FPX_SetResampleMethod()  may be used to select Linear Interpolation.  
			FPX_SetResampleMethod(FPX_LINEAR_INTERPOLATION)
	This will result in better image quality when Reference Implementation 
	interpolates image data from lower resolution data.
	
5)	For best results, use 24-bit color display settings with the Reference
	Implementation libraries.  The results at lesser color bit depths might 
	not meet your user's expectation

---------- (3) Modifications/Bug Fixes since v1.1 SDK release  ------------

NOTE: There where several modifications to the source code since version 1.1
	of the SDK.  If you have modified previous versions of the source, you 
	should  examine the differences between 1.1 and 1.2 versions carefully.

1)	FlashPix files from Microsoft Picture It! v2 can now be read.  Their
	alternative, valid use of the OLE property set dictionary property is
	now supported.

2)	An empty string at the end of a list in the Extension List property set
	is now handled correctly.

3)	Corrected colors used for new image background and writing a background
	rectangle in Windows.  No change for Macintosh.

4)	Adjusted thumbnail creation for non-hierarchical file authoring.

5)	Fixed an obscure crash upon memory deallocation.

6)	Removed assumption that Transform 1 is always associated with the primary
	visible output.

7)	Improved file I/O error reporting.

8)	Fixed buffer overwrite caused by rounding up window dimensions during
	resize.

9)	Added writing a Users property in the Source Description Property Set
	in few conditions where there is a transform, but it was not being
	authored.

10)	Changed rendering of monochrome images with colortwist viewing parameter
	to tint the image.

11)	Authoring 0 value compression subtype field unless single color compression
	is used.

12)	Added support for multiple visible outputs.

13)	Added API to get contents of the Global Info Property Set.

--------------------  (4) Known problems/limitations  ----------------------

1)	FPX_GetToolkitVersion() only returns the major release number. Implementors
	are encouraged to create their own DLLs/Libraries and add their own API
	call for determining release version.

2)	FPX_GetResolutionInfo() API is not intended to return compression
	information for an existing FlashPix file.  It's functionality is limited to
	echoing back resolution information that was set in FPX_SetResolutionInfo() 
	when creating FlashPix files.  FPX_GetResolutionInfo() will correctly return 
	the number of resolutions present.

3)	FPX_ReadImageTile() is returning an error when attempting to read outside
	the current Rectangle of Interest.
	This error can be avoided by:
		- reading the current ROI
		- re-setting the ROI to be (0, 0, r, 1), where "r" is the images
		  result aspect ratio
		- performing FPX_ReadImageTile()
		- restoring the ROI back to the original values obtained earlier

4)	The code that determines if a tile can be single color compressed does not
	test to see if there are changes in opacity. (note, however, that it is
	unlikely that constant color component values will have varying opacity
	values)

5)	JPEG compressed images with opacity will sometimes exhibit overflow; i.e.,
	incorrect pixels with bright or saturated color.

6)	Memory leaks have been reported in the OLE/Structured Storage subsystem.

7)	All errors from the JPEG subsystem are reported as FPX_INVALID_COMPRESSION.

8)	There are some conditions under which opacity values are shifted one
	pixel to the right.  This can cause overflow and the display of bright,
	saturated colors.

9)	Return status can be lost during Page rendering.

10)	Contrast applied to PhotoYCC images with opacity causes magenta line 
	effects at the edges of the opacity area.

11)	JPEG compressed monochrome images can exhibit overflow with a white
	background.

-------------------  (5) Build procedures for Windows  ---------------------

Building Windows Libraries 

There are 2 DLL's and LIB's to build - JPEGLIB and FPXLIB. Just open the
fpxlib.dsw workspace in _win directory in VisualC++ 6 and build the release
configuration of FPXLIB.DLL. This will also cause the JPEGLIB project to
be built.

Results:
	-win\fpx_rls\FPXLIB.DLL
	-win\fpx_rls\FPXLIB.LIB
	-win\jpeg_rls\JPEGLIB.DLL
	-win\jpeg_rls\JPEGLIB.LIB

Use the DLLs found in the rls subdirectories as your runtime DLLs. Ensure that
they are copied or installed into your executable path.

------------------  (6) Build procedures for Macintosh  --------------------

NOTE: The information contained below is based on earlier work and may not be
accurate. The truth is that I do not have a clue if this release builds on the
Macintosh and need someone with Mac programming experience to see if it still
does. I have every reason to believe that whatever issues arise will be minor.
Please e-mail billr@corbis.com if you wish to take on this challenge. Your help
will be greatly appreciated.

Building Macintosh PPC Libraries

1) Make sure you have CW10 installed on your machine

2) Make sure you have OLE 2.07 installed as well. It is provided on this disk
   as a compressed file. (SSTORAGE:LIBS:MAC:MACOLE.SDK).

3) Expand the CW projects and example code by double clicking on the self-
	extracting archive _mac.sea that is located in the REF_IMPL:SOURCE folder. 
	This will create the _MAC folder and enclosed files.
	A READ_ME file is included within the archive that describes the projects
	and example code.
   
4) To build a new Reference Implementation:
    - double click on the jpeglib (or jpeglib68k) project, compile and link 
      it (command Make of the Project menu)
    - double click on the fpxlib (or fpxlib68k) project, compile and link it.
	  This creates a fpxlib.lib (or fpxlib68k.lib) that you should leave in 
	  the _mac folder (for use with the example code) and/or copy to a more
	  global folder such as REFIMPLP:PUBLIC:MAC (for use with your projects.

6) To test the Reference Implementation with the provided example
   projects:
	- double click on the desired example project. Projects with "68k" in 
	  their name make 68k executables. The projects without "68k" in thier
	  name make PowerPC executables.
	- double-click the created applcation. The READ_ME file in the _mac
	  folder describes the example programs and provides further information.
	   
Troubleshooting:

    - Can't compile the OLE part: 
        Make sure that the OLE "CIncludes" folder is in the search path of 
        the project. This folder is in the MACOLE.SDK archive on the FlashPix
        SDK disc. 
        You have 2 solutions:
        - copy the "CIncludes" folder into the Metrowerks "MacOS Support:Headers"
          folder
        - add a path to the "CIncludes" folder in the Access Paths list in the
		  Project Settings for the project.

    - Can't link the OLE part: 
        Same thing for the "Microsoft_OLE2.xcoff" library (PPC) or the object
        file "Ole2Lib.far.debug.o" (68k):
        - copy it into a folder called "OLE" in the "MacOS Support:Libraries"
          of the Metrowerks folder
        - add a path in the Project Settings Access Paths list

    - Can't launch the application (PPC): 
        Make sure that the shared libraries are either in your extension
        folder or (better for debug) in the same folder as your application

    - Crashes in OLE at run time: 
            Look into your "Extensions" folder. You must have:
            - Microsoft OLE Automation
            - Microsoft OLE Extension
            - Microsoft OLE Library
            - Type Libraries: this is a folder containing:
            	Standard OLE Types (PowerMac)
	  These extensions are included in archive SSTORAGE:LIB:MAC:MACOLE.SDK
	  on the SDK disc.

	- ostream or ios objects unresolved during link:
		Make sure that the "MSL C" and "MSL C++" folders are listed in the
		Access Paths project setting and that the ANSI folders are no longer
		listed. (note: this only occurs after updating the provided CW10
		projects to CW11 or later and replacing the obsolete ANSI libraries
		with the new, MSL libraries)
