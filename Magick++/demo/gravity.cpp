// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2000, 2001
//
// Demo of text annotation with gravity.  Produces an animation showing
// the effect of rotated text along with various gravity specifications.
//
// After running demo program, run 'animate gravity_out.miff' if you
// are using X-Windows to see an animated result.
//
// Concept and algorithms lifted from PerlMagick demo script written
// by John Christy.
//

#include <string>
#include <iostream>
#include <list>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char ** argv)
{

  // Initialize ImageMagick install location for Windows
  InitializeMagick(*argv);

  string srcdir("");
  if(getenv("srcdir") != 0)
    srcdir = getenv("srcdir") + string("/");

  // Common font to use.
  string font = srcdir + "Generic.ttf";

  int x = 100;
  int y = 100;

  try {

    list<Image> animation;

    Image base( Geometry(600,600), Color("white") );
    base.strokeColor("#600");
    base.fillColor(Color());
    base.draw( DrawableLine( 300,100, 300,500 ) );
    base.draw( DrawableLine( 100,300, 500,300 ) );
    base.draw( DrawableRectangle( 100,100, 500,500 ) );
    base.density( Geometry(72,72) );
    base.strokeColor(Color());
    base.fillColor("#600");
    base.fontPointsize( 30 );
    base.font( font );
    base.boxColor( "red" );
    base.animationDelay( 20 );
    base.compressType( RunlengthEncodedCompression );

    for ( int angle = 0; angle < 360; angle += 30 )
      {
        cout << "angle " << angle << endl;
        Image pic = base;
        pic.annotate( "NorthWest", Geometry(0,0,x,y), NorthWestGravity, angle );
        pic.annotate( "North", Geometry(0,0,0,y), NorthGravity, angle );
        pic.annotate( "NorthEast", Geometry(0,0,x,y), NorthEastGravity, angle );
        pic.annotate( "East", Geometry(0,0,x,0), EastGravity, angle );
        pic.annotate( "Center", Geometry(0,0,0,0), CenterGravity, angle );
        pic.annotate( "SouthEast", Geometry(0,0,x,y), SouthEastGravity, angle );
        pic.annotate( "South", Geometry(0,0,0,y), SouthGravity, angle );
        pic.annotate( "SouthWest", Geometry(0,0,x,y), SouthWestGravity, angle );
        pic.annotate( "West", Geometry(0,0,x,0), WestGravity, angle );
        animation.push_back( pic );
      }
    cout << "Writing image \"gravity_out.miff\" ..." << endl;
    writeImages( animation.begin(), animation.end(), "gravity_out.miff" );
    // system( "animate gravity_out.miff" );

  }
  catch( exception &error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }
  
  return 0;
}
