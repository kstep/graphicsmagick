#!/usr/local/bin/perl
# Copyright (C) 2003 - 2009 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1991-1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test Ping
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

my ($image, $width, $height, $size, $format, $status, @blob);

print("Ping \"logo:\" ...\n");
$image=Graphics::Magick->new;
($width, $height, $size, $format)=$image->Ping('logo:');
print "  width=$width, height=$height, size=$size, format=$format\n";
if (($width == 654) && ($height == 418) && ($size == 22077) && ($format eq "GIF")) {
    print "ok $test\n";
  } else {
    print "not ok $test\n";
  }
undef $image;
++$test;

print("Ping blob ...\n");
$image=Graphics::Magick->new;
$status=$image->Read('logo:');
warn "$status" if "$status";
@blob=$image->ImageToBlob();
undef $image;
$image=Graphics::Magick->new;
($width, $height, $size, $format)=$image->Ping(blob=>@blob);
undef @blob;
undef $image;
print "  width=$width, height=$height, size=$size, format=$format\n";
if (($width == 654) && ($height == 418) && ($size == 22077) && ($format eq "GIF")) {
    print "ok $test\n";
  } else {
    print "not ok $test\n";
  }

1;
