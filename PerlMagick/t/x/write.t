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
# Test accessing X11 server
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#

BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/x' || die 'Cd failed';

#
# 1) Test reading and displaying an image
#
if ( defined($ENV{'DISPLAY'}) && ($ENV{'DISPLAY'} ne '') ) {
  $image=Graphics::Magick->new;
  $x=$image->ReadImage('congrats.miff');
  if( "$x" ) {
    print "not ok $test\n";
  } else {
    $x = $image->Display(delay=>800);
    if( "$x" ) {
      print "not ok $test\n";
    } else {
      print "ok $test\n";
    }
  }
  undef $image;
} else {
  print "ok $test\n";
}

$test = 0;  # Quench PERL compliaint

