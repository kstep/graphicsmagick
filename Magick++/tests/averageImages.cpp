// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
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

int main( int /*argc*/, char ** /*argv*/)
{
  int failures=0;

  try {

    //
    // Test averageImages
    //
    
    list<Image> imageList;
    readImages( &imageList, "test_image_anim.miff" );
    
    Image averaged;
    averageImages( &averaged, imageList.begin(), imageList.end() );
    // averaged.display();
    if ( averaged.signature() != "678f291ed463419f4cfe26289b99552f" &&
	 averaged.signature() != "990a6b8f3abca6d06936b29c7a5a41af")
      {
	cout << "Line: " << __LINE__
	     << "  Averaging image failed, signature = "
	     << averaged.signature() << endl;
	averaged.display();
	++failures;
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

