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
  '2fc7c63de02f46a7c8abc442e8881cc2a9599c3f0d23fb6f3f2e54b46244a900',
  '9e4a15e65367a8af6c0253139cfddfa24891d97298d21f35c8d541648f39c11d');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '772677f1cfdb4b6d349800a0cd99d82d41dd37f3390eafd90b19828333370063');

++$test;
testFilter('input.miff', 'Channel',
  q/Red/,
  'bb496913755f328dc2c09a9818779138541010f4752555ef419773641acb384c');

++$test;
testFilter('input.miff', 'Chop',
  q/10x10!/,
  '12013c10d0680f575f656a7759ab09870c944deecc3a6e28fc71262806649c35');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/+25+45/,
  '724f5cec455abdeb703ea4034699056fbad9eb882e66f29cf765431da715ad64');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  'efd12417c49cb4f7bc949c88a76781062dc114f312e22e5df4ae0b7c12f4a185');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  'b300ac8d792c7ca771e245ba287dbc85aa4cacf8d8c7260b0e06f2af47ee65e8',
  '2fcb1202f1c7920e0e80005c52a3e7af503225b117dec285476fd6fc58e50a47');

++$test;
testFilter('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  'ea018ff6344a49f7693cc6d9a0fb8609edaf74eba1a40ee5a2163da5cc2b10b5',
  '5412ac33937ab3264c3b85e8c14298441c8ecb5a04ca276c8b06e83ba347119a');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10!/,
  'ef7729f6fe04f555928456445e8210c61c2db8f4cb39418df5dc2d0750d9c69e');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '097ac4b9ce1d2684278e8df51725c443e97b20c394900c66a711643bd125221e',
  'd3c153d8eed545e70371f82d5398c0a9649a90eff964c42ed94074586046438d');

++$test;
testFilter('input.miff', 'Edge',
  q/3/,
  '4d2c0f0f1d522cf7c662b8905b24867055f5b2540c56b8203d20eb6b1b8ff1af',
  '1aab086f11b69c1fd45b7c9892072ca56020f714c3cd5a717cd71e93a18fda1d');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  '6632c771f688d1a868d00add4ddfadcac579423548171c45347d93db86a8560b',
  '5827541e90f687411132692c5f624490ee2504ae018ef5d9b3f07c4023fb3e31');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'ccd32a7a7d6437ed6f8b7260ddf6c82e51bd4157ab0b2e670a492d90b7795047',
  '9e8f3346bec5b9cd8ba0fab680b856f8f90738b5b92c47477902670a1a62fb34');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  '44368f5c7fd14523ae65142f1c6d57930f6cac4f5b53731692758d6aa61eeddd');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  'e50cde0ef733c63f13eb7dc00465a15f196ec900424f24344604683a6021f592');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  '0c72a26b45bd30bbf37795fcb01b8c16313873cd763aee33918e530a8d1a54c4', 
  '0469d077e745fe177f42abe4383cd9fd6f3aba5a1fa24ba67268390f540daeff');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  'ab966ca4d7bb04d8b3513263570d5963d86d7454bfce76a79fb96846ec63dc68',
  '6208dde7055e06976b061dca97492a183354a60250c060d5a051b8d2db3cfee2');

++$test;
testFilter('input.miff', 'Implode',
  q/0.5/,
  'e8a27e05f300b81a93a8a4c34afdd1ef0de1c1490168b52b6c777b99d741bfec',
  'cf34aa84cea00423c0e824326c57283ce2c99c8917dee101f804c7f87418972c');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'be00c49b2935bfe4c79508eccb954e2d0b8334c6cc9b269c86499e2076c4ee8c',
  'ba5ce559cd2c7fc557b882bd3c991654e608c649260178af7b1bd6a37f21735a');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/+25+45/,
  'ba5ce559cd2c7fc557b882bd3c991654e608c649260178af7b1bd6a37f21735a',
  '2c9e13828667822c69a403e789d1783b7d2d875527c9e22c1481e1acf66d889b');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '76eb547486543116dc64791b0d6ae88652d45a93d78c56cc0bda2414f55f3413',
  '34dd1b9c8dbcdb6b1535ebfc57a85041eeda34784d54f24dc7b09cbdd18b29fa');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '88ecad257be381f480dcadf43b60f2854d43175bba2c4245ce68f7e94f9419df',
  '254fca4cc27590ecae41aecb3c70dc6f1689fda4388434652bf6f893257df5d9');

++$test;
testFilter('input.miff', 'Negate',
  q/False/,
  '66fc05d1db389ff6e63ecb74740e81577b4b680dbd7b84604544269fc3864661');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  '72ce456a83d1d460228d809f58aea89298ea2eb59f7b9bd9731a2530b2c9e9a2',
  'd715a330887652acb8e44e12995763d80f53d2fa8f6cede0caa4e25052a59f0b');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  '39d3b89e5a94880505817eca58adb3e02c1f2a3243b4e273ed30810e620638d6',
  'fe861eb4c6d2dcf17c0cc20d5ac183e9d99e99a455648975eff255462869d53b');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '795db8d593b0dcc7329c2d49c9a80af66d7793c15bead65fffe2b265063896e2',
  '02824546ae2ed7e85c3d5bec5339c8c3201054a221eb8f193af7510b43a8dce2');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '14d6791f203a38762cfb511d0e813d17f055bed8d104903eaaa5a486c4e7fc29',
  '9bb8bb10de3323db5f207d6a022ceb29f59e9c7a7951b073b11f9e0a27e2766d');

++$test;
testFilter('input.miff', 'Resize',
  q/50%/,
  '4553fbacb66b71778948ed9aa26c15fe805d7a707397320d13c6ca751ad22b78',
  'c6815c8b064ef7985e96d5fa3fff785f04338a72b0a9fa96b849eb9be6d27409');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  'd1aeb5aa53864a959665ee31760d15979f0eff2b379121b07bcdc65a2bbedc71');

++$test;
testFilter('input.miff', 'Rotate',
  q/10/,
  '1bd7ea1ca49a3570920f087af5cdba4f216c28a38c412ea590cea0d47043f044',
  'ac30e49f17281683b7cec39d0cd8e98679de272329346b671cbad8c90a89f8da');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '01bcf6295ee5c2fede8d18e1e9f668b04affcc9fa0bfff5c40a295ed0b77cacf');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  'aa099fff7ff0a54cd53b57e7b6901fae886f5b231d80f3c0d8344d8a4c2e6227',
  'f622b224f63692654783bce11a9b257e341b7d7fb3ad6ba329bbb77bad01256f');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '7e7957b099cf7fdc0612e6e0d2215afa0576b9373bf9727014d478d3f44d22b4');

++$test;
testFilter('input.miff', 'Shade',
  q/30/,
  '79fb9ba0be4c46713288db8ffba4a07d284d1edbd3292e5fda446bec3b9722dc',
  '1b7ccfad5ece6a58ae688046a4ceb07e3196bafe5b85663b0aab6dc679ce2762');

++$test;
testFilter('input.miff', 'Sharpen',
  q/0.0x1.0/,
  '57700bbf271029f3a5b0ecd49045d1852b8f3e9c696e1a08794a5ca6aca42a3e',
  'd9ed9c48b7f831ac7a7e4bed1bb8469ecda4bf28c3dfd20688e86ff3b276bd54');

++$test;
testFilter('input.miff', 'Shave',
  q/10x10/,
  '4fd60360a00643012e3093ffd3f928c506189bafb4d81c6d60cf36512d386d32');

++$test;
testFilter('input.miff', 'Shear',
  q/10/,
  'f0820f3791a5aaf84c606d1a1be06f44551093261ccafd8890ef0063964f6ee8',
  '28f84572bd76da9809415630151e53765cd182dc880f293c3e3f50461d2e5f86');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '746387641d34c31a52ceee9c322357d3128b5800edee48f834a045d2b76d7eea',
  '66fc05d1db389ff6e63ecb74740e81577b4b680dbd7b84604544269fc3864661');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  '4fd5d14bcd4e7be092b12fc3870499575d2aa158d6e2d15b39b3c2c7ec02a950',
  '11273e5a77ece8b34c513ba1dec132ea78c40a93fcf3fd0c7afd28c5b297cfb8');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  '9156bff1acf78a9f47380209488aed65591738889037de57c81b3d3d9b6a795b',
  '0b53fd4dc7d03dd95d148d4aef9efa50eef65c5434ca2fa5a4ad974f7e0a8f8e');

++$test;
testFilter('input.miff', 'Trim',
  q/0/,
  'b4be64ae0b32a51a62ef1e7001f454b613c9cc2ca2fca1db2331690538f26d8c');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  '1486217c4724bbf2105a5b7ce2d435e1d354699da0c93743d10ac5eb620457ee',
  '10cc8981286fbc9d7a262cf6cc3694480311c6ce6206965550c5acfbd7392cdf');

++$test;

1;
