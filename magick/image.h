/*
  ImageMagick Quantization declarations.
*/
#ifndef _IMAGE_H
#define _IMAGE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Image define declarations.
*/
#define Alphabet  "`-=[]\\;',./~!@#$%^&*()_+{}|:\"<>?" \
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define ColorMatch(color,target,delta) \
  ((((int) ((color).red)-delta) <= (int) ((target).red)) && \
    ((int) ((target).red) <= ((int) ((color).red)+delta)) && \
   (((int) ((color).green)-delta) <= (int) ((target).green)) && \
    ((int) ((target).green) <= ((int) ((color).green)+delta)) && \
   (((int) ((color).blue)-delta) <= (int) ((target).blue)) && \
    ((int) ((target).blue) <= ((int) ((color).blue)+delta)))
#define DegreesToRadians(x) ((x)*M_PI/180.0)
#define Intensity(color)  \
  ((unsigned int) ((color).red*77+(color).green*150+(color).blue*29) >> 8)
#define IsFaxImage(color)  \
  (IsMonochromeImage(image) && ((image)->columns <= 2560))
#define IsGray(color)  \
  (((color).red == (color).green) && ((color).green == (color).blue))
#define MatteMatch(color,target,delta) \
  (ColorMatch(color,target,delta) && ((color).index == (target).index))
#define MaxColormapSize  65535L
#define MaxStacksize  (1 << 15)
#define MaxTextExtent  1664
#define Opaque  MaxRGB
#define PixelOffset(x,y) image->pixels+((y)*image->columns+(x))
#define Push(up,left,right,delta) \
  if ((p < (segment_stack+MaxStacksize)) && (((up)+(delta)) >= 0) && \
      (((up)+(delta)) < (int) image->rows)) \
    { \
      p->y1=(up); \
      p->x1=(left); \
      p->x2=(right); \
      p->y2=(delta); \
      p++; \
    }
#define RadiansToDegrees(x) ((x)*180/M_PI)
#define ReadQuantum(quantum,p)  \
{  \
  if (image->depth <= 8) \
    quantum=UpScale(*p++); \
  else \
    { \
      value=(*p++) << 8;  \
      value|=(*p++);  \
      quantum=value >> (image->depth-QuantumDepth); \
    } \
}
#define ReadQuantumFile(quantum)  \
{  \
  if (image->depth <= 8) \
    quantum=UpScale(fgetc(image->file)); \
  else \
    quantum=MSBFirstReadShort(image->file) >> (image->depth-QuantumDepth); \
}
#define SharpenFactor  60.0
#define Transparent  0
#define UncompressImage  UncondenseImage
#define WriteQuantum(quantum,q)  \
{  \
  if (image->depth <= 8) \
    *q++=DownScale(quantum); \
  else \
    { \
      value=(quantum); \
      if ((QuantumDepth-image->depth) > 0) \
        value*=257; \
      *q++=value >> 8; \
      *q++=value; \
    } \
}
#define WriteQuantumFile(quantum)  \
{  \
  if (image->depth <= 8) \
    (void) fputc(DownScale(quantum),image->file); \
  else \
    if ((QuantumDepth-image->depth) > 0) \
      MSBFirstWriteShort((quantum)*257,image->file); \
    else \
      MSBFirstWriteShort(quantum,image->file); \
}

#if defined(QuantumLeap)
/*
  Color quantum is [0..65535].
*/
#define DownScale(quantum)  (((unsigned int) (quantum)) >> 8)
#define HexColorFormat "#%04x%04x%04x"
#define MaxRGB  65535L
#define MaxRunlength  65535L
#define QuantumDepth  16
#define UpScale(quantum)  (((unsigned int) (quantum))*257)
#define XDownScale(color)  ((unsigned int) (color))
#define XUpScale(color)  ((unsigned int) (color))

typedef unsigned short Quantum;
#else
/*
  Color quantum is [0..255].
*/
#define DownScale(quantum)  ((unsigned int) (quantum))
#define HexColorFormat "#%02x%02x%02x"
#define MaxRGB  255
#define MaxRunlength  255
#define QuantumDepth  8
#define UpScale(quantum)  ((unsigned int) (quantum))
#define XDownScale(color)  (((unsigned int) (color)) >> 8)
#define XUpScale(color)  (((unsigned int) (color))*257)

typedef unsigned char Quantum;
#endif

/*
  Enumeration declarations.
*/
typedef enum
{
  UndefinedClass,
  DirectClass,
  PseudoClass
} ClassType;

typedef enum
{
  UndefinedColorspace,
  RGBColorspace,
  GRAYColorspace,
  TransparentColorspace,
  OHTAColorspace,
  XYZColorspace,
  YCbCrColorspace,
  YCCColorspace,
  YIQColorspace,
  YPbPrColorspace,
  YUVColorspace,
  CMYKColorspace,
  sRGBColorspace
} ColorspaceType;

typedef enum
{
  UndefinedCompositeOp = 0,
  OverCompositeOp,
  InCompositeOp,
  OutCompositeOp,
  AtopCompositeOp,
  XorCompositeOp,
  PlusCompositeOp,
  MinusCompositeOp,
  AddCompositeOp,
  SubtractCompositeOp,
  DifferenceCompositeOp,
  BumpmapCompositeOp,
  ReplaceCompositeOp,
  ReplaceRedCompositeOp,
  ReplaceGreenCompositeOp,
  ReplaceBlueCompositeOp,
  ReplaceMatteCompositeOp,
  BlendCompositeOp,
  DisplaceCompositeOp,
  AnnotateCompositeOp
} CompositeOperator;

typedef enum
{
  UndefinedCompression,
  NoCompression,
  BZipCompression,
  FaxCompression,
  JPEGCompression,
  LZWCompression,
  RunlengthEncodedCompression,
  ZipCompression
} CompressionType;

typedef enum
{
  UndefinedFilter,
  PointFilter,
  BoxFilter,
  TriangleFilter,
  HermiteFilter,
  HanningFilter,
  HammingFilter,
  BlackmanFilter,
  GaussianFilter,
  QuadraticFilter,
  CubicFilter,
  CatromFilter,
  MitchellFilter,
  LanczosFilter,
  BesselFilter,
  SincFilter
} FilterType;

typedef enum
{
  UndefinedId,
  ImageMagickId
} IdType;

typedef enum
{
  UndefinedType,
  BilevelType,
  GrayscaleType,
  PaletteType,
  TrueColorType,
  MatteType,
  ColorSeparationType
} ImageType;

typedef enum
{
  UndefinedInterlace,
  NoInterlace,
  LineInterlace,
  PlaneInterlace,
  PartitionInterlace
} InterlaceType;

typedef enum
{
  UndefinedLayer,
  RedLayer,
  GreenLayer,
  BlueLayer,
  MatteLayer
} LayerType;

typedef enum
{
  UniformNoise,
  GaussianNoise,
  MultiplicativeGaussianNoise,
  ImpulseNoise,
  LaplacianNoise,
  PoissonNoise
} NoiseType;

typedef enum
{
  PointMethod = 0,
  ReplaceMethod,
  FloodfillMethod,
  FillToBorderMethod,
  ResetMethod
} PaintMethod;

typedef enum
{
  UndefinedPreview = 0,
  RotatePreview,
  ShearPreview,
  RollPreview,
  HuePreview,
  SaturationPreview,
  BrightnessPreview,
  GammaPreview,
  SpiffPreview,
  DullPreview,
  GrayscalePreview,
  QuantizePreview,
  DespecklePreview,
  ReduceNoisePreview,
  AddNoisePreview,
  SharpenPreview,
  BlurPreview,
  ThresholdPreview,
  EdgeDetectPreview,
  SpreadPreview,
  SolarizePreview,
  ShadePreview,
  RaisePreview,
  SegmentPreview,
  SwirlPreview,
  ImplodePreview,
  WavePreview,
  OilPaintPreview,
  CharcoalDrawingPreview,
  JPEGPreview
} PreviewType;

typedef enum
{
  UndefinedPrimitive = 0,
  PointPrimitive,
  LinePrimitive,
  RectanglePrimitive,
  FillRectanglePrimitive,
  CirclePrimitive,
  FillCirclePrimitive,
  EllipsePrimitive,
  FillEllipsePrimitive,
  PolygonPrimitive,
  FillPolygonPrimitive,
  ColorPrimitive,
  MattePrimitive,
  TextPrimitive,
  ImagePrimitive
} PrimitiveType;

typedef enum
{
  UndefinedIntent,
  SaturationIntent,
  PerceptualIntent,
  AbsoluteIntent,
  RelativeIntent
} RenderingIntent;

typedef enum
{
  UndefinedResolution,
  PixelsPerInchResolution,
  PixelsPerCentimeterResolution
} ResolutionType;

/*
  Typedef declarations.
*/
typedef struct _ColorPacket
{
  Quantum
    red,
    green,
    blue;

  unsigned short
    index;

  unsigned char
    flags;

  char
    key[3];

  unsigned long
    count;
} ColorPacket;

typedef struct _ContributionInfo
{
  int
    pixel;

  double
    weight;
} ContributionInfo;

typedef struct _FilterInfo
{
  double
    (*function)(double),
    width;
} FilterInfo;

typedef struct _FrameInfo
{
  int
    x,
    y;

  unsigned int
    width,
    height;

  int
    inner_bevel,
    outer_bevel;
} FrameInfo;

typedef struct _ImageInfo
{
  FILE
    *file;

  char
    filename[MaxTextExtent],
    magick[MaxTextExtent];

  unsigned int
    affirm,
    temporary,
    subimage,
    subrange;

  char
    *server_name,
    *font,
    *pen,
    *size,
    *tile,
    *density,
    *page,
    *dispose,
    *delay,
    *iterations,
    *texture,
    *view;

  unsigned int
    adjoin,
    depth,
    dither,
    monochrome,
    pointsize,
    quality,
    verbose;

  int
    fuzz;

  ColorspaceType
    colorspace;

  CompressionType
    compression;

  InterlaceType
    interlace;

  ResolutionType
    units;

  FilterType
    filter;

  PreviewType
    preview_type;

  long
    group;

  char
    *background_color,
    *border_color,
    *matte_color,
    *undercolor;

  char
    unique[MaxTextExtent];

  unsigned int
    ping;
} ImageInfo;

typedef struct _MontageInfo
{
  char
    filename[MaxTextExtent];

  char
    *geometry,
    *tile,
    *background_color,
    *border_color,
    *matte_color,
    *title,
    *frame,
    *texture,
    *pen,
    *font;

  unsigned int
    pointsize,
    border_width,
    gravity,
    shadow;

  CompositeOperator
    compose;
} MontageInfo;

typedef struct _PointInfo
{
  float
    x,
    y,
    z;
} PointInfo;

typedef struct _PrimitiveInfo
{
  PrimitiveType
    primitive;

  unsigned int
    coordinates;

  int
    x,
    y;

  PaintMethod
    method;

  char
    *text;
} PrimitiveInfo;

typedef struct _RectangleInfo
{
  unsigned int
    width,
    height;

  int
    x,
    y;
} RectangleInfo;

typedef struct _RunlengthPacket
{
  Quantum
    red,
    green,
    blue,
    length;

  unsigned short
    index;
} RunlengthPacket;

typedef struct _SegmentInfo
{
  int
    x1,
    y1,
    x2,
    y2;
} SegmentInfo;

typedef struct _AnnotateInfo
{
  char
    *server_name,
    *density,
    *border_color,
    *font,
    *pen;

  unsigned int
    pointsize,
    linewidth,
    gravity;

  char
    *geometry,
    *text,
    *box,
    *primitive,
    *font_name;

  RectangleInfo
    bounds;
} AnnotateInfo;

typedef struct _ChromaticityInfo
{
  PointInfo
    red_primary,
    green_primary,
    blue_primary,
    white_point;
} ChromaticityInfo;

typedef struct _ProfileInfo
{
  unsigned int
    length;

  unsigned char
    *info;
} ProfileInfo;

typedef struct _Image
{
  FILE
    *file;

  int
    exempt,
    status,
    temporary;

  char
    filename[MaxTextExtent];

  long int
    filesize;

  int
    pipe;

  char
    magick[MaxTextExtent],
    *comments,
    *label,
    *text;

  IdType
    id;

  ClassType
#if defined(__cplusplus) || defined(c_plusplus)
    c_class;
#else
    class;
#endif

  unsigned int
    matte;

  CompressionType
    compression;

  unsigned int
    columns,
    rows,
    depth;

  int
    offset;

  RectangleInfo
    tile_info;

  InterlaceType
    interlace;

  unsigned int
    scene,
    number_scenes;

  char
    *montage,
    *directory;

  ColorPacket
    *colormap;

  unsigned int
    colors;

  RenderingIntent
    rendering_intent;

  double
    gamma;

  ChromaticityInfo
    chromaticity;

  ProfileInfo
    color_profile,
    iptc_profile;

  ResolutionType
    units;

  double
    x_resolution,
    y_resolution;

  unsigned int
    mean_error_per_pixel;

  double
    normalized_mean_error,
    normalized_maximum_error;

  unsigned long
    total_colors;

  char
    *signature;

  RunlengthPacket
    *pixels,
    *packet;

  unsigned long
    packets;

  unsigned int
    packet_size;

  unsigned char
    *packed_pixels;

  ColorPacket
    background_color,
    border_color,
    matte_color;

  char
    *geometry,
    *page;

  unsigned int
    dispose,
    delay,
    iterations;

  int
    fuzz;

  FilterType
    filter;

  long int
    magick_time;

  char
    magick_filename[MaxTextExtent];

  unsigned int
    magick_columns,
    magick_rows;

  unsigned int
    orphan;

  struct _Image
    *previous,
    *list,
    *next;
} Image;

/*
  Image utilities methods.
*/
extern Export Image
  *AddNoiseImage(Image *,NoiseType),
  *AllocateImage(const ImageInfo *),
  *AppendImages(Image *,const unsigned int),
  *AverageImages(Image *),
  *BlurImage(Image *,const double),
  *BorderImage(Image *,const RectangleInfo *),
  *ChopImage(Image *,RectangleInfo *),
  *CloneImage(Image *,const unsigned int,const unsigned int,const unsigned int),
  *CropImage(Image *,RectangleInfo *),
  *DespeckleImage(Image *),
  *EdgeImage(Image *,const double),
  *EmbossImage(Image *),
  *EnhanceImage(Image *),
  *FlipImage(Image *),
  *FlopImage(Image *),
  *FrameImage(Image *,const FrameInfo *),
  *ImplodeImage(Image *,double),
  **ListToGroupImage(Image *,unsigned int *),
  *MagnifyImage(Image *),
  *MinifyImage(Image *),
  *MontageImages(Image *,const MontageInfo *),
  *MorphImages(Image *,const unsigned int),
  *OilPaintImage(Image *,const unsigned int),
  *ReadAVSImage(const ImageInfo *image_info),
  *ReadBMPImage(const ImageInfo *image_info),
  *ReadCMYKImage(const ImageInfo *image_info),
  *ReadDCMImage(const ImageInfo *image_info),
  *ReadDPSImage(const ImageInfo *image_info),
  *ReadFAXImage(const ImageInfo *image_info),
  *ReadFITSImage(const ImageInfo *image_info),
  *ReadFPXImage(const ImageInfo *image_info),
  *ReadGIFImage(const ImageInfo *image_info),
  *ReadGRADATIONImage(const ImageInfo *image_info),
  *ReadGRAYImage(const ImageInfo *image_info),
  *ReadHDFImage(const ImageInfo *image_info),
  *ReadHISTOGRAMImage(const ImageInfo *image_info),
  *ReadICCImage(const ImageInfo *image_info),
  *ReadICONImage(const ImageInfo *image_info),
  *ReadImage(ImageInfo *),
  *ReadIPTCImage(const ImageInfo *image_info),
  *ReadJBIGImage(const ImageInfo *image_info),
  *ReadJPEGImage(const ImageInfo *image_info),
  *ReadLABELImage(const ImageInfo *image_info),
  *ReadLOGOImage(const ImageInfo *image_info),
  *ReadMAPImage(const ImageInfo *image_info),
  *ReadMIFFImage(const ImageInfo *image_info),
  *ReadMONOImage(const ImageInfo *image_info),
  *ReadMTVImage(const ImageInfo *image_info),
  *ReadNULLImage(const ImageInfo *image_info),
  *ReadPCDImage(const ImageInfo *image_info),
  *ReadPCLImage(const ImageInfo *image_info),
  *ReadPCXImage(const ImageInfo *image_info),
  *ReadPDFImage(const ImageInfo *image_info),
  *ReadPICTImage(const ImageInfo *image_info),
  *ReadPIXImage(const ImageInfo *image_info),
  *ReadPLASMAImage(const ImageInfo *image_info),
  *ReadPNGImage(const ImageInfo *image_info),
  *ReadPNMImage(const ImageInfo *image_info),
  *ReadPSDImage(const ImageInfo *image_info),
  *ReadPSImage(const ImageInfo *image_info),
  *ReadRGBImage(const ImageInfo *image_info),
  *ReadRLAImage(const ImageInfo *image_info),
  *ReadRLEImage(const ImageInfo *image_info),
  *ReadSGIImage(const ImageInfo *image_info),
  *ReadSTEGANOImage(const ImageInfo *image_info),
  *ReadSUNImage(const ImageInfo *image_info),
  *ReadTGAImage(const ImageInfo *image_info),
  *ReadTIFFImage(const ImageInfo *image_info),
  *ReadTILEImage(const ImageInfo *image_info),
  *ReadTIMImage(const ImageInfo *image_info),
  *ReadTTFImage(const ImageInfo *image_info),
  *ReadTXTImage(const ImageInfo *image_info),
  *ReadUILImage(const ImageInfo *image_info),
  *ReadUYVYImage(const ImageInfo *image_info),
  *ReadVICARImage(const ImageInfo *image_info),
  *ReadVIDImage(const ImageInfo *image_info),
  *ReadVIFFImage(const ImageInfo *image_info),
  *ReadXImage(const ImageInfo *image_info),
  *ReadXBMImage(const ImageInfo *image_info),
  *ReadXCImage(const ImageInfo *image_info),
  *ReadXPMImage(const ImageInfo *image_info),
  *ReadXWDImage(const ImageInfo *image_info),
  *ReadYUVImage(const ImageInfo *image_info),
  *ReduceNoiseImage(Image *),
  *RollImage(Image *,int,int),
  *RotateImage(Image *,double,const unsigned int,const unsigned int),
  *SampleImage(Image *,unsigned int,unsigned int),
  *ScaleImage(Image *,const unsigned int,const unsigned int),
  *ShadeImage(Image *,const unsigned int,double,double),
  *SharpenImage(Image *,const double),
  *ShearImage(Image *,double,double,const unsigned int),
  *SpreadImage(Image *,unsigned int),
  *SteganoImage(Image *,Image *),
  *StereoImage(Image *,const Image *),
  *SwirlImage(Image *,double),
  *WaveImage(Image *,const double,const double),
  *ZoomImage(Image *,const unsigned int,const unsigned int);

extern Export ImageType
  GetImageType(const ImageInfo *,Image *);

extern Export int
  ParseImageGeometry(const char *,int *,int *,unsigned int *,unsigned int *);

extern Export unsigned int
  IsGeometry(const char *),
  IsGrayImage(Image *),
  IsMonochromeImage(Image *),
  IsPseudoClass(Image *),
  IsSubimage(const char *,const unsigned int),
  IsTainted(Image *),
  MapImage(Image *,Image *,const unsigned int),
  MapImages(Image *,Image *,const unsigned int),
  PingImage(ImageInfo *,unsigned int *,unsigned int *),
  PlasmaImage(Image *,SegmentInfo *,int,int),
  UncondenseImage(Image *),
  WriteAVSImage(const ImageInfo *image_info,Image *image),
  WriteBMPImage(const ImageInfo *image_info,Image *image),
  WriteCMYKImage(const ImageInfo *image_info,Image *image),
  WriteEPTImage(const ImageInfo *image_info,Image *image),
  WriteFAXImage(const ImageInfo *image_info,Image *image),
  WriteFITSImage(const ImageInfo *image_info,Image *image),
  WriteFPXImage(const ImageInfo *image_info,Image *image),
  WriteGIFImage(const ImageInfo *image_info,Image *image),
  WriteGRAYImage(const ImageInfo *image_info,Image *image),
  WriteHDFImage(const ImageInfo *image_info,Image *image),
  WriteHISTOGRAMImage(const ImageInfo *image_info, Image *image),
  WriteHTMLImage(const ImageInfo *image_info,Image *image),
  WriteICCImage(const ImageInfo *image_info,Image *image),
  WriteIPTCImage(const ImageInfo *image_info,Image *image),
  WriteImage(ImageInfo *,Image *),
  WriteJBIGImage(const ImageInfo *image_info,Image *image),
  WriteJPEGImage(const ImageInfo *image_info,Image *image),
  WriteLOGOImage(const ImageInfo *image_info,Image *image),
  WriteMAPImage(const ImageInfo *image_info,Image *image),
  WriteMATTEImage(const ImageInfo *image_info,Image *image),
  WriteMIFFImage(const ImageInfo *image_info,Image *image),
  WriteMONOImage(const ImageInfo *image_info,Image *image),
  WriteMTVImage(const ImageInfo *image_info,Image *image),
  WriteNTImage(const ImageInfo *,Image *),
  WritePCDImage(const ImageInfo *image_info,Image *image),
  WritePCLImage(const ImageInfo *image_info,Image *image),
  WritePCXImage(const ImageInfo *image_info,Image *image),
  WritePDFImage(const ImageInfo *image_info,Image *image),
  WritePICTImage(const ImageInfo *image_info,Image *image),
  WritePNGImage(const ImageInfo *image_info,Image *image),
  WritePNMImage(const ImageInfo *image_info,Image *image),
  WritePREVIEWImage(const ImageInfo *image_info,Image *image),
  WritePS2Image(const ImageInfo *image_info,Image *image),
  WritePSDImage(const ImageInfo *image_info,Image *image),
  WritePSImage(const ImageInfo *image_info,Image *image),
  WriteRGBImage(const ImageInfo *image_info,Image *image),
  WriteSGIImage(const ImageInfo *image_info,Image *image),
  WriteSUNImage(const ImageInfo *image_info,Image *image),
  WriteTGAImage(const ImageInfo *image_info,Image *image),
  WriteTIFFImage(const ImageInfo *image_info,Image *image),
  WriteTXTImage(const ImageInfo *image_info,Image *image),
  WriteUILImage(const ImageInfo *image_info,Image *image),
  WriteUYVYImage(const ImageInfo *image_info,Image *image),
  WriteVICARImage(const ImageInfo *image_info,Image *image),
  WriteVIFFImage(const ImageInfo *image_info,Image *image),
  WriteXBMImage(const ImageInfo *image_info,Image *image),
  WriteXImage(const ImageInfo *image_info,Image *image),
  WriteXPMImage(const ImageInfo *image_info,Image *image),
  WriteXWDImage(const ImageInfo *image_info,Image *image),
  WriteYUVImage(const ImageInfo *image_info,Image *image);

extern Export void
  AllocateNextImage(const ImageInfo *,Image *),
  AnnotateImage(Image *,AnnotateInfo *),
  CloseImage(Image *),
  CoalesceImages(Image *),
  ColorFloodfillImage(Image *,const RunlengthPacket *,const char *,int x,int y,
    const PaintMethod),
  ColorizeImage(Image *,const char *,const char *),
  CommentImage(Image *,const char *),
  CompositeImage(Image *,const CompositeOperator,Image *,const int,const int),
  CompressColormap(Image *),
  CondenseImage(Image *),
  ContrastImage(Image *,const unsigned int),
  CycleColormapImage(Image *,int),
  DescribeImage(Image *,FILE *,const unsigned int),
  DestroyImage(Image *),
  DestroyImageInfo(ImageInfo *),
  DestroyImages(Image *),
  DrawImage(Image *,AnnotateInfo *),
  EqualizeImage(Image *),
  GammaImage(Image *,const char *),
  GetAnnotateInfo(ImageInfo *,AnnotateInfo *),
  GetImageInfo(ImageInfo *),
  GetMontageInfo(MontageInfo *),
  HSLTransform(double,const double,const double,Quantum *,Quantum *,Quantum *),
  LabelImage(Image *,const char *),
  LayerImage(Image *,const LayerType),
  MatteFloodfillImage(Image *,const RunlengthPacket *,const unsigned int,int x,
    int y,const PaintMethod),
  MatteImage(Image *),
  ModulateImage(Image *,const char *),
  MogrifyImage(ImageInfo *,const int,char **,Image **),
  MogrifyImages(ImageInfo *,const int,char **,Image **),
  NegateImage(Image *,const unsigned int),
  NormalizeImage(Image *),
  OpaqueImage(Image *,const char *,const char *),
  OpenImage(const ImageInfo *,Image *,const char *),
  RaiseImage(Image *,const RectangleInfo *,const int),
  RGBTransformImage(Image *,const ColorspaceType),
  SetImage(Image *),
  SetImageInfo(ImageInfo *,const unsigned int),
  SetNumberScenes(Image *),
  SignatureImage(Image *),
  SolarizeImage(Image *,const double),
  SortColormapByIntensity(Image *),
  SyncImage(Image *),
  TextureImage(Image *,const char *),
  ThresholdImage(Image *,const double),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  TransformImage(Image **,const char *,const char *),
  TransformRGBImage(Image *,const ColorspaceType),
  TransparentImage(Image *,const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
