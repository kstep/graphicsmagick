/*
  Image define declarations.
*/
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
#define PixelOffset(x,y) image->pixels+((y)*image->columns+(x))
#define Push(up,left,right,delta) \
  if ((p < (segment_stack+MaxStacksize)) && (((up)+(delta)) >= 0) && \
      (((up)+(delta)) < image->rows)) \
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
  if (image->depth == 8) \
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
  if (image->depth == 8) \
    quantum=UpScale(fgetc(image->file)); \
  else \
    quantum=MSBFirstReadShort(image->file) >> (image->depth-QuantumDepth); \
}
#define SharpenFactor  60.0
#define Transparent  0
#define UncompressImage  UncondenseImage
#define WriteQuantum(quantum,q)  \
{  \
  if (image->depth == 8) \
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
  if (image->depth == 8) \
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
#define Opaque  65535L
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
#define Opaque  255
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
  UndefinedAlignment,
  LeftAlignment,
  CenterAlignment,
  RightAlignment
} AlignmentType;

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
  DisplaceCompositeOp
} CompositeOperator;

typedef enum
{
  UndefinedCompression,
  NoCompression,
  BZipCompression,
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

  unsigned char
    flags;

  char
    key[3];

  unsigned short
    index;
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
    subimage,
    subrange;

  char
    *server_name,
    *font,
    *pen,
    *box,
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
    adjoin;

  ColorspaceType
    colorspace;

  CompressionType
    compression;

  unsigned int
    dither;

  InterlaceType
    interlace;

  unsigned int
    monochrome,
    pointsize,
    quality,
    verbose;

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

typedef struct _QuantizeInfo
{
  unsigned int
    number_colors,
    tree_depth,
    dither;

  ColorspaceType
    colorspace;
} QuantizeInfo;

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
  ImageInfo
    *image_info;

  char
    *geometry,
    *text,
    *primitive;

  unsigned int
    linewidth;

  AlignmentType
    alignment;

  unsigned int
    height;
} AnnotateInfo;

typedef struct _ChromaticityInfo
{
  PointInfo
    red_primary,
    green_primary,
    blue_primary,
    white_point;
} ChromaticityInfo;

typedef struct _ColorProfileInfo
{
  unsigned int
    length;

  unsigned char
    *info;
} ColorProfileInfo;

typedef struct _Image
{
  FILE
    *file,
    *ps_file;

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

  ColorProfileInfo
    color_profile;

  ResolutionType
    units;

  float
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

  long int
    magick_time;

  char
    magick_filename[MaxTextExtent];

  unsigned int
    magick_columns,
    magick_rows;

  char
    *geometry,
    *page;

  unsigned int
    dispose,
    delay,
    iterations;

  FilterType
    filter;

  unsigned int
    orphan;

  struct _Image
    *previous,
    *list,
    *next;
} Image;

/*
  Image utilities routines.
*/
extern Export void
  CommentImage(Image *,char *),
  LabelImage(Image *,char *);

extern Export Image
  *AddNoiseImage(Image *,NoiseType),
  *AllocateImage(const ImageInfo *),
  *AppendImages(Image *,unsigned int),
  *AverageImages(Image *),
  *BorderImage(Image *,RectangleInfo *),
  *BlurImage(Image *,double),
  *ChopImage(Image *,RectangleInfo *),
  *CloneImage(Image *,const unsigned int,const unsigned int,const unsigned int),
  *CropImage(Image *,RectangleInfo *),
  *DespeckleImage(Image *),
  *EdgeImage(Image *,double),
  *EmbossImage(Image *),
  *EnhanceImage(Image *),
  *FlipImage(Image *),
  *FlopImage(Image *),
  *FrameImage(Image *,FrameInfo *),
  *ImplodeImage(Image *,double),
  **ListToGroupImage(Image *,unsigned int *),
  *MagnifyImage(Image *),
  *MinifyImage(Image *),
  *MorphImages(Image *,unsigned int),
  *OilPaintImage(Image *,const unsigned int),
  *ReadImage(ImageInfo *),
  *ReadPICTImage(ImageInfo *),
  *ReduceNoiseImage(Image *),
  *RollImage(Image *,int,int),
  *RotateImage(Image *,double,const unsigned int,const unsigned int),
  *SampleImage(Image *,unsigned int,unsigned int),
  *ScaleImage(Image *,const unsigned int,const unsigned int),
  *ShadeImage(Image *,unsigned int,double,double),
  *SharpenImage(Image *,double),
  *ShearImage(Image *,double,double,const unsigned int),
  *SpreadImage(Image *,unsigned int),
  *StereoImage(Image *,Image *),
  *SwirlImage(Image *,double),
  *WaveImage(Image *,double,double),
  *ZoomImage(Image *,const unsigned int,const unsigned int);

extern Export int
  ParseImageGeometry(char *,int *,int *,unsigned int *,unsigned int *);

extern Export unsigned int
  IsGeometry(char *),
  IsGrayImage(Image *),
  IsMonochromeImage(Image *),
  IsPseudoClass(Image *),
  IsSubimage(char *,unsigned int),
  IsTainted(Image *),
  MapImage(Image *,Image *,const unsigned int),
  MapImages(Image *,Image *,const unsigned int),
  PingImage(ImageInfo *,unsigned int *,unsigned int *),
  PlasmaImage(Image *,SegmentInfo *,int,int),
  QuantizationError(Image *),
  QuantizeImage(QuantizeInfo *,Image *),
  QuantizeImages(QuantizeInfo *,Image *),
  SegmentImage(Image *,const unsigned int,const unsigned int,const double,
    const double),
  UncondenseImage(Image *),
  WriteNTImage(const ImageInfo *,Image *),
  WriteImage(ImageInfo *,Image *);

extern Export void
  AllocateNextImage(const ImageInfo *,Image *),
  AnnotateImage(Image *,AnnotateInfo *),
  BlackImage(Image *),
  CloseImage(Image *),
  ColorFloodfillImage(Image *,const RunlengthPacket *,const ColorPacket *,int x,
    int y,const PaintMethod),
  ColorizeImage(Image *,char *,char *),
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
  GammaImage(Image *,char *),
  GetAnnotateInfo(AnnotateInfo *),
  GetImageInfo(ImageInfo *),
  GetQuantizeInfo(QuantizeInfo *),
  HSLTransform(double,const double,const double,Quantum *,Quantum *,Quantum *),
  LayerImage(Image *,LayerType),
  MatteFloodfillImage(Image *,const RunlengthPacket *,const unsigned int,int x,
    int y,const PaintMethod),
  ModulateImage(Image *,char *),
  MogrifyImage(ImageInfo *,int,char **,Image **),
  MogrifyImages(ImageInfo *,int,char **,Image **),
  NegateImage(Image *,unsigned int),
  NormalizeImage(Image *),
  NumberColors(Image *,FILE *),
  OpaqueImage(Image *,char *,char *),
  OpenImage(const ImageInfo *,Image *,const char *),
  RaiseImage(Image *,RectangleInfo *,const int),
  RGBTransformImage(Image *,const unsigned int),
  SetImageInfo(ImageInfo *,unsigned int),
  SetNumberScenes(Image *),
  SignatureImage(Image *),
  SolarizeImage(Image *,const double),
  SortColormapByIntensity(Image *),
  SyncImage(Image *),
  TextureImage(Image *,char *),
  ThresholdImage(Image *,double),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  TransformImage(Image **,char *,char *),
  TransformRGBImage(Image *,const unsigned int),
  TransparentImage(Image *,char *);
