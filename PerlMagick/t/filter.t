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
  '3f4669fa6a5d1920c8f9d21a872eb44a364498f37ee1e31c572a5b7f231f4e58');

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
  '6ff9724ada6de2f830c031231e4c61f3c82a5e9e7f9efbe4e6c507d57f8c9ca2');

++$test;
testFilter('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  'a1d263752e2d4081ba1c4e5ca5be3283d9d662c96e43328c33cba8451fcaaa2f',
  '4f5574a2c1f9d7e45e2c965b8d1308c520157b1bbff5dcddcbda5ac0a5c6e775');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '886abd0600151937beea6143c6c8ab149624ceddc40076cb28072a4c5c904bcf');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '8c63f4f807d48648c73f2eee9c278f068deae3093b4d917d91eff2f5e9ed8551',
  '7594237d2167b5508b41cacaa6c65cd5aa37a3fd065bb7dc7a84f894eaabe459');

++$test;
testFilter('input.miff', 'Edge',
  q/3/,
  '077ae4e1fa8db5e76a60a5764faf3b50fb15a9db0a3eb6d93f50bdbb3455f361',
  '0b7cb9e513f657952560c327dac8ae91ce4ce0374f970b5e8893eb6accde31d2');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  'd68ffa55a526c6cef59cfa18866a1a617b1065eb5a2975106b191d0535b0b477',
  '22ecee4e2e73a7075cd43051a0e29d3b4e5e2039fbd8778664104008c295eb1a');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  'eef347707dd430c87fc90f63445fe4bf4e59e64d60d51db43cb9aae57f960e48',
  '1e689587375f6e2405d7cebd0d09f1d70ff6036ccbad6a63a7d503f3182b1b0b');

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
  '655dd989086af4bf396b9403a89375074ea5a2555fa51aeb7dfa5bdb42de3d93');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  '8f79b178b0c64f6b063a10ad01c7b4828307765b8ce24d6d100a22bd7d91da60',
  '9399a4885ab079ca13a5714e9082d1a5c8ad647bf577772067c3bf0b34fee552');

++$test;
testFilter('input.miff', 'Implode',
  q/0.5/,
  '587985c66eec744fa48e8b6acafaa2bb5c789472134fb89355351179bfb10997',
  '1da73d077fe8302b808d1d5977e814a853d2c91a8967dfaa4acc9e7f4b7b4e79');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'e5e4dbaaea4faba87326d1ccde1f84aaa60c41518bd242f4d65dd9f37d2f2dc5',
  'c8473f332eb313e356cdc0235d3e8d6d4c8881dedad079079d75abbc0f722b76');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/+25+45/,
  '31c493286e8bd7ba5a8f36ba36edb05e1f8a58442347fd0021fb9c3a6a692822',
  '100a892e7d683a23a73ea67fef7efc7b57d48efc1e1bba4cb7117e6ce371a3fe');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  'e2cc6133847f02d4ae3834919ef8329de7b4876581b06dbc46802ca7bbb0ffec',
  '294a0ee1df11c06e322a5af1851fa57a6bcf823a242e78db7fab1c23c6cfacd8');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '6e46f9e5399b8265a1c522750dbdd227d1dcae084d18f00a79b27b5638670bc5',
  '4159e1036b104c4d08ffe2039f50a022c5212b5c2bc79f40f01da3e247589788');

++$test;
testFilter('input.miff', 'Negate',
  q/False/,
  '70eb168ab1fb62363a2353fca5b3565b162ad26e383bf3632993e0ee595b34b8');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  'a5dfb674174da975a35661b907da335ea8e5ee1cdd015dbe87ddeaead3c54389',
  '56e0d852ec546fd70672817d6448bb6e0ccfaabbafcadbba51c015e7b4d0386a');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  '46616c1994530429629966b77afb9a77692335eee2fec1e40b719452d31e9109',
  '2671a81c3a4a860a36c8b1d2f7f67181eabef52c907323d7374931cfc1f35a11');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  'a690f2732ba25e3a101ba3d166b5c9345198606d73eae0f7c8b7fc8e61bbba01',
  'd139c755892ff123818fb09ff013891b44bfd1e0aea54b0cd7cb0e0998a4d13a');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  '0c363702b4b6a27a629426255ce71174a3996580ac5211bf6298f63e655bc400',
  '4aece83f1fe7f48f89c6764bfe06f7e8dda7c4ebc256c99a928980b76aaa31c1');

++$test;
testFilter('input.miff', 'Resize',
  q/50%/,
  '94238afde30c971fe52be34f540bdd7c75a82befc93a3f5ac5c49194716a48d5',
  '2c83ad4a841f4e37c5d3125bf2d5d474f5243d41beb1b423a85fd60c69ea4928');

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
  '39aaac0a23d3ddfd6c8735156f831b78e9bf55cf8a2e67ef70a6c3e8508dfe75');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  'fa219869b3cc8560def29c2f28974d75d3eede767c70d784be931fe44c6b1172');

++$test;
testFilter('input.miff', 'Shade',
  q/30/,
  'd539e24d03ae74330991d28c440bb69d629ddc491689e95290068a5e4e38eabe',
  '06613d5f5ea0b6985c552c6a06317c23c93c399e6d72baacc370f1d704b63301');

++$test;
testFilter('input.miff', 'Sharpen',
  q/0.0x1.0/,
  'eaeea39e3b2a33b9d8761a70a57717214fd4b39944df47bd21a776f4ac2dfa58',
  'f108d3e09a002fbcb820dd633049b7b315ac77d7b9146feb393bef0e13720fb2');

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
  'ceabf4ee59fee83ad7a0900363dd01bdae60f0c7e0ab6c9c68afb29dbce6b2b0');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  'b2c86c8e0f4be2f9cc61e848ab0910ff36c4b1c923e439c9ddcbd4eba9998100',
  '85aaa3903e52aeb07f1905b3338b576ec0e9fcf0d67baa95872b534a79b42236');

++$test;
testFilter('input.miff', 'Trim',
  q/0/,
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  'cc4cd6466273a0a0212cd29e0b628d52068dd375e2d7e641f26c36d348ca07d0',
  'a317ffc70e89862fd893e0b8dbe27333dae8c605fff9b6aae3657f89f83c5ed7');

++$test;

1;
