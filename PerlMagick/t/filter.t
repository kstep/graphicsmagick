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
  '66c8403de767bb48540fcf19e50ae61bb9346b39f744399fade43e28795c181d',
  '9e4a15e65367a8af6c0253139cfddfa24891d97298d21f35c8d541648f39c11d');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '1308b50682539ba231501d336c0f688372becf093fab928c131c0c8e9dfbed87');

++$test;
testFilter('input.miff', 'Channel',
  q/Red/,
  '76b07c5ff3de0530bc6fcba511f1391cef261eaf4b0f9ee1e3859c7466d017e7');

++$test;
testFilter('input.miff', 'Chop',
  q/10x10!/,
  '0cee3408e6e3255317340bde1ebe026359d58ec3989ecf315df34dd52925beca');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/+25+45/,
  '9767e3a22a9cdafe39c448903aea84e68881d39b250b4be921e3f3ebec53f9ee');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  '767b270256d40ccec025494072c070256147a7046b464327ebea6f7934b96121');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  'b6af336a1e37958b0c5ab05ab5a76e62555c1ac4b171cbf8d2e4ab2ddd9b0d1c',
  '2fcb1202f1c7920e0e80005c52a3e7af503225b117dec285476fd6fc58e50a47');

++$test;
testFilter('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  '6afcd7320fa3e02d0927dbde488f2bceca021d7ab06fcae6a578974433b5ba0e',
  '5412ac33937ab3264c3b85e8c14298441c8ecb5a04ca276c8b06e83ba347119a');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10!/,
  'b66e37b497d4e7275b04fc8f0d0539aeca9a73c187ae239352c7abec44ad2fe9');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '9e7f20a6f5a36cbc7809c4ef7e748583fad81c6718f5af799210666b76016368',
  'd3c153d8eed545e70371f82d5398c0a9649a90eff964c42ed94074586046438d');

++$test;
testFilter('input.miff', 'Edge',
  q/3/,
  '8245bada67e7be6509c02ec6f8372a8c2eb0c04bdc6221ff839ac06e770b9bf2',
  '1aab086f11b69c1fd45b7c9892072ca56020f714c3cd5a717cd71e93a18fda1d');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  '5214d01ac3766debeaf2d917db887e9c62ba7cf11232b635c18ada70d219ad2d',
  '5827541e90f687411132692c5f624490ee2504ae018ef5d9b3f07c4023fb3e31');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'ef9ac1ab2f8aae9ca568ba4628cfcec5ef6e7f6e3db3d300372ade342dd244cc',
  '9e8f3346bec5b9cd8ba0fab680b856f8f90738b5b92c47477902670a1a62fb34');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  '092fa2bb0e2c1826a7b0ac9ea42a788684230623fa56bf0c4c2c9e1d71de8bf1');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  'f847ea7cc9b78677c083df656978081827c7bfb532a4c2d5fd3eed19e8b73a3a');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  'b4956b654102b9af96956017f94c57d214110d91cb7be995e5ce19d29b0a3eba', 
  '0469d077e745fe177f42abe4383cd9fd6f3aba5a1fa24ba67268390f540daeff');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  'e29420079d66769f9e571ab9ce091957c9cd12ad9af0e8aa613d46b0b5ddcd3a',
  '6208dde7055e06976b061dca97492a183354a60250c060d5a051b8d2db3cfee2');

++$test;
testFilter('input.miff', 'Implode',
  q/0.5/,
  'dd31bb88ac09b610724833bf0226391f62e57b1768ca171659c0af9a7b6e6a6b',
  'cf34aa84cea00423c0e824326c57283ce2c99c8917dee101f804c7f87418972c');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'f22df13ea461203e4b88e5ed608dc1f8905f18e3580bf934a30da8e3f03d3fb2',
  '6d18a2b8e149727c853c9f2e02f5807ca732ed15f5dbc03f45314d2fab95a631');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/+25+45/,
  '6d18a2b8e149727c853c9f2e02f5807ca732ed15f5dbc03f45314d2fab95a631',
  '2c9e13828667822c69a403e789d1783b7d2d875527c9e22c1481e1acf66d889b');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  'd67721c1bacf41a77479f9dc6db256be011b74de6cace0103b4977b8bf26f08c',
  '34dd1b9c8dbcdb6b1535ebfc57a85041eeda34784d54f24dc7b09cbdd18b29fa');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '4a9c88f7ffaa8937920235af65d4b7d38d70d72315e4868a9068c3b1d0cbb308',
  '254fca4cc27590ecae41aecb3c70dc6f1689fda4388434652bf6f893257df5d9');

++$test;
testFilter('input.miff', 'Negate',
  q/False/,
  '13d2a0623ca4eefa189d9db65896ed0b3af21c7a61b3d34a1644e9fb151daef6');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  'b1bcbe33aaeac67d8a9c8f43142fffb2895c83a4c923ff610e96ad595a75d4de',
  'd715a330887652acb8e44e12995763d80f53d2fa8f6cede0caa4e25052a59f0b');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  'ae81b1636f41eba1923ff4bcdd606fbba6fde92cb2073549c0c2778eeff6d0f6',
  'fe861eb4c6d2dcf17c0cc20d5ac183e9d99e99a455648975eff255462869d53b');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '2cfd3e729a57d13bef868d525d909d806df6b5ff595b6d5c88bd172f0eaef1e7',
  '02824546ae2ed7e85c3d5bec5339c8c3201054a221eb8f193af7510b43a8dce2');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '6e2d128715c0fc965171a8bc66dbca02a3c7526456c93a9b933a9ea5baff48a6',
  '9bb8bb10de3323db5f207d6a022ceb29f59e9c7a7951b073b11f9e0a27e2766d');

++$test;
testFilter('input.miff', 'Resize',
  q/50%/,
  '034a25c1de9337e993d64629478c0f7826653cf8408b95511e98da17c8b16f92',
  'c6815c8b064ef7985e96d5fa3fff785f04338a72b0a9fa96b849eb9be6d27409');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '7d06e76bc4dbc8a1c5908dde113fe2f89644e6b9272b67575c648e97453f1a1d');

++$test;
testFilter('input.miff', 'Rotate',
  q/10/,
  '677cccb5efad4b4188825c5e5d483a643bc764d07437e55f4734769cf17b1918',
  'ac30e49f17281683b7cec39d0cd8e98679de272329346b671cbad8c90a89f8da');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '73662fad184d63125ab5b41543be900b016afaeef20cfc23d7d657f53d2e2c03');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  '1e41fc85f160baa8cce4436ddf64e64e549b1e1bf941c75bc2860c6d1352022f',
  'f622b224f63692654783bce11a9b257e341b7d7fb3ad6ba329bbb77bad01256f');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  'dfddaf9947c009961addec474d86d4142f418d20ff9de2460e8bd4928516aff3');

++$test;
testFilter('input.miff', 'Shade',
  q/30/,
  '952f26eb25cda963e3f53d2dea1e1d3d6673d137835c125208a5609fa7fa35a3',
  '1b7ccfad5ece6a58ae688046a4ceb07e3196bafe5b85663b0aab6dc679ce2762');

++$test;
testFilter('input.miff', 'Sharpen',
  q/0.0x1.0/,
  '80d503a182ba0b6a88e840c5cf5912303cf7765db91cb9ed886d9dbc5f3102f8',
  'd9ed9c48b7f831ac7a7e4bed1bb8469ecda4bf28c3dfd20688e86ff3b276bd54');

++$test;
testFilter('input.miff', 'Shave',
  q/10x10/,
  '6f8749013dbe04020176a6e1d4a8a6783c2651fd0ef05cd9eb2dd66d38fa916f');

++$test;
testFilter('input.miff', 'Shear',
  q/10/,
  '7dd7b15ed83be0a3898ce1c06ef6ff35ace29f4beefd09f51fba88bc5b929389',
  '28f84572bd76da9809415630151e53765cd182dc880f293c3e3f50461d2e5f86');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '7aa9589246439e23b6a4504cae40b9e32b0e5ee34a6c3ede14ffa958d39806b0',
  '13d2a0623ca4eefa189d9db65896ed0b3af21c7a61b3d34a1644e9fb151daef6');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  'e9e50b5e2c8abb68ecda0550fb512c237df3c864be979198fbcdf72cbc491595',
  '11273e5a77ece8b34c513ba1dec132ea78c40a93fcf3fd0c7afd28c5b297cfb8');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  '79af76e4629de988f7a9d83187503f6f7dc76ed3d2842347d0449a9ede0321fb',
  '0b53fd4dc7d03dd95d148d4aef9efa50eef65c5434ca2fa5a4ad974f7e0a8f8e');

++$test;
testFilter('input.miff', 'Trim',
  q/0/,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  '7e193ce861b72cb9e5e405cc1aa882098060fbe35927c8cb5fab294ab0b8a0cf',
  '10cc8981286fbc9d7a262cf6cc3694480311c6ce6206965550c5acfbd7392cdf');

++$test;

1;
