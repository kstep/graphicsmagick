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
# Test image blobs.
#
BEGIN { $| = 1; $test=1, print "1..1\n"; }
END {print "not ok 1\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

chdir 't' || die 'Cd failed';

$image = new Graphics::Magick;
$image->Read( 'input.miff' );
@blob = $image->ImageToBlob();
undef $image;

$image=Graphics::Magick->new( magick=>'MIFF' );
$image->BlobToImage( @blob );

if ($image->Get('signature') ne 
    '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f')
  { print "not ok $test\n"; }
else
  { print "ok $test\n"; }

1;
