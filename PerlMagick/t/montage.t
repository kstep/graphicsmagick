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
  '6d646bd25efbd141c9d43e66955d1c438170a90b23a3144f5db8cff4cca90f28');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  'f85b3c93d992de90e3485427f085b7352d369dbce279f853652ec7c6d297401b');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  'fed6156891f6a2001718fc791a34c659e760cfe6969225c6898b1b909ce8b823');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  '665da83be1a58df8112b4977378b0af053a1e960e6691a6215b56f855fb0fd82');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  '63ac61463b45b600d17e8672dc72c46120d36b3ed3db03abded8c14dda65ef41');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  '5da057d9ac9cbff13ae6a0c38db1e71454478d9e7f6e424b5817be18ac9b09a2');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  'faf2a813ddbc835a44bc10756a725bb168192e04b180fbb756df70d4e34d69c3');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  'fc980687733f2aa23dbe8dcaadd51b73d627a728604e8e64e81bd20ca60e6cfa');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  'd67c76109a9a08833190d7eab88ad5f67afa4fed06072b73d00e8906a0ae2f1f');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  '3ddaa12be863f2683154c9bc79a67888e84292cbefb0804cb3195ca53f78a67f');

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
  'f9990aebcfc938bad890ceb491ae9f53781f8cf901ba069c2fceb854dc8d5037',
  'f9990aebcfc938bad890ceb491ae9f53781f8cf901ba069c2fceb854dc8d5037');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  'fdfc629c242ed442716670fb236bd61add2e18433e62d35b9dc3572c2bbabff4',
  'fdfc629c242ed442716670fb236bd61add2e18433e62d35b9dc3572c2bbabff4');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  '82b75bbbd5c5f3a7ce5e4b6e5e25a36c3112bb12ec71734e6b30d7411f41f30a',
  '82b75bbbd5c5f3a7ce5e4b6e5e25a36c3112bb12ec71734e6b30d7411f41f30a');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'a9fe238f40b79dfadea2170df0bc70fb57aa411baaab58415286873dbfafe4f2',
  'a9fe238f40b79dfadea2170df0bc70fb57aa411baaab58415286873dbfafe4f2');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'cd39984ff198710317e5cf0e99b8e399b538c59fac52698ffd4361a555dff618',
  'cd39984ff198710317e5cf0e99b8e399b538c59fac52698ffd4361a555dff618');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'cd39984ff198710317e5cf0e99b8e399b538c59fac52698ffd4361a555dff618',
  'cd39984ff198710317e5cf0e99b8e399b538c59fac52698ffd4361a555dff618');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'ac0ec3cc389e7e3ac559e7b75b5140c99a026c6b05f4485bcfa3ee590f217977',
  'ac0ec3cc389e7e3ac559e7b75b5140c99a026c6b05f4485bcfa3ee590f217977');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
  '77738966d37ba42471ece56353256f2c8e388f79f59368ea7ef20bce3fb57c87');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
  borderwidth=>'0', background=>'gray'/,
  '77738966d37ba42471ece56353256f2c8e388f79f59368ea7ef20bce3fb57c87');
