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
  '7b63f3d3f0bad2fec1c5df948683df21a542df04a944b7cba46f897e4284af8a');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  '6b8c66a9fd27feea291c60456bdf4193b877ab532cb8437906fe48493e511477');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  'a193d8a3adb7e22eb0242deea786e90257b30c86ceb84fcf8e906282dc27af18');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  '53988447fa24f34bba94325431b3466bb5d95899e3e96bbc4958e413fac1bc07');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  '476bca9ee76273dea0f5119e6285a5723c16218a64e9a86b82109d6fb1243b33');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  '93d6763f385881aeb84e4feb92b7d9c77ea23cdbaf141df7a228a985130f77ec');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  'afbaee72c580f31ad02d1fdec55ccb8b8165f18299b6a377c574bea9f9f362b5');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  '6792c62fea5c93e7a74a76f96e09b6ffd3284ef750009d4524f47c62a59004b5');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  '03e2fcb8b0e447dbe294598bc8f267385133244a010e66c849a6cc9560035e90');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  '8851434def5ece352fc5fe830871ea2a7f6608fe50eef6abaf4e4b0bccacd360');

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
  '780481bfe04c2057b838c7d20e96b73a3efd576e3693856a7a17c2bd0d0c27d1');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  'dec374514ce6a49ff384f0d9c4efd6c2796e94e5902929f91aad9457b963f198',
  '305d256ebd57078fcfc8159156100b6f11fdeedf8673b82144caaa7dd97c8ca4');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  'dfdbd5ddd1d48484822160c22ae3327b9fc068415b5fdf992ee2fe5b7581389e',
  '1fe53d94e8e07b121c037a089ad27dd5f8b4651cb6abf2efb0773281dddfff9b');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'Unframe',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'e8c78e09b2bc842b57470332659b6ca12dd5a6d0224f3109020346f33ae6139f',
  '87ff711946f98edc0b38d8685a01339f47aa80bebdbfc0787b31350db79c8ebb');

#
# 15) Test Bordered, Un-framed Montage (mode=>'Unframe')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'Unframe',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'd369869860ec335d43f4632d5dc6fa91077b9871a57df25cba03b044682108c5',
  'a3ea40117f37ff2a68772aaa1dd7c41cc81bd21179cbd19d193ebfb5381fed92');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '16ecaa37dbeb88ebfbf17ebc216e5d0700b8c7926338b6f5086d7b9989bcc42b',
  'a3ea40117f37ff2a68772aaa1dd7c41cc81bd21179cbd19d193ebfb5381fed92');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', mode=>'Unframe',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'b60e513f1f90718ab5530f7902e58a1628f33598653131f584415c52b5ee1719',
  'fe71f67323d14c1689216cf96b9977dd208f137592dd9708da001a41225e079a');

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
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', mode=>'Unframe', shadow=>'False',
  borderwidth=>'0', background=>'gray'/,
  'bae96cb788c8c5e231945548279605642560708bfca7ddd255a6ab08302031c5',
  '94f72cf702e54c52eda26f461fa7bdd541cbddfdca74d2619b12f1c308b0c37d');
