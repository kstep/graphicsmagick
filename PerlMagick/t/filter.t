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
  'd5731e36e03b9e408280a85fca37c09c');

++$test;
testFilter('input.miff', 'Blur',
  q/60/,
  '8b903e0b804825a294258f98f957ad36',
  '52f7685c94e7d22abe789127e15f9e6d');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '48ea19579994e852415de2b83d31c080');

++$test;
testFilter('input.miff', 'Chop',
  q/+10+10/,
  '9904c07819014a4d9f6c0f4c5e650b08');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/geometry=>+10+10,pen=>black/,
  '4af3b2e075157f4493f8980ec3b9612f',
  '12643c99ecf96cf1d7e8f5cc62b13c78');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  '2ffd120c34e437df526227b1dcd83f7d');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  '5bb2fd58bf4f1c7c956f6f87553aec2c',
  '8f0ab7b7117ffbbf380e8493d4399aba');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '9904c07819014a4d9f6c0f4c5e650b08');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '1594e4794aba1342f205a81d7d7b9d25',
  'a0648ef96fc3a4bb20daf366d96659c6');

++$test;
testFilter('input.miff', 'Edge',
  q/60/,
  'efababe350b4cb8b0e08423fcdf70895',
  '9b85d0feacb3d62aaa9851b1a23a91bf');

++$test;
testFilter('input.miff', 'Emboss',
  q/ /,
  '53d333238024e412189bf0e43b585504',
  'cab0f35d59cd03d7c230df9c29c73a5e');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  '72407ad6aa24d2be2489a4a0890762c6',
  '658fdb444984fd0505e22649d051d8a9');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  '39804bd87007de5f1340d104425f43fe');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  'cc058a59cac9ccd9c3fa63372b26bbc2');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  'd59fe165d2d7880fbb22c09b7d950cef', 
  '97adf83222e170fa289fa72cefc289e2');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  '64b0b4b0cc7be6e81da8286c4abcd759',
  '01b56fba77fa4702291810c25973f21b');

++$test;
testFilter('input.miff', 'Implode',
  q/60/,
  'bdc8fe58816364d52c1d5c3d2f97910e',
  'e7fec4cabebfea874667181cf3a7626e');

++$test;
testFilter('input.miff', 'Layer',
  q/Red/,
  '20b3b7d6e137f0814d770e336fe444a6');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  '184bc6123d35da7ce55cd797b1a242dd',
  '4ef0c8da5a7bfc711c728eb0ea9ba851');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/geometry=>+10+10,matte=>Opaque/,
  '43c99602c51d08df9f6e57ea4d710603',
  '6f55ec5c0a6e1472ea54c4320d3a9030');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '177e696d6b5fd0746b4eeafca8994e36',
  '43c9cff509bd15579c00e466694b85de');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '8942e7e11eb93d8ac4f95ba269ec3557',
  '626f106b1302506ad361b73754ff59ab');

++$test;
testFilter('input.miff', 'Negate',
  q/0/,
  '8aee54346f25ff6fd2bda4efab917a4d');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  '94973214e269266f2fa95c93a45aad3e',
  'efb45f15687cd2b4656d203a21b353f1');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  '93872823e517ba0562fcdcf954bd4a24',
  '355b61e9f3d2210cf753a89f849686bc');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  '96a79b61c080cbedc168200bee1202f1');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '30bfc8ca3cdafb088afde37caf5bb64a',
  'a456dbfe7d80321199b54bedc0aa5e38');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  'b7bdafcac55883e6b4deacb5b3aa8bc6',
  '367626c7ba7248132248cb7fe473debb');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '5a1fa84d03fb6d74221f63dc6bcbc027');

++$test;
testFilter('input.miff', 'Rotate',
  q/45/,
  'a18c8851587b626cf991c7d83e872174',
  'd22d74f973f8ce45a78f6f8c1e4e7b7c');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '2c2fa7ff5fa9dd97218df555b75487e9');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  '4ca074949b56261a96b5f7df8d3b2029',
  '1f3087d4eb6c8407b37d8eae742342f9');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '1d9bb517f5b9584f2f1ee78075b92219');

++$test;
testFilter('input.miff', 'Shade',
  q/30x30/,
  '883d2876639db90581044138c986fb0c',
  '851f346525a2de45605ce967abe70b05');

++$test;
testFilter('input.miff', 'Shear',
  q/45x45/,
  '544995ff24ce4a93ca301bab6ec19645',
  '663c5d969418e1b759a84a5b3545543f');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '6dcb8bbce5436b5f11c6ed0e80ee2c69');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  '93136a8e601741575c3d814e0c3dba12',
  'e4ee48cf7869a973cedee59e56d704df');

++$test;
testFilter('input.miff', 'Texture',
  q/filename=>granite:/,
  '12643c99ecf96cf1d7e8f5cc62b13c78');

++$test;
testFilter('input.miff', 'Transform',
  q/geometry=>50x60/,
  '12643c99ecf96cf1d7e8f5cc62b13c78');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  '137bfacdec4b7a8dba0abe31e43db0f2',
  'caa7ebca666f176a8f4d4a2f012e2236');

++$test;
testFilter('input.miff', 'Trim',
  q/ /,
  '12643c99ecf96cf1d7e8f5cc62b13c78');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  '80ec1412b1c711b94fee56aa9d1c3ead');

++$test;
testFilter('input.miff', 'Zoom',
  q/geometry=>50%,filter=>Point/,
  'c5089c568e0024f7c9b42fa3ab5fe121',
  '85e37abae62fc538cd4fe2fb3797ec77');

++$test;

1;
