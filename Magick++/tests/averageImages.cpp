// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Test STL averageImages function
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
  InitializeMagick(*argv);

  int failures=0;

  string srcdir("");
  if(getenv("srcdir") != 0)
    srcdir = getenv("srcdir") + string("/");

  try {

    //
    // Test averageImages
    //
    
    list<Image> imageList;
    readImages( &imageList, srcdir + "test_image_anim.miff" );
    
    Image averaged;
    averageImages( &averaged, imageList.begin(), imageList.end() );
    // averaged.display();
    if ( averaged.signature() != "06dbc5c5a249c909f9f6950fbb71d5ea18e1f1d2b8e62c52f3d46851f076cd63" &&
         averaged.signature() != "6c1525d33ae0390a1fcbdbdbcf869b2392d0aba2ef398431e2d51cd3e98fffb7")
      {
	cout << "Line: " << __LINE__
	     << "  Averaging image failed, signature = "
	     << averaged.signature() << endl;
	averaged.display();
	++failures;
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

