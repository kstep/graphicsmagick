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
  '31ab58c6097813a9c06cd42d1da5b5f18b33d670f34433a3d0a6c89cc2c4f445');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  '9c2a0632ceabab7e841c0ff88fdacadb6c9c89557bcc162f098666cdc3ee940e');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  'ccb172c4cb9edf2750f637e69b8dfc9da8118f6b518b604bf113f3704891efc9');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  'aad31035ed583a94245c229a25ccaf73c2cb380bca2ae7b5036e83db632b2681');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  'cbbd9c34663afed292b18894a41f0e231b8000819700369ce1d171c30bcd3f9f');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  'f70a8d8a0793e6177536f955d2e77fdeae48cb7296f1d9ce38fd0e3778d33065');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  '42f9df50293ef70375a0624e477342720f9fd968857e267ae417e3bf7ef183cb');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  '7e56557d8bc6243d601cde8cc312b218f220d7df61efdf08701d3729fdd7946e');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  'b9757a06f991d7014be8fd4e1718ab0d7968d5dba68510776794cbba63e984b0');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  'c2b116be3ee92c07c2ed67e20d9f79ea0bed442ba48aa93b198316f520468957');

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
  '4f2038e7525e52835433eaa2860e16169b62427622bc63974c3dd1a61b5f5d27');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  'fdfc629c242ed442716670fb236bd61add2e18433e62d35b9dc3572c2bbabff4',
  '7739f36756f61c960f73be8e7dc6ad62be1a71dbbb21d84f2cb3d9b01d18b31c');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  '82b75bbbd5c5f3a7ce5e4b6e5e25a36c3112bb12ec71734e6b30d7411f41f30a',
  'd4522ded64c564ac69778b0a97993bc2b0ae30a34e6689e33e75cc221407a4be');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'a57484328589c75a96c5e06365837cb60000641cb95eb500c8c4de0b472b5e92',
  'ddfae077c879deff51cfa9834eb74581d78c580e622a544ccfa9a085839fa211');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'aa0fad53ea8a7fc81fb6f12d7b708e762644624c4feec3d628b920c82c7b1c6f',
  '0550cf29231d5af948846a4605fabdeae6b9c09067ad19ce3d88a3306a040f50');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '8addcaa67416c1243e2e73abbca0f9d0669e8acf8a68d3707d14c88703836a7d',
  'fc2290e782cf4eedafdd47dc68789c841297da0f60729f5be21fec3918925ce7');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  '8addcaa67416c1243e2e73abbca0f9d0669e8acf8a68d3707d14c88703836a7d',
  '9d57208e90d6313f3965964a41da360fa2e6fbe63285e08e2274b21935a6a5a5');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
  '73ae9c5474bafee3b42a1a1626c6774c1a7b3371f705f91d507b55cfe29e5d5b',
  '77738966d37ba42471ece56353256f2c8e388f79f59368ea7ef20bce3fb57c87');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
  borderwidth=>'0', background=>'gray'/,
  '73ae9c5474bafee3b42a1a1626c6774c1a7b3371f705f91d507b55cfe29e5d5b',
  '7a7119c257ab46d70ac2e6579b714786a28cd7c7d4837ff756b115f7f8d4c788');
