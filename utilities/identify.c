/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           IIIII  DDDD   EEEEE  N   N  TTTTT  IIIII  FFFFF  Y   Y            %
%             I    D   D  E      NN  N    T      I    F       Y Y             %
%             I    D   D  EEE    N N N    T      I    FFF      Y              %
%             I    D   D  E      N  NN    T      I    F        Y              %
%           IIIII  DDDD   EEEEE  N   N    T    IIIII  F        Y              %
%                                                                             %
%                                                                             %
%               Identify an Image Format and Characteristics.                 %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                            September 1994                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Identify describes the format and characteristics of one or more image
%  files.  It will also report if an image is incomplete or corrupt.
%
%  The identify program syntax is:
%
%  Usage: identify [options ...] file [ files... ]
%
%  Where options include:
%    -cache threshold  megabytes of memory available to the pixel cache
%    -density geometry vertical and horizontal density of the image
%    -depth value      depth of the image
%    -format "string"  output formatted image characteristics
%    -size geometry    width and height of image
%    -verbose          print detailed information about the image
%
%
*/

/*
  Include declarations.
*/
#include "magick/magick.h"
#include "magick/define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s a g e                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure Usage displays the program usage;
%
%  The format of the Usage method is:
%
%      void Usage()
%
%
*/
static void Usage()
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-cache threshold   megabytes of memory available to the pixel cache",
      "-density geometry  vertical and horizontal density of the image",
      "-depth value       depth of the image",
      "-format \"string\"   output formatted image characteristics",
      "-size geometry     width and height of image",
      "-verbose           print detailed information about the image",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [ options ... ] file [ file ... ]\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  Exit(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M a i n                                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(int argc,char **argv)
{
  char
    *format,
    *option;

  double
    sans;

  ExceptionInfo
    exception;

  Image
    *image,
    *p;

  ImageInfo
    *image_info;

  int
    number_images,
    x;

  register int
    i;

  unsigned int
    count,
    status;

  /*
    Initialize command line arguments.
  */
  ReadCommandlLine(argc,&argv);
  if (LocaleCompare("identify",argv[0]) == 0)
    InitializeMagick(GetExecutionPath(argv[0]));
  else
    InitializeMagick(*argv);
  format=(char *) NULL;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if (LocaleNCompare("-format",argv[i],2) == 0)
      {
        i++;
        if (i == argc)
          MagickError(OptionError,"Missing format string",option);
        CloneString(&format,argv[i]);
        argv[i]=(char *) "";
        break;
      }
  }
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  if (argc < 2)
    Usage();
  /*
    Set defaults.
  */
  count=0;
  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image=(Image *) NULL;
  number_images=0;
  /*
    Identify an image.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) >= 2) && ((*option == '-') || (*option == '+')))
      {
        switch(*(option+1))
        {
          case 'c':
          {
            if (LocaleNCompare("cache",option+1,3) == 0)
              {
                SetCacheThreshold(0);
                if (*option == '-')
                  {
                    i++;
                    if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                      MagickError(OptionError,"Missing threshold",option);
                    SetCacheThreshold(atoi(argv[i]));
                  }
                break;
              }
            MagickError(OptionError,"Unrecognized option",option);
            break;
          }
          case 'd':
          {
            if (LocaleNCompare("debug",option+1,3) == 0)
              {
                image_info->debug=(*option == '-');
                break;
              }
            if (LocaleNCompare("density",option+1,3) == 0)
              {
                CloneString(&image_info->density,(char *) NULL);
                if (*option == '-')
                  {
                  i++;
                    if ((i == argc) || !IsGeometry(argv[i]))
                      MagickError(OptionError,"Missing geometry",option);
                    (void) CloneString(&image_info->density,argv[i]);
                  }
                break;
              }
            if (LocaleNCompare("depth",option+1,3) == 0)
              {
                image_info->depth=QuantumDepth;
                if (*option == '-')
                  {
                    i++;
                    if ((i == argc) || !sscanf(argv[i],"%d",&x))
                      MagickError(OptionError,"Missing image depth",option);
                    image_info->depth=atoi(argv[i]);
                  }
                break;
              }
            MagickError(OptionError,"Unrecognized option",option);
            break;
          }
          case 'f':
          {
            if (LocaleNCompare("format",option+1,2) == 0)
              {
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      MagickError(OptionError,"Missing format string",option);
                  }
                break;
              }
            MagickError(OptionError,"Unrecognized option",option);
            break;
          }
          case 'h':
          {
            if (LocaleNCompare("help",option+1,2) == 0)
              {
                Usage();
                break;
              }
            MagickError(OptionError,"Unrecognized option",option);
            break;
          }
          case 'p':
          {
            if (LocaleNCompare("ping",option+1,2) == 0)
              {
                MagickWarning(OptionError,"Deprecated option",option);
                break;
              }
            MagickError(OptionError,"Unrecognized option",option);
            break;
          }
          case 's':
          {
            if (LocaleNCompare("size",option+1,2) == 0)
              {
                CloneString(&image_info->size,(char *) NULL);
                if (*option == '-')
                  {
                    i++;
                    if ((i == argc) || !IsGeometry(argv[i]))
                      MagickError(OptionError,"Missing geometry",option);
                    (void) CloneString(&image_info->size,argv[i]);
                  }
                break;
              }
            MagickError(OptionError,"Unrecognized option",option);
            break;
          }
          case 'v':
          {
            if (LocaleNCompare("verbose",option+1,4) == 0)
              {
                image_info->verbose=(*option == '-');
                break;
              }
            MagickError(OptionError,"Unrecognized option",option);
            break;
          }
          case '?':
          {
            Usage();
            break;
          }
          default:
          {
            MagickError(OptionError,"Unrecognized option",option);
            break;
          }
        }
        continue;
      }
    /*
      Identify image.
    */
    (void) strcpy(image_info->filename,argv[i]);
    if (image_info->verbose)
      image=ReadImage(image_info,&exception);
    else
      image=PingImage(image_info,&exception);
    if (exception.severity != UndefinedException)
      MagickWarning(exception.severity,exception.reason,
        exception.description);
    if (image == (Image *) NULL)
      continue;
    for (p=image; p != (Image *) NULL; p=p->next)
    {
      if (p->scene == 0)
        p->scene=count++;
      if (format == (char *) NULL)
        DescribeImage(p,stdout,image_info->verbose);
      else
        {
          char
            *text;

          text=TranslateText((ImageInfo *) NULL,image,format);
          if (text == (char *) NULL)
            ThrowBinaryException(ResourceLimitWarning,
              "Unable to format image data","Memory allocation failed");
          (void) fputs(text,stdout);
          (void) fputc('\n',stdout);
        }
    }
    DestroyImages(image);
    number_images++;
  }
  if ((i != argc) || (number_images == 0))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  DestroyImageInfo(image_info);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(0);
  return(False);
}
