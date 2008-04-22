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

  color=((double) p->red*(MaxRGB-alpha)+q->red*alpha)/MaxRGB;
  composite.red=RoundSignedToQuantum(color);

  color=((double) p->green*(MaxRGB-alpha)+q->green*alpha)/MaxRGB;
  composite.green=RoundSignedToQuantum(color);

  color=((double) p->blue*(MaxRGB-alpha)+q->blue*alpha)/MaxRGB;
  composite.blue=RoundSignedToQuantum(color);

  composite.opacity=p->opacity;
  return(composite);
}

static inline PixelPacket AlphaComposite(const PixelPacket *p,
  const double alpha,const PixelPacket *q,const double beta)
{
  PixelPacket
    composite;

  double
    color,
    MaxRGB_alpha,
    MaxRGB_beta;

  MaxRGB_alpha=MaxRGB-alpha;
  MaxRGB_beta=MaxRGB-beta;

  color=(MaxRGB_alpha*p->red+alpha*MaxRGB_beta*q->red/MaxRGB)/MaxRGB;
  composite.red=RoundSignedToQuantum(color);

  color=(MaxRGB_alpha*p->green+alpha*MaxRGB_beta*q->green/MaxRGB)/MaxRGB;
  composite.green=RoundSignedToQuantum(color);

  color=(MaxRGB_alpha*p->blue+alpha*MaxRGB_beta*q->blue/MaxRGB)/MaxRGB;
  composite.blue=RoundSignedToQuantum(color);

  color=MaxRGB-(MaxRGB_alpha+alpha*MaxRGB_beta/MaxRGB);
  composite.opacity=RoundSignedToQuantum(color);

  return(composite);
}
#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_ALPHA_COMPOSITE_H */
