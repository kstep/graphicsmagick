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

testFilter('input.miff', 'Blur',
  q/0.0x1.0/,
  '8457df30246e14ce6441b4252554c4e6',
  '5e3a850beaeed6782b3ee9618ada5104');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '9a29118485648b8d1af4190348954103',
  'd786ed89a0fad7a401e200e57629faa8');

++$test;
testFilter('input.miff', 'Channel',
  q/Red/,
  '855e49d855e2dffc8abdbb78014575e0');

++$test;
testFilter('input.miff', 'Chop',
  q/+10+10/,
  'd29da2e9202ebfbaceb18ebd29e47132');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/+25+45/,
  '09c82f6759682ee2adbd09571edfb34f',
  '12643c99ecf96cf1d7e8f5cc62b13c78');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  '62907fa59892a6241e81fa10cd513ef2',
  '2ffd120c34e437df526227b1dcd83f7d');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  'c2156d2134de57d98a03fe8ffdf0a3cd',
  'e4d90d8d2af844650864c4e193acb270');

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
  'cb968877e88b67b28a15da3796698eb0',
  '89457006d11b4d9ea348d90f2fa75ec7');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  '83642cd00e36ca29fa4ddee858cc5e80',
  '5aa0ab5936b49e14d3e9b58caa6a1861');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'e7c25176f93a087008197e82104ab3cb',
  '61eb62ac349952e079237eab1b26302d');

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
  '026f10a0cd41b8b61e31b27f3adb6d71', 
  '1c36dcdff27bd8618598def99f5227d0');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  '23a6358f8dfb340e7daf90c2ac991321',
  'e2570162c1ee89f182107c21d5aa0bbd');

++$test;
testFilter('input.miff', 'Implode',
  q/30/,
  '595ba49fdaa2052c97ad604fefd2d89e',
  'a8e08e25ea60706c37b264edfdc9bd99');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  '066a93e6613bcb143da9eed016ac8121',
  'ad774b9ad972dbc4dca6193ff49f51a3');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/+25+45/,
  '875628edd6e4c05889ddfb279d05dcd0',
  '6f55ec5c0a6e1472ea54c4320d3a9030');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '39610223a220856eee90340cfc08ef80',
  '3a7a5363b30f4c493238992a4b6ecfde');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '1c4736b6d1ed28e1846ab1f47c792ef4',
  'cfc9efc540fe048eefd92ce743c0630e');

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
  '0834dd50453a59049c380bc506ccb089',
  '274418a706b9fee352f7ba148a8ee419');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  'eb4d6e084afe2835a1ad28ad7fc12ced');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '0fc1f19d9f2928defce16394bc4e437a',
  'd51eec15134fee4bfcdb732b6d706a17');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '0cbe112e62a8e8a6e178a55a56b69a4b',
  '1bc9ae340eef6cc6257b0e2bfb5a0ab0');

++$test;
testFilter('input.miff', 'Resize',
  q/50%/,
  'b3a770742494edc995abc2b6e6a7faec',
  'af8c0f254cc0ff3dd259442accbe4d40');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '5b5165d9bfda88340e0095ca4ac190dc');

++$test;
testFilter('input.miff', 'Rotate',
  q/10/,
  '243796938cdf86a0b00a49651f2e2a08',
  'e94be2d9fb268a78b31182aed1c70597');

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
  q/30/,
  '390101e0acbfdf026e9d48e0e5f54c35',
  'c322c40dd3d91dd9086d5867943d2d06');

++$test;
testFilter('input.miff', 'Sharpen',
  q/0.0x1.0/,
  '1e1d678e448d7b2aac7cd0cee6ff1eae',
  'b9614f4892a7839f1eb58c8f5f6a2978');

++$test;
testFilter('input.miff', 'Shear',
  q/10/,
  'd9c9229c08a62444db7ccbd487de4b16',
  '64c4aa4cd14cfa11e357ec880bc818b8');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '2c67db3c251365114d229d5c0a36362b');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  '5430146c239914446bbe1fbffe1a0fd2',
  '9ddffdee2a607ce5e0f2d92561e977d0');

++$test;
testFilter('input.miff', 'Texture',
  q/granite:/,
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
  '48ce152f3e1eb66556dce68fb3240136',
  '5b8f05f6c6162860ef080faf89e34187');

++$test;

1;
