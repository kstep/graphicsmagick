#!/usr/local/bin/perl
#
# Test image filter.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..35\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

testFilter('input.miff', 'Blur',
  q/60/,
  '18828457c93a7828f0be6f6ec86c6fad');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '9a7ac9a5a8277607203c49fc5bf255c6');

++$test;
testFilter('input.miff', 'Charcoal',
  q/60/,
  'e6e381790100fb5300ed464c2b49ecf8');

++$test;
testFilter('input.miff', 'Chop',
  q/+10+10/,
  '1026a48f39484af0800838a548e694b4');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  '2490b2a9004fe6d8f875ce39ebcba207');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '1026a48f39484af0800838a548e694b4');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  '6dc34477296f85d8584e655b3a429945');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '46eafba6959408c656306a5de9f7410d');

++$test;
testFilter('input.miff', 'Edge',
  q/60/,
  '2edaae4a79630e297ef1e3be5ac77aec');

++$test;
testFilter('input.miff', 'Emboss',
  q/ /,
  '6274e0e8b826b2ad18172a71dc0ad810');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  '6095eb735f80131017fb1fd1bb747a36');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  '2fcb3c91e914e5e56f0f060a81e26273');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  'edda94ea81b32d62695edb3bc9c4b3e2');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  '11a79621dd46ae1777f397ff0f84c4c9');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'ba5ea665ae07fb312383aa9c44ba09d6');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '71d299665cc5e76c8c44d443bad11690');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  'fe6d999ec829b4b486a599ba3f3c728b');

++$test;
testFilter('input.miff', 'Negate',
  q/0/,
  '8eed5e17b4e612cadb970940c6435bfe');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  'e1eae58ba4cce4603876c7ee6efe63a6');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  '4841d3c0818c9b577759e4da92e4d36e');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '032db0097c8f913bb72f01d4498a71d4');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '88a540968c9a6eeca1921b08970c4384');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '4a998a37c4acf55ab50addd3dceb2ec5');

++$test;
testFilter('input.miff', 'Rotate',
  q/45/,
  '18a94fddbb2848b584541ed39388a388');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '3d541e271af26574314643698621c4bb');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  '4a3a3b6125fc27998b3f4ee086d2c6d7');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '492cfb48dd301a0dad2cbf79afea916c');

++$test;
testFilter('input.miff', 'Shade',
  q/30x30/,
  '9b34d3b1d6b183905618b02a80dcfdcd');

++$test;
testFilter('input.miff', 'Shear',
  q/45x45/,
  'a8dd14103e76a89e3c2977dd4a1094c7');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '0d16baeec2cb7a0f6251a2722e49dbf6');

++$test;
testFilter('input.miff', 'Transform',
  q/geometry=>50%60/,
  '0b0565f2bebf00dd82ddbde12f0d0083');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  'ad5daf059a1f4dcd132f8aeed878611d');

++$test;
testFilter('input.miff', 'Trim',
  q/ /,
  '30fe8b9fee6480481f3aa4e9931499f6');

++$test;
testFilter('input.miff', 'Wave',
  q/30x30/,
  '4a043e03cbf67ba87815166217a59659');

++$test;
testFilter('input.miff', 'Zoom',
  q/50%/,
  '3d541e271af26574314643698621c4bb');

++$test;

1;
