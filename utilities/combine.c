/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                CCCC   OOO   M   M  BBBB   IIIII  N   N  EEEEE               %
%               C      O   O  MM MM  B   B    I    NN  N  E                   %
%               C      O   O  M M M  BBBB     I    N N N  EEE                 %
%               C      O   O  M   M  B   B    I    N  NN  E                   %
%                CCCC   OOO   M   N  BBBB   IIIII  N   N  EEEEE               %
%                                                                             %
%                                                                             %
%                        Digitally combine two images.                        %
%                                                                             %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               January 1993                                  %
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
%  The combine program syntax is:
%
%  Usage: combine [options ...] image composite [mask] combined
%
%  Where options include:
%    -blend value        blend the two images a given percent
%    -cache threshold    number of megabytes available to the pixel cache
%    -colors value       preferred number of colors in the image
%    -compose operator   composite operator
%    -colorspace type    alternate image colorspace
%    -comment string     annotate image with comment
%    -compress type      type of image compression
%    -density geometry   vertical and horizontal density of the image
%    -displace geometry  shift image pixels as defined by a displacement map
%    -display server     obtain image or font from this X server
%    -dispose method     GIF disposal method
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -font name          X11 font for displaying text
%    -geometry geometry  perferred size or location of the image
%    -gravity type       which direction to gravitate towards
%    -interlace type     None, Line, Plane, or Partition
%    -label name         assign a label to an image
%    -matte              store matte channel if the image has one
%    -monochrome         transform image to black and white
%    -negate             replace every pixel with its complementary color 
%    -page geometry      size and location of an image canvas
%    -quality value      JPEG/MIFF/PNG compression level
%    -scene value        image scene number
%    -size geometry      width and height of image
%    -stegano offset     hide watermark within an image
%    -stereo             combine two image to create a stereo anaglyph
%    -tile               repeat composite operation across image
%    -treedepth value    depth of the color storage_class tree
%    -verbose            print detailed information about the image
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
%  Procedure Usage displays the program usage;
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
      "-blend value        blend the two images a given percent",
      "-cache threshold    number of megabytes available to the pixel cache",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    alternate image colorspace",
      "-comment string     annotate image with comment",
      "-compose operator   composite operator",
      "-compress type      type of image compression",
      "-density geometry   vertical and horizontal density of the image",
      "-displace geometry  shift image pixels as defined by a displacement map",
      "-display server     obtain image or font from this X server",
      "-dispose method     GIF disposal method",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-font name          X11 font for displaying text",
      "-geometry geometry  perferred size or location of the image",
      "-gravity type       which direction to gravitate towards",
      "-interlace type     None, Line, Plane, or Partition",
      "-label name         ssign a label to an image",
      "-matte              store matte channel if the image has one",
      "-monochrome         transform image to black and white",
      "-negate             replace every pixel with its complementary color ",
      "-page geometry      size and location of an image canvas",
      "-quality value      JPEG/MIFF/PNG compression level",
      "-scene value        image scene number",
      "-size geometry      width and height of image",
      "-stegano offset     hide watermark within an image",
      "-stereo             combine two image to create a stereo anaglyph",
      "-tile               repeat composite operation across image",
      "-treedepth value    depth of the color storage_class tree",
      "-verbose            print detailed information about the image",
      (char *) NULL
    };

  (void) printf("Version: %.1024s\n",MagickVersion);
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [options ...] image composite [mask] combined\n",
    client_name);
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
    *client_name,
    *displacement_geometry,
    *watermark_geometry,
    *filename,
    *geometry,
    *option,
    *write_filename;

  CompositeOperator
    compose;

  double
    blend,
    sans;

  ExceptionInfo
    exception;

  Image
    *combined_image,
    *composite_image,
    *image,
    *mask_image;

  ImageInfo
    *image_info;

  int
    gravity,
    x,
    y;

  register int
    i;

  unsigned int
    sendmode,
    status,
    stegano,
    stereo,
    tile;

  void
    *param1,
    *param2;

  /*
    Initialize command line arguments.
  */
  if (LocaleCompare("-combine",argv[0]) == 0)
    {
      sendmode=1; /* set mode to - called as a subroutine */
      client_name=SetClientName((char *) NULL);
      if (argc < 4)
        return(False);
    }
  else
    {
      sendmode=0; /* set mode to - called as normal executable */
      ReadCommandlLine(argc,&argv);
      MagickIncarnate(*argv);
      client_name=SetClientName((char *) NULL);
      status=ExpandFilenames(&argc,&argv);
      if (status == False)
        MagickError(ResourceLimitError,"Memory allocation failed",
          (char *) NULL);
      if (argc < 4)
        Usage(client_name);
    }
  /*
    Set default.
  */
  blend=0.0;
  compose=ReplaceCompositeOp;
  composite_image=(Image *) NULL;
  displacement_geometry=(char *) NULL;
  GetExceptionInfo(&exception);
  geometry=(char *) NULL;
  gravity=NorthWestGravity;
  image=(Image *) NULL;
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strcpy(image_info->filename,argv[argc-1]);
  SetImageInfo(image_info,True);
  if (strcmp(image_info->magick,"CACHE") == 0)
    SetCacheThreshold(0);
  mask_image=(Image *) NULL;
  stegano=0;
  stereo=False;
  tile=False;
  watermark_geometry=(char *) NULL;
  write_filename=argv[argc-1];
  /*
    Check command syntax.
  */
  filename=(char *) NULL;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((Extent(option) < 2) || ((*option != '-') && (*option != '+')))
      {
        /*
          Read input images.
        */
        filename=argv[i];
        (void) strcpy(image_info->filename,filename);
        if (image == (Image *) NULL)
          {
            image=ReadImage(image_info,&exception);
            if (image == (Image *) NULL)
              MagickError(exception.severity,exception.message,
                exception.qualifier);
            continue;
          }
        if (mask_image != (Image *) NULL)
          MagickError(OptionError,"input images already specified",filename);
        if (composite_image == (Image *) NULL)
          {
            composite_image=ReadImage(image_info,&exception);
            if (composite_image == (Image *) NULL)
              MagickError(exception.severity,exception.message,
                exception.qualifier);
            continue;
          }
        mask_image=ReadImage(image_info,&exception);
        if (mask_image == (Image *) NULL)
          MagickError(exception.severity,exception.message,exception.qualifier);
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
          if (LocaleNCompare("blend",option+1,3) == 0)
            {
              blend=0.0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing value",option);
                  blend=atof(argv[i]);
                  compose=BlendCompositeOp;
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
              compose=ReplaceCompositeOp;
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
                  if (LocaleCompare("Bumpmap",option) == 0)
                    compose=BumpmapCompositeOp;
                  if (LocaleCompare("Replace",option) == 0)
                    compose=ReplaceCompositeOp;
                  if (LocaleCompare("ReplaceRed",option) == 0)
                    compose=ReplaceRedCompositeOp;
                  if (LocaleCompare("ReplaceGreen",option) == 0)
                    compose=ReplaceGreenCompositeOp;
                  if (LocaleCompare("ReplaceBlue",option) == 0)
                    compose=ReplaceBlueCompositeOp;
                  if (LocaleCompare("ReplaceMatte",option) == 0)
                    compose=ReplaceMatteCompositeOp;
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
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'd':
        {
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
          if (LocaleCompare("displace",option+1) == 0)
            {
              CloneString(&displacement_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&displacement_geometry,argv[i]);
                  compose=DisplaceCompositeOp;
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
              CloneString(&image_info->dispose,(char *) NULL);
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
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'f':
        {
          CloneString(&image_info->font,(char *) NULL);
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
              CloneString(&geometry,(char *) NULL);
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
              Usage(client_name);
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
        case 's':
        {
          if (LocaleNCompare("scene",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing scene number",option);
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
          if (LocaleNCompare("stegano",option+1,4) == 0)
            {
              stegano=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
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
          MagickError(OptionError,"Unrecognized option",option);
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
        case 'v':
        {
          if (LocaleNCompare("verbose",option+1,2) == 0)
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
              CloneString(&watermark_geometry,(char *) NULL);
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
        case 'x':
        {
          if (LocaleCompare("xbdat",option+1) == 0)
            {
              i++;
              if ((i == argc) && (sendmode>0))
                MagickError(OptionError,"Missing blob buffer",option);
              param1=(void *)argv[i];
              argv[i]=AllocateString("");
              sendmode=2;
              break;
            }
          if (LocaleCompare("xblen",option+1) == 0)
            {
              i++;
              if ((i == argc) && (sendmode>0))
                MagickError(OptionError,"Missing blob length",option);
              param2=(void *)argv[i];
              argv[i]=AllocateString("");
              sendmode=2;
              break;
            }
          if (LocaleCompare("xfunc",option+1) == 0)
            {
              i++;
              if ((i == argc) && (sendmode>0))
                MagickError(OptionError,"Missing stream func",option);
              param1=(void *)argv[i];
              argv[i]=AllocateString("");
              sendmode=3;
              break;
            }
          if (LocaleCompare("xctxt",option+1) == 0)
            {
              i++;
              if ((i == argc) && (sendmode>0))
                MagickError(OptionError,"Missing stream context",option);
              param2=(void *)argv[i];
              argv[i]=AllocateString("");
              sendmode=3;
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
  }
  if ((image == (Image *) NULL) || (composite_image == (Image *) NULL))
    Usage(client_name);
  if (i != (argc-1))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  if (mask_image != (Image *) NULL)
    {
      status=CompositeImage(composite_image,ReplaceMatteCompositeOp,
        mask_image,0,0);
      if (status == False)
        CatchImageException(composite_image);
      DestroyImage(mask_image);
    }
  if (compose == BlendCompositeOp)
    {
      Quantum
        opacity;

      register PixelPacket
        *q;

      /*
        Create mattes for blending.
      */
      opacity=(TransparentOpacity*blend)/100;
      image->storage_class=DirectClass;
      image->matte=True;
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          q->opacity=opacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      composite_image->storage_class=DirectClass;
      composite_image->matte=True;
      for (y=0; y < (int) composite_image->rows; y++)
      {
        q=GetImagePixels(composite_image,0,y,composite_image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) composite_image->columns; x++)
        {
          q->opacity=TransparentOpacity-opacity;
          q++;
        }
        if (!SyncImagePixels(composite_image))
          break;
      }
    }
  if (compose == DisplaceCompositeOp)
    CloneString(&composite_image->geometry,displacement_geometry);
  if (compose == ModulateCompositeOp)
    CloneString(&composite_image->geometry,watermark_geometry);
  /*
    Combine image.
  */
  if (stegano != 0)
    {
      image->offset=stegano-1;
      combined_image=SteganoImage(image,composite_image,&exception);
    }
  else
    if (stereo)
      combined_image=StereoImage(image,composite_image,&exception);
    else
      if (tile)
        {
          /*
            Tile the composite image.
          */
          for (y=0; y < (int) image->rows; y+=(int) composite_image->rows)
            for (x=0; x < (int) image->columns; x+=(int) composite_image->columns)
            {
              status=CompositeImage(image,compose,composite_image,x,y);
              if (status == False)
                CatchImageException(image);
            }
          combined_image=image;
        }
      else
        {
          unsigned int
            size;

          /*
            Digitally composite image.
          */
          size=0;
          x=0;
          y=0;
          if (geometry != (char *) NULL)
            (void) ParseImageGeometry(geometry,&x,&y,&size,&size);
          switch (gravity)
          {
            case NorthWestGravity:
              break;
            case NorthGravity:
            {
              x+=((int) image->columns-(int) composite_image->columns)/2;
              break;
            }
            case NorthEastGravity:
            {
              x+=(int) image->columns-(int) composite_image->columns;
              break;
            }
            case WestGravity:
            {
              y+=(int) (image->rows-(int) composite_image->rows)/2;
              break;
            }
            case ForgetGravity:
            {
              char
                geometry[MaxTextExtent];

              /*
                Stretch composite to the same size as the image.
              */
              FormatString(geometry,"%ux%u+0+0",image->columns,image->rows);
              TransformImage(&composite_image,(char *) NULL,geometry);
              break;
            }
            case StaticGravity:
            case CenterGravity:
            default:
            {
              x+=((int) image->columns-(int) composite_image->columns)/2;
              y+=((int) image->rows-(int) composite_image->rows)/2;
              break;
            }
            case EastGravity:
            {
              x+=(int) image->columns-(int) composite_image->columns;
              y+=((int) image->rows-(int) composite_image->rows)/2;
              break;
            }
            case SouthWestGravity:
            {
              y+=(int) image->rows-(int) composite_image->rows;
              break;
            }
            case SouthGravity:
            {
              x+=((int) image->columns-(int) composite_image->columns)/2;
              y+=(int) image->rows-(int) composite_image->rows;
              break;
            }
            case SouthEastGravity:
            {
              x+=(int) image->columns-(int) composite_image->columns;
              y+=(int) image->rows-(int) composite_image->rows;
              break;
            }
          }
          status=CompositeImage(image,compose,composite_image,x,y);
          if (status == False)
            CatchImageException(image);
          combined_image=image;
        }
  if (combined_image == (Image *) NULL)
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  /*
    Transmogrify image as defined by the image processing options.
  */
  status=MogrifyImage(image_info,argc,argv,&combined_image);
  if (status == False)
    CatchImageException(combined_image);
  /*
    Write image.
  */
  (void) strcpy(combined_image->filename,write_filename);
  SetImageInfo(image_info,True);
  status=True;
  switch (sendmode)
  {
    case 2:
    {
      char
        **blob_data;
      ExceptionInfo
        exception;

      size_t
        *blob_length;

      blob_data=(char **)param1;
      blob_length=(size_t *)param2;
      (void) strcpy(combined_image->magick,image_info->magick);
      if (*blob_length == 0)
        *blob_length=8192;
      *blob_data=ImageToBlob(image_info,combined_image,blob_length,
        &exception);
      if (*blob_data == NULL)
        status=False;
      break;
    }
    case 3:
    {
      int
        (*fifo)(const Image *,const void *,const size_t);

      fifo=(int (*)(const Image *,const void *,const size_t)) param1;
      combined_image->client_data=param2;
      status=WriteStream(image_info,combined_image,fifo);
      break;
    }
  default:
    {
      status=WriteImage(image_info,combined_image);
      break;
    }
  }
  if (status == False)
    CatchImageException(combined_image);
  if (image_info->verbose)
    DescribeImage(combined_image,stderr,False);
  DestroyImages(combined_image);
  DestroyImageInfo(image_info);
  if (sendmode)
    return(True);
  LiberateMemory((void **) &argv);
  Exit(0);
  return(False);
}
