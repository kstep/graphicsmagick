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
    '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f')
  { print "not ok $test\n"; }
else
  { print "ok $test\n"; }

1;
