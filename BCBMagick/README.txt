***********************************************************
**  GraphicsMagick library - v.1.1 - 02 Feb 2004         **
**                                                       **
**  Porting for compilation with Borland C++Builder 6.0  **
**-------------------------------------------------------**
**                                                       **
**  >>> README.txt                                       **
**                                                       **
**  Instructions for compilation                         **
**                                                       **
**-------------------------------------------------------**
**                                                       **
**  by Mike Chiarappa                                    **
**     mikechiarappa@libero.it                           **
***********************************************************

OVERVIEW
========
This is the **first** porting for compilation of GraphicsMagick library 
with Borland C++ Builder 6.0.
Actually is possible to obtain two versions:
 
  -1) LIB static version (.LIB)
  -2) DLL version

with a small subset of the main "delegates" library available. 
These are:

    bzlib
    lcms
    jpeg
    libtiff
    libpng
    FreeType library (ttf)
    zlib

...but the work is only at the beginning :-))
    
    
    
INSTRUCTIONS FOR COMPILATION
============================  
(1) Open the makefile:

        [GM Dir]\BCBMagick\All\BCBMagick.mak

    and modify the directory variable GMDIR to point the right GraphicsMagick's
    main project directory.
   
(2) Execute the batch file:

        [GM Dir]\BCBMagick\All\startup.bat
    
    This perform the copy of necessary modified files into GraphicsMagick's
    directory tree.        
    
(3a) If you want compile the static version of library (LIB), launch BCB IDE 
     and Open the Project Group [File->Open Project...]:

        [GM Dir]\BCBMagick\All\GMlib-1.1.bpg 

     then Build libraries [Project->Build all projects]. Will be segnaled some 
     warnings: nothing of important.
     Finally launch the batch file:

        [GM Dir]\BCBMagick\All\getlibs.bat
 
     Will be created the [LIB] directory that will contain all libs generated.

(3b) If you want compile the DLL version of library, launch BCB IDE and Open
     the Project Group [File->Open Project...]:

        [GM Dir]\BCBMagick\All\GMdll-1.1.bpg 

     The compiler will segnal an error for each DLL to build:
     
        "Cannot find resource file XXX.res. Recreated."
        
     Press Ok each time: will be created needed resource files. Then Build 
     DLLs [Project->Build all projects]. 
     Finally launch the batch file:

        [GM Dir]\BCBMagick\All\getdlls.bat
 
     Will be created the [DLL] directory that will contain all DLLs and needed
     Import Libraries generated.

4)  Collect all configuration files (.MGK) into directory [MGK] launching the
    batch file:
    
        [GM Dir]\BCBMagick\All\getmgks.bat
 
    This is all to do !!!
    


USE OF STATIC LIBRARY IN YOUR PROJECTS
=======================================        
(1) Add into Project Manager of your application the following libraries:

        [GM dir]\BCBMagick\lib\libbz2.lib
        [GM dir]\BCBMagick\lib\libCoders.lib
        [GM dir]\BCBMagick\lib\jpeglib.lib
        [GM dir]\BCBMagick\lib\lcms.lib
        [GM dir]\BCBMagick\lib\libMagick.lib
        [GM dir]\BCBMagick\lib\libMagickpp.lib		(only for Magick++)
        [GM dir]\BCBMagick\lib\libpng.lib
        [GM dir]\BCBMagick\lib\libtiff.lib
        [GM dir]\BCBMagick\lib\libttf.lib
        [GM dir]\BCBMagick\lib\zlib.lib
    
(2) Add into [Projects->Options...->Directories/Conditionals->Include Path]
    the follow(ing) include path(s):

        [GM dir]
        [GM dir]\Magick++\lib				(only for Magick++)   


(3) Copy all following files into execution path of your application: 

        [GM dir]\Coders\modules.mgk
        [GM dir]\locale\C.mgk
        [GM dir]\locale\locale.mgk
        [GM dir]\Magick\colors.mgk
        [GM dir]\Magick\magick.mgk
        [GM dir]\VisualMagick\delegates.mgk
        [GM dir]\VisualMagick\log.mgk
        [GM dir]\VisualMagick\type.mgk
        [GM dir]\VisualMagick\type_ghostscript.mgk


    
USE OF DLLs IN YOUR PROJECTS
==============================        
(1) Add into Project Manager of your application the following Import
    Libraries:

        [GM dir]\BCBMagick\dll\GM_bzlib.lib
        [GM dir]\BCBMagick\dll\GM_lcms.lib
        [GM dir]\BCBMagick\dll\GM_jpeg.lib
        [GM dir]\BCBMagick\dll\GM_magick.lib
        [GM dir]\BCBMagick\dll\GM_Magickpp.lib		(only for Magick++)
        [GM dir]\BCBMagick\dll\GM_png.lib
        [GM dir]\BCBMagick\dll\GM_tiff.lib
        [GM dir]\BCBMagick\dll\GM_ttf.lib
        [GM dir]\BCBMagick\dll\GM_zlib.lib
        
    and copy then following DLLs into execution path of your application
    (test purposes) or into Windows System directory (standard):    

        [GM dir]\BCBMagick\dll\GM_bzlib.dll
        [GM dir]\BCBMagick\dll\GM_lcms.dll
        [GM dir]\BCBMagick\dll\GM_jpeg.dll
        [GM dir]\BCBMagick\dll\GM_magick.dll
        [GM dir]\BCBMagick\dll\GM_Magickpp.dll		(only for Magick++)
        [GM dir]\BCBMagick\dll\GM_png.dll
        [GM dir]\BCBMagick\dll\GM_tiff.dll
        [GM dir]\BCBMagick\dll\GM_ttf.dll
        [GM dir]\BCBMagick\dll\GM_zlib.dll

    
(2) Add into [Projects->Options...->Directories/Conditionals->Include Path]
    the follow(ing) include path(s):

        [GM dir]
        [GM dir]\Magick++\lib				(only for Magick++)   



CONFIGURATION FILES
=====================        

(1) Copy all following configuration files into execution path of your
    application: 

        [GM dir]\BCBMagick\mgk\c.mgk
        [GM dir]\BCBMagick\mgk\colors.mgk
        [GM dir]\BCBMagick\mgk\delegates.mgk
        [GM dir]\BCBMagick\mgk\locale.mgk
        [GM dir]\BCBMagick\mgk\log.mgk
        [GM dir]\BCBMagick\mgk\magick.mgk
        [GM dir]\BCBMagick\mgk\modules.mgk
        [GM dir]\BCBMagick\mgk\type.mgk
        [GM dir]\BCBMagick\mgk\type_ghostscript.mgk
   


All comments and/or suggestions are welcomed.

Good Luck !!!
(Please excuse me for bad english... I am italian!!!)

-------------------------------------------------------------
Mike Chiarappa
mikechiarappa@libero.it