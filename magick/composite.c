/*
% Copyright (C) 2003 - 2008 GraphicsMagick Group
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
#include "magick/pixel_row_iterator.h"
#include "magick/utility.h"

typedef struct _CompositePixelsOptions_t
{
  /* Composition operator */
  CompositeOperator compose;

  /* ModulateComposite */
  double            percent_brightness;

  /* ThresholdComposite */
  double            amount;
  double            threshold;
} CompositePixelsOptions_t;

static MagickPassFail
CompositePixels(void *user_data,                   /* User provided mutable data */
                const Image *source_image,         /* Source image */
                const long source_x,               /* X-offset in source image */
                const long source_y,               /* Y-offset in source image */
                const PixelPacket *source_pixels,  /* Pixel row in source image */
                const IndexPacket *source_indexes, /* Pixel row indexes in source image */
                Image *update_image,               /* Update image */
                const long update_x,               /* X-offset in update image */
                const long update_y,               /* Y-offset in update image */
                PixelPacket *update_pixels,        /* Pixel row in update image */
                IndexPacket *update_indexes,       /* Pixel row indexes in update image */
                const long npixels,                /* Number of pixels in row */
                ExceptionInfo *exception           /* Exception report */
                )
{
  const CompositePixelsOptions_t
    *options = (const CompositePixelsOptions_t *) user_data;

  const CompositeOperator
    compose = options->compose;

  const double
    percent_brightness = options->percent_brightness;

  const double
    amount = options->amount,
    threshold = options->threshold;

  double
    brightness,
    hue,
    saturation,
    sans;

  PixelPacket
    destination,
    source;

  double
    midpoint;

  DoublePixelPacket
    pixel;

  register long
    i;
  
  ARG_NOT_USED(user_data);
  ARG_NOT_USED(source_x);
  ARG_NOT_USED(source_y);
  ARG_NOT_USED(update_x);
  ARG_NOT_USED(update_y);
  ARG_NOT_USED(exception);

  midpoint=((double) MaxRGB+1.0)/2;
  for (i=0; i < npixels; i++)
    {
      /*
        Build source and destination working pixels.
      */
      source=source_pixels[i];
      if (!source_image->matte)
        source.opacity=OpaqueOpacity;
      else
        if (source_image->colorspace == CMYKColorspace)
          source.opacity=source_indexes[i];

      destination=update_pixels[i];
      if (!update_image->matte)
        destination.opacity=OpaqueOpacity;
      else
        if (update_image->colorspace == CMYKColorspace)
          destination.opacity=update_indexes[i];

      /*
        Compose pixel.
      */
      switch (compose)
        {
        case OverCompositeOp:
          {
            /*
              The result will be the union of the two image shapes, with
              opaque areas of change-image obscuring base-image in the
              region of overlap.
            */
            destination=AlphaComposite(&source,source.opacity,&destination,destination.opacity);
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

            pixel.opacity=(double)
              (((double) MaxRGBDouble-source.opacity)*
               (MaxRGBDouble-destination.opacity)/MaxRGBDouble);

            destination.red=(Quantum)
              (((double) MaxRGBDouble-source.opacity)*
               (MaxRGBDouble-destination.opacity)*source.red/MaxRGBDouble/pixel.opacity+0.5);

            destination.green=(Quantum)
              (((double) MaxRGBDouble-source.opacity)*
               (MaxRGBDouble-destination.opacity)*source.green/MaxRGBDouble/pixel.opacity+0.5);

            destination.blue=(Quantum)
              (((double) MaxRGBDouble-source.opacity)*
               (MaxRGBDouble-destination.opacity)*source.blue/MaxRGBDouble/pixel.opacity+0.5);

            destination.opacity=(Quantum) (MaxRGBDouble-pixel.opacity+0.5);
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
              (MaxRGBDouble-source.opacity)*destination.opacity/MaxRGBDouble;

            destination.red=(Quantum)
              (((double) MaxRGBDouble-source.opacity)*
               destination.opacity*source.red/MaxRGBDouble/pixel.opacity+0.5);

            destination.green=(Quantum)
              (((double) MaxRGBDouble-source.opacity)*
               destination.opacity*source.green/MaxRGBDouble/pixel.opacity+0.5);

            destination.blue=(Quantum)
              (((double) MaxRGBDouble-source.opacity)*
               destination.opacity*source.blue/MaxRGBDouble/pixel.opacity+0.5);

            destination.opacity=(Quantum) (MaxRGBDouble-pixel.opacity+0.5);
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
            destination=AtopComposite(&destination,&source);
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
            source_alpha=(double) source.opacity/MaxRGBDouble;
            dest_alpha=(double) destination.opacity/MaxRGBDouble;
          
            gamma=(1.0-source_alpha)+(1.0-dest_alpha)-
              2.0*(1.0-source_alpha)*(1.0-dest_alpha);
          
            composite=MaxRGBDouble*(1.0-gamma);
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
            pixel.red=((double) (MaxRGBDouble-source.opacity)*source.red+(double)
                       (MaxRGBDouble-destination.opacity)*destination.red)/MaxRGBDouble;
            destination.red=RoundDoubleToQuantum(pixel.red);

            pixel.green=((double) (MaxRGBDouble-source.opacity)*source.green+(double)
                         (MaxRGBDouble-destination.opacity)*destination.green)/MaxRGBDouble;
            destination.green=RoundDoubleToQuantum(pixel.green);

            pixel.blue=((double) (MaxRGBDouble-source.opacity)*source.blue+(double)
                        (MaxRGBDouble-destination.opacity)*destination.blue)/MaxRGBDouble;
            destination.blue=RoundDoubleToQuantum(pixel.blue);

            pixel.opacity=((double) (MaxRGBDouble-source.opacity)+
                           (double) (MaxRGBDouble-destination.opacity))/MaxRGBDouble;
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

            composite=((double) (MaxRGBDouble-destination.opacity)*destination.red-
                       (double) (MaxRGBDouble-source.opacity)*source.red)/MaxRGBDouble;
            destination.red=RoundDoubleToQuantum(composite);

            composite=((double) (MaxRGBDouble-destination.opacity)*destination.green-
                       (double) (MaxRGBDouble-source.opacity)*source.green)/MaxRGBDouble;
            destination.green=RoundDoubleToQuantum(composite);

            composite=((double) (MaxRGBDouble-destination.opacity)*destination.blue-
                       (double) (MaxRGBDouble-source.opacity)*source.blue)/MaxRGBDouble;
            destination.blue=RoundDoubleToQuantum(composite);

            composite=((double) (MaxRGBDouble-destination.opacity)-
                       (double) (MaxRGBDouble-source.opacity))/MaxRGBDouble;
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
            if (composite > MaxRGBDouble) composite -= ((double) MaxRGBDouble+1.0);
            destination.red=RoundDoubleToQuantum(composite);

            composite=(double) source.green+destination.green;
            if (composite > MaxRGBDouble) composite -= ((double) MaxRGBDouble+1.0);
            destination.green=RoundDoubleToQuantum(composite);

            composite=(double) source.blue+destination.blue;
            if (composite > MaxRGBDouble) composite -= ((double) MaxRGBDouble+1.0);
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
            if (composite < 0) composite += ((double) MaxRGBDouble+1.0);
            destination.red=RoundDoubleToQuantum(composite);

            composite=(double) source.green-destination.green;
            if (composite < 0) composite += ((double) MaxRGBDouble+1.0);
            destination.green=RoundDoubleToQuantum(composite);

            composite=(double) source.blue-destination.blue;
            if (composite < 0) composite += ((double) MaxRGBDouble+1.0);
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

            composite=((double) source.red*destination.red)/MaxRGBDouble;
            destination.red=RoundDoubleToQuantum(composite);

            composite=((double) source.green*destination.green)/MaxRGBDouble;
            destination.green=RoundDoubleToQuantum(composite);

            composite=((double) source.blue*destination.blue)/MaxRGBDouble;
            destination.blue=RoundDoubleToQuantum(composite);

            composite=((double) source.opacity*destination.opacity)/MaxRGBDouble;
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

            source_intensity=(double) PixelIntensity(&source)/MaxRGBDouble;

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
            if (!source_image->matte)
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
            if ((update_image->colorspace == CMYKColorspace) &&
                (source_image->colorspace == CMYKColorspace))
              update_pixels[i].opacity=source_pixels[i].opacity;
            else
              update_pixels[i].opacity=PixelIntensityToQuantum(&source);
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
            destination.red=(Quantum)
              (((double) source.opacity*source.red+
                (MaxRGBDouble-source.opacity)*destination.red)/MaxRGBDouble+0.5);
            destination.green=(Quantum)
              (((double) source.opacity*source.green+
                (MaxRGBDouble-source.opacity)*destination.green)/MaxRGBDouble+0.5);
            destination.blue=(Quantum)
              (((double) source.opacity*source.blue+
                (MaxRGBDouble-source.opacity)*destination.blue)/MaxRGBDouble+0.5);
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
            destination.red=RoundDoubleToQuantum(pixel.red);
            destination.green=RoundDoubleToQuantum(pixel.green);
            destination.blue=RoundDoubleToQuantum(pixel.blue);
            destination.opacity=RoundDoubleToQuantum(pixel.opacity);
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
      /*
        Update pixel.
      */
      update_pixels[i].red=destination.red;
      update_pixels[i].green=destination.green;
      update_pixels[i].blue=destination.blue;
      if (update_image->colorspace != CMYKColorspace)
        {
          /*
            RGB stores opacity in 'opacity'.
          */
          update_pixels[i].opacity=destination.opacity;
        }
      else
        {
          /*
            CMYK(A) stores K in 'opacity' and A in the indexes.
          */
          update_indexes[i]=destination.opacity; /* opacity */
        }
    }

  return MagickPass;
}


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
  CompositePixelsOptions_t
    options;

  double
    amount,
    percent_brightness,
    percent_saturation,
    threshold;

  long
    y;

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
  canvas_image->storage_class=DirectClass;
  switch (compose)
  {
    case CopyCyanCompositeOp:
    case CopyMagentaCompositeOp:
    case CopyYellowCompositeOp:
    case CopyBlackCompositeOp:
      {
        canvas_image->colorspace=CMYKColorspace;
        break;
      }
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
/* FIXME - Leak here! */
/* Should probably always clone composite_image so that we can make sure that it is in a useful colorspace. */
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
  options.compose=compose;
  options.percent_brightness=percent_brightness;
  options.amount=amount;
  options.threshold=threshold;

  {
    unsigned long
      columns,
      rows;

    long
      composite_x,
      composite_y,
      canvas_x,
      canvas_y;

    columns=composite_image->columns;
    rows=composite_image->rows;

    composite_x=0;
    composite_y=0;
    canvas_x=x_offset;
    canvas_y=y_offset;

    if (x_offset < 0)
      composite_x += -x_offset;
    if (y_offset < 0)
      composite_y += -y_offset;

    columns -= composite_x;
    rows -= composite_y;

    if (canvas_x < 0)
      canvas_x=0;
    if (canvas_y < 0)
      canvas_y=0;

#if 0
    printf("canvas=%lux%lu composite=%lux%lu offset=%ldx%ld | canvas=%ldx%ld composite=%ldx%ld size=%ldx%ld\n",
           canvas_image->columns,canvas_image->rows,
           composite_image->columns,composite_image->rows,
           x_offset,y_offset,
           canvas_x,canvas_y,
           composite_x,composite_y,
           columns,rows);
#endif

    if (((unsigned long) canvas_x < canvas_image->columns) &&
        ((unsigned long) canvas_y < canvas_image->rows) &&
        ((unsigned long) composite_x < composite_image->columns) &&
        ((unsigned long) composite_y < composite_image->rows))
      {
        if ((canvas_x + composite_image->columns) > canvas_image->columns)
          columns -= ((canvas_x + composite_image->columns) - canvas_image->columns);
        if ((canvas_y + composite_image->rows) > canvas_image->rows)
          rows -= ((canvas_y + composite_image->rows) - canvas_image->rows);
        status=PixelRowIterateDualModify(CompositePixels,        /* Callback */
                                         "Composite image pixels ...", /* Description */
                                         &options,               /* Options */
                                         columns,                /* Number of columns */
                                         rows,                   /* Number of rows */
                                         composite_image,        /* Composite image */
                                         composite_x,            /* Composite x offset */
                                         composite_y,            /* Composite y offset */
                                         canvas_image,           /* Canvas image */
                                         canvas_x,               /* Canvas x offset */
                                         canvas_y,               /* Canvas y offset */
                                         &canvas_image->exception); /* Exception */
      }
  }
  return(status);
}
