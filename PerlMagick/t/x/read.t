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
# 1) Test rendering text using common X11 font
#
if ( defined($ENV{'DISPLAY'}) && ($ENV{'DISPLAY'} ne '') ) {

  $font   = '-*-courier-bold-r-normal--14-100-100-100-m-90-iso8859-1';

  # Ensure that Ghostscript is out of the picture
  $SAVEDPATH=$ENV{'PATH'};
  $ENV{'PATH'}='';

  $image=Graphics::Magick->new;
  $x=$image->Set(font=>"$font", pen=>'#0000FF', dither=>'False');
  if( "$x" ) {
    print "$x\n";
    print "not ok $test\n";
  } else {
    $x=$image->ReadImage('label:The quick brown fox jumps over the lazy dog.');
    if( "$x" ) {
      print "ReadImage: $x\n";
      # If server can't be accessed, ImageMagick returns this warning
      # Warning 305: Unable to open X server
      $x =~ /(\d+)/;
      my $errorCode = $1;
      if ( $errorCode > 0 ) {
        print "not ok $test\n";
      } else {
        print "ok $test\n";
      }
    } else {
      #$image->Display();
      print "ok $test\n";
    }
  }
  undef $image;

  $ENV{'PATH'}=$SAVEDPATH;
} else {
  print "ok $test\n";
}

$test = 0;  # Quench PERL compliaint

