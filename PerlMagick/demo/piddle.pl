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
  $image->Draw(primitive=>'line',points=>"$i,0 $i,300",pen=>"#ccf");
  $image->Draw(primitive=>'line',points=>"0,$i 300,$i",pen=>"#ccf");
}
#
# Draw rounded rectangle.
#
$image->Draw(pen=>blue,primitve=>'RoundRectangle',fill=>'True',
  points=>'65,65 70,70 10,10');
$image->Draw(pen=>maroon,primitve=>'roundRectangle',points=>'65,65 70,70 10,10',
  linewidth=>4);
#
# Draw curve.
#
$image->Draw(primitve=>'bezier',points=>'20,20, 100,50, 50,100, 160,160',
  pen=>black,linewidth=>4);
#
# Draw line.
#
$image->Draw(primitive=>'line',points=>"10,200 20,190",pen=>red);
#
# Draw arc within a circle.
#
$image->Draw(pen=>yellow,primitive=>'circle',fill=>'True',
  points=>"160,70 200,70");
$image->Draw(pen=>blue,primitve=>'Arc',fill=>'True',
  points=>'120,30 200,100 45,90');
$image->Draw(pen=>navy,primitve=>'Arc',points=>'120,30 200,100 45,90',
  linewidth=>4);
$image->Draw(pen=>black,primitive=>'circle',points=>"160,70 200,70",
  linewidth=>4);
#
# Draw pentogram.
#
$image->Draw(primitive=>'polygon',
  points=>"160,120 130,190 210,145 110,145 190,190 160,120",pen=>red,
  linewidth=>3);
$image->Draw(primitive=>'color',points=>'160,140',pen=>LimeGreen,
  method=>'filltoborder',bordercolor=>red);
$image->Draw(primitive=>'color',points=>'190,155',pen=>LimeGreen,
  method=>'filltoborder',bordercolor=>red);
$image->Draw(primitive=>'color',points=>'140,170',pen=>LimeGreen,
  method=>'filltoborder',bordercolor=>red);
$image->Draw(primitive=>'color',points=>'130,155',pen=>LimeGreen,
  method=>'filltoborder',bordercolor=>red);
$image->Draw(primitive=>'color',points=>'180,170',pen=>LimeGreen,
  method=>'filltoborder',bordercolor=>red);
#
# Draw rectangle.
#
$image->Draw(primitve=>'rectangle',points=>'200,200 260,260',pen=>yellow,
  linewidth=>5);
$image->Draw(primitve=>'line',points=>'200,260 260,260',pen=>green,
  linewidth=>5);
$image->Draw(primitve=>'line',points=>'260,200 260,260',pen=>red,linewidth=>5);
#
# Draw text.
#
$image->Annotate(text=>'This is a test!',geometry=>'+30+130',
  font=>'@Generic.ttf',pen=>'green',pointsize=>18,degrees=>45.0);
$image->Display();
