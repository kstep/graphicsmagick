@echo off
set exedir=..\bin\
set srcdir=..\..\tests\
%exedir%drawtest drawtest_out.miff
%exedir%drawtest drawtest_out.mvg
%exedir%drawtest drawtest_out.svg
%exedir%rwfile.exe %srcdir%input_bilevel.miff AVS 
%exedir%rwfile.exe %srcdir%input_gray.miff AVS 
%exedir%rwfile.exe %srcdir%input_256c.miff AVS 
%exedir%rwfile.exe %srcdir%input_truecolor.miff AVS 
%exedir%rwfile.exe %srcdir%input_bilevel.miff BMP 
%exedir%rwfile.exe %srcdir%input_gray.miff BMP 
%exedir%rwfile.exe %srcdir%input_256c.miff BMP 
%exedir%rwfile.exe %srcdir%input_truecolor.miff BMP 
%exedir%rwfile.exe %srcdir%input_bilevel.miff DIB 
%exedir%rwfile.exe %srcdir%input_gray.miff DIB 
%exedir%rwfile.exe %srcdir%input_256c.miff DIB 
%exedir%rwfile.exe %srcdir%input_truecolor.miff DIB 
%exedir%rwfile.exe %srcdir%input_bilevel.miff DPX 
%exedir%rwfile.exe %srcdir%input_gray.miff DPX 
%exedir%rwfile.exe %srcdir%input_256c.miff DPX 
%exedir%rwfile.exe %srcdir%input_truecolor.miff DPX 
%exedir%rwfile.exe %srcdir%input_bilevel.miff EPDF 
%exedir%rwfile.exe %srcdir%input_gray.miff EPDF 
%exedir%rwfile.exe %srcdir%input_256c.miff EPDF 
%exedir%rwfile.exe %srcdir%input_truecolor.miff EPDF 
%exedir%rwfile.exe %srcdir%input_bilevel.miff EPSF 
%exedir%rwfile.exe %srcdir%input_gray.miff EPSF 
%exedir%rwfile.exe %srcdir%input_256c.miff EPSF 
%exedir%rwfile.exe %srcdir%input_truecolor.miff EPSF 
%exedir%rwfile.exe %srcdir%input_bilevel.miff EPSI 
%exedir%rwfile.exe %srcdir%input_gray.miff EPSI 
%exedir%rwfile.exe %srcdir%input_256c.miff EPSI 
%exedir%rwfile.exe %srcdir%input_truecolor.miff EPSI 
%exedir%rwfile.exe %srcdir%input_bilevel.miff EPI 
%exedir%rwfile.exe %srcdir%input_gray.miff EPI 
%exedir%rwfile.exe %srcdir%input_256c.miff EPI 
%exedir%rwfile.exe %srcdir%input_truecolor.miff EPI 
%exedir%rwfile.exe %srcdir%input_bilevel.miff EPS 
%exedir%rwfile.exe %srcdir%input_gray.miff EPS 
%exedir%rwfile.exe %srcdir%input_256c.miff EPS 
%exedir%rwfile.exe %srcdir%input_truecolor.miff EPS 
%exedir%rwfile.exe %srcdir%input_bilevel.miff EPT 
%exedir%rwfile.exe %srcdir%input_gray.miff EPT 
%exedir%rwfile.exe %srcdir%input_256c.miff EPT 
%exedir%rwfile.exe %srcdir%input_truecolor.miff EPT 
%exedir%rwfile.exe %srcdir%input_bilevel.miff FAX 
%exedir%rwfile.exe %srcdir%input_gray.miff FAX 
%exedir%rwfile.exe %srcdir%input_256c.miff FAX 
%exedir%rwfile.exe %srcdir%input_truecolor.miff FAX 
%exedir%rwfile.exe %srcdir%input_bilevel.miff FITS 
%exedir%rwfile.exe %srcdir%input_gray.miff FITS 
%exedir%rwfile.exe %srcdir%input_256c.miff FITS 
%exedir%rwfile.exe %srcdir%input_truecolor.miff FITS 
%exedir%rwfile.exe %srcdir%input_bilevel.miff FPX 
%exedir%rwfile.exe %srcdir%input_gray.miff FPX 
%exedir%rwfile.exe %srcdir%input_256c.miff FPX 
%exedir%rwfile.exe %srcdir%input_truecolor.miff FPX 
%exedir%rwfile.exe %srcdir%input_bilevel.miff GIF 
%exedir%rwfile.exe %srcdir%input_gray.miff GIF 
%exedir%rwfile.exe %srcdir%input_256c.miff GIF 
%exedir%rwfile.exe %srcdir%input_truecolor.miff GIF 
%exedir%rwfile.exe %srcdir%input_bilevel.miff GRAY 
%exedir%rwfile.exe %srcdir%input_gray.miff GRAY 
%exedir%rwfile.exe %srcdir%input_256c.miff GRAY 
%exedir%rwfile.exe %srcdir%input_truecolor.miff GRAY 
%exedir%rwfile.exe %srcdir%input_bilevel.miff ICB 
%exedir%rwfile.exe %srcdir%input_gray.miff ICB 
%exedir%rwfile.exe %srcdir%input_256c.miff ICB 
%exedir%rwfile.exe %srcdir%input_truecolor.miff ICB 
%exedir%rwfile.exe %srcdir%input_bilevel.miff JBIG 
%exedir%rwfile.exe %srcdir%input_gray.miff JBIG 
%exedir%rwfile.exe %srcdir%input_256c.miff JBIG 
%exedir%rwfile.exe %srcdir%input_truecolor.miff JBIG 
%exedir%rwfile.exe %srcdir%input_bilevel.miff JPEG 
%exedir%rwfile.exe %srcdir%input_gray.miff JPEG 
%exedir%rwfile.exe %srcdir%input_256c.miff JPEG 
%exedir%rwfile.exe %srcdir%input_truecolor.miff JPEG 
%exedir%rwfile.exe %srcdir%input_bilevel.miff JP2 
%exedir%rwfile.exe %srcdir%input_gray.miff JP2 
%exedir%rwfile.exe %srcdir%input_256c.miff JP2 
%exedir%rwfile.exe %srcdir%input_truecolor.miff JP2 
%exedir%rwfile.exe %srcdir%input_bilevel.miff MIFF 
%exedir%rwfile.exe %srcdir%input_gray.miff MIFF 
%exedir%rwfile.exe %srcdir%input_256c.miff MIFF 
%exedir%rwfile.exe %srcdir%input_truecolor.miff MIFF 
%exedir%rwfile.exe %srcdir%input_bilevel.miff MNG 
%exedir%rwfile.exe %srcdir%input_gray.miff MNG 
%exedir%rwfile.exe %srcdir%input_256c.miff MNG 
%exedir%rwfile.exe %srcdir%input_truecolor.miff MNG 
%exedir%rwfile.exe %srcdir%input_bilevel.miff MTV 
%exedir%rwfile.exe %srcdir%input_gray.miff MTV 
%exedir%rwfile.exe %srcdir%input_256c.miff MTV 
%exedir%rwfile.exe %srcdir%input_truecolor.miff MTV 
%exedir%rwfile.exe %srcdir%input_bilevel.miff P7 
%exedir%rwfile.exe %srcdir%input_gray.miff P7 
%exedir%rwfile.exe %srcdir%input_256c.miff P7 
%exedir%rwfile.exe %srcdir%input_truecolor.miff P7 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PBM 
%exedir%rwfile.exe %srcdir%input_gray.miff PBM 
%exedir%rwfile.exe %srcdir%input_256c.miff PBM 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PBM 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PCD 
%exedir%rwfile.exe %srcdir%input_gray.miff PCD 
%exedir%rwfile.exe %srcdir%input_256c.miff PCD 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PCD 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PCDS 
%exedir%rwfile.exe %srcdir%input_gray.miff PCDS 
%exedir%rwfile.exe %srcdir%input_256c.miff PCDS 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PCDS 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PCX 
%exedir%rwfile.exe %srcdir%input_gray.miff PCX 
%exedir%rwfile.exe %srcdir%input_256c.miff PCX 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PCX 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PGM 
%exedir%rwfile.exe %srcdir%input_gray.miff PGM 
%exedir%rwfile.exe %srcdir%input_256c.miff PGM 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PGM 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PICON 
%exedir%rwfile.exe %srcdir%input_gray.miff PICON 
%exedir%rwfile.exe %srcdir%input_256c.miff PICON 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PICON 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PICT 
%exedir%rwfile.exe %srcdir%input_gray.miff PICT 
%exedir%rwfile.exe %srcdir%input_256c.miff PICT 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PICT 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PNG 
%exedir%rwfile.exe %srcdir%input_gray.miff PNG 
%exedir%rwfile.exe %srcdir%input_256c.miff PNG 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PNG 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PNM 
%exedir%rwfile.exe %srcdir%input_gray.miff PNM 
%exedir%rwfile.exe %srcdir%input_256c.miff PNM 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PNM 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PPM 
%exedir%rwfile.exe %srcdir%input_gray.miff PPM 
%exedir%rwfile.exe %srcdir%input_256c.miff PPM 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PPM 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PS 
%exedir%rwfile.exe %srcdir%input_gray.miff PS 
%exedir%rwfile.exe %srcdir%input_256c.miff PS 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PS 
%exedir%rwfile.exe %srcdir%input_bilevel.miff PSD 
%exedir%rwfile.exe %srcdir%input_gray.miff PSD 
%exedir%rwfile.exe %srcdir%input_256c.miff PSD 
%exedir%rwfile.exe %srcdir%input_truecolor.miff PSD 
%exedir%rwfile.exe %srcdir%input_bilevel.miff RAS 
%exedir%rwfile.exe %srcdir%input_gray.miff RAS 
%exedir%rwfile.exe %srcdir%input_256c.miff RAS 
%exedir%rwfile.exe %srcdir%input_truecolor.miff RAS 
%exedir%rwfile.exe %srcdir%input_bilevel.miff SGI 
%exedir%rwfile.exe %srcdir%input_gray.miff SGI 
%exedir%rwfile.exe %srcdir%input_256c.miff SGI 
%exedir%rwfile.exe %srcdir%input_truecolor.miff SGI 
%exedir%rwfile.exe %srcdir%input_bilevel.miff SUN 
%exedir%rwfile.exe %srcdir%input_gray.miff SUN 
%exedir%rwfile.exe %srcdir%input_256c.miff SUN 
%exedir%rwfile.exe %srcdir%input_truecolor.miff SUN 
%exedir%rwfile.exe %srcdir%input_bilevel.miff TGA 
%exedir%rwfile.exe %srcdir%input_gray.miff TGA 
%exedir%rwfile.exe %srcdir%input_256c.miff TGA 
%exedir%rwfile.exe %srcdir%input_truecolor.miff TGA 
%exedir%rwfile.exe %srcdir%input_bilevel.miff TIFF 
%exedir%rwfile.exe %srcdir%input_gray.miff TIFF 
%exedir%rwfile.exe %srcdir%input_256c.miff TIFF 
%exedir%rwfile.exe %srcdir%input_truecolor.miff TIFF 
%exedir%rwfile.exe %srcdir%input_bilevel.miff VDA 
%exedir%rwfile.exe %srcdir%input_gray.miff VDA 
%exedir%rwfile.exe %srcdir%input_256c.miff VDA 
%exedir%rwfile.exe %srcdir%input_truecolor.miff VDA 
%exedir%rwfile.exe %srcdir%input_bilevel.miff VICAR 
%exedir%rwfile.exe %srcdir%input_gray.miff VICAR 
%exedir%rwfile.exe %srcdir%input_256c.miff VICAR 
%exedir%rwfile.exe %srcdir%input_truecolor.miff VICAR 
%exedir%rwfile.exe %srcdir%input_bilevel.miff VIFF 
%exedir%rwfile.exe %srcdir%input_gray.miff VIFF 
%exedir%rwfile.exe %srcdir%input_256c.miff VIFF 
%exedir%rwfile.exe %srcdir%input_truecolor.miff VIFF 
%exedir%rwfile.exe %srcdir%input_bilevel.miff VST 
%exedir%rwfile.exe %srcdir%input_gray.miff VST 
%exedir%rwfile.exe %srcdir%input_256c.miff VST 
%exedir%rwfile.exe %srcdir%input_truecolor.miff VST 
%exedir%rwfile.exe %srcdir%input_bilevel.miff WBMP 
%exedir%rwfile.exe %srcdir%input_gray.miff WBMP 
%exedir%rwfile.exe %srcdir%input_256c.miff WBMP 
%exedir%rwfile.exe %srcdir%input_truecolor.miff WBMP 
%exedir%rwfile.exe %srcdir%input_bilevel.miff XBM 
%exedir%rwfile.exe %srcdir%input_gray.miff XBM 
%exedir%rwfile.exe %srcdir%input_256c.miff XBM 
%exedir%rwfile.exe %srcdir%input_truecolor.miff XBM 
%exedir%rwfile.exe %srcdir%input_bilevel.miff XPM 
%exedir%rwfile.exe %srcdir%input_gray.miff XPM 
%exedir%rwfile.exe %srcdir%input_256c.miff XPM 
%exedir%rwfile.exe %srcdir%input_truecolor.miff XPM 
%exedir%rwfile.exe %srcdir%input_bilevel.miff XWD 
%exedir%rwfile.exe %srcdir%input_gray.miff XWD 
%exedir%rwfile.exe %srcdir%input_256c.miff XWD 
%exedir%rwfile.exe %srcdir%input_truecolor.miff XWD
%exedir%rwfile.exe -size 70x46 %srcdir%input_bilevel.miff CMYK 
%exedir%rwfile.exe -size 70x46 %srcdir%input_gray.miff CMYK 
%exedir%rwfile.exe -size 70x46 %srcdir%input_256c.miff CMYK 
%exedir%rwfile.exe -size 70x46 %srcdir%input_truecolor.miff CMYK 
%exedir%rwfile.exe -size 70x46 %srcdir%input_bilevel.miff GRAY 
%exedir%rwfile.exe -size 70x46 %srcdir%input_gray.miff GRAY 
%exedir%rwfile.exe -size 70x46 %srcdir%input_256c.miff GRAY 
%exedir%rwfile.exe -size 70x46 %srcdir%input_truecolor.miff GRAY 
%exedir%rwfile.exe -size 70x46 %srcdir%input_bilevel.miff MONO 
%exedir%rwfile.exe -size 70x46 %srcdir%input_gray.miff MONO 
%exedir%rwfile.exe -size 70x46 %srcdir%input_256c.miff MONO 
%exedir%rwfile.exe -size 70x46 %srcdir%input_truecolor.miff MONO 
%exedir%rwfile.exe -size 70x46 %srcdir%input_bilevel.miff PAL 
%exedir%rwfile.exe -size 70x46 %srcdir%input_gray.miff PAL 
%exedir%rwfile.exe -size 70x46 %srcdir%input_256c.miff PAL 
%exedir%rwfile.exe -size 70x46 %srcdir%input_truecolor.miff PAL 
%exedir%rwfile.exe -size 70x46 %srcdir%input_bilevel.miff RGB 
%exedir%rwfile.exe -size 70x46 %srcdir%input_gray.miff RGB 
%exedir%rwfile.exe -size 70x46 %srcdir%input_256c.miff RGB 
%exedir%rwfile.exe -size 70x46 %srcdir%input_truecolor.miff RGB 
%exedir%rwfile.exe -size 70x46 %srcdir%input_bilevel.miff RGBA 
%exedir%rwfile.exe -size 70x46 %srcdir%input_gray.miff RGBA 
%exedir%rwfile.exe -size 70x46 %srcdir%input_256c.miff RGBA 
%exedir%rwfile.exe -size 70x46 %srcdir%input_truecolor.miff RGBA 
%exedir%rwfile.exe -size 70x46 %srcdir%input_bilevel.miff UYVY 
%exedir%rwfile.exe -size 70x46 %srcdir%input_gray.miff UYVY 
%exedir%rwfile.exe -size 70x46 %srcdir%input_256c.miff UYVY 
%exedir%rwfile.exe -size 70x46 %srcdir%input_truecolor.miff UYVY 
%exedir%rwfile.exe -size 70x46 %srcdir%input_bilevel.miff YUV 
%exedir%rwfile.exe -size 70x46 %srcdir%input_gray.miff YUV 
%exedir%rwfile.exe -size 70x46 %srcdir%input_256c.miff YUV 
%exedir%rwfile.exe -size 70x46 %srcdir%input_truecolor.miff YUV 

