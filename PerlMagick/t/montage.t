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
	     'c048f9060660bf671f1ce96b0a6d13bc');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
	     '773c76f377240779d57693b1e061c2e1');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
	     'ba111240a6b0e03b4f3502cff715933b');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
	     '61c6c5ec44ca7bbe7d63ae16579a1cb0');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
	     '3a042eec453f5cfe77289f9639000990');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
	     '1ba4b9c080fcc9ac5fa075c35b85a8a4');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
	     '657530ae0a585d3028daa27a777d7e1f');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
	     '66be904e157a3aa6d51f290d74c3f18b');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
	     '64ae933418fd15771a1148b8280227c8');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
	     '58393a2e6394fb9dd1f8f9a36dc51f65');

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
	     '969cb6a3934ccca1fe226601ac5d8975');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
               borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
	     '70787dcea5cd6e0f21f9a28e025cfa8a');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
               borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
	     '5580697b4d24ecf1c9de44ba15aa34d3');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
	       borderwidth=>'0', gravity=>'Center', background=>'gray'/,
	     '8c63b9fe77d66fd4655d6a3f27869149');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
	     q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
	       borderwidth=>'5', gravity=>'Center', background=>'gray'/,
	     'a97d75a48d2f90c37de28a1c341cefcc');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
	     q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
	       borderwidth=>'5', gravity=>'Center', background=>'gray'/,
	     'a97d75a48d2f90c37de28a1c341cefcc');

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
