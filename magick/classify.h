/*
  ImageMagick image and error classifications.
*/
#ifndef _CLASSIFY_H
#define _CLASSIFY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Classify image attributes.
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
  Group4Compression,
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
  Classify errors and warnings.
*/
typedef enum
{
  UndefinedError,
  ResourceLimitError = 400,
  XServerError = 405,
  OptionError = 410,
  DelegateError = 415,
  MissingDelegateError = 420,
  CorruptImageError = 425,
  FileOpenError = 430
} ErrorType;

typedef enum
{
  UndefinedWarning,
  ResourceLimitWarning = 300,
  XServerWarning = 305,
  OptionWarning = 310,
  DelegateWarning = 315,
  MissingDelegateWarning = 320,
  CorruptImageWarning = 325,
  FileOpenWarning = 330
} WarningType;

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
