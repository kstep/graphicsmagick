// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// PerlMagick "piddle" demo re-implemented using Magick++ methods.
// The PerlMagick "piddle" demo is written by John Cristy
//

#include <string>
#include <iostream>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char ** argv)
{

  // Initialize ImageMagick install location for Windows
  MagickIncarnate(*argv);

  string srcdir("");
  if(getenv("srcdir") != (char*)NULL)
    srcdir = getenv("srcdir") + string("/");

  // Common font to use.
  string font = string("@") + srcdir + "Generic.ttf";

  try {

    //
    // Create a 300x300 white canvas.
    //
    Image image( "300x300", "white" );

    //
    // Draw blue grid
    //
    {
      std::list<Magick::Drawable> drawList;
      drawList.push_back(DrawableStrokeColor("#ccf"));
      for ( int i=0; i < 300; i += 10 )
        {
          drawList.push_back(DrawableLine(i,0, i,300));
          drawList.push_back(DrawableLine(0,i, 300,i));
        }
      image.draw(drawList);
    }

    //
    // Draw rounded rectangle.
    //
    {
//       image.fillColor("blue");
//       image.strokeColor("red");
//       image.draw(DrawableRoundRectangle(15,15, 70,70, 10,10));

//       image.fillColor("blue");
//       image.strokeColor("maroon");
//       image.lineWidth(4);
//       image.draw(DrawableRoundRectangle(15,15, 70,70, 10,10));

      std::list<Magick::Drawable> drawList;
      drawList.push_back(DrawableFillColor("blue"));
      drawList.push_back(DrawableStrokeColor("red"));
      drawList.push_back(DrawableRoundRectangle(15,15, 70,70, 10,10));

      drawList.push_back(DrawableFillColor("blue"));
      drawList.push_back(DrawableStrokeColor("maroon"));
      drawList.push_back(DrawableStrokeWidth(4));
      drawList.push_back(DrawableRoundRectangle(15,15, 70,70, 10,10));
      image.draw(drawList);
    }

    //
    // Draw curve.
    //
    {
      std::list<Magick::Coordinate> points;
      image.strokeColor("black");
      image.fillColor(Color()); // Unset
      image.lineWidth(4);
      points.push_back(Coordinate(20,20));
      points.push_back(Coordinate(100,50));
      points.push_back(Coordinate(50,100));
      points.push_back(Coordinate(160,160));
      
      image.draw(DrawableBezier(points));
    }

    //     cout << "Writing image \"piddle.miff\" ..." << endl;
    //     image.write( "piddle.miff" );

    cout << "Display image..." << endl;
    image.display( );

  }
  catch( Exception error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }
  catch( exception error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }
  
  return 0;
}
