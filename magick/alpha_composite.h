/*
  Copyright (C) 2003, 2005 GraphicsMagick Group
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

static inline PixelPacket BlendComposite(const PixelPacket *p,
  const PixelPacket *q,const double alpha)
{
  double
    color;

  PixelPacket
    composite;

  color=((double) p->red*(MaxRGB-alpha)+q->red*alpha)/MaxRGB;
  composite.red=(Quantum)
    ((color < 0) ? 0 : (color > MaxRGB) ? MaxRGB : color+0.5);
  color=((double) p->green*(MaxRGB-alpha)+q->green*alpha)/MaxRGB;
  composite.green=(Quantum)
    ((color < 0) ? 0 : (color > MaxRGB) ? MaxRGB : color+0.5);
  color=((double) p->blue*(MaxRGB-alpha)+q->blue*alpha)/MaxRGB;
  composite.blue=(Quantum)
    ((color < 0) ? 0 : (color > MaxRGB) ? MaxRGB : color+0.5);
  composite.opacity=p->opacity;
  return(composite);
}

static inline PixelPacket AlphaComposite(const PixelPacket *p,
  const double alpha,const PixelPacket *q,const double beta)
{
  PixelPacket
    composite;

  double
    MaxRGB_alpha,
    MaxRGB_beta;

  MaxRGB_alpha=MaxRGB-alpha;
  MaxRGB_beta=MaxRGB-beta;
  composite.red=(Quantum)
    ((MaxRGB_alpha*p->red+alpha*MaxRGB_beta*q->red/MaxRGB)/MaxRGB+0.5);
  composite.green=(Quantum)
    ((MaxRGB_alpha*p->green+alpha*MaxRGB_beta*q->green/MaxRGB)/MaxRGB+0.5);
  composite.blue=(Quantum)
    ((MaxRGB_alpha*p->blue+alpha*MaxRGB_beta*q->blue/MaxRGB)/MaxRGB+0.5);
  composite.opacity=(Quantum)
    (MaxRGB-(MaxRGB_alpha+alpha*MaxRGB_beta/MaxRGB)+0.5);
  return(composite);
}
#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_ALPHA_COMPOSITE_H */
