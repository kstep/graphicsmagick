/*
 *
 * Test DispatchImage/ConstituteImage operations via
 * write/read/write/read sequence to detect any data corruption
 * problems.
 *
 * Written by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
 *
 * The image returned by both reads must be identical in order for the
 * test to pass.
 * */

#if !defined(_VISUALC_)
#include <magick_config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
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

  void
    *pixels=0;

  char
    infile[MaxTextExtent],
    map[MaxTextExtent];

  int
    arg = 1,
    exit_status = 0,
    rows = 0,
    columns = 0,
    pause = 0;

  unsigned int
    quantum_size=sizeof(unsigned char);

  double
    fuzz_factor = 0;

  ImageInfo
    imageInfo;

  ExceptionInfo
    exception;

  StorageType
    storage_type=CharPixel;

  if (LocaleNCompare("constitute",argv[0],7) == 0)
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
          if (LocaleCompare("debug",option+1) == 0)
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
          else if (LocaleCompare("storagetype",option+1) == 0)
            {
              arg++;
              if ((arg == argc))
                {
                  printf("-storagetype argument missing\n");
                  fflush(stdout);
                  exit_status = 1;
                  goto program_exit;
                }
              if (LocaleCompare("Char",argv[arg]) == 0)
                {
                  storage_type=CharPixel;
                  quantum_size=sizeof(unsigned char);
                }
              else if (LocaleCompare("Short",argv[arg]) == 0)
                {
                  storage_type=ShortPixel;
                  quantum_size=sizeof(unsigned short);
                }
              else if (LocaleCompare("Integer",argv[arg]) == 0)
                {
                  storage_type=IntegerPixel;
                  quantum_size=sizeof(unsigned int);
                }
              else if (LocaleCompare("Long",argv[arg]) == 0)
                {
                  storage_type=LongPixel;
                  quantum_size=sizeof(unsigned long);
                }
              else if (LocaleCompare("Float",argv[arg]) == 0)
                {
                  storage_type=FloatPixel;
                  quantum_size=sizeof(float);
                }
              else if (LocaleCompare("Double",argv[arg]) == 0)
                {
                  storage_type=DoublePixel;
                  quantum_size=sizeof(double);
                }
              else
                {
                  printf("Unrecognized storagetype argument %s\n",argv[arg]);
                  fflush(stdout);
                  exit_status = 1;
                  goto program_exit;
                }
            }
        }
      else
        break;
    }
  if (arg != argc-2)
    {
      printf ( "Usage: %s -debug events -depth integer -log format -size geometry -storagetype type] infile map\n", argv[0] );
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  strncpy(infile, argv[arg], MaxTextExtent-1 );
  arg++;
  strncpy( map, argv[arg], MaxTextExtent-1 );

  for (arg=0; arg < argc; arg++)
    printf("%s ", argv[arg]);
  printf("\n");
  fflush(stdout);

  /*
   * Read original image
   */
  GetImageInfo( &imageInfo );
  GetExceptionInfo( &exception );
  imageInfo.dither = 0;
  strncpy( imageInfo.filename, infile, MaxTextExtent-1 );
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

  /*  If a CMYK map is specified, make sure that input image is CMYK */
  if (strchr(map,'c') || strchr(map,'C') || strchr(map,'m') || strchr(map,'M') ||
      strchr(map,'y') || strchr(map,'y') || strchr(map,'k') || strchr(map,'k'))
    TransformColorspace(original,CMYKColorspace);

  /*
   * Obtain original image size if format requires it
   */
  rows    = original->rows;
  columns = original->columns;

  /*
   * Save image to array
   */
  pixels=AcquireMemory(quantum_size*strlen(map)*rows*columns);
  if( !pixels )
    {
      printf ( "Failed to allocate memory for pixels\n");
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "Writing image to pixel array");
  if( !DispatchImage(original,0,0,original->columns,original->rows,map,
                storage_type,pixels,&exception) )
    {
      printf ( "DispatchImage returned error status\n");
      if (exception.severity != UndefinedException)
        CatchException(&exception);
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  DestroyImage( original );
  original = (Image*)NULL;

  /*
   * Read image back from pixel array
   */

  original = ConstituteImage(columns,rows,map,storage_type,pixels,&exception);
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read image from pixels array\n" );
      if (exception.severity != UndefinedException)
        CatchException(&exception);
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  /*
   * Save image to pixel array
   */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "Writing image to pixel array");
  if( !DispatchImage(original,0,0,original->columns,original->rows,map,
                storage_type,pixels,&exception) )
    {
      printf ( "DispatchImage returned error status\n");
      if (exception.severity != UndefinedException)
        CatchException(&exception);
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  /*
   * Read image back from pixel array
   */
  final = ConstituteImage(columns,rows,map,storage_type,pixels,&exception);
  if ( final == (Image *)NULL )
    {
      printf ( "Failed to read image from pixels array\n" );
      if (exception.severity != UndefinedException)
        CatchException(&exception);
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

  LiberateMemory( (void**)&pixels );

  /*
   * Check final output
   */
  
  if ( !IsImagesEqual(original, final ) &&
       (original->error.normalized_mean_error > fuzz_factor) )
    {
      printf( "Constitute check failed: %u/%.6f/%.6fe\n",
              (unsigned int) original->error.mean_error_per_pixel,
              original->error.normalized_mean_error,
              original->error.normalized_maximum_error);
      fflush(stdout);
      exit_status = 1;
      goto program_exit;
    }

 program_exit:
  if (original)
    DestroyImage( original );
  original = (Image*)NULL;
  if (final)
    DestroyImage( final );
  final = (Image*)NULL;
  if (pixels)
    LiberateMemory( (void**)&pixels );

  DestroyMagick();

  if (pause)
    getc(stdin);
  return exit_status;
}
