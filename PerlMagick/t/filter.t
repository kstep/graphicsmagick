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
  'a0e5624d4bc5b48a62abf0a80f2d77808b5462955e80227c1af8c7efe5f8c6df',
  '39adcbabb6f04c63f4a6ff7df27472ce3dfa2271252b618750b8e283b5fd8124');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '886abd0600151937beea6143c6c8ab149624ceddc40076cb28072a4c5c904bcf');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '8c63f4f807d48648c73f2eee9c278f068deae3093b4d917d91eff2f5e9ed8551',
  'a1496ec3554953265568f18d175d30cd227ca53b8337149daf6b4ae196418e81');

++$test;
testFilter('input.miff', 'Edge',
  q/3/,
  '077ae4e1fa8db5e76a60a5764faf3b50fb15a9db0a3eb6d93f50bdbb3455f361',
  '400576abfb90419a7d1fa40febff664150397f3e272c87a2113ac056cbc1ccbe');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  '2d69baf1dd0c3f6bf4bae4e6956f540641f380072a36d7d41084a26e84bf8944',
  'f8e18973054fc25d4b93d895216ca9783ca0cea4d06e8132fb08774bff27015e');

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
  '55968aaed92f68054b058b1e69e569464441a25c9db3973dc0f7202199856440',
  'be9d900c168711ced80fece0a8a7f9b51aba59322f8e23810155abf72886ec14');

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
  '7676bc46a8915b2309d3100685b4ebb3ba26c3f431dc0b23cad568ae61af1605',
  '76496d84270ffc34b3341bb37549b6a52feca578ba9255569ae3b680112c3f9b');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  'c492f7a1938e6ee5676c3eead3eb1754d8c5b59bffa132efda0dd49f436a210a');

++$test;
testFilter('input.miff', 'Rotate',
  q/10/,
  'cf2020a876d33aca25fe75210890e2f621a510e8e167ae750c26a13db92a9c31',
  '21579b7545e251c5d5a52e0911874577392d7b28004dbf4042ec04a53be0798c');

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
  '5de54151157c7a7f0ee5dae746f8a122c304e4c262b1e656b04277c1f54c780a',
  '5704ba4b386315c284371eff8ba549896181d35948ae29dcdc11e923e040b786');

++$test;
testFilter('input.miff', 'Shave',
  q/10x10/,
  '1f3cb0be4e297e1fc20ee7b2fedf7ea6d4c8b5b91856e4f7be44ddd278b8ff81');

++$test;
testFilter('input.miff', 'Shear',
  q/10/,
  '70f708b88623141ad6ce1d98dd83ece931bfdc62e48f77a5416f115f8bf3abe3',
  '7194a11c4b46dfae686578212df14f0fe3fdc30b4bb83b9ef003fadd637b2481');

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
