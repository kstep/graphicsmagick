/*
% Copyright (C) 2003, 2004 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
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
%                   GraphicsMagick Image Composition Methods                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/alpha_composite.h"
#include "magick/composite.h"
#include "magick/gem.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

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
%  CompositeImage() returns the second image (composite_image) composited
%  onto the first (canvas_image) at the specified offsets.
%
%  The format of the CompositeImage method is:
%
%      unsigned int CompositeImage(Image *canvas_image,
%        const CompositeOperator compose,const Image *composite_image,
%        const long x_offset,const long y_offset)
%
%  A description of each parameter follows:
%
%    o canvas_image: The image to be updated.
%
%    o compose: This operator affects how the composite is applied to
%      the image.  The default is Over.  Choose from one of these
%      operators: OverCompositeOp, InCompositeOp, OutCompositeOP,
%      AtopCompositeOP, XorCompositeOP, PlusCompositeOP, MinusCompositeOP,
%      AddCompositeOP, SubtractCompositeOP, DifferenceCompositeOP,
%      BumpmapCompositeOP, CopyCompositeOP, CopyRedCompositeOP,
%      CopyGreenCompositeOP, CopyBlueCompositeOP, CopyOpacityCompositeOP.
%
%    o composite_image: The composite image.
%
%    o x_offset: The column offset of the composited image.
%
%    o y_offset: The row offset of the composited image.
%
%
*/

MagickExport MagickPassFail CompositeImage(Image *canvas_image,
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

  MagickPassFail
    status=MagickPass;

  /*
    Prepare composite image.
  */
  assert(canvas_image != (Image *) NULL);
  assert(canvas_image->signature == MagickSignature);
  assert(composite_image != (Image *) NULL);
  assert(composite_image->signature == MagickSignature);
  if (compose == NoCompositeOp)
    return(MagickPass);
  (void) SetImageType(canvas_image,TrueColorType);
  switch (compose)
  {
    case CopyOpacityCompositeOp:
    {
      canvas_image->matte=MagickTrue;
      break;
    }
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
      displace_image=CloneImage(composite_image,0,0,True,&canvas_image->exception);
      if (displace_image == (Image *) NULL)
        return(MagickFail);
      horizontal_scale=20.0;
      vertical_scale=20.0;
      if (composite_image->geometry != (char *) NULL)
        {
          int
            count;

          /*
            Determine the horizontal and vertical displacement scale.
          */
          count=GetMagickDimension(composite_image->geometry,
            &horizontal_scale,&vertical_scale);
          if (count == 1)
            vertical_scale=horizontal_scale;
        }
      /*
        Shift image pixels as defined by a displacement map.
      */
      for (y=0; y < (long) composite_image->rows; y++)
      {
        if (((y+y_offset) < 0) || ((y+y_offset) >= (long) canvas_image->rows))
          continue;
        p=AcquireImagePixels(composite_image,0,y,composite_image->columns,1,
          &canvas_image->exception);
        q=GetImagePixels(canvas_image,0,y+y_offset,canvas_image->columns,1);
        r=GetImagePixels(displace_image,0,y,displace_image->columns,1);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL) ||
            (r == (PixelPacket *) NULL))
          {
            status=MagickFail;
            break;
          }
        q+=x_offset;
        for (x=0; x < (long) composite_image->columns; x++)
        {
          if (((x_offset+x) < 0) || ((x_offset+x) >= (long) canvas_image->columns))
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
          *r=InterpolateColor(canvas_image,x_offset+x+x_displace,y_offset+y+y_displace,
            &canvas_image->exception);
          p++;
          q++;
          r++;
        }
        if (!SyncImagePixels(displace_image))
          {
            status=MagickFail;
            break;
          }
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
          count=GetMagickDimension(composite_image->geometry,
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
        (void) GetMagickDimension(composite_image->geometry,&amount,&threshold);
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
  for (y=0; y < (long) canvas_image->rows; y++)
  {
    if (y < y_offset)
      continue;
    if ((y-y_offset) >= (long) composite_image->rows)
      break;
    p=AcquireImagePixels(composite_image,0,y-y_offset,composite_image->columns,
      1,&canvas_image->exception);
    q=GetImagePixels(canvas_image,0,y,canvas_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=MagickFail;
        break;
      }
    pixels=p;
    if (x_offset < 0)
      p-=x_offset;
    indexes=GetIndexes(canvas_image);
    composite_indexes=GetIndexes(composite_image);
    for (x=0; x < (long) canvas_image->columns; x++)
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
      if (!canvas_image->matte)
        destination.opacity=OpaqueOpacity;
      else
        if (canvas_image->colorspace == CMYKColorspace)
          destination.opacity=indexes[x];
      switch (compose)
      {
        case OverCompositeOp:
        {
          /*
            The result will be the union of the two image shapes, with
            opaque areas of change-image obscuring base-image in the
            region of overlap.
          */
          destination=AlphaComposite(&source,source.opacity,&destination,
            destination.opacity);
          break;
        }
        case InCompositeOp:
        {
          /*
            The result is simply change-image cut by the shape of
            base-image. None of the image data of base-image will be
            in the result.
          */
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
          /*
            The resulting image is change-image with the shape of
            base-image cut out.
          */
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
          /*
            The result is the same shape as base-image, with
            change-image obscuring base-image where the image shapes
            overlap. Note this differs from over because the portion
            of change-image outside base-image's shape does not appear
            in the result.
          */
          pixel.opacity=((double)(MaxRGB-source.opacity)*
            (MaxRGB-destination.opacity)+(double) source.opacity*
            (MaxRGB-destination.opacity))/MaxRGB;

          pixel.red=((double) (MaxRGB-source.opacity)*(MaxRGB-
            destination.opacity)*source.red/MaxRGB+(double)
            source.opacity*(MaxRGB-destination.opacity)*
            destination.red/MaxRGB)/pixel.opacity;
          destination.red=RoundDoubleToQuantum(pixel.red);

          pixel.green=((double) (MaxRGB-source.opacity)*(MaxRGB-
            destination.opacity)*source.green/MaxRGB+(double)
            source.opacity*(MaxRGB-destination.opacity)*
            destination.green/MaxRGB)/pixel.opacity;
          destination.green=RoundDoubleToQuantum(pixel.green);

          pixel.blue=((double) (MaxRGB-source.opacity)*(MaxRGB-
            destination.opacity)*source.blue/MaxRGB+(double)
            source.opacity*(MaxRGB-destination.opacity)*
            destination.blue/MaxRGB)/pixel.opacity;
          destination.blue=RoundDoubleToQuantum(pixel.blue);

          destination.opacity=MaxRGB-RoundDoubleToQuantum(pixel.opacity);
          break;
        }
        case XorCompositeOp:
        {
          /*
            The result is the image data from both change-image and
            base-image that is outside the overlap region. The overlap
            region will be blank.
          */
          double gamma;
          double source_alpha;
          double dest_alpha;
          double composite;
          source_alpha=(double) source.opacity/MaxRGB;
          dest_alpha=(double) destination.opacity/MaxRGB;
          
          gamma=(1.0-source_alpha)+(1.0-dest_alpha)-
            2.0*(1.0-source_alpha)*(1.0-dest_alpha);
          
          composite=MaxRGB*(1.0-gamma);
          destination.opacity=RoundDoubleToQuantum(composite);
          
          gamma=1.0/(gamma <= MagickEpsilon ? 1.0 : gamma);
          
          composite=((1.0-source_alpha)*source.red*dest_alpha+
                     (1.0-dest_alpha)*destination.red*source_alpha)*gamma;
          destination.red=RoundDoubleToQuantum(composite);
          
          composite=((1.0-source_alpha)*source.green*dest_alpha+
                     (1.0-dest_alpha)*destination.green*source_alpha)*gamma;
          destination.green=RoundDoubleToQuantum(composite);
          
          composite=((1.0-source_alpha)*source.blue*dest_alpha+
                     (1.0-dest_alpha)*destination.blue*source_alpha)*gamma;
          destination.blue=RoundDoubleToQuantum(composite);
          break;
        }
        case PlusCompositeOp:
        {
          /*
            The result is just the sum of the image data. Output
            values are cropped to MaxRGB (no overflow). This operation
            is independent of the matte channels.
          */
          pixel.red=((double) (MaxRGB-source.opacity)*source.red+(double)
            (MaxRGB-destination.opacity)*destination.red)/MaxRGB;
          destination.red=RoundDoubleToQuantum(pixel.red);

          pixel.green=((double) (MaxRGB-source.opacity)*source.green+(double)
            (MaxRGB-destination.opacity)*destination.green)/MaxRGB;
          destination.green=RoundDoubleToQuantum(pixel.green);

          pixel.blue=((double) (MaxRGB-source.opacity)*source.blue+(double)
            (MaxRGB-destination.opacity)*destination.blue)/MaxRGB;
          destination.blue=RoundDoubleToQuantum(pixel.blue);

          pixel.opacity=((double) (MaxRGB-source.opacity)+
            (double) (MaxRGB-destination.opacity))/MaxRGB;
          destination.opacity=MaxRGB-RoundDoubleToQuantum(pixel.opacity);
          break;
        }
        case MinusCompositeOp:
        {
          /*
            The result of change-image - base-image, with underflow
            cropped to zero. The matte channel is ignored (set to
            opaque, full coverage).
          */
          double composite;

          composite=((double) (MaxRGB-destination.opacity)*destination.red-
            (double) (MaxRGB-source.opacity)*source.red)/MaxRGB;
          destination.red=RoundDoubleToQuantum(composite);

          composite=((double) (MaxRGB-destination.opacity)*destination.green-
            (double) (MaxRGB-source.opacity)*source.green)/MaxRGB;
          destination.green=RoundDoubleToQuantum(composite);

          composite=((double) (MaxRGB-destination.opacity)*destination.blue-
            (double) (MaxRGB-source.opacity)*source.blue)/MaxRGB;
          destination.blue=RoundDoubleToQuantum(composite);

          composite=((double) (MaxRGB-destination.opacity)-
            (double) (MaxRGB-source.opacity))/MaxRGB;
          destination.opacity=MaxRGB-RoundDoubleToQuantum(composite);
          break;
        }
        case AddCompositeOp:
        {
          /*
            The result of change-image + base-image, with overflow
            wrapping around (mod MaxRGB+1).
          */
          double composite;

          composite=(double) source.red+destination.red;
          if (composite > MaxRGB) composite -= ((double) MaxRGB+1.0);
          destination.red=RoundDoubleToQuantum(composite);

          composite=(double) source.green+destination.green;
          if (composite > MaxRGB) composite -= ((double) MaxRGB+1.0);
          destination.green=RoundDoubleToQuantum(composite);

          composite=(double) source.blue+destination.blue;
          if (composite > MaxRGB) composite -= ((double) MaxRGB+1.0);
          destination.blue=RoundDoubleToQuantum(composite);

          destination.opacity=OpaqueOpacity;
          break;
        }
        case SubtractCompositeOp:
        {
          /*
            The result of change-image - base-image, with underflow
            wrapping around (mod MaxRGB+1). The add and subtract
            operators can be used to perform reversible
            transformations.
          */
          double composite;

          composite=(double) source.red-destination.red;
          if (composite < 0) composite += ((double) MaxRGB+1.0);
          destination.red=RoundDoubleToQuantum(composite);

          composite=(double) source.green-destination.green;
          if (composite < 0) composite += ((double) MaxRGB+1.0);
          destination.green=RoundDoubleToQuantum(composite);

          composite=(double) source.blue-destination.blue;
          if (composite < 0) composite += ((double) MaxRGB+1.0);
          destination.blue=RoundDoubleToQuantum(composite);

          destination.opacity=OpaqueOpacity;
          break;
        }
        case MultiplyCompositeOp:
        {
          /*
            The result of change-image * base-image. This is useful
            for the creation of drop-shadows.
          */
          double composite;

          composite=((double) source.red*destination.red)/MaxRGB;
          destination.red=RoundDoubleToQuantum(composite);

          composite=((double) source.green*destination.green)/MaxRGB;
          destination.green=RoundDoubleToQuantum(composite);

          composite=((double) source.blue*destination.blue)/MaxRGB;
          destination.blue=RoundDoubleToQuantum(composite);

          composite=((double) source.opacity*destination.opacity)/MaxRGB;
          destination.opacity=RoundDoubleToQuantum(composite);
          break;
        }
        case DifferenceCompositeOp:
        {
          /*
            The result of abs(change-image - base-image). This is
            useful for comparing two very similar images.
          */
          double composite;

          composite=source.red-(double) destination.red;
          destination.red=(Quantum) AbsoluteValue(composite);

          composite=source.green-(double) destination.green;
          destination.green=(Quantum) AbsoluteValue(composite);

          composite=source.blue-(double) destination.blue;
          destination.blue=(Quantum) AbsoluteValue(composite);

          composite=source.opacity-(double) destination.opacity;
          destination.opacity=(Quantum) AbsoluteValue(composite);
          break;
        }
        case BumpmapCompositeOp:
        {
          /*
            The result base-image shaded by change-image.
          */
          double composite;
          double source_intensity;

          source_intensity=(double) PixelIntensity(&source)/MaxRGB;

          composite=source_intensity*destination.red;
          destination.red=RoundDoubleToQuantum(composite);

          composite=source_intensity*destination.green;
          destination.green=RoundDoubleToQuantum(composite);

          composite=source_intensity*destination.blue;
          destination.blue=RoundDoubleToQuantum(composite);

          composite=source_intensity*destination.opacity;
          destination.opacity=RoundDoubleToQuantum(composite);

          break;
        }
        case CopyCompositeOp:
        {
          /*
            The resulting image is base-image replaced with
            change-image. Here the matte information is ignored.
          */
          destination=source;
          break;
        }
        case CopyRedCompositeOp:
        case CopyCyanCompositeOp:
        {
          /*
            The resulting image is the red channel in base-image
            replaced with the red channel in change-image. The other
            channels are copied untouched.
          */
          destination.red=source.red;
          break;
        }
        case CopyGreenCompositeOp:
        case CopyMagentaCompositeOp:
        {
          /*
            The resulting image is the green channel in base-image
            replaced with the green channel in change-image. The other
            channels are copied untouched.
          */
          destination.green=source.green;
          break;
        }
        case CopyBlueCompositeOp:
        case CopyYellowCompositeOp:
        {
          /*
            The resulting image is the blue channel in base-image
            replaced with the blue channel in change-image. The other
            channels are copied untouched.
          */
          destination.blue=source.blue;
          break;
        }
        case CopyOpacityCompositeOp:
        default:
        {
          /*
            The resulting image is the opacity channel in base-image
            replaced with the opacity channel in change-image.  The
            other channels are copied untouched.
          */
          if (!composite_image->matte)
            {
              destination.opacity=(Quantum)
                (MaxRGB-PixelIntensityToQuantum(&source));
              break;
            }
          destination.opacity=source.opacity;
          break;
        }
        case CopyBlackCompositeOp:
        {
          /*
            Copy the CMYK Black (K) channel into the image.
          */
          if ((canvas_image->colorspace == CMYKColorspace) &&
              (composite_image->colorspace == CMYKColorspace))
            indexes[x]=(*composite_indexes++);
          break;
        }
        case ClearCompositeOp:
        {
          /*
            Set destination pixels to transparent.
          */
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
      if (canvas_image->colorspace != CMYKColorspace)
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
    if (!SyncImagePixels(canvas_image))
      {
        status=MagickFail;
        break;
      }
  }
  return(status);
}
