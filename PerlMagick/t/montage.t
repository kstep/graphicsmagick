#!/usr/local/bin/perl
#
# Test montage method.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..19\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

#
# 1) Test montage defaults (except no label that requires an exact font)
#
testMontage( q//,
  q/background=>'#696e7e', label=>''/,
  '9570106438c91baaf7e31996e0eec131e3746a4c560603a0acd774c71531edc0');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  'ef76a465840be1e679ad5b0f552910b5cb94f97f9a2931f967059d13282806aa');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  '639bdf816a73e677194ca95a01d1e7a07c308c772b52dba7156fa53eceacd928');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  'c207e5278f87dfb6036e22d1b4dc7acd7a5b8e5dc0c119e59fd85ad725e1d573');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  '8f0a58a7e7aef7db00e1017fdcef5b9870c1464a9d9bd4cf3fd98107f4a0854d');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  '8d94d447f637e65513adb208ffdd37ca202f1b482676f1026c85ea9c2403916d');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  '21e8b2db282efa561e5b3614d408971d6f2c19b737c48bb9bd4f3b1cea7cb60c');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  '885694475dd77a6d31a1c35854e0a86ff2c3d889b89be7e3270d0d29ad4eff1c');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  '06ba0059d2442d956c44e71b42d91f024c449dd0dac1abb5616e8fced6fdf9e4');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  '1ef1cb1e0a09eb5a6b81518987b385cb7d63c2b18b46878d54c6fa6934173d31');

#
# 11) Test Framed Montage
#
# Image border color 'bordercolor' controls frame background color
# Image matte color 'mattecolor' controls frame color
# Image pen color 'pen' controls label text foreground color
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+3+3>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  '111d56103b135401d223a89f36177b07df884183c64cd3e08354d51d1baa6a5a',
  '111d56103b135401d223a89f36177b07df884183c64cd3e08354d51d1baa6a5a');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  '7484a6a5e2a68714fa9eff1437ce765703da32cd1f7786d46efb1a6079ede01a',
  '7484a6a5e2a68714fa9eff1437ce765703da32cd1f7786d46efb1a6079ede01a');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  '8e3f467be000d2a597aa80d93ef8e164db68103fb251f3c54c35c78ae0c6530d',
  '8e3f467be000d2a597aa80d93ef8e164db68103fb251f3c54c35c78ae0c6530d');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  '8e4f784a84585fb8f11f8cc2c2348b674f069b4cd5d936a03b9380102484c840',
  '8e4f784a84585fb8f11f8cc2c2348b674f069b4cd5d936a03b9380102484c840');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '9c97bee82720d7b15ece157002cdc24bfec60a3193b1f0bf58e0485f89327c97',
  '9c97bee82720d7b15ece157002cdc24bfec60a3193b1f0bf58e0485f89327c97');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '93d223ec6dd59eeb7899f50ed781d2c0',
  '9c97bee82720d7b15ece157002cdc24bfec60a3193b1f0bf58e0485f89327c97');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  '9e79abf622d32730bfacf2fe63ecf8191ea1fc859a1ddd4e452bb55e70e1d675',
  '9e79abf622d32730bfacf2fe63ecf8191ea1fc859a1ddd4e452bb55e70e1d675');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
  'b47082e977b3978a1493895d8aeeff1fdc1b43bb647a98744c9f6a10ef3ff5ee');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
  borderwidth=>'0', background=>'gray'/,
  'b47082e977b3978a1493895d8aeeff1fdc1b43bb647a98744c9f6a10ef3ff5ee');
