/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%         CCCC   OOO   M   M  PPPP    OOO   SSSSS  IIIII  TTTTT  EEEEE        %
%        C      O   O  MM MM  P   P  O   O  SS       I      T    E            %
%        C      O   O  M.M M  PPPP   O   O   SSS     I      T    EEE          %
%        C      O   O  M   M  P      O   O     SS    I      T    E            %
%         CCCC   OOO   M   M  P       OOO   SSSSS  IIIII    T    EEEEE        %
%                                                                             %
%                                                                             %
%                        Digitally composite two images.                      %
%                                                                             %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               January 1993                                  %
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
%  Program Composite composites images to create new images.
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p o s i t e U s a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompositeUsage() displays the program command syntax.
%
%  The format of the CompositeUsage method is:
%
%      void CompositeUsage()
%
%
*/
static void CompositeUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-affine matrix       affine transform matrix",
      "-cache threshold     number of megabytes available to the pixel cache",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compose operator    composite operator",
      "-compress type       image compression tyhpe",
      "-debug               display copious debugging information",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-displace geometry   shift image pixels defined by a displacement map",
      "-display server      get image or font from this X server",
      "-dispose method      GIF disposal method",
      "-dissolve value      dissolve the two images a given percent",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-encoding type       font encoding type",
      "-endian type         LSB or MSB",
      "-filter type         use this filter when resizing an image",
      "-font name           render text with this font",
      "-geometry geometry   location of the composite image",
      "-gravity type        which direction to gravitate towards",
      "-help                print program options",
      "-interlace type      None, Line, Plane, or Partition",
      "-label name          ssign a label to an image",
      "-matte               store matte channel if the image has one",
      "-monochrome          transform image to black and white",
      "-negate              replace every pixel with its complementary color ",
      "-page geometry       size and location of an image canvas",
      "-profile filename    add ICM or IPTC information profile to image",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-resize geometry     resize the image",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sampling_factor geometry",
      "                     horizontal and vertical sampling factor",
      "-scene value         image scene number",
      "-sharpen geometry    sharpen the image",
      "-size geometry       width and height of image",
      "-stegano offset      hide watermark within an image",
      "-stereo              combine two image to create a stereo anaglyph",
      "-tile                repeat composite operation across image",
      "-transform           affine transform image",
      "-treedepth value     color tree depth",
      "-type type           image type",
      "-units type          PixelsPerInch, PixelsPerCentimeter, or Undefined",
      "-unsharp geometry    sharpen the image",
      "-verbose             print detailed information about the image",
      "-watermark geometry  percent brightness and saturation of a watermark",
      "-write filename      write images to this file",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [options ...] image [options ...] composite\n"
    "  [ [options ...] mask ] [options ...] composite\n",
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
  char
    *option;

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  register int
    i;

  unsigned int
    status;

  ReadCommandlLine(argc,&argv);
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleCompare("help",option+1) == 0)
      CompositeUsage();
    if (LocaleCompare("?",option+1) == 0)
      CompositeUsage();
  }
  InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,"Memory allocation failed",
      (char *) NULL);
  if (argc < 4)
    CompositeUsage();
  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  status=CompositeImageCommand(image_info,argc,argv,(char **) NULL,&exception);
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  DestroyImageInfo(image_info);
  DestroyExceptionInfo(&exception);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(!status);
  return(False);
}
