// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
//
// Tests for throwing exceptions
//

#include <string>
#include <iostream>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char ** argv)
{

  // Initialize ImageMagick install location for Windows
  InitializeMagick(*argv);

  int failures=0;

  try
    {
      throw int(100);
    }
  catch ( int value_ )
    {
      cout << "Caught integer exception with value " << value_ << endl;
    }

  try {
    throw WarningResourceLimit("How now brown cow?");
  }
  catch( Exception &error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
    }

  try {

    unsigned int columns = 640;
    unsigned int rows = 480;
    Geometry geometry(columns,rows);
    Color canvasColor( "red" );
    Image image( geometry, canvasColor);

    {
      // Since this is not a montage image, simply verify error report
      cout << "Checking for working exceptions (may crash) ... ";
      bool caughtException = false;
      try
	{
	  image.directory();
	}
      catch ( Exception& error_ )
	{
	  caughtException = true;
	}
      if ( caughtException != true )
	{
	  ++failures;
          cout << "failed to catch exception!" << endl;
	}
      else
        {
          cout << "exceptions are working!" << endl;
        }

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
