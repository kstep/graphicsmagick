#!/usr/local/bin/perl
# Copyright (C) 2003 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test composition operators
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..18\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

#
# Add
#
# The result of composite image + image, with overflow wrapping around
# (mod 256).
#
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Add'/,
                     'reference/composite/Add.miff',0,0);
#
# Atop
#
# The result is the same shape as image image, with composite image
# obscuring image where the image shapes overlap. Note this differs
# from over because the portion of composite image outside image's
# shape does not appear in the result.
#
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Atop'/,
                     'reference/composite/Atop.miff',0,0);

#
# Bumpmap
#
# The result image shaded by composite image.
#
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Bumpmap'/,
                     'reference/composite/Bumpmap.miff',0,0);

#
# Clear
#
# The result is transparent where composite image obscures the image.
#
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Clear'/,
                     'reference/composite/Clear.miff',0,0);

#
# Copy
#
# The resulting image is image replaced with composite image.  The
# matte information is ignored.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Copy'/,
                     'reference/composite/Copy.miff',0,0);

#
# CopyBlue
#
# The resulting image is the blue layer in image replaced with the
# blue layer in composite image.  The other layers are copied
# untouched.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'CopyBlue'/,
                     'reference/composite/CopyBlue.miff',0,0);

#
# CopyGreen
#
# The resulting image is the green layer in image replaced with the
# green layer in composite image.  The other layers are copied
# untouched.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'CopyGreen'/,
                     'reference/composite/CopyGreen.miff',0,0);

#
# CopyRed
#
# The resulting image is the red layer in image replaced with the red
# layer in composite image.  The other layers are copied untouched.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'CopyRed'/,
                     'reference/composite/CopyRed.miff',0,0);

#
# CopyOpacity
#
# The resulting image is the opacity layer in image replaced with the
# opacity layer in composite image.  The other layers are copied
# untouched.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'CopyOpacity'/,
                     'reference/composite/CopyOpacity.miff',0,0);

#
# Difference
#
# The result of abs(composite image - image).  This is useful for
# comparing two very similar images.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Difference'/,
                     'reference/composite/Difference.miff',0,0);

#
# In
#
# The result is composite image cut by the shape of image.  None of
# the image data of image will be in the result.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'In'/,
                     'reference/composite/In.miff',0,0);

#
# Minus
#
# The result of composite image - image, with underflow cropped to
# zero.  The matte channel is ignored (set to 255, full coverage).
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Minus'/,
                     'reference/composite/Minus.miff',0,0);

#
# Multiply
#
# The result of composite image * image.  This is useful for the
# creation of drop-shadows.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Multiply'/,
                     'reference/composite/Multiply.miff',0,0);

#
# Out
#
# The resulting image is composite image with the shape of image cut
# out.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Out'/,
                     'reference/composite/Out.miff',0,0);

#
# Over
#
# The result will be the union of the two image shapes, with
# opaque  areas  of  composite  image obscuring image in the
# region of overlap.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Over'/,
                     'reference/composite/Over.miff',0,0);

#
# Plus
#
# The result is just the sum of the image data.  Output values are
# cropped to 255 (no overflow).  This operation is independent of the
# matte channels.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Plus'/,
                     'reference/composite/Plus.miff',0,0);

#
# Subtract
#
# The result of composite image - image, with underflow wrapping
# around (mod 256).  The add and subtract operators can be used to
# perform reversible transformations.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Subtract'/,
                     'reference/composite/Subtract.miff',0,0);

#
# Xor
#
# The result is the image data from both composite image and image
# that is outside the overlap region.  The overlap region will be
# blank.
++$test;
testCompositeCompare('gradient:white-black',q/size=>"100x80"/,
                     'input.miff', q//,
                     q/, gravity=>'Center', compose=>'Xor'/,
                     'reference/composite/Xor.miff',0,0);

1;
