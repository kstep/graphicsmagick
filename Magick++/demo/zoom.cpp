// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001
//
// Resize image using specified filter algorithm with Magick++ API
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
      cout << "Usage: " << argv[0] << " filter_type geometry input_file output_file" << endl
           << "   filters - point box triangle hermite hanning hamming blackman gaussian" << endl
           << "             quadratic cubic catrom mitchell lanczos bessel sinc" << endl;
      exit( 1 );
    }

  std::string filter_type(argv[1]);
  std::string input_file(argv[3]);
  std::string output_file(argv[4]);

  Magick::FilterTypes filter(LanczosFilter);
  if (filter_type.compare("point") == 0)
    filter=PointFilter;
  else if (filter_type.compare("box") == 0)
    filter=BoxFilter;
  else if (filter_type.compare("triangle") == 0)
    filter=TriangleFilter;
  else if (filter_type.compare("hermite") == 0)
    filter=HermiteFilter;
  else if (filter_type.compare("hanning") == 0)
    filter=HanningFilter;
  else if (filter_type.compare("hamming") == 0)
    filter=HammingFilter;
  else if (filter_type.compare("blackman") == 0)
    filter=BlackmanFilter;
  else if (filter_type.compare("gaussian") == 0)
    filter=GaussianFilter;
  else if (filter_type.compare("quadratic") == 0)
    filter=QuadraticFilter;
  else if (filter_type.compare("cubic") == 0)
    filter=CubicFilter;
  else if (filter_type.compare("catrom") == 0)
    filter=CatromFilter;
  else if (filter_type.compare("mitchell") == 0)
    filter=MitchellFilter;
  else if (filter_type.compare("lanczos") == 0)
    filter=LanczosFilter;
  else if (filter_type.compare("bessel") == 0)
    filter=BesselFilter;
  else if (filter_type.compare("sinc") == 0)
    filter=SincFilter;

  try {
    Geometry geometry(argv[2]);
    Image image(input_file);
    image.filterType(filter);
    image.zoom(geometry);
    image.write(output_file);
  }
  catch( exception &error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }

  return 0; 
}
