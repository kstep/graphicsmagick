@echo off
set srcdir=..\..\..\Magick++\demo
set bindir=..\..\bin\
set outdir=
set PATH=%bindir%;%PATH%

echo button ...
button

echo executing demo ...
demo

echo executing flip ...
flip

echo executing gravity ...
gravity

echo executing piddle ...
piddle

echo executing shapes ...
shapes

echo executing zoom point 600x600 model.miff zoom_point_out.miff
zoom point 600x600 %srcdir%\model.miff %outdir%zoom_point_out.miff

echo executing zoom box 600x600 model.miff zoom_box_out.miff
zoom box 600x600 %srcdir%\model.miff %outdir%zoom_box_out.miff

echo executing zoom triangle 600x600 model.miff zoom_triangle_out.miff
zoom triangle 600x600 %srcdir%\model.miff %outdir%zoom_triangle_out.miff

echo executing hermite 600x600 model.miff zoom_hermite_out.miff
zoom hermite 600x600 %srcdir%\model.miff %outdir%zoom_hermite_out.miff

echo zoom hanning 600x600 model.miff zoom_hanning_out.miff
zoom hanning 600x600 %srcdir%\model.miff %outdir%zoom_hanning_out.miff

echo zoom hamming 600x600 model.miff zoom_hamming_out.miff
zoom hamming 600x600 %srcdir%\model.miff %outdir%zoom_hamming_out.miff

echo zoom blackman 600x600 model.miff zoom_blackman_out.miff
zoom blackman 600x600 %srcdir%\model.miff %outdir%zoom_blackman_out.miff

echo zoom gaussian 600x600 model.miff zoom_gaussian_out.miff
zoom gaussian 600x600 %srcdir%\model.miff %outdir%zoom_gaussian_out.miff

echo zoom quadratic 600x600 model.miff zoom_quadratic_out.miff
zoom quadratic 600x600 %srcdir%\model.miff %outdir%zoom_quadratic_out.miff

echo zoom cubic 600x600 model.miff zoom_cubic_out.miff
zoom cubic 600x600 %srcdir%\model.miff %outdir%zoom_cubic_out.miff

echo zoom catrm 600x600 model.miff zoom_catrm_out.miff
zoom catrm 600x600 %srcdir%\model.miff %outdir%zoom_catrm_out.miff

echo zoom mitchell 600x600 model.miff zoom_mitchell_out.miff
zoom mitchell 600x600 %srcdir%\model.miff %outdir%zoom_mitchell_out.miff

echo zoom lanczos 600x600 model.miff zoom_lanczos_out.miff
zoom lanczos 600x600 %srcdir%\model.miff %outdir%zoom_lanczos_out.miff

echo zoom bessel 600x600 model.miff zoom_bessel_out.miff
zoom bessel 600x600 %srcdir%\model.miff %outdir%zoom_bessel_out.miff

echo zoom sinc 600x600 model.miff zoom_sinc_out.miff
zoom sinc 600x600 %srcdir%\model.miff %outdir%zoom_sinc_out.miff


