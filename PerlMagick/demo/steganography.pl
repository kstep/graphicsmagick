#!/usr/local/bin/perl

use Graphics::Magick;

#
# Create watermark.
#
$watermark=Graphics::Magick->new;
$watermark->ReadImage('smile.gif');
($width, $height)=$watermark->Get('width','height');
#
# Hide watermark in image.
#
$image=Graphics::Magick->new;
$image->ReadImage('model.gif');
$image->SteganoImage(image=>$watermark,offset=>91);
$image->Write('model.png');
$image->Write('win:');
#
# Extract watermark from image.
#
$size="$width" . "x" . "$height" . "+91";
$stegano=Graphics::Magick->new(size=>$size);
$stegano->ReadImage('stegano:model.png');
$stegano->Write('stegano.gif');
$stegano->Write('win:');
