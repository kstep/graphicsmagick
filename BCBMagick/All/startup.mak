# 
# ***********************************************************
# **  GraphicsMagick library - v.1.1 - 10 July 2003        **
# **                                                       **
# **  Porting for compilation with Borland C++Builder 6.0  **
# **-------------------------------------------------------**
# **                                                       **
# **  >>> startup.mak                                      ** 
# **                                                       **
# **  Makefile for copy modified files into right dirs     **
# **                                                       **
# **-------------------------------------------------------**
# **  by Mike Chiarappa                                    **
# **     mikechiarappa@libero.it                           **
# ***********************************************************
#

# Main dirs
GMDIR=d:\GraphicsMagick-1.1
GMBCB=d:\GraphicsMagick-1.1\BCBMagick


copy-all: 


##########
# bzlib
##########
  copy $(GMBCB)\bzlib\libbz2.bpr $(GMDIR)\bzlib
  copy $(GMBCB)\bzlib\libbz2.bpf $(GMDIR)\bzlib
  
##########
# coders 
##########
  copy $(GMBCB)\coders\libCoders.bpr $(GMDIR)\coders
  copy $(GMBCB)\coders\libCoders.bpf $(GMDIR)\coders
  copy $(GMBCB)\coders\svg.c $(GMDIR)\coders

##########
# jpeg
##########
  copy $(GMBCB)\jpeg\jpeg.bpg $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\cjpeg.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\djpeg.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jpeglib.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jpeglib.bpf $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jpegtran.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\rdjpgcom.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\wrjpgcom.bpr $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jconfig.h $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jmorecfg.h $(GMDIR)\jpeg
  copy $(GMBCB)\jpeg\jpeglib.h $(GMDIR)\jpeg

##########
# lcms
##########
  @if not exist $(GMDIR)\lcms\Projects\BCB6 mkdir $(GMDIR)\lcms\Projects\BCB6
    
  copy $(GMBCB)\lcms\Projects\BCB6\lcms.bpg $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\lcms.bpr $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\lcms.cpp $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\lcmsdll.bpr $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\lcmsdll.cpp $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\lcmsdll.res $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\lcmsbcb4.def $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\testcms.bpr $(GMDIR)\lcms\Projects\BCB6
  copy $(GMBCB)\lcms\Projects\BCB6\testcms.cpp $(GMDIR)\lcms\Projects\BCB6


##########
# magick
##########
  copy $(GMBCB)\magick\libMagick.bpr $(GMDIR)\magick
  copy $(GMBCB)\magick\libMagick.bpf $(GMDIR)\magick
  copy $(GMBCB)\magick\magick_types.h.in $(GMDIR)\magick\magick_types.h
  copy $(GMBCB)\magick\magick_config.h.in $(GMDIR)\magick\magick_config.h
  copy $(GMBCB)\magick\cache.c $(GMDIR)\magick

##########
# magick++
##########
  xcopy /Y $(GMBCB)\Magick++\lib\libMagickpp.bpr $(GMDIR)\Magick++\lib
  xcopy /Y $(GMBCB)\Magick++\lib\libMagick.bpf $(GMDIR)\Magick++\lib
  
##########
# tiff
##########
  copy $(GMBCB)\tiff\libtiff.bpg $(GMDIR)\tiff
  copy $(GMBCB)\tiff\libtiff\libtiff.bpr $(GMDIR)\tiff\libtiff
  copy $(GMBCB)\tiff\libtiff\libtiff.bpf $(GMDIR)\tiff\libtiff
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
  
##########
# ttf
##########
  copy $(GMBCB)\ttf\libttf.bpr $(GMDIR)\ttf
  copy $(GMBCB)\ttf\libttf.bpf $(GMDIR)\ttf
  
##########
# zlib
##########
  @if not exist $(GMDIR)\zlib\contrib\BCB6 mkdir $(GMDIR)\zlib\contrib\BCB6

  copy $(GMBCB)\zlib\contrib\BCB6\zlib.bpg $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\zlib.bpr $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\zlib.cpp $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\d_zlib.bpr $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\d_zlib.cpp $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\zlib32.bpr $(GMDIR)\zlib\contrib\BCB6
  copy $(GMBCB)\zlib\contrib\BCB6\zlib32.cpp $(GMDIR)\zlib\contrib\BCB6

#############
# utilities
#############
# copy $(GMBCB)\utilities\GMUtilities.bpg $(GMDIR)\utilities
# copy $(GMBCB)\utilities\animate.bpr $(GMDIR)\utilities
# copy $(GMBCB)\utilities\composite.bpr $(GMDIR)\utilities
# copy $(GMBCB)\utilities\conjure.bpr $(GMDIR)\utilities
# copy $(GMBCB)\utilities\convert.bpr $(GMDIR)\utilities
# copy $(GMBCB)\utilities\display.bpr $(GMDIR)\utilities
# copy $(GMBCB)\utilities\identify.bpr $(GMDIR)\utilities
# copy $(GMBCB)\utilities\import.bpr $(GMDIR)\utilities
# copy $(GMBCB)\utilities\mogrify.bpr $(GMDIR)\utilities
# copy $(GMBCB)\utilities\montage.bpr $(GMDIR)\utilities

