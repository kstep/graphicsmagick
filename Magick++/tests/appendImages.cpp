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
  InitializeMagick(*argv);

  int failures=0;

  string srcdir("");
  if(getenv("SRCDIR") != 0)
    srcdir = getenv("SRCDIR");

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
    if (( appended.signature() != "3a90bb0bb8f69f6788ab99e9e25598a0d6c5cdbbb797f77ad68011e0a8b1689d" ) &&
     ( appended.signature() != "0c2eaf1bed46526e089cfb7359a46ae0606acafd06e059657d4014e02d5b12b8" ))
      {
	++failures;
	cout << "Line: " << __LINE__
	     << "  Horizontal append failed, signature = "
	     << appended.signature() << endl;
	appended.write("appendImages_horizontal_out.miff");
	// appended.display();
      }

    // Vertical
    appendImages( &appended, imageList.begin(), imageList.end(), true );
    if (( appended.signature() != "d73d25ccd6011936d08b6d0d89183b7a61790544c2195269aff4db2f782ffc08" ) &&
       ( appended.signature() != "c27dc6a04aed2170524abc7211f2b8c5321e5440b068a162f4a14e143a9f454a" ))
      {
	++failures;
	cout << "Line: " << __LINE__
	     << "  Vertical append failed, signature = "
	     << appended.signature() << endl;
	appended.write("appendImages_vertical_out.miff");
	// appended.display();
      }
    
  }

  catch( Exception &error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
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

