/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              PPPP   RRRR   EEEEE  V   V  IIIII  EEEEE  W   W                %
%              P   P  R   R  E      V   V    I    E      W   W                %
%              PPPP   RRRR   EEE    V   V    I    EEE    W   W                %
%              P      R R    E       V V     I    E      W W W                %
%              P      R  R   EEEEE    V    IIIII  EEEEE   W W                 %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
  Constant declarations.
*/
static const char
  *DefaultPreviewGeometry = "204x204+10+10";

/*
  Forward declarations.
*/
static unsigned int
  WritePreviewImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P R E V I E W I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPREVIEWImage adds attributes for the Preview image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPREVIEWImage method is:
%
%      RegisterPREVIEWImage(void)
%
*/
ModuleExport void RegisterPREVIEWImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PREVIEW");
  entry->encoder=WritePreviewImage;
  entry->adjoin=False;
  entry->description=
    AcquireString("Show a preview an image enhancement, effect, or f/x");
  entry->module=AcquireString("PREVIEW");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P R E V I E W I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPREVIEWImage removes format registrations made by the
%  PREVIEW module from the list of supported formats.
%
%  The format of the UnregisterPREVIEWImage method is:
%
%      UnregisterPREVIEWImage(void)
%
*/
ModuleExport void UnregisterPREVIEWImage(void)
{
  (void) UnregisterMagickInfo("PREVIEW");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P R E V I E W I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePreviewImage creates several tiles each with a varying
%  stength of an image enhancement function (e.g. gamma).  The image is written
%  in the MIFF format.
%
%  The format of the WritePreviewImage method is:
%
%      unsigned int WritePreviewImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePreviewImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WritePreviewImage(const ImageInfo *image_info,Image *image)
{
#define NumberTiles  9
#define PreviewImageText  "  Creating image preview...  "

  char
    *commands[NumberTiles+6],
    factor[MaxTextExtent],
    label[MaxTextExtent];

  float
    degrees,
    gamma,
    percentage,
    radius,
    sigma,
    threshold;

  Image
		*images,
    *montage_image,
    *preview_image;

  ImageInfo
    *clone_info;

  int
    argc;

  long
    y;

  MonitorHandler
    handler;

  MontageInfo
    *montage_info;

  RectangleInfo
    geometry;

  register long
    i,
    x;

  unsigned int
    status;

  unsigned long
    colors;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  CloseBlob(image);
  (void) TransformRGBImage(image,RGBColorspace);
  clone_info=CloneImageInfo(image_info);
  clone_info->quality=0;
  colors=2;
  commands[0]=SetClientName((char *) NULL);
  degrees=0;
  gamma=(-0.2f);
  SetGeometry(image,&geometry);
  (void) GetMagickGeometry(DefaultPreviewGeometry,&geometry.x,&geometry.y,
    &geometry.width,&geometry.height);
  images=NewImageList();
  percentage=12.5;
  radius=0.0;
  sigma=1.0;
  threshold=0.0;
  x=0;
  y=0;
  for (i=0; i < NumberTiles; i++)
  {
    preview_image=ZoomImage(image,geometry.width,geometry.height,
      &image->exception);
    if (preview_image == (Image *) NULL)
      break;
    (void) SetImageAttribute(preview_image,"label",DefaultTileLabel);
    if (i == (NumberTiles >> 1))
      {
        (void) QueryColorDatabase("#dfdfdf",&preview_image->matte_color);
        PushImageList(&images,preview_image,&image->exception);
        DestroyImage(preview_image);
        continue;
      }
    argc=1;
    handler=SetMonitorHandler((MonitorHandler) NULL);
    switch (image_info->preview_type)
    {
      case RotatePreview:
      {
        FormatString(factor,"%.1f",degrees+=45.0);
        FormatString(label,"rotate %.1024s",factor);
        commands[argc++]=(char *) "-rotate";
        commands[argc++]=factor;
        break;
      }
      case ShearPreview:
      {
        degrees+=10.0;
        FormatString(factor,"%.1fx%.1f",degrees,2.0*degrees);
        FormatString(label,"shear %.1024s",factor);
        commands[argc++]=(char *) "-shear";
        commands[argc++]=factor;
        break;
      }
      case RollPreview:
      {
        x=((i+1)*preview_image->columns)/NumberTiles;
        y=((i+1)*preview_image->rows)/NumberTiles;
        FormatString(factor,"%+ld%+ld",x,y);
        FormatString(label,"roll %.1024s",factor);
        commands[argc++]=(char *) "-roll";
        commands[argc++]=factor;
        break;
      }
      case HuePreview:
      {
        FormatString(factor,"100/100/%g",2.0*percentage);
        FormatString(label,"modulate %.1024s",factor);
        commands[argc++]=(char *) "-modulate";
        commands[argc++]=factor;
        break;
      }
      case SaturationPreview:
      {
        FormatString(factor,"100/%g",2.0*percentage);
        FormatString(label,"modulate %.1024s",factor);
        commands[argc++]=(char *) "-modulate";
        commands[argc++]=factor;
        break;
      }
      case BrightnessPreview:
      {
        FormatString(factor,"%g",2.0*percentage);
        FormatString(label,"modulate %.1024s",factor);
        commands[argc++]=(char *) "-modulate";
        commands[argc++]=factor;
        break;
      }
      case GammaPreview:
      default:
      {
        FormatString(factor,"%g",gamma+=0.4f);
        FormatString(label,"gamma %.1024s",factor);
        commands[argc++]=(char *) "-gamma";
        commands[argc++]=factor;
        break;
      }
      case SpiffPreview:
      {
        for (x=0; x < i; x++)
          commands[argc++]=(char *) "-contrast";
        FormatString(label,"-contrast %d",i+1);
        break;
      }
      case DullPreview:
      {
        for (x=0; x < i; x++)
          commands[argc++]=(char *) "+contrast";
        FormatString(label,"+contrast %d",i+1);
        break;
      }
      case GrayscalePreview:
      {
        FormatString(factor,"%lu",colors);
        colors<<=1;
        FormatString(label,"colors %.1024s",factor);
        commands[argc++]=(char *) "-colorspace";
        commands[argc++]=(char *) "gray";
        commands[argc++]=(char *) "-colors";
        commands[argc++]=factor;
        break;
      }
      case QuantizePreview:
      {
        FormatString(factor,"%lu",colors<<=1);
        FormatString(label,"colors %.1024s",factor);
        commands[argc++]=(char *) "-colors";
        commands[argc++]=factor;
        break;
      }
      case DespecklePreview:
      {
        for (x=0; x < i; x++)
          commands[argc++]=(char *) "-despeckle";
        FormatString(label,"despeckle %d",i+1);
        break;
      }
      case ReduceNoisePreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"noise %.1024s",factor);
        commands[argc++]=(char *) "-noise";
        commands[argc++]=factor;
        break;
      }
      case AddNoisePreview:
      {
        switch ((int) x)
        {
          case 0: (void) strcpy(factor,"uniform"); break;
          case 1: (void) strcpy(factor,"Gaussian"); break;
          case 2: (void) strcpy(factor,"multiplicative"); break;
          case 3: (void) strcpy(factor,"impulse"); break;
          case 4: (void) strcpy(factor,"laplacian"); break;
          case 5: (void) strcpy(factor,"Poisson"); break;
          default: (void) strcpy(preview_image->magick,"NULL"); break;
        }
        x++;
        FormatString(label,"+noise %.1024s",factor);
        commands[argc++]=(char *) "+noise";
        commands[argc++]=factor;
        break;
      }
      case SharpenPreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"sharpen %.1024s",factor);
        commands[argc++]=(char *) "-sharpen";
        commands[argc++]=factor;
        break;
      }
      case BlurPreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"-blur %.1024s",factor);
        commands[argc++]=(char *) "-blur";
        commands[argc++]=factor;
        break;
      }
      case ThresholdPreview:
      {
        FormatString(factor,"%d",(int) ((percentage*(MaxRGB+1))/100));
        FormatString(label,"threshold %.1024s",factor);
        commands[argc++]=(char *) "-threshold";
        commands[argc++]=factor;
        break;
      }
      case EdgeDetectPreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"edge %.1024s",factor);
        commands[argc++]=(char *) "-edge";
        commands[argc++]=factor;
        break;
      }
      case SpreadPreview:
      {
        FormatString(factor,"%d",i+1);
        FormatString(label,"spread %.1024s",factor);
        commands[argc++]=(char *) "-spread";
        commands[argc++]=factor;
        break;
      }
      case SolarizePreview:
      {
        FormatString(factor,"%g",percentage);
        FormatString(label,"solarize %.1024s",factor);
        commands[argc++]=(char *) "-solarize";
        commands[argc++]=factor;
        break;
      }
      case ShadePreview:
      {
        if (i == 0)
          {
            FormatString(factor,"30.0x30.0");
            FormatString(label,"+shade %.1024s",factor);
            commands[argc++]=(char *) "+shade";
            commands[argc++]=factor;
            break;
          }
        degrees+=10.0;
        FormatString(factor,"%gx%g",degrees,degrees);
        FormatString(label,"shade %.1024s",factor);
        commands[argc++]=(char *) "-shade";
        commands[argc++]=factor;
        break;
      }
      case RaisePreview:
      {
        FormatString(factor,"%dx%d",2*i+2,2*i+2);
        FormatString(label,"raise %.1024s",factor);
        commands[argc++]=(char *) "-raise";
        commands[argc++]=factor;
        break;
      }
      case SegmentPreview:
      {
        threshold+=0.4f;
        FormatString(factor,"%.1fx%.1f",threshold,threshold);
        FormatString(label,"segment %.1024s",factor);
        commands[argc++]=(char *) "-colors";
        commands[argc++]=factor;
        break;
      }
      case SwirlPreview:
      {
        FormatString(factor,"%.1f",degrees+=45.0);
        FormatString(label,"swirl %.1024s",factor);
        commands[argc++]=(char *) "-swirl";
        commands[argc++]=factor;
        break;
      }
      case ImplodePreview:
      {
        FormatString(factor,"%.1f",percentage/100.0);
        FormatString(label,"implode %.1024s",factor);
        commands[argc++]=(char *) "-implode";
        commands[argc++]=factor;
        break;
      }
      case WavePreview:
      {
        degrees+=5.0;
        FormatString(factor,"%.1fx%.1f",0.5*degrees,2.0*degrees);
        FormatString(label,"wave %.1024s",factor);
        commands[argc++]=(char *) "-wave";
        commands[argc++]=factor;
        break;
      }
      case OilPaintPreview:
      {
        FormatString(factor,"%g",0.5*(i+1));
        FormatString(label,"paint %.1024s",factor);
        commands[argc++]=(char *) "-paint";
        commands[argc++]=factor;
        break;
      }
      case CharcoalDrawingPreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"charcoal %.1024s",factor);
        commands[argc++]=(char *) "-charcoal";
        commands[argc++]=factor;
        break;
      }
      case JPEGPreview:
      {
        char
          filename[MaxTextExtent];

        clone_info->quality=(unsigned int) (percentage+13.0);
        FormatString(factor,"%u",clone_info->quality);
        TemporaryFilename(filename);
        FormatString(preview_image->filename,"jpeg:%.1024s",filename);
        status=WriteImage(clone_info,preview_image);
        if (status != False)
          {
            Image
              *quality_image;

            (void) strncpy(clone_info->filename,preview_image->filename,
              MaxTextExtent-1);
            quality_image=ReadImage(clone_info,&image->exception);
            (void) remove(preview_image->filename);
            if (quality_image != (Image *) NULL)
              {
                DestroyImage(preview_image);
                preview_image=quality_image;
              }
          }
        if (GetBlobSize(preview_image) >= (1 << 24))
          FormatString(label,"quality %.1024s\n%lumb ",factor,
            (unsigned long) (GetBlobSize(preview_image)/1024/1024));
        else
          if (GetBlobSize(preview_image) >= (1 << 16))
            FormatString(label,"quality %.1024s\n%lukb ",factor,
              (unsigned long) (GetBlobSize(preview_image)/1024));
          else
            FormatString(label,"quality %.1024s\n%lub ",factor,
              (unsigned long) GetBlobSize(preview_image));
        break;
      }
    }
    percentage+=12.5;
    radius+=0.5;
    sigma+=0.25;
    commands[argc++]=(char *) "-label";
    commands[argc++]=label;
    (void) MogrifyImage(clone_info,argc,commands,&preview_image);
    (void) SetMonitorHandler(handler);
    PushImageList(&images,preview_image,&image->exception);
    DestroyImage(preview_image);
    MagickMonitor(PreviewImageText,i,NumberTiles);
  }
  DestroyImageInfo(clone_info);
  if (images == (Image *) NULL)
		return(False);
  /*
    Create the montage.
  */
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  (void) strncpy(montage_info->filename,image->filename,MaxTextExtent-1);
  montage_info->shadow=True;
  (void) CloneString(&montage_info->tile,"3x3");
  (void) CloneString(&montage_info->geometry,DefaultPreviewGeometry);
  (void) CloneString(&montage_info->frame,DefaultTileFrame);
  montage_image=MontageImages(images,montage_info,&image->exception);
  DestroyMontageInfo(montage_info);
  DestroyImageList(images);
  if (montage_image == (Image *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
      image);
  if (montage_image->montage != (char *) NULL)
    {
      /*
        Free image directory.
      */
      LiberateMemory((void **) &montage_image->montage);
      montage_image->montage=(char *) NULL;
      if (image->directory != (char *) NULL)
        {
          LiberateMemory((void **) &montage_image->directory);
          montage_image->directory=(char *) NULL;
        }
    }
  FormatString(montage_image->filename,"miff:%.1024s",image_info->filename);
  status=WriteImage(image_info,montage_image);
  DestroyImage(montage_image);
  return(status);
}
