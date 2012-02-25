#!/usr/local/bin/perl
# Copyright (C) 2003 - 2008 GraphicsMagick Group
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
BEGIN { $| = 1; $test=1, print "1..49\n"; }
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
  q/geometry=>"0.0x1.0"/, 0.003, 0.007);

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
  q/sharpen=>True/, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//,
 'reference/filter/Convolve.miff', 'Convolve', q/coefficients=>[1, 2, 1, 2, 4, 2, 1, 2, 1]/, 0.003, 0.004);

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
  q/geometry=>"2x1"/, 0.003, 0.15);

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
  q/geometry=>"10x10+2+2"/, 0.0005, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Gamma.miff', 'Gamma',
  q/gamma=>"2.2"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Implode.miff', 'Implode',
  q/amount=>0.5/, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Magnify.miff', 'Magnify',
  q//, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/MatteFloodfill.miff', 'MatteFloodfill',
  q/geometry=>"+25+45"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Minify.miff', 'Minify',
  q//, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Modulate.miff', 'Modulate',
  q/brightness=>60, saturation=>30, hue=>20/, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/MotionBlur.miff', 'MotionBlur',
  q/radius=>0,sigma=>7,angle=>30/, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Negate.miff', 'Negate',
  q/gray=>False/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Normalize.miff', 'Normalize',
  q//, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/OilPaint.miff', 'OilPaint',
  q/radius=>3/, 0.03, 0.7);

++$test;
testFilterCompare('input.miff', "fuzz=>$fuzz", 'reference/filter/Opaque.miff', 'Opaque',
  q/color=>"#e23834", fill=>"green"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Quantize.miff', 'Quantize',
                  q/colors=>128/, 0.02, 0.16);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Raise.miff', 'Raise',
  q/geometry=>"10x10"/, 0.003, 0.004);
# Q:32   mean-error=0.000608108204635624, maximum-error=0.0117954632778599

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Resize.miff', 'Resize',
  q/geometry=>"50%", filter=>"Box"/, 0.004, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Roll.miff', 'Roll',
  q/geometry=>"+10+10"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Rotate10.miff', 'Rotate',
  q/degrees=>10,color=>"green"/, 0.003, 0.007);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Rotate0.miff', 'Rotate',
  q/degrees=>0/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Rotate90.miff', 'Rotate',
  q/degrees=>90/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Rotate180.miff', 'Rotate',
  q/degrees=>180/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Rotate270.miff', 'Rotate',
  q/degrees=>270/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Sample.miff', 'Sample',
  q/geometry=>"50%"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Scale.miff', 'Scale',
  q/geometry=>"50%"/, 0.004, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Segment.miff', 'Segment',
  q/geometry=>"1.0x1.5"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Shade.miff', 'Shade',
  q/geometry=>"30x30",gray=>'true'/, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Sharpen.miff', 'Sharpen',
  q/geometry=>"0.0x1.0"/, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Shave.miff', 'Shave',
  q/geometry=>"10x10"/, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Shear.miff', 'Shear',
  q/geometry=>10,color=>"yellow"/, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Solarize.miff', 'Solarize',
  0.40*MaxRGB, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Swirl.miff', 'Swirl',
  q/degrees=>60/, 0.003, 0.004);

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
testFilterCompare('input.miff', q//, 'reference/filter/UnsharpMask.miff', 'UnsharpMask',
  q/geometry=>"0.0x3.0+30+5"/, 0.003, 0.004);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Wave.miff', 'Wave',
  q/geometry=>"25x157"/, 0.003, 0.004);

++$test;

1;
