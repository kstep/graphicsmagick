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
  'cee332de1fe5a7dc4bbb0ec4cd0e357f96be62dcd4742fb594f375c62dc7d0f1');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  'f89ae3a8f4a91be550890137dcb804317696fae4c18fa697fa40c3446d6fe35b');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  'ea4bfcb24282bf5aa716b022c6d2d890a382196c9437f3d5c9f8dd05555b7068');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  '718af54fd99a1a7df593f5978c509195de698f3d79948833ac4d621ffa115f35');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  'c2cac47798aa2c9be9fa5dbc9ce3a3d42968300c22d8c78a0e190391d5daa5de');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  '5b10c7c4517974eeef470f43d3d3af5cf6dfd46dcb532c934092a8b28e4b9ac4');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  '71a3023f80f25cddec4bb34a54c7fc93259f1a9d944c3b8aeb7eabd3be812936');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  '8d54bc3354827d4257c323d1539a457c42d1340f6ed8b89b8c2896c341024bbb');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  '8c0ac0891186dd1a903547f1088f8e2a8d60c970b41658bfb5bac5729c8f3d4c');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  '018f6bf49dbac9ccb19fecc0f6b149b62a51c019199148f6b18b9d433e256e82');

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
  '716c6cdfb75b3ce0f45b0c3ad8ac6bf16870b44829dafb7a88411f0f2eb17725',
  '53f044bcdb8aa13a07c524df32bb6ca394360126a4957e41600d11fadbfd6d52');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  'dec374514ce6a49ff384f0d9c4efd6c2796e94e5902929f91aad9457b963f198',
  '27955d81e511b0ba2d0b1695ecadf2826f7372a5d0101914f7e7e5d14ad30a95');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  'dfdbd5ddd1d48484822160c22ae3327b9fc068415b5fdf992ee2fe5b7581389e',
  'd940d0d9b35b8900ca5265078020ee25dfad693bf6548100219eb3351ff746e2');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'fd7ad72249c7fbd3dff4942d916edc5b5d7211827196a6d50c717212de95961c',
  'a73039fa7cf86a10af62696014e56a41dbbec25d4e9cb90592125c3ba89994f8');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'cea55f11c06a780b82e4125ee5f3105169f7b6bcbf24742e8a4a6c06f62ef327',
  '1ccb942f72745ef25f284f8905091580387b89f2efe9215265737af173b5d372');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '16ecaa37dbeb88ebfbf17ebc216e5d0700b8c7926338b6f5086d7b9989bcc42b',
  '69e6a8ed56a2c09404c91c039916678ce73d17f392ae380db6912497751de01c');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'b60e513f1f90718ab5530f7902e58a1628f33598653131f584415c52b5ee1719',
  '2191caf6179dd98bb6e7546c34b5b3d2e86929f76d6b01d98f5b89352376d94e');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
  'aa24b5ea6f3775fd8f115730fc2ece8a9c6c7bccd2ae835caa6778c6c4db8e2f',
  '94f72cf702e54c52eda26f461fa7bdd541cbddfdca74d2619b12f1c308b0c37d');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
  borderwidth=>'0', background=>'gray'/,
  'bae96cb788c8c5e231945548279605642560708bfca7ddd255a6ab08302031c5',
  'e3aa9c4c515bd8ddcc7dae52d608da62c20dbe0b30d3ae5c948a113401630eaf');
