#!/usr/local/bin/perl
# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1991-1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test image filters.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..44\n"; }
END {print "not ok 1\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';
use FileHandle;
autoflush STDOUT 1;
autoflush STDERR 1;

#$QuantumDepth=quantumDepth();
$fuzz=int($MaxRGB*0.05);

testFilterCompare('input.miff', q//, 'reference/filter/Blur.miff', 'Blur',
  q/geometry=>"0.0x1.0"/, 8.1e-06, 4.7e-05);

++$test;
testFilterCompare('input.miff',  q//, 'reference/filter/Border.miff', 'Border',
  q/geometry=>"10"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Channel.miff', 'Channel',
  q/channel=>"Red"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Chop.miff', 'Chop',
  q/geometry=>"10x10!"/, 0, 0);

++$test;
testFilterCompare('input.miff', "fuzz=>$fuzz", 'reference/filter/ColorFloodfill.miff', 'ColorFloodfill',
  q/geometry=>"+25+45"/, 0, 0);

++$test;
testFilterCompare('input.miff', "fuzz=>$fuzz", 'reference/filter/Colorize.miff', 'Colorize',
  q/fill=>"red", opacity=>50/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Contrast.miff', 'Contrast',
  q/sharpen=>True/, 8e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//,
 'reference/filter/Convolve.miff', 'Convolve', q/coefficients=>[1, 2, 1, 2, 4, 2, 1, 2, 1]/, 7.7e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Crop.miff', 'Crop',
  q/geometry=>"10x10!"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Despeckle.miff', 'Despeckle',
  q//, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Edge.miff', 'Edge',
  q/radius=>3/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Emboss.miff', 'Emboss',
  q/geometry=>"2x1"/, 4.6e-05, 0.03);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Equalize.miff', 'Equalize',
  q//, 9e-06, 2e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Flip.miff', 'Flip',
  q//, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Flop.miff', 'Flop',
  q//, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Frame.miff', 'Frame',
  q/geometry=>"10x10+2+2"/, 1.7e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Gamma.miff', 'Gamma',
  q/gamma=>"2.2"/, 7.3e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Implode.miff', 'Implode',
  q/amount=>0.5/, 6e-06, 8.7e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Magnify.miff', 'Magnify',
  q//, 5.6e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/MatteFloodfill.miff', 'MatteFloodfill',
  q/geometry=>"+25+45"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Minify.miff', 'Minify',
  q//, 7.9e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Modulate.miff', 'Modulate',
  q/brightness=>60, saturation=>30, hue=>20/, 7.6e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/MotionBlur.miff', 'MotionBlur',
  q/radius=>2,sigma=>1,angle=>30/, 4e-8, 2e-5);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Negate.miff', 'Negate',
  q/gray=>False/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Normalize.miff', 'Normalize',
  q//, 7.5e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/OilPaint.miff', 'OilPaint',
  q/radius=>3/, 0.03, 0.7);

++$test;
testFilterCompare('input.miff', "fuzz=>$fuzz", 'reference/filter/Opaque.miff', 'Opaque',
  q/color=>"#e23834", fill=>"green"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Quantize.miff', 'Quantize',
                  q/colors=>128/, 0.002, 0.025);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Raise.miff', 'Raise',
  q/geometry=>"10x10"/, 1.6e-06, 1.6e-05);
# Q:32   mean-error=0.000608108204635624, maximum-error=0.0117954632778599

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Resize.miff', 'Resize',
  q/geometry=>"50%", filter=>"Box"/, 7.6e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Roll.miff', 'Roll',
  q/geometry=>"+10+10"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Rotate.miff', 'Rotate',
  q/degrees=>10,color=>"green"/, 7e-06, 6.2e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Sample.miff', 'Sample',
  q/geometry=>"50%"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Scale.miff', 'Scale',
  q/geometry=>"50%"/, 1.6e-05, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Segment.miff', 'Segment',
  q/geometry=>"1.0x1.5"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Shade.miff', 'Shade',
  q/geometry=>"30x30",gray=>'true'/, 3.9e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Sharpen.miff', 'Sharpen',
  q/geometry=>"0.0x1.0"/, 7.4e-06, 1.6e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Shave.miff', 'Shave',
  q/geometry=>"10x10"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Shear.miff', 'Shear',
  q/geometry=>10,color=>"yellow"/, 3.8e-06, 4.2e-05);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Solarize.miff', 'Solarize',
  q/60/, 0.14, 0.60);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Swirl.miff', 'Swirl',
  q/degrees=>60/, 6e-06, 1.6e-05);

# Being just one pixel different spikes maximum error for the threshold
# test, so check only mean error.  This test is primarily influenced by
# the pixel intensity macro.
++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Threshold.miff', 'Threshold',
  q/threshold=>"50%"/, 0.0013, 1);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Trim.miff', 'Trim',
  q//, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Wave.miff', 'Wave',
  q/geometry=>"25x157"/, 3.4e-06, 1.6e-05);

++$test;

1;
