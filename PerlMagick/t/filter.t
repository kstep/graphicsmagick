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
  '4c9b95c87366a150088e5a2ad71031ff97f2cfd1cfcad10b6c1b834f5a5bf249',
  '3f4669fa6a5d1920c8f9d21a872eb44a364498f37ee1e31c572a5b7f231f4e58');

++$test;
testFilter('input.miff', 'Border',
  q/10/,
  '587d880964499064191370e24ac9504be526be7c9bdd1fb631dd2df3f049d3cf',
  '587d880964499064191370e24ac9504be526be7c9bdd1fb631dd2df3f049d3cf');

++$test;
testFilter('input.miff', 'Channel',
  q/Red/,
  'c86b06e9a85047575a64653b2bcda841b66b037a95de9fccc831f6f2400f82aa');

++$test;
testFilter('input.miff', 'Chop',
  q/+10+10/,
  '52ebbcbba66092875a763bbb5177f835e5fcc78002ebc1a533b61fb1d9c80aaa');

++$test;
testFilter('input.miff', 'ColorFloodfill',
  q/+25+45/,
  '4acbc1f121024a9dc6999970bf59a569a7c9296108a04a6bd2dbba84c88e0dfa',
  '4acbc1f121024a9dc6999970bf59a569a7c9296108a04a6bd2dbba84c88e0dfa');

++$test;
testFilter('input.miff', 'Colorize',
  q/red/,
  '23ea99eada5c88384c229d80a157635e6c6c342642efed1948f7251702e18279',
  '23ea99eada5c88384c229d80a157635e6c6c342642efed1948f7251702e18279');

++$test;
testFilter('input.miff', 'Contrast',
  q/1/,
  'ea2f98e22af6318c285c2135e0ada2660cc49d5358b6247fdfb2f78fe33416fa',
  '6ff9724ada6de2f830c031231e4c61f3c82a5e9e7f9efbe4e6c507d57f8c9ca2');

++$test;
testFilter('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  'e9a8236711bf0656818525f2d79f9b3bfc9d259d08b93d9b1db471db71f71b36',
  '4f5574a2c1f9d7e45e2c965b8d1308c520157b1bbff5dcddcbda5ac0a5c6e775');

++$test;
testFilter('input.miff', 'Crop',
  q/10x10/,
  '52ebbcbba66092875a763bbb5177f835e5fcc78002ebc1a533b61fb1d9c80aaa');

++$test;
testFilter('input.miff', 'Despeckle',
  q/ /,
  '1de7142485bc03ce3f7c82ff55bcb69f9e6d50627ff1979fe327476d0d1d1109',
  '7594237d2167b5508b41cacaa6c65cd5aa37a3fd065bb7dc7a84f894eaabe459');

++$test;
testFilter('input.miff', 'Edge',
  q/3/,
  'e9a8236711bf0656818525f2d79f9b3bfc9d259d08b93d9b1db471db71f71b36',
  '70faa3bd4d2df8dd918a0ee136eccd5149e9777293096bd202a68a4aeeb25a83');

++$test;
testFilter('input.miff', 'Emboss',
  q/3/,
  '9fd7f4e612897c42f4226bbc2a34a856cd9d905a10555376c2f67ca763f848fc',
  '22ecee4e2e73a7075cd43051a0e29d3b4e5e2039fbd8778664104008c295eb1a');

++$test;
testFilter('input.miff', 'Equalize',
  q/ /,
  '428bca0b3ce457fd42ac672c2e61a5ba1cc598f6234a0f5131429db719367d0d',
  '1e689587375f6e2405d7cebd0d09f1d70ff6036ccbad6a63a7d503f3182b1b0b');

++$test;
testFilter('input.miff', 'Flip',
  q/ /,
  '39d5630424917124393097166e291e884498bd3954d44242ffee60a1a6548a43');

++$test;
testFilter('input.miff', 'Flop',
  q/ /,
  '6d245f3a1063f611d400b3c9ec0b8687bc4428de655cc4194600c44f557363b1');

++$test;
testFilter('input.miff', 'Frame',
  q/10x10/,
  '6c3c5e29874ec030a337be00044d81f1a42b52a6bddb67d17d0adaafa34fed15', 
  '655dd989086af4bf396b9403a89375074ea5a2555fa51aeb7dfa5bdb42de3d93');

++$test;
testFilter('input.miff', 'Gamma',
  q/2.2/,
  'cb240877838165536bcc4445e4338c8c2aa923cb5e9f9eadc64d98661fd279f5',
  '9399a4885ab079ca13a5714e9082d1a5c8ad647bf577772067c3bf0b34fee552');

++$test;
testFilter('input.miff', 'Implode',
  q/0.5/,
  '6e2b5da51c519b0da5dfdd8b6db737ebe545447dc3779107448378a2d295721b',
  '1da73d077fe8302b808d1d5977e814a853d2c91a8967dfaa4acc9e7f4b7b4e79');

++$test;
testFilter('input.miff', 'Magnify',
  q/ /,
  'ba0d9a4e37eef7397dba998bd8c439856b0dc5628376adc7eee347ce4134dab4',
  'c8473f332eb313e356cdc0235d3e8d6d4c8881dedad079079d75abbc0f722b76');

++$test;
testFilter('input.miff', 'MatteFloodfill',
  q/+25+45/,
  'ba0d9a4e37eef7397dba998bd8c439856b0dc5628376adc7eee347ce4134dab4',
  '100a892e7d683a23a73ea67fef7efc7b57d48efc1e1bba4cb7117e6ce371a3fe');

++$test;
testFilter('input.miff', 'Minify',
  q/ /,
  '62fe2660d7b0b693f965c72128ba4ba2d0b8869928ac438f985951d60c4cfef5',
  '294a0ee1df11c06e322a5af1851fa57a6bcf823a242e78db7fab1c23c6cfacd8');

++$test;
testFilter('input.miff', 'Modulate',
  q/60/,
  '9eb7e44e6c093d85cbfd8458b3c1f5e59e2dbda2354922a1bab99185f6394f4d',
  '4159e1036b104c4d08ffe2039f50a022c5212b5c2bc79f40f01da3e247589788');

++$test;
testFilter('input.miff', 'Negate',
  q/False/,
  'd4a746563c80415efad4f45865d66ad918070465a55c207a85046636a02f171a');

++$test;
testFilter('input.miff', 'Normalize',
  q/ /,
  '91d1b271fa7b32b70349af54ac2a339f62174e4511d5d0f11299c527ab3663ec',
  '56e0d852ec546fd70672817d6448bb6e0ccfaabbafcadbba51c015e7b4d0386a');

++$test;
testFilter('input.miff', 'OilPaint',
  q/3/,
  'caeccb8ca9a5cfe17cf68a3894dd14d1ebcc1843c254153072b3d2da8f1fb9f1',
  '2671a81c3a4a860a36c8b1d2f7f67181eabef52c907323d7374931cfc1f35a11');

++$test;
testFilter('input.miff', 'Opaque',
  q/#fefefd/,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

++$test;
testFilter('input.miff', 'Quantize',
  q/128/,
  '12515aea70b913b661a450475757fca1e19eb9393370181c75e95ebf08f7bed8',
  'd139c755892ff123818fb09ff013891b44bfd1e0aea54b0cd7cb0e0998a4d13a');

++$test;
testFilter('input.miff', 'Raise',
  q/10x10/,
  'eb9d6cfd06988016e4e2540ddd27c2134dd49642a4bfbdbd33b29882511366b3',
  '4aece83f1fe7f48f89c6764bfe06f7e8dda7c4ebc256c99a928980b76aaa31c1');

++$test;
testFilter('input.miff', 'Resize',
  q/50%/,
  'ed2c1b52965a313e7e68325961b484c0d95e9f19bded9e7f09ef0b8d9b4a624f',
  '2c83ad4a841f4e37c5d3125bf2d5d474f5243d41beb1b423a85fd60c69ea4928');

++$test;
testFilter('input.miff', 'Roll',
  q/+10+10/,
  '1d23c7183c420c73c2181eecd0037d5521ab25f960c3c0654304d46ba3e9b563');

++$test;
testFilter('input.miff', 'Rotate',
  q/10/,
  'e6edc84899ad7506cd939cb36f028b34af20ae88bda7307673259880c439b460',
  '3c2f77dc24f6a30ddc6f78a6e8071909344651f76c4da390c8052768ffde1911');

++$test;
testFilter('input.miff', 'Sample',
  q/50%/,
  '3287c61ff84fe3e8d7e7086d924a8469627e4bb3fddf029231fe9522cbe7ee90');

++$test;
testFilter('input.miff', 'Scale',
  q/50%/,
  '35e6f6d33d1612b02486c2800a3916751cfc6d2d45b61a0dbeca7b2a3cdf026d',
  '39aaac0a23d3ddfd6c8735156f831b78e9bf55cf8a2e67ef70a6c3e8508dfe75');

++$test;
testFilter('input.miff', 'Segment',
  q/1/,
  '13682533b7380eec89460d2e995e9025bd5eb1f56c2de702c0e44f0033b1c55a');

++$test;
testFilter('input.miff', 'Shade',
  q/30/,
  '7c3dda51f6f939dc23b892ea1157e01bbfa523184446927cfe3693caf46cf88a',
  '06613d5f5ea0b6985c552c6a06317c23c93c399e6d72baacc370f1d704b63301');

++$test;
testFilter('input.miff', 'Sharpen',
  q/0.0x1.0/,
  '9fe2eecec8e4b42388b6251a25fb8a6526d3882cb490868f648c1766404799cf',
  'f108d3e09a002fbcb820dd633049b7b315ac77d7b9146feb393bef0e13720fb2');

++$test;
testFilter('input.miff', 'Shave',
  q/10x10/,
  '5ffcab00c997ac48a98f7829da8579e87e1b3a407a1db83097f70bad0c07ee41');

++$test;
testFilter('input.miff', 'Shear',
  q/10/,
  '961cbb9642ee5499af3e7e43be214e6ddbd2cfe00c6f239a2236895b4864c21d',
  '41beaaf308f7f6662bed19143a8b3f60805fcd57cc6c140ecb72b21cab06705a');

++$test;
testFilter('input.miff', 'Solarize',
  q/60/,
  'c7078cc5d28b543e82e4b1de16baa61f2268620c91555deb34013b8b23246e25',
  'd4a746563c80415efad4f45865d66ad918070465a55c207a85046636a02f171a');

++$test;
testFilter('input.miff', 'Swirl',
  q/60/,
  'b4cd909d5c16fcb31c5acd03f4067c3019ce0798e3697ef87b305f16611078fe',
  'ceabf4ee59fee83ad7a0900363dd01bdae60f0c7e0ab6c9c68afb29dbce6b2b0');

++$test;
testFilter('input.miff', 'Threshold',
  q/128/,
  '06f1704512c4e47f5af5333f8e05a14941677f3825f3e8691de33ca48d489601',
  '85aaa3903e52aeb07f1905b3338b576ec0e9fcf0d67baa95872b534a79b42236');

++$test;
testFilter('input.miff', 'Trim',
  q/0/,
  '25b14945dc0131b7010b1fadb5249ad38ad9b8fdc3e0c7a9c56590940fd9734a');

++$test;
testFilter('input.miff', 'Wave',
  q/25x157/,
  '119b9e74e7b77e8df87d540cad4af4a3629c277efc60507c11f5d5a86f86366e',
  'a317ffc70e89862fd893e0b8dbe27333dae8c605fff9b6aae3657f89f83c5ed7');

++$test;

1;
