$!
$! Make ImageMagick image utilities for VMS.
$!
$ copy config.h_vms magick_config.h
$
$if (f$trnlnm("X11") .eqs. "") then define/nolog X11 decw$include:
$compile_options="/nodebug/optimize"
$if (f$search("sys$system:decc$compiler.exe") .nes. "") 
$then     ! VAX with DEC C
$  compile_options="/decc/nodebug/optimize/warning=(disable=rightshiftovr)"
$else     ! VAX with VAX C
$define/nolog lnk$library sys$library:vaxcrtl
$define/nolog sys sys$share
$endif
$if (f$getsyi("HW_MODEL") .gt. 1023)
$then     ! Alpha with DEC C
$  define/nolog sys decc$library_include
$  compile_options="/nodebug/optimize/prefix=all/warning=(disable=rightshiftovr)/name=(as_is,short)
$endif
$
$write sys$output "Making Magick..."
$call Make PreRvIcccm.c
$call Make animate.c
$call Make annotate.c
$call Make attribute.c
$call Make blob.c
$call Make cache.c
$call Make cache_view.c
$call Make color.c
$call Make compress.c
$call Make constitute.c
$call Make decorate.c
$call Make delegate.c
$call Make display.c
$call Make draw.c
$call Make effect.c
$call Make enhance.c
$call Make error.c
$call Make gem.c
$call Make image.c
$call Make semaphore.c
$call Make magic.c
$call Make magick.c
$call Make memory.c
$call Make module.c
$call Make monitor.c
$call Make montage.c
$call Make quantize.c
$call Make registry.c
$call Make segment.c
$call Make shear.c
$call Make signature.c
$call Make static.c
$call Make stream.c
$call Make timer.c
$call Make transform.c
$call Make type.c
$call Make utility.c
$call Make vms.c
$call Make widget.c
$call Make xwindow.c
$call Make zoom.c
$library/create libMagick.olb memory,blob,cache,error,monitor,constitute, -
  image,color,quantize,segment,zoom,transform,shear,enhance,effect,decorate, -
  annotate,type,draw,montage,semaphore,attribute,signature,compress,gem, -
  registry,magic,magick,delegate,module,static,timer,utility,cache_view,stream, -
  animate,display,widget,xwindow,PreRvIcccm,vms
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
$    cc'compile_options'/include_directory=([-],[-.jpeg],[-.png], -
       [-.tiff],[-.ttf],[-.zlib]) 'source_file'  
$  endif
$endif
$exit
$endsubroutine
