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
%  The composite program syntax is:
%
%  Usage: composite [options ...] image composite [mask] composite
%
%  Where options include:
%    -cache threshold    number of megabytes available to the pixel cache
%    -colors value       preferred number of colors in the image
%    -compose operator   composite operator
%    -colorspace type    alternate image colorspace
%    -comment string     annotate image with comment
%    -compress type      type of image compression
%    -density geometry   vertical and horizontal density of the image
%    -depth value        depth of the image
%    -displace geometry  shift image pixels as defined by a displacement map
%    -display server     obtain image or font from this X server
%    -dispose method     GIF disposal method
%    -dissolve value     dissolve the two images a given percent
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -font name          font for rendering text
%    -geometry geometry  location of the composite image
%    -gravity type       which direction to gravitate towards
%    -interlace type     None, Line, Plane, or Partition
%    -label name         assign a label to an image
%    -matte              store matte channel if the image has one
%    -monochrome         transform image to black and white
%    -negate             replace every pixel with its complementary color 
%    -page geometry      size and location of an image canvas
%    -quality value      JPEG/MIFF/PNG compression level
%    -rotate degrees     apply Paeth rotation to the image
%    -scene value        image scene number
%    -size geometry      width and height of image
%    -stegano offset     hide watermark within an image
%    -stereo             combine two image to create a stereo anaglyph
%    -tile               repeat composite operation across image
%    -treedepth value    depth of the color tree
%    -type type          image type
%    -verbose            print detailed information about the image
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
static void Usage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-cache threshold    number of megabytes available to the pixel cache",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    alternate image colorspace",
      "-comment string     annotate image with comment",
      "-compose operator   composite operator",
      "-compress type      type of image compression",
      "-density geometry   vertical and horizontal density of the image",
      "-depth value        depth of the image",
      "-displace geometry  shift image pixels as defined by a displacement map",
      "-display server     obtain image or font from this X server",
      "-dispose method     GIF disposal method",
      "-dissolve value     dissolve the two images a given percent",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-font name          font for rendering text",
      "-geometry geometry  location of the composite image",
      "-gravity type       which direction to gravitate towards",
      "-interlace type     None, Line, Plane, or Partition",
      "-label name         ssign a label to an image",
      "-matte              store matte channel if the image has one",
      "-monochrome         transform image to black and white",
      "-negate             replace every pixel with its complementary color ",
      "-page geometry      size and location of an image canvas",
      "-quality value      JPEG/MIFF/PNG compression level",
      "-rotate degrees     apply Paeth rotation to the image",
      "-scene value        image scene number",
      "-size geometry      width and height of image",
      "-stegano offset     hide watermark within an image",
      "-stereo             combine two image to create a stereo anaglyph",
      "-tile               repeat composite operation across image",
      "-treedepth value    depth of the color tree",
      "-type type          image type",
      "-verbose            print detailed information about the image",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [options ...] image composite [mask] composite\n",
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
#define NotInitialized  (unsigned int) (~0)

  char
    *displace_geometry,
    *filename,
    *geometry,
    *option,
    *unsharp_geometry,
    *watermark_geometry,
    *write_filename;

  CompositeOperator
    compose;

  double
    dissolve,
    sans;

  ExceptionInfo
    exception;

  Image
    *combine_image,
    *composite_image,
    *image,
    *mask_image;

  ImageInfo
    *image_info;

  int
    gravity;

  long
    x,
    y;

  register int
    i;

  unsigned int
    doexit,
    matte,
    status,
    stegano,
    stereo,
    tile;

  ImageAttribute
    *attribute;

  /*
    Initialize command line arguments.
  */
  if (LocaleCompare("-composite",argv[0]) == 0)
    {
      doexit=False;
      if (argc < 4)
        return(False);
    }
  else
    {
      doexit=True;
      ReadCommandlLine(argc,&argv);
      if (LocaleNCompare("composite",argv[0],9) == 0)
        InitializeMagick(GetExecutionPath(argv[0]));
      else
        InitializeMagick(*argv);
      status=ExpandFilenames(&argc,&argv);
      if (status == False)
        MagickError(ResourceLimitError,"Memory allocation failed",
          (char *) NULL);
      if (argc < 4)
        Usage();
    }
  /*
    Set default.
  */
  dissolve=0.0;
  compose=CopyCompositeOp;
  composite_image=(Image *) NULL;
  displace_geometry=(char *) NULL;
  GetExceptionInfo(&exception);
  geometry=(char *) NULL;
  gravity=NorthWestGravity;
  image=(Image *) NULL;
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  (void) SetImageInfo(image_info,True,&exception);
  mask_image=(Image *) NULL;
  stegano=0;
  stereo=False;
  tile=False;
  watermark_geometry=(char *) NULL;
  unsharp_geometry=(char *) NULL;
  write_filename=argv[argc-1];
  /*
    Check command syntax.
  */
  filename=(char *) NULL;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) || ((*option != '-') && (*option != '+')))
      {
        /*
          Read input images.
        */
        filename=argv[i];
        (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
        if (composite_image == (Image *) NULL)
          {
            composite_image=ReadImage(image_info,&exception);
            if (exception.severity != UndefinedException)
              MagickWarning(exception.severity,exception.reason,
                exception.description);
            continue;
          }
        if (mask_image != (Image *) NULL)
          MagickError(OptionError,"input images already specified",filename);
        if (image == (Image *) NULL)
          {
            image=ReadImage(image_info,&exception);
            if (exception.severity != UndefinedException)
              MagickWarning(exception.severity,exception.reason,
                exception.description);
            continue;
          }
        mask_image=ReadImage(image_info,&exception);
        if (mask_image == (Image *) NULL)
          MagickError(exception.severity,exception.reason,
            exception.description);
        if (exception.severity != UndefinedException)
          MagickWarning(exception.severity,exception.reason,
            exception.description);
      }
    else
      switch(*(option+1))
      {
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
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing colors",option);
                }
              break;
            }
          if (LocaleNCompare("colorspace",option+1,7) == 0)
            {
              image_info->colorspace=RGBColorspace;
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
          if (LocaleNCompare("compose",option+1,5) == 0)
            {
              compose=CopyCompositeOp;
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
                  if (LocaleCompare("Clear",option) == 0)
                    compose=ClearCompositeOp;
                  if (compose == UndefinedCompositeOp)
                    MagickError(OptionError,"Invalid compose type",option);
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
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'd':
        {
          if (LocaleNCompare("density",option+1,3) == 0)
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
          if (LocaleNCompare("depth",option+1,3) == 0)
            {
              image_info->depth=QuantumDepth;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing image depth",option);
                  image_info->depth=atoi(argv[i]);
                }
              break;
            }
          if (LocaleCompare("displace",option+1) == 0)
            {
              (void) CloneString(&displace_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&displace_geometry,argv[i]);
                  compose=DisplaceCompositeOp;
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
          if (LocaleNCompare("dispose",option+1,5) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing method",option);
                }
              break;
            }
          if (LocaleNCompare("dissolve",option+1,3) == 0)
            {
              dissolve=0.0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing value",option);
                  dissolve=atof(argv[i]);
                  compose=DissolveCompositeOp;
                }
              break;
            }
          if (LocaleNCompare("dither",option+1,3) == 0)
            {
              image_info->dither=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'f':
        {
          (void) CloneString(&image_info->font,(char *) NULL);
          if (*option == '-')
            {
              i++;
              if (i == argc)
                MagickError(OptionError,"Missing font name",option);
              (void) CloneString(&image_info->font,argv[i]);
            }
          break;
        }
        case 'g':
        {
          if (LocaleNCompare("geometry",option+1,2) == 0)
            {
              (void) CloneString(&geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&geometry,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("gravity",option+1,2) == 0)
            {
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
          if (LocaleNCompare("matte",option+1,5) == 0)
            break;
          if (LocaleNCompare("monochrome",option+1,2) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'n':
        {
          if (LocaleNCompare("negate",option+1,3) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'p':
        {
          if (LocaleNCompare("page",option+1,3) == 0)
            {
              (void) CloneString(&image_info->page,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing page geometry",option);
                  image_info->page=PostscriptGeometry(argv[i]);
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
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
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
          if (LocaleNCompare("scene",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing scene number",option);
                }
              break;
            }
          if (LocaleNCompare("size",option+1,2) == 0)
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
          if (LocaleNCompare("stegano",option+1,4) == 0)
            {
              stegano=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing offset",option);
                  stegano=atoi(argv[i])+1;
                }
              break;
            }
          if (LocaleNCompare("stereo",option+1,4) == 0)
            {
              stereo=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 't':
        {
          if (LocaleNCompare("tile",option+1,2) == 0)
            {
              tile=(*option == '-');
              break;
            }
          if (LocaleNCompare("treedepth",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing depth",option);
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
        case 'u':
        {
          if (LocaleCompare("unsharp",option+1) == 0)
            {
              (void) CloneString(&unsharp_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&unsharp_geometry,argv[i]);
                  compose=ThresholdCompositeOp;
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
        case 'w':
        {
          if (LocaleCompare("watermark",option+1) == 0)
            {
              (void) CloneString(&watermark_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&watermark_geometry,argv[i]);
                  compose=ModulateCompositeOp;
                }
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
  }
  if ((image == (Image *) NULL) || (composite_image == (Image *) NULL))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  if (i != (argc-1))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  if (mask_image != (Image *) NULL)
    SetImageClipMask(image,mask_image);
  if (compose == DissolveCompositeOp)
    {
      register PixelPacket
        *q;

      /*
        Create mattes for dissolve.
      */
      for (y=0; y < (int) composite_image->rows; y++)
      {
        q=GetImagePixels(composite_image,0,y,composite_image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) composite_image->columns; x++)
        {
          if (composite_image->matte)
            q->opacity=(Quantum) (((MaxRGB-q->opacity)*dissolve)/100);
          else
            q->opacity=(Quantum) ((MaxRGB*dissolve)/100);
          q++;
        }
        if (!SyncImagePixels(composite_image))
          break;
      }
      composite_image->storage_class=DirectClass;
      composite_image->matte=True;
    }
  if (compose == DisplaceCompositeOp)
    (void) CloneString(&composite_image->geometry,displace_geometry);
  if (compose == ModulateCompositeOp)
    (void) CloneString(&composite_image->geometry,watermark_geometry);
  if (compose == ThresholdCompositeOp)
    (void) CloneString(&composite_image->geometry,unsharp_geometry);
  /*
    Combine image.
  */
  matte=image->matte;
  if (stegano != 0)
    {
      image->offset=stegano-1;
      combine_image=SteganoImage(image,composite_image,&exception);
    }
  else
    if (stereo)
      combine_image=StereoImage(image,composite_image,&exception);
    else
      if (tile)
        {
          /*
            Tile the composite image.
          */
          for (y=0; y < (int) image->rows; y+=composite_image->rows)
            for (x=0; x < (int) image->columns; x+=composite_image->columns)
            {
              status=CompositeImage(image,compose,composite_image,x,y);
              if (status == False)
                CatchImageException(image);
            }
          combine_image=image;
        }
      else
        {
          int
            flags;

          unsigned long
            height,
            width;

          /*
            Digitally composite image.
          */
          width=image->columns;
          height=image->rows;
          x=0;
          y=0;
          flags=ParseGeometry(geometry,&x,&y,&width,&height);
          if ((flags & XNegative) != 0)
            x+=image->columns;
          if ((flags & WidthValue) == 0)
            width-=2*x > (int) width ? width : 2*x;
          if ((flags & YNegative) != 0)
            y+=image->rows;
          if ((flags & HeightValue) == 0)
            height-=2*y > (int) height ? height : 2*y;
          switch (gravity)
          {
            case NorthWestGravity:
              break;
            case NorthGravity:
            {
              x+=(int) (0.5*width-composite_image->columns/2);
              break;
            }
            case NorthEastGravity:
            {
              x+=width-composite_image->columns;
              break;
            }
            case WestGravity:
            {
              y+=(int) (0.5*height-composite_image->rows/2);
              break;
            }
            case ForgetGravity:
            case StaticGravity:
            case CenterGravity:
            default:
            {
              x+=(int) (0.5*width-composite_image->columns/2);
              y+=(int) (0.5*height-composite_image->rows/2);
              break;
            }
            case EastGravity:
            {
              x+=width-composite_image->columns;
              y+=(int) (0.5*height-composite_image->rows/2);
              break;
            }
            case SouthWestGravity:
            {
              y+=height-composite_image->rows;
              break;
            }
            case SouthGravity:
            {
              x+=(int) (0.5*width-composite_image->columns/2);
              y+=height-composite_image->rows;
              break;
            }
            case SouthEastGravity:
            {
              x+=width-composite_image->columns;
              y+=height-composite_image->rows;
              break;
            }
          }
          status=CompositeImage(image,compose,composite_image,x,y);
          if (status == False)
            CatchImageException(image);
          combine_image=image;
        }
  if (combine_image == (Image *) NULL)
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  combine_image->matte=matte;
  /*
    Transmogrify image as defined by the image processing options.
  */
  status=MogrifyImage(image_info,argc,argv,&combine_image);
  if (status == False)
    CatchImageException(combine_image);
  /*
    Write image.
  */
  (void) strncpy(combine_image->filename,write_filename,MaxTextExtent-1);
  (void) SetImageInfo(image_info,True,&combine_image->exception);
  status=WriteImage(image_info,combine_image);
  if (status == False)
    CatchImageException(combine_image);
  if (image_info->verbose)
    DescribeImage(combine_image,stderr,False);
  attribute=GetImageAttribute(image,"ReceiveMode");
  if ((attribute != (ImageAttribute *) NULL) &&
      (LocaleCompare("XTRNIMAGE",attribute->value) == 0))
    {
      DestroyImageInfo(image_info);
      return(True);
    }
  attribute=GetImageAttribute(image,"TransmitMode");
  if ((attribute != (ImageAttribute *) NULL) &&
      (LocaleCompare("XTRNIMAGE",attribute->value) == 0))
    {
      DestroyImageInfo(image_info);
      return(True);
    }
  DestroyImages(combine_image);
  DestroyImageInfo(image_info);
  DestroyMagick();
  if (doexit == False)
    return(True);
  LiberateMemory((void **) &argv);
  Exit(0);
  return(False);
}
