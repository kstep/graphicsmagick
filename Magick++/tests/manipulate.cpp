// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Simple image manipulation test program for Magick++
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

  try {

    {
      //
      // Single-frame operations
      //

      // Create/read image
      cout << "Reading image ..." << endl;
      Image image1( "test_image.jpg" );
      image1.label("Test Image");
    
      // Zoom image with explicitly constructed geometry
      cout << endl<< "Zooming image to 591x397" << endl;
      Geometry geometry( 591, 397 );
      image1.zoom( geometry );

      // Zoom image using implicit type conversion from string
      cout << endl<< "Zooming image to 500x400" << endl;
      image1.zoom( "500x400>" );

      // Annotate image with text
      cout << endl << "Annotating with text" << endl;
      image1.font( "@Generic.ttf" );
      image1.penColor( "white" );
      image1.annotate( "100x200",
		       "The quick brown fox jumped over the hollow log." );

      // Draw a circle on image
      cout << endl << "Drawing circle ..." << endl;
      Drawable drawable;
      drawable.circle( 100,100, 150,150 );
      image1.penColor("red");
      image1.lineWidth(5) ;
      image1.boxColor("black"); 
      image1.draw( drawable );
    
      // Border
      cout << endl << "Adding 10x15 border ..." << endl;
      image1.border( "10x15" );
    
      cout << endl << "Creating second image from first" << endl;
      Image image2 = image1;
    
      // Quantize second image
      cout << endl << "Quantizing second image ..." << endl;
      image2.quantizeColors( 32 );
      image2.quantize( );
    
      // Write second image
      cout << endl << "Writing second image to testmagick2_out.gif ..." << endl;
      image2.write( "testmagick2_out.gif");

      cout << endl << "Assigning first image to second" << endl;
      image2 = image1;

      cout << endl << "Writing second (assigned) image to testmagick1_out.gif..." << endl;
      image2.write( "testmagick1_out.gif");
    }

    {
      //
      // Test appendImages
      //
      cout << endl << "Testing STL appendImages()" << endl;

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      Image appended;

      // Horizontal
      appendImages( &appended, imageList.begin(), imageList.end() );
      if ( appended.signature() != "0e600860c5cb47cd5e2377c015753d3c" )
	  cout << "  Horizontal append failed, signature = " << appended.signature() << endl;

      // Vertical
      appendImages( &appended, imageList.begin(), imageList.end(), true );
      if ( appended.signature() != "b65606228af103f08d21d437725ba90d" )
	  cout << "  Vertical append failed, signature = " << appended.signature() << endl;
    }

    {
      //
      // Test averageImages
      //
      cout << endl << "Testing STL averageImages()" << endl;

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      Image averaged;
      averageImages( &averaged, imageList.begin(), imageList.end() );

      if ( averaged.signature() != "678f291ed463419f4cfe26289b99552f" )
	cout << "  Averaging image failed, signature = " << averaged.signature() << endl;
    }

    {
      //
      // Test coalesceImages
      //
      cout << endl << "Testing STL coalesceImages()" << endl;

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      coalesceImages( imageList.begin(), imageList.end() );
    }

    {
      //
      // Test montageImages
      //
      cout << endl << "Testing STL montageImages()" << endl;

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      vector<Image> montage;
      MontageFramed montageOpts;

      // Default montage
      montageImages( &montage, imageList.begin(), imageList.end(), montageOpts );
      if ( montage[0].signature() != "220a8e16d1fb54c88740d7c8c124287b" )
	cout << "  Montage images failed, signature = " << montage[0].signature() << endl;

      // Montage with options set
      montage.clear();
      montageOpts.borderColor( "green" );
      montageOpts.borderWidth( 1 );
      montageOpts.compose( OverCompositeOp );
      montageOpts.fileName( "Montage" );
      montageOpts.frameGeometry( "6x6+3+3" );
      montageOpts.geometry("50x50+2+2>");
      montageOpts.gravity( CenterGravity );
      montageOpts.penColor( "yellow" );
      montageOpts.shadow( true );
      montageOpts.texture( "granite:" );
      montageOpts.tile("2x1");
      montageImages( &montage, imageList.begin(), imageList.end(), montageOpts );

      if ( montage.size() != 3 )
	cout << "  Montage images failed, number of montage frames is " << montage.size()
	     << " rather than 3 as expected." << endl;

      if ( montage[0].signature() != "763572b91f301691275d54463d464f04" )
	cout << "  Montage images failed, frame 0 signature = "
	     << montage[0].signature() << endl;

      if ( montage[1].signature() != "1f00b3ab7de5608dfe0b371254ef0bac" )
	cout << "  Montage images failed, frame 1 signature = "
	     << montage[1].signature() << endl;

      if ( montage[2].signature() != "14520bac0e876b5bb16cdefcc682e81b" )
	cout << "  Montage images failed, frame 2 signature = "
	     << montage[2].signature() << endl;
    }

    {
      //
      // Test morphImages
      //
      cout << endl << "Testing STL morphImages()" << endl;

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      list<Image> morphed;
      morphImages( &morphed, imageList.begin(), imageList.end(), 3 );

      if ( morphed.size() != 21 )
	cout << "  Morph images failed, number of frames is " << morphed.size()
	     << " rather than 21 as expected." << endl;

      vector<Image> montage;
      Montage montageOpts;
      montageOpts.geometry("50x50+2+2>");
      montageOpts.tile("10x2");
      montageImages( &montage, morphed.begin(), morphed.end(), montageOpts );

      if ( montage[0].signature() != "3cd77c8ecda3ea978a2b0a845f5b3eba" )
	cout << "  Morph images montage signature incorrect, signature = "
	     << montage[0].signature() << endl;
    }

    {
      //
      // Test readImages and writeImages
      //
      cout << endl << "Testing STL readImages() and writeImages()" << endl;
      
      list<Image> first;
      readImages( &first, "test_image_anim.gif" );
      
      if ( first.size() != 6 )
	cout << "  Read images failed, number of frames is " << first.size()
	     << " rather than 6 as expected." << endl;
      
      writeImages( first.begin(), first.end(), "testmagick_anim_out.gif" );
      
      list<Image> second;
      readImages( &second, "testmagick_anim_out.gif" );
      
      list<Image>::iterator firstIter = first.begin();
      list<Image>::iterator secondIter = second.begin();
      while( firstIter != first.end() && secondIter != second.end() )
	{
	  if ( *firstIter != *secondIter )
	    cout << "  Image scene: " << secondIter->scene()
		 << " is not equal to original" << endl;

	  if ( firstIter->scene() != secondIter->scene() )
	    cout << "  Image scene: " << secondIter->scene()
		 << " is not equal to original "
		 << firstIter->scene()
		 << endl;

	  if ( firstIter->rows() != secondIter->rows() )
	    cout << "  Image rows " << secondIter->rows()
		 << " are not equal to original "
		 << firstIter->rows()
		 << endl;

	  if ( firstIter->columns() != secondIter->columns() )
	    cout << "  Image columns " << secondIter->columns()
		 << " are not equal to original "
		 << firstIter->rows()
		 << endl;

	  if ( firstIter->packets() != secondIter->packets() )
	    cout << "  Image packets " << secondIter->packets()
		 << " are not equal to original "
		 << firstIter->packets()
		 << endl;

	  firstIter++;
	  secondIter++;
	}
    }
  }
  catch( ErrorFileOpen error_ )
    {
      cout << "Caught ExceptionFileOpen: " << error_ << endl;
      return 1;
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
  
  return 0;
}

#if 0
//
// Hack for linking with libraries compiled with GNU C when using Sun Workshop compiler.
// This gets things compiled but that doesn't mean the code will run ...
//
extern "C" {
  int __eprintf( const char *format, /* args */ ... );
}
int __eprintf( const char */*format*/, /* args */ ... )
{
  return 0;
}
#endif // out

#if 0
//
// This particular hack allows compiling with support for namespaces enabled
// using egcs 1.1.1.
// In order for it to work, the compilation line must include:
//
//  -fhonor-std -U__HONOR_STD
//
// This *only* works because code in the library looks like:
//
// #ifdef __HONOR_STD
// namespace std {
// #endif
//
// As it turns out, only the exception code has namespace support at this time.
// The end result is that only namespaces defined by the local library
// will be observed.
//
namespace std {
  void terminate()
  {
    exit(1);
  }
}
#endif
