$!
$! Make ImageMagick X image utilities for VMS.
$!
$
$if (f$trnlnm("X11") .eqs. "") then define/nolog X11 decw$include:
$compile_options="/nodebug/optimize"
$if (f$search("sys$system:decc$compiler.exe") .nes. "") 
$then     ! VAX with DEC C
$  compile_options="/decc/nodebug/optimize"
$else     ! VAX with VAX C
$define/nolog lnk$library sys$library:vaxcrtl
$define/nolog sys sys$share
$endif
$if (f$getsyi("HW_MODEL") .gt. 1023)
$then     ! Alpha with DEC C
$  define/nolog sys decc$library_include
$  compile_options="/nodebug/optimize/prefix=all"
$endif
$
$write sys$output "Making Magick..."
$call Make image.c
$call Make effects.c
$call Make shear.c
$call Make segment.c
$call Make quantize.c
$call Make colors.c
$call Make gems.c
$call Make signature.c
$call Make decode.c ("HasJPEG","HasLZW","HasPNG","HasTIFF","HasTTF","HasZLIB")
$call Make encode.c ("HasJPEG","HasLZW","HasPNG","HasTIFF","HasTTF","HasZLIB")
$call Make compress.c ("HasJPEG","HasLZW","HasPNG","HasTIFF","HasTTF","HasZLIB")
$call Make utility.c
$call Make monitor.c
$call Make error.c
$call Make delegates.c
$call Make magick.c ("HasJPEG","HasLZW","HasPNG","HasTIFF","HasTTF","HasZLIB")
$call Make display.c
$call Make animate.c
$call Make widget.c
$call Make X.c
$call Make PreRvIcccm.c
$call Make memory.c
$call Make vms.c
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
