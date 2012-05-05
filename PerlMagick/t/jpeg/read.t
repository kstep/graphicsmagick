#!/usr/local/bin/perl
# Copyright (C) 2003 - 2012 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1991-1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test read image method on non-interlaced JPEG.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..3\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/jpeg' || die 'Cd failed';

#
# 1) Test non-interlaced image read
# 
print( "Non-interlaced JPEG ...\n" );
testReadCompare('input.jpg', '../reference/jpeg/read_non_interlaced.miff', q//, 0, 0);

#
# 2) Test plane-interlaced image read
# 
++$test;
print( "Plane-interlaced JPEG ...\n" );
testReadCompare('input_plane.jpg', '../reference/jpeg/read_plane_interlaced.miff', q//, 0, 0);

#
# 3) Test Seattle FilmWorks image file ...\n");
#
print("Seattle FilmWorks image file ...\n");
++$test;
testReadCompare('input.sfw', '../reference/jpeg/read_sfw.miff', q//, 0.003, 0.1);
