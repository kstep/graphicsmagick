/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%               CCCC   OOO   M   M  M   M   AAA   N   N   DDDD                %
%              C      O   O  MM MM  MM MM  A   A  NN  N   D   D               %
%              C      O   O  M M M  M M M  AAAAA  N N N   D   D               %
%              C      O   O  M   M  M   M  A   A  N  NN   D   D               %
%               CCCC   OOO   M   M  M   M  A   A  N   N   DDDD                %
%                                                                             %
%                                                                             %
%                        Image Command Methods                                %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                            Bill Radcliffe                                   %
%                              July 2002                                      %
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
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "cache.h"
#include "delegate.h"
#include "list.h"
#include "log.h"
#include "magic.h"
#include "magick.h"
#include "module.h"
#include "quantize.h"
#include "render.h"
#include "utility.h"
#include "version.h"

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

  GravityType
    gravity;

  double
    dissolve;

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
%  C o m p o s i t e I m a g e C o m m a n d                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompositeImageCommand() reads one or more images and an optional mask and
%  composites them into a new image.
%
%  The format of the CompositeImageCommand method is:
%
%      unsigned int CompositeImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int CompositeImageList(ImageInfo *image_info,Image **image,
  Image *composite_image,Image *mask_image,CompositeOptions *option_info,
  ExceptionInfo *exception)
{
  long
    x,
    y;

  unsigned int
    matte,
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  status=True;
  if (composite_image != (Image *) NULL)
    {
      assert(composite_image->signature == MagickSignature);
      if (mask_image != (Image *) NULL)
        {
          assert(mask_image->signature == MagickSignature);
          SetImageType(composite_image,TrueColorMatteType);
          if (!composite_image->matte)
            SetImageOpacity(composite_image,OpaqueOpacity);
          status&=CompositeImage(composite_image,CopyOpacityCompositeOp,
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
          if (!composite_image->matte)
            SetImageOpacity(composite_image,OpaqueOpacity);
          for (y=0; y < (long) composite_image->rows; y++)
          {
            q=GetImagePixels(composite_image,0,y,composite_image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) composite_image->columns; x++)
            {
              q->opacity=(Quantum)
                (((MaxRGB-q->opacity)*option_info->dissolve)/100.0);
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
              char
                composite_geometry[MaxTextExtent];

              RectangleInfo
                geometry;

              /*
                Digitally composite image.
              */
              geometry.x=0;
              geometry.y=0;
              (void) GetGeometry(option_info->geometry,&geometry.x,&geometry.y,
                &geometry.width,&geometry.height);
              FormatString(composite_geometry,"%lux%lu%+ld%+ld",
                composite_image->columns,composite_image->rows,geometry.x,
                geometry.y);
              (*image)->gravity=option_info->gravity;
              (void) GetImageGeometry(*image,composite_geometry,False,
                &geometry);
              status&=CompositeImage(*image,option_info->compose,
                composite_image,geometry.x,geometry.y);
              (void) CatchImageException(*image);
            }
      (*image)->matte=matte;
    }
  return(status);
}

static void LiberateCompositeOptions(CompositeOptions *option_info)
{
  if (option_info->displace_geometry != (char *) NULL)
    LiberateMemory((void **) &(option_info->displace_geometry));
  if (option_info->geometry != (char *) NULL)
    LiberateMemory((void **) &(option_info->geometry));
  if (option_info->unsharp_geometry != (char *) NULL)
    LiberateMemory((void **) &(option_info->unsharp_geometry));
  if (option_info->watermark_geometry != (char *) NULL)
    LiberateMemory((void **) &(option_info->watermark_geometry));
}

MagickExport unsigned int CompositeImageCommand(ImageInfo *image_info,
  const int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
#define NotInitialized  (unsigned int) (~0)
#define ThrowCompositeException(code,reason,description) \
{ \
  LiberateCompositeOptions(&option_info); \
  DestroyImageList(image); \
  DestroyImageList(composite_image); \
  DestroyImageList(mask_image); \
  ThrowException(exception,code,reason,description); \
  return(False); \
}

  char
    *filename,
    *format,
    *option;

  CompositeOptions
    option_info;

  double
    sans;

  Image
    *composite_image,
    *image,
    *mask_image;

  long
    j,
    x;

  register long
    i;

  unsigned int
    status;

  /*
    Set default.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  status=True;
  memset(&option_info,0,sizeof(CompositeOptions));
  option_info.dissolve=0.0;
  option_info.compose=OverCompositeOp;
  composite_image=(Image *) NULL;
  option_info.displace_geometry=(char *) NULL;
  filename=(char *) NULL;
  format="%w,%h,%m";
  option_info.geometry=(char *) NULL;
  image=(Image *) NULL;
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  (void) SetImageInfo(image_info,True,exception);
  mask_image=(Image *) NULL;
  option_info.stegano=0;
  option_info.stereo=False;
  option_info.tile=False;
  option_info.watermark_geometry=(char *) NULL;
  option_info.unsharp_geometry=(char *) NULL;
  status=True;
  /*
    Check command syntax.
  */
  j=1;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        /*
          Read input images.
        */
        filename=argv[i];
        (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
        if (composite_image == (Image *) NULL)
          {
            composite_image=ReadImage(image_info,exception);
            if (composite_image != (Image *) NULL)
              {
                status&=MogrifyImages(image_info,i-j,argv+j,&composite_image);
                (void) CatchImageException(composite_image);
              }
            j=i+1;
            continue;
          }
        if (mask_image != (Image *) NULL)
          ThrowCompositeException(OptionError,"input images already specified",
            filename);
        if (image == (Image *) NULL)
          {
            image=ReadImage(image_info,exception);
            if (image != (Image *) NULL)
              {
                status&=MogrifyImages(image_info,i-j,argv+j,&image);
                (void) CatchImageException(image);
              }
            j=i+1;
            continue;
          }
        mask_image=ReadImage(image_info,exception);
        status&=mask_image != (Image *) NULL;
        if (mask_image != (Image *) NULL)
          {
            status&=MogrifyImages(image_info,i-j,argv+j,&mask_image);
            (void) CatchImageException(mask_image);
          }
        j=i+1;
        continue;
      }
    switch(*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("affine",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowCompositeException(OptionError,"Missing matrix",option);
              }
            break;
          }
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,"Missing password",
                    option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,
                    "Missing background color",option);
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            if (*option == '-')
              {
                unsigned long
                  maximum,
                  minimum;

                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,"Missing threshold",
                    option);
                minimum=(~0);
                maximum=(~0);
                (void) sscanf(argv[i],"%lux%lu",&minimum,&maximum);
                SetCacheThreshold(minimum,maximum);
              }
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,"Missing colors",option);
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
                  ThrowCompositeException(OptionError,"Missing type",option);
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
                  ThrowCompositeException(OptionError,"Invalid colorspace type",
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,"Missing comment",option);
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
                  ThrowCompositeException(OptionError,"Missing type",option);
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
                  ThrowCompositeException(OptionError,"Invalid compose type",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing type",option);
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
                  image_info->compression=RLECompression;
                if (LocaleCompare("Zip",option) == 0)
                  image_info->compression=ZipCompression;
                if (image_info->compression == UndefinedCompression)
                  ThrowCompositeException(OptionError,
                    "Invalid compression type",option);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,"Missing event mask",
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,"Missing geometry",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing image depth",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing geometry",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing server name",
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,"Missing method",option);
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  ThrowCompositeException(OptionError,"Invalid dispose method",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing value",option);
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
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'e':
      {
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,"Missing encoding type",
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,"Missing type",option);
                option=argv[i];
                image_info->endian=UndefinedEndian;
                if (LocaleCompare("LSB",option) == 0)
                  image_info->endian=LSBEndian;
                if (LocaleCompare("MSB",option) == 0)
                  image_info->endian=MSBEndian;
                if (image_info->endian == UndefinedEndian)
                  ThrowCompositeException(OptionError,"Invalid endian type",
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
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
                  ThrowCompositeException(OptionError,"Missing type",option);
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
                  ThrowCompositeException(OptionError,"Invalid filter type",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing font name",
                    option);
                (void) CloneString(&image_info->font,argv[i]);
              }
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,"Missing format string",
                    option);
                format=argv[i];
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
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
                  ThrowCompositeException(OptionError,"Missing geometry",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing type",option);
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
                  ThrowCompositeException(OptionError,"Invalid gravity type",
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          break;
        ThrowCompositeException(OptionError,"Unrecognized option",option)
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
                  ThrowCompositeException(OptionError,"Missing type",option);
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
                  ThrowCompositeException(OptionError,"Invalid interlace type",
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,"Missing label name",
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'm':
      {
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("monochrome",option+1) == 0)
          break;
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          break;
        if (LocaleCompare("noop",option+1) == 0)
          {
            status&=CompositeImageList(image_info,&image,composite_image,
              mask_image,&option_info,exception);
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
            (void) CatchImageException(image);
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
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
                  ThrowCompositeException(OptionError,"Missing page geometry",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing argument",
                    option);
              }
            break;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowCompositeException(OptionError,"Missing profile",option);
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
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
                  ThrowCompositeException(OptionError,"Missing quality",option);
                image_info->quality=atol(argv[i]);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'r':
      {
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowCompositeException(OptionError,"Missing degrees",option);
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          break;
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,"Missing geometry",
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 's':
      {
        if (LocaleCompare("sampling_factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,"Missing geometry",
                    option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
              }
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,"Missing scene number",
                    option);
              }
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowCompositeException(OptionError,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,"Missing geometry",
                    option);
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
                  ThrowCompositeException(OptionError,"Missing offset",option);
                option_info.stegano=atol(argv[i])+1;
              }
            break;
          }
        if (LocaleCompare("stereo",option+1) == 0)
          {
            option_info.stereo=(*option == '-');
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 't':
      {
        if (LocaleCompare("tile",option+1) == 0)
          {
            option_info.tile=(*option == '-');
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          break;
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,"Missing depth",option);
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
                  ThrowCompositeException(OptionError,"Missing type",option);
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
                  ThrowCompositeException(OptionError,"Invalid image type",
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
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
                  ThrowCompositeException(OptionError,"Missing type",option);
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
                  ThrowCompositeException(OptionError,"Missing geometry",
                    option);
                (void) CloneString(&option_info.unsharp_geometry,argv[i]);
                option_info.compose=ThresholdCompositeOp;
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          {
            (void) fprintf(stdout,"Version: %.1024s\n",
              GetMagickVersion((unsigned long *) NULL));
            (void) fprintf(stdout,"Copyright: %.1024s\n\n",
              GetMagickCopyright());
            break;
          }
        if (LocaleCompare("virtual_pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,
                    "Missing virtual pixel method",option);
                option=argv[i];
                virtual_pixel_method=UndefinedVirtualPixelMethod;
                if (LocaleCompare("Constant",option) == 0)
                  virtual_pixel_method=ConstantVirtualPixelMethod;
                if (LocaleCompare("Edge",option) == 0)
                  virtual_pixel_method=EdgeVirtualPixelMethod;
                if (LocaleCompare("Mirror",option) == 0)
                  virtual_pixel_method=MirrorVirtualPixelMethod;
                if (LocaleCompare("Tile",option) == 0)
                  virtual_pixel_method=TileVirtualPixelMethod;
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowCompositeException(OptionError,
                    "Invalid virtual pixel method",option);
              }
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
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
                  ThrowCompositeException(OptionError,"Missing geometry",
                    option);
                (void) CloneString(&option_info.watermark_geometry,argv[i]);
                option_info.compose=ModulateCompositeOp;
              }
            break;
          }
        if (LocaleCompare("write",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowCompositeException(OptionError,"Missing filename",option);
            break;
          }
        ThrowCompositeException(OptionError,"Unrecognized option",option)
      }
      case '?':
        break;
      default:
        ThrowCompositeException(OptionError,"Unrecognized option",option)
    }
  }
  if (image == (Image *) NULL)
    {
      if (exception->severity == UndefinedException)
        ThrowCompositeException(OptionError,"Missing an image file name",
          (char *) NULL);
      return(False);
    }
  if (i != (argc-1))
    ThrowCompositeException(OptionError,"Missing an image file name",
      (char *) NULL);
  status&=MogrifyImages(image_info,i-j,argv+j,&image);
  (void) CatchImageException(image);
  status&=CompositeImageList(image_info,&image,composite_image,mask_image,
    &option_info,exception);
  /*
    Write composite images.
  */
  status&=WriteImages(image_info,image,argv[argc-1],exception);
  if (metadata != (char **) NULL)
    {
      char
        *text;

      text=TranslateText(image_info,image,format);
      if (text == (char *) NULL)
        ThrowCompositeException(ResourceLimitError,
          "Unable to format image metadata","Memory allocation failed");
      (void) ConcatenateString(&(*metadata),text);
      (void) ConcatenateString(&(*metadata),"\n");
    }
  LiberateCompositeOptions(&option_info);
  DestroyImageList(composite_image);
  DestroyImageList(mask_image);
  DestroyImageList(image);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n v e r t I m a g e C o m m a n d                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConvertImageCommand() reads one or more images, applies one or more image
%  processing operations, and writes out the image in the same or differing
%  format.
%
%  The format of the ConvertImageCommand method is:
%
%      unsigned int ConvertImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static unsigned int ConcatenateImages(const int argc,char **argv,
  ExceptionInfo *exception)
{
  FILE
    *input,
    *output;

  int
    c;

  register long
    i;

  /*
    Open output file.
  */
  output=fopen(argv[argc-1],"wb");
  if (output == (FILE *) NULL)
    {
      ThrowException(exception,FileOpenError,"Unable to open file",
        argv[argc-1]);
      return(False);
    }
  for (i=2; i < (argc-1); i++)
  {
    input=fopen(argv[i],"rb");
    if (input == (FILE *) NULL)
      {
        ThrowException(exception,FileOpenError,"Unable to open file",argv[i]);
        continue;
      }
    for (c=fgetc(input); c != EOF; c=fgetc(input))
      (void) fputc((char) c,output);
    (void) fclose(input);
    (void) remove(argv[i]);
  }
  (void) fclose(output);
  return(True);
}

MagickExport unsigned int ConvertImageCommand(ImageInfo *image_info,
  const int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
#define NotInitialized  (unsigned int) (~0)
#define ThrowConvertException(code,reason,description) \
{ \
  DestroyImageList(image); \
  DestroyImageList(image_list); \
  ThrowException(exception,code,reason,description); \
  return(False); \
}

  char
    *filename,
    *format,
    *option;

  double
    sans;

  Image
    *image,
    *image_list,
    *next_image;

  long
    j,
    k,
    x;

  register Image
    *p;

  register long
    i;

  unsigned int
    ping,
    status;

  /*
    Set defaults.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  status=True;
  filename=(char *) NULL;
  format="%w,%h,%m";
  image=(Image *) NULL;
  image_list=(Image *) NULL;
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  (void) SetImageInfo(image_info,True,exception);
  ping=False;
  option=(char *) NULL;
  status=True;
  /*
    Parse command-line arguments.
  */
  if ((argc > 2) && (LocaleCompare("-concatenate",argv[1]) == 0))
    return(ConcatenateImages(argc,argv,exception));
  j=1;
  k=0;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        /*
          Read input image.
        */
        k=i;
        filename=argv[i];
        (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
        if (ping)
          next_image=PingImage(image_info,exception);
        else
          next_image=ReadImage(image_info,exception);
        status&=(next_image != (Image *) NULL) &&
          (exception->severity < ErrorException);
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
        continue;
      }
    if ((image != (Image *) NULL) && (j != (k+1)))
      {
        status&=MogrifyImages(image_info,i-j,argv+j,&image);
        (void) CatchImageException(image);
        PushImageList(&image_list,image,exception);
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
        if (LocaleCompare("affine",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing matrix",option);
              }
            break;
          }
        if (LocaleCompare("antialias",option+1) == 0)
          {
            image_info->antialias=(*option == '-');
            break;
          }
        if (LocaleCompare("append",option+1) == 0)
          break;
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing password",option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        if (LocaleCompare("average",option+1) == 0)
          break;
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing background color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowConvertException(OptionError,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("border",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing border color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("box",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing color",option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            if (*option == '-')
              {
                unsigned long
                  maximum,
                  minimum;

                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing threshold",option);
                minimum=(~0);
                maximum=(~0);
                (void) sscanf(argv[i],"%lux%lu",&minimum,&maximum);
                SetCacheThreshold(minimum,maximum);
              }
            break;
          }
        if (LocaleCompare("channel",option+1) == 0)
          {
            if (*option == '-')
              {
                ChannelType
                  channel;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing type",option);
                option=argv[i];
                channel=UndefinedChannel;
                if (LocaleCompare("Red",option) == 0)
                  channel=RedChannel;
                if (LocaleCompare("Cyan",option) == 0)
                  channel=CyanChannel;
                if (LocaleCompare("Green",option) == 0)
                  channel=GreenChannel;
                if (LocaleCompare("Magenta",option) == 0)
                  channel=MagentaChannel;
                if (LocaleCompare("Blue",option) == 0)
                  channel=BlueChannel;
                if (LocaleCompare("Yellow",option) == 0)
                  channel=YellowChannel;
                if (LocaleCompare("Opacity",option) == 0)
                  channel=OpacityChannel;
                if (LocaleCompare("Black",option) == 0)
                  channel=BlackChannel;
                if (LocaleCompare("Matte",option) == 0)
                  channel=MatteChannel;
                if (channel == UndefinedChannel)
                  ThrowConvertException(OptionError,"Invalid channel type",
                    option);
              }
            break;
          }
        if (LocaleCompare("charcoal",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("chop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("clip",option+1) == 0)
          break;
        if (LocaleCompare("coalesce",option+1) == 0)
          break;
        if (LocaleCompare("colorize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing value",option);
              }
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing colors",option);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            if ((*option == '-') || (*option == '+'))
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing type",option);
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
                  ThrowConvertException(OptionError,"Invalid colorspace type",
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing comment",option);
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
                  ThrowConvertException(OptionError,"Missing type",option);
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
                  image_info->compression=RLECompression;
                if (LocaleCompare("Zip",option) == 0)
                  image_info->compression=ZipCompression;
                if (image_info->compression == UndefinedCompression)
                  ThrowConvertException(OptionError,"Invalid compression type",
                    option);
              }
            break;
          }
        if (LocaleCompare("contrast",option+1) == 0)
          break;
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("cycle",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing amount",option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'd':
      {
        if (LocaleCompare("deconstruct",option+1) == 0)
          break;
        if (LocaleCompare("debug",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing event mask",
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing delay",option);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
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
                  ThrowConvertException(OptionError,"Missing image depth",
                    option);
                image_info->depth=atol(argv[i]);
              }
            break;
          }
        if (LocaleCompare("despeckle",option+1) == 0)
          break;
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing server name",
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing method",option);
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  ThrowConvertException(OptionError,"Invalid dispose method",
                    option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            image_info->dither=(*option == '-');
            break;
          }
        if (LocaleCompare("draw",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing primitive",option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'e':
      {
        if (LocaleCompare("edge",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("emboss",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing encoding type",
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing type",option);
                option=argv[i];
                image_info->endian=UndefinedEndian;
                if (LocaleCompare("LSB",option) == 0)
                  image_info->endian=LSBEndian;
                if (LocaleCompare("MSB",option) == 0)
                  image_info->endian=MSBEndian;
                if (image_info->endian == UndefinedEndian)
                  ThrowConvertException(OptionError,"Invalid endian type",
                    option);
              }
            break;
          }
        if (LocaleCompare("enhance",option+1) == 0)
          break;
        if (LocaleCompare("equalize",option+1) == 0)
          break;
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'f':
      {
        if (LocaleCompare("fill",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing fill color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
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
                  ThrowConvertException(OptionError,"Missing type",option);
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
                  ThrowConvertException(OptionError,"Invalid filter type",
                    option);
              }
            break;
          }
        if (LocaleCompare("flatten",option+1) == 0)
          break;
        if (LocaleCompare("flip",option+1) == 0)
          break;
        if (LocaleCompare("flop",option+1) == 0)
          break;
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing font name",option);
                (void) CloneString(&image_info->font,argv[i]);
              }
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing format string",
                    option);
                format=argv[i];
              }
            break;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("fuzz",option+1) == 0)
          {
            image_info->fuzz=0.0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing distance",option);
                image_info->fuzz=StringToDouble(argv[i],MaxRGB);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowConvertException(OptionError,"Missing value",option);
            break;
          }
        if (LocaleCompare("gaussian",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowConvertException(OptionError,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
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
                  ThrowConvertException(OptionError,"Missing type",option);
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
                  ThrowConvertException(OptionError,"Invalid gravity type",
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          break;
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'i':
      {
        if (LocaleCompare("implode",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing amount",option);
              }
            break;
          }
        if (LocaleCompare("intent",option+1) == 0)
          {
            if (*option == '-')
              {
                RenderingIntent
                  rendering_intent;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing type",option);
                option=argv[i];
                rendering_intent=UndefinedIntent;
                if (LocaleCompare("Absolute",option) == 0)
                  rendering_intent=AbsoluteIntent;
                if (LocaleCompare("Perceptual",option) == 0)
                  rendering_intent=PerceptualIntent;
                if (LocaleCompare("Relative",option) == 0)
                  rendering_intent=RelativeIntent;
                if (LocaleCompare("Saturation",option) == 0)
                  rendering_intent=SaturationIntent;
                if (rendering_intent == UndefinedIntent)
                  ThrowConvertException(OptionError,"Invalid intent type",
                    option);
              }
            break;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=NoInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing type",option);
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
                  ThrowConvertException(OptionError,"Invalid interlace type",
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing label name",
                    option);
              }
            break;
          }
        if (LocaleCompare("level",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowConvertException(OptionError,"Missing value",option);
            break;
          }
        if (LocaleCompare("list",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing list name",option);
                option=argv[i];
                switch (*option)
                {
                  case 'C':
                  case 'c':
                  {
                    if (LocaleCompare("Color",option) == 0)
                      {
                        (void) ListColorInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,"Invalid list type",
                      option)
                  }
                  case 'D':
                  case 'd':
                  {
                    if (LocaleCompare("Delegate",option) == 0)
                      {
                        (void) ListDelegateInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,"Invalid list type",
                      option)
                  }
                  case 'F':
                  case 'f':
                  {
                    if (LocaleCompare("Format",option) == 0)
                      {
                        (void) ListMagickInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,"Invalid list type",
                      option)
                  }
                  case 'M':
                  case 'm':
                  {
                    if (LocaleCompare("Magic",option) == 0)
                      {
                        (void) ListMagicInfo((FILE *) NULL,exception);
                        break;
                      }
#if defined(HasMODULES)
                    if (LocaleCompare("Module",option) == 0)
                      {
                        (void) ListModuleInfo((FILE *) NULL,exception);
                        break;
                      }
#endif /* HasMODULES */
                    ThrowConvertException(OptionError,"Invalid list type",
                      option)
                  }
                  case 'T':
                  case 't':
                  {
                    if (LocaleCompare("Type",option) == 0)
                      {
                        (void) ListTypeInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,"Invalid list type",
                      option)
                  }
                  default:
                    ThrowConvertException(OptionError,"Invalid list type",
                      option)
                }
                return(True);
              }
            break;
          }
        if (LocaleCompare("loop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing iterations",
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'm':
      {
        if (LocaleCompare("map",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing file name",option);
              }
            break;
          }
        if (LocaleCompare("mask",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing file name",option);
              }
            break;
          }
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing matte color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("median",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("modulate",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing value",option);
              }
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            break;
          }
        if (LocaleCompare("morph",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing frames",option);
              }
            break;
          }
        if (LocaleCompare("mosaic",option+1) == 0)
          break;
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          break;
        if (LocaleCompare("noise",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing value",option);
              }
            if (*option == '+')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing type",option);
                option=argv[i];
                if ((LocaleCompare("Uniform",option) != 0) &&
                    (LocaleCompare("Gaussian",option) != 0) &&
                    (LocaleCompare("Multiplicative",option) != 0) &&
                    (LocaleCompare("Impulse",option) != 0) &&
                    (LocaleCompare("Laplacian",option) != 0) &&
                    (LocaleCompare("Poisson",option) != 0))
                  ThrowConvertException(OptionError,"Invalid noise type",
                    option);
              }
            break;
          }
        if (LocaleCompare("noop",option+1) == 0)
          break;
        if (LocaleCompare("normalize",option+1) == 0)
          break;
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'o':
      {
        if (LocaleCompare("opaque",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing opaque color",
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
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
                  ThrowConvertException(OptionError,"Missing page geometry",
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("paint",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("pen",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing pen color",option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
              }
            break;
          }
        if (LocaleCompare("ping",option+1) == 0)
          {
            ping=(*option == '-');
            break;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            image_info->pointsize=12;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing size",option);
                image_info->pointsize=atof(argv[i]);
              }
            break;
          }
        if (LocaleCompare("preview",option+1) == 0)
          {
            image_info->preview_type=UndefinedPreview;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing type",option);
                option=argv[i];
                image_info->preview_type=UndefinedPreview;
                if (LocaleCompare("Rotate",option) == 0)
                  image_info->preview_type=RotatePreview;
                if (LocaleCompare("Shear",option) == 0)
                  image_info->preview_type=ShearPreview;
                if (LocaleCompare("Roll",option) == 0)
                  image_info->preview_type=RollPreview;
                if (LocaleCompare("Hue",option) == 0)
                  image_info->preview_type=HuePreview;
                if (LocaleCompare("Saturation",option) == 0)
                  image_info->preview_type=SaturationPreview;
                if (LocaleCompare("Brightness",option) == 0)
                  image_info->preview_type=BrightnessPreview;
                if (LocaleCompare("Gamma",option) == 0)
                  image_info->preview_type=GammaPreview;
                if (LocaleCompare("Spiff",option) == 0)
                  image_info->preview_type=SpiffPreview;
                if (LocaleCompare("Dull",option) == 0)
                  image_info->preview_type=DullPreview;
                if (LocaleCompare("Grayscale",option) == 0)
                  image_info->preview_type=GrayscalePreview;
                if (LocaleCompare("Quantize",option) == 0)
                  image_info->preview_type=QuantizePreview;
                if (LocaleCompare("Despeckle",option) == 0)
                  image_info->preview_type=DespecklePreview;
                if (LocaleCompare("ReduceNoise",option) == 0)
                  image_info->preview_type=ReduceNoisePreview;
                if (LocaleCompare("AddNoise",option) == 0)
                  image_info->preview_type=AddNoisePreview;
                if (LocaleCompare("Sharpen",option) == 0)
                  image_info->preview_type=SharpenPreview;
                if (LocaleCompare("Blur",option) == 0)
                  image_info->preview_type=BlurPreview;
                if (LocaleCompare("Threshold",option) == 0)
                  image_info->preview_type=ThresholdPreview;
                if (LocaleCompare("EdgeDetect",option) == 0)
                  image_info->preview_type=EdgeDetectPreview;
                if (LocaleCompare("Spread",option) == 0)
                  image_info->preview_type=SpreadPreview;
                if (LocaleCompare("Shade",option) == 0)
                  image_info->preview_type=ShadePreview;
                if (LocaleCompare("Raise",option) == 0)
                  image_info->preview_type=RaisePreview;
                if (LocaleCompare("Segment",option) == 0)
                  image_info->preview_type=SegmentPreview;
                if (LocaleCompare("Solarize",option) == 0)
                  image_info->preview_type=SolarizePreview;
                if (LocaleCompare("Swirl",option) == 0)
                  image_info->preview_type=SwirlPreview;
                if (LocaleCompare("Implode",option) == 0)
                  image_info->preview_type=ImplodePreview;
                if (LocaleCompare("Wave",option) == 0)
                  image_info->preview_type=WavePreview;
                if (LocaleCompare("OilPaint",option) == 0)
                  image_info->preview_type=OilPaintPreview;
                if (LocaleCompare("CharcoalDrawing",option) == 0)
                  image_info->preview_type=CharcoalDrawingPreview;
                if (LocaleCompare("JPEG",option) == 0)
                  image_info->preview_type=JPEGPreview;
                if (image_info->preview_type == UndefinedPreview)
                  ThrowConvertException(OptionError,"Invalid interlace type",
                     option);
              }
            break;
          }
        if (LocaleCompare("process",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing argument",option);
              }
            break;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,"Missing profile",option);
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
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
                  ThrowConvertException(OptionError,"Missing quality",option);
                image_info->quality=atol(argv[i]);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'r':
      {
        if (LocaleCompare("raise",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowConvertException(OptionError,"Missing bevel width",option);
            break;
          }
        if (LocaleCompare("region",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          break;
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("roll",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,"Missing degrees",option);
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 's':
      {
        if (LocaleCompare("sample",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("sampling_factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
              }
            break;
          }
        if (LocaleCompare("scale",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing scene number",
                    option);
              }
            break;
          }
        if (LocaleCompare("seed",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing value",option);
              }
            srand(atoi(argv[i]));
            break;
          }
        if (LocaleCompare("segment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing threshold",option);
              }
            break;
          }
        if (LocaleCompare("shade",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowConvertException(OptionError,"Missing azimuth",option);
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowConvertException(OptionError,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("shave",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("shear",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowConvertException(OptionError,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,"Missing geometry",option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("solarize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing threshold",option);
              }
            break;
          }
        if (LocaleCompare("spread",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing amount",option);
              }
            break;
          }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing color",option);
              }
            break;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing size",option);
              }
            break;
          }
        if (LocaleCompare("swirl",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing degrees",option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 't':
      {
        if (LocaleCompare("texture",option+1) == 0)
          {
            (void) CloneString(&image_info->texture,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing filename",option);
                (void) CloneString(&image_info->texture,argv[i]);
              }
            break;
          }
        if (LocaleCompare("threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing value",option);
              }
            break;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,"Missing tile",option);
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          break;
        if (LocaleCompare("transparent",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing transparent color",
                    option);
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,"Missing depth",option);
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
                  ThrowConvertException(OptionError,"Missing type",option);
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
                  ThrowConvertException(OptionError,"Invalid image type",
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'u':
      {
        if (LocaleCompare("undercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing color",option);
              }
            break;
          }
        if (LocaleCompare("units",option+1) == 0)
          {
            image_info->units=UndefinedResolution;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing type",option);
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
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,"Missing geometry",option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          {
            (void) fprintf(stdout,"Version: %.1024s\n",
              GetMagickVersion((unsigned long *) NULL));
            (void) fprintf(stdout,"Copyright: %.1024s\n\n",
              GetMagickCopyright());
            break;
          }
        if (LocaleCompare("view",option+1) == 0)
          {
            (void) CloneString(&image_info->view,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,"Missing view transform",
                    option);
                (void) CloneString(&image_info->view,argv[i]);
              }
            break;
          }
        if (LocaleCompare("virtual_pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,
                    "Missing virtual pixel method",option);
                option=argv[i];
                virtual_pixel_method=UndefinedVirtualPixelMethod;
                if (LocaleCompare("Constant",option) == 0)
                  virtual_pixel_method=ConstantVirtualPixelMethod;
                if (LocaleCompare("Edge",option) == 0)
                  virtual_pixel_method=EdgeVirtualPixelMethod;
                if (LocaleCompare("Mirror",option) == 0)
                  virtual_pixel_method=MirrorVirtualPixelMethod;
                if (LocaleCompare("Tile",option) == 0)
                  virtual_pixel_method=TileVirtualPixelMethod;
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowConvertException(OptionError,
                    "Invalid virtual pixel method",option);
              }
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case 'w':
      {
        if (LocaleCompare("wave",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowConvertException(OptionError,"Missing amplitude",option);
            break;
          }
        if (LocaleCompare("write",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,"Missing filename",option);
            break;
          }
        ThrowConvertException(OptionError,"Unrecognized option",option)
      }
      case '?':
        break;
      default:
        ThrowConvertException(OptionError,"Unrecognized option",option)
    }
  }
  if ((image == (Image *) NULL) && (image_list == (Image *) NULL))
    {
      if (exception->severity == UndefinedException)
        ThrowConvertException(OptionError,"Missing an image file name",
          (char *) NULL);
      return(False);
    }
  if (i != (argc-1))
    ThrowConvertException(OptionError,"Missing an image file name",
      (char *) NULL);
  if (image == (Image *) NULL)
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image_list);
      (void) CatchImageException(image_list);
    }
  else
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image);
      (void) CatchImageException(image);
      PushImageList(&image_list,image,exception);
      DestroyImageList(image);
    }
  status&=WriteImages(image_info,image_list,argv[argc-1],exception);
  if (metadata != (char **) NULL)
    {
      char
        *text;

      text=TranslateText(image_info,image_list,format);
      if (text == (char *) NULL)
        ThrowConvertException(ResourceLimitError,
          "Unable to format image metadata","Memory allocation failed");
      (void) ConcatenateString(&(*metadata),text);
      (void) ConcatenateString(&(*metadata),"\n");
    }
  DestroyImageList(image_list);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I d e n t i f y I m a g e C o m m a n d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IdentifyImageCommand() describes the format and characteristics of one or
%  more image files. It will also report if an image is incomplete or corrupt.
%  The information displayed includes the scene number, the file name, the
%  width and height of the image, whether the image is colormapped or not,
%  the number of colors in the image, the number of bytes in the image, the
%  format of the image (JPEG, PNM, etc.), and finally the number of seconds
%  it took to read and process the image.
%
%  The format of the IdentifyImageCommand method is:
%
%      unsigned int IdentifyImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IdentifyImageCommand(ImageInfo *image_info,
  const int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
#define ThrowIdentifyException(code,reason,description) \
{ \
  if (format != (char *) NULL) \
    LiberateMemory((void **) &format); \
  DestroyImageList(image); \
  ThrowException(exception,code,reason,description); \
  return(False); \
}

  char
    *format,
    *option,
    *q;

  Image
    *image;

  long
    count,
    number_images,
    x;

  register Image
    *p;

  register long
    i;

  unsigned int
    ping,
    status;

  /*
    Set defaults.
  */
  count=0;
  format=(char *) NULL;
  image=(Image *) NULL;
  number_images=0;
  status=True;
  ping=True;
  /*
    Identify an image.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if (LocaleCompare("-format",argv[i]) == 0)
      {
        i++;
        if (i == argc)
          ThrowIdentifyException(OptionError,"Missing format string",option);
        (void) CloneString(&format,argv[i]);
        break;
      }
  }
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
                ping=False;
                break;
              }
        if (image_info->verbose || !ping)
          image=ReadImage(image_info,exception);
        else
          image=PingImage(image_info,exception);
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
          if (metadata != (char **) NULL)
            {
              char
                *text;

              text=TranslateText(image_info,p,format);
              if (text == (char *) NULL)
                ThrowIdentifyException(ResourceLimitError,
                  "Unable to format image metadata","Memory allocation failed");
              (void) ConcatenateString(&(*metadata),text);
              (void) ConcatenateString(&(*metadata),"\n");
            }
        }
        DestroyImageList(image);
        image=(Image *) NULL;
        number_images++;
        continue;
      }
    switch(*(option+1))
    {
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            if (*option == '-')
              {
                unsigned long
                  maximum,
                  minimum;

                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowIdentifyException(OptionError,"Missing threshold",
                    option);
                minimum=(~0);
                maximum=(~0);
                (void) sscanf(argv[i],"%lux%lu",&minimum,&maximum);
                SetCacheThreshold(minimum,maximum);
              }
            break;
          }
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,"Missing event mask",
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
              i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowIdentifyException(OptionError,"Missing geometry",option);
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
                  ThrowIdentifyException(OptionError,"Missing image depth",
                    option);
                image_info->depth=atol(argv[i]);
              }
            break;
          }
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
      }
      case 'f':
      {
        if (LocaleCompare("format",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,"Missing format string",
                    option);
              }
            break;
          }
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          break;
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
      }
      case 'i':
      {
        if (LocaleCompare("interlace",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,"Missing type",option);
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
                  ThrowIdentifyException(OptionError,"Invalid interlace type",
                    option);
              }
            break;
          }
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
      }
      case 'p':
      {
        if (LocaleCompare("ping",option+1) == 0)
          break;  /* default is ping; silently ignore */
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
      }
      case 's':
      {
        if (LocaleCompare("sampling_factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowIdentifyException(OptionError,"Missing geometry",option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
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
                  ThrowIdentifyException(OptionError,"Missing geometry",option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          {
            (void) fprintf(stdout,"Version: %.1024s\n",
              GetMagickVersion((unsigned long *) NULL));
            (void) fprintf(stdout,"Copyright: %.1024s\n\n",
              GetMagickCopyright());
            break;
          }
        if (LocaleCompare("virtual_pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,
                    "Missing virtual pixel method",option);
                option=argv[i];
                virtual_pixel_method=UndefinedVirtualPixelMethod;
                if (LocaleCompare("Constant",option) == 0)
                  virtual_pixel_method=ConstantVirtualPixelMethod;
                if (LocaleCompare("Edge",option) == 0)
                  virtual_pixel_method=EdgeVirtualPixelMethod;
                if (LocaleCompare("Mirror",option) == 0)
                  virtual_pixel_method=MirrorVirtualPixelMethod;
                if (LocaleCompare("Tile",option) == 0)
                  virtual_pixel_method=TileVirtualPixelMethod;
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowIdentifyException(OptionError,
                    "Invalid virtual pixel method",option);
              }
            break;
          }
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
      }
      case '?':
        break;
      default:
        ThrowIdentifyException(OptionError,"Unrecognized option",option)
    }
  }
  if (number_images == 0)
    {
      if (exception->severity == UndefinedException)
        ThrowIdentifyException(OptionError,"Missing an image file name",
          (char *) NULL);
      return(False);
    }
  if (i != argc)
    ThrowIdentifyException(OptionError,"Missing an image file name",
      (char *) NULL);
  if (format != (char *) NULL)
    LiberateMemory((void **) &format);
  DestroyImageList(image);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M o g r i f y I m a g e C o m m a n d                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MogrifyImageCommand() transforms an image or a sequence of images. These
%  transforms include image scaling, image rotation, color reduction, and
%  others. The transmogrified image overwrites the original image.
%
%  The format of the MogrifyImageCommand method is:
%
%      unsigned int MogrifyImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int MogrifyImageCommand(ImageInfo *image_info,
  const int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
#define ThrowMogrifyException(code,reason,description) \
{ \
  ThrowException(exception,code,reason,description); \
  return(False); \
}

  char
    filename[MaxTextExtent],
    *format,
    *option;

  double
    sans;

  Image
    *image;

  long
    j,
    k,
    x;

  register long
    i;

  unsigned int
    global_colormap,
    status;

  /*
    Set defaults.
  */
  format=(char *) NULL;
  global_colormap=False;
  image=(Image *) NULL;
  status=True;
  /*
    Parse command line.
  */
  j=1;
  k=0;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        /*
          Option is a file name: begin by reading image from specified file.
        */
        k=i;
        (void) strncpy(image_info->filename,argv[i],MaxTextExtent-1);
        image=ReadImage(image_info,exception);
        status&=(image != (Image *) NULL) &&
          (exception->severity < ErrorException);
        if (image == (Image *) NULL)
          continue;
        if (format != (char *) NULL)
          {
            register char
              *p;

            /*
              Modify filename to include a user specified image format.
            */
            p=image->filename+strlen(image->filename)-1;
            while ((*p != '.') && (p > (image->filename+1)))
              p--;
            p++;
            if (LocaleCompare(p,image->magick) == 0)
              (void) strncpy(p,format,MaxTextExtent-(p-image->filename)-1);
            else
              {
                FormatString(image_info->filename,"%.1024s:%.1024s",format,
                  image->filename);
                (void) strncpy(image->filename,image_info->filename,
                  MaxTextExtent-1);
              }
          }
        /*
          Transmogrify image as defined by the image processing options.
        */
        status&=MogrifyImages(image_info,i-j,argv+j,&image);
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
              (void) strncpy(filename,image->filename,MaxTextExtent-1);
              AppendImageFormat("mgk",image->filename);
              if (IsAccessible(image->filename))
                {
                  (void) strcat(image->filename,"~");
                  if (IsAccessible(image->filename))
                    ThrowMogrifyException(FileOpenFatalError,
                      "Unable to create temporary file",filename);
                }
            }
        status&=WriteImages(image_info,image,image->filename,&image->exception);
        if ((status != False) && (LocaleCompare(image_info->filename,"-") != 0))
          {
            (void) remove(filename);
            (void) rename(image->filename,filename);
          }
        DestroyImage(image);
        continue;
      }
    j=k+1;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("affine",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionWarning,"Missing matrix",option);
              }
            break;
          }
        if (LocaleCompare("antialias",option+1) == 0)
          {
            image_info->antialias=(*option == '-');
            break;
          }
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,"Missing password",option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,
                    "Missing background color",option);
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowMogrifyException(OptionWarning,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("border",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing border color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("box",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing color",option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            if (*option == '-')
              {
                unsigned long
                  maximum,
                  minimum;

                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,"Missing threshold",option);
                minimum=(~0);
                maximum=(~0);
                (void) sscanf(argv[i],"%lux%lu",&minimum,&maximum);
                SetCacheThreshold(minimum,maximum);
              }
            break;
          }
        if (LocaleCompare("channel",option+1) == 0)
          {
            if (*option == '-')
              {
                ChannelType
                  channel;

                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
                option=argv[i];
                channel=UndefinedChannel;
               if (LocaleCompare("Red",option) == 0)
                 channel=RedChannel;
               if (LocaleCompare("Cyan",option) == 0)
                 channel=CyanChannel;
               if (LocaleCompare("Green",option) == 0)
                 channel=GreenChannel;
               if (LocaleCompare("Magenta",option) == 0)
                 channel=MagentaChannel;
               if (LocaleCompare("Blue",option) == 0)
                 channel=BlueChannel;
               if (LocaleCompare("Yellow",option) == 0)
                 channel=YellowChannel;
               if (LocaleCompare("Opacity",option) == 0)
                 channel=OpacityChannel;
               if (LocaleCompare("Black",option) == 0)
                 channel=BlackChannel;
               if (LocaleCompare("Matte",option) == 0)
                 channel=MatteChannel;
                if (channel == UndefinedChannel)
                  ThrowMogrifyException(OptionWarning,"Invalid channel type",
                    option);
              }
            break;
          }
        if (LocaleCompare("charcoal",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionWarning,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("chop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("colorize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing value",option);
              }
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing colors",option);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
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
                  ThrowMogrifyException(OptionWarning,"Invalid colorspace type",
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing comment",option);
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
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
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
                  image_info->compression=RLECompression;
                if (LocaleCompare("Zip",option) == 0)
                  image_info->compression=ZipCompression;
                if (image_info->compression == UndefinedCompression)
                  ThrowMogrifyException(OptionWarning,
                    "Invalid compression type",option);
              }
            break;
          }
        if (LocaleCompare("contrast",option+1) == 0)
          break;
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("cycle",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing amount",option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,"Missing event mask",
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing delay",option);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
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
                  ThrowMogrifyException(OptionWarning,"Missing image depth",
                    option);
                image_info->depth=atol(argv[i]);
              }
            break;
          }
        if (LocaleCompare("despeckle",option+1) == 0)
          break;
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing server name",
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing method",option);
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  ThrowMogrifyException(OptionError,"Invalid dispose method",
                    option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            image_info->dither=(*option == '-');
            break;
          }
        if (LocaleCompare("draw",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing primitive",
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'e':
      {
        if (LocaleCompare("edge",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionWarning,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("emboss",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing encoding type",
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
                option=argv[i];
                image_info->endian=UndefinedEndian;
                if (LocaleCompare("LSB",option) == 0)
                  image_info->endian=LSBEndian;
                if (LocaleCompare("MSB",option) == 0)
                  image_info->endian=MSBEndian;
                if (image_info->endian == UndefinedEndian)
                  ThrowMogrifyException(OptionWarning,"Invalid endian type",
                    option);
              }
            break;
          }
        if (LocaleCompare("enhance",option+1) == 0)
          break;
        if (LocaleCompare("equalize",option+1) == 0)
          break;
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'f':
      {
        if (LocaleCompare("fill",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing fill color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
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
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
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
                  ThrowMogrifyException(OptionWarning,"Invalid filter type",
                    option);
              }
            break;
          }
        if (LocaleCompare("flip",option+1) == 0)
          break;
        if (LocaleCompare("flop",option+1) == 0)
          break;
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing font name",
                    option);
                (void) CloneString(&image_info->font,argv[i]);
              }
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            (void) CloneString(&format,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing image type",
                    option);
                (void) CloneString(&format,argv[i]);
                (void) strncpy(image_info->filename,format,MaxTextExtent-1);
                (void) strcat(image_info->filename,":");
                (void) SetImageInfo(image_info,False,exception);
                if (*image_info->magick == '\0')
                  ThrowMogrifyException(OptionWarning,
                    "Unrecognized image format",format);
              }
            break;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("fuzz",option+1) == 0)
          {
            image_info->fuzz=0.0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing distance",
                    option);
                image_info->fuzz=StringToDouble(argv[i],MaxRGB);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowMogrifyException(OptionWarning,"Missing value",option);
            break;
          }
        if (LocaleCompare("gaussian",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowMogrifyException(OptionWarning,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
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
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
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
                  ThrowMogrifyException(OptionWarning,"Invalid gravity type",
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          break;
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'i':
      {
        if (LocaleCompare("implode",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionWarning,"Missing amount",option);
              }
            break;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=NoInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
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
                  ThrowMogrifyException(OptionWarning,"Invalid interlace type",
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing label name",
                    option);
              }
            break;
          }
        if (LocaleCompare("level",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowMogrifyException(OptionWarning,"Missing value",option);
            break;
          }
        if (LocaleCompare("linewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing size",option);
              }
            break;
          }
        if (LocaleCompare("list",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing list name",
                    option);
                option=argv[i];
                switch (*option)
                {
                  case 'C':
                  case 'c':
                  {
                    if (LocaleCompare("Color",option) == 0)
                      {
                        (void) ListColorInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionWarning,"Invalid list type",
                      option)
                  }
                  case 'D':
                  case 'd':
                  {
                    if (LocaleCompare("Delegate",option) == 0)
                      {
                        (void) ListDelegateInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionWarning,"Invalid list type",
                      option)
                  }
                  case 'F':
                  case 'f':
                  {
                    if (LocaleCompare("Format",option) == 0)
                      {
                        (void) ListMagickInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionWarning,"Invalid list type",
                      option)
                  }
                  case 'M':
                  case 'm':
                  {
                    if (LocaleCompare("Magic",option) == 0)
                      {
                        (void) ListMagicInfo((FILE *) NULL,exception);
                        break;
                      }
#if defined(HasMODULES)
                    if (LocaleCompare("Module",option) == 0)
                      {
                        (void) ListModuleInfo((FILE *) NULL,exception);
                        break;
                      }
#endif /* HasMODULES */
                    ThrowMogrifyException(OptionWarning,"Invalid list type",
                      option)
                  }
                  case 'T':
                  case 't':
                  {
                    if (LocaleCompare("Type",option) == 0)
                      {
                        (void) ListTypeInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionWarning,"Invalid list type",
                      option)
                  }
                  default:
                    ThrowMogrifyException(OptionWarning,"Invalid list type",
                      option)
                }
                return(True);
              }
            break;
          }
        if (LocaleCompare("loop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing iterations",
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'm':
      {
        if (LocaleCompare("map",option+1) == 0)
          {
            global_colormap=(*option == '+');
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing file name",
                    option);
              }
            break;
          }
        if (LocaleCompare("mask",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing file name",
                    option);
              }
            break;
          }
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing matte color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("modulate",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionWarning,"Missing value",option);
              }
            break;
          }
        if (LocaleCompare("median",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          break;
        if (LocaleCompare("noise",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionWarning,"Missing value",option);
              }
            if (*option == '+')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
                option=argv[i];
                if ((LocaleCompare("Uniform",option) != 0) &&
                    (LocaleCompare("Gaussian",option) != 0) &&
                    (LocaleCompare("Multiplicative",option) != 0) &&
                    (LocaleCompare("Impulse",option) != 0) &&
                    (LocaleCompare("Laplacian",option) != 0) &&
                    (LocaleCompare("Poisson",option) != 0))
                  ThrowMogrifyException(OptionWarning,"Invalid noise type",
                    option);
              }
            break;
          }
        if (LocaleCompare("noop",option+1) == 0)
          break;
        if (LocaleCompare("normalize",option+1) == 0)
          break;
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'o':
      {
        if (LocaleCompare("opaque",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing opaque color",
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
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
                  ThrowMogrifyException(OptionWarning,"Missing page geometry",
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("paint",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing radius",option);
              }
            break;
          }
        if (LocaleCompare("pen",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing fill color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
              }
            break;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            image_info->pointsize=12;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing size",option);
                image_info->pointsize=atof(argv[i]);
              }
            break;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMogrifyException(OptionWarning,"Missing profile",option);
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
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
                  ThrowMogrifyException(OptionWarning,"Missing quality",option);
                image_info->quality=atol(argv[i]);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'r':
      {
        if (LocaleCompare("raise",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowMogrifyException(OptionWarning,"Missing bevel width",option);
            break;
          }
        if (LocaleCompare("region",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          break;
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("roll",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionWarning,"Missing degrees",option);
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 's':
      {
        if (LocaleCompare("sample",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("sampling_factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
              }
            break;
          }
        if (LocaleCompare("scale",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing scene number",
                    option);
              }
            break;
          }
        if (LocaleCompare("seed",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing value",option);
              }
            srand(atoi(argv[i]));
            break;
          }
        if (LocaleCompare("segment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionWarning,"Missing threshold",
                    option);
              }
            break;
          }
        if (LocaleCompare("shade",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowMogrifyException(OptionWarning,"Missing azimuth",option);
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowMogrifyException(OptionWarning,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("shave",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
              }
            break;
          }
        if (LocaleCompare("shear",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowMogrifyException(OptionWarning,"Missing geometry",option);
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionWarning,"Missing geometry",
                    option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("solarize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing threshold",
                    option);
              }
            break;
          }
        if (LocaleCompare("spread",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing amount",option);
              }
            break;
          }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing color",option);
              }
            break;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing size",option);
              }
            break;
          }
        if (LocaleCompare("swirl",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionWarning,"Missing degrees",option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 't':
      {
        if (LocaleCompare("texture",option+1) == 0)
          {
            (void) CloneString(&image_info->texture,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing filename",
                    option);
                (void) CloneString(&image_info->texture,argv[i]);
              }
            break;
          }
        if (LocaleCompare("threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing value",option);
              }
            break;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMogrifyException(OptionWarning,"Missing tile",option);
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          break;
        if (LocaleCompare("transparent",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,
                    "Missing transparent color",option);
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionWarning,"Missing depth",option);
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
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
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
                  ThrowMogrifyException(OptionWarning,"Invalid image type",
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'u':
      {
        if (LocaleCompare("undercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing color",option);
              }
            break;
          }
        if (LocaleCompare("units",option+1) == 0)
          {
            image_info->units=UndefinedResolution;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing type",option);
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
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowMogrifyException(OptionWarning,"Missing geometry",option);
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          {
            (void) fprintf(stdout,"Version: %.1024s\n",
              GetMagickVersion((unsigned long *) NULL));
            (void) fprintf(stdout,"Copyright: %.1024s\n\n",
              GetMagickCopyright());
            break;
          }
        if (LocaleCompare("view",option+1) == 0)
          {
            (void) CloneString(&image_info->view,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionWarning,"Missing view transform",
                    option);
                (void) CloneString(&image_info->view,argv[i]);
              }
            break;
          }
        if (LocaleCompare("virtual_pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,
                    "Missing virtual pixel method",option);
                option=argv[i];
                virtual_pixel_method=UndefinedVirtualPixelMethod;
                if (LocaleCompare("Constant",option) == 0)
                  virtual_pixel_method=ConstantVirtualPixelMethod;
                if (LocaleCompare("Edge",option) == 0)
                  virtual_pixel_method=EdgeVirtualPixelMethod;
                if (LocaleCompare("Mirror",option) == 0)
                  virtual_pixel_method=MirrorVirtualPixelMethod;
                if (LocaleCompare("Tile",option) == 0)
                  virtual_pixel_method=TileVirtualPixelMethod;
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowMogrifyException(OptionError,
                    "Invalid virtual pixel method",option);
              }
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case 'w':
      {
        if (LocaleCompare("wave",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              ThrowMogrifyException(OptionWarning,"Missing amplitude",option);
            break;
          }
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
      }
      case '?':
        break;
      default:
        ThrowMogrifyException(OptionWarning,"Unrecognized option",option)
    }
  }
  if ((i != argc) || (image == (Image *) NULL))
    ThrowMogrifyException(OptionWarning,"Missing an image file name",
      (char *) NULL);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M o n t a g e I m a g e C o m m a n d                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MontageImageCommand() reads one or more images, applies one or more image
%  processing operations, and writes out the image in the same or
%  differing format.
%
%  The format of the MontageImageCommand method is:
%
%      unsigned int MontageImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int MontageImageCommand(ImageInfo *image_info,
  const int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
#define ThrowMontageException(code,reason,description) \
{ \
  DestroyImageList(image); \
  DestroyImageList(image_list); \
  DestroyImageList(montage_image); \
  ThrowException(exception,code,reason,description); \
  return(False); \
}
  char
    *format,
    *option,
    *transparent_color;

  double
    sans;

  Image
    *image,
    *image_list,
    *montage_image,
    *next_image;

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
  format="%w,%h,%m";
  first_scene=0;
  image=(Image *) NULL;
  image_list=(Image *) NULL;
  last_scene=0;
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  (void) SetImageInfo(image_info,True,exception);
  montage_image=(Image *) NULL;
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
                FormatString(filename,"%.1024s.%lu",image_info->filename,scene);
              (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
            }
          (void) CloneString(&image_info->font,montage_info->font);
          image_info->colorspace=quantize_info.colorspace;
          image_info->dither=quantize_info.dither;
          if (image_info->size == (char *) NULL)
            (void) CloneString(&image_info->size,montage_info->geometry);
          next_image=ReadImage(image_info,exception);
          status&=(next_image != (Image *) NULL) &&
            (exception->severity < ErrorException);
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
    if ((image != (Image *) NULL) && (j != (k+1)))
      {
        status&=MogrifyImages(image_info,i-j,argv+j,&image);
        (void) CatchImageException(image);
        PushImageList(&image_list,image,exception);
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
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing password",option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing color",option);
                (void) QueryColorDatabase(argv[i],
                  &montage_info->background_color,exception);
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing color",option);
                (void) QueryColorDatabase(argv[i],&montage_info->border_color,
                  exception);
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
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
                  ThrowMontageException(OptionError,"Missing width",option);
                montage_info->border_width=atol(argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            if (*option == '-')
              {
                unsigned long
                  maximum,
                  minimum;

                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,"Missing threshold",option);
                minimum=(~0);
                maximum=(~0);
                (void) sscanf(argv[i],"%lux%lu",&minimum,&maximum);
                SetCacheThreshold(minimum,maximum);
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
                  ThrowMontageException(OptionError,"Missing colors",option);
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
                  ThrowMontageException(OptionError,"Missing type",option);
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
                  ThrowMontageException(OptionError,"Invalid colorspace type",
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing comment",option);
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
                  ThrowMontageException(OptionError,"Missing type",option);
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
                  image_info->compression=RLECompression;
                if (LocaleCompare("Zip",option) == 0)
                  image_info->compression=ZipCompression;
                if (image_info->compression == UndefinedCompression)
                  ThrowMontageException(OptionError,"Invalid compression type",
                    option);
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
                  ThrowMontageException(OptionError,"Missing type",option);
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
                  ThrowMontageException(OptionError,"Invalid compose type",
                    option);
              }
            break;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,"Missing geometry",option);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing event mask",
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,"Missing geometry",option);
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
                  ThrowMontageException(OptionError,"Missing image depth",
                    option);
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
                  ThrowMontageException(OptionError,"Missing server name",
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing method",option);
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  ThrowMontageException(OptionError,"Invalid dispose method",
                    option);
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
                  ThrowMontageException(OptionError,"Missing primitive",option);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'e':
      {
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing encoding type",
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing type",option);
                option=argv[i];
                image_info->endian=UndefinedEndian;
                if (LocaleCompare("LSB",option) == 0)
                  image_info->endian=LSBEndian;
                if (LocaleCompare("MSB",option) == 0)
                  image_info->endian=MSBEndian;
                if (image_info->endian == UndefinedEndian)
                  ThrowMontageException(OptionError,"Invalid endian type",
                    option);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'f':
      {
        if (LocaleCompare("fill",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            (void) QueryColorDatabase("none",&montage_info->fill,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing fill color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
                (void) QueryColorDatabase(argv[i],&montage_info->fill,
                  exception);
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
                  ThrowMontageException(OptionError,"Missing type",option);
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
                  ThrowMontageException(OptionError,"Invalid filter type",
                    option);
              }
            break;
          }
        if (LocaleCompare("flip",option+1) == 0)
          break;
        if (LocaleCompare("flop",option+1) == 0)
          break;
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing font name",option);
                (void) CloneString(&image_info->font,argv[i]);
                (void) CloneString(&montage_info->font,argv[i]);
              }
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing format string",
                    option);
                format=argv[i];
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
                  ThrowMontageException(OptionError,"Missing geometry",option);
                (void) CloneString(&montage_info->frame,argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowMontageException(OptionError,"Missing value",option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            (void) CloneString(&montage_info->geometry,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,"Missing geometry",option);
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
                  ThrowMontageException(OptionError,"Missing type",option);
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
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          break;
        ThrowMontageException(OptionError,"Unrecognized option",option)
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
                  ThrowMontageException(OptionError,"Missing type",option);
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
                  ThrowMontageException(OptionError,"Invalid interlace type",
                    option);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing label name",
                    option);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
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
                  ThrowMontageException(OptionError,"Missing color",option);
                (void) QueryColorDatabase(argv[i],&montage_info->matte_color,
                  exception);
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  exception);
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
                  ThrowMontageException(OptionError,"Missing type",option);
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
                  ThrowMontageException(OptionError,"Invalid mode type",option);
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
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'n':
      {
        if (LocaleCompare("noop",option+1) == 0)
          break;
        ThrowMontageException(OptionError,"Unrecognized option",option)
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
                  ThrowMontageException(OptionError,"Missing page geometry",
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("pen",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing pen color",option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
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
                  ThrowMontageException(OptionError,"Missing size",option);
                image_info->pointsize=atof(argv[i]);
                montage_info->pointsize=atof(argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
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
                  ThrowMontageException(OptionError,"Missing quality",option);
                image_info->quality=atol(argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'r':
      {
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          break;
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMontageException(OptionError,"Missing degrees",option);
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 's':
      {
        if (LocaleCompare("sampling_factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,"Missing geometry",option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
              }
            break;
          }
        if (LocaleCompare("scenes",option+1) == 0)
          {
            first_scene=0;
            last_scene=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,"Missing scene number",
                    option);
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
                  ThrowMontageException(OptionError,"Missing geometry",option);
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
                  ThrowMontageException(OptionError,"Missing geometry",option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&montage_info->stroke,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing color",option);
                (void) QueryColorDatabase(argv[i],&montage_info->stroke,
                  exception);
              }
            break;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,"Missing size",option);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
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
                  ThrowMontageException(OptionError,"Missing filename",option);
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
                  ThrowMontageException(OptionError,"Missing geometry",option);
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
                  ThrowMontageException(OptionError,"Missing title",option);
                (void) CloneString(&montage_info->title,argv[i]);
              }
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          break;
        if (LocaleCompare("transparent",option+1) == 0)
          {
            transparent_color=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,"Missing color",option);
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
                  ThrowMontageException(OptionError,"Missing depth",option);
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
                  ThrowMontageException(OptionError,"Missing type",option);
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
                  ThrowMontageException(OptionError,"Invalid image type",
                    option);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          {
            (void) fprintf(stdout,"Version: %.1024s\n",
              GetMagickVersion((unsigned long *) NULL));
            (void) fprintf(stdout,"Copyright: %.1024s\n\n",
              GetMagickCopyright());
            break;
          }
        if (LocaleCompare("virtual_pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,
                    "Missing virtual pixel method",option);
                option=argv[i];
                virtual_pixel_method=UndefinedVirtualPixelMethod;
                if (LocaleCompare("Constant",option) == 0)
                  virtual_pixel_method=ConstantVirtualPixelMethod;
                if (LocaleCompare("Edge",option) == 0)
                  virtual_pixel_method=EdgeVirtualPixelMethod;
                if (LocaleCompare("Mirror",option) == 0)
                  virtual_pixel_method=MirrorVirtualPixelMethod;
                if (LocaleCompare("Tile",option) == 0)
                  virtual_pixel_method=TileVirtualPixelMethod;
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowMontageException(OptionError,
                    "Invalid virtual pixel method",option);
              }
            break;
          }
        ThrowMontageException(OptionError,"Unrecognized option",option)
      }
      case '?':
        break;
      default:
        ThrowMontageException(OptionError,"Unrecognized option",option)
    }
  }
  if ((image == (Image *) NULL) && (image_list == (Image *) NULL))
    {
      if (exception->severity == UndefinedException)
        ThrowConvertException(OptionError,"Missing an image file name",
          (char *) NULL);
      return(False);
    }
  if (i != (argc-1))
    ThrowMontageException(OptionError,"Missing an image file name",
      (char *) NULL);
  if (image != (Image *) NULL)
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image);
      (void) CatchImageException(image);
      PushImageList(&image_list,image,exception);
      DestroyImageList(image);
      image=(Image *) NULL;
      j=i;
    }
  (void) strncpy(montage_info->filename,argv[argc-1],MaxTextExtent-1);
  montage_image=MontageImages(image_list,montage_info,exception);
  if (montage_image == (Image *) NULL)
    ThrowMontageException(OptionError,"Missing an image file name",
      (char *) NULL);
  DestroyImageList(image_list);
  /*
    Write image.
  */
  status&=MogrifyImages(image_info,i-j,argv+j,&montage_image);
  (void) CatchImageException(montage_image);
  (void) strncpy(image_info->filename,argv[argc-1],MaxTextExtent-1);
  (void) strncpy(montage_image->magick_filename,argv[argc-1],MaxTextExtent-1);
  status&=WriteImages(image_info,montage_image,argv[argc-1],&image->exception);
  if (metadata != (char **) NULL)
    {
      char
        *text;

      text=TranslateText(image_info,montage_image,format);
      if (text == (char *) NULL)
        ThrowMontageException(ResourceLimitError,
          "Unable to format image metadata","Memory allocation failed");
      (void) ConcatenateString(&(*metadata),text);
      (void) ConcatenateString(&(*metadata),"\n");
    }
  DestroyImageList(montage_image);
  DestroyMontageInfo(montage_info);
  return(status);
}
