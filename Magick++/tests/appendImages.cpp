// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
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

int main( int /*argc*/, char ** /*argv*/)
{
  int failures=0;

  try {

    //
    // Test appendImages
    //

    list<Image> imageList;
    readImages( &imageList, "test_image_anim.miff" );

    Image appended;

    // Horizontal
    appendImages( &appended, imageList.begin(), imageList.end() );
    // appended.display();
    if ( appended.signature() != "0e600860c5cb47cd5e2377c015753d3c" &&
	 appended.signature() != "7c7c64a91c652c0eadbee38ac315539b" )
      {
	++failures;
	cout << "Line: " << __LINE__
	     << "  Horizontal append failed, signature = "
	     << appended.signature() << endl;
	// appended.display();
      }

    // Vertical
    appendImages( &appended, imageList.begin(), imageList.end(), true );
    if ( appended.signature() != "b65606228af103f08d21d437725ba90d" &&
	 appended.signature() != "39c8d58e479462844d8e38d9a0016bb6" )
      {
	++failures;
	cout << "Line: " << __LINE__
	     << "  Vertical append failed, signature = "
	     << appended.signature() << endl;
	// appended.display();
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

