$!
$! Make ImageMagick X image utilities for VMS.
$!
$
$if (f$trnlnm("X11") .eqs. "") then define/nolog X11 decw$include:
$compile_options="/nodebug/optimize"
$if (f$trnlnm("sys$system:decc$compiler.exe") .nes. "") 
$then     ! VAX with DEC C
$  compile_options="/decc/standard=vaxc/nodebug/optimize"
$else     ! VAX with VAX C
$define/nolog lnk$library sys$library:vaxcrtl
$define/nolog sys sys$share
$endif
$if (f$getsyi("HW_MODEL") .gt. 1023)
$then     ! Alpha with DEC C
$  define/nolog sys decc$library_include
$  compile_options="/standard=vaxc/nodebug/optimize"
$endif
$
$write sys$output "Making Magick..."
$call Make image
$call Make effects
$call Make shear
$call Make segment
$call Make quantize
$call Make colors
$call Make gems
$call Make signature
$call Make decode ("HasJPEG","HasLZW","HasPNG","HasTIFF","HasTTF","HasZLIB")
$call Make encode ("HasJPEG","HasLZW","HasPNG","HasTIFF","HasTTF","HasZLIB")
$call Make compress ("HasJPEG","HasLZW","HasPNG","HasTIFF","HasTTF","HasZLIB")
$call Make utility
$call Make monitor
$call Make error
$call Make delegates
$call Make magick
$call Make display
$call Make animate
$call Make widget
$call Make X
$call Make PreRvIcccm
$call Make memory
$call Make vms
$library/create libmagick.olb image.obj,effects.obj,shear.obj,segment.obj, -
  quantize.obj,colors.obj,gems.obj,signature.obj,decode.obj,encode.obj, -
  compress.obj,utility.obj,monitor.obj,error.obj,delegates.obj,magick.obj, -
  display.obj,animate.obj,widget.obj,X.obj,PreRvIcccm.obj,memory.obj,vms.obj
$exit
$
$Make: subroutine
$!
$! Primitive MMS hack for DCL.
$!
$if (p1 .eqs. "") then exit
$source_file=f$search(f$parse(p1,".c"))
$if (source_file .nes. "")
$then
$  object_file=f$parse(source_file,,,"name")+".obj"
$  object_file=f$search( object_file )
$  if (object_file .nes. "")
$  then
$    object_time=f$file_attribute(object_file,"cdt")
$    source_time=f$file_attribute(source_file,"cdt")
$    if (f$cvtime(object_time) .lts. f$cvtime(source_time)) then -
$      object_file=""
$  endif
$  if (object_file .eqs. "")
$  then
$    write sys$output "Compiling ",p1
$    if (p2 .eqs. "")
$    then
$      cc'compile_options'/include_directory=([-],[-.jpeg],[-.png], -
         [-.tiff],[-.ttf],[-.zlib]) 'source_file'
$    else
$      cc'compile_options'/include_directory=([-],[-.jpeg],[-.png], -
         [-.tiff],[-.ttf],[-.zlib])/define='p2' 'source_file'  
$    endif
$  endif
$endif
$exit
$endsubroutine
