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

#if !defined(_VISUALC_)
#include <config.h>
#endif
#include <sys/types.h>
#include <stdio.h>
#if defined(_VISUALC_)
#include <stdlib.h>
#include <sys\types.h>
#endif
#include <time.h>
#include <magick/api.h>

int main ( int argc, char **argv )
{
  Image *original = (Image *)NULL;
  Image *final = (Image *)NULL;
  char infile[80];
  char format[80];
  char *size = NULL;
  int rows, columns = 0;
  char filename[80];
  double fuzz_factor = 0;
  ImageInfo imageInfo;
  ExceptionInfo exception;

  if ( argc != 3 )
    {
      printf ( "Usage: %s infile format\n", argv[0] );
      exit( 1 );
    }

  if (LocaleNCompare("rwfile",argv[0],7) == 0)
    InitializeMagick(GetExecutionPath(argv[0]));
  else
    InitializeMagick(*argv);

  strcpy( infile, argv[1] );
  strcpy( format, argv[2] );

  printf("%s %s %s\n",
	  argv[0], infile, format );
  fflush(stdout);

  /*
   * Read original image
   */
  GetImageInfo( &imageInfo );
  imageInfo.dither = 0;
  strcpy( imageInfo.filename, infile );
  printf("  read \"%s\"\n", imageInfo.filename);
  fflush(stdout);
  original = ReadImage ( &imageInfo, &exception );
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
  size = AcquireMemory( 40 );
  sprintf( size, "%dx%d", columns, rows );

  /*
   * Save image to file
   */
  sprintf( filename, "out_1.%s", format );
  strcpy( original->magick, format );
  strcpy( original->filename, filename );
  original->delay = 10;
  printf("  write \"%s\"\n", original->filename);
  fflush(stdout);
  WriteImage ( &imageInfo, original );
  imageInfo.depth=GetImageDepth(original);
  DestroyImage( original );
  original = (Image*)NULL;

  /*
   * Read image back from file
   */
  strcpy( imageInfo.magick, format );
  strcpy( imageInfo.filename, filename );
  if (size != NULL)
    CloneString( &imageInfo.size, size );
  printf("  read \"%s\"\n", imageInfo.filename);
  fflush(stdout);
  original = ReadImage ( &imageInfo, &exception );
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
  sprintf( filename, "out_2.%s", format );
  strcpy( original->magick, format );
  strcpy( original->filename, filename );
  original->delay = 10;
  printf("  write \"%s\"\n", original->filename);
  fflush(stdout);
  WriteImage ( &imageInfo, original );

  /*
   * Read image back from file
   */
  strcpy( imageInfo.magick, format );
  strcpy( imageInfo.filename, filename );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  printf("  read \"%s\"\n", imageInfo.filename);
  fflush(stdout);
  final = ReadImage ( &imageInfo, &exception );
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
       !strcmp( "JPG24", format ) ||
       !strcmp( "JP2", format ) ||
       !strcmp( "PAL", format ) ||
       !strcmp( "PCD", format ) ||
       !strcmp( "PCDS", format ) ||
       !strcmp( "UYVY", format ) ||
       !strcmp( "YUV", format ) )
    fuzz_factor = 0.008;

  if ( !IsImagesEqual(original, final ) &&
       (original->normalized_mean_error > fuzz_factor) )
    {
      printf( "R/W file check for format \"%s\" failed: %u/%.6f/%.6fe\n",
        format,(unsigned int) original->mean_error_per_pixel,original->normalized_mean_error,
        original->normalized_maximum_error);
      fflush(stdout);
    }

  DestroyImage( original );
  original = (Image*)NULL;
  DestroyImage( final );
  final = (Image*)NULL;

  return 0;
}
