// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001, 2002
//
// Resize image using specified resize algorithm with Magick++ API
//
// Usage: zoom filter_type geometry input_file output_file
//

#include <Magick++.h>
#include <iostream>
#include <string>
using namespace std; 
using namespace Magick;
int main(int argc,char **argv) 
{
  // Initialize ImageMagick install location for Windows
  InitializeMagick(*argv);

  if ( argc < 5 )
    {
      cout << "Usage: " << argv[0] << " algorithm geometry input_file output_file" << endl
           << "   algorithm - bessel blackman box catrom cubic gaussian hamming hanning hermite" << endl
           << "               lanczos mitchell point quadratic sample scale sinc triangle" << endl;
      exit( 1 );
    }

  std::string algorithm(argv[1]);
  std::string input_file(argv[3]);
  std::string output_file(argv[4]);

  Magick::FilterTypes filter(LanczosFilter);
  if (algorithm.compare("point") == 0)
    filter=PointFilter;
  else if (algorithm.compare("box") == 0)
    filter=BoxFilter;
  else if (algorithm.compare("triangle") == 0)
    filter=TriangleFilter;
  else if (algorithm.compare("hermite") == 0)
    filter=HermiteFilter;
  else if (algorithm.compare("hanning") == 0)
    filter=HanningFilter;
  else if (algorithm.compare("hamming") == 0)
    filter=HammingFilter;
  else if (algorithm.compare("blackman") == 0)
    filter=BlackmanFilter;
  else if (algorithm.compare("gaussian") == 0)
    filter=GaussianFilter;
  else if (algorithm.compare("quadratic") == 0)
    filter=QuadraticFilter;
  else if (algorithm.compare("cubic") == 0)
    filter=CubicFilter;
  else if (algorithm.compare("catrom") == 0)
    filter=CatromFilter;
  else if (algorithm.compare("mitchell") == 0)
    filter=MitchellFilter;
  else if (algorithm.compare("lanczos") == 0)
    filter=LanczosFilter;
  else if (algorithm.compare("bessel") == 0)
    filter=BesselFilter;
  else if (algorithm.compare("sinc") == 0)
    filter=SincFilter;

  try {
    Geometry geometry(argv[2]);
    Image image(input_file);
    if (algorithm.compare("sample") == 0)
      {
        image.sample(geometry);
      }
    else if (algorithm.compare("scale") == 0)
      {
        image.scale(geometry);
      }
    else
      {
        image.filterType(filter);
        image.zoom(geometry);
      }
    image.write(output_file);
  }
  catch( exception &error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }

  return 0; 
}
