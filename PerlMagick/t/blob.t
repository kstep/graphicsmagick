#!/usr/local/bin/perl
#
# Test image blobs.
#
BEGIN { $| = 1; $test=1, print "1..1\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

chdir 't' || die 'Cd failed';

$image = new Image::Magick;
$image->Read( 'input.miff' );
@blob = $image->ImageToBlob();
undef $image;

$image=Image::Magick->new( magick=>'MIFF' );
$image->BlobToImage( @blob );

if ($image->Get('signature') ne 
    'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca')
  { print "not ok $test\n"; }
else
  { print "ok $test\n"; }

1;
