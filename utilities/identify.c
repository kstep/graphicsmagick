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
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
%   I d e n t i f y U s a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IdentifyUsage() displays the program command syntax.
%
%  The format of the IdentifyUsage method is:
%
%      void IdentifyUsage()
%
%
*/
static void IdentifyUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-cache threshold   megabytes of memory available to the pixel cache",
      "-debug             display copious debugging information",
      "-density geometry  vertical and horizontal density of the image",
      "-depth value       depth of the image",
      "-format \"string\"   output formatted image characteristics",
      "-help              print program options",
      "-size geometry     width and height of image",
      "-verbose           print detailed information about the image",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] "
    "file ... ]\n",SetClientName((char *) NULL));
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
%   I d e n t i f y U t i l i t y                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IdentifyUtility() describes the format and characteristics of one or more
%  image files. It will also report if an image is incomplete or corrupt.
%  The information displayed includes the scene number, the file name, the
%  width and height of the image, whether the image is colormapped or not,
%  the number of colors in the image, the number of bytes in the image, the
%  format of the image (JPEG, PNM, etc.), and finally the number of seconds
%  it took to read and process the image.
%
%  The format of the IdentifyUtility method is:
%
%      unsigned int IdentifyUtility(const int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%
*/
int IdentifyUtility(int argc,char **argv)
{
  char
    *format,
    *option,
    *q,
    *text;

  double
    sans;

  ExceptionInfo
    exception;

  Image
    *image;

  ImageInfo
    *image_info;

  long
    count,
    number_images,
    x;

  register Image
    *p;

  register long
    i;

  unsigned int
    status;

  format=(char *) NULL;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if (LocaleCompare("-format",argv[i]) == 0)
      {
        i++;
        if (i == argc)
          MagickError(OptionError,"Missing format string",option);
        (void) CloneString(&format,argv[i]);
        break;
      }
  }
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  if (argc < 2)
    IdentifyUsage();
  /*
    Set defaults.
  */
  count=0;
  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image=(Image *) NULL;
  number_images=0;
  status=True;
  /*
    Identify an image.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        /*
          Identify image.
        */
        (void) strncpy(image_info->filename,argv[i],MaxTextExtent-1);
        if (format != (char *) NULL)
          for (q=strchr(format,'%'); q != (char *) NULL; q=strchr(q+1,'%'))
            if ((*(q+1) == 'k') || (*(q+1) == '#'))
              {
                image_info->verbose=True;
                break;
              }
        if (image_info->verbose)
          image=ReadImage(image_info,&exception);
        else
          image=PingImage(image_info,&exception);
        if (exception.severity != UndefinedException)
          MagickWarning(exception.severity,exception.reason,
            exception.description);
        status&=image != (Image *) NULL;
        if (image == (Image *) NULL)
          continue;
        for (p=image; p != (Image *) NULL; p=p->next)
        {
          if (p->scene == 0)
            p->scene=count++;
          if (format == (char *) NULL)
            {
              DescribeImage(p,stdout,image_info->verbose);
              continue;
            }
          text=TranslateText(image_info,p,format);
          if (text == (char *) NULL)
            ThrowBinaryException(ResourceLimitWarning,
              "Unable to format image metadata","Memory allocation failed");
          (void) fputs(text,stdout);
          (void) fputc('\n',stdout);
          LiberateMemory((void **) &text);
        }
        DestroyImageList(image);
        number_images++;
        continue;
      }
    switch(*(option+1))
    {
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            SetCacheThreshold(0);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  MagickError(OptionError,"Missing threshold",option);
                SetCacheThreshold(atol(argv[i]));
              }
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            image_info->debug=(*option == '-');
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
              i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickError(OptionError,"Missing geometry",option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing image depth",option);
                image_info->depth=atol(argv[i]);
              }
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 'f':
      {
        if (LocaleCompare("format",option+1) == 0)
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
        if (LocaleCompare("help",option+1) == 0)
          {
            IdentifyUsage();
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 'p':
      {
        if (LocaleCompare("ping",option+1) == 0)
          break;  /* default is ping; silently ignore */
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 's':
      {
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
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
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case '?':
      {
        IdentifyUsage();
        break;
      }
      default:
      {
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
    }
  }
  if ((i != argc) || (number_images == 0))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  DestroyImageInfo(image_info);
  return(status);
}

#if !defined(MagickAPI)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a i n                                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(int argc,char **argv)
{
  unsigned int
    status;

  /*
    Initialize command line arguments.
  */
  ReadCommandlLine(argc,&argv);
  InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  if (argc < 2)
    IdentifyUsage();
  status=IdentifyUtility(argc,argv);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(!status);
  return(False);
}
#endif
