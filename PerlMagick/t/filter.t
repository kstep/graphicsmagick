#!/usr/local/bin/perl
#
# Test image filter.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..44\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

testFilter('input.miff', 'Blur',
  q/3/,
  '741ee7b9f9f31a4dd2daaf6197692617',
  '6046acf8622ed1bdf5628416cf26ea88');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '9a29118485648b8d1af4190348954103',
  'd786ed89a0fad7a401e200e57629faa8');

++$test;
testFilter('input.miff', 'Chop',
  q/+10+10/,
  'd29da2e9202ebfbaceb18ebd29e47132');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/geometry=>+10+10,pen=>black/,
  'a8f72ee2ed80304df33760c65a14b5e4',
  '12643c99ecf96cf1d7e8f5cc62b13c78');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  '4d2b370daf4cbe131b0753f71023f748',
  '2ffd120c34e437df526227b1dcd83f7d');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  'ade6d7be494f4d3437c7f9a3fa008adc',
  'e4dc79d4309f1c7cded302a52b57614c');

++$test;
testFilter('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  '7b8a78e07c46b64396a2dc9955996c82',
  '78da2d7c584ffd74b41137ea23460abb');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  'd29da2e9202ebfbaceb18ebd29e47132');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '79b2523ae5fb8d6c3219efedfc5c97f2',
  'f4e6ed6a3dd1696e78fc69a761802f01');

++$test;
testFilter('input.miff', 'Edge',
  q/3/,
  'f666933a66ea0ef3ed431cb1a3ba6dcd',
  '89457006d11b4d9ea348d90f2fa75ec7');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  '08f4ee64dcf16fb7ce9be112d535379c',
  '817529f3213a24f68989c3a327cb82a7');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'e7c25176f93a087008197e82104ab3cb',
  '4731d138fa4bf61a12911686b3bebf85');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  '5343a506b26bc6af15c78f1dea9cfdbd');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  '4770e370fb842b770122a16a789f9bbb');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  'ce95aac45517ddbf9ea866c453207c01', 
  '281e5e39e7dc6e13da4506fe2f64f6f7');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  '23a6358f8dfb340e7daf90c2ac991321',
  'e2570162c1ee89f182107c21d5aa0bbd');

++$test;
testFilter('input.miff', 'Implode',
  q/60/,
  '0b86e917d3e117c0ec879d0a16d4cb3d',
  '3199982815d5c078d647fdbe91eda550');

++$test;
testFilter('input.miff', 'Layer',
  q/Red/,
  '855e49d855e2dffc8abdbb78014575e0');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  '066a93e6613bcb143da9eed016ac8121',
  'ad774b9ad972dbc4dca6193ff49f51a3');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/geometry=>+10+10,matte=>Opaque/,
  '91c59ea30eb4f5d9047d42f1d46d79bb',
  '6f55ec5c0a6e1472ea54c4320d3a9030');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '39610223a220856eee90340cfc08ef80',
  '3a7a5363b30f4c493238992a4b6ecfde');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '65f8486a7df15abe362e4252b7294e6a',
  '37f5e04c9ea0c788680c4353ed1d4060');

++$test;
testFilter('input.miff', 'Negate',
  q/0/,
  '7c1ca362ad2afc2eb11d61bc08e48026');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  '3874fb4ca728b2e4bdf020a6b00d47b0',
  '72cee3dd1622629f43093cb02fcdac43');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  'eaa4da0ea1326b7d553d5f2c9d1957d8',
  'e16d40c46206f6c3dc682139de76f76d');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  'eb4d6e084afe2835a1ad28ad7fc12ced');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '8ff5790c89425da8ddca2647526b7ab9',
  'd51eec15134fee4bfcdb732b6d706a17');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '0cbe112e62a8e8a6e178a55a56b69a4b',
  '1bc9ae340eef6cc6257b0e2bfb5a0ab0');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '5b5165d9bfda88340e0095ca4ac190dc');

++$test;
testFilter('input.miff', 'Rotate',
  q/45/,
  '1beb08b2ee50740ec51ecbf919926ac3',
  'c31868569301db9e41f46bc690107c36');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '602eb9131754c2467f87a8a4ddd21cb7');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  'fd9f86e0fb31bc2fd67aa43084e64e24',
  'f37f5d4ca4ac9e243cedb3abf35758ac');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '441f25c2c60c224061352f546b6ddff3');

++$test;
testFilter('input.miff', 'Shade',
  q/30x30/,
  'b306584435d209023ced321daee65af7',
  'f958c014337453f741ffcbea791005ed');

++$test;
testFilter('input.miff', 'Sharpen',
  q/3/,
  '087e8130210a07139b7fcb9d0b7d5eff',
  'd2a0063811738be757a6d427499a6187');

++$test;
testFilter('input.miff', 'Shear',
  q/45x45/,
  '5f88bff0dccf6533796750f024aa7222',
  '7dbbe302b5a4daeef1105766c40baf48');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '2c67db3c251365114d229d5c0a36362b');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  '80d5a065d0f040d509c4f5a81d0d4b5b',
  '23da17c30a2277cd35203bb9fd6fd790');

++$test;
testFilter('input.miff', 'Texture',
  q/filename=>granite:/,
  'eb4d6e084afe2835a1ad28ad7fc12ced');

++$test;
testFilter('input.miff', 'Transform',
  q/geometry=>50x60/,
  'eb4d6e084afe2835a1ad28ad7fc12ced');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  '32730cfe0d9939d04049cd90b2acdb2d',
  'caa7ebca666f176a8f4d4a2f012e2236');

++$test;
testFilter('input.miff', 'Trim',
  q/ /,
  'eb4d6e084afe2835a1ad28ad7fc12ced');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  '220ead97541447e9d10643724c26bce7',
  '938819868d883a2fca7f8395445b66e5');

++$test;
testFilter('input.miff', 'Zoom',
  q/geometry=>50%,filter=>Point/,
  '7f2ece454edfd8555c4f071a87547eb4',
  '9e78d6fcaf019530500dff7078a23efc');

++$test;

1;
