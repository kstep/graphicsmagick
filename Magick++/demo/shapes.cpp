// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// GD/PerlMagick example using Magick++ methods.
//
// Concept and algorithms lifted from PerlMagick demo script
//

#include <string>
#include <iostream>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char ** /*argv*/)
{

  try {

    //
    // Create a 300x300 white canvas.
    //
    Image image( "300x300", "white" );


    //
    // Draw texture-filled polygon
    //
    // Polygon list
    std::list<Coordinate> poly_coord;
    poly_coord.push_back( Coordinate(30,30) );
    poly_coord.push_back( Coordinate(100,10) );
    poly_coord.push_back( Coordinate(190,290) );
    poly_coord.push_back( Coordinate(30,290) );

    Image texture( "tile.miff" );
    image.penTexture( texture );
    image.draw( DrawablePolygon( poly_coord ) );
    texture.isValid( false );
    image.penTexture( texture );  // Unset texture

    //
    // Draw filled ellipse with black border, and red fill color
    //
    image.strokeColor( "black" );
    image.fillColor( "red" );
    image.lineWidth( 5 );
    image.draw( DrawableEllipse( 100,100, 100,150, 0,360 ) );
    image.fillColor( Color() ); // Clear out fill color

    //
    // Draw ellipse, and polygon, with black stroke, linewidth of 5
    //
    image.strokeColor( "black" );
    image.lineWidth( 5 );
    list<Drawable> drawlist;

    // Add ellipse to list
    drawlist.push_back( DrawableEllipse( 100,100, 100,150, 0,360 ) );

    // Add polygon to list
    poly_coord.clear();
    poly_coord.push_back( Coordinate(30,30) );
    poly_coord.push_back( Coordinate(100,10) );
    poly_coord.push_back( Coordinate(190,290) );
    poly_coord.push_back( Coordinate(30,290) );
    drawlist.push_back( DrawablePolygon( poly_coord ) );
    image.draw( drawlist );

    //
    // Floodfill object with blue
    //
    image.colorFuzz( 80 );
    image.floodFillColor( "+132+62", "blue" );

    //
    // Draw text
    //
    image.fillColor( "red" );
    image.font( "@Generic.ttf" );
    image.fontPointsize( 12 );
    image.annotate( "Hello world!", "+150+10" );

    image.fillColor( "blue" );
    image.font( "@Generic.ttf" );
    image.fontPointsize( 10 );
    image.annotate( "Goodbye cruel world!", "+150+28" );

    image.fillColor( "black" );
    image.font( "@Generic.ttf" );
    image.fontPointsize( 10 );
    image.annotate( "I'm climbing the wall!", "+280+150",
                    NorthWestGravity, 90.0);
    //image.display();
    //
    // Write image.
    //

    cout << "Writing image \"shapes.miff\" ..." << endl;
    image.write( "shapes.miff" );

    // cout << "Display image..." << endl;
    // image.display( );

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
