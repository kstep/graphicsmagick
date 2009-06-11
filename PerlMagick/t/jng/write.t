#!/usr/local/bin/perl
# Copyright (C) 2003-2009 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test reading JPEG images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..11\n"; }
END {print "not ok $test\n" unless $loaded;}

use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/jng' || die 'Cd failed';

# 1
testReadWriteCompare( 'input_gray_idat.jng', 'gray_idat_tmp.jng', '../reference/jng/write_gray_idat.miff', q//, q//, 1.0e-04, 0.008);
++$test;
# 2
testReadWriteCompare( 'input_gray_jdaa.jng', 'gray_jdaa_tmp.jng', '../reference/jng/write_gray_jdaa.miff', q//, q//, 1.0e-04, 0.008);
++$test;
# 3
testReadWriteCompare( 'input_gray.jng', 'gray_tmp.jng', '../reference/jng/write_gray.miff', q//, q//, 1.0e-04, 0.008);
++$test;
# 4
testReadWriteCompare( 'input_gray_prog_idat.jng', 'gray_prog_idat_tmp.jng', '../reference/jng/write_gray_prog_idat.miff', q//, q//, 1.0e-04, 0.008);
++$test;
# 5
testReadWriteCompare( 'input_gray_prog_jdaa.jng', 'gray_prog_jdaa_tmp.jng', '../reference/jng/write_gray_prog_jdaa.miff', q//, q//, 1.0e-04, 0.008);
++$test;
# 6
testReadWriteCompare( 'input_gray_prog.jng', 'gray_prog_tmp.jng', '../reference/jng/write_gray_prog.miff', q//, q//, 0.012, 0.145);
++$test;
# 7
testReadWriteCompare( 'input_idat.jng', 'idat_tmp.jng', '../reference/jng/write_idat.miff', q//, q//, 0.012, 0.145);
++$test;
# 8
testReadWriteCompare( 'input_jdaa.jng', 'jdaa_tmp.jng', '../reference/jng/write_jdaa.miff', q//, q//, 0.012, 0.145);
++$test;
# 9
testReadWriteCompare( 'input_prog_idat.jng', 'prog_idat_tmp.jng', '../reference/jng/write_prog_idat.miff', q//, q//, 0.012, 0.145);
++$test;
# 10
testReadWriteCompare( 'input_prog_jdaa.jng', 'prog_jdaa_tmp.jng', '../reference/jng/write_prog_jdaa.miff', q//, q//, 0.012, 0.145);
++$test;
# 11
testReadWriteCompare( 'input_prog.jng', 'prog_tmp.jng', '../reference/jng/write_prog.miff', q//, q//, 0.012, 0.145);

