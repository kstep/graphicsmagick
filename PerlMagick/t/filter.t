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
  'f42939fc9f2d3e51541a2df134e21ae2');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '177787895bce2b8590c3328184037457');

++$test;
testFilter('input.miff', 'Charcoal',
  q/60/,
  '731d90ca7cab0d848e52abb1e499080e');

++$test;
testFilter('input.miff', 'Chop',
  q/+10+10/,
  '5774965192a6189e545271bc7d227c74');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  '46bbff3554df0b30564ac6d8bcc9d43e');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '5774965192a6189e545271bc7d227c74');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  '886a1c8a4fbb4007760ff3656baaef44');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  'afba119232cacaf81bfcb3520a7e75e7');

++$test;
testFilter('input.miff', 'Edge',
  q/60/,
  '3ec9af687a9b28577cf1cbc2efde8815');

++$test;
testFilter('input.miff', 'Emboss',
  q/ /,
  'a5149d1d362c7fd6fccc1b23e16f16db');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  'fff20dd9a90d802f8761f56570ba1688');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  'db681da3fcc75975787e1e66a3208cb9');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  'b96ee24e7d275d8c6e923248593a88f5');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  '352b86e60dd345799fa966e4ef2a1633');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'b14fe6e908ca45710755de7a6affa589');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  'cf340bbd349721cb5bd00c02a3635a66');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '4ae442b6d09a08ac8d0f8b54e4b76de6');

++$test;
testFilter('input.miff', 'Negate',
  q/0/,
  'bbc1fd54b682d6a9bf98df23e28a5d29');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  '0049094b77a0666c02f22270a3c4708f');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  'c27bcf1d11ef52baf95a99132ef772c1');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '89fd3dcdd9283e4c7ee9ba88f3065900');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '9b281bda22fc88b14d322051139ced96');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '6cc829ed34e8514eea3c755e8f006eb5');

++$test;
testFilter('input.miff', 'Rotate',
  q/45/,
  '83ded92b9643c6510cd414b03a578e9e');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '79dee73aa8f751bf089e159654f4f8df');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  '4a970cbd6977ccd3fb3c3c290863f597');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '4f7188f831b9bfe731b41e1ce5a4b3d0');

++$test;
testFilter('input.miff', 'Shade',
  q/30x30/,
  'c819b1a977a652491691b67f915563bc');

++$test;
testFilter('input.miff', 'Shear',
  q/45x45/,
  '40ec38f0ac85cec9fef13bc36527e931');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '1ca2093a9a65e152bed48414131d180b');

++$test;
testFilter('input.miff', 'Transform',
  q/geometry=>50%60/,
  '9d9140904fe18aec2bf1e0775c7735bd');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  'b095405aa47d64dfe00bb14cd49b7c97');

++$test;
testFilter('input.miff', 'Trim',
  q/ /,
  '25c550da420eeb782f57dd88c689e819');

++$test;
testFilter('input.miff', 'Wave',
  q/30x30/,
  '27c9a93e069c693242490938b7fbbbcc');

++$test;
testFilter('input.miff', 'Zoom',
  q/50%/,
  '79dee73aa8f751bf089e159654f4f8df');

++$test;

1;
