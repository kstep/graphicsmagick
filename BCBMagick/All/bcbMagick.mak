# 
# ***************************************************************
# **  GraphicsMagick library - v.1.1 - 03 November 2003        **
# **                                                           **
# **  Porting for compilation with Borland C++Builder 6.0      **
# **-----------------------------------------------------------**
# **                                                           **
# **  >>> BCBMagick.mak                                        ** 
# **                                                           **
# **  Makefile for:                                            **
# **    [] copy modified files into right directories          **
# **    [] collect libraries and DLLs for distribution         **
# **                                                           **
# **-----------------------------------------------------------**
# **  by Mike Chiarappa                                        **
# **     mikechiarappa@libero.it                               **
# ***************************************************************
#

# Main dirs
GMDIR=d:\GraphicsMagick-1.1
GMBCB=$(GMDIR)\BCBMagick


##############################################
# Copy modified files into right directories
##############################################
copy-all: 


# bzlib		<<<<<<<<<<<<<<<<<<<<
  copy $(GMBCB)\bzlib\libbz2.bpr $(GMDIR)\bzlib
  copy $(GMBCB)\bzlib\libbz2.bpf $(GMDIR)\bzlib
  copy $(GMBCB)\bzlib\GM_bzlib.bpr $(GMDIR)\bzlib
  copy $(GMBCB)\bzlib\GM_bzlib.c $(GMDIR)\bzlib
  
# coders	<<<<<<<<<<<<<<<<<<<< 
  copy $(GMBCB)\coders\libCoders.bpr $(GMDIR)\coders
  copy $(GMBCB)\coders\libCoders.bpf $(GMDIR)\coders

# jpeg		<<<<<<<<<<<<<<<<<<<<
  copy $(GMBCB)\jpeg\jpeg.bpg $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\cjpeg.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\djpeg.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jpeglib.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jpeglib.bpf $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jpegtran.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\rdjpgcom.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\wrjpgcom.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\GM_jpeg.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\GM_jpeg.c $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jconfig.h $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jmorecfg.h $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jpeglib.h $(GMDIR)\jpeg

# lcms		<<<<<<<<<<<<<<<<<<<<
  @if not exist $(GMDIR)\lcms\Projects\BCB6 mkdir $(GMDIR)\lcms\Projects\BCB6
    
  copy $(GMBCB)\lcms\Projects\BCB6\lcms.bpg $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\lcms.bpr $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\lcms.cpp $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\GM_lcms.bpr $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\GM_lcms.cpp $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\testcms.bpr $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\testcms.cpp $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\include\lcms.h $(GMDIR)\lcms\include\lcms.h


# magick	<<<<<<<<<<<<<<<<<<<<
  copy $(GMBCB)\magick\libMagick.bpr $(GMDIR)\magick
  copy $(GMBCB)\magick\libMagick.bpf $(GMDIR)\magick
  copy $(GMBCB)\magick\GM_magick.bpr $(GMDIR)\magick
  copy $(GMBCB)\magick\GM_magick.c $(GMDIR)\magick
  copy $(GMBCB)\magick\magick_types.h.in $(GMDIR)\magick\magick_types.h
  copy $(GMBCB)\magick\magick_config.h.in $(GMDIR)\magick\magick_config.h

# magick++	<<<<<<<<<<<<<<<<<<<<
  xcopy /Y $(GMBCB)\Magickpp\lib\libMagickpp.bpr $(GMDIR)\Magick++\lib
  xcopy /Y $(GMBCB)\Magickpp\lib\libMagick.bpf $(GMDIR)\Magick++\lib
  xcopy /Y $(GMBCB)\Magickpp\lib\GM_Magickpp.bpr $(GMDIR)\Magick++\lib
  xcopy /Y $(GMBCB)\Magickpp\lib\GM_Magickpp.cpp $(GMDIR)\Magick++\lib
  xcopy /Y $(GMBCB)\Magickpp\lib\Magickpp\Include.h $(GMDIR)\Magick++\lib\Magick++
  
# tiff		<<<<<<<<<<<<<<<<<<<<
  copy $(GMBCB)\tiff\libtiff.bpg $(GMDIR)\tiff
  copy $(GMBCB)\tiff\libtiff\libtiff.bpr $(GMDIR)\tiff\libtiff
  copy $(GMBCB)\tiff\libtiff\libtiff.bpf $(GMDIR)\tiff\libtiff
  copy $(GMBCB)\tiff\libtiff\GM_tiff.bpr $(GMDIR)\tiff\libtiff
  copy $(GMBCB)\tiff\libtiff\GM_tiff.c $(GMDIR)\tiff\libtiff
  copy $(GMBCB)\tiff\libtiff\tiffio.h $(GMDIR)\tiff\libtiff\tiffio.h
  copy $(GMBCB)\tiff\libtiff\tiffconf.h.in $(GMDIR)\tiff\libtiff\tiffconf.h
  
  copy $(GMBCB)\tiff\tools\fax2ps.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\fax2tiff.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\gif2tiff.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\pal2rgb.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\ppm2tif.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\ras2tiff.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\raw2tiff.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\rgb2ycbcr.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\thumbnail.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiff2bw.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiff2ps.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiff2rgba.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiffcmp.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiffcp.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiffdither.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiffdump.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiffinfo.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiffmedian.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiffset.bpr $(GMDIR)\tiff\tools
  copy $(GMBCB)\tiff\tools\tiffsplit.bpr $(GMDIR)\tiff\tools
  
# ttf		<<<<<<<<<<<<<<<<<<<<
  copy $(GMBCB)\ttf\libttf.bpr $(GMDIR)\ttf
  copy $(GMBCB)\ttf\libttf.bpf $(GMDIR)\ttf
  copy $(GMBCB)\ttf\GM_ttf.bpr $(GMDIR)\ttf
  copy $(GMBCB)\ttf\GM_ttf.c $(GMDIR)\ttf
  copy $(GMBCB)\ttf\include\freetype\config\ftoption.h $(GMDIR)\ttf\include\freetype\config
  
# zlib		<<<<<<<<<<<<<<<<<<<<

  @if not exist $(GMDIR)\zlib\contrib\BCB6 mkdir $(GMDIR)\zlib\contrib\BCB6

  copy $(GMBCB)\zlib\contrib\BCB6\zlib.bpg $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\zlib.bpr $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\zlib.cpp $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\d_zlib.bpr $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\d_zlib.cpp $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\GM_zlib.bpr $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\GM_zlib.cpp $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\zconf.h $(GMDIR)\zlib\zconf.h
  

# utilities	<<<<<<<<<<<<<<<<<<<<
  copy $(GMBCB)\utilities\gm_lib.bpr $(GMDIR)\utilities
  copy $(GMBCB)\utilities\gm_dll.bpr $(GMDIR)\utilities



##########################################
# Put all builded libraries into LIB dir
##########################################

get-libs:
  @if not exist $(GMBCB)\lib mkdir $(GMBCB)\lib
  
  copy $(GMDIR)\bzlib\libbz2.lib $(GMBCB)\lib
  copy $(GMDIR)\coders\libCoders.lib $(GMBCB)\lib
  copy $(GMDIR)\lcms\Projects\BCB6\lcms.lib $(GMBCB)\lib
  copy $(GMDIR)\jpeg\jpeglib.lib $(GMBCB)\lib
  copy $(GMDIR)\magick\libMagick.lib $(GMBCB)\lib
  xcopy /Y $(GMDIR)\Magick++\lib\libMagickpp.lib $(GMBCB)\lib
  copy $(GMDIR)\tiff\libtiff\libtiff.lib $(GMBCB)\lib
  copy $(GMDIR)\ttf\libttf.lib $(GMBCB)\lib
  copy $(GMDIR)\zlib\contrib\BCB6\zlib.lib $(GMBCB)\lib


  
#########################################################
# Put all builded DLLs and respectives Import Libraries
# into DLL dir
#########################################################
  
get-dlls:  
  @if not exist $(GMBCB)\dll mkdir $(GMBCB)\dll
  
  copy $(GMDIR)\bzlib\GM_bzlib.lib $(GMBCB)\dll
  copy $(GMDIR)\bzlib\GM_bzlib.dll $(GMBCB)\dll
  copy $(GMDIR)\lcms\Projects\BCB6\GM_lcms.lib $(GMBCB)\dll
  copy $(GMDIR)\lcms\Projects\BCB6\GM_lcms.dll $(GMBCB)\dll
  copy $(GMDIR)\jpeg\GM_jpeg.lib $(GMBCB)\dll
  copy $(GMDIR)\jpeg\GM_jpeg.dll $(GMBCB)\dll
  copy $(GMDIR)\magick\GM_magick.lib $(GMBCB)\dll
  copy $(GMDIR)\magick\GM_magick.dll $(GMBCB)\dll
  xcopy /Y $(GMDIR)\Magick++\lib\GM_Magickpp.lib $(GMBCB)\dll
  xcopy /Y $(GMDIR)\Magick++\lib\GM_Magickpp.dll $(GMBCB)\dll
  copy $(GMDIR)\tiff\libtiff\GM_tiff.lib $(GMBCB)\dll
  copy $(GMDIR)\tiff\libtiff\GM_tiff.dll $(GMBCB)\dll
  copy $(GMDIR)\ttf\GM_ttf.lib $(GMBCB)\dll
  copy $(GMDIR)\ttf\GM_ttf.dll $(GMBCB)\dll
  copy $(GMDIR)\zlib\contrib\BCB6\GM_zlib.lib $(GMBCB)\dll
  copy $(GMDIR)\zlib\contrib\BCB6\GM_zlib.dll $(GMBCB)\dll



############################################
# Put all Configuration files into MGK dir
############################################

get-mgks:
  @if not exist $(GMBCB)\mgk mkdir $(GMBCB)\mgk
  
  copy $(GMDIR)\config\colors.mgk $(GMBCB)\mgk
  copy $(GMDIR)\config\log.mgk $(GMBCB)\mgk
  copy $(GMDIR)\config\magic.mgk $(GMBCB)\mgk
  copy $(GMDIR)\locale\C.mgk $(GMBCB)\mgk
  copy $(GMDIR)\locale\locale.mgk $(GMBCB)\mgk
  copy $(GMDIR)\VisualMagick\bin\delegates.mgk $(GMBCB)\mgk
  copy $(GMDIR)\VisualMagick\bin\modules.mgk $(GMBCB)\mgk
  copy $(GMDIR)\VisualMagick\bin\type.mgk $(GMBCB)\mgk
  copy $(GMDIR)\VisualMagick\bin\type-ghostscript.mgk $(GMBCB)\mgk
