/*
  Graphic Gems - Graphic Support Methods.
*/
#ifndef _MAGICK_GEM_H
#define _MAGICK_GEM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Graphic gems define declarations.
*/
extern MagickExport double
  ExpandAffine(const AffineMatrix *),
  Permutate(long,long);

extern MagickExport int
  GetOptimalKernelWidth(const double,const double),
  GetOptimalKernelWidth1D(const double,const double),
  GetOptimalKernelWidth2D(const double,const double);

extern MagickExport PixelPacket
  AlphaComposite(const CompositeOperator,const PixelPacket *,const double,
    const PixelPacket *,const double),
  InterpolateColor(Image *,const double,const double);

extern MagickExport Quantum
  GenerateNoise(const Quantum,const NoiseType);

extern MagickExport void
  Contrast(const int,Quantum *,Quantum *,Quantum *),
  HSLTransform(const double,const double,const double,Quantum *,Quantum *,
    Quantum *),
  Hull(const long,const long,const unsigned long,const unsigned long,Quantum *,
    Quantum *,const int),
  IdentityAffine(AffineMatrix *),
  Modulate(double,double,double,Quantum *,Quantum *,Quantum *),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  Upsample(const unsigned long,const unsigned long,const unsigned long,
    unsigned char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
