/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                CCCC   OOO   N   N  V   V  EEEEE  RRRR   TTTTT               %
%               C      O   O  NN  N  V   V  E      R   R    T                 %
%               C      O   O  N N N  V   V  EEE    RRRR     T                 %
%               C      O   O  N  NN   V V   E      R R      T                 %
%                CCCC   OOO   N   N    V    EEEEE  R  R     T                 %
%                                                                             %
%                Convert an image from one format to another.                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                April 1992                                   %
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
%  Convert converts an input file using one image format to an output file
%  with a differing image format. By default, the image format is determined
%  by its magic number. To specify a particular image format, precede the
%  filename with an image format name and a colon (i.e. ps:image) or specify
%  the image type as the filename suffix (i.e. image.ps). Specify file as -
%  for standard input or output. If file has the extension .Z, the file is
%  decoded with uncompress.
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
%   C o n v e r t U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure ConvertUsage displays the program command syntax.
%
%  The format of the ConvertUsage method is:
%
%      void ConvertUsage()
%
*/
static void ConvertUsage(void)
{
  static const char
    *options[]=
    {
      "-adjoin              join images into a single multi-image file",
      "-affine matrix       affine transform matrix",
      "-antialias           remove pixel-aliasing",
      "-append              append an image sequence",
      "-authenticate value  decrypt image with this password",
      "-average             average an image sequence",
      "-background color    background color",
      "-blur geometry       blur the image",
      "-border geometry     surround image with a border of color",
      "-bordercolor color   border color",
      "-cache threshold     megabytes of memory available to the pixel cache",
      "-channel type        extract a particular color channel from image",
      "-charcoal radius     simulate a charcoal drawing",
      "-chop geometry       remove pixels from the image interior",
      "-clip                apply clipping path if the image has one",
      "-coalesce            merge a sequence of images",
      "-colorize value      colorize the image with the fill color",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compress type       image compression tyhpe",
      "-contrast            enhance or reduce the image contrast",
      "-crop geometry       preferred size and location of the cropped image",
      "-cycle amount        cycle the image colormap",
      "-debug events        display copious debugging information",
      "-deconstruct         break down an image sequence into constituent parts",
      "-delay value         display the next image after pausing",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-despeckle           reduce the speckles within an image",
      "-display server      get image or font from this X server",
      "-dispose method      GIF disposal method",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-draw string         annotate the image with a graphic primitive",
      "-edge radius         apply a filter to detect edges in the image",
      "-emboss radius       emboss an image",
      "-encoding type       text encoding type",
      "-endian type         LSB or MSB",
      "-enhance             apply a digital filter to enhance a noisy image",
      "-equalize            perform histogram equalization to an image",
      "-fill color          color to use when filling a graphic primitive",
      "-filter type         use this filter when resizing an image",
      "-flatten             flatten a sequence of images",
      "-flip                flip image in the vertical direction",
      "-flop                flop image in the horizontal direction",
      "-font name           render text with this font",
      "-frame geometry      surround image with an ornamental border",
      "-fuzz distance       colors within this distance are considered equal",
      "-gamma value         level of gamma correction",
      "-gaussian geometry   gaussian blur an image",
      "-geometry geometry   perferred size or location of the image",
      "-gravity type        horizontal and vertical text placement",
      "-help                print program options",
      "-implode amount      implode image pixels about the center",
      "-intent type         Absolute, Perceptual, Relative, or Saturation",
      "-interlace type      None, Line, Plane, or Partition",
      "-label name          assign a label to an image",
      "-level value         adjust the level of image contrast",
      "-list type           Color, Delegate, Format, Magic, Module, or Type",
      "-loop iterations     add Netscape loop extension to your GIF animation",
      "-map filename        transform image colors to match this set of colors",
      "-mask filename       set the image clip mask",
      "-matte               store matte channel if the image has one",
      "-median radius       apply a median filter to the image",
      "-modulate value      vary the brightness, saturation, and hue",
      "-monochrome          transform image to black and white",
      "-morph value         morph an image sequence",
      "-mosaic              create a mosaic from an image sequence",
      "-negate              replace every pixel with its complementary color ",
      "-noop                do not apply options to image",
      "-noise radius        add or reduce noise in an image",
      "-normalize           transform image to span the full range of colors",
      "-opaque color        change this color to the fill color",
      "-page geometry       size and location of an image canvas",
      "-paint radius        simulate an oil painting",
      "-ping                efficiently determine image attributes",
      "-pointsize value     font point size",
      "-preview type        image preview type",
      "-profile filename    add ICM or IPTC information profile to image",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-raise value         lighten/darken image edges to create a 3-D effect",
      "-region geometry     apply options to a portion of the image",
      "-raise value         lighten/darken image edges to create a 3-D effect",
      "-render              render vector graphics",
      "-resize geometry     resize the image",
      "-roll geometry       roll an image vertically or horizontally",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sample geometry     scale image with pixel sampling",
      "-sampling_factor geometry",
      "                     horizontal and vertical sampling factor",
      "-scale geometry      scale the image",
      "-scene value         image scene number",
      "-seed value          pseudo-random number generator seed value",
      "-segment values      segment an image",
      "-shade degrees       shade the image using a distant light source",
      "-sharpen geometry    sharpen the image",
      "-shave geometry      shave pixels from the image edges",
      "-shear geometry      slide one edge of the image along the X or Y axis",
      "-size geometry       width and height of image",
      "-solarize threshold  negate all pixels above the threshold level",
      "-spread amount       displace image pixels by a random amount",
      "-stroke color        graphic primitive stroke color",
      "-strokewidth value   graphic primitive stroke width",
      "-swirl degrees       swirl image pixels about the center",
      "-texture filename    name of texture to tile onto the image background",
      "-threshold value     threshold the image",
      "-tile filename       tile image when filling a graphic primitive",
      "-transform           affine transform image",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-type type           image type",
      "-undercolor color    annotation bounding box color",
      "-units type          PixelsPerInch, PixelsPerCentimeter, or Undefined",
      "-unsharp geometry    sharpen the image",
      "-verbose             print detailed information about the image",
      "-view                FlashPix viewing transforms",
      "-wave geometry       alter an image along a sine wave",
      "-write filename      write images to this file",
      (char *) NULL
    };

  const char
    **p;

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion((unsigned long *) NULL));
  (void) printf("Copyright: %.1024s\n\n",GetMagickCopyright());
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] "
    "file ...] [options ...] file\n",SetClientName((char *) NULL));
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
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
      ConvertUsage();
    if (LocaleCompare("?",option+1) == 0)
      ConvertUsage();
  }
  InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,"Memory allocation failed",
      (char *) NULL);
  if (argc < 3)
    ConvertUsage();
  image_info=CloneImageInfo((ImageInfo *) NULL);
  GetExceptionInfo(&exception);
  status=ConvertImageCommand(image_info,argc,argv,(char **) NULL,&exception);
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  DestroyImageInfo(image_info);
  DestroyExceptionInfo(&exception);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(!status);
  return(False);
}
