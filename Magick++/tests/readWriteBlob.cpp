// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Test reading/writing BLOBs using Magick++
//

#include <string>
#include <iostream>
#include <list>
#include <vector>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int main( int argc, char **argv)
{
  int failures=0;

  if ( argc != 3 )
    {
      cout << "Usage: " << argv[0] << " file format" << endl;
      exit( 1 );
    }

  string infile = argv[1];
  string format = argv[2];

#include <magick/config.h>

  try {
    
    //
    // Test reading and writing BLOBs
    //

    Image master;
    Blob blob;
    // Read image from file
    master.read( infile );
    Geometry size( master.columns(), master.rows() );

#if !defined(HasHDF)
    if ( format == "HDF" )
      {
	cout << "HDF not supported in build" << endl;
	return 0;
      }
#endif

#if !defined(HasJBIG)
    if ( format == "JBIG" || format == "JBG" || format == "BIE" )
      {
	cout << "JBIG not supported in build" << endl;
	return 0;
      }
#endif

#if !defined(HasJPEG)
    if ( format == "JPEG" || format == "JPG" || format == "JPEG24")
      {
	cout << "JPEG not supported in build" << endl;
	return 0;
      }
#endif

#if !defined(HasPNG)
    if ( format == "PNG" )
      {
	cout << "PNG not supported in build" << endl;
	return 0;
      }
    if ( format == "MNG" )
      {
	cout << "MNG not supported in build" << endl;
	return 0;
      }
#endif

#if !defined(HasTIFF)
    if ( format == "TIFF"  || format == "FAX" || format == "G3" ||
	 format == "PTIFF" || format == "TIF" || format == "TIFF24" )
      {
	cout << "TIFF not supported in build" << endl;
	return 0;
      }
#endif

    try {

      bool needSize = false;
      if ( format == "CMYK" ||
	   format == "GRAY" ||
	   format == "MONO" ||
	   format == "RGB"  ||
	   format == "RGBA" ||
	   format == "UYVY" ||
	   format == "YUV"
	   )
	{
	  needSize = true;
	}

      cout << " write ..." << endl;
      Image original = master;

      // Write and read image initially to create repeatable image
      original.quantizeDither( false );
      original.magick( format );
      original.animationDelay( 10 );
      original.write( &blob );

      cout << " read ..." << endl;
      original.quantizeDither( false );
      original.magick( format );
      if ( needSize )
	original.size( size );
      original.read( blob );

      // Now test writing and reading image
      cout << " write ..." << endl;
      original.quantizeDither( false );
      original.magick( format );
      original.animationDelay( 10 );
      original.write( &blob );
	  
      Image image;
	  
      // Read image from BLOB
      cout << " read ..." << endl;
      image.quantizeDither( false );
      image.magick( format );
      if ( needSize )
	image.size( size );
      image.read( blob );


      if ( 
	  format != "JPEG" &&
	  format != "JPG" &&
	  format != "JPEG24" &&
	  format != "P7" &&
	  format != "PCD" &&
	  format != "PCDS" &&
	  format != "PIC" &&
	  format != "PICT" &&
	  format != "UYVY" &&
	  format != "VICAR" &&
	  format != "YUV"
	  )
	{
	  if ( image.signature(true) != original.signature() )
	    {
	      cout << "Line: " << __LINE__
		   << " Format \"" << image.magick()
		   << "\" BLOB signature " << image.signature() << " "
		   << "does not match original "
		   << original.signature()
		   << endl;
	      //Image errimage = image;
	      image.display();
	      image.write( string( "output." ) + format );
	      ++failures;
	    }
	}
      else
	{
	  // PhotoCD changes size to 768x512 by default
	  if ( format == "PCD" ||
	       format == "PCDS" )
	    {
	      if ( image.columns() != 768 || image.rows() != 512 )
		{
		  cout << "Line: " << __LINE__
		       << " Format \"" << format
		       << "\" Image size " << image.columns() << "x" << image.rows() << " "
		       << "does not match expected 768x512"
		       << endl;
		}
	    }
	  else
	    {
	      // Lossy formats
	      if ( image.columns()*image.rows() != master.columns()*master.rows() )
		{
		  cout << "Line: " << __LINE__
		       << " Format \"" << format
		       << "\" Image size " << image.columns() << "x" << image.rows() << " "
		       << "does not match original "
		       << master.columns() << "x" << master.rows()
		       << endl;
		  ++failures;
		}
	    }
	}
      image.write( string( "output." ) + format );
    }
    catch( Exception error_ )
      {
	cout << "Line: " << __LINE__
	     << " Format \"" << format << "\" "
	     << "Caught exception: " << error_.what() << endl;
	++failures;
      }
    catch( exception error_ )
      {
	cout << "Line: " << __LINE__
	     << " Format \"" << format << "\" "
	     << "Caught exception: " << error_.what() << endl;
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


