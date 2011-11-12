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
# Test reading an image which uses BZip compression
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/bzlib' || die 'Cd failed';

#
# Test reading BZip compressed MIFF
# 
testRead( 'input.miff',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );

#
# 2) Test reading BZip stream-compressed MIFF (.bz2 extension)
#
print("Reading BZip stream-compressed MIFF (.bz2 extension) ...\n");
++$test;
testRead( 'input.miff.bz2',
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f' );
