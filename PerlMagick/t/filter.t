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
  '9a4e6e77fd7aa20d99a2d086b15627432bd78750cdf5d336ee45c6c6ea2745c3');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '90a0ef26496810bef40cef8e5d3bc28cb64676e2b8b6c43268257e7c481b7452');

++$test;
testFilter('input.miff', 'Channel',
  q/Red/,
  'cfdb638c95a4ab24dbf93cad1959bd94df6465611f03bc8ca8bc562d353655d2');

++$test;
testFilter('input.miff', 'Chop',
  q/+10+10/,
  '886abd0600151937beea6143c6c8ab149624ceddc40076cb28072a4c5c904bcf');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/+25+45/,
  '4f575497a2a7bd00c078d169f7e21309b9dab1319c9601c900f233ca4d3996a6');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  'cd24b808aa19a0710563cf4705ebf8efc3cddd82a7d3648c9121c5058f4c9cf5');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  'b3ddcb8d22d1dfb9ffbb288f07fdf64a90c415787fd2b71e5f5c81d63415f490',
  '46a67f879c6e1849fdcabf14f1c48e19945705411e63db27e5a6fb6d60909bb1');

++$test;
testFilter('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  'a1d263752e2d4081ba1c4e5ca5be3283d9d662c96e43328c33cba8451fcaaa2f',
  '077ae4e1fa8db5e76a60a5764faf3b50fb15a9db0a3eb6d93f50bdbb3455f361');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '886abd0600151937beea6143c6c8ab149624ceddc40076cb28072a4c5c904bcf');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '8c63f4f807d48648c73f2eee9c278f068deae3093b4d917d91eff2f5e9ed8551',
  '82ef64b02d8d223e8880d07be102ba0f11c398bc47dbee866867401b3d95eee0');

++$test;
testFilter('input.miff', 'Edge',
  q/3/,
  '077ae4e1fa8db5e76a60a5764faf3b50fb15a9db0a3eb6d93f50bdbb3455f361',
  '0b7cb9e513f657952560c327dac8ae91ce4ce0374f970b5e8893eb6accde31d2');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  'd68ffa55a526c6cef59cfa18866a1a617b1065eb5a2975106b191d0535b0b477',
  'be5ebb42e66abda1cffa78cb4586ebf9906789c1781e4f776365f057d793fce7');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'eef347707dd430c87fc90f63445fe4bf4e59e64d60d51db43cb9aae57f960e48',
  '17e317a255885296bbd7bcd6db79d99605826c60e4df8e51fd9930c28b072e02');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  '33d7e8ab8f3e51619191bac5b0234c126a0fca006d39a682b1ddb6d1e515b8eb');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  'bba4072d75dd5696176c8b7862724fe0309f60b3b67cc25583077ff5dcac1c9a');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  'bd822acc21e1d7ea0b1fae3efe36e4dd02e4462bde1c7eb3a5f10fa52a08e4c1', 
  '61e51802a1b11a86b1f192cf63da321bdf55292c13583370547f2ad785ec602c');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  '8f79b178b0c64f6b063a10ad01c7b4828307765b8ce24d6d100a22bd7d91da60',
  '18e4c9ebb351a562c5eb844cdfbf091933c34bd911a25841b19c8e365828e43e');

++$test;
testFilter('input.miff', 'Implode',
  q/0.5/,
  '587985c66eec744fa48e8b6acafaa2bb5c789472134fb89355351179bfb10997',
  '800bd416e3b963aa43bdff57bd511289800eda82a35f172152ee9cff1b45fe32');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'e5e4dbaaea4faba87326d1ccde1f84aaa60c41518bd242f4d65dd9f37d2f2dc5',
  '31c493286e8bd7ba5a8f36ba36edb05e1f8a58442347fd0021fb9c3a6a692822');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/+25+45/,
  '31c493286e8bd7ba5a8f36ba36edb05e1f8a58442347fd0021fb9c3a6a692822',
  '100a892e7d683a23a73ea67fef7efc7b57d48efc1e1bba4cb7117e6ce371a3fe');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  'e2cc6133847f02d4ae3834919ef8329de7b4876581b06dbc46802ca7bbb0ffec',
  'c33bd56e6384526024abd702b6d87faac74e499b67e9c6e37164cfd04c6acc78');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '6e46f9e5399b8265a1c522750dbdd227d1dcae084d18f00a79b27b5638670bc5',
  '7ff720b84e67ab83b084ca394dbab311ef8954c6404e5709aa2055d46bfc3aac');

++$test;
testFilter('input.miff', 'Negate',
  q/False/,
  '70eb168ab1fb62363a2353fca5b3565b162ad26e383bf3632993e0ee595b34b8');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  'a5dfb674174da975a35661b907da335ea8e5ee1cdd015dbe87ddeaead3c54389',
  'c72eafafef071a5073172a3e53e7611cc1e0dbb9bae16b43b18ffefdffdcb7f1');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  '46616c1994530429629966b77afb9a77692335eee2fec1e40b719452d31e9109',
  'c5a4ef6389b426ca230cb0237bf4b471beb491e5086072084bcd37adfb7f4e9d');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  'a690f2732ba25e3a101ba3d166b5c9345198606d73eae0f7c8b7fc8e61bbba01',
  'dc224abeddcb1bb0efdaad191167935b99daaa989cf57dc6fe1aa11b86a93ce3');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '0c363702b4b6a27a629426255ce71174a3996580ac5211bf6298f63e655bc400',
  'ef9158abb43a7955f05b826ac8ce2f8e643a76df1ed829a9ea9c290e7d4ad54a');

++$test;
testFilter('input.miff', 'Resize',
  q/50%/,
  '94238afde30c971fe52be34f540bdd7c75a82befc93a3f5ac5c49194716a48d5',
  '80af84c10bf9df578ef1caca59b96df8bd7646a39d9dbf2e2e75a33936cf03c5');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  'c492f7a1938e6ee5676c3eead3eb1754d8c5b59bffa132efda0dd49f436a210a');

++$test;
testFilter('input.miff', 'Rotate',
  q/10/,
  'e6edc84899ad7506cd939cb36f028b34af20ae88bda7307673259880c439b460',
  '3c2f77dc24f6a30ddc6f78a6e8071909344651f76c4da390c8052768ffde1911');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  'cd5fa80486baa545df95bbae9a089d0946e9636ad413ec5bae10e350664ad975');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  '2bee86bfedf751c4a58d791427d4c24fac99de06808588c22e3f54a358ccac3c',
  '7720d2340dfa14a2dbff6c2ad491c2ffd3366bebfd6918eb3b25035518236ca9');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  'fa219869b3cc8560def29c2f28974d75d3eede767c70d784be931fe44c6b1172');

++$test;
testFilter('input.miff', 'Shade',
  q/30/,
  'd539e24d03ae74330991d28c440bb69d629ddc491689e95290068a5e4e38eabe',
  '6dfc24506f84516feaf3e106119d3a17f6059db67d1e196f8d4810d2816c0eb6');

++$test;
testFilter('input.miff', 'Sharpen',
  q/0.0x1.0/,
  'eaeea39e3b2a33b9d8761a70a57717214fd4b39944df47bd21a776f4ac2dfa58',
  '7cbd5fdf28a9744d27bd056fd0915538a6a3d780d85231229147f9c1bb573981');

++$test;
testFilter('input.miff', 'Shave',
  q/10x10/,
  '1f3cb0be4e297e1fc20ee7b2fedf7ea6d4c8b5b91856e4f7be44ddd278b8ff81');

++$test;
testFilter('input.miff', 'Shear',
  q/10/,
  '961cbb9642ee5499af3e7e43be214e6ddbd2cfe00c6f239a2236895b4864c21d',
  '41beaaf308f7f6662bed19143a8b3f60805fcd57cc6c140ecb72b21cab06705a');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  '1a38bb91dbf862e46fc98be85e2ebb540de35da01fa69bebb899ee8b2c440a52',
  '70eb168ab1fb62363a2353fca5b3565b162ad26e383bf3632993e0ee595b34b8');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  '270e1d5da49b7c47b2cd1b5827890f1c0ba9ba039827a45169881537ab96c36d',
  '04fc9c3b7b0edb7ded34f516b73893dfff158e3e33714d01f2e61df015bdf0d0');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  'b2c86c8e0f4be2f9cc61e848ab0910ff36c4b1c923e439c9ddcbd4eba9998100',
  '16cfbd3c201b8bd388bd5a25b941736c64874bd165e2c17997452aad49a7eca8');

++$test;
testFilter('input.miff', 'Trim',
  q/0/,
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  'cc4cd6466273a0a0212cd29e0b628d52068dd375e2d7e641f26c36d348ca07d0',
  '2b349dd31b1ad4e304abebe4e2f75fff9fdb60a48f951884e81661c46ad1726a');

++$test;

1;
