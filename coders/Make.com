$!
$! Make ImageMagick image coders for VMS.
$!
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
$  compile_options="/nodebug/optimize/prefix=all/warning=(disable=rightshiftovr)"
$endif
$
$
$write sys$output "Making Coders..."
$call Make 8bim.c
$call Make art.c
$call Make avi.c
$call Make avs.c
$call Make bmp.c
$call Make cmyk.c
$call Make cut.c
$call Make dcm.c
$call Make dps.c
$call Make dpx.c
$call Make ept.c
$call Make fax.c
$call Make fits.c
$call Make fpx.c
$call Make gif.c
$call Make gradient.c
$call Make gray.c
$call Make hdf.c
$call Make histogram.c
$call Make html.c
$call Make icm.c
$call Make icon.c
$call Make iptc.c
$call Make jbig.c
$call Make jp2.c
$call Make jpeg.c
$call Make label.c
$call Make logo.c
$call Make map.c
$call Make mat.c
$call Make matte.c
$call Make miff.c
$call Make mono.c
$call Make mpc.c
$call Make mpeg.c
$call Make mtv.c
$call Make mvg.c
$call Make null.c
$call Make pcd.c
$call Make pcl.c
$call Make pcx.c
$call Make pdb.c
$call Make pdf.c
$call Make pict.c
$call Make pix.c
$call Make plasma.c
$call Make png.c
$call Make pnm.c
$call Make preview.c
$call Make ps.c
$call Make ps2.c
$call Make ps3.c
$call Make psd.c
$call Make pwp.c
$call Make rgb.c
$call Make rla.c
$call Make rle.c
$call Make sct.c
$call Make sfw.c
$call Make sgi.c
$call Make stegano.c
$call Make sun.c
$call Make svg.c
$call Make tga.c
$call Make tiff.c
$call Make tile.c
$call Make tim.c
$call Make ttf.c
$call Make txt.c
$call Make uil.c
$call Make url.c
$call Make uyvy.c
$call Make vicar.c
$call Make vid.c
$call Make viff.c
$call Make wbmp.c
$call Make wmf.c
$call Make wpg.c
$call Make x.c
$call Make xbm.c
$call Make xc.c
$call Make xpm.c
$call Make xwd.c
$call Make yuv.c
$library/create libCoders.olb 8bim,art,avi,avs,bmp,cmyk,cut,dcm,dps,dpx,ept, -
 fax,fits,fpx,gif,gradient,gray,hdf,histogram,html,icm,icon,iptc,jbig,jp2, -
  jpeg,label,logo,map,mat,matte,miff,mono,mpc,mpc,mtv,mvg,null,pcd,pcl,pcx, -
  pdb,pdf,pict,pix,plasma,png,pnm,preview,ps,ps2,ps3,psd,pwp,rgb, -
  rla,rle,sct,sfw,sgi,stegano,sun,svg,tga,tiff,tile,tim,ttf, -
  txt,uil,url,uyvy,vicar,vid,viff,wbmp,wmf,wpg,x,xbm,xc,xpm,xwd,yuv, -
  [-.magick]compress,[-.magick]prervicccm
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
$    cc'compile_options'/include_directory=([-],[-.magick],[-.jpeg],[-.png], -
       [-.tiff],[-.ttf],[-.zlib]) 'source_file'  
$  endif
$endif
$exit
$endsubroutine
