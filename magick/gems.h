/*
  Graphic gems define declarations.
*/
extern Export Quantum
  GenerateNoise(Quantum,NoiseType);
   
extern Export RunlengthPacket
  Interpolate(Image *,RunlengthPacket *,double,double);

extern Export unsigned int
  InsidePrimitive(PrimitiveInfo *,AnnotateInfo *,int,int,Image *);

extern Export void
  Contrast(const int,Quantum *,Quantum *,Quantum *),
  HSLTransform(double,const double,const double,Quantum *,Quantum *,Quantum *),
  Hull(int,int,int,unsigned int,unsigned int,Quantum *,Quantum *),
  Modulate(double,double,double,Quantum *,Quantum *,Quantum *),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  Upsample(unsigned int,unsigned int,unsigned int,unsigned char *);

