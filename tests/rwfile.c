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
#include <magick_config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#if defined(_VISUALC_)
#include <stdlib.h>
#include <sys\types.h>
#endif
#include <time.h>
#include <magick/api.h>

int main ( int argc, char **argv )
{
  Image
    *final = (Image *) NULL,
    *original = (Image *) NULL;

  char
    filename[80],
    format[MaxTextExtent],
    infile[MaxTextExtent],
    *size = NULL;

  int
    arg = 1,
    exit_status = 0,
    rows = 0,
    columns = 0,
    pause = 0;

  double
    fuzz_factor = 0;

  ImageInfo
    imageInfo;

  ExceptionInfo
    exception;

  if (LocaleNCompare("rwfile",argv[0],7) == 0)
    InitializeMagick((char *) NULL);
  else
    InitializeMagick(*argv);

  GetImageInfo( &imageInfo );
  GetExceptionInfo( &exception );

  for (arg=1; arg < argc; arg++)
    {
      char
        *option = argv[arg];
    
      if (*option == '-')
        {
          if (LocaleCompare("compress",option+1) == 0)
            {
              arg++;
              option=argv[arg];
              imageInfo.compression=UndefinedCompression;
              if (LocaleCompare("None",option) == 0)
                imageInfo.compression=NoCompression;
              if (LocaleCompare("BZip",option) == 0)
                imageInfo.compression=BZipCompression;
              if (LocaleCompare("Fax",option) == 0)
                imageInfo.compression=FaxCompression;
              if (LocaleCompare("Group4",option) == 0)
                imageInfo.compression=Group4Compression;
              if (LocaleCompare("JPEG",option) == 0)
                imageInfo.compression=JPEGCompression;
              if (LocaleCompare("Lossless",option) == 0)
                imageInfo.compression=LosslessJPEGCompression;
              if (LocaleCompare("LZW",option) == 0)
                imageInfo.compression=LZWCompression;
              if (LocaleCompare("RLE",option) == 0)
                imageInfo.compression=RunlengthEncodedCompression;
              if (LocaleCompare("Zip",option) == 0)
                imageInfo.compression=ZipCompression;
            }
          else if (LocaleCompare("debug",option+1) == 0)
              (void) SetLogEventMask(argv[++arg]);
          else if (LocaleCompare("depth",option+1) == 0)
            {
              imageInfo.depth=QuantumDepth;
              arg++;
              if ((arg == argc) || !sscanf(argv[arg],"%ld",&imageInfo.depth))
                {
                  printf("-depth argument missing or not integer\n");
                  fflush(stdout);
                  exit_status = 1;
                  goto program_exit;
                }
              if(imageInfo.depth != 8 && imageInfo.depth != 16 && imageInfo.depth != 32)
                {
                  printf("-depth (%ld) not 8, 16, or 32\n", imageInfo.depth);
                  fflush(stdout);
                  exit_status = 1;
                  goto program_exit;
                }
            }
          else if (LocaleCompare("log",option+1) == 0)
              (void) SetLogFormat(argv[++arg]);
          else if (LocaleCompare("pause",option+1) == 0)
              pause=1;
          else if (LocaleCompare("size",option+1) == 0)
            {
              arg++;
              if ((arg == argc) || !IsGeometry(argv[arg]))
                {
                  printf("-size argument missing or not geometry\n");
                  fflush(stdout);
                  exit_status = 1;
                  goto program_exit;
                }
              (void) CloneString(&imageInfo.size,argv[arg]);
            }
        }
      else
        break;
    }
  if (arg != argc-2)
    {
      printf("arg=%d, argc=%d\n", arg, argc);
      printf ( "Usage: %s [-compress algorithm -debug events -depth integer -log format -size geometry] infile format\n", argv[0] );
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }
  
  strncpy(infile, argv[arg], MaxTextExtent-1 );
  arg++;
  strncpy( format, argv[arg], MaxTextExtent-1 );

  for (arg=0; arg < argc; arg++)
    printf("%s ", argv[arg]);
  printf("\n");
  fflush(stdout);

  /*
   * Read original image
   */

  imageInfo.dither = 0;
  strncpy( imageInfo.filename, infile, MaxTextExtent-1 );
  fflush(stdout);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "Reading image %s", imageInfo.filename);
  original = ReadImage ( &imageInfo, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read original image %s\n", imageInfo.filename );
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  /*
   * Obtain original image size if format requires it
   */
  rows    = original->rows;
  columns = original->columns;
  size = (char *) AcquireMemory( 40 );
  sprintf( size, "%dx%d", columns, rows );

  /*
   * Save image to file
   */
  sprintf( filename, "out_1.%s", format );
  strncpy( original->magick, format, MaxTextExtent-1 );
  strncpy( original->filename, filename, MaxTextExtent-1 );
  original->delay = 10;
  fflush(stdout);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "Writing image %s", original->filename);
  WriteImage ( &imageInfo, original );
  imageInfo.depth=original->depth;
  DestroyImage( original );
  original = (Image*)NULL;

  /*
   * Read image back from file
   */
  strncpy( imageInfo.magick, format, MaxTextExtent-1 );
  strncpy( imageInfo.filename, filename, MaxTextExtent-1 );
  if (size != NULL)
    CloneString( &imageInfo.size, size );
  fflush(stdout);
  original = ReadImage ( &imageInfo, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read image from file in format %s\n",
               imageInfo.magick );
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  /*
   * Save image to file
   */
  sprintf( filename, "out_2.%s", format );
  strncpy( original->magick, format, MaxTextExtent-1 );
  strncpy( original->filename, filename, MaxTextExtent-1 );
  original->delay = 10;
  fflush(stdout);
  WriteImage ( &imageInfo, original );

  /*
   * Read image back from file
   */
  strncpy( imageInfo.magick, format, MaxTextExtent-1 );
  strncpy( imageInfo.filename, filename, MaxTextExtent-1 );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  fflush(stdout);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "Reading image %s", imageInfo.filename);
  final = ReadImage ( &imageInfo, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( final == (Image *)NULL )
    {
      printf ( "Failed to read image from file in format %s\n",
               imageInfo.magick );
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  /*
   * Check final output
   */

  if ( !strcmp( "DPX", format ) ||
       !strcmp( "JPEG", format ) ||
       !strcmp( "JNG", format ) ||
       !strcmp( "JPG", format ) ||
       !strcmp( "JPG24", format ) ||
       !strcmp( "JP2", format ) ||
       !strcmp( "PAL", format ) ||
       !strcmp( "GRAY", format ) ||
       !strcmp( "CMYK", format ) ||
       !strcmp( "PCD", format ) ||
       !strcmp( "PCDS", format ) ||
       !strcmp( "UYVY", format ) ||
       !strcmp( "YUV", format ) )
    fuzz_factor = 0.008;

  if ( !IsImagesEqual(original, final ) &&
       (original->error.normalized_mean_error > fuzz_factor) )
    {
      printf( "R/W file check for format \"%s\" failed: %u/%.6f/%.6fe\n",
              format,(unsigned int) original->error.mean_error_per_pixel,
              original->error.normalized_mean_error,
              original->error.normalized_maximum_error);
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

 program_exit:
  DestroyImage( original );
  original = (Image*)NULL;
  DestroyImage( final );
  final = (Image*)NULL;

  DestroyMagick();

  if (pause)
    getc(stdin);
  return exit_status;
}
