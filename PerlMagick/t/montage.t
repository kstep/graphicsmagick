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
	     'ff82ce564b529b56992502e06d415ad6');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
	     '1a71f5f297124da50c2f6ba1921cfaf9');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
	     '6a4411966db53ad8171c6ff90f36bb75');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
	     'dd003b8697ac5192a476a0cf7fbbacb0');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
	     'c6623cbae7aca21b9cec86c424780c25');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
	     '03896e5d08f7157d0fa911a4d2a8d52c');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
	     '5c33f52973ca90986ed39b212e8f966c');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
	     'a3881e319ea144b631af2afdde2bb672');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
	     '88c37b0f2ea24a133e4e2c66f388e9e4');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
	     '38b6243fe1742332900005263021e34d');

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
	     '7cc952ab820d9922e6ab1f56328b41bd');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
               borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
	     'f369deba22d6601b52f1355928b0ff86');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
               borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
	     '0959a117426bad77c3138ae84283c2b0');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
	       borderwidth=>'0', gravity=>'Center', background=>'gray'/,
	     'f73c3cfd0b79924dd9c9bd28080a4f9a');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
	       borderwidth=>'5', gravity=>'Center', background=>'gray'/,
	     'ae5a2442f2e7d5a164dfa8e248d1f1e1');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
	     q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
	       borderwidth=>'5', gravity=>'Center', background=>'gray'/,
	     'ae5a2442f2e7d5a164dfa8e248d1f1e1');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
	       borderwidth=>'0', gravity=>'Center', background=>'gray'/,
	     'e0af842865d4b474864827aad48a02ba');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
	     '0300b9160c3ba398c6a3d475cc69820f');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
	       borderwidth=>'0', background=>'gray'/,
	     '0300b9160c3ba398c6a3d475cc69820f');
