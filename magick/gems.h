/*
  Graphic gems define declarations.
*/
extern Export ColorPacket
  InterpolateColor(Image *,double,double);

extern Export Quantum
  GenerateNoise(const Quantum,const NoiseType);

extern Export unsigned short
  InsidePrimitive(PrimitiveInfo *,AnnotateInfo *,const PointInfo *,Image *);

extern Export void
  Contrast(const int,Quantum *,Quantum *,Quantum *),
  HSLTransform(double,const double,const double,Quantum *,Quantum *,Quantum *),
  Hull(const int,const int,const int,const unsigned int,const unsigned int,
    Quantum *,Quantum *),
  Modulate(double,double,double,Quantum *,Quantum *,Quantum *),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  Upsample(const unsigned int,const unsigned int,const unsigned int,
    unsigned char *);

