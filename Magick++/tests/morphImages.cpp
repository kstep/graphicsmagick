// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Test STL morphImages function
//

#include <string>
#include <iostream>
#include <list>
#include <vector>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char **argv)
{

  // Initialize ImageMagick install location for Windows
  MagickIncarnate(*argv);

  int failures=0;

  string srcdir("");
  if(getenv("srcdir") != 0)
    srcdir = getenv("srcdir") + string("/");

  try {

    //
    // Test morphImages
    //

    list<Image> imageList;
    readImages( &imageList, srcdir + "test_image_anim.miff" );

    list<Image> morphed;
    morphImages( &morphed, imageList.begin(), imageList.end(), 3 );

    if ( morphed.size() != 21 )
      {
	++failures;
	cout << "Line: " << __LINE__ 
	     << "  Morph images failed, number of frames is "
	     << morphed.size()
	     << " rather than 21 as expected." << endl;
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

