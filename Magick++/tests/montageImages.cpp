// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Test STL montageImages function
//

#include <string>
#include <iostream>
#include <list>
#include <vector>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char ** /*argv*/)
{
  int failures=0;

  try {

    //
    // Test montageImages
    //

    list<Image> imageList;
    readImages( &imageList, "test_image_anim.gif" );

    vector<Image> montage;
    MontageFramed montageOpts;

    // Default montage
    montageImages( &montage, imageList.begin(), imageList.end(), montageOpts );

    if ( montage[0].montageGeometry() != Geometry( 114, 112 ) )
      {
	++failures;
	cout << "Line: " << __LINE__ 
	     << "  Montage geometry ("
	     << string(montage[0].montageGeometry())
	     << ") is incorrect."
	     << endl;
      }

    if ( montage[0].columns() != 684 || montage[0].rows() != 114 )
      {
	++failures;
	cout << "Line: " << __LINE__ 
	     << "  Montage columns/rows ("
	     << montage[0].columns() << "x"
	     << montage[0].rows()
	     << ") incorrect." << endl;
      }

    // Montage with options set
    montage.clear();
    montageOpts.borderColor( "green" );
    montageOpts.borderWidth( 1 );
    montageOpts.compose( OverCompositeOp );
    montageOpts.fileName( "Montage" );
    montageOpts.frameGeometry( "6x6+3+3" );
    montageOpts.geometry("50x50+2+2>");
    montageOpts.gravity( CenterGravity );
    montageOpts.penColor( "yellow" );
    montageOpts.shadow( true );
    montageOpts.texture( "granite:" );
    montageOpts.tile("2x1");
    montageImages( &montage, imageList.begin(), imageList.end(), montageOpts );

    if ( montage.size() != 3 )
      {
	++failures;
	cout << "Line: " << __LINE__ 
	     << "  Montage images failed, number of montage frames is "
	     << montage.size()
	     << " rather than 3 as expected." << endl;
      }

    if ( montage[0].montageGeometry() != Geometry( 66, 70 ) )
      {
	++failures;
	cout << "Line: " << __LINE__ 
	     << "  Montage geometry ("
	     << string(montage[0].montageGeometry())
	     << ") is incorrect."
	     << endl;
      }

    if ( montage[0].columns() != 132 || montage[0].rows() != 72 )
      {
	++failures;
	cout << "Line: " << __LINE__ 
	     << "  Montage columns/rows ("
	     << montage[0].columns() << "x"
	     << montage[0].rows()
	     << ") incorrect." << endl;
      }
  }

  catch( Exception error_ )
    {
      cout << "Caught exception: " << error_ << endl;
      return 1;
    }
  catch( exception error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }

  if ( failures )
    {
      cout << failures << " failures" << endl;
      return 1;
    }
  
  return 0;
}

