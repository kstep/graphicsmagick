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
  'c9056dec49248808a7018b9594238d76949c1306943e74c4a4ad1c9250b123e6');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  '6f1bbb862040ff155a3df5684be702af9e3b0c8fefd91c35c4de0fab123656db');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  '7da4b7d20c6a79fadce99f5eb33fd725a16ac94069c287ea40511bbb3dea899f');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  '0efa95e075f5f3b8ccb8307e7897ab5afa796aee1c7063482ca088ca0a977038');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  '6ad8296b49dc6f87986fbe7f43345cc688ffddc65fd3c7972305f56e5e59b882');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  '1e415697b6e1dfe1e9668cac629ae752e0a7f86710250bfca86d774cadfc09ed');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  '2a75071245e28dfc54dbd91181c6b1983e5bef367d7b9b125a615d5331e8c4bd');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  'bfe1152891a6e55a76e8d2c3e332ecec64aa0ca5d3a00c157c45257708ec2e25');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  '865d64033b26dec5ae97ea53868083c7900f98da951f9e70692a45f0f65817b5');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  '58db9538863bd7ba2c7f9a41c45d610784b3f463d61d7b09ead8a325455ce13f');

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
  '6b76ea786c7411da5fa94a8c96b0c4835177c5460abdbac4866b0d12094a15dd',
  '8752bcb6bfb0376e726326f97d5619d280d80b79af0cc4178bcddb07b99027d8');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  '5894c9e6a0b2d1a36fe93d4a5c17561a04c99c9afd00b636d88faf9135e744a6',
  '19bc6f6c4a2a3250894bd31ca729bf22c8ecfeeb3e0f1d876e8a9201f111859f');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  '533fb649e7655c8f15a71268044abf720fc1cdb97946fa85004b466a79de9d45',
  'dbb71f796bf5deb7c93adde7088a659349191f5dc002573079d46a5e60587d9a');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'ef65a6309880f2b520b6b48f760767557977f4f6f3278298043a73ed8eff4dbd',
  '76cfa9952d40163fcc05d76c787b794f3594f4d41cc8bd9ef8847bcf38e7e0fb');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '262b9b76100d47420b3e547eaa39234af3e174a0a6caac2b227ecef7d35bd237',
  'd6e5bdfacae3855bf26ec1e7d1e371c85fd9126af83c5b8b03df89e8c4b81648');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'b62cbaf0f5ce55cc65c2a4be7331d644de190be60c3782532978afcd4fbaf935',
  '4f2de7a0b5aa53a508fb3c3fc6331a85f5af1ff47424ae9c57e74d600a77d854');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'b62cbaf0f5ce55cc65c2a4be7331d644de190be60c3782532978afcd4fbaf935',
  '5ff1163c22eb36b0ef51efb4249bbd5c669cab17f5cf630396bc7c473973f6fe');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
  'ab84e1c55a9bbacbf7dab3ac6132fc5b4eca2d64e9423631faf916e571980e07',
  '2bf69ba579093b3db76c74e4b450ae63ffd4303c55f195ea502a402316c2c539');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
  borderwidth=>'0', background=>'gray'/,
  'ab84e1c55a9bbacbf7dab3ac6132fc5b4eca2d64e9423631faf916e571980e07',
  '2c81d407d0b679f147d610637488ad544d704ea2defaf6352f21bdb689cb906a');
