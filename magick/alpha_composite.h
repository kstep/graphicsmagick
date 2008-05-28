/*
  Copyright (C) 2003, 2005, 2008 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Alpha Composite Methods.
*/
#ifndef _MAGICK_ALPHA_COMPOSITE_H
#define _MAGICK_ALPHA_COMPOSITE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#if defined(MAGICK_IMPLEMENTATION)

static inline magick_uint32_t BlendQuantumOpacity(magick_uint32_t q,
  const magick_uint32_t opacity)
{
  magick_uint32_t
    result = 0U;

  if (opacity != 0U)
    {
#if QuantumDepth > 16
      result = ((magick_uint64_t) opacity*q)/MaxRGB;
#else
      result = ((magick_uint32_t) opacity*q)/MaxRGB;
#endif
    }
  return result;
}

static inline PixelPacket BlendComposite(const PixelPacket *p,
  const PixelPacket *q,const double alpha)
{
  double
    color;

  PixelPacket
    composite;

  color=((double) p->red*(MaxRGBDouble-alpha)+q->red*alpha)/MaxRGBDouble;
  composite.red=RoundDoubleToQuantum(color);

  color=((double) p->green*(MaxRGBDouble-alpha)+q->green*alpha)/MaxRGBDouble;
  composite.green=RoundDoubleToQuantum(color);

  color=((double) p->blue*(MaxRGBDouble-alpha)+q->blue*alpha)/MaxRGBDouble;
  composite.blue=RoundDoubleToQuantum(color);

  composite.opacity=p->opacity;
  return(composite);
}

/*
  Alpha compose pixel 'change' over pixel 'source'.

  The result will be the union of the two image shapes, with
  opaque areas of change-image obscuring base-image in the
  region of overlap.
*/
#define  MagickAlphaCompositeQuantum(change,change_alpha,base,base_alpha) \
   (1.0-(change_alpha/MaxRGBDouble))*(double) change+(1.0-(base_alpha/MaxRGBDouble))*(double) base*(change_alpha/MaxRGBDouble)
static inline PixelPacket AlphaComposite(const PixelPacket *change,
  const double change_alpha,const PixelPacket *base,const double base_alpha)
{
  PixelPacket
    composite;

  double
    gamma,
    value;

  composite=*base;
  if (change_alpha != TransparentOpacity)
    {
      gamma=1.0-(change_alpha/MaxRGBDouble)*(base_alpha/MaxRGBDouble);
      
      value=MaxRGBDouble*(1.0-gamma);
      composite.opacity=RoundDoubleToQuantum(value);
      
      gamma=1.0/(gamma <= MagickEpsilon ? 1.0 : gamma);
      
      value=gamma*MagickAlphaCompositeQuantum(change->red,change_alpha,composite.red,base_alpha);
      composite.red=RoundDoubleToQuantum(value);
      
      value=gamma*MagickAlphaCompositeQuantum(change->green,change_alpha,composite.green,base_alpha);
      composite.green=RoundDoubleToQuantum(value);
      
      value=gamma*MagickAlphaCompositeQuantum(change->blue,change_alpha,composite.blue,base_alpha);
      composite.blue=RoundDoubleToQuantum(value);
    }

  return composite;
}

/*
  The result is the same shape as base-image, with change-image
  obscuring base-image where the image shapes overlap. Note this
  differs from over because the portion of change-image outside
  base-image's shape does not appear in the result.
*/
static inline PixelPacket AtopComposite(const PixelPacket *base,
                                        const PixelPacket *change)
{
  double
    color,
    opacity;

  PixelPacket
    composite;

  opacity=((double)(MaxRGBDouble-change->opacity)*
           (MaxRGBDouble-base->opacity)+(double) change->opacity*
           (MaxRGBDouble-base->opacity))/MaxRGBDouble;
  
  color=((double) (MaxRGBDouble-change->opacity)*
         (MaxRGBDouble-base->opacity)*change->red/MaxRGBDouble+(double)
         change->opacity*(MaxRGBDouble-base->opacity)*
         base->red/MaxRGBDouble)/opacity;
  composite.red=RoundDoubleToQuantum(color);
  
  color=((double) (MaxRGBDouble-change->opacity)*
         (MaxRGBDouble-base->opacity)*change->green/MaxRGBDouble+(double)
         change->opacity*(MaxRGBDouble-base->opacity)*
         base->green/MaxRGBDouble)/opacity;
  composite.green=RoundDoubleToQuantum(color);
  
  color=((double) (MaxRGBDouble-change->opacity)*
         (MaxRGBDouble-base->opacity)*change->blue/MaxRGBDouble+(double)
         change->opacity*(MaxRGBDouble-base->opacity)*
         base->blue/MaxRGBDouble)/opacity;
  composite.blue=RoundDoubleToQuantum(color);
  
  composite.opacity=MaxRGB-RoundDoubleToQuantum(opacity);

  return composite;
}


#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_ALPHA_COMPOSITE_H */
