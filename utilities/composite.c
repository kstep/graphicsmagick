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
%  The composite program syntax is:
%
%  Usage: composite [ -option value ... ] image composite [ mask ] composite
%
%  Where options include:
%    -cache threshold    number of megabytes available to the pixel cache
%    -colors value       preferred number of colors in the image
%    -compose operator   composite operator
%    -colorspace type    alternate image colorspace
%    -comment string     annotate image with comment
%    -compress type      type of image compression
%    -debug              display copious debugging information
%    -density geometry   vertical and horizontal density of the image
%    -depth value        depth of the image
%    -displace geometry  shift image pixels as defined by a displacement map
%    -display server     obtain image or font from this X server
%    -dispose method     GIF disposal method
%    -dissolve value     dissolve the two images a given percent
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -filter type        use this filter when resizing an image
%    -font name          font for rendering text
%    -geometry geometry  location of the composite image
%    -gravity type       which direction to gravitate towards
%    -help               print program options
%    -interlace type     None, Line, Plane, or Partition
%    -label name         assign a label to an image
%    -matte              store matte channel if the image has one
%    -monochrome         transform image to black and white
%    -negate             replace every pixel with its complementary color
%    -page geometry      size and location of an image canvas
%    -profile filename   add ICM or IPTC information profile to image
%    -quality value      JPEG/MIFF/PNG compression level
%    -resize geometry    resize the image
%    -rotate degrees     apply Paeth rotation to the image
%    -scene value        image scene number
%    -size geometry      width and height of image
%    -stegano offset     hide watermark within an image
%    -stereo             combine two image to create a stereo anaglyph
%    -tile               repeat composite operation across image
%    -treedepth value    depth of the color tree
%    -type type          image type
%    -units type         PixelsPerInch, PixelsPerCentimeter, or Undefined
%    -unsharp geometry   sharpen the image
%    -verbose            print detailed information about the image
%    -watermark geometry percent brightness and saturation of a watermark
%
%
*/

/*
  Include declarations.
*/
#include "magick/magick.h"
#include "magick/define.h"

/*
  Typedef declarations.
*/
typedef struct _CompositeOptions
{
  char
    *displace_geometry,
    *geometry,
    *unsharp_geometry,
    *watermark_geometry;

  CompositeOperator
    compose;

  double
    dissolve;

  int
    gravity;

  long
    stegano;

  unsigned int
    stereo,
    tile;
} CompositeOptions;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p o s i t e I m a g e L i s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CompositeImageList performs all the steps needed to take the images
%  that have been read and send them to an output file.
%
%  The format of the CompositeImageList method is:
%
%      unsigned int CompositeImageList(const ImageInfo *image_info,
%        Image **image,CompositeOptions *option_info,const int argc,
%        char **argv,Image **composite_image,Image **mask_image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o image: The image.
%
%    o option_info: The option info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o composite: The composite image.
%
%    o mask_image: The mask image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int CompositeImageList(ImageInfo *image_info,Image **image,
  CompositeOptions *option_info,const int argc,char **argv,
  Image *composite_image,Image *mask_image,ExceptionInfo *exception)
{
  long
    x,
    y;

  register Image
    *p;

  unsigned int
    matte,
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  if (argc < 2)
    return(False);
  while ((*image)->previous != (Image *) NULL)
    (*image)=(*image)->previous;
  status=MogrifyImages(image_info,argc-1,argv,image);
  (void) CatchImageException(*image);
  if (composite_image != (Image *) NULL)
    {
      assert(composite_image->signature == MagickSignature);
      if (mask_image != (Image *) NULL)
        {
          assert(mask_image != (Image *) NULL);
          assert(mask_image->signature == MagickSignature);
          SetImageType(composite_image,TrueColorMatteType);
          if (!composite_image->matte)
            SetImageOpacity(composite_image,OpaqueOpacity);;
          status=CompositeImage(composite_image,CopyOpacityCompositeOp,
            mask_image,0,0);
          if (status == False)
            (void) CatchImageException(composite_image);
        }
      if (option_info->compose == DissolveCompositeOp)
        {
          register PixelPacket
            *q;

          /*
            Create mattes for dissolve.
          */
          composite_image->storage_class=DirectClass;
          composite_image->matte=True;
          for (y=0; y < (long) composite_image->rows; y++)
          {
            q=GetImagePixels(composite_image,0,y,composite_image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) composite_image->columns; x++)
            {
              if (composite_image->matte)
                q->opacity=(Quantum)
                  (((MaxRGB-q->opacity)*option_info->dissolve)/100);
              else
                q->opacity=(Quantum) ((MaxRGB*option_info->dissolve)/100);
              q++;
            }
            if (!SyncImagePixels(composite_image))
              break;
          }
        }
      if (option_info->compose == DisplaceCompositeOp)
        (void) CloneString(&composite_image->geometry,
          option_info->displace_geometry);
      if (option_info->compose == ModulateCompositeOp)
        (void) CloneString(&composite_image->geometry,
          option_info->watermark_geometry);
      if (option_info->compose == ThresholdCompositeOp)
        (void) CloneString(&composite_image->geometry,
          option_info->unsharp_geometry);
      /*
        Composite image.
      */
      matte=(*image)->matte;
      if (option_info->stegano != 0)
        {
          Image
            *stegano_image;

          (*image)->offset=option_info->stegano-1;
          stegano_image=SteganoImage(*image,composite_image,exception);
          if (stegano_image != (Image *) NULL)
            {
              DestroyImages(*image);
              *image=stegano_image;
            }
        }
      else
        if (option_info->stereo)
          {
            Image
              *stereo_image;

            stereo_image=StereoImage(*image,composite_image,exception);
            if (stereo_image != (Image *) NULL)
              {
                DestroyImages(*image);
                *image=stereo_image;
              }
          }
        else
          if (option_info->tile)
            {
              /*
                Tile the composite image.
              */
              for (y=0; y < (long) (*image)->rows; y+=composite_image->rows)
                for (x=0; x < (long) (*image)->columns; x+=composite_image->columns)
                {
                  status&=CompositeImage(*image,option_info->compose,
                    composite_image,x,y);
                  (void) CatchImageException(*image);
                }
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
              width=(*image)->columns;
              height=(*image)->rows;
              x=0;
              y=0;
              flags=ParseGeometry(option_info->geometry,&x,&y,&width,&height);
              if ((flags & XNegative) != 0)
                x+=(*image)->columns;
              if ((flags & WidthValue) == 0)
                width-=2*x > (long) width ? width : 2*x;
              if ((flags & YNegative) != 0)
                y+=(*image)->rows;
              if ((flags & HeightValue) == 0)
                height-=2*y > (long) height ? height : 2*y;
              switch (option_info->gravity)
              {
                case NorthWestGravity:
                  break;
                case NorthGravity:
                {
                  x+=(long) (0.5*width-composite_image->columns/2);
                  break;
                }
                case NorthEastGravity:
                {
                  x+=(long) (width-composite_image->columns);
                  break;
                }
                case WestGravity:
                {
                  y+=(long) (height/2-composite_image->rows/2);
                  break;
                }
                case ForgetGravity:
                case StaticGravity:
                case CenterGravity:
                default:
                {
                  x+=(long) (width/2-composite_image->columns/2);
                  y+=(long) (height/2-composite_image->rows/2);
                  break;
                }
                case EastGravity:
                {
                  x+=(long) (width-composite_image->columns);
                  y+=(long) (height/2-composite_image->rows/2);
                  break;
                }
                case SouthWestGravity:
                {
                  y+=(long) (height-composite_image->rows);
                  break;
                }
                case SouthGravity:
                {
                  x+=(long) (width/2-composite_image->columns/2);
                  y+=(long) (height-composite_image->rows);
                  break;
                }
                case SouthEastGravity:
                {
                  x+=(long) (width-composite_image->columns);
                  y+=(long) (height-composite_image->rows);
                  break;
                }
              }
              status&=CompositeImage(*image,option_info->compose,
                composite_image,x,y);
              (void) CatchImageException(*image);
            }
      (*image)->matte=matte;
    }
  /*
    Write composite images.
  */
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  for (p=(*image); p != (Image *) NULL; p=p->next)
    (void) strncpy(p->filename,argv[argc-1],MaxTextExtent-1);
  (void) SetImageInfo(image_info,True,exception);
  for (p=(*image); p != (Image *) NULL; p=p->next)
  {
    status&=WriteImage(image_info,p);
    (void) CatchImageException(p);
  }
  if (image_info->verbose)
    DescribeImage(*image,stderr,False);
  return(status);
}

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
      "-cache threshold    number of megabytes available to the pixel cache",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    alternate image colorspace",
      "-comment string     annotate image with comment",
      "-compose operator   composite operator",
      "-compress type      type of image compression",
      "-debug              display copious debugging information",
      "-density geometry   vertical and horizontal density of the image",
      "-depth value        depth of the image",
      "-displace geometry  shift image pixels as defined by a displacement map",
      "-display server     obtain image or font from this X server",
      "-dispose method     GIF disposal method",
      "-dissolve value     dissolve the two images a given percent",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-filter type        use this filter when resizing an image",
      "-font name          font for rendering text",
      "-geometry geometry  location of the composite image",
      "-gravity type       which direction to gravitate towards",
      "-help               print program options",
      "-interlace type     None, Line, Plane, or Partition",
      "-label name         ssign a label to an image",
      "-matte              store matte channel if the image has one",
      "-monochrome         transform image to black and white",
      "-negate             replace every pixel with its complementary color ",
      "-page geometry      size and location of an image canvas",
      "-profile filename   add ICM or IPTC information profile to image",
      "-quality value      JPEG/MIFF/PNG compression level",
      "-resize geometry    resize the image",
      "-rotate degrees     apply Paeth rotation to the image",
      "-scene value        image scene number",
      "-size geometry      width and height of image",
      "-stegano offset     hide watermark within an image",
      "-stereo             combine two image to create a stereo anaglyph",
      "-tile               repeat composite operation across image",
      "-treedepth value    depth of the color tree",
      "-type type          image type",
      "-units type         PixelsPerInch, PixelsPerCentimeter, or Undefined",
      "-unsharp geometry   sharpen the image",
      "-verbose            print detailed information about the image",
      "-watermark geometry percent brightness and saturation of a watermark",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf(
    "Usage: %.1024s [ -option value ... ] image composite [ mask ] composite\n",
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
%  C o m p o s i t e U t i l i t y                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompositeUtility() reads one or more images and an optional mask and
%  composites them into a new image.
%
%  The format of the CompositeUtility method is:
%
%      unsigned int CompositeUtility(const int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%
*/
MagickExport unsigned int CompositeUtility(int argc,char **argv)
{
#define NotInitialized  (unsigned int) (~0)

  char
    *filename,
    *option,
    *write_filename;

  double
    sans;

  ExceptionInfo
    exception;

  Image
    *composite_image,
    *image,
    *mask_image;

  ImageInfo
    *image_info;

  int
    j;

  long
    x;

  CompositeOptions
    option_info;

  register int
    i;

  unsigned int
    status;

  /*
    Set default.
  */
  memset(&option_info,0,sizeof(CompositeOptions));
  option_info.dissolve=0.0;
  option_info.compose=OverCompositeOp;
  composite_image=(Image *) NULL;
  option_info.displace_geometry=(char *) NULL;
  GetExceptionInfo(&exception);
  option_info.geometry=(char *) NULL;
  option_info.gravity=NorthWestGravity;
  image=(Image *) NULL;
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  (void) SetImageInfo(image_info,True,&exception);
  mask_image=(Image *) NULL;
  option_info.stegano=0;
  option_info.stereo=False;
  option_info.tile=False;
  option_info.watermark_geometry=(char *) NULL;
  option_info.unsharp_geometry=(char *) NULL;
  status=True;
  write_filename=argv[argc-1];
  /*
    Check command syntax.
  */
  filename=(char *) NULL;
  j=0;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) || ((*option != '-') && (*option != '+')))
      {
        int
          k;

        /*
          Read input images.
        */
        k=j;
        j=i+1; /* track option after the input image */
        filename=argv[i];
        (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
        if (composite_image == (Image *) NULL)
          {
            composite_image=ReadImage(image_info,&exception);
            if (exception.severity != UndefinedException)
              MagickWarning(exception.severity,exception.reason,
                exception.description);
            if (composite_image == (Image *) NULL)
              continue;
            status&=MogrifyImages(image_info,i-k,argv+k,&composite_image);
            (void) CatchImageException(composite_image);
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
            if (image == (Image *) NULL)
              continue;
            status=MogrifyImages(image_info,i-k,argv+k,&image);
            (void) CatchImageException(image);
            continue;
          }
        mask_image=ReadImage(image_info,&exception);
        if (mask_image == (Image *) NULL)
          MagickError(exception.severity,exception.reason,
            exception.description);
        if (exception.severity != UndefinedException)
          MagickWarning(exception.severity,exception.reason,
            exception.description);
        status&=mask_image != (Image *) NULL;
        if (mask_image == (Image *) NULL)
          continue;
        status&=MogrifyImages(image_info,i-k,argv+k,&mask_image);
        (void) CatchImageException(mask_image);
      }
    else
      switch(*(option+1))
      {
        case 'b':
        {
          if (LocaleCompare("background",option+1) == 0)
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
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing colors",option);
                }
              break;
            }
          if (LocaleCompare("colorspace",option+1) == 0)
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
          if (LocaleCompare("compose",option+1) == 0)
            {
              option_info.compose=CopyCompositeOp;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  option_info.compose=UndefinedCompositeOp;
                  if (LocaleCompare("Over",option) == 0)
                    option_info.compose=OverCompositeOp;
                  if (LocaleCompare("In",option) == 0)
                    option_info.compose=InCompositeOp;
                  if (LocaleCompare("Out",option) == 0)
                    option_info.compose=OutCompositeOp;
                  if (LocaleCompare("Atop",option) == 0)
                    option_info.compose=AtopCompositeOp;
                  if (LocaleCompare("Xor",option) == 0)
                    option_info.compose=XorCompositeOp;
                  if (LocaleCompare("Plus",option) == 0)
                    option_info.compose=PlusCompositeOp;
                  if (LocaleCompare("Minus",option) == 0)
                    option_info.compose=MinusCompositeOp;
                  if (LocaleCompare("Add",option) == 0)
                    option_info.compose=AddCompositeOp;
                  if (LocaleCompare("Subtract",option) == 0)
                    option_info.compose=SubtractCompositeOp;
                  if (LocaleCompare("Difference",option) == 0)
                    option_info.compose=DifferenceCompositeOp;
                  if (LocaleCompare("Multiply",option) == 0)
                    option_info.compose=MultiplyCompositeOp;
                  if (LocaleCompare("Bumpmap",option) == 0)
                    option_info.compose=BumpmapCompositeOp;
                  if (LocaleCompare("Copy",option) == 0)
                    option_info.compose=CopyCompositeOp;
                  if (LocaleCompare("CopyRed",option) == 0)
                    option_info.compose=CopyRedCompositeOp;
                  if (LocaleCompare("CopyGreen",option) == 0)
                    option_info.compose=CopyGreenCompositeOp;
                  if (LocaleCompare("CopyBlue",option) == 0)
                    option_info.compose=CopyBlueCompositeOp;
                  if (LocaleCompare("CopyOpacity",option) == 0)
                    option_info.compose=CopyOpacityCompositeOp;
                  if (LocaleCompare("Clear",option) == 0)
                    option_info.compose=ClearCompositeOp;
                  if (option_info.compose == UndefinedCompositeOp)
                    MagickError(OptionError,"Invalid compose type",option);
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
          if (LocaleCompare("copy",option+1) == 0)
            {
              if (*option == '-')
                {
                  Image
                    *clone_image;

                  ImageInfo
                    *clone_info;

                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing output filename",
                      option);
                  if (image == (Image *) NULL)
                    MagickError(OptionError,"Missing source image",
                      (char *) NULL);
                  clone_info=CloneImageInfo(image_info);
                  clone_image=CloneImage(image,0,0,True,&image->exception);
                  if (clone_image == (Image *) NULL)
                    MagickError(OptionError,"Missing an image file name",
                      (char *) NULL);
                  status&=CompositeImageList(clone_info,&clone_image,&option_info,
                    i-j+2,argv+j-1,composite_image,mask_image,&exception);
                  if (composite_image != (Image *) NULL)
                    {
                      DestroyImages(composite_image);
                      composite_image=(Image *) NULL;
                    }
                  if (mask_image != (Image *) NULL)
                    {
                      DestroyImages(mask_image);
                      mask_image=(Image *) NULL;
                    }
                  DestroyImages(clone_image);
                  DestroyImageInfo(clone_info);
                  j=i+1;
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
          if (LocaleCompare("displace",option+1) == 0)
            {
              (void) CloneString(&option_info.displace_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&option_info.displace_geometry,argv[i]);
                  option_info.compose=DisplaceCompositeOp;
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
          if (LocaleCompare("dissolve",option+1) == 0)
            {
              option_info.dissolve=0.0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing value",option);
                  option_info.dissolve=atof(argv[i]);
                  option_info.compose=DissolveCompositeOp;
                }
              break;
            }
          if (LocaleCompare("dither",option+1) == 0)
            {
              image_info->dither=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'f':
        {
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
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'g':
        {
          if (LocaleCompare("geometry",option+1) == 0)
            {
              (void) CloneString(&option_info.geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&option_info.geometry,argv[i]);
                }
              break;
            }
          if (LocaleCompare("gravity",option+1) == 0)
            {
              option_info.gravity=ForgetGravity;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  if (LocaleCompare("Forget",option) == 0)
                    option_info.gravity=ForgetGravity;
                  if (LocaleCompare("NorthWest",option) == 0)
                    option_info.gravity=NorthWestGravity;
                  if (LocaleCompare("North",option) == 0)
                    option_info.gravity=NorthGravity;
                  if (LocaleCompare("NorthEast",option) == 0)
                    option_info.gravity=NorthEastGravity;
                  if (LocaleCompare("West",option) == 0)
                    option_info.gravity=WestGravity;
                  if (LocaleCompare("Center",option) == 0)
                    option_info.gravity=CenterGravity;
                  if (LocaleCompare("East",option) == 0)
                    option_info.gravity=EastGravity;
                  if (LocaleCompare("SouthWest",option) == 0)
                    option_info.gravity=SouthWestGravity;
                  if (LocaleCompare("South",option) == 0)
                    option_info.gravity=SouthGravity;
                  if (LocaleCompare("SouthEast",option) == 0)
                    option_info.gravity=SouthEastGravity;
                  if (option_info.gravity == ForgetGravity)
                    MagickError(OptionError,"Invalid gravity type",option);
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
              CompositeUsage();
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
          if (LocaleCompare("monochrome",option+1) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'n':
        {
          if (LocaleCompare("negate",option+1) == 0)
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
                  image_info->page=PostscriptGeometry(argv[i]);
                }
              break;
            }
          if (LocaleCompare("process",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing argument",option);
                }
              break;
            }
          if (LocaleCompare("profile",option+1) == 0)
            {
              i++;
              if (i == argc)
                MagickError(OptionError,"Missing profile",option);
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
          if (LocaleCompare("rotate",option+1) == 0)
            {
              i++;
              if ((i == argc) || !IsGeometry(argv[i]))
                MagickError(OptionError,"Missing degrees",option);
              break;
            }
          if (LocaleCompare("replace",option+1) == 0)
            {
              i++;
              if (*option == '-')
                {
                  if (i == argc)
                    MagickError(OptionError,"Missing output filename",
                      option);
                  if (image == (Image *) NULL)
                    MagickError(OptionError,"Missing source image",
                      (char *) NULL);
                  status&=CompositeImageList(image_info,&image,&option_info,i-j+2,
                    argv+j-1,composite_image,mask_image,&exception);
                  if (composite_image != (Image *) NULL)
                    {
                      DestroyImages(composite_image);
                      composite_image=(Image *) NULL;
                    }
                  if (mask_image != (Image *) NULL)
                    {
                      DestroyImages(mask_image);
                      mask_image=(Image *) NULL;
                    }
                  j=i+1;
                }
              break;
            }
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
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 's':
        {
          if (LocaleCompare("scene",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing scene number",option);
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
          if (LocaleCompare("stegano",option+1) == 0)
            {
              option_info.stegano=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing offset",option);
                  option_info.stegano=atol(argv[i])+1;
                }
              break;
            }
          if (LocaleCompare("stereo",option+1) == 0)
            {
              option_info.stereo=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 't':
        {
          if (LocaleCompare("tile",option+1) == 0)
            {
              option_info.tile=(*option == '-');
              break;
            }
          if (LocaleCompare("treedepth",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                    MagickError(OptionError,"Missing depth",option);
                }
              break;
            }
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
        case 'u':
        {
          if (LocaleCompare("units",option+1) == 0)
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
          if (LocaleCompare("unsharp",option+1) == 0)
            {
              (void) CloneString(&option_info.unsharp_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&option_info.unsharp_geometry,argv[i]);
                  option_info.compose=ThresholdCompositeOp;
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
        case 'w':
        {
          if (LocaleCompare("watermark",option+1) == 0)
            {
              (void) CloneString(&option_info.watermark_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&option_info.watermark_geometry,argv[i]);
                  option_info.compose=ModulateCompositeOp;
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case '?':
        {
          CompositeUsage();
          break;
        }
        default:
        {
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
      }
  }
  if ((i != (argc-1)) || (image == (Image *) NULL))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  status&=CompositeImageList(image_info,&image,&option_info,argc-j+1,argv+j-1,
    composite_image,mask_image,&exception);
  if (option_info.displace_geometry != (char *) NULL)
    LiberateMemory((void **) &option_info.displace_geometry);
  if (option_info.geometry != (char *) NULL)
    LiberateMemory((void **) &option_info.geometry);
  if (option_info.unsharp_geometry != (char *) NULL)
    LiberateMemory((void **) &option_info.unsharp_geometry);
  if (option_info.watermark_geometry != (char *) NULL)
    LiberateMemory((void **) &option_info.watermark_geometry);
  if (composite_image != (Image *) NULL)
    DestroyImages(composite_image);
  if (mask_image != (Image *) NULL)
    DestroyImages(mask_image);
  DestroyImageList(image);
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
  InitializeMagick((const char *) NULL);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  if (argc < 4)
    CompositeUsage();
  status=CompositeUtility(argc,argv);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(!status);
  return(False);
}
#endif
