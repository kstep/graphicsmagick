/*
  Graphic Gems - Graphic Support Methods.
*/
#ifndef _GEMS_H
#define _GEMS_H

/*
  Graphic gems define declarations.
*/
extern MagickExport double
  AffineExpansion(const AffineInfo *),
  Permutate(int,int);

extern MagickExport int
  GetOptimalKernelWidth(const double,const double),
  GetOptimalKernelWidth1D(const double,const double),
  GetOptimalKernelWidth2D(const double,const double);

extern MagickExport PixelPacket
  InterpolateColor(Image *,const double,const double);

extern MagickExport Quantum
  GenerateNoise(const Quantum,const NoiseType);

extern MagickExport void
  Contrast(const int,Quantum *,Quantum *,Quantum *),
  HSLTransform(const double,const double,const double,Quantum *,Quantum *,
    Quantum *),
  Hull(const int,const int,const int,const unsigned int,const unsigned int,
    Quantum *,Quantum *),
  Modulate(double,double,double,Quantum *,Quantum *,Quantum *),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  Upsample(const unsigned int,const unsigned int,const unsigned int,
    unsigned char *);

#endif
