/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%        CCCC   OOO  M   M  PPPP    OOO   SSSSS  IIIII  TTTTT  EEEEE          %
%       C      O   O MM MM  P   P  O   O  SS       I      T    E              %
%       C      O   O M M M  PPPP   O   O   SSS     I      T    EEE            %
%       C      O   O M   M  P      O   O     SS    I      T    E              %
%        CCCC   OOO  M   M  P       OOO   SSSSS  IIIII    T    EEEEE          %
%                                                                             %
%                                                                             %
%                    ImageMagick Image Composite Methods                      %
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
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "cache.h"
#include "composite.h"
#include "gem.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p o s i t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompositeImage() returns the second image composited onto the first at the
%  specified offsets.
%
%  The format of the CompositeImage method is:
%
%      unsigned int CompositeImage(Image *image,const CompositeOperator compose,
%        const Image *composite_image,const long x_offset,const long y_offset)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o compose: This operator affects how the composite is applied to
%      the image.  The default is Over.  Choose from these operators:
%
%        OverCompositeOp       InCompositeOp         OutCompositeOP
%        AtopCompositeOP       XorCompositeOP        PlusCompositeOP
%        MinusCompositeOP      AddCompositeOP        SubtractCompositeOP
%        DifferenceCompositeOP BumpmapCompositeOP    CopyCompositeOP
%        DisplaceCompositeOP
%
%    o composite_image: The composite image.
%
%    o x_offset: The column offset of the composited image.
%
%    o y_offset: The row offset of the composited image.
%
%
*/

static inline PixelPacket AlphaComposite(const PixelPacket *p,
  const double alpha,const PixelPacket *q,const double beta)
{
  PixelPacket
    composite;

  composite.red=(Quantum)
    (((MaxRGB-alpha)*p->red+alpha*(MaxRGB-beta)*q->red/MaxRGB)/MaxRGB+0.5);
  composite.green=(Quantum)
    (((MaxRGB-alpha)*p->green+alpha*(MaxRGB-beta)*q->green/MaxRGB)/MaxRGB+0.5);
  composite.blue=(Quantum)
    (((MaxRGB-alpha)*p->blue+alpha*(MaxRGB-beta)*q->blue/MaxRGB)/MaxRGB+0.5);
  composite.opacity=(Quantum)
    (MaxRGB-((MaxRGB-alpha)+alpha*(MaxRGB-beta)/MaxRGB)+0.5);
  return(composite);
}

MagickExport unsigned int CompositeImage(Image *image,
  const CompositeOperator compose,const Image *composite_image,
  const long x_offset,const long y_offset)
{
  const PixelPacket
    *pixels;

  double
    amount,
    brightness,
    hue,
    midpoint,
    percent_brightness,
    percent_saturation,
    sans,
    saturation,
    threshold;

  DoublePixelPacket
    pixel;

  IndexPacket
    *composite_indexes,
    *indexes;

  long
    y;

  PixelPacket
    destination,
    source;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Prepare composite image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(composite_image != (Image *) NULL);
  assert(composite_image->signature == MagickSignature);
  if (compose == NoCompositeOp)
    return(True);
  SetImageType(image,TrueColorType);
  switch (compose)
  {
    case DisplaceCompositeOp:
    {
      double
        x_displace,
        y_displace;

      double
        horizontal_scale,
        vertical_scale;

      Image
        *displace_image;

      register PixelPacket
        *r;

      /*
        Allocate the displace image.
      */
      displace_image=CloneImage(composite_image,0,0,True,&image->exception);
      if (displace_image == (Image *) NULL)
        return(False);
      horizontal_scale=20.0;
      vertical_scale=20.0;
      if (composite_image->geometry != (char *) NULL)
        {
          int
            count;

          /*
            Determine the horizontal and vertical displacement scale.
          */
          count=sscanf(composite_image->geometry,"%lfx%lf\n",
            &horizontal_scale,&vertical_scale);
          if (count == 1)
            vertical_scale=horizontal_scale;
        }
      /*
        Shift image pixels as defined by a displacement map.
      */
      for (y=0; y < (long) composite_image->rows; y++)
      {
        if (((y+y_offset) < 0) || ((y+y_offset) >= (long) image->rows))
          continue;
        p=AcquireImagePixels(composite_image,0,y,composite_image->columns,1,
          &image->exception);
        q=GetImagePixels(image,0,y+y_offset,image->columns,1);
        r=GetImagePixels(displace_image,0,y,displace_image->columns,1);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL) ||
            (r == (PixelPacket *) NULL))
          break;
        q+=x_offset;
        for (x=0; x < (long) composite_image->columns; x++)
        {
          if (((x_offset+x) < 0) || ((x_offset+x) >= (long) image->columns))
            {
              p++;
              q++;
              continue;
            }
          x_displace=(horizontal_scale*(PixelIntensityToQuantum(p)-
            (((double) MaxRGB+1.0)/2)))/(((double) MaxRGB+1.0)/2);
          y_displace=x_displace;
          if (composite_image->matte)
            y_displace=(vertical_scale*(p->opacity-
              (((double) MaxRGB+1.0)/2)))/(((double) MaxRGB+1.0)/2);
          *r=InterpolateColor(image,x_offset+x+x_displace,y_offset+y+y_displace,
            &image->exception);
          p++;
          q++;
          r++;
        }
        if (!SyncImagePixels(displace_image))
          break;
      }
      composite_image=displace_image;
      break;
    }
    case ModulateCompositeOp:
    {
      percent_saturation=50.0;
      percent_brightness=50.0;
      if (composite_image->geometry != (char *) NULL)
        {
          int
            count;

          /*
            Determine the brightness and saturation scale.
          */
          count=sscanf(composite_image->geometry,"%lfx%lf\n",
            &percent_brightness,&percent_saturation);
          if (count == 1)
            percent_saturation=percent_brightness;
        }
      percent_brightness/=100.0;
      percent_saturation/=100.0;
      break;
    }
    case ThresholdCompositeOp:
    {
      /*
        Determine the amount and threshold.
      */
      amount=0.5;
      threshold=0.05;
      if (composite_image->geometry != (char *) NULL)
        (void) sscanf(composite_image->geometry,"%lfx%lf\n",&amount,&threshold);
      threshold*=MaxRGB;
      break;
    }
    default:
      break;
  }
  /*
    Composite image.
  */
  midpoint=((double) MaxRGB+1.0)/2;
  for (y=0; y < (long) image->rows; y++)
  {
    if (y < y_offset)
      continue;
    if ((y-y_offset) >= (long) composite_image->rows)
      break;
    p=AcquireImagePixels(composite_image,0,y-y_offset,composite_image->columns,
      1,&image->exception);
    q=GetImagePixels(image,0,y,image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    pixels=p;
    if (x_offset < 0)
      p-=x_offset;
    indexes=GetIndexes(image);
    composite_indexes=GetIndexes(composite_image);
    for (x=0; x < (long) image->columns; x++)
    {
      if (x < x_offset)
        {
          q++;
          continue;
        }
      if ((x-x_offset) >= (long) composite_image->columns)
        break;
      source=(*p);
      if (!composite_image->matte)
        source.opacity=OpaqueOpacity;
      else
        if (composite_image->colorspace == CMYKColorspace)
          source.opacity=composite_indexes[x];
      destination=(*q);
      if (!image->matte)
        destination.opacity=OpaqueOpacity;
      else
        if (image->colorspace == CMYKColorspace)
          destination.opacity=indexes[x];
      switch (compose)
      {
        case OverCompositeOp:
        {
          destination=AlphaComposite(&source,source.opacity,&destination,
            destination.opacity);
          break;
        }
        case InCompositeOp:
        {
          if (source.opacity == TransparentOpacity)
            {
              destination=source;
              break;
            }
          if (destination.opacity == TransparentOpacity)
            break;
          pixel.opacity=(double) (((double) MaxRGB-source.opacity)*
            (MaxRGB-destination.opacity)/MaxRGB);
          destination.red=(Quantum) (((double) MaxRGB-source.opacity)*
            (MaxRGB-destination.opacity)*source.red/MaxRGB/pixel.opacity+0.5);
          destination.green=(Quantum) (((double) MaxRGB-source.opacity)*
            (MaxRGB-destination.opacity)*source.green/MaxRGB/pixel.opacity+0.5);
          destination.blue=(Quantum) (((double) MaxRGB-source.opacity)*
            (MaxRGB-destination.opacity)*source.blue/MaxRGB/pixel.opacity+0.5);
          destination.opacity=(Quantum) (MaxRGB-pixel.opacity+0.5);
          break;
        }
        case OutCompositeOp:
        {
          if (source.opacity == TransparentOpacity)
            {
              destination=source;
              break;
            }
          if (destination.opacity == OpaqueOpacity)
            {
              destination.opacity=TransparentOpacity;
              break;
            }
          pixel.opacity=(double)
            (MaxRGB-source.opacity)*destination.opacity/MaxRGB;
          destination.red=(Quantum) (((double) MaxRGB-source.opacity)*
            destination.opacity*source.red/MaxRGB/pixel.opacity+0.5);
          destination.green=(Quantum) (((double) MaxRGB-source.opacity)*
            destination.opacity*source.green/MaxRGB/pixel.opacity+0.5);
          destination.blue=(Quantum) (((double) MaxRGB-source.opacity)*
            destination.opacity*source.blue/MaxRGB/pixel.opacity+0.5);
          destination.opacity=(Quantum) (MaxRGB-pixel.opacity+0.5);
          break;
        }
        case AtopCompositeOp:
        {
          pixel.opacity=((double)(MaxRGB-source.opacity)*
            (MaxRGB-destination.opacity)+(double) source.opacity*
            (MaxRGB-destination.opacity))/MaxRGB;
          pixel.red=((double) (MaxRGB-source.opacity)*(MaxRGB-
            destination.opacity)*source.red/MaxRGB+(double)
            source.opacity*(MaxRGB-destination.opacity)*
            destination.red/MaxRGB)/pixel.opacity;
          destination.red=(Quantum)
            (pixel.red > MaxRGB ? MaxRGB : pixel.red+0.5);
          pixel.green=((double) (MaxRGB-source.opacity)*(MaxRGB-
            destination.opacity)*source.green/MaxRGB+(double)
            source.opacity*(MaxRGB-destination.opacity)*
            destination.green/MaxRGB)/pixel.opacity;
          destination.green=(Quantum)
            (pixel.green > MaxRGB ? MaxRGB : pixel.green+0.5);
          pixel.blue=((double) (MaxRGB-source.opacity)*(MaxRGB-
            destination.opacity)*source.blue/MaxRGB+(double)
            source.opacity*(MaxRGB-destination.opacity)*
            destination.blue/MaxRGB)/pixel.opacity;
          destination.blue=(Quantum)
            (pixel.blue > MaxRGB ? MaxRGB : pixel.blue+0.5);
          destination.opacity=(Quantum)
            (MaxRGB-(pixel.opacity > MaxRGB ? MaxRGB : pixel.opacity)+0.5);
          break;
        }
        case XorCompositeOp:
        {
          pixel.opacity=((double) (MaxRGB-destination.opacity)*source.opacity/
            MaxRGB+(MaxRGB-source.opacity)*destination.opacity/MaxRGB)/MaxRGB;
          pixel.red=((double) (MaxRGB-destination.opacity)*source.red/
            MaxRGB+(MaxRGB-source.opacity)*destination.red/MaxRGB)/
            pixel.opacity;
          destination.red=(Quantum)
            (pixel.red > MaxRGB ? MaxRGB : pixel.red+0.5);
          pixel.green=((double) (MaxRGB-destination.opacity)*source.green/
            MaxRGB+(MaxRGB-source.opacity)*destination.green/MaxRGB)/
            pixel.opacity;
          destination.green=(Quantum)
            (pixel.green > MaxRGB ? MaxRGB : pixel.green+0.5);
          pixel.blue=((double) (MaxRGB-destination.opacity)*source.blue/
            MaxRGB+(MaxRGB-source.opacity)*destination.blue/MaxRGB)/
            pixel.opacity;
          destination.blue=(Quantum)
            (pixel.blue > MaxRGB ? MaxRGB : pixel.blue+0.5);
          pixel.opacity=((double) (MaxRGB-destination.opacity)*source.opacity/
            MaxRGB+(MaxRGB-source.opacity)*destination.opacity/MaxRGB)/
            pixel.opacity;
          destination.opacity=(Quantum)
            (MaxRGB-(pixel.opacity > MaxRGB ? MaxRGB : pixel.opacity)+0.5);
          break;
        }
        case PlusCompositeOp:
        {
          pixel.red=((double) (MaxRGB-source.opacity)*source.red+
            (double) (MaxRGB-destination.opacity)*destination.red)/MaxRGB;
          destination.red=(Quantum)
            (pixel.red > MaxRGB ? MaxRGB : pixel.red+0.5);
          pixel.green=((double) (MaxRGB-source.opacity)*source.green+
            (double) (MaxRGB-destination.opacity)*destination.green)/MaxRGB;
          destination.green=(Quantum)
            (pixel.green > MaxRGB ? MaxRGB : pixel.green+0.5);
          pixel.blue=((double) (MaxRGB-source.opacity)*source.blue+
            (double) (MaxRGB-destination.opacity)*destination.blue)/MaxRGB;
          destination.blue=(Quantum)
            (pixel.blue > MaxRGB ? MaxRGB : pixel.blue+0.5);
          pixel.opacity=MaxRGB-((double) (MaxRGB-source.opacity)+
            (MaxRGB-destination.opacity));
          destination.opacity=(Quantum)
            (pixel.opacity > MaxRGB ? MaxRGB : pixel.opacity+0.5);
          break;
        }
        case MinusCompositeOp:
        {
          pixel.red=((double) (MaxRGB-source.opacity)*source.red-
            (double) (MaxRGB-destination.opacity)*destination.red)/MaxRGB;
          destination.red=(Quantum) (pixel.red < 0 ? 0 : pixel.red+0.5);
          pixel.green=((double) (MaxRGB-source.opacity)*source.green-
            (double) (MaxRGB-destination.opacity)*destination.green)/MaxRGB;
          destination.green=(Quantum) (pixel.green < 0 ? 0 : pixel.green+0.5);
          pixel.blue=((double) (MaxRGB-source.opacity)*source.blue-
            (double) (MaxRGB-destination.opacity)*destination.blue)/MaxRGB;
          destination.blue=(Quantum) (pixel.blue < 0 ? 0 : pixel.blue+0.5);
          pixel.opacity=MaxRGB-((double) (MaxRGB-source.opacity)-
            (MaxRGB-destination.opacity));
          destination.opacity=(Quantum)
            (pixel.opacity < 0 ? 0 : pixel.opacity+0.5);
          break;
        }
        case AddCompositeOp:
        {
          pixel.red=(double) source.red+destination.red;
          destination.red=(Quantum)
            (pixel.red > MaxRGB ? pixel.red-=MaxRGB : pixel.red+0.5);
          pixel.green=(double) source.green+destination.green;
          destination.green=(Quantum)
            (pixel.green > MaxRGB ? pixel.green-=MaxRGB : pixel.green+0.5);
          pixel.blue=(double) source.blue+destination.blue;
          destination.blue=(Quantum)
            (pixel.blue > MaxRGB ? pixel.blue-=MaxRGB : pixel.blue+0.5);
          destination.opacity=OpaqueOpacity;
          break;
        }
        case SubtractCompositeOp:
        {
          pixel.red=(double) source.red-destination.red;
          destination.red=(Quantum)
            (pixel.red < 0 ? pixel.red+=MaxRGB : pixel.red+0.5);
          pixel.green=(double) source.green-destination.green;
          destination.green=(Quantum)
            (pixel.green < 0 ? pixel.green+=MaxRGB : pixel.green+0.5);
          pixel.blue=(double) source.blue-destination.blue;
          destination.blue=(Quantum)
            (pixel.blue < 0 ? pixel.blue+=MaxRGB : pixel.blue+0.5);
          destination.opacity=OpaqueOpacity;
          break;
        }
        case MultiplyCompositeOp:
        {
          destination.red=(Quantum)
            ((double) (source.red*destination.red/MaxRGB));
          destination.green=(Quantum)
            ((unsigned long) (source.green*destination.green/MaxRGB));
          destination.blue=(Quantum)
            ((double) (source.blue*destination.blue/MaxRGB));
          destination.opacity=(Quantum)
            ((double) (source.opacity*destination.opacity/MaxRGB+0.5));
          break;
        }
        case DifferenceCompositeOp:
        {
          destination.red=(Quantum)
            AbsoluteValue(source.red-(double) destination.red);
          destination.green=(Quantum)
            AbsoluteValue(source.green-(double) destination.green);
          destination.blue=(Quantum)
            AbsoluteValue(source.blue-(double) destination.blue);
          destination.opacity=(Quantum)
            AbsoluteValue(source.opacity-(double) destination.opacity);
          break;
        }
        case BumpmapCompositeOp:
        {
          destination.red=(Quantum) (((double)
            PixelIntensityToQuantum(&source)*destination.red)/MaxRGB+0.5);
          destination.green=(Quantum) (((double)
            PixelIntensityToQuantum(&source)*destination.green)/MaxRGB+0.5);
          destination.blue=(Quantum) (((double)
            PixelIntensityToQuantum(&source)*destination.blue)/MaxRGB+0.5);
          destination.opacity=(Quantum) (((double)
            PixelIntensityToQuantum(&source)*destination.opacity)/MaxRGB+0.5);
          break;
        }
        case CopyCompositeOp:
        {
          destination=source;
          break;
        }
        case CopyRedCompositeOp:
        {
          destination.red=source.red;
          break;
        }
        case CopyGreenCompositeOp:
        {
          destination.green=source.green;
          break;
        }
        case CopyBlueCompositeOp:
        {
          destination.blue=source.blue;
          break;
        }
        case CopyOpacityCompositeOp:
        default:
        {
          if (!composite_image->matte)
            {
              destination.opacity=(Quantum)
                (MaxRGB-PixelIntensityToQuantum(&source));
              break;
            }
          destination.opacity=source.opacity;
          break;
        }
        case ClearCompositeOp:
        {
          destination.opacity=TransparentOpacity;
          break;
        }
        case DissolveCompositeOp:
        {
          destination.red=(Quantum) (((double) source.opacity*source.red+
            (MaxRGB-source.opacity)*destination.red)/MaxRGB+0.5);
          destination.green=(Quantum) (((double) source.opacity*source.green+
            (MaxRGB-source.opacity)*destination.green)/MaxRGB+0.5);
          destination.blue=(Quantum) (((double) source.opacity*source.blue+
            (MaxRGB-source.opacity)*destination.blue)/MaxRGB+0.5);
          destination.opacity=OpaqueOpacity;
          break;
        }
        case DisplaceCompositeOp:
        {
          destination=source;
          break;
        }
        case ThresholdCompositeOp:
        {
          pixel.red=destination.red-(double) source.red;
          if (fabs(2.0*pixel.red) < threshold)
            pixel.red=destination.red;
          else
            pixel.red=destination.red+(pixel.red*amount);
          pixel.green=destination.green-(double) source.green;
          if (fabs(2.0*pixel.green) < threshold)
            pixel.green=destination.green;
          else
            pixel.green=destination.green+(pixel.green*amount);
          pixel.blue=destination.blue-(double) source.blue;
          if (fabs(2.0*pixel.blue) < threshold)
            pixel.blue=destination.blue;
          else
            pixel.blue=destination.blue+(pixel.blue*amount);
          pixel.opacity=destination.opacity-(double) source.opacity;
          if (fabs(2.0*pixel.opacity) < threshold)
            pixel.opacity=destination.opacity;
          else
            pixel.opacity=destination.opacity+(pixel.opacity*amount);
          destination.red=(Quantum) ((pixel.red < 0.0) ? 0 :
            (pixel.red > MaxRGB) ? MaxRGB : pixel.red+0.5);
          destination.green=(Quantum) ((pixel.green < 0.0) ? 0 :
            (pixel.green > MaxRGB) ? MaxRGB : pixel.green+0.5);
          destination.blue=(Quantum) ((pixel.blue < 0) ? 0 :
            (pixel.blue > MaxRGB) ? MaxRGB : pixel.blue+0.5);
          destination.opacity=(Quantum) ((pixel.opacity < 0.0) ? 0 :
            (pixel.opacity > MaxRGB) ? MaxRGB : pixel.opacity+0.5);
          break;
        }
        case ModulateCompositeOp:
        {
          long
            offset;

          offset=(long) (PixelIntensityToQuantum(&source)-midpoint);
          if (offset == 0)
            break;
          TransformHSL(destination.red,destination.green,destination.blue,
            &hue,&saturation,&brightness);
          brightness+=(percent_brightness*offset)/midpoint;
          if (brightness < 0.0)
            brightness=0.0;
          else
            if (brightness > 1.0)
              brightness=1.0;
          HSLTransform(hue,saturation,brightness,&destination.red,
            &destination.green,&destination.blue);
          break;
        }
        case DarkenCompositeOp:
        {
          if (source.opacity == TransparentOpacity)
            break;
          if (destination.opacity == TransparentOpacity)
            {
              destination=source;
              break;
            }
          if (source.red < destination.red)
            destination.red=source.red;
          if (source.green < destination.green)
            destination.green=source.green;
          if (source.blue < destination.blue)
            destination.blue=source.blue;
          if (source.opacity < destination.opacity)
            destination.opacity=source.opacity;
          break;
        }
        case LightenCompositeOp:
        {
          if (source.opacity == TransparentOpacity)
            break;
          if (destination.opacity == TransparentOpacity)
            {
              destination=source;
              break;
            }
          if (source.red > destination.red)
            destination.red=source.red;
          if (source.green > destination.green)
            destination.green=source.green;
          if (source.blue > destination.blue)
            destination.blue=source.blue;
          if (source.opacity > destination.opacity)
            destination.opacity=source.opacity;
          break;
        }
        case HueCompositeOp:
        {
          if (source.opacity == TransparentOpacity)
            break;
          if (destination.opacity == TransparentOpacity)
            {
              destination=source;
              break;
            }
          TransformHSL(destination.red,destination.green,destination.blue,
            &hue,&saturation,&brightness);
          TransformHSL(source.red,source.green,source.blue,&hue,&sans,&sans);
          HSLTransform(hue,saturation,brightness,&destination.red,
            &destination.green,&destination.blue);
          if (source.opacity < destination.opacity)
            destination.opacity=source.opacity;
          break;
        }
        case SaturateCompositeOp:
        {
          if (source.opacity == TransparentOpacity)
            break;
          if (destination.opacity == TransparentOpacity)
            {
              destination=source;
              break;
            }
          TransformHSL(destination.red,destination.green,destination.blue,
            &hue,&saturation,&brightness);
          TransformHSL(source.red,source.green,source.blue,&sans,&saturation,
            &sans);
          HSLTransform(hue,saturation,brightness,&destination.red,
            &destination.green,&destination.blue);
          if (source.opacity < destination.opacity)
            destination.opacity=source.opacity;
          break;
        }
        case LuminizeCompositeOp:
        {
          if (source.opacity == TransparentOpacity)
            break;
          if (destination.opacity == TransparentOpacity)
            {
              destination=source;
              break;
            }
          TransformHSL(destination.red,destination.green,destination.blue,
            &hue,&saturation,&brightness);
          TransformHSL(source.red,source.green,source.blue,&sans,&sans,
            &brightness);
          HSLTransform(hue,saturation,brightness,&destination.red,
            &destination.green,&destination.blue);
          if (source.opacity < destination.opacity)
            destination.opacity=source.opacity;
          break;
        }
        case ColorizeCompositeOp:
        {
          if (source.opacity == TransparentOpacity)
            break;
          if (destination.opacity == TransparentOpacity)
            {
              destination=source;
              break;
            }
          TransformHSL(destination.red,destination.green,destination.blue,
            &sans,&sans,&brightness);
          TransformHSL(source.red,source.green,source.blue,&hue,&saturation,
            &sans);
          HSLTransform(hue,saturation,brightness,&destination.red,
            &destination.green,&destination.blue);
          if (source.opacity < destination.opacity)
            destination.opacity=source.opacity;
          break;
        }
      }
      q->red=destination.red;
      q->green=destination.green;
      q->blue=destination.blue;
      if (image->colorspace != CMYKColorspace)
        q->opacity=destination.opacity;
      else
        {
          q->opacity=p->opacity;
          indexes[x]=destination.opacity;
        }
      p++;
      if (p >= (pixels+composite_image->columns))
        p=pixels;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  return(True);
}
