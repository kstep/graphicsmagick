// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Test STL appendImages function
//

#include <string>
#include <iostream>
#include <list>
#include <vector>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char ** argv)
{

  // Initialize ImageMagick install location for Windows
  MagickIncarnate(*argv);

  int failures=0;

  string srcdir("");
  if(getenv("srcdir") != 0)
    srcdir = getenv("srcdir") + string("/");

  try {

    //
    // Test appendImages
    //

    list<Image> imageList;
    readImages( &imageList, srcdir + "test_image_anim.miff" );

    Image appended;

    // Horizontal
    appendImages( &appended, imageList.begin(), imageList.end() );
    // appended.display();
    if ( appended.signature() != "586f22dd8ff100c760972af77b13437b" )
      {
	++failures;
	cout << "Line: " << __LINE__
	     << "  Horizontal append failed, signature = "
	     << appended.signature() << endl;
	// appended.display();
      }

    // Vertical
    appendImages( &appended, imageList.begin(), imageList.end(), true );
    if ( appended.signature() != "1b32743b917dc97c5ed354b3b64fe3c8" )
      {
	++failures;
	cout << "Line: " << __LINE__
	     << "  Vertical append failed, signature = "
	     << appended.signature() << endl;
	// appended.display();
      }
    
  }

  catch( Exception &error_ )
    {
      cout << "Caught exception: " << error_ << endl;
      return 1;
    }
  catch( exception &error_ )
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

