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
  int failures=0;

  try {

    {
      //
      // Test appendImages
      //

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      Image appended;

      // Horizontal
      appendImages( &appended, imageList.begin(), imageList.end() );
      // appended.display();
      if ( appended.signature() != "0e600860c5cb47cd5e2377c015753d3c" &&
	   appended.signature() != "eed68e3f06a225f24d8aa56d9a2848db" )
	{
	  ++failures;
	  cout << "Line: " << __LINE__
	       << "  Horizontal append failed, signature = "
	       << appended.signature() << endl;
	}

      // Vertical
      appendImages( &appended, imageList.begin(), imageList.end(), true );
      if ( appended.signature() != "b65606228af103f08d21d437725ba90d" &&
	   appended.signature() != "29b2e6300df902a177440a4e9c7dbbbc" )
	{
	  ++failures;
	  cout << "Line: " << __LINE__
	       << "  Vertical append failed, signature = "
	       << appended.signature() << endl;
	}
    }

    {
      //
      // Test averageImages
      //

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      Image averaged;
      averageImages( &averaged, imageList.begin(), imageList.end() );
      // averaged.display();
      if ( averaged.signature() != "678f291ed463419f4cfe26289b99552f" &&
	   averaged.signature() != "990a6b8f3abca6d06936b29c7a5a41af")
	{
	  ++failures;
	  cout << "Line: " << __LINE__
	       << "  Averaging image failed, signature = "
	       << averaged.signature() << endl;
	}
    }

    {
      //
      // Test coalesceImages
      //

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      coalesceImages( imageList.begin(), imageList.end() );
    }

    {
      //
      // Test montageImages
      //

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      vector<Image> montage;
      MontageFramed montageOpts;

      // Default montage
      montageImages( &montage, imageList.begin(), imageList.end(), montageOpts );

      if ( montage[0].montageGeometry() != Geometry( 114, 112 ) )
	{
	  ++failures;
	  cout << "Line: " << __LINE__ 
	       << "  Montage geometry ("
	       << string(montage[0].montageGeometry())
	       << ") is incorrect."
	       << endl;
	}

      if ( montage[0].columns() != 684 || montage[0].rows() != 114 )
	{
	  ++failures;
	  cout << "Line: " << __LINE__ 
	       << "  Montage columns/rows ("
	       << montage[0].columns() << "x"
	       << montage[0].rows()
	       << ") incorrect." << endl;
	}

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
	{
	  ++failures;
	  cout << "Line: " << __LINE__ 
	       << "  Montage images failed, number of montage frames is "
	       << montage.size()
	       << " rather than 3 as expected." << endl;
	}

      if ( montage[0].montageGeometry() != Geometry( 66, 70 ) )
	{
	  ++failures;
	  cout << "Line: " << __LINE__ 
	       << "  Montage geometry ("
	       << string(montage[0].montageGeometry())
	       << ") is incorrect."
	       << endl;
	}

      if ( montage[0].columns() != 132 || montage[0].rows() != 72 )
	{
	  ++failures;
	  cout << "Line: " << __LINE__ 
	       << "  Montage columns/rows ("
	       << montage[0].columns() << "x"
	       << montage[0].rows()
	       << ") incorrect." << endl;
	}
    }

    {
      //
      // Test morphImages
      //

      list<Image> imageList;
      readImages( &imageList, "test_image_anim.gif" );

      list<Image> morphed;
      morphImages( &morphed, imageList.begin(), imageList.end(), 3 );

      if ( morphed.size() != 21 )
	{
	  ++failures;
	  cout << "Line: " << __LINE__ 
	       << "  Morph images failed, number of frames is "
	       << morphed.size()
	       << " rather than 21 as expected." << endl;
	}
    }

    {
      //
      // Test readImages and writeImages
      //
      
      list<Image> first;
      readImages( &first, "test_image_anim.gif" );
      
      if ( first.size() != 6 )
	{
	  ++failures;
	  cout << "Line: " << __LINE__ 
	       << "  Read images failed, number of frames is "
	       << first.size()
	       << " rather than 6 as expected." << endl;
	}
      
      writeImages( first.begin(), first.end(), "testmagick_anim_out.gif" );
      
      list<Image> second;
      readImages( &second, "testmagick_anim_out.gif" );
      
      list<Image>::iterator firstIter = first.begin();
      list<Image>::iterator secondIter = second.begin();
      while( firstIter != first.end() && secondIter != second.end() )
	{
	  if ( *firstIter != *secondIter )
	    {
	      ++failures;
	      cout << "Line: " << __LINE__
		   << "  Image scene: " << secondIter->scene()
		   << " is not equal to original" << endl;
	    }

	  if ( firstIter->scene() != secondIter->scene() )
	    {
	      ++failures;
	      cout << "Line: " << __LINE__ 
		   << "  Image scene: " << secondIter->scene()
		   << " is not equal to original "
		   << firstIter->scene()
		   << endl;
	    }

	  if ( firstIter->rows() != secondIter->rows() )
	    {
	      ++failures;
	      cout << "Line: " << __LINE__ 
		   << "  Image rows " << secondIter->rows()
		   << " are not equal to original "
		   << firstIter->rows()
		   << endl;
	    }

	  if ( firstIter->columns() != secondIter->columns() )
	    {
	      ++failures;
	      cout << "Line: " << __LINE__ 
		   << "  Image columns " << secondIter->columns()
		   << " are not equal to original "
		   << firstIter->rows()
		   << endl;
	    }

	  if ( firstIter->packets() != secondIter->packets() )
	    {
	      ++failures;
	      cout << "Line: " << __LINE__ 
		   << "  Image packets " << secondIter->packets()
		   << " are not equal to original "
		   << firstIter->packets()
		   << endl;
	    }

	  firstIter++;
	  secondIter++;
	}
    }

    {
      //
      // Test reading and writing BLOBs
      //

#include <magick/config.h>

      Image master;
      Blob blob;
      // Read image from file
      master.read( "test_image.miff" );
      master.size( Geometry( master.columns(), master.rows()) );

      // Formats to test
      string formats[] =
      {
	"AVS",
	"BIE",
	"BMP",
	"CMYK",
	// 	"DCM",
	"DCX",
	"DIB",
	"FITS",
	"GIF",
	"GIF87",
	// 	"GRANITE", // cores
	"GRAY",
	// #ifdef HasHDF
	// 	"HDF",
	// #endif // HasHDF
	"ICB",
#ifdef HasJBIG
	"JBG",
	"JBIG",
#endif // HasJBIG
#ifdef HasJPEG
	"JPG",
	"JPEG",
	"JPEG24",
#endif // HasJPEG
	// 	"LOGO", // cores
	"MIFF",
	"MNG",
	"MONO",
	// 	"MTV",  // cores
	"P7",
	"PBM",
	// 	"PCD",
	// 	"PCDS",
	"PCT",
	"PCX",
	"PIC",
	"PICT",
	"PGM",
	"PM",
#ifdef HasPNG
	"PNG",
#endif // HasPNG
	"PNM",
	"PPM",
	"PSD",
	"RAS",
	"RGB",
	"RGBA",
	"SGI",
	"SUN",
#ifdef HasTIFF
// 	"FAX",
// 	"G3",
// 	"PTIFF",
// 	"TIFF",
// 	"TIF",
// 	"TIFF24",
#endif // HasTIFF
	"TGA",
	"UYVY",
	"VDA",
	"VICAR",
	"VIFF",
	"VST",
	"XBM",
	"XPM",
	"XV",
	"XWD",
	// 	"YUV",  // consumes memory forever
	""
      };

      for ( unsigned int i = 0 ; formats[i].length() > 0 ; ++i )
	{
	  try {
	    string format = formats[ i ];
	    // 	  cout << "Format \"" << format << "\"" << endl;
	    // Write image to BLOB in specified format
	    Image original = master;

	    // 	  cout << " write ..." << endl;
	    original.magick( formats[ i ] );
	    original.animationDelay( 10 );
	    original.size( master.size() );
	    original.write( &blob );
	  
	    Image image;
	  
	    // Read image from BLOB
	    // 	  cout << " read ..." << endl;
	    image.magick( formats[ i ] );
	    image.size( master.size() );
	    image.read( blob );


// 	    if ( format != "AVS" &&
// 		 format != "BIE" &&
// 		 format != "CMYK" &&
// 		 format != "FITS" &&
// 		 format != "GIF" &&
// 		 format != "GIF87" &&
// 		 format != "GRAY" &&
// 		 format != "JBG" &&
// 		 format != "JBIG" &&
// 		 format != "JPEG" &&
// 		 format != "JPG" &&
// 		 format != "JPEG24" &&
// 		 format != "MONO" &&
// 		 format != "P7" &&
// 		 format != "PBM" &&
// 		 format != "PCT" &&
// 		 format != "PGM" &&
// 		 format != "PIC" &&
// 		 format != "PICT" &&
// 		 format != "PM" &&
// 		 format != "RGBA" &&
// 		 format != "UYVY" &&
// 		 format != "VICAR" &&
// 		 format != "XBM" &&
// 		 format != "XPM"

// 		 )
// 	      {
// 		if ( image.signature(true) != original.signature() )
// 		  {
// 		    cout << "Line: " << __LINE__
// 			 << " Format \"" << image.magick()
// 			 << "\" BLOB signature " << image.signature() << " "
// 			 << "does not match original "
// 			 << original.signature()
// 			 << endl;
// 		    ++failures;
// 		  }
// 	      }
// 	    else
// 	      {
		// Lossy formats
		if ( image.columns()*image.rows() != master.columns()*master.rows() )
		  {
		    cout << "Line: " << __LINE__
			 << " Format \"" << formats[ i ]
			 << "\" Image size " << image.columns() << "x" << image.rows() << " "
			 << "does not match original "
			 << master.columns() << "x" << master.rows()
			 << endl;
		    ++failures;
		  }
// 	      }
	  }
	
	  catch( exception error_ )
	    {
	      cout << "Line: " << __LINE__
		   << " Format \"" << formats[ i ] << "\" "
		   << "Caught exception: " << error_.what() << endl;
	      ++failures;
	      continue;
	    }
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

  if ( failures )
    {
      cout << failures << " failures" << endl;
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
