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
%  Montage creates a composite by combining several separate images. The
%  images are tiled on the composite image with the name of the image
%  optionally appearing just below the individual tile.
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
%   M o n t a g e U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MontageUsage() displays the program command syntax.
%
%  The format of the MontageUsage method is:
%
%      void MontageUsage()
%
%
*/
static void MontageUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-adjoin              join images into a single multi-image file",
      "-blur factor         apply a filter to blur the image",
      "-cache threshold     megabytes of memory available to the pixel cache",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorsapce",
      "-comment string      annotate image with comment",
      "-compose operator    composite operator",
      "-compress type       type of image compression",
      "-crop geometry       preferred size and location of the cropped image",
      "-debug               display copious debugging information",
      "-density geometry    vertical and horizontal density of the image",
      "-depth value         depth of the image",
      "-display server      query font from this X server",
      "-dispose method      GIF disposal method",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-draw string         annotate the image with a graphic primitive",
      "-fill color          color to use when filling a graphic primitive",
      "-filter type         use this filter when resizing an image",
      "-frame geometry      surround image with an ornamental border",
      "-gamma value         level of gamma correction",
      "-geometry geometry   preferred tile and border sizes",
      "-gravity direction   which direction to gravitate towards",
      "-interlace type      None, Line, Plane, or Partition",
      "-help                print program options",
      "-label name          assign a label to an image",
      "-matte               store matte channel if the image has one",
      "-mode type           Frame, Unframe, or Concatenate",
      "-monochrome          transform image to black and white",
      "-noop                do not apply options to image"
      "-page geometry       size and location of an image canvas",
      "-pointsize value     pointsize of Postscript font",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-resize geometry     resize the image",
      "-rotate degrees      apply Paeth rotation to the image",
      "-scenes range        image scene range",
      "-shadow              add a shadow beneath a tile to simulate depth",
      "-size geometry       width and height of image",
      "-stroke color        color to use when stroking a graphic primitive",
      "-texture filename    name of texture to tile onto the image background",
      "-tile geometry       number of tiles per row and column",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     depth of the color tree",
      "-trim                trim image edges",
      "-type type           image type",
      "-verbose             print detailed information about the image",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] file ...]\n",
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
%    M o n t a g e U t i l i t y                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MontageUtility() reads one or more images, applies one or more image
%  processing operations, and writes out the image in the same or
%  differing format.
%
%  The format of the MontageUtility method is:
%
%      unsigned int MontageUtility(const int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%
*/
static unsigned int MontageUtility(int argc,char **argv)
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
    *image_list,
    *montage_image,
    *next_image;

  ImageInfo
    *image_info;

  long
    first_scene,
    j,
    k,
    last_scene,
    scene,
    x;

  MontageInfo
    *montage_info;

  QuantizeInfo
    quantize_info;

  register Image
    *p;

  register long
    i;

  unsigned int
    status;

  /*
    Set defaults.
  */
  GetExceptionInfo(&exception);
  first_scene=0;
  image=(Image *) NULL;
  image_list=(Image *) NULL;
  last_scene=0;
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  (void) SetImageInfo(image_info,True,&exception);
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  GetQuantizeInfo(&quantize_info);
  quantize_info.number_colors=0;
  scene=0;
  status=True;
  transparent_color=(char *) NULL;
  /*
    Parse command line.
  */
  j=1;
  k=0;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        k=i;
        for (scene=first_scene; scene <= last_scene ; scene++)
        {
          /*
            Option is a file name: begin by reading image from specified file.
          */
          (void) strncpy(image_info->filename,argv[i],MaxTextExtent-1);
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
              (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
            }
          (void) CloneString(&image_info->font,montage_info->font);
          image_info->colorspace=quantize_info.colorspace;
          image_info->dither=quantize_info.dither;
          if (image_info->size == (char *) NULL)
            (void) CloneString(&image_info->size,montage_info->geometry);
          next_image=ReadImage(image_info,&exception);
          if (exception.severity != UndefinedException)
            MagickWarning(exception.severity,exception.reason,
              exception.description);
          status&=next_image != (Image *) NULL;
          if (next_image == (Image *) NULL)
            continue;
          if (image == (Image *) NULL)
            {
              image=next_image;
              continue;
            }
          /*
            Link image into image list.
          */
          for (p=image; p->next != (Image *) NULL; p=p->next);
          next_image->previous=p;
          p->next=next_image;
        }
        continue;
      }
    if (j != (k+1))
      {
        status&=MogrifyImages(image_info,i-j,argv+j,&image);
        (void) CatchImageException(image);
        PushImageList(&image_list,image,&exception);
        DestroyImageList(image);
        image=(Image *) NULL;
        j=k+1;
      }
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("adjoin",option+1) == 0)
          {
            image_info->adjoin=(*option == '-');
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
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
        if (LocaleCompare("blur",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
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
        if (LocaleCompare("borderwidth",option+1) == 0)
          {
            montage_info->border_width=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing width",option);
                montage_info->border_width=atol(argv[i]);
              }
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
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
        if (LocaleCompare("colors",option+1) == 0)
          {
            quantize_info.number_colors=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing colors",option);
                quantize_info.number_colors=atol(argv[i]);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
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
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickError(OptionError,"Missing comment",option);
              }
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
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
        if (LocaleCompare("compose",option+1) == 0)
          {
            CompositeOperator
                compose;

            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickError(OptionError,"Missing type",option);
                option=argv[i];
                compose=UndefinedCompositeOp;
                if (LocaleCompare("Over",option) == 0)
                  compose=OverCompositeOp;
                if (LocaleCompare("In",option) == 0)
                  compose=InCompositeOp;
                if (LocaleCompare("Out",option) == 0)
                  compose=OutCompositeOp;
                if (LocaleCompare("Atop",option) == 0)
                  compose=AtopCompositeOp;
                if (LocaleCompare("Xor",option) == 0)
                  compose=XorCompositeOp;
                if (LocaleCompare("Plus",option) == 0)
                  compose=PlusCompositeOp;
                if (LocaleCompare("Minus",option) == 0)
                  compose=MinusCompositeOp;
                if (LocaleCompare("Add",option) == 0)
                  compose=AddCompositeOp;
                if (LocaleCompare("Subtract",option) == 0)
                  compose=SubtractCompositeOp;
                if (LocaleCompare("Difference",option) == 0)
                  compose=DifferenceCompositeOp;
                if (LocaleCompare("Multiply",option) == 0)
                  compose=MultiplyCompositeOp;
                if (LocaleCompare("Bumpmap",option) == 0)
                  compose=BumpmapCompositeOp;
                if (LocaleCompare("Copy",option) == 0)
                  compose=CopyCompositeOp;
                if (LocaleCompare("CopyRed",option) == 0)
                  compose=CopyRedCompositeOp;
                if (LocaleCompare("CopyGreen",option) == 0)
                  compose=CopyGreenCompositeOp;
                if (LocaleCompare("CopyBlue",option) == 0)
                  compose=CopyBlueCompositeOp;
                if (LocaleCompare("CopyOpacity",option) == 0)
                  compose=CopyOpacityCompositeOp;
                if (compose == UndefinedCompositeOp)
                  MagickError(OptionError,"Invalid compose type",option);
              }
            break;
          }
        if (LocaleCompare("crop",option+1) == 0)
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
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickError(OptionError,"Missing server name",option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing method",option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            quantize_info.dither=(*option == '-');
            break;
          }
        if (LocaleCompare("draw",option+1) == 0)
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
            (void) QueryColorDatabase("none",&image_info->pen);
            (void) QueryColorDatabase("none",&montage_info->fill);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickError(OptionError,"Missing fill color",option);
                (void) QueryColorDatabase(argv[i],&image_info->pen);
                (void) QueryColorDatabase(argv[i],&montage_info->fill);
              }
            break;
          }
        if (LocaleCompare("filter",option+1) == 0)
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
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
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
        if (LocaleCompare("frame",option+1) == 0)
          {
            (void) CloneString(&montage_info->frame,(char *) NULL);
            (void) strcpy(argv[i]+1,"sans");
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
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              MagickError(OptionError,"Missing value",option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            (void) CloneString(&montage_info->geometry,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickError(OptionError,"Missing geometry",option);
                (void) CloneString(&montage_info->geometry,argv[i]);
              }
            break;
          }
        if (LocaleCompare("gravity",option+1) == 0)
          {
            GravityType
              gravity;

            gravity=ForgetGravity;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickError(OptionError,"Missing type",option);
                option=argv[i];
                if (LocaleCompare("Forget",option) == 0)
                  gravity=ForgetGravity;
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
                if (LocaleCompare("Static",option) == 0)
                  gravity=StaticGravity;
              }
            montage_info->gravity=gravity;
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          {
            MontageUsage();
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 'i':
      {
        if (LocaleCompare("interlace",option+1) == 0)
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
        if (LocaleCompare("label",option+1) == 0)
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
        if (LocaleCompare("mattecolor",option+1) == 0)
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
        if (LocaleCompare("mode",option+1) == 0)
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
        if (LocaleCompare("monochrome",option+1) == 0)
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
      case 'n':
      {
        if (LocaleCompare("noop",option+1) == 0)
          break;
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            (void) CloneString(&image_info->page,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickError(OptionError,"Missing page geometry",option);
                image_info->page=GetPageGeometry(argv[i]);
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
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            image_info->pointsize=12;
            montage_info->pointsize=12;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
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
        if (LocaleCompare("quality",option+1) == 0)
          {
            image_info->quality=75;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing quality",option);
                image_info->quality=atol(argv[i]);
              }
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 'r':
      {
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickError(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              MagickError(OptionError,"Missing degrees",option);
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 's':
      {
        if (LocaleCompare("scenes",option+1) == 0)
          {
            first_scene=0;
            last_scene=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing scene number",option);
                first_scene=atol(argv[i]);
                last_scene=first_scene;
                (void) sscanf(argv[i],"%ld-%ld",&first_scene,&last_scene);
              }
            break;
          }
        if (LocaleCompare("shadow",option+1) == 0)
          {
            montage_info->shadow=(*option == '-');
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing geometry",option);
              }
            break;
          }
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
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing size",option);
              }
            break;
          }
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
      case 't':
      {
        if (LocaleCompare("texture",option+1) == 0)
          {
            (void) CloneString(&montage_info->texture,(char *) NULL);
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
            (void) CloneString(&montage_info->tile,(char *) NULL);
            (void) strcpy(argv[i]+1,"sans");
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickError(OptionError,"Missing geometry",option);
                (void) CloneString(&montage_info->tile,argv[i]);
              }
            break;
          }
        if (LocaleCompare("title",option+1) == 0)
          {
            (void) CloneString(&montage_info->title,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickError(OptionError,"Missing title",option);
                (void) CloneString(&montage_info->title,argv[i]);
              }
            break;
          }
        if (LocaleCompare("transparent",option+1) == 0)
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
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info.tree_depth=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickError(OptionError,"Missing depth",option);
                quantize_info.tree_depth=atoi(argv[i]);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          break;
        if (LocaleCompare("type",option+1) == 0)
          {
            image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickError(OptionError,"Missing type",option);
                option=argv[i];
                if (LocaleCompare("Bilevel",option) == 0)
                  image_info->type=BilevelType;
                if (LocaleCompare("Grayscale",option) == 0)
                  image_info->type=GrayscaleType;
                if (LocaleCompare("GrayscaleMatte",option) == 0)
                  image_info->type=GrayscaleMatteType;
                if (LocaleCompare("Palette",option) == 0)
                  image_info->type=PaletteType;
                if (LocaleCompare("PaletteMatte",option) == 0)
                  image_info->type=PaletteMatteType;
                if (LocaleCompare("TrueColor",option) == 0)
                  image_info->type=TrueColorType;
                if (LocaleCompare("TrueColorMatte",option) == 0)
                  image_info->type=TrueColorMatteType;
                if (LocaleCompare("ColorSeparation",option) == 0)
                  image_info->type=ColorSeparationType;
                if (LocaleCompare("ColorSeparationMatte",option) == 0)
                  image_info->type=ColorSeparationMatteType;
                if (LocaleCompare("Optimize",option) == 0)
                  image_info->type=OptimizeType;
                if (image_info->type == UndefinedType)
                  MagickError(OptionError,"Invalid image type",option);
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
        MontageUsage();
        break;
      }
      default:
      {
        MagickError(OptionError,"Unrecognized option",option);
        break;
      }
    }
  }
  if (i != (argc-1))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  if (image == (Image *) NULL)
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image_list);
      (void) CatchImageException(image_list);
    }
  else
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image);
      (void) CatchImageException(image);
      PushImageList(&image_list,image,&exception);
      DestroyImageList(image);
    }
  (void) strncpy(montage_info->filename,argv[argc-1],MaxTextExtent-1);
  montage_image=MontageImages(image_list,montage_info,&exception);
  if (montage_image == (Image *) NULL)
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  DestroyImageList(image_list);
  /*
    Write image.
  */
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  for (p=montage_image; p != (Image *) NULL; p=p->next)
  {
    if (transparent_color != (char *) NULL)
      {
        PixelPacket
          target;

        target=GetOnePixel(p,0,0);
        (void) QueryColorDatabase(transparent_color,&target);
        (void) TransparentImage(p,target,TransparentOpacity);
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
    (void) strncpy(p->filename,argv[argc-1],MaxTextExtent-1);
  }
  (void) strncpy(montage_image->magick_filename,argv[argc-1],MaxTextExtent-1);
  status&=WriteImages(image_info,montage_image,argv[argc-1],&image->exception);
  DestroyImageList(montage_image);
  DestroyMontageInfo(montage_info);
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
  if (argc < 3)
    MontageUsage();
  status=MontageUtility(argc,argv);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(!status);
  return(False);
}
#endif
