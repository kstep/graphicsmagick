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
# Test writing Postscript images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}

use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/ps' || die 'Cd failed';

#
# 1) Test Postscript
#
testReadWriteNoVerify( 'input.miff',
		       'output.ps',
		       q// );
#
# 2) Test Encapsulated Postscript
#
++$test;
testReadWriteNoVerify( 'input.miff',
		       'output.eps',
		       q// );

