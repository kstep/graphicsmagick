// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001
//
// Test Magick::CoderInfo class
//

#include <string>
#include <iostream>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char **argv)
{

  // Initialize ImageMagick install location for Windows
  InitializeMagick(*argv);

  int failures=0;

  try {

    cout << "Here is a list of image formats recognized by ImageMagick.  Mode 'rw+'"
         << "means ImageMagick can read, write, and save more than one image of a"
         << "sequence to the same blob or file."
         << endl
         << endl
         << "    Format  Mode  Description"
         << endl
         << "-------------------------------------------------------------------------------"
         << endl;

    list<CoderInfo> coderList;
    coderInfoList( &coderList, AnyMatch, AnyMatch, AnyMatch );

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
