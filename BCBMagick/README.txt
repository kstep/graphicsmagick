***********************************************************
**  GraphicsMagick library - v.1.1 - 24 Sept 2003        **
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
Actually is possible to obtain only a static version of library (no DLLs,
only LIB) with a small subset of the main "delegates" library available. 
These are:

    bzlib
    lcms
    jpeg
    libtiff
    FreeType library (ttf)
    zlib

...but the work is only at the beginning :-))
    
    
    
INSTRUCTIONS FOR COMPILATION
============================    
(1) Execute the batch file:

        [GM Dir]\BCBMagick\All\startup.bat
    
    This launch the right makefile (startup.mak) that perform the copy of 
    necessary modified files into the "standard" GraphicsMagick directory
    tree.        
    
(2) Launch BCB IDE and Open the Project Group [File->Open Project...]:

        [GM Dir]\BCBMagick\All\GM-1.1.bpg   
        
(3) Build libraries [Project->Build all projects]
    Will be segnaled some warnings: nothing of important. 
    This is all to do !!!
    


USE OF LIBRARY IN OUR PROJECTS
==============================        
(1) Add into Project Manager of your application the following libraries:

        [GM dir]\bzlib\libbz2.lib
        [GM dir]\coders\libCoders.lib
        [GM dir]\jpeg\jpeglib.lib
        [GM dir]\lcms\Lib\BC\lcms.lib
        [GM dir]\magick\libMagick.lib
        [GM dir]\magick++\lib\libMagickpp.lib		(only for Magick++)
        [GM dir]\tiff\libtiff\libtiff.lib
        [GM dir]\ttf\libttf.lib
        [GM dir]\zlib\contrib\BCB6\zlib.lib
    
(2) Add into [Projects->Options...->Directories/Conditionals->Include Path]
    the follow(ing) include path(s):

        [GM dir]
        [GM dir]\Magick++\lib				(only for Magick++)   


(3) Copy all following files into execution path of your application: 

        [GM dir]\Magick\color.mgk
        [GM dir]\Magick\log.mgk
        [GM dir]\Magick\magick.mgk

    

All comments and/orsuggestions are welcome.

Good Luck !!!
(Please excuse me for bad english... I am italian!!!)

-------------------------------------------------------------
Mike Chiarappa
mikechiarappa@libero.it