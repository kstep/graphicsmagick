#!/usr/local/bin/perl
#
# Overall demo of the major PerlMagick methods.
#
use Image::Magick;

#
# Read model & smile image.
#
print "Read...\n";
$null=Image::Magick->new;
$null->Set(size=>'70x70');
$x=$null->ReadImage('NULL:black');
warn "$x" if "$x";

$model=Image::Magick->new();
$x=$model->ReadImage('model.gif');
warn "$x" if "$x";
$model->Label('Magick');
$model->Set(bordercolor=>'black');
$model->Set(background=>'black');

$smile=Image::Magick->new;
$x=$smile->ReadImage('smile.gif');
warn "$x" if "$x";
$smile->Label('Smile');
$smile->Set(bordercolor=>'black');
#
# Create image stack.
#
print "Transform image...\n";
$images=Image::Magick->new();
$example=$null->Clone();
push(@$images,$example);
$example=$null->Clone();
push(@$images,$example);
$example=$null->Clone();
push(@$images,$example);
$example=$null->Clone();
push(@$images,$example);
$example=$null->Clone();
push(@$images,$example);

print "Add Noise...\n";
$example=$model->Clone();
$example->Label('Add Noise');
$example->AddNoise("LaplacianNoise");
push(@$images,$example);

print "Annotate...\n";
$example=$model->Clone();
$example->Label('Annotate');
$example->Annotate(text=>'Magick',geometry=>'+0+20',font=>'Generic.ttf',
  fill=>'gold',gravity=>'North',pointsize=>14);
push(@$images,$example);

print "Blur...\n";
$example=$model->Clone();
$example->Label('Blur');
$example->Blur('0.0x1.0');
push(@$images,$example);

print "Border...\n";
$example=$model->Clone();
$example->Label('Border');
$example->Border(geometry=>'6x6',color=>'gold');
push(@$images,$example);

print "Channel...\n";
$example=$model->Clone();
$example->Label('Channel');
$example->Channel();
push(@$images,$example);

print "Charcoal...\n";
$example=$model->Clone();
$example->Label('Charcoal');
$example->Charcoal();
push(@$images,$example);

print "Composite...\n";
$example=$model->Clone();
$example->Label('Composite');
$example->Composite(image=>$smile,compose=>'over',geometry=>'+35+65');
push(@$images,$example);

print "Contrast...\n";
$example=$model->Clone();
$example->Label('Contrast');
$example->Contrast();
push(@$images,$example);

print "Convolve...\n";
$example=$model->Clone();
$example->Label('Convolve');
$example->Convolve([1, 1, 1, 1, 4, 1, 1, 1, 1]);
push(@$images,$example);

print "Crop...\n";
$example=$model->Clone();
$example->Label('Crop');
$example->Crop(geometry=>'80x80+25+50');
push(@$images,$example);

print "Despeckle...\n";
$example=$model->Clone();
$example->Label('Despeckle');
$example->Despeckle();
push(@$images,$example);

print "Draw...\n";
$example=$model->Clone();
$example->Label('Draw');
$example->Draw(fill=>'none',stroke=>'gold',primitive=>'circle',
  points=>'60,90 60,120',strokewidth=>2);
push(@$images,$example);

print "Detect Edges...\n";
$example=$model->Clone();
$example->Label('Detect Edges');
$example->Edge();
push(@$images,$example);

print "Emboss...\n";
$example=$model->Clone();
$example->Label('Emboss');
$example->Emboss();

print "Equalize...\n";
push(@$images,$example);
$example=$model->Clone();
$example->Label('Equalize');
$example->Equalize();
push(@$images,$example);

print "Implode...\n";
$example=$model->Clone();
$example->Label('Explode');
$example->Set(background=>'#000000FF');
$example->Implode(-1);
push(@$images,$example);

print "Flip...\n";
$example=$model->Clone();
$example->Label('Flip');
$example->Flip();
push(@$images,$example);

print "Flop...\n";
$example=$model->Clone();
$example->Label('Flop');
$example->Flop();
push(@$images,$example);

print "Frame...\n";
$example=$model->Clone();
$example->Label('Frame');
$example->Frame('15x15+3+3');
push(@$images,$example);

print "Gamma...\n";
$example=$model->Clone();
$example->Label('Gamma');
$example->Gamma(1.6);
push(@$images,$example);

print "Gaussian Blur...\n";
$example=$model->Clone();
$example->Label('Gaussian Blur');
$example->GaussianBlur('0.0x1.5');
push(@$images,$example);

print "Gradient...\n";
$gradient=Image::Magick->new;
$gradient->Set(size=>'130x194');
$x=$gradient->ReadImage('gradient:#20a0ff-#ffff00');
warn "$x" if "$x";
$gradient->Label('Gradient');
push(@$images,$gradient);

print "Grayscale...\n";
$example=$model->Clone();
$example->Label('Grayscale');
$example->Quantize(colorspace=>'gray');
push(@$images,$example);

print "Implode...\n";
$example=$model->Clone();
$example->Label('Implode');
$example->Implode(0.5);
push(@$images,$example);

print "Median Filter...\n";
$example=$model->Clone();
$example->Label('Median Filter');
$example->MedianFilter();
push(@$images,$example);

print "Modulate...\n";
$example=$model->Clone();
$example->Label('Modulate');
$example->Modulate(brightness=>110,saturation=>110,hue=>110);
push(@$images,$example);
$example=$model->Clone();

print "Monochrome...\n";
$example->Label('Monochrome');
$example->Quantize(colorspace=>'gray',colors=>2,dither=>'false');
push(@$images,$example);

print "Negate...\n";
$example=$model->Clone();
$example->Label('Negate');
$example->Negate();
push(@$images,$example);

print "Normalize...\n";
$example=$model->Clone();
$example->Label('Normalize');
$example->Normalize();
push(@$images,$example);

print "Oil Paint...\n";
$example=$model->Clone();
$example->Label('Oil Paint');
$example->OilPaint();
push(@$images,$example);

print "Plasma...\n";
$plasma=Image::Magick->new;
$plasma->Set(size=>'130x194');
$x=$plasma->ReadImage('plasma:fractal');
warn "$x" if "$x";
$plasma->Label('Plasma');
push(@$images,$plasma);

print "Quantize...\n";
$example=$model->Clone();
$example->Label('Quantize');
$example->Quantize();
push(@$images,$example);

print "Raise...\n";
$example=$model->Clone();
$example->Label('Raise');
$example->Raise('10x10');
push(@$images,$example);

print "Reduce Noise...\n";
$example=$model->Clone();
$example->Label('Reduce Noise');
$example->ReduceNoise();
push(@$images,$example);

print "Resize...\n";
$example=$model->Clone();
$example->Label('Resize');
$example->Resize('50%');
push(@$images,$example);

print "Roll...\n";
$example=$model->Clone();
$example->Label('Roll');
$example->Roll(geometry=>'+20+10');
push(@$images,$example);

print "Rotate...\n";
$example=$model->Clone();
$example->Label('Rotate');
$example->Rotate(45);
$example->Transparent(color=>'black');
push(@$images,$example);

print "Scale...\n";
$example=$model->Clone();
$example->Label('Scale');
$example->Scale('60%');
push(@$images,$example);

print "Segment...\n";
$example=$model->Clone();
$example->Label('Segment');
$example->Segment();
push(@$images,$example);

print "Shade...\n";
$example=$model->Clone();
$example->Label('Shade');
$example->Shade(geometry=>'30x30',color=>'false');
push(@$images,$example);

print "Sharpen...\n";
$example=$model->Clone();
$example->Label('Sharpen');
$example->Sharpen('0.0x1.0');
push(@$images,$example);

print "Shave...\n";
$example=$model->Clone();
$example->Label('Shave');
$example->Shave('10x10');
push(@$images,$example);

print "Shear...\n";
$example=$model->Clone();
$example->Label('Shear');
$example->Shear('45x45');
$example->Transparent(color=>'black');
push(@$images,$example);

print "Spread...\n";
$example=$model->Clone();
$example->Label('Spread');
$example->Spread();
push(@$images,$example);

print "Solarize...\n";
$example=$model->Clone();
$example->Label('Solarize');
$example->Solarize();
push(@$images,$example);

print "Swirl...\n";
$example=$model->Clone();
$example->Set(background=>'#000000FF');
$example->Label('Swirl');
$example->Swirl(90);
push(@$images,$example);

print "Unsharp Mask...\n";
$example=$model->Clone();
$example->Label('Unsharp Mask');
$example->UnsharpMask('0.0x1.0');
push(@$images,$example);

print "Wave...\n";
$example=$model->Clone();
$example->Label('Wave');
$example->Set(background=>'#000000FF');
$example->Wave('25x150');
push(@$images,$example);
#
# Create image montage.
#
print "Montage...\n";
$montage=$images->Montage(geometry=>'130x194+10+5>',gravity=>'Center',
  bordercolor=>'green',borderwidth=>1,tile=>'5x1000',compose=>'over',
  background=>'#ffffff',font=>'Generic.ttf',pointsize=>18,fill=>'#600',
  stroke=>'none');

$logo=Image::Magick->new();
$logo->Read('logo:');
$logo->Zoom('40%');
$montage->Composite(image=>$logo,gravity=>'North');

print "Write...\n";
$montage->Set(matte=>'false');
$montage->Write('demo.jpg');
print "Display...\n";
$montage->Write('win:');
