/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              M   M   OOO    GGGG  RRRR   IIIII  FFFFF  Y   Y                %
%              MM MM  O   O  G      R   R    I    F       Y Y                 %
%              M M M  O   O  G GG   RRRRR    I    FFF      Y                  %
%              M   M  O   O  G   G  R R      I    F        Y                  %
%              M   M   OOO    GGG   R  R   IIIII  F        Y                  %
%                                                                             %
%                                                                             %
%               Transmogrify an Image or Sequence of Images.                  %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                            December 1992                                    %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
%  `Mogrify' applies one or more image transforms to an image or sequence of
%   images and overwrites the original image.
%
%  The Mogrify program command syntax is:
%
%  Usage: mogrify [options ...] file [ [options ...] file ...]
%
%  Where options include:
%    -antialias           remove pixel-aliasing
%    -blur order          apply a filter to blur the image
%    -border geometry     surround image with a border of color
%    -bordercolor color   border color
%    -box color           color for annotation bounding box
%    -cache threshold     megabytes of memory available to the pixel cache
%    -charcoal order      simulate a charcoal drawing
%    -colorize value      colorize the image with the pen color
%    -colors value        preferred number of colors in the image
%    -colorspace type     alternate image colorspace
%    -comment string      annotate image with comment
%    -compress type       type of image compression
%    -contrast            enhance or reduce the image contrast
%    -crop geometry       preferred size and location of the cropped image
%    -cycle amount        cycle the image colormap
%    -delay value         display the next image after pausing
%    -density geometry    vertical and horizontal density of the image
%    -depth value         depth of the image
%    -despeckle           reduce the speckles within an image",
%    -display server      obtain image or font from this X server
%    -dispose method      GIF disposal method
%    -dither              apply Floyd/Steinberg error diffusion to image
%    -draw string         annotate the image with a graphic primitive
%    -edge order          apply a filter to detect edges in the image
%    -emboss order        emboss an image
%    -enhance             apply a digital filter to enhance a noisy image
%    -equalize            perform histogram equalization to an image
%    -fill color          color to use when filling a graphic primitive
%    -filter type         use this filter when resizing an image
%    -flip                flip image in the vertical direction
%    -flop                flop image in the horizontal direction
%    -font name           X11 font for displaying text
%    -format type         image format type
%    -frame geometry      surround image with an ornamental border
%    -fuzz distance       colors within this distance are considered equal
%    -gamma value         level of gamma correction
%    -gaussian geometry   gaussian blur an image
%    -geometry geometry   perferred size or location of the image
%    -implode amount      implode image pixels about the center
%    -interlace type      None, Line, Plane, or Partition
%    -label name          assign a label to an image
%    -layer type          Red, Green, Blue, Matte
%    -linewidth value     width of line in pixels
%    -loop iterations     add Netscape loop extension to your GIF animation
%    -map filename        transform image colors to match this set of colors
%    -matte               store matte channel if the image has one
%    -median order        apply a median filter to the image
%    -modulate value      vary the brightness, saturation, and hue
%    -monochrome          transform image to black and white
%    -negate              replace every pixel with its complementary color 
%    -noise value         add or reduce noise in an image
%    -normalize           transform image to span the full range of colors
%    -opaque color        change this color to the pen color
%    -page geometry       size and location of an image canvas
%    -paint radius        simulate an oil painting
%    -pointsize value     pointsize of Postscript font
%    -quality value       JPEG/MIFF/PNG compression level
%    -raise value         lighten/darken image edges to create a 3-D effect
%    -region geometry     apply options to a portion of the image
%    -roll geometry       roll an image vertically or horizontally
%    -rotate degrees      apply Paeth rotation to the image
%    -sample geometry     scale image with pixel sampling
%    -scale geometry      resize image
%    -scene value         image scene number
%    -seed value          pseudo-random number generator seed value
%    -segment values      segment an image
%    -shade degrees       shade the image using a distant light source
%    -sharpen order       apply a filter to sharpen the image
%    -shear geometry      slide one edge of the image along the X or Y axis
%    -size geometry       width and height of image
%    -solarize threshold  negate all pixels above the threshold level
%    -spread amount       displace image pixels by a random amount
%    -stroke color        color to use when stoking a graphic primitive
%    -swirl degrees       swirl image pixels about the center
%    -texture filename    name of texture to tile onto the image background
%    -threshold value     threshold the image
%    -transparent color   make this color transparent within the image
%    -treedepth value     depth of the color classification tree
%    -units type          PixelsPerInch, PixelsPerCentimeter, or Undefined
%    -verbose             print detailed information about the image
%    -view                FlashPix viewing transforms
%    -wave geometry       alter an image along a sine wave
%
%  By default, the image format of `file' is determined by its magic
%  number.  To specify a particular image format, precede the filename
%  with an image format name and a colon (i.e. ps:image) or specify the
%  image type as the filename suffix (i.e. image.ps).  Specify 'file' as
%  '-' for standard input or output.
%
%
*/

/*
  Include declarations.
*/
#include "magick/magick.h"
#include "magick/defines.h"

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
%  Method Usage displays the program command syntax.
%
%  The format of the Usage method is:
%
%      void Usage(const char *client_name)
%
%    o client_name: a character string representing the name of the client
%      program.
%
%
*/
static void Usage(const char *client_name)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-antialias           remove pixel-aliasing",
      "-blur order          apply a filter to blur the image",
      "-border geometry     surround image with a border of color",
      "-bordercolor color   border color",
      "-box color           color for annotation bounding box",
      "-cache threshold     megabytes of memory available to the pixel cache",
      "-charcoal order      simulate a charcoal drawing",
      "-colorize value      colorize the image with the pen color",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compress type       type of image compression",
      "-contrast            enhance or reduce the image contrast",
      "-crop geometry       preferred size and location of the cropped image",
      "-cycle amount        cycle the image colormap",
      "-delay value         display the next image after pausing",
      "-density geometry    vertical and horizontal density of the image",
      "-depth value         depth of the image",
      "-despeckle           reduce the speckles within an image",
      "-display server      obtain image or font from this X server",
      "-dispose method      GIF disposal method",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-draw string         annotate the image with a graphic primitive",
      "-edge order          apply a filter to detect edges in the image",
      "-emboss order        emboss an image",
      "-enhance             apply a digital filter to enhance a noisy image",
      "-equalize            perform histogram equalization to an image",
      "-fill color          color to use when filling a graphic primitive",
      "-filter type         use this filter when resizing an image",
      "-flip                flip image in the vertical direction",
      "-flop                flop image in the horizontal direction",
      "-font name           X11 font for displaying text",
      "-format type         image format type",
      "-frame geometry      surround image with an ornamental border",
      "-fuzz distance       colors within this distance are considered equal",
      "-gamma value         level of gamma correction",
      "-gaussian geometry   gaussian blur an image",
      "-geometry geometry   perferred size or location of the image",
      "-implode amount      implode image pixels about the center",
      "-interlace type      None, Line, Plane, or Partition",
      "-label name          assign a label to an image",
      "-layer type          Red, Green, Blue, Matte",
      "-linewidth value     width of line in pixels",
      "-loop iterations     add Netscape loop extension to your GIF animation",
      "-map filename        transform image colors to match this set of colors",
      "-matte               store matte channel if the image has one",
      "-median order        apply a median filter to the image",
      "-modulate value      vary the brightness, saturation, and hue",
      "-monochrome          transform image to black and white",
      "-negate              replace every pixel with its complementary color ",
      "-noise value         add or reduce noise in an image.",
      "-normalize           transform image to span the full range of colors",
      "-opaque color        change this color to the pen color",
      "-page geometry       size and location of an image canvas",
      "-paint radius        simulate an oil painting",
      "-pen color           color for annotating or changing opaque color",
      "-pointsize value     pointsize of Postscript font",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-raise value         lighten/darken image edges to create a 3-D effect",
      "-region geometry     apply options to a portion of the image",
      "-roll geometry       roll an image vertically or horizontally",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sample geometry     scale image with pixel sampling",
      "-scale geometry      resize image",
      "-scene number        image scene number",
      "-seed value          pseudo-random number generator seed value",
      "-segment values      segment an image",
      "-shade degrees       shade the image using a distant light source",
      "-sharpen order       apply a filter to sharpen the image",
      "-shear geometry      slide one edge of the image along the X or Y axis",
      "-size geometry       width and height of image",
      "-solarize threshold  negate all pixels above the threshold level",
      "-spread amount       displace image pixels by a random amount",
      "-stroke color        color to use when stoking a graphic primitive",
      "-swirl degrees       swirl image pixels about the center",
      "-texture filename    name of texture to tile onto the image background",
      "-threshold value     threshold the image",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     depth of the color classification tree",
      "-units type          PixelsPerInch, PixelsPerCentimeter, or Undefined",
      "-verbose             print detailed information about the image",
      "-view                FlashPix viewing transforms",
      "-wave geometry       alter an image along a sine wave",
      (char *) NULL
    };

  (void) printf("Version: %.1024s\n",MagickVersion);
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf(
    "Usage: %.1024s [-options ...] file [ [-options ...] file ...]\n",
    client_name);
  (void) printf("\nWhere options include: \n");
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
  ListMagickInfo((FILE *) NULL);
  ListDelegateInfo((FILE *) NULL);
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
    *client_name,
    filename[MaxTextExtent],
    *format,
    *option;

  double
    sans;

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  Image
    *image;

  int
    x;

  register Image
    *p;

  register int
    i;

  unsigned int
    global_colormap,
    scene,
    status;

  /*
    Initialize command line arguments.
  */
  ReadCommandlLine(argc,&argv);
  client_name=SetClientName(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  if (argc < 2)
    Usage(client_name);
  /*
    Set defaults.
  */
  format=(char *) NULL;
  global_colormap=False;
  image=(Image *) NULL;
  image_info=CloneImageInfo((ImageInfo *) NULL);
  scene=0;
  status=False;
  /*
    Parse command line.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((Extent(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'a':
        {
          if (LocaleNCompare("antialias",option+1,3) == 0)
            {
              image_info->antialias=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'b':
        {
          if (LocaleNCompare("background",option+1,5) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing background color",option);
                  (void) QueryColorDatabase(argv[i],
                    &image_info->background_color);
                }
              break;
            }
          if (LocaleNCompare("blur",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing order",option);
                }
              break;
            }
          if (LocaleNCompare("border",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("bordercolor",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing border color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->border_color);
                }
              break;
            }
          if (LocaleNCompare("box",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing box color",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'c':
        {
          if (LocaleNCompare("cache",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing threshold",option);
                }
              SetCacheThreshold(atoi(argv[i]));
              break;
            }
          if (LocaleNCompare("charcoal",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing order",option);
                }
              break;
            }
          if (LocaleNCompare("colorize",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing value",option);
                }
              break;
            }
          if (LocaleNCompare("colors",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing colors",option);
                }
              break;
            }
          if (LocaleNCompare("colorspace",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->colorspace=UndefinedColorspace;
                  if (LocaleCompare("cmyk",option) == 0)
                    image_info->colorspace=CMYKColorspace;
                  if (LocaleCompare("gray",option) == 0)
                    image_info->colorspace=GRAYColorspace;
                  if (LocaleCompare("ohta",option) == 0)
                    image_info->colorspace=OHTAColorspace;
                  if (LocaleCompare("rgb",option) == 0)
                    image_info->colorspace=RGBColorspace;
                  if (LocaleCompare("srgb",option) == 0)
                    image_info->colorspace=sRGBColorspace;
                  if (LocaleCompare("transparent",option) == 0)
                    image_info->colorspace=TransparentColorspace;
                  if (LocaleCompare("xyz",option) == 0)
                    image_info->colorspace=XYZColorspace;
                  if (LocaleCompare("ycbcr",option) == 0)
                    image_info->colorspace=YCbCrColorspace;
                  if (LocaleCompare("ycc",option) == 0)
                    image_info->colorspace=YCCColorspace;
                  if (LocaleCompare("yiq",option) == 0)
                    image_info->colorspace=YIQColorspace;
                  if (LocaleCompare("ypbpr",option) == 0)
                    image_info->colorspace=YPbPrColorspace;
                  if (LocaleCompare("yuv",option) == 0)
                    image_info->colorspace=YUVColorspace;
                  if (image_info->colorspace == UndefinedColorspace)
                    MagickError(OptionError,"Invalid colorspace type",option);
                }
              break;
            }
          if (LocaleNCompare("comment",option+1,4) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing comment",option);
                }
              break;
            }
          if (LocaleNCompare("compress",option+1,5) == 0)
            {
              image_info->compression=NoCompression;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->compression=UndefinedCompression;
                  if (LocaleCompare("None",option) == 0)
                    image_info->compression=NoCompression;
                  if (LocaleCompare("BZip",option) == 0)
                    image_info->compression=BZipCompression;
                  if (LocaleCompare("Fax",option) == 0)
                    image_info->compression=FaxCompression;
                  if (LocaleCompare("Group4",option) == 0)
                    image_info->compression=Group4Compression;
                  if (LocaleCompare("JPEG",option) == 0)
                    image_info->compression=JPEGCompression;
                  if (LocaleCompare("LZW",option) == 0)
                    image_info->compression=LZWCompression;
                  if (LocaleCompare("RunlengthEncoded",option) == 0)
                    image_info->compression=RunlengthEncodedCompression;
                  if (LocaleCompare("Zip",option) == 0)
                    image_info->compression=ZipCompression;
                  if (image_info->compression == UndefinedCompression)
                    MagickError(OptionError,"Invalid compression type",option);
                }
              break;
            }
          if (LocaleNCompare("contrast",option+1,3) == 0)
            break;
          if (LocaleNCompare("crop",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("cycle",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing amount",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'd':
        {
          if (LocaleNCompare("delay",option+1,3) == 0)
            {
              image_info->delay=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing delay",option);
                  image_info->delay=PostscriptGeometry(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("density",option+1,3) == 0)
            {
              image_info->density=(char *) NULL;
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
          if (LocaleNCompare("despeckle",option+1,3) == 0)
            break;
          if (LocaleCompare("display",option+1) == 0)
            {
              image_info->server_name=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing server name",option);
                  (void) CloneString(&image_info->server_name,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("dispose",option+1,5) == 0)
            {
              image_info->dispose=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing method",option);
                  image_info->dispose=PostscriptGeometry(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("dither",option+1,3) == 0)
            {
              image_info->dither=(*option == '-');
              break;
            }
          if (LocaleNCompare("draw",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing primitive",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'e':
        {
          if (LocaleNCompare("edge",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing order",option);
                }
              break;
            }
          if (LocaleNCompare("emboss",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing order",option);
                }
              break;
            }
          if (LocaleNCompare("enhance",option+1,2) == 0)
            break;
          if (LocaleNCompare("equalize",option+1,2) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'f':
        {
          if (LocaleCompare("fill",option+1) == 0)
            {
              (void) QueryColorDatabase("none",&image_info->fill);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing fill color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->fill);
                }
              break;
            }
          if (LocaleNCompare("filter",option+1,4) == 0)
            {
              if (*option == '-')
                {
                  FilterType
                    filter;

                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  filter=UndefinedFilter;
                  if (LocaleCompare("Point",option) == 0)
                    filter=PointFilter;
                  if (LocaleCompare("Box",option) == 0)
                    filter=BoxFilter;
                  if (LocaleCompare("Triangle",option) == 0)
                    filter=TriangleFilter;
                  if (LocaleCompare("Hermite",option) == 0)
                    filter=HermiteFilter;
                  if (LocaleCompare("Hanning",option) == 0)
                    filter=HanningFilter;
                  if (LocaleCompare("Hamming",option) == 0)
                    filter=HammingFilter;
                  if (LocaleCompare("Blackman",option) == 0)
                    filter=BlackmanFilter;
                  if (LocaleCompare("Gaussian",option) == 0)
                    filter=GaussianFilter;
                  if (LocaleCompare("Quadratic",option) == 0)
                    filter=QuadraticFilter;
                  if (LocaleCompare("Cubic",option) == 0)
                    filter=CubicFilter;
                  if (LocaleCompare("Catrom",option) == 0)
                    filter=CatromFilter;
                  if (LocaleCompare("Mitchell",option) == 0)
                    filter=MitchellFilter;
                  if (LocaleCompare("Lanczos",option) == 0)
                    filter=LanczosFilter;
                  if (LocaleCompare("Bessel",option) == 0)
                    filter=BesselFilter;
                  if (LocaleCompare("Sinc",option) == 0)
                    filter=SincFilter;
                  if (filter == UndefinedFilter)
                    MagickError(OptionError,"Invalid filter type",option);
                }
              break;
            }
          if (LocaleNCompare("flip",option+1,3) == 0)
            break;
          if (LocaleNCompare("flop",option+1,3) == 0)
            break;
          if (LocaleNCompare("font",option+1,3) == 0)
            {
              image_info->font=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing font name",option);
                  (void) CloneString(&image_info->font,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("format",option+1,3) == 0)
            {
              format=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing image type",option);
                  (void) CloneString(&format,argv[i]);
                  (void) strcpy(image_info->filename,format);
                  (void) strcat(image_info->filename,":");
                  SetImageInfo(image_info,False);
                  if (*image_info->magick == '\0')
                    MagickError(OptionError,"Unrecognized image format",format);
                }
              break;
            }
          if (LocaleNCompare("frame",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("fuzz",option+1,2) == 0)
            {
              image_info->fuzz=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing distance",option);
                  image_info->fuzz=atoi(argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'g':
        {
          if (LocaleNCompare("gamma",option+1,3) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                MagickError(OptionError,"Missing value",option);
              break;
            }
          if (LocaleCompare("gaussian",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing width",option);
              break;
            }
          if (LocaleNCompare("geometry",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("gravity",option+1,2) == 0)
            {
              int
                gravity;

              gravity=ForgetGravity;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  if (LocaleCompare("NorthWest",option) == 0)
                    gravity=NorthWestGravity;
                  if (LocaleCompare("North",option) == 0)
                    gravity=NorthGravity;
                  if (LocaleCompare("NorthEast",option) == 0)
                    gravity=NorthEastGravity;
                  if (LocaleCompare("West",option) == 0)
                    gravity=WestGravity;
                  if (LocaleCompare("Center",option) == 0)
                    gravity=CenterGravity;
                  if (LocaleCompare("East",option) == 0)
                    gravity=EastGravity;
                  if (LocaleCompare("SouthWest",option) == 0)
                    gravity=SouthWestGravity;
                  if (LocaleCompare("South",option) == 0)
                    gravity=SouthGravity;
                  if (LocaleCompare("SouthEast",option) == 0)
                    gravity=SouthEastGravity;
                  if (gravity == ForgetGravity)
                    MagickError(OptionError,"Invalid gravity type",option);
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
              Usage(client_name);
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'i':
        {
          if (LocaleNCompare("implode",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing amount",option);
                }
              break;
            }
          if (LocaleNCompare("interlace",option+1,3) == 0)
            {
              image_info->interlace=NoInterlace;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->interlace=UndefinedInterlace;
                  if (LocaleCompare("None",option) == 0)
                    image_info->interlace=NoInterlace;
                  if (LocaleCompare("Line",option) == 0)
                    image_info->interlace=LineInterlace;
                  if (LocaleCompare("Plane",option) == 0)
                    image_info->interlace=PlaneInterlace;
                  if (LocaleCompare("Partition",option) == 0)
                    image_info->interlace=PartitionInterlace;
                  if (image_info->interlace == UndefinedInterlace)
                    MagickError(OptionError,"Invalid interlace type",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'l':
        {
          if (LocaleNCompare("label",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing label name",option);
                }
              break;
            }
          if (LocaleNCompare("layer",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  LayerType
                    layer;

                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  layer=UndefinedLayer;
                  if (LocaleCompare("Red",option) == 0)
                    layer=RedLayer;
                  if (LocaleCompare("Green",option) == 0)
                    layer=GreenLayer;
                  if (LocaleCompare("Blue",option) == 0)
                    layer=BlueLayer;
                  if (LocaleCompare("Matte",option) == 0)
                    layer=MatteLayer;
                  if (layer == UndefinedLayer)
                    MagickError(OptionError,"Invalid layer type",option);
                }
              break;
            }
          if (LocaleNCompare("linewidth",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing size",option);
                }
              break;
            }
          if (LocaleNCompare("loop",option+1,2) == 0)
            {
              image_info->iterations=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing iterations",option);
                  image_info->iterations=PostscriptGeometry(argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'm':
        {
          if (LocaleNCompare("map",option+1,3) == 0)
            {
              global_colormap=(*option == '+');
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing file name",option);
                }
              break;
            }
          if (LocaleCompare("matte",option+1) == 0)
            break;
          if (LocaleNCompare("mattecolor",option+1,6) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing matte color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->matte_color);
                }
              break;
            }
          if (LocaleNCompare("modulate",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing value",option);
                }
              break;
            }
          if (LocaleNCompare("monochrome",option+1,4) == 0)
            {
              image_info->monochrome=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'n':
        {
          if (LocaleNCompare("negate",option+1,3) == 0)
            break;
         if (LocaleNCompare("noise",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing value",option);
                }
              if (*option == '+')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  if ((LocaleCompare("Uniform",option) != 0) &&
                      (LocaleCompare("Gaussian",option) != 0) &&
                      (LocaleCompare("Multiplicative",option) != 0) &&
                      (LocaleCompare("Impulse",option) != 0) &&
                      (LocaleCompare("Laplacian",option) != 0) &&
                      (LocaleCompare("Poisson",option) != 0))
                    MagickError(OptionError,"Invalid noise type",option);
                }
              break;
            }
          if (LocaleNCompare("normalize",option+1,3) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'o':
        {
          if (LocaleNCompare("opaque",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing opaque color",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'p':
        {
          if (LocaleNCompare("page",option+1,3) == 0)
            {
              image_info->page=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing page geometry",option);
                  image_info->page=PostscriptGeometry(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("paint",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing radius",option);
                }
              break;
            }
          if (LocaleCompare("pen",option+1) == 0)
            {
              (void) QueryColorDatabase("none",&image_info->fill);
              (void) QueryColorDatabase("none",&image_info->stroke);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing pen color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->fill);
                  (void) QueryColorDatabase(argv[i],&image_info->stroke);
                }
              break;
            }
          if (LocaleNCompare("pointsize",option+1,2) == 0)
            {
              image_info->pointsize=12;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing size",option);
                  image_info->pointsize=atof(argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'q':
        {
          if (LocaleNCompare("quality",option+1,2) == 0)
            {
              image_info->quality=75;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing quality",option);
                  image_info->quality=atoi(argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'r':
        {
          if (LocaleNCompare("raise",option+1,2) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing bevel width",option);
              break;
            }
          if (LocaleNCompare("region",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("roll",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("rotate",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing degrees",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 's':
        {
          if (LocaleNCompare("sample",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("scale",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("scene",option+1,3) == 0)
            {
              scene=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing scene number",option);
                  scene=atoi(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("seed",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing value",option);
                }
              srand((unsigned int) atoi(argv[i]));
              break;
            }
          if (LocaleNCompare("segment",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing threshold",option);
                }
              break;
            }
          if (LocaleNCompare("shade",option+1,5) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing azimuth",option);
              break;
            }
          if (LocaleNCompare("sharpen",option+1,5) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing order",option);
                }
              break;
            }
          if (LocaleNCompare("shear",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("size",option+1,2) == 0)
            {
              image_info->size=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&image_info->size,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("solarize",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing threshold",option);
                }
              break;
            }
          if (LocaleNCompare("spread",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing amount",option);
                }
              break;
            }
          if (LocaleCompare("stroke",option+1) == 0)
            {
              (void) QueryColorDatabase("none",&image_info->stroke);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->stroke);
                }
              break;
            }
          if (LocaleNCompare("swirl",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing degrees",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 't':
        {
          if (LocaleNCompare("texture",option+1,5) == 0)
            {
              image_info->texture=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing filename",option);
                  (void) CloneString(&image_info->texture,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("threshold",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing value",option);
                }
              break;
            }
          if (LocaleNCompare("transparent",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing transparent color",option);
                }
              break;
            }
          if (LocaleNCompare("treedepth",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing depth",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'u':
        {
          if (LocaleNCompare("units",option+1,3) == 0)
            {
              image_info->units=UndefinedResolution;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->units=UndefinedResolution;
                  if (LocaleCompare("PixelsPerInch",option) == 0)
                    image_info->units=PixelsPerInchResolution;
                  if (LocaleCompare("PixelsPerCentimeter",option) == 0)
                    image_info->units=PixelsPerCentimeterResolution;
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'v':
        {
          if (LocaleNCompare("verbose",option+1,2) == 0)
            {
              image_info->verbose=(*option == '-');
              break;
            }
          if (LocaleNCompare("view",option+1,3) == 0)
            {
              image_info->view=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing view transform",option);
                  (void) CloneString(&image_info->view,argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'w':
        {
          if (LocaleCompare("wave",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing amplitude",option);
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case '?':
        {
          Usage(client_name);
          break;
        }
        default:
        {
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
      }
    else
      {
        /*
          Option is a file name: begin by reading image from specified file.
        */
        (void) strcpy(image_info->filename,argv[i]);
        image=ReadImage(image_info,&exception);
        if (image == (Image *) NULL)
          {
            MagickWarning(exception.severity,exception.message,
              exception.qualifier);
            continue;
          }
        if (scene != 0)
          image->scene=scene;
        if (format != (char *) NULL)
          {
            register char
              *p;

            /*
              Modify filename to include a user specified image format.
            */
            p=image->filename+Extent(image->filename)-1;
            while ((*p != '.') && (p > (image->filename+1)))
              p--;
            p++;
            if (LocaleCompare(p,image->magick) == 0)
              (void) strcpy(p,format);
            else
              {
                FormatString(image_info->filename,"%.1024s:%.1024s",format,
                  image->filename);
                (void) strcpy(image->filename,image_info->filename);
              }
          }
        /*
          Transmogrify image as defined by the image processing options.
        */
        status=MogrifyImages(image_info,i,argv,&image);
        if (status == False)
          CatchImageException(image);
        if (global_colormap)
          (void) MapImages(image,(Image *) NULL,image_info->dither);
        /*
          Write transmogrified image to disk.
        */
        if (format != (char *) NULL)
          AppendImageFormat(format,image->filename);
        else
          if (LocaleCompare(image_info->filename,"-") != 0)
            {
              (void) strcpy(filename,image->filename);
              LocaleFilename(image->filename);
            }
        for (p=image; p != (Image *) NULL; p=p->next)
        {
          (void) strcpy(p->filename,image->filename);
          p->scene=scene++;
        }
        SetImageInfo(image_info,True);
        for (p=image; p != (Image *) NULL; p=p->next)
        {
          status=WriteImage(image_info,p);
          if (status == False)
            CatchImageException(p);
          if (image_info->adjoin)
            break;
        }
        for (p=image; p != (Image *) NULL; p=p->next)
          scene--;
        if (image_info->verbose)
          DescribeImage(image,stderr,False);
        if ((format == (char *) NULL) && (status != False))
          if (LocaleCompare(image_info->filename,"-") != 0)
            {
              (void) remove(filename);
              (void) rename(image->filename,filename);
            }
        DestroyImage(image);
      }
  }
  if ((i != argc) || (image == (Image *) NULL))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  DestroyImageInfo(image_info);
  DestroyDelegateInfo();
  DestroyMagickInfo();
  FreeMemory((void **) &argv);
  Exit(0);
  return(False);
}
