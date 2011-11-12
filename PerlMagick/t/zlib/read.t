#!/usr/local/bin/perl
# Copyright (C) 2003, 2008 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1991-1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test reading an image which uses Zip compression
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..3\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/zlib' || die 'Cd failed';

#
# 1) Test reading Zip compressed MIFF
#
print("Reading Zip compressed MIFF ...\n");
testRead( 'input.miff',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );

#
# 2) Test reading Zip compressed MAT
#
print("Reading Zip compressed MAT ...\n");
++$test;
testRead( 'input_gray_lsb_08bit_zip.mat',
  'cd0e75dd75bce03537c17bf067e19e97d1e28feb18a5ba54dddaf3309097c255' );

#
# 3) Test reading Zip stream-compressed MIFF (.gz extension)
#
print("Reading Zip stream-compressed MIFF (.gz extension) ...\n");
++$test;
testRead( 'input.miff.gz',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );
