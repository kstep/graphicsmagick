// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Demonstration of unary function-object based operations
//
// Reads the multi-frame file "smile_anim.miff" and writes a
// flipped and morphed version to "flip_out.miff".
//

#include <string>
#include <iostream>
#include <list>
#include <algorithm>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char ** argv)
{

  // Initialize ImageMagick install location for Windows
  MagickIncarnate(*argv);

  string srcdir("");
  if(getenv("srcdir") != 0)
    srcdir = getenv("srcdir") + string("/");

  try {

    // Read images into STL list
    list<Image> imageList;
    readImages( &imageList, srcdir + "smile_anim.miff" );

    // cout << "Total scenes: " << imageList.size() << endl;

    // Flip images
    for_each( imageList.begin(), imageList.end(), flipImage() );

    // Create a morphed version, adding three frames between each
    // existing frame.
    list<Image> morphed;
    morphImages( &morphed, imageList.begin(), imageList.end(), 3 );

    // Write out images
    cout << "Writing image \"flip_out.miff\" ..." << endl;
    writeImages( morphed.begin(), morphed.end(), "flip_out.miff" );

  }
  catch( exception error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }
  
  return 0;
}
