This file adapted from hp2xx_nt originally by Jim Shaw
===================================================

How to generate .EXE for HP2xx with visual C++
written for version 6.00 of visualc
Bengt-Arne Fjellner
Bengt-Arne.Fjellner@tt.luth.se

To generate this project from the distribution using Visual C++ 

1.  extract the distribution into a directory. (example: c:\data\hp2xx)

2.  start VisualC, and create a new Win32 console application, 
    in the directory you used in step1/sources. (example: c:\data\hp2xx\sources)
    after pressing next choose Empty Project

3.  In project/settings/c++/general/preprocessordefinitions add ,EMF,_NO_VCL,NORINT at the end of the line

4.  Add the Files necessary to compile the program (Project|Add to Project|files).
    This may be hp2xx version and installation dependent.  Here is a
    (perhaps partial) list:
	     bresnham.c
	     chardraw.c
	     clip.c
	     fillpoly.c
	     getopt.c
	     getopt1.c
	     hp2xx.c
	     hpgl.c
	     lindef.c
	     no_prev.c
	     pendef.c
	     picbuf.c
	     std_main.c
	     to_emf.c
	     to_eps.c
	     to_escp2.c
	     to_fig.c
	     to_ilbm.c
	     to_img.c
	     to_pac.c
	     to_pbm.c
	     to_pcl.c
	     to_pcx.c
	     to_pic.c
	     to_rgip.c
	     to_vec.c
	 
	 Skip these files they either need extra libraries or platform dependent files
	   lines.c	Outdated file ???
	   to_amiga.c	AMIGA previewer
	   to_atari.c	ATARI previewer
	   to_dj_gr.c	DOS full-screen (S)VGA previewer, based on DJ Delorie's gr lib
	   to_hgc.c	DOS Hercules previewer VERY outdated
	   to_os2.c	OS/2 full-screen previewer (only b/w); uses to_vga.c in DOS mode
	   to_pm.c	OS/2 PM previewer (unsupported -- stderr output is lost yet)
	   to_png.c	Output converter for PNG format (requires libpng and libz)
	   to_sunvw.c	SunView previewer (unsupported -- outdated)
	   to_tif.c	Output converter for TIFF format (requires libtiff)
	   to_uis.c	VAX-VMS UIS previewer
	   to_vga.c	DOS full-screen VGA previewer
	   to_x11.c	X11 previewer
	   png.c	More for png 

	   to_tif.c and to_png.c should be possible to add by acquiring libng,libz and libtiff
				 but i have not tried it.

5.  Build the project.  You will get many warnings, but should not
    get any errors.

6. Good luck
   You should now have a file hp2xx.exe that has three windows specific modes
   -m pre ( this is the default mode ) preview in windows dialog
   -m emf enhaced meta file
   -m emp  windows print
   i have not been able to test more than these modes and eps/pcl so no guarantees for the rest.


