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
# Test reading CGM files
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/cgm' || die 'Cd failed';

testRead( 'CGM:input.cgm',
  'fe8a94eb6a8ecb95badb6c69e1d5c21a75fa3b97c8367eed1a3fc76a407638bb',
  '5b0c5cb4e921b78666b19ac3fcb32d5398f3f229fe8a7b88df39d4a4ceb0033e');

