#!/usr/local/bin/perl
# Piddle example using PerlMagick methods.

use Image::Magick;

#
# Create white canvas.
#
$image=Image::Magick->new(size=>'300x300');
$image->Read('xc:white');
#
# Draw blue grid
#
for ($i=0; $i < 300; $i+=10)
{
  $image->Draw(primitive=>'line',points=>"$i,0 $i,300",stroke=>"#ccf");
  $image->Draw(primitive=>'line',points=>"0,$i 300,$i",stroke=>"#ccf");
}
#
# Draw rounded rectangle.
#
$image->Draw(primitve=>'RoundRectangle',fill=>'blue',stroke=>'maroon',
  stroke_width=>4,points=>'30,30 100,100 10,10');
#
# Draw curve.
#
$image->Draw(primitve=>'bezier',points=>'20,20, 100,50, 50,100, 160,160',
  fill=>'none',stroke=>'black',stroke_width=>4);
#
# Draw line.
#
$image->Draw(primitive=>'line',points=>"10,200 20,190",stroke=>red);
#
# Draw arc within a circle.
#
$image->Draw(primitive=>'circle',stroke=>'none',fill=>'yellow',,
  points=>"170,70 200,70");
$image->Draw(primitve=>'Path',stroke=>'none',fill=>'blue',stroke_width=>4,
  points=>'M170,70 v-30 a30,30 0 0,0 -30,30 z');
$image->Draw(primitive=>'circle',stroke=>'black',fill=>'none',stroke_width=>4,
  points=>"170,70 200,70");
#
# Draw pentogram.
#
$image->Draw(primitive=>'polygon',
  points=>"160,120 130,190 210,145 110,145 190,190 160,120",stroke=>red,
  fill=>LimeGreen,stroke_width=>3);
#
# Draw rectangle.
#
$image->Draw(primitve=>'line',points=>'200,260 200,200',stroke=>yellow,
  stroke_width=>5);
$image->Draw(primitve=>'line',points=>'200,200 260,200',stroke=>yellow,
  stroke_width=>5);
$image->Draw(primitve=>'line',points=>'260,200 260,260',stroke=>red,
  stroke_width=>5);
$image->Draw(primitve=>'line',points=>'200,260 260,260',stroke=>green,
  stroke_width=>5);
#
# Draw text.
#
$image->Annotate(text=>'This is a test!',geometry=>'+30+140',
  font=>'Generic.ttf',fill=>'green',pointsize=>24,rotate=>45.0);
$image->Display();
