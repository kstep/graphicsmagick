/*
 *
 * Test file encode/decode operations via write/read/write/read
 * sequence to detect any data corruption problems.  This does not
 * verify that the image is correct, only that encode/decode process
 * is repeatable.
 *
 * Written by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
 *
 * The image returned by both reads must be identical (or deemed close
 * enough) in order for the test to pass.
 * */


#include <stdio.h>
#include <time.h>
#include <magick/api.h>

static int CompareImage( int fuzz, Image *original, Image *final );

static int CompareImage( int fuzz, Image *original, Image *final )
{
  int
    x,
    y,
    factor;

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

      orig_p  = original->pixels;
      final_p = final->pixels;

      if ( original->matte )
	{
	  /* RGBA */
	  for ( x = 0; x < (int) original->columns; x++ )
	    {
	      if ( ( labs((long)orig_p->red     - (long)final_p->red)     > factor ) ||
		   ( labs((long)orig_p->green   - (long)final_p->green)   > factor ) ||
		   ( labs((long)orig_p->blue    - (long)final_p->blue)    > factor ) ||
		   ( labs((long)orig_p->opacity - (long)final_p->opacity) > factor ) )
		return 1;
	    }
	}
      else
	{
	  /* Plain RGB */
	  for ( x = 0; x < (int) original->columns; x++ )
	    {
	      if ( ( labs((long)orig_p->red     - (long)final_p->red)     > factor ) ||
		   ( labs((long)orig_p->green   - (long)final_p->green)   > factor ) ||
		   ( labs((long)orig_p->blue    - (long)final_p->blue)    > factor ) )
		return 1;
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
  char infile[80];
  char format[80];
  char *size = NULL;
  int rows, columns = 0;
  char filename[80];
  int fuzz_factor = 0;
  ImageInfo imageInfo;

  if ( argc != 3 )
    {
      printf ( "Usage: %s infile format\n", argv[0] );
      exit( 1 );
    }

  strcpy( infile, argv[1] );
  strcpy( format, argv[2] );

  printf( "Testing R/W file: original image \"%s\"; test format \"%s\" ...\n",
	  infile, format );
  fflush(stdout);

  sprintf( filename, "out.%s", format );

  /*
   * Read original image
   */
  GetImageInfo( &imageInfo );
  imageInfo.dither = 0;
  strcpy( imageInfo.filename, infile );

  original = ReadImage ( &imageInfo );
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
   * Save image to file
   */
  strcpy( original->magick, format );
  strcpy( original->filename, filename );
  original->delay = 10;
  WriteImage ( &imageInfo, original );
  DestroyImage( original );
  original = (Image*)NULL;

  /*
   * Read image back from file
   */
  strcpy( imageInfo.magick, format );
  strcpy( imageInfo.filename, filename );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  original = ReadImage ( &imageInfo );
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read image from file in format %s\n",
	       imageInfo.magick );
      fflush(stdout);
      return 1;
    }

  /*
   * Save image to file
   */
  strcpy( original->magick, format );
  strcpy( original->filename, filename );
  original->delay = 10;
  WriteImage ( &imageInfo, original );

  /*
   * Read image back from file
   */
  strcpy( imageInfo.magick, format );
  strcpy( imageInfo.filename, filename );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  final = ReadImage ( &imageInfo );
  if ( final == (Image *)NULL )
    {
      printf ( "Failed to read image from file in format %s\n",
	       imageInfo.magick );
      fflush(stdout);
      return 1;
    }

  /*
   * Check final output
   */

  if ( !strcmp( "JPEG", format ) ||
       !strcmp( "JPG", format ) ||
       !strcmp( "JPEG24", format ) )
    fuzz_factor = 5;

  if ( !strcmp( "PCD", format ) ||
       !strcmp( "PCDS", format ) )
    fuzz_factor = 7;

  if ( !strcmp( "UYVY", format ) )
    fuzz_factor = 1;

  if ( !strcmp( "YUV", format ) )
    fuzz_factor = 1;

  if ( CompareImage( fuzz_factor, original, final ) )
    {
      printf( "R/W file check for format \"%s\" failed.\n", format );
      fflush(stdout);
    }

  DestroyImage( original );
  original = (Image*)NULL;
  DestroyImage( final );
  final = (Image*)NULL;

  return 0;
}
