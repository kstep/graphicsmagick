#!/usr/local/bin/perl
#
# Test image filter.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..43\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

testFilter('input.miff', 'Border',
  q/#ccc/,
  '36bc2833b05ad1e4e2cace027e045d9d');

++$test;
testFilter('input.miff', 'Blur',
  q/60/,
  'a1af7b4d08186a26ed9e01738b568051');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '60b20d26fbf98095cc994c21d013d42e');

++$test;
testFilter('input.miff', 'Chop',
  q/+10+10/,
  '92fd46db3668d557b12c30fc810b1fe6');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/geometry=>+10+10,pen=>black/,
  'a51dc2d20c36c8e4df18afc567db636e');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  'd61bcae2abd48b9a8cb789f054c609e8');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  '60e07b732b6cd84177e0ba22a8aa4106');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '92fd46db3668d557b12c30fc810b1fe6');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '9166d0fff05dc5805eef1275863ec244');

++$test;
testFilter('input.miff', 'Edge',
  q/60/,
  '60c956dcc6cd627cdd583a6d3c3686dc');

++$test;
testFilter('input.miff', 'Emboss',
  q/ /,
  '3bd50587263c86a4229dda55b3c80e16');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'c484e3f1b74e35e911f514ad465451df');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  'cac8dfaf79af279c6b1f37a495e8552f');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  '64ce85ac3a977b2ddbeebb12690965c6');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  '7558cc1e69972d2e0d9272b9e94dbaf6');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  'b5f821615ecb6f3856fef4a6939d9ad1');

++$test;
testFilter('input.miff', 'Implode',
  q/60/,
  '3d7e8331469ab7afbbeeb1ef39f61581');

++$test;
testFilter('input.miff', 'Layer',
  q/Red/,
  'bf8f741ba7273610138b8e0ef5b5f79b');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  '107ef834d2f53864c021a4801d67adee');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/geometry=>+10+10,matte=>Opaque/,
  'd7c89e6b946d94187dc34c58d4c5e2f1');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '8b31e05a3a8710cf9864e7de0567fbec');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  'e9b8baed761cc95bd60ba1b6272bad2b');

++$test;
testFilter('input.miff', 'Negate',
  q/0/,
  'e46a038426f4de5a3f051316aee46046');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  '352b86fce0e86dd83c326d257820be9c');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  '7bfd698305d42711246edfa48f6aa9f9');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  '6444b7dae412d1d441b3ff8278e874d3');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  'f4696fd19e5c100a50381235c86689cc');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '1e5e69f5c89a366cead97e4c71b97dc5');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '6d1f073f2a9c2c7bb156e25c4436e691');

++$test;
testFilter('input.miff', 'Rotate',
  q/45/,
  'd8227927877e43f75c517bea4b0a9720');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '3d541e271af26574314643698621c4bb');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  'ab8ba5119e2979ba7ba76d4a6d944b34');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '02179931bd9f63a64883c210ba52641e');

++$test;
testFilter('input.miff', 'Shade',
  q/30x30/,
  '3a54f6cfe29c02f5044796a9af527f17');

++$test;
testFilter('input.miff', 'Shear',
  q/45x45/,
  '9bb77fc01cf42c37f9e2c4f130375701');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '2594080986a34434961fe5841ae1bebc');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  '099d55d589ff2a7fd6e970cca743818b');

++$test;
testFilter('input.miff', 'Texture',
  q/filename=>granite:/,
  '7785d161ea66bd39394bb74ef704a0aa');

++$test;
testFilter('input.miff', 'Transform',
  q/geometry=>50x60/,
  '7785d161ea66bd39394bb74ef704a0aa');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  'ad5daf059a1f4dcd132f8aeed878611d');

++$test;
testFilter('input.miff', 'Trim',
  q/ /,
  '7785d161ea66bd39394bb74ef704a0aa');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  'f1408829aaa27452850131e22f222ab6');

++$test;
testFilter('input.miff', 'Zoom',
  q/geometry=>50%,filter=>Point/,
  'f9a5ccced93db546404214742d5329d2');

++$test;

1;
