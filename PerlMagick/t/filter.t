#!/usr/local/bin/perl
#
# Test image filter.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..43\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

testFilter('input.miff', 'Blur',
  q/0.0x1.0/,
  '5f9da8b86a8751391d9d33c0f422bb6b7fd52e517f3f215eb6776ad1987c5c97',
  '9e4a15e65367a8af6c0253139cfddfa24891d97298d21f35c8d541648f39c11d');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  'ab598357f97c7324137d42a2c28cd5cee4fc577e3918a9c1f9049349e6355649');

++$test;
testFilter('input.miff', 'Channel',
  q/Red/,
  'ffa9b05716de08e1d6bc45acb6d45f538e63cac0a7bad3e59c3494f6b6a850a6');

++$test;
testFilter('input.miff', 'Chop',
  q/10x10!/,
  'a63a2d009971cb952a4414edc76f31f7b09209d72313484272f51708ecd225c5');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/+25+45/,
  '823c396975b4f3edde64cb7e146d8cac8c990c71d6c08db0a6decccc45cf01c6');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  '7203c2c81e7a49594b387d30467a4bff6509e1289ed6f922ac598fb97bf0961b');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  'b3ddcb8d22d1dfb9ffbb288f07fdf64a90c415787fd2b71e5f5c81d63415f490',
  '2fcb1202f1c7920e0e80005c52a3e7af503225b117dec285476fd6fc58e50a47');

++$test;
testFilter('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  '26fbd8853a8bc6bbdc545fc9914b64c327fa9cb987a02bec842e883a5f1d5d21',
  '5412ac33937ab3264c3b85e8c14298441c8ecb5a04ca276c8b06e83ba347119a');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10!/,
  '09341c77174d372d6a3bdd32badfec8ee17b3a1d93957fb3cfcea1004c37bec3');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '8c63f4f807d48648c73f2eee9c278f068deae3093b4d917d91eff2f5e9ed8551',
  'd3c153d8eed545e70371f82d5398c0a9649a90eff964c42ed94074586046438d');

++$test;
testFilter('input.miff', 'Edge',
  q/3/,
  '077ae4e1fa8db5e76a60a5764faf3b50fb15a9db0a3eb6d93f50bdbb3455f361',
  '1aab086f11b69c1fd45b7c9892072ca56020f714c3cd5a717cd71e93a18fda1d');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  '73562755f324ea9e06cc8e05ef30022310b12aa863a572e471391242a611db15',
  '5827541e90f687411132692c5f624490ee2504ae018ef5d9b3f07c4023fb3e31');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'c5dd4b023cf8f05eda5a86ae54a3cc2f26bdd0bdee9099014a992a4dadcf5c84',
  '9e8f3346bec5b9cd8ba0fab680b856f8f90738b5b92c47477902670a1a62fb34');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  '41624de9b2002cee621e771fd9433177fd91c2560e75dab4e1c964cce0da7365');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  'dce42b2ed4080faa02c12729c4da21440a3c31e08585787f72181e5b2809e109');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  '808ad51705e83a0bbc21720b770f198949df0f99a7f060a93eba22ada7768240', 
  '0469d077e745fe177f42abe4383cd9fd6f3aba5a1fa24ba67268390f540daeff');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  '8f79b178b0c64f6b063a10ad01c7b4828307765b8ce24d6d100a22bd7d91da60',
  '6208dde7055e06976b061dca97492a183354a60250c060d5a051b8d2db3cfee2');

++$test;
testFilter('input.miff', 'Implode',
  q/0.5/,
  '587985c66eec744fa48e8b6acafaa2bb5c789472134fb89355351179bfb10997',
  'cf34aa84cea00423c0e824326c57283ce2c99c8917dee101f804c7f87418972c');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'e5e4dbaaea4faba87326d1ccde1f84aaa60c41518bd242f4d65dd9f37d2f2dc5',
  'f77f819a175d55f74d6e7c3f1b5270662a8cd53867844b244faa15dc77658922');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/+25+45/,
  'f77f819a175d55f74d6e7c3f1b5270662a8cd53867844b244faa15dc77658922',
  '2c9e13828667822c69a403e789d1783b7d2d875527c9e22c1481e1acf66d889b');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '38a4a42688d0c7ffa5355204d2077d57c9f9456888f04780460c51cbf08972c5',
  '34dd1b9c8dbcdb6b1535ebfc57a85041eeda34784d54f24dc7b09cbdd18b29fa');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '6e46f9e5399b8265a1c522750dbdd227d1dcae084d18f00a79b27b5638670bc5',
  '254fca4cc27590ecae41aecb3c70dc6f1689fda4388434652bf6f893257df5d9');

++$test;
testFilter('input.miff', 'Negate',
  q/False/,
  'aa9cdf74506bdd007db2ddf4ad1dc4a7149872e62eb4592ebd56d2426fd6808a');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  '3fa3d1cce7e10f0700550499ef4402c98767a4b6f9a799bc9d92dbacfdadd281',
  'd715a330887652acb8e44e12995763d80f53d2fa8f6cede0caa4e25052a59f0b');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  '064322f748252c8ca8d4c91e062c41cba327629f389263d6a74ad07049234fa2',
  'fe861eb4c6d2dcf17c0cc20d5ac183e9d99e99a455648975eff255462869d53b');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  'a690f2732ba25e3a101ba3d166b5c9345198606d73eae0f7c8b7fc8e61bbba01',
  '02824546ae2ed7e85c3d5bec5339c8c3201054a221eb8f193af7510b43a8dce2');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '58fb8633fe6b4a9e97957aabbc3bf5dbbd2c035306e5e1dd2d93f2e18476ee7c',
  '9bb8bb10de3323db5f207d6a022ceb29f59e9c7a7951b073b11f9e0a27e2766d');

++$test;
testFilter('input.miff', 'Resize',
  q/50%/,
  'e37ea53b8e167838add16e4a46be3689150df6b8d496da15ec41bee74d2fb9f1',
  'c6815c8b064ef7985e96d5fa3fff785f04338a72b0a9fa96b849eb9be6d27409');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '9c3cf59e1384927568fcb526bf56cc21e76e2605be13f9ebc81f2779e032ff46');

++$test;
testFilter('input.miff', 'Rotate',
  q/10/,
  '5bf1ebdbad22c4e650e2e49602790157331ae8080c6cdade9d67363ef12b28c0',
  'ac30e49f17281683b7cec39d0cd8e98679de272329346b671cbad8c90a89f8da');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  'b71321d0629a9070e6f561d41c77455b7f77f313735ea70af9c7fb0f2f39aa4f');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  '2bee86bfedf751c4a58d791427d4c24fac99de06808588c22e3f54a358ccac3c',
  'f622b224f63692654783bce11a9b257e341b7d7fb3ad6ba329bbb77bad01256f');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '04e71d8c9d501336446a252ee401b204939021a932644b80305fd4d30749b6e3');

++$test;
testFilter('input.miff', 'Shade',
  q/30/,
  'e28a7f907b771d18053bd8861eafe8fefc8d61de820fb893b91d5c53734fa98b',
  '1b7ccfad5ece6a58ae688046a4ceb07e3196bafe5b85663b0aab6dc679ce2762');

++$test;
testFilter('input.miff', 'Sharpen',
  q/0.0x1.0/,
  '1621c4ad8e033095f0fdf7f227f0ad1ef6f91c749d72718435636885fd7d5917',
  'd9ed9c48b7f831ac7a7e4bed1bb8469ecda4bf28c3dfd20688e86ff3b276bd54');

++$test;
testFilter('input.miff', 'Shave',
  q/10x10/,
  '719925355ca8a0b87d181784df353d8ce007641a739274f750fa87977aa33697');

++$test;
testFilter('input.miff', 'Shear',
  q/10/,
  'fdaa73066e3d6ce44921aba2d64673ca931a1edd7c7b74aa9b7f87180cff79d3',
  '28f84572bd76da9809415630151e53765cd182dc880f293c3e3f50461d2e5f86');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '1a38bb91dbf862e46fc98be85e2ebb540de35da01fa69bebb899ee8b2c440a52',
  'aa9cdf74506bdd007db2ddf4ad1dc4a7149872e62eb4592ebd56d2426fd6808a');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  '270e1d5da49b7c47b2cd1b5827890f1c0ba9ba039827a45169881537ab96c36d',
  '11273e5a77ece8b34c513ba1dec132ea78c40a93fcf3fd0c7afd28c5b297cfb8');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  'b2c86c8e0f4be2f9cc61e848ab0910ff36c4b1c923e439c9ddcbd4eba9998100',
  '0b53fd4dc7d03dd95d148d4aef9efa50eef65c5434ca2fa5a4ad974f7e0a8f8e');

++$test;
testFilter('input.miff', 'Trim',
  q/0/,
  'c891f62cd29ac4b8cca647fa1d42e0a7965f9fcc92e34f5b20cb988fe80563ca');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  'cc4cd6466273a0a0212cd29e0b628d52068dd375e2d7e641f26c36d348ca07d0',
  '10cc8981286fbc9d7a262cf6cc3694480311c6ce6206965550c5acfbd7392cdf');

++$test;

1;
