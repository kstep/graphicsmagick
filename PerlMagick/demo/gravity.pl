#!/usr/local/bin/perl

use Image::Magick;

$font = '-adobe-helvetica-medium-r-normal--25-180-100-100-p-130-iso8729-1';
$font = 'Arial';
$font = 'Arial.ttf';

$image = Image::Magick->new();
$x = 100;
$y = 100;
for ($angle=0; $angle < 360; $angle+=30)
{
  print "angle $angle\n";
  $label = Image::Magick->new(size=>"600x600",density=>72,pointsize=>30,
    font=>$font,fill=>'black');
  $label->Read("xc:white");
  $label->Draw(primitive=>'line',points=>"300,100 300,500",stroke=>'#600');
  $label->Draw(primitive=>'line',points=>"100,300 500,300",stroke=>'#600');
  $label->Draw(primitive=>'rectangle',points=>"100,100 500,500",fill=>'none',
    stroke=>'#600');
  $label->Annotate(text=>"Cyborg",gravity=>"NorthWest",x=>$x,y=>$y,
    rotate=>$angle);
  $label->Annotate(text=>"Cyborg",gravity=>"North",x=>$x,y=>$y,
    rotate=>$angle);
  $label->Annotate(text=>"Cyborg",gravity=>"NorthEast",x=>$x,y=>$y,
    rotate=>$angle);
  $label->Annotate(text=>"Cyborg",gravity=>"East",x=>$x,y=>$y,
    rotate=>$angle);
  $label->Annotate(text=>"Cyborg",gravity=>"Center",x=>$x,y=>$y,
    rotate=>$angle);
  $label->Annotate(text=>"Cyborg",gravity=>"SouthEast",x=>$x,y=>$y,
    rotate=>$angle);
  $label->Annotate(text=>"Cyborg",gravity=>"South",x=>$x,y=>$y,rotate=>$angle);
  $label->Annotate(text=>"Cyborg",gravity=>"SouthWest",x=>$x,y=>$y,
    rotate=>$angle);
  $label->Annotate(text=>"Cyborg",gravity=>"West",x=>$x,y=>$y,rotate=>$angle);
  push(@$image,$label);
}
$image->Set(delay=>20);
$image->Write(filename=>"gravity.miff",compress=>'RunlengthEncoded');
system("animate gravity.miff");
