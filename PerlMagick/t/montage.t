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
             '315efbf3f4dcbd165c20f65ec42d7656');

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
	     'b7ee500a49e6da32ab68d0fcce6dd634',
             '22b9f14c5f34f64640156cda9f425b12');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
               borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
	     '029336aea21abccde27978ecebeb9a10',
             'e1933b39fb865cfb7c9d16d277874254');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
               borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
	     '3056501397bfdcc083d72f87e9098f49',
             'b26c68cab49bd2aad619b1ac1f2366af');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
	       borderwidth=>'0', gravity=>'Center', background=>'gray'/,
             'cd33564b909282094bdbd1354b6b6ed4',
             '2a2af73dc31487f7cd23d0d95d036dc1');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
	       borderwidth=>'5', gravity=>'Center', background=>'gray'/,
             '93d223ec6dd59eeb7899f50ed781d2c0',
             '06894df56df2d77a26641bb7cf1d2a81');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
	     q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
	       borderwidth=>'5', gravity=>'Center', background=>'gray'/,
             '93d223ec6dd59eeb7899f50ed781d2c0',
             '06894df56df2d77a26641bb7cf1d2a81');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
	       borderwidth=>'0', gravity=>'Center', background=>'gray'/,
             'e79c6e3b16924e7942494544021e8e5e',
             '371d4c587bfd53d63ebeca6dd32a18a1');

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
