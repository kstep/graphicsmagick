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
  'a1af7b4d08186a26ed9e01738b568051',
  '4feca1e68d3036db3997310a63231b51');

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
  '60e07b732b6cd84177e0ba22a8aa4106',
  'e7385ab2db1c1aee9ae33dbab44daebb');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '92fd46db3668d557b12c30fc810b1fe6');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '9166d0fff05dc5805eef1275863ec244',
  '07ac0065b7c5aa0826c721faaa462a2c');

++$test;
testFilter('input.miff', 'Edge',
  q/60/,
  '3e8c5c72a869f1158141bf0da0a2e25c',
  '18df2894f1ed7eab251c3d89872fa4cf');

++$test;
testFilter('input.miff', 'Emboss',
  q/ /,
  '3bd50587263c86a4229dda55b3c80e16',
  '13369e7bc6da29962fc73834305e454b');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'c484e3f1b74e35e911f514ad465451df',
  '2f56b395d3b86f5f5e0d2df4a9037364');

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
  '7558cc1e69972d2e0d9272b9e94dbaf6',
  'aa803690f5e4f940d4a9e1b14896d38c');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  'b5f821615ecb6f3856fef4a6939d9ad1',
  '16f54dd1ea09d26b4a240f8c20be42e0');

++$test;
testFilter('input.miff', 'Implode',
  q/60/,
  '3d7e8331469ab7afbbeeb1ef39f61581',
  '9b1a3972baad96b958f68e3e36867fe7');

++$test;
testFilter('input.miff', 'Layer',
  q/Red/,
  'bf8f741ba7273610138b8e0ef5b5f79b');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'c71947bf22503d1e06f21213a0e0cb3b',
  'b276d893a0ba26f924e493c8d1339fec');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/geometry=>+10+10,matte=>Opaque/,
  'd7c89e6b946d94187dc34c58d4c5e2f1');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '8b31e05a3a8710cf9864e7de0567fbec',
  '53c73ef4da2f601052ca8a08fbf255b4');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  'e9b8baed761cc95bd60ba1b6272bad2b',
  'ab1ca0d1a91337bb0f8c96d40fcd00b9');

++$test;
testFilter('input.miff', 'Negate',
  q/0/,
  'e46a038426f4de5a3f051316aee46046');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  '352b86fce0e86dd83c326d257820be9c',
  '734077e250b645a3fa05ef33085b6c56');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  '7bfd698305d42711246edfa48f6aa9f9',
  '2a4603d9d93d29cf463fc24d218d66df');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  '6444b7dae412d1d441b3ff8278e874d3');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  'f4696fd19e5c100a50381235c86689cc',
  'f88964db06cde78987d0fafdf63040e3');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '1e5e69f5c89a366cead97e4c71b97dc5',
  '098fb2e5b76f2eb1a12bc03f7df2d3d5');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '6d1f073f2a9c2c7bb156e25c4436e691');

++$test;
testFilter('input.miff', 'Rotate',
  q/45/,
  'd8227927877e43f75c517bea4b0a9720',
  '2be65e679ddc401a8994400114078bf4');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '3d541e271af26574314643698621c4bb');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  'ed70966b7e264bcc36f3813d6de07f4f',
  '13c229dc92426097f74443c945142b0f');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '02179931bd9f63a64883c210ba52641e');

++$test;
testFilter('input.miff', 'Shade',
  q/30x30/,
  'fa6ae11e0b14697429fa5261396ee7fd',
  '93168809b907d270554533830a4cbd62');

++$test;
testFilter('input.miff', 'Shear',
  q/45x45/,
  '9bb77fc01cf42c37f9e2c4f130375701',
  'f1493545b68b868d21502d27d2b112c5');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '2594080986a34434961fe5841ae1bebc');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  '099d55d589ff2a7fd6e970cca743818b',
  'df3ba375840a79e33d9358d703f69a8d');

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
  'ad5daf059a1f4dcd132f8aeed878611d',
  '466d22c8ed1f73d913359ac063cc9317');

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
  'f9a5ccced93db546404214742d5329d2',
  'ff9c22952d8df931e62b81c4f8b44c37');

++$test;

1;
