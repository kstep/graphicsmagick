/*
 *
 * Test BLOB operations via write/read/write/read sequence to detect
 * any data corruption problems. This does not verify that the image is
 * correct, only that encode/decode process is repeatable.
 *
 * Written by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
 *
 * The image returned by both reads must be identical in order for the
 * test to pass.
 *
 */

#include <stdio.h>
#if defined(_VISUALC_)
#include <stdlib.h>
#include <sys\types.h>
#endif
#include <time.h>
#include <magick/api.h>

static int CompareImage( int fuzz, Image *original, Image *final );

static int CompareImage( int fuzz, Image *original, Image *final )
{
  int
    x,
    y,
    factor,
    diff;

  PixelPacket
    *orig_p, *final_p;

  factor = ( MaxRGB * fuzz ) / 100;

  if ( ( original->rows      != final->rows      ) ||
       ( original->columns   != final->columns   ) ||
       ( original->depth     != final->depth     ) ||
       ( original->interlace != final->interlace ) ||
       ( original->matte     != final->matte     ) )
    return 1;

  if ( original->class != DirectClass )
    {
      SyncImage( original );
      original->class = DirectClass;
    }

  if ( final->class != DirectClass )
    {
      SyncImage( original );
      final->class = DirectClass;
    }

  for ( y = 0; y < (int) original->rows; y++ )
    {
      /* Get row from original */
      if (!GetPixelCache(original,0,y,original->columns,1))
	return 1;
      /* Get row from final */
      if (!GetPixelCache(final,0,y,final->columns,1))
	return 1;

      /* Compare pixels in row */
      orig_p  = original->pixels;
      final_p = final->pixels;
      for ( x = 0; x < (int) original->columns; x++ )
	{
	  if ( ( ( diff = labs((long)orig_p->red     - (long)final_p->red   ) )  > factor ) ||
	       ( ( diff = labs((long)orig_p->green   - (long)final_p->green ) )  > factor ) ||
	       ( ( diff = labs((long)orig_p->blue    - (long)final_p->blue  ) )  > factor ) )
	    return ((float)diff/MaxRGB*100);
	  if ( original->matte )
	    {
	      if ( ( diff = labs((long)orig_p->opacity - (long)final_p->opacity) ) > factor )
		return ((float)diff/MaxRGB*100);
	    }
	}

      orig_p++;
      final_p++;
    }

  return 0;
}

int main ( int argc, char **argv )
{
  Image *original = (Image *)NULL;
  Image *final = (Image *)NULL;
  char * blob = NULL;
  char infile[80];
  char format[80];
  char scratch[80];
  size_t blob_length = 0;
  char *size = NULL;
  int rows, columns = 0;
  int fuzz_factor = 0;
  int diff = 0;
  ImageInfo imageInfo;

  if ( argc != 3 )
    {
      printf ( "Usage: %s infile format\n", argv[0] );
      exit( 1 );
    }

  strcpy( infile, argv[1] );
  strcpy( format, argv[2] );

  printf("%s %s %s\n",
	  argv[0],infile, format );
  fflush(stdout);

  /*
   * Read original image
   */
  GetImageInfo( &imageInfo );
  imageInfo.dither = 0;
  strcpy( imageInfo.filename, infile );

  original = ReadImage ( &imageInfo, &error );
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read original image %s\n", imageInfo.filename );
      fflush(stdout);
      return 1;
    }

  /*
   * Obtain original image size if format requires it
   */
  rows    = original->rows;
  columns = original->columns;
  if (
      !strcmp( "CMYK", format ) ||
      !strcmp( "GRAY", format ) ||
      !strcmp( "MONO", format ) ||
      !strcmp( "RGB", format ) ||
      !strcmp( "RGBA", format ) ||
      !strcmp( "UYVY", format ) ||
      !strcmp( "YUV", format )
      )
    {
      size = AllocateMemory( 40 );
      sprintf( size, "%dx%d", columns, rows );
    }

  /*
   * Save image to BLOB
   */
  blob_length = 8192;
  strcpy( original->magick, format );
  strcpy( imageInfo.filename, "" );
  original->delay = 10;
  FormatString( scratch, "%d", 10 );
  CloneString( &imageInfo.delay, scratch );
  blob = ImageToBlob ( &imageInfo, original, &blob_length );
  if ( blob == NULL )
    {
      printf ( "Failed to write BLOB in format %s\n", imageInfo.magick );
      fflush(stdout);
      return 1;
    }
  DestroyImage( original );
  original = (Image*)NULL;

  /*
   * Read image back from BLOB
   */
  strcpy( imageInfo.magick, format );
  strcpy( imageInfo.filename, "" );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  original = BlobToImage( &imageInfo, blob, blob_length );
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read image from BLOB in format %s\n", imageInfo.magick );
      fflush(stdout);
      return 1;
    }
  FreeMemory( blob );
  blob = 0;

  /*
   * Save image to BLOB
   */
  blob_length = 8192;
  strcpy( original->magick, format );
  strcpy( imageInfo.filename, "" );
  original->delay = 10;
  FormatString( scratch, "%d", 10 );
  CloneString( &imageInfo.delay, scratch );
  blob = ImageToBlob ( &imageInfo, original, &blob_length );
  if ( blob == NULL )
    {
      printf ( "Failed to write BLOB in format %s\n", imageInfo.magick );
      fflush(stdout);
      return 1;
    }

  /*
   * Read image back from BLOB
   */
  strcpy( imageInfo.magick, format );
  strcpy( imageInfo.filename, "" );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  final = BlobToImage( &imageInfo, blob, blob_length );
  if ( final == (Image *)NULL )
    {
      printf ( "Failed to read image from BLOB in format %s\n", imageInfo.magick );
      fflush(stdout);
      return 1;
    }
  FreeMemory( blob );
  blob = 0;

  /*
   * Check final output
   */

  if ( !strcmp( "JPEG", format ) ||
       !strcmp( "JPG", format ) ||
       !strcmp( "JPEG24", format ) )
    fuzz_factor = 5;

  if ( !strcmp( "P7", format ) )
       fuzz_factor = 15;

  if ( !strcmp( "PCD", format ) )
    fuzz_factor = 8;

  if (!strcmp( "PCDS", format ) )
    fuzz_factor = 12;

  if ( !strcmp( "UYVY", format ) )
    fuzz_factor = 1;

/*   if ( !strcmp( "VIFF", format ) ) */
/*     fuzz_factor = 1; */

  if ( !strcmp( "YUV", format ) )
    fuzz_factor = 2;

  if ( CompareImage( fuzz_factor, original, final ) )
    {
      printf( "R/W file check for format \"%s\" failed: (%d%% component difference)\n", format, diff );
      fflush(stdout);
    }

  DestroyImage( original );
  original = (Image*)NULL;
  DestroyImage( final );
  final = (Image*)NULL;

  return 0;
}
