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
             '519392084cb0b1bc9e930a0d4e5054d0');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
             'e17edf72967802c96d1b5790342e7c2f');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
             '23d93c27fd9c3d001058a4a96ba5b81e');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
             '6c44f7cb1039f9f6a3c75768c8590e26');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
             '44e40ab252d1cfdf1c4041f2e6fabc08');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
             '5f824b6b4fa38641926ec7e081ee8886');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
             'c118f7182ff28a321e6c5d7008f7e146');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
             '5129a9569218161632e5a211cd351489');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
             'e687ea6af4542c244506ac77d42092f2');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
             '0acaebf38de97268e084a497b8d9dcb1');

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
	     'b9be068a39143613b03c8dc8178d03c6',
             '60ea9340bb48b5ea1be1096fec70b533');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
               borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
	     '5fe2b72ff6a4c092d1a4166a9535f489',
             '30e961a27827c6b244e4f489bfbf7764');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
               borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
	     'ddf8eb5d270af94b0c55635b9d05a5b8',
             '0d5959c27c6b42bd660377c55e2963c5');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
	       borderwidth=>'0', gravity=>'Center', background=>'gray'/,
             'cd33564b909282094bdbd1354b6b6ed4',
             '5ffe284b3e9716c21283c76469eaa38c');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
	       borderwidth=>'5', gravity=>'Center', background=>'gray'/,
             '93d223ec6dd59eeb7899f50ed781d2c0',
             '22e16d6a3c71a8fc2d708629432ec7af');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
	     q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
	       borderwidth=>'5', gravity=>'Center', background=>'gray'/,
             '93d223ec6dd59eeb7899f50ed781d2c0',
             '22e16d6a3c71a8fc2d708629432ec7af');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
	       borderwidth=>'0', gravity=>'Center', background=>'gray'/,
             'e79c6e3b16924e7942494544021e8e5e',
             'cdef571727216a0a1e1cfff5bfa4ca4d');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
             '6162200344ec5ad9808ea5002d9a04a5');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
	       borderwidth=>'0', background=>'gray'/,
             '6162200344ec5ad9808ea5002d9a04a5');
