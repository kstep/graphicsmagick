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
  '86429e2662d7083733e0b0d75497da57ba212649859ac261a72f1245b4d220d2');

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
  '5dfad29a83f2adc433c316d5dabaebf1ba1aac81f0895e2a15deb5d7d50cd81a',
  '770788157ae3d5589f0774bf4321c67990688bd9d4a5ce7969e42c31ecfad8f4');

++$test;
testFilter('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  '6afcd7320fa3e02d0927dbde488f2bceca021d7ab06fcae6a578974433b5ba0e',
  '890d51302e1e6aabd680b5a5c61eca58020e50e2c85a68bb25eedc913a6c6ba1');

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
  'ce3bc0200041d0e19fbb56c664224c02e9f09f19916995bba32fa2bae56bbb14');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'ef9ac1ab2f8aae9ca568ba4628cfcec5ef6e7f6e3db3d300372ade342dd244cc',
  '72bb6b273d938e1445a53302a54e30c21e46e43df192bd21e5682f8c257cc836');

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
  'a643f8c1998c4d4e643e6de69b488d9ea349d9def527481a511ff7839c523651');

++$test;
testFilter('input.miff', 'Implode',
  q/0.5/,
  'dd31bb88ac09b610724833bf0226391f62e57b1768ca171659c0af9a7b6e6a6b',
  '2dddb18733e67f366072b9b2cf35a21c69860e33e3ae1d7d19edd5f8d5a45b40');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'f22df13ea461203e4b88e5ed608dc1f8905f18e3580bf934a30da8e3f03d3fb2',
  '207fe33176b222d1d1c543141fbe292ff20372a3727fd5fd576d172e9cddf95e');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/+25+45/,
  '6d18a2b8e149727c853c9f2e02f5807ca732ed15f5dbc03f45314d2fab95a631',
  '2c9e13828667822c69a403e789d1783b7d2d875527c9e22c1481e1acf66d889b');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  'd67721c1bacf41a77479f9dc6db256be011b74de6cace0103b4977b8bf26f08c',
  '579999c158898f628c917f359f93c5c5936590dad258dace69cbc87be1083190');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '4a9c88f7ffaa8937920235af65d4b7d38d70d72315e4868a9068c3b1d0cbb308',
  '5d2a14f4ebece1c43e185ee9cefc05f6942f830d1a62b1a70de982da9627eda5');

++$test;
testFilter('input.miff', 'Negate',
  q/False/,
  '13d2a0623ca4eefa189d9db65896ed0b3af21c7a61b3d34a1644e9fb151daef6');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  'b1bcbe33aaeac67d8a9c8f43142fffb2895c83a4c923ff610e96ad595a75d4de',
  '00df35b62736d621efe8c3f12666bee9581984a0dd24d847ea4c9452e1f906c5');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  'fd92178558fa6bb4c3175d11f05ee68c182d6a3c0029fe8f5792a4d7e91edafd',
  'f0546cdb07cc0c20d3226c9d96abaa7de78bda798d5616115e0f151d27402386');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '2cfd3e729a57d13bef868d525d909d806df6b5ff595b6d5c88bd172f0eaef1e7',
  '8b93248e8d83be9caae3f0ff78a47bd85cc63372c18846943df0778de96a630c');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '6e2d128715c0fc965171a8bc66dbca02a3c7526456c93a9b933a9ea5baff48a6',
  'cddf279aa6c96ad32734cdf15023b8addf37062da31fee7afede2a3d31242b30');

++$test;
testFilter('input.miff', 'Resize',
  q/50%/,
  '034a25c1de9337e993d64629478c0f7826653cf8408b95511e98da17c8b16f92',
  '9d54ceadb2dd6b36eef5cd9e3c4f2015d277f93bf2205927ba41f449915b6edf');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '7d06e76bc4dbc8a1c5908dde113fe2f89644e6b9272b67575c648e97453f1a1d');

++$test;
testFilter('input.miff', 'Rotate',
  q/10/,
  '677cccb5efad4b4188825c5e5d483a643bc764d07437e55f4734769cf17b1918',
  '138c2338b6216e981f8b0a95a67d60d58d8d6e98599e2efbba79d71c4a473df3');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '73662fad184d63125ab5b41543be900b016afaeef20cfc23d7d657f53d2e2c03');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  '1e41fc85f160baa8cce4436ddf64e64e549b1e1bf941c75bc2860c6d1352022f',
  'ff2af316c85f4d38a707e974525def24ed8b9c25eaf247c960d8d923575f1add');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  'dfddaf9947c009961addec474d86d4142f418d20ff9de2460e8bd4928516aff3');

++$test;
testFilter('input.miff', 'Shade',
  q/30/,
  '47505dbc0872060a8718b589eee1099cda25823b20dea5565e18f18321056ee9',
  '451d93885c7d973c8c653bba5ecc0261734413759283d2ee8d1e4253dbb111e0');

++$test;
testFilter('input.miff', 'Sharpen',
  q/0.0x1.0/,
  '80d503a182ba0b6a88e840c5cf5912303cf7765db91cb9ed886d9dbc5f3102f8',
  'f93eefa1d389e3e62c332c55b47a4a7e434259363d9d7d769024246edd641c27');

++$test;
testFilter('input.miff', 'Shave',
  q/10x10/,
  '6f8749013dbe04020176a6e1d4a8a6783c2651fd0ef05cd9eb2dd66d38fa916f');

++$test;
testFilter('input.miff', 'Shear',
  q/10/,
  '7dd7b15ed83be0a3898ce1c06ef6ff35ace29f4beefd09f51fba88bc5b929389',
  'b40146de3357c0c156c3b23398b294b125aca4c832ca83ee5e737c5ab2df403d');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '7aa9589246439e23b6a4504cae40b9e32b0e5ee34a6c3ede14ffa958d39806b0',
  '13d2a0623ca4eefa189d9db65896ed0b3af21c7a61b3d34a1644e9fb151daef6');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  'e9e50b5e2c8abb68ecda0550fb512c237df3c864be979198fbcdf72cbc491595',
  '03e08e53dccd96b1bea8aaeb61e6cd81172bc135c2e5f22f481dc6bb5fb37158');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  '79af76e4629de988f7a9d83187503f6f7dc76ed3d2842347d0449a9ede0321fb',
  '40db28af0162a4abd3ee8a1094591cd7eb07463464d7c235e4c2f2ae40aa3686');

++$test;
testFilter('input.miff', 'Trim',
  q/0/,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  '7e193ce861b72cb9e5e405cc1aa882098060fbe35927c8cb5fab294ab0b8a0cf',
  '95c3deca4a6155e199a09919164d3b7ef10aaaf50296c9a25aad63b08f15fc7e');

++$test;

1;
