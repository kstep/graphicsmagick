#!/usr/local/bin/perl
#
# Test setting & getting attributes.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..111\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

testSetAttribute('input.miff','adjoin','True');
++$test;
testSetAttribute('input.miff','adjoin','False');

++$test;
testSetAttribute('input.miff','background','#808080');

++$test;
testSetAttribute('input.miff','bordercolor','#808080');

++$test;
testSetAttribute('input.miff','colormap[20]','#808080');

++$test;
testSetAttribute('input.miff','colorspace','RGB');

++$test;
testSetAttribute('input.miff','colorspace','Gray');

++$test;
testSetAttribute('input.miff','colorspace','Transparent');

++$test;
testSetAttribute('input.miff','colorspace','OHTA');

++$test;
testSetAttribute('input.miff','colorspace','XYZ');

++$test;
testSetAttribute('input.miff','colorspace','YCbCr');

++$test;
testSetAttribute('input.miff','colorspace','YCC');

++$test;
testSetAttribute('input.miff','colorspace','YIQ');

++$test;
testSetAttribute('input.miff','colorspace','YPbPr');

++$test;
testSetAttribute('input.miff','colorspace','YUV');

++$test;
testSetAttribute('input.miff','colorspace','CMYK');

++$test;
testSetAttribute('input.miff','colors',64);

++$test;
testSetAttribute('input.miff','compress','None');

++$test;
testSetAttribute('input.miff','compress','JPEG');

++$test;
testSetAttribute('input.miff','compress','LZW');

++$test;
testSetAttribute('input.miff','compress','Runlength');

++$test;
testSetAttribute('input.miff','compress','Zip');

++$test;
testSetAttribute('input.miff','density','72');

++$test;
testSetAttribute('input.miff','dispose',1);

++$test;
testSetAttribute('input.miff','dispose',2);

++$test;
testSetAttribute('input.miff','dispose',3);

++$test;
testSetAttribute('input.miff','dispose',4);

++$test;
testSetAttribute('input.miff','delay',100);

++$test;
testSetAttribute('input.miff','dither','True');

++$test;
testSetAttribute('input.miff','dither','False');

++$test;
testSetAttribute('input.miff','display','bogus:0.0');

++$test;
testSetAttribute('input.miff','filename','bogus.jpg');

++$test;
testSetAttribute('input.miff','filter','Point');

++$test;
testSetAttribute('input.miff','filter','Box');

++$test;
testSetAttribute('input.miff','filter','Triangle');

++$test;
testSetAttribute('input.miff','filter','Hermite');

++$test;
testSetAttribute('input.miff','filter','Hanning');

++$test;
testSetAttribute('input.miff','filter','Hamming');

++$test;
testSetAttribute('input.miff','filter','Blackman');

++$test;
testSetAttribute('input.miff','filter','Gaussian');

++$test;
testSetAttribute('input.miff','filter','Quadratic');

++$test;
testSetAttribute('input.miff','filter','Cubic');

++$test;
testSetAttribute('input.miff','filter','Catrom');

++$test;
testSetAttribute('input.miff','filter','Mitchell');

++$test;
testSetAttribute('input.miff','filter','Lanczos');

++$test;
testSetAttribute('input.miff','filter','Bessel');

++$test;
testSetAttribute('input.miff','filter','Sinc');

++$test;
testSetAttribute('input.miff','font',q/-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-*/);

++$test;
testSetAttribute('input.miff','format','JPEG');

++$test;
testSetAttribute('input.miff','iterations',10);

++$test;
testSetAttribute('input.miff','interlace','None');

++$test;
testSetAttribute('input.miff','interlace','Line');

++$test;
testSetAttribute('input.miff','interlace','Plane');

++$test;
testSetAttribute('input.miff','interlace','Partition');

++$test;
testSetAttribute('input.miff','loop',100);

++$test;
testSetAttribute('input.miff','magick','TIFF');

++$test;
testSetAttribute('input.miff','mattecolor','#808080');

++$test;
testSetAttribute('input.miff','monochrome','True');

++$test;
testSetAttribute('input.miff','monochrome','False');

++$test;
testSetAttribute('input.miff','page','595x842>+0+0');

++$test;
# The value must be equal to the value of the image at this pixel (currently #ccc)
testSetAttribute('input.miff','pixel[20,23]','192,192,192,145');

++$test;
testSetAttribute('input.miff','pointsize',12);

++$test;
testSetAttribute('input.miff','preview','Rotate');

++$test;
testSetAttribute('input.miff','preview','Shear');

++$test;
testSetAttribute('input.miff','preview','Roll');

++$test;
testSetAttribute('input.miff','preview','Hue');

++$test;
testSetAttribute('input.miff','preview','Saturation');

++$test;
testSetAttribute('input.miff','preview','Brightness');

++$test;
testSetAttribute('input.miff','preview','Gamma');

++$test;
testSetAttribute('input.miff','preview','Spiff');

++$test;
testSetAttribute('input.miff','preview','Dull');

++$test;
testSetAttribute('input.miff','preview','Grayscale');

++$test;
testSetAttribute('input.miff','preview','Quantize');

++$test;
testSetAttribute('input.miff','preview','Despeckle');

++$test;
testSetAttribute('input.miff','preview','ReduceNoise');

++$test;
testSetAttribute('input.miff','preview','AddNoise');

++$test;
testSetAttribute('input.miff','preview','Sharpen');

++$test;
testSetAttribute('input.miff','preview','Blur');

++$test;
testSetAttribute('input.miff','preview','Threshold');

++$test;
testSetAttribute('input.miff','preview','EdgeDetect');

++$test;
testSetAttribute('input.miff','preview','Spread');

++$test;
testSetAttribute('input.miff','preview','Solarize');

++$test;
testSetAttribute('input.miff','preview','Shade');

++$test;
testSetAttribute('input.miff','preview','Raise');

++$test;
testSetAttribute('input.miff','preview','Segment');

++$test;
testSetAttribute('input.miff','preview','Solarize');

++$test;
testSetAttribute('input.miff','preview','Swirl');

++$test;
testSetAttribute('input.miff','preview','Implode');

++$test;
testSetAttribute('input.miff','preview','Wave');

++$test;
testSetAttribute('input.miff','preview','OilPaint');

++$test;
testSetAttribute('input.miff','preview','Charcoal');

++$test;
testSetAttribute('input.miff','quality',25);

++$test;
testSetAttribute('input.miff','scene',5);

++$test;
testSetAttribute('input.miff','subimage',9);

++$test;
testSetAttribute('input.miff','subrange',16);

++$test;
testSetAttribute('input.miff','server','mymachine:0.0');

++$test;
testSetAttribute('input.miff','size','25x25');

++$test;
testSetAttribute('input.miff','size','25x25');

# I have no idea what this does
++$test;
testSetAttribute('input.miff','tile','some value');

++$test;
testSetAttribute('input.miff','texture','granite:');

++$test;
testSetAttribute('input.miff','treedepth',0);

++$test;
testSetAttribute('input.miff','treedepth',1);

++$test;
testSetAttribute('input.miff','treedepth',2);

++$test;
testSetAttribute('input.miff','treedepth',3);

++$test;
testSetAttribute('input.miff','treedepth',4);

++$test;
testSetAttribute('input.miff','treedepth',5);

++$test;
testSetAttribute('input.miff','treedepth',6);

++$test;
testSetAttribute('input.miff','treedepth',7);

++$test;
testSetAttribute('input.miff','undercolor','0.5x0.7');

++$test;
testSetAttribute('input.miff','verbose','True');

++$test;
testSetAttribute('input.miff','verbose','False');

