/*
  ImageMagick Image Methods.
*/
#ifndef _MAGICK_IMAGE_H
#define _MAGICK_IMAGE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Include declarations.
*/
#include "semaphore.h"
#include "error.h"
#include "timer.h"

/*
  Define declarations.
*/
#if !defined(QuantumDepth)
#define QuantumDepth  16
#define QuantumLeap
#endif

#if (QuantumDepth == 8)
#define MaxColormapSize  256UL
#define MaxMap  255UL
#define MaxRGB  255UL
#define ScaleCharToQuantum(value)  ((Quantum) (value))
#define ScaleLongToQuantum(value)  ((Quantum) ((value)/16843009UL))
#define ScaleMapToQuantum(value)  ((Quantum) (value))
#define ScaleQuantum(quantum)  ((unsigned long) (quantum))
#define ScaleQuantumToChar(quantum)  ((unsigned char) (quantum))
#define ScaleQuantumToLong(quantum)  ((unsigned long) (16843009UL*(quantum)))
#define ScaleQuantumToMap(quantum)  ((unsigned char) (quantum))
#define ScaleQuantumToShort(quantum)  ((unsigned short) (257UL*(quantum)))
#define ScaleShortToQuantum(value)  ((Quantum) ((value)/257UL))
#define ScaleToQuantum(value)  ((unsigned long) (value))

typedef unsigned char Quantum;
#elif (QuantumDepth == 16)
#define MaxColormapSize  65536UL
#define MaxMap 65535UL
#define MaxRGB  65535UL
#define ScaleCharToQuantum(value)  ((Quantum) (257UL*(value)))
#define ScaleLongToQuantum(value)  ((Quantum) ((value)/65537UL))
#define ScaleMapToQuantum(value)  ((Quantum) (value))
#define ScaleQuantum(quantum)  ((unsigned long) ((quantum)/257UL))
#define ScaleQuantumToChar(quantum)  ((unsigned char) ((quantum)/257UL))
#define ScaleQuantumToLong(quantum)  ((unsigned long) (65537UL*(quantum)))
#define ScaleQuantumToMap(quantum)  ((unsigned short) (quantum))
#define ScaleQuantumToShort(quantum)  ((unsigned short) (quantum))
#define ScaleShortToQuantum(value)  ((Quantum) (value))
#define ScaleToQuantum(value)  ((unsigned long) (257UL*(value)))

typedef unsigned short Quantum;
#elif (QuantumDepth == 32)
#define MaxColormapSize  65536UL
#define MaxMap 65535UL
#define MaxRGB  4294967295UL
#define ScaleCharToQuantum(value)  ((Quantum) (16843009UL*(value)))
#define ScaleLongToQuantum(value)  ((Quantum) ((value)))
#define ScaleMapToQuantum(value)  ((Quantum) (65537UL*(value)))
#define ScaleQuantum(quantum)  ((unsigned long) ((quantum)/16843009UL))
#define ScaleQuantumToChar(quantum)  ((unsigned char) ((quantum)/16843009UL))
#define ScaleQuantumToLong(quantum)  ((unsigned long) (quantum))
#define ScaleQuantumToMap(quantum)  ((unsigned short) ((quantum)/65537UL))
#define ScaleQuantumToShort(quantum)  ((unsigned short) ((quantum)/65537UL))
#define ScaleShortToQuantum(value)  ((Quantum) (65537UL*(value)))
#define ScaleToQuantum(value)  ((unsigned long) (16843009UL*(value)))

typedef unsigned int Quantum;
#else
# error "Specified value of QuantumDepth is not supported"
#endif

#define ColorMatch(p,q) (((p)->red == (q)->red) && \
  ((p)->green == (q)->green) && ((p)->blue == (q)->blue))
#define OpaqueOpacity  0UL
#define PixelIntensity(pixel) ((unsigned long) \
  (0.299*(pixel)->red+0.587*(pixel)->green+0.114*(pixel)->blue+0.5))
#define PixelIntensityToQuantum(pixel) ((Quantum) \
  (0.299*(pixel)->red+0.587*(pixel)->green+0.114*(pixel)->blue+0.5))
#define TransparentOpacity  MaxRGB
/*
  Deprecated defines.
*/
#define Downscale(quantum)  ScaleQuantumToChar(quantum)
#define Intensity(color)  PixelIntensityToQuantum(color)
#define RunlengthEncodedCompression RLECompression
#define Upscale(value)  ScaleCharToQuantum(value)
#define XDownscale(value)  ScaleShortToQuantum(value)
#define XUpscale(quantum)  ScaleQuantumToShort(quantum)

/*
  Enum declarations.
*/
typedef enum
{
  UndefinedChannel,
  RedChannel,
  CyanChannel,
  GreenChannel,
  MagentaChannel,
  BlueChannel,
  YellowChannel,
  OpacityChannel,
  BlackChannel,
  MatteChannel
} ChannelType;

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
  UndefinedCompliance = 0x0000,
  NoCompliance = 0x0000,
  SVGCompliance = 0x0001,
  X11Compliance = 0x0002,
  XPMCompliance = 0x0004,
  AllCompliance = 0xffff
} ComplianceType;

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
  MultiplyCompositeOp,
  BumpmapCompositeOp,
  CopyCompositeOp,
  CopyRedCompositeOp,
  CopyGreenCompositeOp,
  CopyBlueCompositeOp,
  CopyOpacityCompositeOp,
  ClearCompositeOp,
  DissolveCompositeOp,
  DisplaceCompositeOp,
  ModulateCompositeOp,
  ThresholdCompositeOp,
  NoCompositeOp,
  DarkenCompositeOp,
  LightenCompositeOp,
  HueCompositeOp,
  SaturateCompositeOp,
  ColorizeCompositeOp,
  LuminizeCompositeOp,
  ScreenCompositeOp,
  OverlayCompositeOp
} CompositeOperator;

typedef enum
{
  UndefinedCompression,
  NoCompression,
  BZipCompression,
  FaxCompression,
  Group4Compression,
  JPEGCompression,
  LosslessJPEGCompression,
  LZWCompression,
  RLECompression,
  ZipCompression
} CompressionType;

typedef enum
{
  UndefinedDispose,
  NoneDispose,
  BackgroundDispose,
  PreviousDispose
} DisposeType;

typedef enum
{
  UndefinedEndian,
  LSBEndian,
  MSBEndian
} EndianType;

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
} FilterTypes;

typedef enum
{
#undef NoValue
  NoValue = 0x0000,
#undef XValue
  XValue = 0x0001,
#undef YValue
  YValue = 0x0002,
#undef WidthValue
  WidthValue = 0x0004,
#undef HeightValue
  HeightValue = 0x0008,
#undef AllValues
  AllValues = 0x000F,
#undef XNegative
  XNegative = 0x0010,
#undef YNegative
  YNegative = 0x0020,
  PercentValue = 0x1000,
  AspectValue = 0x2000,
  LessValue = 0x4000,
  GreaterValue = 0x8000,
  AreaValue = 0x10000
} GeometryFlags;

typedef enum
{
#undef ForgetGravity
  ForgetGravity,
#undef NorthWestGravity
  NorthWestGravity,
#undef NorthGravity
  NorthGravity,
#undef NorthEastGravity
  NorthEastGravity,
#undef WestGravity
  WestGravity,
#undef CenterGravity
  CenterGravity,
#undef EastGravity
  EastGravity,
#undef SouthWestGravity
  SouthWestGravity,
#undef SouthGravity
  SouthGravity,
#undef SouthEastGravity
  SouthEastGravity,
#undef StaticGravity
  StaticGravity
} GravityType;

typedef enum
{
  UndefinedType,
  BilevelType,
  GrayscaleType,
  GrayscaleMatteType,
  PaletteType,
  PaletteMatteType,
  TrueColorType,
  TrueColorMatteType,
  ColorSeparationType,
  ColorSeparationMatteType,
  OptimizeType
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
  UndefinedMode,
  FrameMode,
  UnframeMode,
  ConcatenateMode
} MontageMode;

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
  UndefinedProfile,
  ICMProfile,
  IPTCProfile
} ProfileType;

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
  IndexQuantum,
  GrayQuantum,
  IndexAlphaQuantum,
  GrayAlphaQuantum,
  RedQuantum,
  CyanQuantum,
  GreenQuantum,
  YellowQuantum,
  BlueQuantum,
  MagentaQuantum,
  AlphaQuantum,
  BlackQuantum,
  RGBQuantum,
  RGBAQuantum,
  CMYKQuantum,
  CMYKAQuantum
} QuantumType;

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

typedef enum
{
  CharPixel,
  ShortPixel,
  IntegerPixel,
  LongPixel,
  FloatPixel,
  DoublePixel
} StorageType;

typedef enum
{
  UndefinedTransmitType,
  FileTransmitType,
  BlobTransmitType,
  StreamTransmitType,
  ImageTransmitType
} TransmitType;

/*
  Typedef declarations.
*/
typedef struct _AffineMatrix
{
  double
    sx,
    rx,
    ry,
    sy,
    tx,
    ty;
} AffineMatrix;

typedef struct _PrimaryInfo
{
  double
    x,
    y,
    z;
} PrimaryInfo;

typedef struct _ChromaticityInfo
{
  PrimaryInfo
    red_primary,
    green_primary,
    blue_primary,
    white_point;
} ChromaticityInfo;

typedef struct _PixelPacket
{
#if defined(WORDS_BIGENDIAN)
  Quantum
    red,
    green,
    blue,
    opacity;
#else
#if defined(macintosh)
  Quantum
    opacity,
    red,
    green,
    blue;
#else
  Quantum
    blue,
    green,
    red,
    opacity;
#endif
#endif
} PixelPacket;

typedef struct _ColorInfo
{
  const char
    *path,
    *name;

  ComplianceType
    compliance;

  PixelPacket
    color;

  unsigned int
    stealth;

  unsigned long
    signature;

  struct _ColorInfo
    *previous,
    *next;
} ColorInfo;

typedef struct _DoublePixelPacket
{
  double
    red,
    green,
    blue,
    opacity;
} DoublePixelPacket;

typedef struct _ErrorInfo
{
  double
    mean_error_per_pixel,
    normalized_mean_error,
    normalized_maximum_error;
} ErrorInfo;

#if !defined(WIN32)
typedef off_t ExtendedSignedIntegralType;
typedef size_t ExtendedUnsignedIntegralType;
#else
typedef __int64 ExtendedSignedIntegralType;
typedef unsigned __int64 ExtendedUnsignedIntegralType;
#endif

typedef struct _FrameInfo
{
  unsigned long
    width,
    height;

  long
    x,
    y,
    inner_bevel,
    outer_bevel;
} FrameInfo;

typedef Quantum IndexPacket;

typedef struct _LongPixelPacket
{
  unsigned long
    red,
    green,
    blue,
    opacity;
} LongPixelPacket;

typedef struct _MontageInfo
{
  char
    *geometry,
    *tile,
    *title,
    *frame,
    *texture,
    *font;

  double
    pointsize;

  unsigned long
    border_width;

  unsigned int
    shadow;

  PixelPacket
    fill,
    stroke,
    background_color,
    border_color,
    matte_color;

  GravityType
    gravity;

  char
    filename[MaxTextExtent];

  unsigned long
    signature;
} MontageInfo;

typedef struct _ProfileInfo
{
  size_t
    length;

  char
    *name;

  unsigned char
    *info;
} ProfileInfo;

typedef struct _RectangleInfo
{
  unsigned long
    width,
    height;

  long
    x,
    y;
} RectangleInfo;

typedef struct _SegmentInfo
{
  double
    x1,
    y1,
    x2,
    y2;
} SegmentInfo;

typedef struct _Ascii85Info _Ascii85Info_;

typedef struct _BlobInfo _BlobInfo_;

typedef struct _ImageAttribute  _ImageAttribute_;

typedef struct _Image
{
  ClassType
    storage_class;

  ColorspaceType
    colorspace;

  CompressionType
    compression;

  unsigned int
    taint,
    matte;

  unsigned long
    columns,
    rows;

  unsigned long
    depth,
    colors;

  PixelPacket
    *colormap;

  PixelPacket
    background_color,
    border_color,
    matte_color;

  double
    gamma;

  ChromaticityInfo
    chromaticity;

  ProfileInfo
    color_profile,
    iptc_profile,
    *generic_profile;

  unsigned long
    generic_profiles;

  RenderingIntent
    rendering_intent;

  ResolutionType
    units;

  char
    *montage,
    *directory,
    *geometry;

  long
    offset;

  double
    x_resolution,
    y_resolution;

  RectangleInfo
    page,
    tile_info;

  double
    blur,
    fuzz;

  FilterTypes
    filter;

  InterlaceType
    interlace;

  EndianType
    endian;

  GravityType
    gravity;

  CompositeOperator
    compose;

  DisposeType
    dispose;

  struct _Image
    *clip_mask;

  unsigned long
    scene,
    delay,
    iterations,
    total_colors;

  long
    start_loop;

  ErrorInfo
    error;

  TimerInfo
    timer;

  void
    *client_data;

  void
    *cache;

  _ImageAttribute_
    *attributes;

  _Ascii85Info_
    *ascii85;

  _BlobInfo_
    *blob;

  char
    filename[MaxTextExtent],
    magick_filename[MaxTextExtent],
    magick[MaxTextExtent];

  unsigned long
    magick_columns,
    magick_rows;

  ExceptionInfo
    exception;

  long
    reference_count;

  SemaphoreInfo
    *semaphore;

  unsigned long
    signature;

  struct _Image
    *previous,
    *list,
    *next;
} Image;

typedef unsigned int
  (*StreamHandler)(const Image *,const void *,const size_t);

typedef struct _ImageInfo
{
  CompressionType
    compression;

  unsigned int
    temporary,
    adjoin,
    affirm,
    antialias;

  unsigned long
    subimage,
    subrange,
    depth;

  char
    *size,
    *tile,
    *page;

  InterlaceType
    interlace;

  EndianType
    endian;

  ResolutionType
    units;

  unsigned long
    quality;

  char
    *sampling_factor,
    *server_name,
    *font,
    *texture,
    *density;

  double
    pointsize;

  double
    fuzz;

  PixelPacket
    pen,
    background_color,
    border_color,
    matte_color;

  unsigned int
    dither,
    monochrome;

  ColorspaceType
    colorspace;

  ImageType
    type;

  PreviewType
    preview_type;

  long
    group;

  unsigned int
    ping,
    verbose;

  char
    *view,
    *authenticate;

  Image
    *attributes;

  void
    *client_data;

  void
    *cache;

  StreamHandler
    stream;

  FILE
    *file;

  void
    *blob;

  size_t
    length;

  char
    magick[MaxTextExtent],
    unique[MaxTextExtent],
    zero[MaxTextExtent],
    filename[MaxTextExtent];

  unsigned long
    signature;
} ImageInfo;

/*
  Image const declarations.
*/
extern MagickExport const char
  *BackgroundColor, /* image.c */
  *BorderColor, /* image.c */
  *DefaultTileFrame, /* image.c */
  *DefaultTileGeometry, /* image.c */
  *DefaultTileLabel, /* image.c */
  *ForegroundColor, /* image.c */
  *MatteColor, /* image.c */
  *LoadImageText, /* image.c */
  *LoadImagesText, /* image.c */
  *PSDensityGeometry, /* image.c */
  *PSPageGeometry, /* image.c */
  *SaveImageText, /* image.c */
  *SaveImagesText; /* image.c */

extern MagickExport const unsigned long
  DefaultCompressionQuality; /* image.c */

/*
  Image utilities methods.
*/
extern MagickExport char
  **GetColorList(const char *,unsigned long *); /* color.c */

extern MagickExport const char
  *GetImageMagick(const unsigned char *,const size_t); /* magick.c */

extern MagickExport const ColorInfo
  *GetColorInfo(const char *,ExceptionInfo *); /* color.c */

extern MagickExport ExceptionType
  CatchImageException(Image *); /* image.c */

extern MagickExport const PixelPacket
  *AcquireImagePixels(const Image *,const long,const long,const unsigned long,
    const unsigned long,ExceptionInfo *); /* cache.c */

extern MagickExport Image
  *AdaptiveThresholdImage(const Image *,const unsigned long,const unsigned long,
     const double,ExceptionInfo *), /* effect.c */
  *AddNoiseImage(const Image *,const NoiseType,ExceptionInfo *), /* effect.c */
  *AffineTransformImage(const Image *,const AffineMatrix *,ExceptionInfo *), /* shear.c */
  *AllocateImage(const ImageInfo *), /* image.c */
  *AppendImages(const Image *,const unsigned int,ExceptionInfo *), /* image.c */
  *AverageImages(const Image *,ExceptionInfo *), /* image.c */
  *BlurImage(const Image *,const double,const double,ExceptionInfo *), /* effect.c */
  *BorderImage(const Image *,const RectangleInfo *,ExceptionInfo *), /* decorate.c */
  *CharcoalImage(const Image *,const double,const double,ExceptionInfo *), /* fx.c */
  *ChopImage(const Image *,const RectangleInfo *,ExceptionInfo *), /* transform.c */
  *CloneImage(const Image *,const unsigned long,const unsigned long,
   const unsigned int,ExceptionInfo *), /* image.c */
  *CoalesceImages(const Image *,ExceptionInfo *), /* transform.c */
  *ColorizeImage(const Image *,const char *,const PixelPacket,ExceptionInfo *), /* fx.c */
  *ConstituteImage(const unsigned long,const unsigned long,const char *,
   const StorageType,const void *,ExceptionInfo *), /* constitute.c */
  *ConvolveImage(const Image *,const unsigned int,const double *,
     ExceptionInfo *), /* fx.c */
  *CropImage(const Image *,const RectangleInfo *,ExceptionInfo *), /* transform.c */
  *DeconstructImages(const Image *,ExceptionInfo *), /* transform.c */
  *DespeckleImage(const Image *,ExceptionInfo *), /* effect.c */
  *EdgeImage(const Image *,const double,ExceptionInfo *), /* effect.c */
  *EmbossImage(const Image *,const double,const double,ExceptionInfo *), /* effect.c */
  *EnhanceImage(const Image *,ExceptionInfo *), /* effect.c */
  *FlattenImages(const Image *,ExceptionInfo *), /* transform.c */
  *FlipImage(const Image *,ExceptionInfo *), /* transform.c */
  *FlopImage(const Image *,ExceptionInfo *), /* transform.c */
  *FrameImage(const Image *,const FrameInfo *,ExceptionInfo *), /* decorate.c */
  *GaussianBlurImage(const Image *,const double,const double,ExceptionInfo *), /* effect.c */
  *ImplodeImage(const Image *,const double,ExceptionInfo *), /* fx.c */
  *MagnifyImage(const Image *,ExceptionInfo *), /* resize.c */
  *MedianFilterImage(const Image *,const double,ExceptionInfo *), /* effect.c */
  *MinifyImage(const Image *,ExceptionInfo *), /* resize.c */
  *MontageImages(const Image *,const MontageInfo *,ExceptionInfo *), /* montage.c */
  *MorphImages(const Image *,const unsigned long,ExceptionInfo *), /* fx.c */
  *MosaicImages(const Image *,ExceptionInfo *), /* transform.c */
  *MotionBlurImage(const Image *,const double,const double,const double,
     ExceptionInfo *), /* effect.c */
  *OilPaintImage(const Image *,const double,ExceptionInfo *), /* fx.c */
  *PingImage(const ImageInfo *,ExceptionInfo *), /* constitute.c */
  *ReadImage(const ImageInfo *,ExceptionInfo *), /* constitute.c */
  *ReadInlineImage(const ImageInfo *,const char *,ExceptionInfo *), /* constitute.c */
  *ReadStream(const ImageInfo *,StreamHandler,ExceptionInfo *), /* stream.c */
  *ReduceNoiseImage(const Image *,const double,ExceptionInfo *), /* effect.c */
  *ReferenceImage(Image *), /* image.c */
  *ResizeImage(const Image *,const unsigned long,const unsigned long,
     const FilterTypes,const double,ExceptionInfo *), /* resize.c */
  *RollImage(const Image *,const long,const long,ExceptionInfo *), /* transform.c */
  *RotateImage(const Image *,const double,ExceptionInfo *), /* shear.c */
  *SampleImage(const Image *,const unsigned long,const unsigned long,
   ExceptionInfo *), /* SampleImage */
  *ScaleImage(const Image *,const unsigned long,const unsigned long,
     ExceptionInfo *), /* resize.c */
  *ShadeImage(const Image *,const unsigned int,double,double,ExceptionInfo *), /* effect.c */
  *SharpenImage(const Image *,const double,const double,ExceptionInfo *), /* effect.c */
  *ShaveImage(const Image *,const RectangleInfo *,ExceptionInfo *), /* transform.c */
  *ShearImage(const Image *,const double,const double,ExceptionInfo *), /* shear.c */
  *SpreadImage(const Image *,const unsigned int,ExceptionInfo *), /* effect.c */
  *SteganoImage(const Image *,const Image *,ExceptionInfo *), /* fx.c */
  *StereoImage(const Image *,const Image *,ExceptionInfo *), /* fx.c */
  *SwirlImage(const Image *,double,ExceptionInfo *), /* fx.c */
  *ThumbnailImage(const Image *,const unsigned long,const unsigned long,
   ExceptionInfo *), /* resize.c */
  *UnsharpMaskImage(const Image *,const double,const double,const double,
     const double,ExceptionInfo *), /* effect.c */
  *WaveImage(const Image *,const double,const double,ExceptionInfo *), /* fx.c */
  *ZoomImage(const Image *,const unsigned long,const unsigned long,
     ExceptionInfo *); /* resize.c */

extern MagickExport ImageInfo
  *CloneImageInfo(const ImageInfo *); /* image.c */

extern MagickExport ImageType
  GetImageType(const Image *,ExceptionInfo *); /* image.c */

extern MagickExport IndexPacket
  ConstrainColormapIndex(Image *,const unsigned long), /* color.c */
  *GetIndexes(const Image *); /* cache.c */

extern MagickExport int
  GetImageGeometry(const Image *,const char *,const unsigned int,
  RectangleInfo *); /* image.c */

extern MagickExport MontageInfo
  *CloneMontageInfo(const ImageInfo *,const MontageInfo *); /* montage.c */

extern MagickExport PixelPacket
  AcquireOnePixel(const Image *,const long,const long,ExceptionInfo *), /* cache.c */
  *GetImagePixels(Image *,const long,const long,const unsigned long,
    const unsigned long), /* cache.c */
  GetOnePixel(Image *,const long,const long), /* cache.c */
  *GetPixels(const Image *), /* cache.c */
  *SetImagePixels(Image *,const long,const long,const unsigned long,
     const unsigned long); /* cache.c */

extern MagickExport RectangleInfo
  GetImageBoundingBox(const Image *,ExceptionInfo *exception); /* image.c */

extern MagickExport unsigned int
  AllocateImageColormap(Image *,const unsigned long), /* image.c */
  AnimateImages(const ImageInfo *image_info,Image *image), /* image.c */
  ChannelImage(Image *,const ChannelType), /* image.c */
  ChannelThresholdImage(Image *,const char *), /* effect.c */
  ClipImage(Image *), /* image.c */
  CompositeImage(Image *,const CompositeOperator,const Image *,const long,
    const long), /* composite.c */
  ContrastImage(Image *,const unsigned int), /* enhance.c */
  DispatchImage(const Image *,const long,const long,const unsigned long,
    const unsigned long,const char *,const StorageType,void *,ExceptionInfo *), /* constitute.c */
  DisplayImages(const ImageInfo *image_info,Image *image), /* image.c */
  EqualizeImage(Image *), /* enhance.c */
  FuzzyColorMatch(const PixelPacket *,const PixelPacket *,const double), /* color.c */
  GammaImage(Image *,const char *), /* enhance.c */
  GradientImage(Image *,const PixelPacket *,const PixelPacket *), /* image.c */
  IsGrayImage(const Image *,ExceptionInfo *), /* color.c */
  IsImagesEqual(Image *,const Image *), /* image.c */
  IsTaintImage(const Image *), /* image.c */
  IsMagickConflict(const char *), /* magick.c */
  IsMonochromeImage(const Image *,ExceptionInfo *), /* color.c */
  IsOpaqueImage(const Image *,ExceptionInfo *), /* color.c */
  IsPaletteImage(const Image *,ExceptionInfo *), /* color.c */
  IsSubimage(const char *,const unsigned int), /* image.c */
  LevelImage(Image *,const char *), /* enhance.c */
  LevelImageChannel(Image *,const ChannelType,const double,const double,
    const double), /* enhance.c */
  ListColorInfo(FILE *,ExceptionInfo *), /* color.c */
  ListMagickInfo(FILE *,ExceptionInfo *), /* magick.c */
  ModulateImage(Image *,const char *), /* enhance.c */
  MogrifyImage(const ImageInfo *,const int,char **,Image **), /* command.c */
  MogrifyImages(const ImageInfo *,const int,char **,Image **), /* command.c */
  NegateImage(Image *,const unsigned int), /* enhance.c */
  NormalizeImage(Image *), /* enhance.c */
  PlasmaImage(Image *,const SegmentInfo *,unsigned long,unsigned long), /* image.c */
  PopImagePixels(const Image *,const QuantumType,unsigned char *), /* constitute.c */
  ProfileImage(Image *,const char *,const unsigned char *,const size_t,
    unsigned int), /* transform.c */
  PushImagePixels(Image *,const QuantumType,const unsigned char *), /* constitute.c */
  QueryColorDatabase(const char *,PixelPacket *,ExceptionInfo *), /* color.c */
  QueryColorname(const Image *,const PixelPacket *,const ComplianceType,char *,
    ExceptionInfo *), /* color.c */
  RaiseImage(Image *,const RectangleInfo *,const int), /* decorate.c */
  RGBTransformImage(Image *,const ColorspaceType), /* image.c */
  SetImageClipMask(Image *,Image *), /* image.c */
  SetImageDepth(Image *,const unsigned long), /* image.c */
  SetImageInfo(ImageInfo *,const unsigned int,ExceptionInfo *), /* image.c */
  SignatureImage(Image *), /* signature.c */
  SortColormapByIntensity(Image *), /* image.c */
  SyncImagePixels(Image *), /* cache.c */
  ThresholdImage(Image *,const double), /* effect.c */
  TransformRGBImage(Image *,const ColorspaceType), /* image.c */
  UnregisterMagickInfo(const char *), /* magick.c */
  WriteImage(const ImageInfo *,Image *), /* constitute.c */
  WriteImages(ImageInfo *,Image *,const char *,ExceptionInfo *), /* constitute.c */
  WriteStream(const ImageInfo *,Image *,StreamHandler); /* stream.c */

extern MagickExport unsigned long
  GetImageDepth(const Image *,ExceptionInfo *), /* image.c */
  GetNumberColors(const Image *,FILE *,ExceptionInfo *); /* color.c */

extern MagickExport void
  AllocateNextImage(const ImageInfo *,Image *), /* image.c */
  CycleColormapImage(Image *,const int), /* image.c */
  DescribeImage(Image *,FILE *,const unsigned int), /* image.c */
  DestroyColorInfo(void), /* color.c */
  DestroyConstitute(void), /* constitute.c */
  DestroyImage(Image *), /* image.c */
  DestroyImageInfo(ImageInfo *), /* image.c */
  DestroyImagePixels(Image *), /* cache.c */
  DestroyMagick(void), /* magick.c */
  DestroyMagickInfo(void), /* magick.c */
  DestroyMontageInfo(MontageInfo *), /* montage.c */
  GetColorTuple(const PixelPacket *,const unsigned int,const unsigned int,
    const unsigned int,char *), /* color.c */
  GetImageException(Image *,ExceptionInfo *), /* image.c */
  GetImageInfo(ImageInfo *), /* image.c */
  GetMontageInfo(const ImageInfo *,MontageInfo *), /* montage.c */
  InitializeMagick(const char *), /* magick.c */
  ModifyImage(Image **,ExceptionInfo *), /* image.c */
  SetImage(Image *,const Quantum), /* image.c */
  SetImageOpacity(Image *,const unsigned int), /* image.c */
  SetImageType(Image *,const ImageType), /* image.c */
  SolarizeImage(Image *,const double), /* fx.c */
  SyncImage(Image *), /* image.c */
  TextureImage(Image *,const Image *), /* image.c */
  TransformImage(Image **,const char *,const char *); /* transform.c */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
