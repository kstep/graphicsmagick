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
$image->Draw(primitve=>'RoundRectangle',fill=>blue,stroke=>'red',
  points=>'65,65 70,70 10,10');
$image->Draw(primitve=>'roundRectangle',stroke=>maroon,linewidth=>4,
  points=>'65,65 70,70 10,10');
#
# Draw curve.
#
$image->Draw(primitve=>'bezier',points=>'20,20, 100,50, 50,100, 160,160',
  stroke=>black,linewidth=>4);
#
# Draw line.
#
$image->Draw(primitive=>'line',points=>"10,200 20,190",stroke=>red);
#
# Draw arc within a circle.
#
$image->Draw(primitive=>'circle',stroke=>'black',fill=>'yellow',linewidth=>4,
  points=>"160,70 200,70");
$image->Draw(primitve=>'Arc',stroke=>'black',fill=>'blue',linewidth=>4,
  points=>'120,30 200,100 45,90');
#
# Draw pentogram.
#
$image->Draw(primitive=>'polygon',
  points=>"160,120 130,190 210,145 110,145 190,190 160,120",stroke=>red,
  fill=>LimeGreen,linewidth=>3);
#
# Draw rectangle.
#
$image->Draw(primitve=>'rectangle',points=>'200,200 260,260',stroke=>yellow,
  linewidth=>5);
$image->Draw(primitve=>'line',points=>'200,260 260,260',stroke=>green,
  linewidth=>5);
$image->Draw(primitve=>'line',points=>'260,200 260,260',stroke=>red,linewidth=>5);
#
# Draw text.
#
$image->Annotate(text=>'This is a test!',geometry=>'+30+140',
  font=>'@Generic.ttf',fill=>'green',pointsize=>24,rotate=>45.0);
$image->Display();
