/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             M   M   OOO   N   N  TTTTT   AAA    GGGG  EEEEE                 %
%             MM MM  O   O  NN  N    T    A   A  G      E                     %
%             M M M  O   O  N N N    T    AAAAA  G  GG  EEE                   %
%             M   M  O   O  N  NN    T    A   A  G   G  E                     %
%             M   M   OOO   N   N    T    A   A   GGGG  EEEEE                 %
%                                                                             %
%                                                                             %
%              Montage Magick Image File Format Image via X11.                %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
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
%  Montage creates a composite image by combining several separate
%  images.  The images are tiled on the composite image with the name of
%  the image appearing just above the individual tile.
%
%  The composite image is constructed in the following manner.  First,
%  each image specified on the command line, except for the last, is
%  scaled to fit the maximum tile size.  The maximum tile size by default
%  is 256x256.  It can be modified with the -geometry command line
%  argument or X resource.  Note that the maximum tile size need not be a
%  square.  To respect the aspect ratio of each image append ~ to the
%  geometry specification.
%
%  Next the composite image is initialized with the color specified by the
%  -background command line argument or X resource.  The width and height
%  of the composite image is determined by the maximum tile size, the
%  number of tiles per row, the tile border width and height, the image
%  border width, and the label height.  The number of tiles per row specifies
%  how many images are to appear in each row of the composite image.  The
%  default is to have an equal number of images in each row and column of the
%  composite.  This value can be specified with -tiles.  The tile border
%  width and height, and the image border width defaults to the value of the
%  X resource -borderwidth.  It can be changed with the -borderwidth or
%  -geometry command line argument or X resource.  The label height is
%  determined by the font you specify with the -font command line argument or
%  X resource.  If you do not specify a font, a font is chosen that allows
%  the name of the image to fit the maximum width of a tiled area.  The label
%  colors is determined by the -background and -stroke command line
%  argument or X resource.  Note, that if the background and fill colors
%  are the same, labels will not appear.
%
%  Finally, each image is set onto the composite image, surrounded by its
%  border color, with its name centered just below it.  The individual images
%  are centered within the width of the tiled area.  The final argument on the
%  command line is the name assigned to the composite image.  The image is
%  written in the MIFF format and may by viewed or printed with `display'.
%
%  The Montage program command syntax is:
%
%  Usage: montage [options ...] file [ [options ...] file ...] file
%
%  Where options include:
%    -adjoin             join images into a single multi-image file
%    -blur geometry      blur the image
%    -cache threshold    megabytes of memory available to the pixel cache
%    -colors value       preferred number of colors in the image
%    -colorspace type    alternate image colorspace
%    -comment string     annotate image with comment
%    -compose operator   composite operator
%    -compress type      type of image compression
%    -crop geometry      preferred size and location of the cropped image
%    -density geometry   vertical and horizontal density of the image
%    -depth value        depth of the image
%    -display server     query fonts from this X server
%    -dispose method     GIF disposal method
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -draw string        annotate the image with a graphic primitive
%    -fill color         color to use when filling a graphic primitive
%    -filter type        use this filter when resizing an image
%    -frame geometry     surround image with an ornamental border
%    -gamma value        level of gamma correction
%    -geometry geometry  preferred tile and border sizes
%    -gravity direction  which direction to gravitate towards
%    -interlace type     None, Line, Plane, or Partition
%    -label name         assign a label to an image
%    -matte              store matte channel if the image has one
%    -mode type          Frame, Unframe, or Concatenate
%    -monochrome         transform image to black and white
%    -page geometry      size and location of an image canvas
%    -pointsize value    pointsize of Postscript font
%    -quality value      JPEG/MIFF/PNG compression level
%    -rotate degrees     apply Paeth rotation to the image
%    -scene value        image scene number
%    -shadow             add a shadow beneath a tile to simulate depth
%    -sharpen geometry   sharpen the image
%    -size geometry      width and height of image
%    -stroke color       color to use when stroking a graphic primitive
%    -texture filename   name of texture to tile onto the image background
%    -tile geometry      number of tiles per row and column
%    -transparent color  make this color transparent within the image
%    -treedepth value    depth of the color tree
%    -type type          image type
%    -verbose            print detailed information about the image
%
%  In addition to those listed above, you can specify these standard X
%  resources as command line options:  -background, -bordercolor -borderwidth,
%  -font, -mattecolor, or -title.
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
%  Method Usage displays the program command syntax.
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
      "-adjoin             join images into a single multi-image file",
      "-blur factor        apply a filter to blur the image",
      "-cache threshold    megabytes of memory available to the pixel cache",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    alternate image colorsapce",
      "-comment string     annotate image with comment",
      "-compose operator   composite operator",
      "-compress type      type of image compression",
      "-crop geometry      preferred size and location of the cropped image",
      "-depth value        depth of the image",
      "-density geometry   vertical and horizontal density of the image",
      "-display server     query font from this X server",
      "-dispose method     GIF disposal method",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-draw string        annotate the image with a graphic primitive",
      "-fill color         color to use when filling a graphic primitive",
      "-filter type        use this filter when resizing an image",
      "-frame geometry     surround image with an ornamental border",
      "-gamma value        level of gamma correction",
      "-geometry geometry  preferred tile and border sizes",
      "-gravity direction  which direction to gravitate towards",
      "-interlace type     None, Line, Plane, or Partition",
      "-label name         assign a label to an image",
      "-matte              store matte channel if the image has one",
      "-mode type          Frame, Unframe, or Concatenate",
      "-monochrome         transform image to black and white",
      "-page geometry      size and location of an image canvas",
      "-pointsize value    pointsize of Postscript font",
      "-quality value      JPEG/MIFF/PNG compression level",
      "-rotate degrees     apply Paeth rotation to the image",
      "-scene value        image scene number",
      "-shadow             add a shadow beneath a tile to simulate depth",
      "-size geometry      width and height of image",
      "-stroke color       color to use when stroking a graphic primitive",
      "-texture filename   name of texture to tile onto the image background",
      "-tile geometry      number of tiles per row and column",
      "-transparent color  make this color transparent within the image",
      "-treedepth value    depth of the color tree",
      "-type type          image type",
      "-verbose            print detailed information about the image",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf(
    "Usage: %.1024s [-options ...] file [ [-options ...] file ...] file\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) printf(
    "resources as command line options:  -background, -bordercolor,\n");
  (void) printf(
    "-borderwidth, -font, -mattecolor, or -title\n");
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
    *option,
    *transparent_color;

  double
    sans;

  ExceptionInfo
    exception;

  Image
    *image,
    *montage_image,
    *next_image,
    *p;

  ImageInfo
    *image_info;

  int
    x;

  MontageInfo
    *montage_info;

  QuantizeInfo
    quantize_info;

  register int
    i;

  unsigned int
    first_scene,
    last_scene,
    scene,
    status;

  /*
    Initialize command line arguments.
  */
  ReadCommandlLine(argc,&argv);
  if (LocaleCompare("montage",argv[0]) == 0)
    InitializeMagick(GetExecutionPath(argv[0]));
  else
    InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  if (argc < 3)
    Usage();
  /*
    Set defaults.
  */
  GetExceptionInfo(&exception);
  first_scene=0;
  image=(Image *) NULL;
  last_scene=0;
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strcpy(image_info->filename,argv[argc-1]);
  SetImageInfo(image_info,True,&exception);
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  GetQuantizeInfo(&quantize_info);
  quantize_info.number_colors=0;
  scene=0;
  transparent_color=(char *) NULL;
  /*
    Parse command line.
  */
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'a':
        {
          if (LocaleNCompare("adjoin",option+1,2) == 0)
            {
              image_info->adjoin=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'b':
        {
          if ((LocaleNCompare("background",option+1,5) == 0) ||
              (LocaleNCompare("bg",option+1,2) == 0))
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                  (void) QueryColorDatabase(argv[i],
                    &montage_info->background_color);
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
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
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
                    MagickError(OptionError,"Missing color",option);
                  (void) QueryColorDatabase(argv[i],&montage_info->border_color);
                  (void) QueryColorDatabase(argv[i],&image_info->border_color);
                }
              break;
            }
          if (LocaleNCompare("borderwidth",option+1,7) == 0)
            {
              montage_info->border_width=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing width",option);
                  montage_info->border_width=atoi(argv[i]);
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
          if (LocaleNCompare("colors",option+1,7) == 0)
            {
              quantize_info.number_colors=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing colors",option);
                  quantize_info.number_colors=atoi(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("colorspace",option+1,7) == 0)
            {
              quantize_info.colorspace=RGBColorspace;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  quantize_info.colorspace=UndefinedColorspace;
                  if (LocaleCompare("cmyk",option) == 0)
                    quantize_info.colorspace=CMYKColorspace;
                  if (LocaleCompare("gray",option) == 0)
                    {
                      quantize_info.colorspace=GRAYColorspace;
                      quantize_info.number_colors=256;
                      quantize_info.tree_depth=8;
                    }
                  if (LocaleCompare("ohta",option) == 0)
                    quantize_info.colorspace=OHTAColorspace;
                  if (LocaleCompare("rgb",option) == 0)
                    quantize_info.colorspace=RGBColorspace;
                  if (LocaleCompare("srgb",option) == 0)
                    quantize_info.colorspace=sRGBColorspace;
                  if (LocaleCompare("transparent",option) == 0)
                    quantize_info.colorspace=TransparentColorspace;
                  if (LocaleCompare("xyz",option) == 0)
                    quantize_info.colorspace=XYZColorspace;
                  if (LocaleCompare("ycbcr",option) == 0)
                    quantize_info.colorspace=YCbCrColorspace;
                  if (LocaleCompare("ycc",option) == 0)
                    quantize_info.colorspace=YCCColorspace;
                  if (LocaleCompare("yiq",option) == 0)
                    quantize_info.colorspace=YIQColorspace;
                  if (LocaleCompare("ypbpr",option) == 0)
                    quantize_info.colorspace=YPbPrColorspace;
                  if (LocaleCompare("yuv",option) == 0)
                    quantize_info.colorspace=YUVColorspace;
                  if (quantize_info.colorspace == UndefinedColorspace)
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
                  if (LocaleCompare("Lossless",option) == 0)
                    image_info->compression=LosslessJPEGCompression;
                  if (LocaleCompare("LZW",option) == 0)
                    image_info->compression=LZWCompression;
                  if (LocaleCompare("RLE",option) == 0)
                    image_info->compression=RunlengthEncodedCompression;
                  if (LocaleCompare("Zip",option) == 0)
                    image_info->compression=ZipCompression;
                  if (image_info->compression == UndefinedCompression)
                    MagickError(OptionError,"Invalid compression type",option);
                }
              break;
            }
          if (LocaleNCompare("compose",option+1,5) == 0)
            {
              montage_info->compose=CopyCompositeOp;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  montage_info->compose=UndefinedCompositeOp;
                  if (LocaleCompare("Over",option) == 0)
                    montage_info->compose=OverCompositeOp;
                  if (LocaleCompare("In",option) == 0)
                    montage_info->compose=InCompositeOp;
                  if (LocaleCompare("Out",option) == 0)
                    montage_info->compose=OutCompositeOp;
                  if (LocaleCompare("Atop",option) == 0)
                    montage_info->compose=AtopCompositeOp;
                  if (LocaleCompare("Xor",option) == 0)
                    montage_info->compose=XorCompositeOp;
                  if (LocaleCompare("Plus",option) == 0)
                    montage_info->compose=PlusCompositeOp;
                  if (LocaleCompare("Minus",option) == 0)
                    montage_info->compose=MinusCompositeOp;
                  if (LocaleCompare("Add",option) == 0)
                    montage_info->compose=AddCompositeOp;
                  if (LocaleCompare("Subtract",option) == 0)
                    montage_info->compose=SubtractCompositeOp;
                  if (LocaleCompare("Difference",option) == 0)
                    montage_info->compose=DifferenceCompositeOp;
                  if (LocaleCompare("Multiply",option) == 0)
                    montage_info->compose=MultiplyCompositeOp;
                  if (LocaleCompare("Bumpmap",option) == 0)
                    montage_info->compose=BumpmapCompositeOp;
                  if (LocaleCompare("Copy",option) == 0)
                    montage_info->compose=CopyCompositeOp;
                  if (LocaleCompare("CopyRed",option) == 0)
                    montage_info->compose=CopyRedCompositeOp;
                  if (LocaleCompare("CopyGreen",option) == 0)
                    montage_info->compose=CopyGreenCompositeOp;
                  if (LocaleCompare("CopyBlue",option) == 0)
                    montage_info->compose=CopyBlueCompositeOp;
                  if (LocaleCompare("CopyOpacity",option) == 0)
                    montage_info->compose=CopyOpacityCompositeOp;
                  if (montage_info->compose == UndefinedCompositeOp)
                    MagickError(OptionError,"Invalid compose type",option);
                }
              break;
            }
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
          if (LocaleCompare("display",option+1) == 0)
            {
              CloneString(&image_info->server_name,(char *) NULL);
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
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing method",option);
                }
              break;
            }
          if (LocaleNCompare("dither",option+1,3) == 0)
            {
              quantize_info.dither=(*option == '-');
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
        case 'f':
        {
          if (LocaleCompare("fill",option+1) == 0)
            {
              (void) QueryColorDatabase("none",&montage_info->fill);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing fill color",option);
                  (void) QueryColorDatabase(argv[i],&montage_info->fill);
                }
              break;
            }
          if (LocaleNCompare("filter",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  FilterTypes
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
          if (LocaleNCompare("font",option+1,3) == 0)
            {
              CloneString(&image_info->font,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing font name",option);
                  (void) CloneString(&image_info->font,argv[i]);
                  (void) CloneString(&montage_info->font,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("frame",option+1,2) == 0)
            {
              CloneString(&montage_info->frame,(char *) NULL);
              argv[i]=(char *) "+sans";
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&montage_info->frame,argv[i]);
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
          if (LocaleNCompare("geometry",option+1,2) == 0)
            {
              CloneString(&montage_info->geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&montage_info->geometry,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("gravity",option+1,2) == 0)
            {
              montage_info->gravity=SouthGravity;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  if (LocaleCompare("Forget",option) == 0)
                    montage_info->gravity=ForgetGravity;
                  if (LocaleCompare("NorthWest",option) == 0)
                    montage_info->gravity=NorthWestGravity;
                  if (LocaleCompare("North",option) == 0)
                    montage_info->gravity=NorthGravity;
                  if (LocaleCompare("NorthEast",option) == 0)
                    montage_info->gravity=NorthEastGravity;
                  if (LocaleCompare("West",option) == 0)
                    montage_info->gravity=WestGravity;
                  if (LocaleCompare("Center",option) == 0)
                    montage_info->gravity=CenterGravity;
                  if (LocaleCompare("East",option) == 0)
                    montage_info->gravity=EastGravity;
                  if (LocaleCompare("SouthWest",option) == 0)
                    montage_info->gravity=SouthWestGravity;
                  if (LocaleCompare("South",option) == 0)
                    montage_info->gravity=SouthGravity;
                  if (LocaleCompare("SouthEast",option) == 0)
                    montage_info->gravity=SouthEastGravity;
                  if (LocaleCompare("Static",option) == 0)
                    montage_info->gravity=StaticGravity;
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
        case 'i':
        {
          if (LocaleNCompare("interlace",option+1,5) == 0)
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
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'm':
        {
          if (LocaleCompare("matte",option+1) == 0)
            break;
          if (LocaleNCompare("mattecolor",option+1,6) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                  (void) QueryColorDatabase(argv[i],&montage_info->matte_color);
                  (void) QueryColorDatabase(argv[i],&image_info->matte_color);
                }
              break;
            }
          if (LocaleNCompare("mode",option+1,4) == 0)
            {
              if (*option == '-')
                {
                  MontageMode
                    mode;

                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  mode=UndefinedMode;
                  if (LocaleCompare("frame",option) == 0)
                    {
                      mode=FrameMode;
                      (void) CloneString(&montage_info->frame,"15x15+3+3");
                      montage_info->shadow=True;
                      break;
                    }
                  if (LocaleCompare("unframe",option) == 0)
                    {
                      mode=UnframeMode;
                      montage_info->frame=(char *) NULL;
                      montage_info->shadow=False;
                      montage_info->border_width=0;
                      break;
                    }
                  if (LocaleCompare("concatenate",option) == 0)
                    {
                      mode=ConcatenateMode;
                      montage_info->frame=(char *) NULL;
                      montage_info->shadow=False;
                      (void) CloneString(&montage_info->geometry,"+0+0");
                      montage_info->border_width=0;
                      break;
                    }
                  if (mode == UndefinedMode)
                    MagickError(OptionError,"Invalid mode type",option);
                }
              break;
            }
          if (LocaleNCompare("monochrome",option+1,3) == 0)
            {
              image_info->monochrome=(*option == '-');
              if (image_info->monochrome)
                {
                  quantize_info.number_colors=2;
                  quantize_info.tree_depth=8;
                  quantize_info.colorspace=GRAYColorspace;
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
              CloneString(&image_info->page,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing page geometry",option);
                  image_info->page=PostscriptGeometry(argv[i]);
                }
              break;
            }
          if (LocaleCompare("pen",option+1) == 0)
            {
              (void) QueryColorDatabase("none",&image_info->pen);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing pen color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->pen);
                }
              break;
            }
          if (LocaleNCompare("pointsize",option+1,2) == 0)
            {
              image_info->pointsize=12;
              montage_info->pointsize=12;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing size",option);
                  image_info->pointsize=atof(argv[i]);
                  montage_info->pointsize=atof(argv[i]);
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
          if (LocaleNCompare("scene",option+1,3) == 0)
            {
              first_scene=0;
              last_scene=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing scene number",option);
                  first_scene=atoi(argv[i]);
                  last_scene=first_scene;
                  (void) sscanf(argv[i],"%u-%u",&first_scene,&last_scene);
                }
              break;
            }
          if (LocaleNCompare("shadow",option+1,4) == 0)
            {
              montage_info->shadow=(*option == '-');
              break;
            }
          if (LocaleNCompare("sharpen",option+1,5) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
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
          if (LocaleCompare("stroke",option+1) == 0)
            {
              (void) QueryColorDatabase("none",&montage_info->stroke);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                  (void) QueryColorDatabase(argv[i],&montage_info->stroke);
                }
              break;
            }
          if (LocaleNCompare("strokewidth",option+1,6) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing size",option);
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
              CloneString(&montage_info->texture,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing filename",option);
                  (void) CloneString(&montage_info->texture,argv[i]);
                }
              break;
            }
          if (LocaleCompare("tile",option+1) == 0)
            {
              CloneString(&montage_info->tile,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&montage_info->tile,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("title",option+1,4) == 0)
            {
              CloneString(&montage_info->title,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing title",option);
                  (void) CloneString(&montage_info->title,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("transparent",option+1,7) == 0)
            {
              transparent_color=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                  (void) CloneString(&transparent_color,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("treedepth",option+1,3) == 0)
            {
              quantize_info.tree_depth=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing depth",option);
                  quantize_info.tree_depth=atoi(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("type",option+1,4) == 0)
            {
              if (*option == '-')
                {
                  ImageType
                    image_type;

                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_type=UndefinedType;
                  if (LocaleCompare("Bilevel",option) == 0)
                    image_type=BilevelType;
                  if (LocaleCompare("Grayscale",option) == 0)
                    image_type=GrayscaleType;
                  if (LocaleCompare("GrayscaleMatte",option) == 0)
                    image_type=GrayscaleMatteType;
                  if (LocaleCompare("Palette",option) == 0)
                    image_type=PaletteType;
                  if (LocaleCompare("PaletteMatte",option) == 0)
                    image_type=PaletteMatteType;
                  if (LocaleCompare("TrueColor",option) == 0)
                    image_type=TrueColorType;
                  if (LocaleCompare("TrueColorMatte",option) == 0)
                    image_type=TrueColorMatteType;
                  if (LocaleCompare("ColorSeparation",option) == 0)
                    image_type=ColorSeparationType;
                  if (LocaleCompare("ColorSeparationMatte",option) == 0)
                    image_type=ColorSeparationMatteType;
                  if (image_type == UndefinedType)
                    MagickError(OptionError,"Invalid image type",option);
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
    else
      for (scene=first_scene; scene <= last_scene ; scene++)
      {
        /*
          Option is a file name: begin by reading image from specified file.
        */
        (void) strcpy(image_info->filename,argv[i]);
        if (first_scene != last_scene)
          {
            char
              filename[MaxTextExtent];

            /*
              Form filename for multi-part images.
            */
            FormatString(filename,image_info->filename,scene);
            if (LocaleCompare(filename,image_info->filename) == 0)
              FormatString(filename,"%.1024s.%u",image_info->filename,scene);
            (void) strcpy(image_info->filename,filename);
          }
        CloneString(&image_info->font,montage_info->font);
        image_info->colorspace=quantize_info.colorspace;
        image_info->dither=quantize_info.dither;
        if (image_info->size == (char *) NULL)
          CloneString(&image_info->size,montage_info->geometry);
        next_image=ReadImage(image_info,&exception);
        if (exception.severity != UndefinedException)
          MagickWarning(exception.severity,exception.reason,
            exception.description);
        if (next_image == (Image *) NULL)
          continue;
        status=MogrifyImages(image_info,i,argv,&next_image);
        if (status == False)
          CatchImageException(next_image);
        if (image == (Image *) NULL)
          image=next_image;
        else
          {
            /*
              Link image into image list.
            */
            for (p=image; p->next != (Image *) NULL; p=p->next);
            next_image->previous=p;
            p->next=next_image;
          }
      }
  }
  if ((i != (argc-1)) || (image == (Image *) NULL))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  /*
    Create composite image.
  */
  (void) strcpy(montage_info->filename,argv[argc-1]);
  montage_image=MontageImages(image,montage_info,&exception);
  if (montage_image == (Image *) NULL)
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  DestroyImages(image);
  /*
    Write image.
  */
  (void) strcpy(image_info->filename,argv[argc-1]);
  for (p=montage_image; p != (Image *) NULL; p=p->next)
  {
    if (transparent_color != (char *) NULL)
      {
        PixelPacket
          target;

        target=GetOnePixel(p,0,0);
        (void) QueryColorDatabase(transparent_color,&target);
        TransparentImage(p,target,TransparentOpacity);
      }
    if (quantize_info.number_colors != 0)
      {
        /*
          Reduce the number of colors in the image.
        */
        if ((p->storage_class == DirectClass) ||
            (p->colors > quantize_info.number_colors) ||
            (quantize_info.colorspace == GRAYColorspace))
          (void) QuantizeImage(&quantize_info,p);
        if (image_info->verbose)
          (void) QuantizationError(p);
        SyncImage(p);
      }
    (void) strcpy(p->filename,argv[argc-1]);
  }
  SetImageInfo(image_info,True,&image->exception);
  for (p=montage_image; p != (Image *) NULL; p=p->next)
  {
    status=WriteImage(image_info,p);
    if (status == False)
      CatchImageException(p);
    if (image_info->adjoin)
      break;
  }
  (void) strcpy(montage_image->magick_filename,argv[argc-1]);
  if (image_info->verbose)
    DescribeImage(montage_image,stderr,False);
  DestroyImages(montage_image);
  DestroyMontageInfo(montage_info);
  DestroyImageInfo(image_info);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(0);
  return(False);
}
