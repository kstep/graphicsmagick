/*
% Copyright (C) 2003, 2004, 2005 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                  Methods to transform the image colorspace                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                Bob Friesnhahn                               %
%                                 March 2003                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/color.h"
#include "magick/colorspace.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

typedef float TransformQuantum;
#define TransformValue(value) ((TransformQuantum) (value))

/* Round floating value to an integer */
#define RndToInt(value) ((int)((value)+0.5))

/* Assign value of attribute to double if attribute exists for key */
#define MagickAttributeToDouble(image,key,variable) \
{ \
    const ImageAttribute \
      *attribute; \
\
  if ((attribute=GetImageAttribute(image,key))) \
  { \
    variable=strtod(attribute->value,(char **) NULL); \
  } \
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  C o l o r s p a c e T y p e T o S t r i n g                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ColorspaceTypeToString returns a string describing the specified
%  colorspace type.
%
%  The format of the ColorspaceTypeToString method is:
%
%     const char *ColorspaceTypeToString(const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o colorspace:  Colorspace type
%
*/
MagickExport const char *ColorspaceTypeToString(const ColorspaceType colorspace)
{
  const char
    *log_colorspace = "Unknown";
  
  switch (colorspace)
    {
    case UndefinedColorspace:
      log_colorspace="Undefined";
      break;
    case CineonLogRGBColorspace:
      log_colorspace="CineonLogRGB";
      break;
    case RGBColorspace:
      log_colorspace="RGB";
      break;
    case GRAYColorspace:
      log_colorspace="Gray";
      break;
    case Rec601LumaColorspace:
      log_colorspace="Rec601Luma";
      break;
    case Rec709LumaColorspace:
      log_colorspace="Rec709Luma";
      break;
    case TransparentColorspace:
      log_colorspace="Transparent";
      break;
    case OHTAColorspace:
      log_colorspace="OHTA";
      break;
    case XYZColorspace:
      log_colorspace="XYZ";
      break;
    case Rec601YCbCrColorspace:
      log_colorspace="Rec601YCbCr";
      break;
    case Rec709YCbCrColorspace:
      log_colorspace="Rec709YCbCr";
      break;
    case YCCColorspace:
      log_colorspace="PhotoCD YCC";
      break;
    case YIQColorspace:
      log_colorspace="YIQ";
      break;
    case YPbPrColorspace:
      log_colorspace="YPbPr";
      break;
    case YUVColorspace:
      log_colorspace="YUV";
      break;
    case CMYKColorspace:
      log_colorspace="CMYK";
      break;
    case sRGBColorspace:
      log_colorspace="PhotoCD sRGB";
      break;
    case HSLColorspace:
      log_colorspace="HSL";
      break;
    case HWBColorspace:
      log_colorspace="HWB";
      break;
    case LABColorspace:
      log_colorspace="LAB";
      break;
    }
  return log_colorspace;
}

MagickExport ColorspaceType StringToColorspaceType(const char *colorspace_string)
{
  ColorspaceType
    colorspace;

  colorspace=UndefinedColorspace;

  if (LocaleCompare("cineonlog",colorspace_string) == 0)
    colorspace=CineonLogRGBColorspace;
  else if (LocaleCompare("cmyk",colorspace_string) == 0)
    colorspace=CMYKColorspace;
  else if (LocaleCompare("gray",colorspace_string) == 0)
    colorspace=GRAYColorspace;
  else if (LocaleCompare("hsl",colorspace_string) == 0)
    colorspace=HSLColorspace;
  else if (LocaleCompare("hwb",colorspace_string) == 0)
    colorspace=HWBColorspace;
  else if (LocaleCompare("ohta",colorspace_string) == 0)
    colorspace=OHTAColorspace;
  else if (LocaleCompare("rec601luma",colorspace_string) == 0)
    colorspace=Rec601LumaColorspace;
  else if (LocaleCompare("rec709luma",colorspace_string) == 0)
    colorspace=Rec709LumaColorspace;
  else if (LocaleCompare("rgb",colorspace_string) == 0)
    colorspace=RGBColorspace;
  else if (LocaleCompare("srgb",colorspace_string) == 0)
    colorspace=sRGBColorspace;
  else if (LocaleCompare("transparent",colorspace_string) == 0)
    colorspace=TransparentColorspace;
  else if (LocaleCompare("xyz",colorspace_string) == 0)
    colorspace=XYZColorspace;
  else if (LocaleCompare("ycbcr",colorspace_string) == 0)
    colorspace=YCbCrColorspace;
  else if (LocaleCompare("rec601ycbcr",colorspace_string) == 0)
    colorspace=Rec601YCbCrColorspace;
  else if (LocaleCompare("rec709ycbcr",colorspace_string) == 0)
    colorspace=Rec709YCbCrColorspace;
  else if (LocaleCompare("ycc",colorspace_string) == 0)
    colorspace=YCCColorspace;
  else if (LocaleCompare("yiq",colorspace_string) == 0)
    colorspace=YIQColorspace;
  else if (LocaleCompare("ypbpr",colorspace_string) == 0)
    colorspace=YPbPrColorspace;
  else if (LocaleCompare("yuv",colorspace_string) == 0)
    colorspace=YUVColorspace;

  return colorspace;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     R G B T r a n s f o r m I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RGBTransformImage converts the reference image from RGB to
%  an alternate colorspace.  The transformation matrices are not the standard
%  ones: the weights are rescaled to normalize the range of the transformed
%  values to be [0..MaxRGB].
%
%  The format of the RGBTransformImage method is:
%
%      unsigned int RGBTransformImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: the image
%
%    o colorspace: the colorspace to transform the image to.
%
%
*/

typedef struct _XYZColorTransformPacket
{
  TransformQuantum
    x,
    y,
    z;
} XYZColorTransformPacket;

static const size_t
  XYZMapAllocSize=(MaxMap+1)*sizeof(XYZColorTransformPacket);


static inline void XYZTransformPacket(PixelPacket *pixel,
  const XYZColorTransformPacket *x_map,
  const XYZColorTransformPacket *y_map,
  const XYZColorTransformPacket *z_map,
  const XYZColorTransformPacket *primary_info)
{
  double
    blue,
    green,
    red;

  register const XYZColorTransformPacket
    *x_p,
    *y_p,
    *z_p;

  x_p = &x_map[ScaleQuantumToMap(pixel->red)];
  y_p = &y_map[ScaleQuantumToMap(pixel->green)];
  z_p = &z_map[ScaleQuantumToMap(pixel->blue)];

  red=(x_p->x+y_p->x+z_p->x+primary_info->x);
  red = red < 0 ? 0 : red > MaxMap ? MaxMap : (unsigned int) (red + 0.5);
  pixel->red=ScaleMapToQuantum(red);

  green=(x_p->y+y_p->y+z_p->y+primary_info->y);
  green = green < 0 ? 0 : green > MaxMap ? MaxMap : (unsigned int) (green + 0.5);
  pixel->green=ScaleMapToQuantum(green);

  blue=(x_p->z+y_p->z+z_p->z+primary_info->z);
  blue = blue < 0 ? 0 : blue > MaxMap ? MaxMap : (unsigned int) (blue + 0.5);
  pixel->blue=ScaleMapToQuantum(blue);
}
MagickExport MagickPassFail RGBTransformImage(Image *image,
  const ColorspaceType colorspace)
{
  char
    progress_message[MaxTextExtent];

  XYZColorTransformPacket
    *x_map,
    *y_map,
    *z_map;

  register XYZColorTransformPacket
    *x_p,
    *y_p,
    *z_p;

  long
    y;

  XYZColorTransformPacket
    primary_info;

  register long
    i;

  register unsigned long
    x;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /* Detect bogus request to convert to RGB */
  assert(colorspace != RGBColorspace);
  assert(colorspace != TransparentColorspace);
  assert(colorspace != UndefinedColorspace);

  /*
    Ensure that image is an RGB-compatable colorspace prior to
    transforming to an alternate colorspace.
  */
  if (!IsRGBColorspace(image->colorspace))
    (void) TransformRGBImage(image,image->colorspace);

  /*
    Log colorspace transform event
  */
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Transform colorspace from %s to %s",
                        ColorspaceTypeToString(image->colorspace),
                        ColorspaceTypeToString(colorspace));
  FormatString(progress_message,"  Transform colorspace from %s to %s...  ",
               ColorspaceTypeToString(image->colorspace),
               ColorspaceTypeToString(colorspace));

  /*
    Store colorspace in image.
  */
  image->colorspace=colorspace;
  
  if (colorspace == CMYKColorspace)
    {
      IndexPacket
        *indexes;

      Quantum
        black,
        cyan,
        magenta,
        yellow;

      register PixelPacket
        *q;

      /*
        Convert RGB to CMYK colorspace.
      */
      if (image->storage_class == PseudoClass)
        {
          if (SyncImage(image) == MagickFail)
            return (MagickFail);
          image->storage_class=DirectClass;
        }
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        indexes=GetIndexes(image);
        for (x=image->columns; x != 0; x--)
        {
          cyan=(Quantum) (MaxRGB-q->red);
          magenta=(Quantum) (MaxRGB-q->green);
          yellow=(Quantum) (MaxRGB-q->blue);
          black=(cyan < magenta ? Min(cyan,yellow) : Min(magenta,yellow));
          q->red=cyan;
          q->green=magenta;
          q->blue=yellow;
          *indexes++=q->opacity;
          q->opacity=black;
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
      }
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Transform to colorspace %s completed",
                            ColorspaceTypeToString(colorspace)); 
      return(status);
    }

  if (colorspace == CineonLogRGBColorspace)
    {
      /*
        Transform from linear RGB to Cineon Log RGB.
      */
      register PixelPacket
        *q;

      double
        DisplayGamma,
        Gain,
        MaxLinearValue,
        NegativeFilmGamma,
        Offset,
        ReferenceBlack,
        ReferenceWhite;

      unsigned int
        *logmap,
        scale_to_short;

      /*
        Establish defaults.
      */
      MaxLinearValue=MaxRGB; /* Maximum linear value output */
      ReferenceWhite=685;    /* 90% white card (default 685) */
      ReferenceBlack=95;     /* 1% black card  (default 95) */
      DisplayGamma=1.7;      /* Typical display gamma (Kodak recommends 1.7) */
      NegativeFilmGamma=0.6; /* Typical gamma for a film negative */

      /*
        Allow image attributes to override defaults.
      */
      MagickAttributeToDouble(image,"reference-white",ReferenceWhite);
      MagickAttributeToDouble(image,"reference-black",ReferenceBlack);
      MagickAttributeToDouble(image,"display-gamma",DisplayGamma);
      MagickAttributeToDouble(image,"negative-film-gamma",NegativeFilmGamma);

      /*
        FIXME: Math seems to be producing data with gamma 1.0 rather than 1.7.
      */

#if 1
      Gain=MaxLinearValue/(1.0 - pow(pow(10,((ReferenceBlack-ReferenceWhite)
                                             *0.002/NegativeFilmGamma)),
                                     (DisplayGamma/1.7)));
#else
      Gain=MaxLinearValue/(1.0 - pow(pow(10,((ReferenceBlack-ReferenceWhite)
                                             *0.002/NegativeFilmGamma)),
                                     (1.0/DisplayGamma)));
#endif
      Offset=Gain-MaxLinearValue;

      /*
        Build LUT
      */
      logmap=MagickAllocateMemory(unsigned int *,(MaxMap+1)*sizeof(unsigned int));
      if (logmap == 0)
        ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                              UnableToTransformColorspace);
      scale_to_short=(65535U / (65535U >> (16-10)));
      for (i=0; i <= (long) MaxMap; i++)
        {
          double
            linearval,
            logval;

          linearval=i*(double) MaxRGB/MaxMap;

          /*
            FIXME: Math seems to be expecting data with gamma 1.0 rather than 1.7.
          */
          logval=685+log10(pow((((double) linearval+Offset)/Gain),
                               (1.7/DisplayGamma)))/(0.002/NegativeFilmGamma);

/*           logval=685+log10(pow((((double) linearval+Offset)/Gain), */
/*                                (DisplayGamma/1.0)))/(0.002/NegativeFilmGamma); */

          logval *= scale_to_short;
          logmap[i]=ScaleShortToQuantum(RndToInt(logval));
          /* printf("logmap[%u]=%u\n",i,(unsigned int) logmap[i]); */
        }
      /*
        Transform pixels.
      */
      image->storage_class=DirectClass;
      for (y=0; y < (long) image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            {
              status=MagickFail;
              break;
            }
          for (x=image->columns; x != 0; x--)
            {
              q->red=logmap[ScaleQuantumToMap(q->red)];
              q->green=logmap[ScaleQuantumToMap(q->green)];
              q->blue=logmap[ScaleQuantumToMap(q->blue)];
              q++;
            }
          if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        }

      MagickFreeMemory(logmap);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Transform to colorspace %s completed",
                            ColorspaceTypeToString(colorspace));
      return(status);
    }

  if ((colorspace == HSLColorspace) || (colorspace == HWBColorspace))
    {
      void (*transform)(const Quantum,const Quantum,const Quantum,
        double *,double *,double *);

      switch (colorspace)
        {
        case HSLColorspace:
        default:
          transform=TransformHSL;
        case HWBColorspace:
          transform=TransformHWB;
        }

      switch (image->storage_class)
        {
        case DirectClass:
        default:
          {
            register PixelPacket
              *q;
            
            /*
              Convert DirectClass image.
            */
            for (y=0; y < (long) image->rows; y++)
              {
                q=GetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  {
                    status=MagickFail;
                    break;
                  }
                for (x=image->columns; x != 0; x--)
                  {
                    double
                      p1, /* H */
                      p2, /* S or W */
                      p3; /* L or B */

                    (transform)(q->red,q->green,q->blue,&p1,&p2,&p3);
                    q->red=(Quantum) RndToInt(p1*MaxRGB);
                    q->green=(Quantum) RndToInt(p2*MaxRGB);
                    q->blue=(Quantum) RndToInt(p3*MaxRGB);
                    q++;
                  }
                if (!SyncImagePixels(image))
                  {
                    status=MagickFail;
                    break;
                  }
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitor(progress_message,y,image->rows,
                                     &image->exception))
                    break;
              }
            break;
          }
        case PseudoClass:
          {
            /*
              Convert PseudoClass image.
            */
            register PixelPacket
              *q;
            
            q=image->colormap;
            for (i=(long) image->colors; i > 0; i--)
              {
                double
                  p1, /* H */
                  p2, /* S or W */
                  p3; /* L or B */
                
                (transform)(q->red,q->green,q->blue,&p1,&p2,&p3);
                q->red=(Quantum) RndToInt(p1*MaxRGB);
                q->green=(Quantum) RndToInt(p2*MaxRGB);
                q->blue=(Quantum) RndToInt(p3*MaxRGB);
                q++;
              }
            status &= SyncImage(image);
            break;
          }
        }
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Transform to colorspace %s completed",
                            ColorspaceTypeToString(colorspace)); 
      return(status);
    }

  /*
    Allocate the tables.
  */
  x_map=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
  y_map=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
  z_map=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
  if ((x_map == 0) || (y_map == 0) || (z_map == 0))
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToTransformColorspace);
  primary_info.x=primary_info.y=primary_info.z=0;
  switch (colorspace)
  {
    case GRAYColorspace:
    case Rec601LumaColorspace:
    {
      /*
        Initialize Rec. 601 Luma tables:

          G = 0.29900*R+0.58700*G+0.11400*B
      */
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        x_p->x=x_p->y=x_p->z=TransformValue(0.299*i);
        y_p->x=y_p->y=y_p->z=TransformValue(0.587*i);
        z_p->x=z_p->y=z_p->z=TransformValue(0.114*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case Rec709LumaColorspace:
    {
      /*
        Initialize Rec. 709 Luma tables:

          G = 0.2126*R+0.7152*G+0.0722*B
      */
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        x_p->x=x_p->y=x_p->z=TransformValue(0.2126*i);
        y_p->x=y_p->y=y_p->z=TransformValue(0.7152*i);
        z_p->x=z_p->y=z_p->z=TransformValue(0.0722*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          I1 = 0.33333*R+0.33334*G+0.33333*B
          I2 = 0.50000*R+0.00000*G-0.50000*B
          I3 =-0.25000*R+0.50000*G-0.25000*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=TransformValue((MaxMap+1)/2);
      primary_info.z=TransformValue((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        x_p->x=TransformValue(0.33333*i);
        y_p->x=TransformValue(0.33334*i);
        z_p->x=TransformValue(0.33333*i);
        x_p->y=TransformValue(0.5*i);
        y_p->y=TransformValue(0.0);
        z_p->y=TransformValue((-0.5)*i);
        x_p->z=TransformValue((-0.25)*i);
        y_p->z=TransformValue(0.5*i);
        z_p->z=TransformValue((-0.25)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Kodak PhotoYCC Color Space

        Initialize sRGB tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      /* FIXME! The scaling factors for this transform look bizarre,
         and in fact, the results are not correct. */
      primary_info.y=TransformValue(ScaleCharToMap(156));
      primary_info.z=TransformValue(ScaleCharToMap(137));
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) (0.018*MaxMap); i++)
      {
        x_p->x=TransformValue(0.003962014134275617*i);
        y_p->x=TransformValue(0.007778268551236748*i);
        z_p->x=TransformValue(0.001510600706713781*i);
        x_p->y=TransformValue((-0.002426619775463276)*i);
        y_p->y=TransformValue((-0.004763965913702149)*i);
        z_p->y=TransformValue(0.007190585689165425*i);
        x_p->z=TransformValue(0.006927257754597858*i);
        y_p->z=TransformValue((-0.005800713697502058)*i);
        z_p->z=TransformValue((-0.0011265440570958)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      for ( ; i <= (long) MaxMap; i++)
      {
        x_p->x=TransformValue(0.2201118963486454*(1.099*i-0.099));
        y_p->x=TransformValue(0.4321260306242638*(1.099*i-0.099));
        z_p->x=TransformValue(0.08392226148409894*(1.099*i-0.099));
        x_p->y=TransformValue((-0.1348122097479598)*(1.099*i-0.099));
        y_p->y=TransformValue((-0.2646647729834528)*(1.099*i-0.099));
        z_p->y=TransformValue(0.3994769827314126*(1.099*i-0.099));
        x_p->z=TransformValue(0.3848476530332144*(1.099*i-0.099));
        y_p->z=TransformValue((-0.3222618720834477)*(1.099*i-0.099));
        z_p->z=TransformValue((-0.06258578094976668)*(1.099*i-0.099));
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables (from ITU-R 709 RGB):

          X = 0.412453*X+0.357580*Y+0.180423*Z
          Y = 0.212671*X+0.715160*Y+0.072169*Z
          Z = 0.019334*X+0.119193*Y+0.950227*Z
      */
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        x_p->x=TransformValue(0.412453*i);
        y_p->x=TransformValue(0.35758*i);
        z_p->x=TransformValue(0.180423*i);
        x_p->y=TransformValue(0.212671*i);
        y_p->y=TransformValue(0.71516*i);
        z_p->y=TransformValue(0.072169*i);
        x_p->z=TransformValue(0.019334*i);
        y_p->z=TransformValue(0.119193*i);
        z_p->z=TransformValue(0.950227*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case Rec601YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables (using ITU-R BT.601 luma):

          Y =  0.299000*R+0.587000*G+0.114000*B
          Cb= -0.168736*R-0.331264*G+0.500000*B
          Cr=  0.500000*R-0.418688*G-0.081312*B

        Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=TransformValue((MaxMap+1)/2);
      primary_info.z=TransformValue((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        /* Red */
        x_p->x=TransformValue(0.299*i);
        y_p->x=TransformValue(0.587*i);
        z_p->x=TransformValue(0.114*i);
        /* Green */
        x_p->y=TransformValue((-0.16873)*i);
        y_p->y=TransformValue((-0.331264)*i);
        z_p->y=TransformValue(0.500000*i);
        /* Blue */
        x_p->z=TransformValue(0.500000*i);
        y_p->z=TransformValue((-0.418688)*i);
        z_p->z=TransformValue((-0.081312)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case Rec709YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables (using ITU-R BT.709 luma):

          Y =  0.212600*R+0.715200*G+0.072200*B
          Cb= -0.114572*R-0.385428*G+0.500000*B
          Cr=  0.500000*R-0.454153*G-0.045847*B

        Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=TransformValue((MaxMap+1)/2);
      primary_info.z=TransformValue((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        /* Red */
        x_p->x=TransformValue(0.212600*i);
        y_p->x=TransformValue(0.715200*i);
        z_p->x=TransformValue(0.072200*i);
        /* Green */
        x_p->y=TransformValue((-0.114572)*i);
        y_p->y=TransformValue((-0.385428)*i);
        z_p->y=TransformValue(0.500000*i);
        /* Blue */
        x_p->z=TransformValue(0.500000*i);
        y_p->z=TransformValue((-0.454153)*i);
        z_p->z=TransformValue((-0.045847)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Kodak PhotoYCC Color Space.

        Initialize YCC tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      /* FIXME! The scaling factors for this transform look bizarre,
         and in fact, the results are not correct. */
      primary_info.y=TransformValue(ScaleCharToMap(156));
      primary_info.z=TransformValue(ScaleCharToMap(137));
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) (0.018*MaxMap); i++)
      {
        x_p->x=TransformValue(0.003962014134275617*i);
        y_p->x=TransformValue(0.007778268551236748*i);
        z_p->x=TransformValue(0.001510600706713781*i);
        x_p->y=TransformValue((-0.002426619775463276)*i);
        y_p->y=TransformValue((-0.004763965913702149)*i);
        z_p->y=TransformValue(0.007190585689165425*i);
        x_p->z=TransformValue(0.006927257754597858*i);
        y_p->z=TransformValue((-0.005800713697502058)*i);
        z_p->z=TransformValue((-0.0011265440570958)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      for ( ; i <= (long) MaxMap; i++)
      {
        x_p->x=TransformValue(0.2201118963486454*(1.099*i-0.099));
        y_p->x=TransformValue(0.4321260306242638*(1.099*i-0.099));
        z_p->x=TransformValue(0.08392226148409894*(1.099*i-0.099));
        x_p->y=TransformValue((-0.1348122097479598)*(1.099*i-0.099));
        y_p->y=TransformValue((-0.2646647729834528)*(1.099*i-0.099));
        z_p->y=TransformValue(0.3994769827314126*(1.099*i-0.099));
        x_p->z=TransformValue(0.3848476530332144*(1.099*i-0.099));
        y_p->z=TransformValue((-0.3222618720834477)*(1.099*i-0.099));
        z_p->z=TransformValue((-0.06258578094976668)*(1.099*i-0.099));
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          Y = 0.29900*R+0.58700*G+0.11400*B
          I = 0.59600*R-0.27400*G-0.32200*B
          Q = 0.21100*R-0.52300*G+0.31200*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=TransformValue((MaxMap+1)/2);
      primary_info.z=TransformValue((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        x_p->x=TransformValue(0.299*i);
        y_p->x=TransformValue(0.587*i);
        z_p->x=TransformValue(0.114*i);
        x_p->y=TransformValue(0.596*i);
        y_p->y=TransformValue((-0.274)*i);
        z_p->y=TransformValue((-0.322)*i);
        x_p->z=TransformValue(0.211*i);
        y_p->z=TransformValue((-0.523)*i);
        z_p->z=TransformValue(0.312*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables (according to ITU-R BT.601):

          Y =  0.299000*R+0.587000*G+0.114000*B
          Pb= -0.168736*R-0.331264*G+0.500000*B
          Pr=  0.500000*R-0.418688*G-0.081312*B

        Pb and Pr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=TransformValue((MaxMap+1)/2);
      primary_info.z=TransformValue((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        x_p->x=TransformValue(0.299*i);
        y_p->x=TransformValue(0.587*i);
        z_p->x=TransformValue(0.114*i);
        x_p->y=TransformValue((-0.168736)*i);
        y_p->y=TransformValue((-0.331264)*i);
        z_p->y=TransformValue(0.5*i);
        x_p->z=TransformValue(0.5*i);
        y_p->z=TransformValue((-0.418688)*i);
        z_p->z=TransformValue((-0.081312)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          U = -0.14740*R-0.28950*G+0.43690*B
          V =  0.61500*R-0.51500*G-0.10000*B

        U and V, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
      */
      primary_info.y=TransformValue((MaxMap+1)/2);
      primary_info.z=TransformValue((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) MaxMap; i++)
      {
        x_p->x=TransformValue(0.299*i);
        y_p->x=TransformValue(0.587*i);
        z_p->x=TransformValue(0.114*i);
        x_p->y=TransformValue((-0.1474)*i);
        y_p->y=TransformValue((-0.2895)*i);
        z_p->y=TransformValue(0.4369*i);
        x_p->z=TransformValue(0.615*i);
        y_p->z=TransformValue((-0.515)*i);
        z_p->z=TransformValue((-0.1)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
  }

#if 0
  /*
    Dump tables
  */
  for (i=0; i <= (long) MaxMap; i++)
    {
      printf("%5ld: x_map(%g,%g,%g) y_map(%g,%g,%g) z_map(%g,%g,%g)\n",
             i,
             ((x_map[i].x)),
             ((x_map[i].y)),
             ((x_map[i].z)),
             
             ((y_map[i].x)),
             ((y_map[i].y)),
             ((y_map[i].z)),
             
             ((z_map[i].x)),
             ((z_map[i].y)),
             ((z_map[i].z)));
    }
#endif

  /*
    Convert from RGB.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      ExceptionInfo
        *exception;

      register PixelPacket
        *q;

       /*
        Convert DirectClass image.
      */
      exception=(&image->exception);
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        for (x=image->columns; x != 0; x--)
        {
          XYZTransformPacket(q,x_map,y_map,z_map,&primary_info);
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(progress_message,y,image->rows,exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      register PixelPacket
        *q;

      q=image->colormap;
      for (i=(long) image->colors; i > 0; i--)
      {
        XYZTransformPacket(q,x_map,y_map,z_map,&primary_info);
        q++;
      }
      status &= SyncImage(image);
      break;
    }
  }
  /*
    Free allocated memory.
  */
  MagickFreeMemory(z_map);
  MagickFreeMemory(y_map);
  MagickFreeMemory(x_map);

  image->is_grayscale=IsGrayColorspace(colorspace);
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Transform to colorspace %s completed",
                        ColorspaceTypeToString(colorspace)); 
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     T r a n s f o r m C o l o r s p a c e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method (void) TransformColorspace converts the image to a specified colorspace.
%  If the image is already in the requested colorspace, no work is performed.
%  Note that the current colorspace is stored in the image colorspace member.
%  The transformation matrices are not necessarily the standard ones: the
%  weights are rescaled to normalize the range of the transformed values to
%  be [0..MaxRGB].
%
%  The format of the (void) TransformColorspace method is:
%
%      unsigned int (void) TransformColorspace(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: the image to transform
%
%    o colorspace: the desired colorspace.
%
*/
MagickExport MagickPassFail TransformColorspace(Image *image,
  const ColorspaceType colorspace)
{
  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(colorspace != UndefinedColorspace);
  assert(image->colorspace != UndefinedColorspace);
  /*
    If the image colorspace is the same as requested, do nothing.
  */
  if (image->colorspace == colorspace)
     return (status);

  /*
    If the requested colorspace is RGB or Transparent, then convert
    via TransformRGBImage.
  */
  if ((colorspace == RGBColorspace) ||
      (colorspace == TransparentColorspace))
      {
        status &= TransformRGBImage(image,image->colorspace);
        return  (status);
      }

  /*
    If the image is not already in an RGB-compatible colorspace, then
    convert it to RGB via TransformRGBImage, and then to the target
    colorspace via RGBTransformImage, otherwise just convert to the
    target colorspace via RGBTransformImage.
  */
  if (!IsRGBColorspace(image->colorspace))
    status=TransformRGBImage(image,image->colorspace);

  status &= RGBTransformImage(image,colorspace);
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     T r a n s f o r m R G B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformRGBImage converts the reference image from an alternate
%  colorspace to RGB.  The transformation matrices are not the standard ones:
%  the weights are rescaled to normalize the range of the transformed values
%  to be [0..MaxRGB].
%
%  The format of the TransformRGBImage method is:
%
%      unsigned int TransformRGBImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: the image
%
%    o colorspace: the colorspace to transform the image to.
%
*/
typedef struct _RGBColorTransformPacket
{
  TransformQuantum
    red,
    green,
    blue;
} RGBColorTransformPacket;

static void RGBTransformPacket(PixelPacket *pixel,
  const RGBColorTransformPacket *red_map,
  const RGBColorTransformPacket *green_map,
  const RGBColorTransformPacket *blue_map,
  const unsigned char *rgb_map,
  const unsigned int rgb_map_max_index)
{
  double
    blue,
    green,
    red;

  register const RGBColorTransformPacket
    *red_p,
    *green_p,
    *blue_p;

  red_p = &red_map[ScaleQuantumToMap(pixel->red)];
  green_p = &green_map[ScaleQuantumToMap(pixel->green)];
  blue_p = &blue_map[ScaleQuantumToMap(pixel->blue)];

  red=(red_p->red+green_p->red+blue_p->red);
  red = red < 0 ? 0 : red > MaxMap ? MaxMap : (unsigned int) (red + 0.5);

  green=(red_p->green+green_p->green+blue_p->green);
  green = green < 0 ? 0 : green > MaxMap ? MaxMap : (unsigned int) (green + 0.5);

  blue=(red_p->blue+green_p->blue+blue_p->blue);
  blue = blue < 0 ? 0 : blue > MaxMap ? MaxMap : (unsigned int) (blue + 0.5);

  if ( rgb_map != 0 )
    {
      unsigned int
        red_index,
        green_index,
        blue_index;

      red_index = ScaleMapToChar(red);
      green_index = ScaleMapToChar(green);
      blue_index = ScaleMapToChar(blue);

      if (red_index > rgb_map_max_index) red_index = rgb_map_max_index;
      if (green_index > rgb_map_max_index) green_index = rgb_map_max_index;
      if (blue_index > rgb_map_max_index) blue_index = rgb_map_max_index;

      pixel->red=ScaleCharToQuantum(rgb_map[red_index]);
      pixel->green=ScaleCharToQuantum(rgb_map[green_index]);
      pixel->blue=ScaleCharToQuantum(rgb_map[blue_index]);
    }
  else
    {
      pixel->red=ScaleMapToQuantum(red);
      pixel->green=ScaleMapToQuantum(green);
      pixel->blue=ScaleMapToQuantum(blue);
    }
}
MagickExport MagickPassFail TransformRGBImage(Image *image,
  const ColorspaceType colorspace)
{

  static const unsigned char
    sRGBMap[351] =
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33,
      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
      50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 65, 66,
      67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
      83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 95, 96, 97, 98, 99,
      100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
      114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
      127, 128, 129, 130, 131, 132, 133, 135, 136, 137, 138, 139, 140,
      141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
      154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166,
      167, 168, 169, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178,
      179, 180, 181, 182, 183, 184, 185, 186, 187, 187, 188, 189, 190,
      191, 192, 193, 194, 194, 195, 196, 197, 198, 199, 199, 200, 201,
      202, 203, 203, 204, 205, 206, 207, 207, 208, 209, 210, 210, 211,
      212, 213, 213, 214, 215, 215, 216, 217, 218, 218, 219, 220, 220,
      221, 222, 222, 223, 223, 224, 225, 225, 226, 227, 227, 228, 228,
      229, 229, 230, 230, 231, 232, 232, 233, 233, 234, 234, 235, 235,
      235, 236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 240,
      241, 241, 242, 242, 242, 243, 243, 243, 243, 244, 244, 244, 245,
      245, 245, 245, 246, 246, 246, 247, 247, 247, 247, 247, 248, 248,
      248, 248, 249, 249, 249, 249, 249, 249, 250, 250, 250, 250, 250,
      250, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 255,
      255, 255, 255, 255, 255
    },
    YCCMap[351] =  /* Photo CD information beyond 100% white, Gamma 2.2 */
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35,
      36, 37, 38, 39, 40, 41, 42, 43, 45, 46, 47, 48, 49, 50, 51, 52,
      53, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68, 69, 70,
      71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 82, 83, 84, 86, 87, 88,
      89, 90, 91, 92, 93, 94, 95, 97, 98, 99, 100, 101, 102, 103, 104,
      105, 106, 107, 108, 110, 111, 112, 113, 114, 115, 116, 117, 118,
      119, 120, 121, 122, 123, 124, 125, 126, 127, 129, 130, 131, 132,
      133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
      146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158,
      159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
      172, 173, 174, 175, 176, 176, 177, 178, 179, 180, 181, 182, 183,
      184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 193, 194, 195,
      196, 197, 198, 199, 200, 201, 201, 202, 203, 204, 205, 206, 207,
      207, 208, 209, 210, 211, 211, 212, 213, 214, 215, 215, 216, 217,
      218, 218, 219, 220, 221, 221, 222, 223, 224, 224, 225, 226, 226,
      227, 228, 228, 229, 230, 230, 231, 232, 232, 233, 234, 234, 235,
      236, 236, 237, 237, 238, 238, 239, 240, 240, 241, 241, 242, 242,
      243, 243, 244, 244, 245, 245, 245, 246, 246, 247, 247, 247, 248,
      248, 248, 249, 249, 249, 249, 250, 250, 250, 250, 251, 251, 251,
      251, 251, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253,
      253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255
    };

  char
    progress_message[MaxTextExtent];

  RGBColorTransformPacket
    *blue_map,
    *green_map,
    *red_map;

  long
    y;

  register long
    x;

  register long
    i;

  unsigned int
    is_grayscale;

  const unsigned char
    *rgb_map=0;

  unsigned int
    rgb_map_max_index=0;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->colorspace != UndefinedColorspace);

  is_grayscale=((image->is_grayscale) || IsGrayColorspace(image->colorspace));

  /*
    If colorspace is already an RGB type then simply return.
  */
  if (IsRGBColorspace(image->colorspace))
    return(status);

  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Transform colorspace from %s to RGB",
                        ColorspaceTypeToString(image->colorspace));

  FormatString(progress_message,"  Transform colorspace from %s to RGB...  ",
               ColorspaceTypeToString(image->colorspace));

  if (image->colorspace == CMYKColorspace)
    {
      IndexPacket
        *indexes;

      register PixelPacket
        *q;

      /*
        Transform image from CMYK to RGB.
      */
      if (image->storage_class == PseudoClass)
        {
          if (SyncImage(image) == MagickFail)
            return (MagickFail);
          image->storage_class=DirectClass;
        }
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=(Quantum) (((double)(MaxRGB-q->red)*(MaxRGB-q->opacity))/MaxRGB+0.5);
          q->green=(Quantum) (((double)(MaxRGB-q->green)*(MaxRGB-q->opacity))/MaxRGB+0.5);
          q->blue=(Quantum) (((double)(MaxRGB-q->blue)*(MaxRGB-q->opacity))/MaxRGB+0.5);
          q->opacity=image->matte ? indexes[x] : OpaqueOpacity;
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
      }
      image->colorspace=RGBColorspace;
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Colorspace transform completed"); 
      return(status);
    }

  if (colorspace == CineonLogRGBColorspace)
    {
      /*
        Transform from Cineon Log RGB to Linear RGB
      */
      register PixelPacket
        *q;

      double
        BreakPoint,
        DisplayGamma,
        Gain,
        KneeGain,
        KneeOffset,
        MaxLinearValue,
        NegativeFilmGamma,
        Offset,
        ReferenceBlack,
        ReferenceWhite,
        SoftClip;

      Quantum
        *linearmap;

      /*
        Establish defaults.
      */
      MaxLinearValue=MaxRGB; /* Maximum linear value output */
      ReferenceWhite=685;    /* 90% white card (default 685) */
      ReferenceBlack=95;     /* 1% black card  (default 95) */
      DisplayGamma=1.7;      /* Typical display gamma (Kodak recommended 1.7) */
      NegativeFilmGamma=0.6; /* Typical gamma for a film negative */
      SoftClip=0.0;          /* Soft clip offset */

      /*
        Allow image attributes to override defaults.
      */
      MagickAttributeToDouble(image,"reference-white",ReferenceWhite);
      MagickAttributeToDouble(image,"reference-black",ReferenceBlack);
      MagickAttributeToDouble(image,"display-gamma",DisplayGamma);
      MagickAttributeToDouble(image,"negative-film-gamma",NegativeFilmGamma);
      MagickAttributeToDouble(image,"soft-clip-offset",SoftClip);

      BreakPoint=ReferenceWhite-SoftClip;
      Gain=MaxLinearValue/(1.0 - pow(pow(10,((ReferenceBlack-ReferenceWhite)
                                                    *0.002/NegativeFilmGamma)),
                                            (DisplayGamma/1.7)));
      Offset=Gain-MaxLinearValue;
      KneeOffset=pow(pow(10,((BreakPoint-ReferenceWhite)*0.002/NegativeFilmGamma)),
                            (DisplayGamma/1.7))*Gain-Offset;
      KneeGain=(MaxLinearValue-KneeOffset)/pow((5*SoftClip),(SoftClip/100));


      linearmap=MagickAllocateMemory(Quantum *,1024*sizeof(Quantum));
      if (linearmap == 0)
        ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                              UnableToTransformColorspace);

      for (i=0; i < 1024; i++)
        {
          double
            linearval,
            logval;

          logval=i;
          if (logval < ReferenceBlack)
            {
              /* Values below reference black are clipped to zero */
              linearval=0.0;
            }
          else if (logval > BreakPoint)
            {
              /* Values above the breakpoint are soft-clipped. */
              linearval=pow((logval-BreakPoint),(SoftClip/100))*KneeGain+KneeOffset;
            }
          else
            {
              /* Otherwise, normal values */
              linearval=pow(pow(10,((logval-ReferenceWhite)*0.002/NegativeFilmGamma)),
                            (DisplayGamma/1.7))*Gain-Offset;
            }

          linearmap[i]=RndToInt(linearval);
        }
      /*
        Transform pixels.
      */
      image->storage_class=DirectClass;
      for (y=0; y < (long) image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            {
              status=MagickFail;
              break;
            }
          for (x=(long) image->columns; x > 0; x--)
            {
              q->red=linearmap[ScaleQuantumToShort(q->red)/64U];
              q->green=linearmap[ScaleQuantumToShort(q->green)/64U];
              q->blue=linearmap[ScaleQuantumToShort(q->blue)/64U];
              q++;
            }
          if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        }

      MagickFreeMemory(linearmap);
      image->colorspace=RGBColorspace;
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Transform to colorspace %s completed",
                            ColorspaceTypeToString(colorspace));
      
      return(status);
    }

  if ((image->colorspace == HSLColorspace) ||
      (image->colorspace == HWBColorspace))
    {
      void (*transform)(const double,const double,const double,
        Quantum *,Quantum *,Quantum *);

      switch (colorspace)
        {
        case HSLColorspace:
        default:
          transform=HSLTransform;
        case HWBColorspace:
          transform=HWBTransform;
        }

      switch (image->storage_class)
        {
        case DirectClass:
        default:
          {
            ExceptionInfo
              *exception;
            
            register PixelPacket
              *q;
            
            /*
              Convert DirectClass image.
            */
            exception=(&image->exception);
            for (y=0; y < (long) image->rows; y++)
              {
                q=GetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  {
                    status=MagickFail;
                    break;
                  }
                for (x=(long) image->columns; x > 0; x--)
                  {
                    (transform)((double)q->red/MaxRGB,(double)q->green/MaxRGB,
                      (double)q->blue/MaxRGB,&q->red,&q->green,&q->blue);
                    q++;
                  }
                if (!SyncImagePixels(image))
                  {
                    status=MagickFail;
                    break;
                  }
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitor(progress_message,y,image->rows,exception))
                    {
                      status=MagickFail;
                      break;
                    }
              }
            break;
          }
        case PseudoClass:
          {
            /*
              Convert PseudoClass image.
            */
            register PixelPacket
              *q;
            
            q=image->colormap;
            for (i=(long) image->colors; i > 0; i--)
              {
                (transform)((double)q->red/MaxRGB,(double)q->green/MaxRGB,
                  (double)q->blue/MaxRGB,&q->red,&q->green,&q->blue);
                q++;
              }
            status &= SyncImage(image);
            break;
          }
        }
      image->colorspace=RGBColorspace;
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Colorspace transform completed"); 
      return(status);
    }

  /*
    Allocate the tables.
  */
  red_map=MagickAllocateMemory(RGBColorTransformPacket *,
    (MaxMap+1)*sizeof(RGBColorTransformPacket));
  green_map=MagickAllocateMemory(RGBColorTransformPacket *,
    (MaxMap+1)*sizeof(RGBColorTransformPacket));
  blue_map=MagickAllocateMemory(RGBColorTransformPacket *,
    (MaxMap+1)*sizeof(RGBColorTransformPacket));
  if ((red_map == (RGBColorTransformPacket *) NULL) ||
      (green_map == (RGBColorTransformPacket *) NULL) ||
      (blue_map == (RGBColorTransformPacket *) NULL))
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToTransformColorspace);
  switch (image->colorspace)
  {
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          R = I1+1.00000*I2-0.66668*I3
          G = I1+0.00000*I2+1.33333*I3
          B = I1-1.00000*I2-0.66668*I3

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxMap.
      */
      for (i=0; i <= (long) MaxMap; i++)
      {
        red_map[i].red=TransformValue(i);
        green_map[i].red=TransformValue(0.5*(2.0*i-MaxMap));
        blue_map[i].red=TransformValue((-0.33334)*(2.0*i-MaxMap));
        red_map[i].green=TransformValue(i);
        green_map[i].green=TransformValue(0);
        blue_map[i].green=TransformValue(0.666665*(2.0*i-MaxMap));
        red_map[i].blue=TransformValue(i);
        green_map[i].blue=TransformValue((-0.5)*(2.0*i-MaxMap));
        blue_map[i].blue=TransformValue((-0.33334)*(2.0*i-MaxMap));
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          R = Y            +1.032096*C2
          G = Y-0.326904*C1-0.704445*C2
          B = Y+1.685070*C1

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      rgb_map=sRGBMap;
      rgb_map_max_index=350;
      for (i=0; i <= (long) MaxMap; i++)
      {
        red_map[i].red=TransformValue(1.40200*i);
        green_map[i].red=TransformValue(0);
        blue_map[i].red=TransformValue(1.88000*(i-ScaleCharToMap(137)));
        red_map[i].green=TransformValue(1.40200*i);
        green_map[i].green=TransformValue((-0.444066)*(i-ScaleCharToMap(156)));
        blue_map[i].green=TransformValue((-0.95692)*(i-ScaleCharToMap(137)));
        red_map[i].blue=TransformValue(1.40200*i);
        green_map[i].blue=TransformValue(2.28900*(i-ScaleCharToMap(156)));
        blue_map[i].blue=TransformValue(0);
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables (to ITU R-709 RGB):

          R =  3.240479*R-1.537150*G-0.498535*B
          G = -0.969256*R+1.875992*G+0.041556*B
          B =  0.055648*R-0.204043*G+1.057311*B
      */
      for (i=0; i <= (long) MaxMap; i++)
      {
        red_map[i].red=TransformValue(3.240479*i);
        green_map[i].red=TransformValue((-1.537150)*i);
        blue_map[i].red=TransformValue((-0.498535)*i);
        red_map[i].green=TransformValue((-0.969256)*i);
        green_map[i].green=TransformValue(1.875992*i);
        blue_map[i].green=TransformValue(0.041556*i);
        red_map[i].blue=TransformValue(0.055648*i);
        green_map[i].blue=TransformValue((-0.204043)*i);
        blue_map[i].blue=TransformValue(1.057311*i);
      }
      break;
    }
    case Rec601YCbCrColorspace:
    {
      /*
        Y'CbCr based on ITU-R 601 Luma

        Initialize Y'CbCr tables:

          R' = Y'            +1.402000*Cr
          G' = Y'-0.344136*Cb-0.714136*Cr
          B' = Y'+1.772000*Cb

        Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxMap.
      */
      for (i=0; i <= (long) MaxMap; i++)
      {
        /* Y */
        red_map[i].red=TransformValue(i);
        green_map[i].red=TransformValue(0);
        blue_map[i].red=TransformValue((1.402000*0.5)*(2.0*i-MaxMap));
        /* Pb */
        red_map[i].green=TransformValue(i);
        green_map[i].green=TransformValue((-0.344136*0.5)*(2.0*i-MaxMap));
        blue_map[i].green=TransformValue((-0.714136*0.5)*(2.0*i-MaxMap));
        /* Pr */
        red_map[i].blue=TransformValue(i);
        green_map[i].blue=TransformValue((1.772000*0.5)*(2.0*i-MaxMap));
        blue_map[i].blue=TransformValue(0);
      }
      break;
    }
    case Rec709YCbCrColorspace:
    {
      /*
        Y'CbCr based on ITU-R 709 Luma.

          R' = Y'            +1.574800*Cr
          G' = Y'-0.187324*Cb-0.468124*Cr
          B' = Y'+1.855600*Cb

        Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxMap.
      */
      for (i=0; i <= (long) MaxMap; i++)
      {
        /* Y */
        red_map[i].red=TransformValue(i);
        green_map[i].red=TransformValue(0);
        blue_map[i].red=TransformValue((1.5748*0.5)*(2.0*i-MaxMap));
        /* Pb */
        red_map[i].green=TransformValue(i);
        green_map[i].green=TransformValue((-0.187324*0.5)*(2.0*i-MaxMap));
        blue_map[i].green=TransformValue((-0.468124*0.5)*(2.0*i-MaxMap));
        /* Pr */
        red_map[i].blue=TransformValue(i);
        green_map[i].blue=TransformValue((1.8556*0.5)*(2.0*i-MaxMap));
        blue_map[i].blue=TransformValue(0);
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Kodak PhotoYCC Color Space.

        Initialize YCC tables:

          R = Y            +1.340762*C2
          G = Y-0.317038*C1-0.682243*C2
          B = Y+1.632639*C1

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      rgb_map=YCCMap;
      rgb_map_max_index=350;
      for (i=0; i <= (long) MaxMap; i++)
      {
        red_map[i].red=TransformValue(1.3584*i);
        green_map[i].red=TransformValue(0);
        blue_map[i].red=TransformValue(1.8215*(i-ScaleCharToMap(137)));
        red_map[i].green=TransformValue(1.3584*i);
        green_map[i].green=TransformValue((-0.4302726)*(i-ScaleCharToMap(156)));
        blue_map[i].green=TransformValue((-0.9271435)*(i-ScaleCharToMap(137)));
        red_map[i].blue=TransformValue(1.3584*i);
        green_map[i].blue=TransformValue(2.2179*(i-ScaleCharToMap(156)));
        blue_map[i].blue=TransformValue(0);
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          R = Y+0.95620*I+0.62140*Q
          G = Y-0.27270*I-0.64680*Q
          B = Y-1.10370*I+1.70060*Q

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxMap.
      */
      for (i=0; i <= (long) MaxMap; i++)
      {
        red_map[i].red=TransformValue(i);
        green_map[i].red=TransformValue(0.4781*(2.0*i-MaxMap));
        blue_map[i].red=TransformValue(0.3107*(2.0*i-MaxMap));
        red_map[i].green=TransformValue(i);
        green_map[i].green=TransformValue((-0.13635)*(2.0*i-MaxMap));
        blue_map[i].green=TransformValue((-0.3234)*(2.0*i-MaxMap));
        red_map[i].blue=TransformValue(i);
        green_map[i].blue=TransformValue((-0.55185)*(2.0*i-MaxMap));
        blue_map[i].blue=TransformValue(0.8503*(2.0*i-MaxMap));
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize Y'PbPr tables using ITU-R 601 luma:

          R = Y            +1.402000*C2
          G = Y-0.344136*C1+0.714136*C2
          B = Y+1.772000*C1

        Pb and Pr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxMap.
      */
      for (i=0; i <= (long) MaxMap; i++)
      {
        red_map[i].red=TransformValue(i);
        green_map[i].red=TransformValue(0);
        blue_map[i].red=TransformValue(0.701*(2.0*i-MaxMap));
        red_map[i].green=TransformValue(i);
        green_map[i].green=TransformValue((-0.172068)*(2.0*i-MaxMap));
        blue_map[i].green=TransformValue(0.357068*(2.0*i-MaxMap));
        red_map[i].blue=TransformValue(i);
        green_map[i].blue=TransformValue(0.886*(2.0*i-MaxMap));
        blue_map[i].blue=TransformValue(0);
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          R = Y          +1.13980*V
          G = Y-0.39380*U-0.58050*V
          B = Y+2.02790*U

        U and V, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxMap.
      */
      for (i=0; i <= (long) MaxMap; i++)
      {
        red_map[i].red=TransformValue(i);
        green_map[i].red=TransformValue(0);
        blue_map[i].red=TransformValue(0.5699*(2.0*i-MaxMap));
        red_map[i].green=TransformValue(i);
        green_map[i].green=TransformValue((-0.1969)*(2.0*i-MaxMap));
        blue_map[i].green=TransformValue((-0.29025)*(2.0*i-MaxMap));
        red_map[i].blue=TransformValue(i);
        green_map[i].blue=TransformValue(1.01395*(2.0*i-MaxMap));
        blue_map[i].blue=TransformValue(0);
      }
      break;
    }
  }

#if 0
  /*
    Dump tables
  */
  for (i=0; i <= (long) MaxMap; i++)
    {
      printf("%5ld: red_map(%g,%g,%g) green_map(%g,%g,%g) blue_map(%g,%g,%g)\n",
             i,
             ((red_map[i].red)),
             ((red_map[i].green)),
             ((red_map[i].blue)),
             
             ((green_map[i].red)),
             ((green_map[i].green)),
             ((green_map[i].blue)),
             
             ((blue_map[i].red)),
             ((blue_map[i].green)),
             ((blue_map[i].blue)));
    }
#endif

  /*
    Convert to RGB.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      ExceptionInfo
        *exception;

      register PixelPacket
        *q;

      /*
        Convert DirectClass image.
      */
      exception=(&image->exception);
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        for (x=(long) image->columns; x > 0; x--)
        {
          RGBTransformPacket(q,red_map,green_map,blue_map,rgb_map,
                             rgb_map_max_index);
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(progress_message,y,image->rows,exception))
            {
              status=MagickFail;
              break;
            }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      register PixelPacket
        *q;

      q=image->colormap;
      for (i=(long) image->colors; i > 0; i--)
      {
        RGBTransformPacket(q,red_map,green_map,blue_map,rgb_map,
                           rgb_map_max_index);
        q++;
      }
      (void) SyncImagePixels(image);
      break;
    }
  }
  image->is_grayscale=is_grayscale;
  image->colorspace=RGBColorspace;

  /*
    Free allocated memory.
  */
  MagickFreeMemory(blue_map);
  MagickFreeMemory(green_map);
  MagickFreeMemory(red_map);
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Colorspace transform completed"); 
  return(status);
}
