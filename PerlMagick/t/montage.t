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
  '9b54c8c0a44ddc18ff316a01de13c8f1c6e600a6f8e7e08bd21d9246c844eb68');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  '10ba1c32f7ed3a11fb5b58980f96c55eb807dc189b06126790ef0b52878d2083');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  'fc1c1686f8cc0130298f328b413d893e515a288288d71084f9cc50f2fb33cd28');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  'd6532f31a14910c8425900825b60dc335a53e78a0f7948cd468b2410dc4e1491');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  'd499093d0d4ad1de5fbb4efe81680e12e5db387e4a93ec8463a00b2cd43316b4');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  '07ab85259fc7d2cadac36293db57cee1aa68d42170e26ed22d2c2f38fcc236a7');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  '9e29f330ffe5101d9024688dcacb1cb476560824ca8c0076f71931fb174cd97d');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  '6acba46513eaf58d9c9cf061beff4b03e488042a52547fa8b456e2f48bc63840');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  '7e3024c7a1885b35e891e1f970f747fc18051dbe90290e30200a9044712e7e83');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  'b72b514b4aeb1a1e5954c99b3a4b468e5189adc002e92e2529f2442c411ed1a8');

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
  '8752bcb6bfb0376e726326f97d5619d280d80b79af0cc4178bcddb07b99027d8');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  'fdfc629c242ed442716670fb236bd61add2e18433e62d35b9dc3572c2bbabff4',
  '19bc6f6c4a2a3250894bd31ca729bf22c8ecfeeb3e0f1d876e8a9201f111859f');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  '82b75bbbd5c5f3a7ce5e4b6e5e25a36c3112bb12ec71734e6b30d7411f41f30a',
  'dbb71f796bf5deb7c93adde7088a659349191f5dc002573079d46a5e60587d9a');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'a57484328589c75a96c5e06365837cb60000641cb95eb500c8c4de0b472b5e92',
  '76cfa9952d40163fcc05d76c787b794f3594f4d41cc8bd9ef8847bcf38e7e0fb');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'aa0fad53ea8a7fc81fb6f12d7b708e762644624c4feec3d628b920c82c7b1c6f',
  'd6e5bdfacae3855bf26ec1e7d1e371c85fd9126af83c5b8b03df89e8c4b81648');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '8addcaa67416c1243e2e73abbca0f9d0669e8acf8a68d3707d14c88703836a7d',
  '4f2de7a0b5aa53a508fb3c3fc6331a85f5af1ff47424ae9c57e74d600a77d854');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  '8addcaa67416c1243e2e73abbca0f9d0669e8acf8a68d3707d14c88703836a7d',
  '5ff1163c22eb36b0ef51efb4249bbd5c669cab17f5cf630396bc7c473973f6fe');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
  '73ae9c5474bafee3b42a1a1626c6774c1a7b3371f705f91d507b55cfe29e5d5b',
  '2bf69ba579093b3db76c74e4b450ae63ffd4303c55f195ea502a402316c2c539');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
  borderwidth=>'0', background=>'gray'/,
  '73ae9c5474bafee3b42a1a1626c6774c1a7b3371f705f91d507b55cfe29e5d5b',
  '2c81d407d0b679f147d610637488ad544d704ea2defaf6352f21bdb689cb906a');
