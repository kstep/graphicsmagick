// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
//
// Inclusion of ImageMagick headers (with namespace magic)

#ifndef Magick_Include_header
#define Magick_Include_header

#if !defined(_MAGICK_CONFIG_H)
# define _MAGICK_CONFIG_H
# if !defined(vms) && !defined(macintosh)
#  include "magick/magick_config.h"
# else
#  include "magick_config.h"
# endif
# undef inline // Remove possible definition from config.h
# undef class
#endif

// Forward declarations for iostream classes
#if defined(HAVE_IOSFWD)
# include <iosfwd>
#else
class std::ios;
class std::streambuf;
class std::istream;
class std::ostream;
class std::iostream;
class std::filebuf;
class std::ifstream;
class std::ofstream;
class std::fstream;
#endif // HAVE_IOSFWD

// Needed for stdio FILE
#include <stdio.h>

// Needed for time_t
#include <time.h>

#if defined(HAVE_SYS_TYPES_H)
# include <sys/types.h>
#endif

#if defined(macintosh)
#  include <stat.mac.h>  /* Needed for off_t */
#endif

#if defined(__BORLANDC__)
# include <vcl.h> /* Borland C++ Builder 4.0 requirement */
#endif // defined(__BORLANDC__)

//
// Include ImageMagick headers into namespace "MagickLib". If
// MAGICK_IMPLEMENTATION is defined, include ImageMagick development
// headers.  This scheme minimizes the possibility of conflict with
// user code.
//
namespace MagickLib
{
#include <magick/api.h>
#undef inline // Remove possible definition from config.h

#undef class
}

//
// Import ImageMagick symbols and types which are used as part of the
// Magick++ API definition into namespace "Magick".
//
namespace Magick
{
  // The datatype for an RGB component
  using MagickLib::Quantum;

  // ImageMagick library initialization routine
  using MagickLib::InitializeMagick;

  // Image class types
  using MagickLib::ClassType;
  using MagickLib::UndefinedClass;
  using MagickLib::DirectClass;
  using MagickLib::PseudoClass;
  
  // Color-space types
  using MagickLib::ColorspaceType;
  using MagickLib::UndefinedColorspace;
  using MagickLib::RGBColorspace;
  using MagickLib::GRAYColorspace;
  using MagickLib::TransparentColorspace;
  using MagickLib::OHTAColorspace;
  using MagickLib::XYZColorspace;
  using MagickLib::YCbCrColorspace;
  using MagickLib::YCCColorspace;
  using MagickLib::YIQColorspace;
  using MagickLib::YPbPrColorspace;
  using MagickLib::YUVColorspace;
  using MagickLib::CMYKColorspace;
  using MagickLib::sRGBColorspace;
  
  // Composition operations
  using MagickLib::AddCompositeOp;
  using MagickLib::AtopCompositeOp;
  using MagickLib::BumpmapCompositeOp;
  using MagickLib::ClearCompositeOp;
  using MagickLib::ColorizeCompositeOp;
  using MagickLib::CompositeOperator;
  using MagickLib::CopyBlueCompositeOp;
  using MagickLib::CopyCompositeOp;
  using MagickLib::CopyGreenCompositeOp;
  using MagickLib::CopyOpacityCompositeOp;
  using MagickLib::CopyRedCompositeOp;
  using MagickLib::DarkenCompositeOp;
  using MagickLib::DifferenceCompositeOp;
  using MagickLib::DisplaceCompositeOp;
  using MagickLib::DissolveCompositeOp;
  using MagickLib::HueCompositeOp;
  using MagickLib::InCompositeOp;
  using MagickLib::LightenCompositeOp;
  using MagickLib::LuminizeCompositeOp;
  using MagickLib::MinusCompositeOp;
  using MagickLib::ModulateCompositeOp;
  using MagickLib::MultiplyCompositeOp;
  using MagickLib::NoCompositeOp;
  using MagickLib::OutCompositeOp;
  using MagickLib::OverCompositeOp;
  using MagickLib::OverlayCompositeOp;
  using MagickLib::PlusCompositeOp;
  using MagickLib::SaturateCompositeOp;
  using MagickLib::ScreenCompositeOp;
  using MagickLib::SubtractCompositeOp;
  using MagickLib::ThresholdCompositeOp;
  using MagickLib::UndefinedCompositeOp;
  using MagickLib::XorCompositeOp;
  
  // Compression algorithms
  using MagickLib::CompressionType;
  using MagickLib::UndefinedCompression;
  using MagickLib::NoCompression;
  using MagickLib::BZipCompression;
  using MagickLib::FaxCompression;
  using MagickLib::Group4Compression;
  using MagickLib::JPEGCompression;
  using MagickLib::LZWCompression;
  using MagickLib::RunlengthEncodedCompression;
  using MagickLib::ZipCompression;

  // Endian options
  using MagickLib::EndianType;
  using MagickLib::UndefinedEndian;
  using MagickLib::LSBEndian;
  using MagickLib::MSBEndian;

  // Fill rules
  using MagickLib::FillRule;
  using MagickLib::UndefinedRule;
  using MagickLib::EvenOddRule;
  using MagickLib::NonZeroRule;
  
  // Filter types
  using MagickLib::FilterTypes;
  using MagickLib::UndefinedFilter;
  using MagickLib::PointFilter;
  using MagickLib::BoxFilter;
  using MagickLib::TriangleFilter;
  using MagickLib::HermiteFilter;
  using MagickLib::HanningFilter;
  using MagickLib::HammingFilter;
  using MagickLib::BlackmanFilter;
  using MagickLib::GaussianFilter;
  using MagickLib::QuadraticFilter;
  using MagickLib::CubicFilter;
  using MagickLib::CatromFilter;
  using MagickLib::MitchellFilter;
  using MagickLib::LanczosFilter;
  using MagickLib::BesselFilter;
  using MagickLib::SincFilter;

  // Bit gravity
  using MagickLib::GravityType;
  using MagickLib::ForgetGravity;
  using MagickLib::NorthWestGravity;
  using MagickLib::NorthGravity;
  using MagickLib::NorthEastGravity;
  using MagickLib::WestGravity;
  using MagickLib::CenterGravity;
  using MagickLib::EastGravity;
  using MagickLib::SouthWestGravity;
  using MagickLib::SouthGravity;
  using MagickLib::SouthEastGravity;
  using MagickLib::StaticGravity;

  // Image types
  using MagickLib::ImageType;
  using MagickLib::UndefinedType;
  using MagickLib::BilevelType;
  using MagickLib::GrayscaleType;
  using MagickLib::GrayscaleMatteType;
  using MagickLib::PaletteType;
  using MagickLib::PaletteMatteType;
  using MagickLib::TrueColorType;
  using MagickLib::TrueColorMatteType;
  using MagickLib::ColorSeparationType;
  using MagickLib::ColorSeparationMatteType;
  using MagickLib::OptimizeType;
  
  // Interlace types
  using MagickLib::InterlaceType;
  using MagickLib::UndefinedInterlace;
  using MagickLib::NoInterlace;
  using MagickLib::LineInterlace;
  using MagickLib::PlaneInterlace;
  using MagickLib::PartitionInterlace;
  
  // Channel types
  using MagickLib::ChannelType;
  using MagickLib::UndefinedChannel;
  using MagickLib::RedChannel;
  using MagickLib::GreenChannel;
  using MagickLib::BlueChannel;
  using MagickLib::MatteChannel;

  // Line cap types
  using MagickLib::LineCap;
  using MagickLib::UndefinedCap;
  using MagickLib::ButtCap;
  using MagickLib::RoundCap;
  using MagickLib::SquareCap;

  // Line join types
  using MagickLib::LineJoin;
  using MagickLib::UndefinedJoin;
  using MagickLib::MiterJoin;
  using MagickLib::RoundJoin;
  using MagickLib::BevelJoin;

  // Noise types
  using MagickLib::NoiseType;
  using MagickLib::UniformNoise;
  using MagickLib::GaussianNoise;
  using MagickLib::MultiplicativeGaussianNoise;
  using MagickLib::ImpulseNoise;
  using MagickLib::LaplacianNoise;
  using MagickLib::PoissonNoise;
  
  // Paint methods
  using MagickLib::PaintMethod;
  using MagickLib::PointMethod;
  using MagickLib::ReplaceMethod;
  using MagickLib::FloodfillMethod;
  using MagickLib::FillToBorderMethod;
  using MagickLib::ResetMethod;

  // Preview types.  Not currently used by Magick++
  using MagickLib::PreviewType;
  using MagickLib::UndefinedPreview;
  using MagickLib::RotatePreview;
  using MagickLib::ShearPreview;
  using MagickLib::RollPreview;
  using MagickLib::HuePreview;
  using MagickLib::SaturationPreview;
  using MagickLib::BrightnessPreview;
  using MagickLib::GammaPreview;
  using MagickLib::SpiffPreview;
  using MagickLib::DullPreview;
  using MagickLib::GrayscalePreview;
  using MagickLib::QuantizePreview;
  using MagickLib::DespecklePreview;
  using MagickLib::ReduceNoisePreview;
  using MagickLib::AddNoisePreview;
  using MagickLib::SharpenPreview;
  using MagickLib::BlurPreview;
  using MagickLib::ThresholdPreview;
  using MagickLib::EdgeDetectPreview;
  using MagickLib::SpreadPreview;
  using MagickLib::SolarizePreview;
  using MagickLib::ShadePreview;
  using MagickLib::RaisePreview;
  using MagickLib::SegmentPreview;
  using MagickLib::SwirlPreview;
  using MagickLib::ImplodePreview;
  using MagickLib::WavePreview;
  using MagickLib::OilPaintPreview;
  using MagickLib::CharcoalDrawingPreview;
  using MagickLib::JPEGPreview;

  // Quantum types
  using MagickLib::QuantumType;
  using MagickLib::IndexQuantum;
  using MagickLib::GrayQuantum;
  using MagickLib::IndexAlphaQuantum;
  using MagickLib::GrayAlphaQuantum;
  using MagickLib::RedQuantum;
  using MagickLib::CyanQuantum;
  using MagickLib::GreenQuantum;
  using MagickLib::YellowQuantum;
  using MagickLib::BlueQuantum;
  using MagickLib::MagentaQuantum;
  using MagickLib::AlphaQuantum;
  using MagickLib::BlackQuantum;
  using MagickLib::RGBQuantum;
  using MagickLib::RGBAQuantum;
  using MagickLib::CMYKQuantum;

  // Rendering intents
  using MagickLib::RenderingIntent;
  using MagickLib::UndefinedIntent;
  using MagickLib::SaturationIntent;
  using MagickLib::PerceptualIntent;
  using MagickLib::AbsoluteIntent;
  using MagickLib::RelativeIntent;
  
  // Resolution units
  using MagickLib::ResolutionType;
  using MagickLib::UndefinedResolution;
  using MagickLib::PixelsPerInchResolution;
  using MagickLib::PixelsPerCentimeterResolution;

  // PixelPacket structure
  using MagickLib::PixelPacket;

  // IndexPacket type
  using MagickLib::IndexPacket;

  // StorageType type
  using MagickLib::StorageType;
  using MagickLib::CharPixel;
  using MagickLib::ShortPixel;
  using MagickLib::IntegerPixel;
  using MagickLib::FloatPixel;
  using MagickLib::DoublePixel;

  // StretchType type
  using MagickLib::StretchType;
  using MagickLib::NormalStretch;
  using MagickLib::UltraCondensedStretch;
  using MagickLib::ExtraCondensedStretch;
  using MagickLib::CondensedStretch;
  using MagickLib::SemiCondensedStretch;
  using MagickLib::SemiExpandedStretch;
  using MagickLib::ExpandedStretch;
  using MagickLib::ExtraExpandedStretch;
  using MagickLib::UltraExpandedStretch;
  using MagickLib::AnyStretch;

  // StyleType type
  using MagickLib::StyleType;
  using MagickLib::NormalStyle;
  using MagickLib::ItalicStyle;
  using MagickLib::ObliqueStyle;
  using MagickLib::AnyStyle;

  // Decoration types
  using MagickLib::DecorationType;
  using MagickLib::NoDecoration;
  using MagickLib::UnderlineDecoration;
  using MagickLib::OverlineDecoration;
  using MagickLib::LineThroughDecoration;

#if defined(MAGICK_IMPLEMENTATION)
  //
  // ImageMagick symbols used in implementation code
  //
  using MagickLib::AcquireCacheView;
  using MagickLib::AcquireImagePixels;
  using MagickLib::AcquireMemory;
  using MagickLib::AddNoiseImage;
  using MagickLib::AffineMatrix;
  using MagickLib::AllocateImage;
  using MagickLib::AnnotateImage;
  using MagickLib::AspectValue;
  using MagickLib::Base64Decode;
  using MagickLib::Base64Encode;
  using MagickLib::BlobError;
  using MagickLib::BlobToImage;
  using MagickLib::BlobWarning;
  using MagickLib::BlurImage;
  using MagickLib::BorderImage;
  using MagickLib::CacheError;
  using MagickLib::CacheWarning;
  using MagickLib::ChannelImage;
  using MagickLib::CharcoalImage;
  using MagickLib::ChopImage;
  using MagickLib::CloneDrawInfo;
  using MagickLib::CloneImage;
  using MagickLib::CloneImageInfo;
  using MagickLib::CloneQuantizeInfo;
  using MagickLib::CloseCacheView;
  using MagickLib::ColorFloodfillImage;
  using MagickLib::ColorizeImage;
  using MagickLib::CompositeImage;
  using MagickLib::ConstituteImage;
  using MagickLib::ContrastImage;
  using MagickLib::ConvolveImage;
  using MagickLib::CorruptImageError;
  using MagickLib::CorruptImageWarning;
  using MagickLib::CropImage;
  using MagickLib::CycleColormapImage;
  using MagickLib::DelegateError;
  using MagickLib::DelegateWarning;
  using MagickLib::DeleteMagickRegistry;
  using MagickLib::DespeckleImage;
  using MagickLib::DestroyDrawInfo;
  using MagickLib::DestroyExceptionInfo;
  using MagickLib::DestroyImageInfo;
  using MagickLib::DestroyImageList;
  using MagickLib::DestroyMagick;
  using MagickLib::DestroyQuantizeInfo;
  using MagickLib::DispatchImage;
  using MagickLib::DisplayImages;
  using MagickLib::DrawAllocateContext;
  using MagickLib::DrawAnnotation;
  using MagickLib::DrawArc;
  using MagickLib::DrawBezier;
  using MagickLib::DrawCircle;
  using MagickLib::DrawColor;
  using MagickLib::DrawComment;
  using MagickLib::DrawComposite;
  using MagickLib::DrawContext;
  using MagickLib::DrawDestroyContext;
  using MagickLib::DrawEllipse;
  using MagickLib::DrawImage;
  using MagickLib::DrawInfo;
  using MagickLib::DrawLine;
  using MagickLib::DrawMatte;
  using MagickLib::DrawPathClose;
  using MagickLib::DrawPathCurveToAbsolute;
  using MagickLib::DrawPathCurveToQuadraticBezierAbsolute;
  using MagickLib::DrawPathCurveToQuadraticBezierRelative;
  using MagickLib::DrawPathCurveToQuadraticBezierSmoothAbsolute;
  using MagickLib::DrawPathCurveToQuadraticBezierSmoothRelative;
  using MagickLib::DrawPathCurveToRelative;
  using MagickLib::DrawPathCurveToSmoothAbsolute;
  using MagickLib::DrawPathCurveToSmoothRelative;
  using MagickLib::DrawPathEllipticArcAbsolute;
  using MagickLib::DrawPathEllipticArcRelative;
  using MagickLib::DrawPathFinish;
  using MagickLib::DrawPathLineToAbsolute;
  using MagickLib::DrawPathLineToHorizontalAbsolute;
  using MagickLib::DrawPathLineToHorizontalRelative;
  using MagickLib::DrawPathLineToRelative;
  using MagickLib::DrawPathLineToVerticalAbsolute;
  using MagickLib::DrawPathLineToVerticalRelative;
  using MagickLib::DrawPathMoveToAbsolute;
  using MagickLib::DrawPathMoveToRelative;
  using MagickLib::DrawPathStart;
  using MagickLib::DrawPoint;
  using MagickLib::DrawPolygon;
  using MagickLib::DrawPolyline;
  using MagickLib::DrawPopClipPath;
  using MagickLib::DrawPopDefs;
  using MagickLib::DrawPopGraphicContext;
  using MagickLib::DrawPopPattern;
  using MagickLib::DrawPushClipPath;
  using MagickLib::DrawPushDefs;
  using MagickLib::DrawPushGraphicContext;
  using MagickLib::DrawPushPattern;
  using MagickLib::DrawRectangle;
  using MagickLib::DrawRender;
  using MagickLib::DrawRoundRectangle;
  using MagickLib::DrawSetAffine;
  using MagickLib::DrawSetClipPath;
  using MagickLib::DrawSetClipRule;
  using MagickLib::DrawSetClipUnits;
  using MagickLib::DrawSetFillColor;
  using MagickLib::DrawSetFillColorString;
  using MagickLib::DrawSetFillOpacity;
  using MagickLib::DrawSetFillPatternURL;
  using MagickLib::DrawSetFillRule;
  using MagickLib::DrawSetFont;
  using MagickLib::DrawSetFontFamily;
  using MagickLib::DrawSetFontSize;
  using MagickLib::DrawSetFontStretch;
  using MagickLib::DrawSetFontStyle;
  using MagickLib::DrawSetFontWeight;
  using MagickLib::DrawSetGravity;
  using MagickLib::DrawSetRotate;
  using MagickLib::DrawSetScale;
  using MagickLib::DrawSetSkewX;
  using MagickLib::DrawSetSkewY;
  using MagickLib::DrawSetStopColor;
  using MagickLib::DrawSetStrokeAntialias;
  using MagickLib::DrawSetStrokeColor;
  using MagickLib::DrawSetStrokeColorString;
  using MagickLib::DrawSetStrokeDashArray;
  using MagickLib::DrawSetStrokeDashOffset;
  using MagickLib::DrawSetStrokeLineCap;
  using MagickLib::DrawSetStrokeLineJoin;
  using MagickLib::DrawSetStrokeMiterLimit;
  using MagickLib::DrawSetStrokeOpacity;
  using MagickLib::DrawSetStrokePatternURL;
  using MagickLib::DrawSetStrokeWidth;
  using MagickLib::DrawSetTextAntialias;
  using MagickLib::DrawSetTextDecoration;
  using MagickLib::DrawSetTextUnderColor;
  using MagickLib::DrawSetTextUnderColorString;
  using MagickLib::DrawSetTranslate;
  using MagickLib::DrawSetViewbox;
  using MagickLib::EdgeImage;
  using MagickLib::EmbossImage;
  using MagickLib::EnhanceImage;
  using MagickLib::EqualizeImage;
  using MagickLib::ExceptionInfo;
  using MagickLib::ExceptionType;
  using MagickLib::FileOpenError;
  using MagickLib::FileOpenWarning;
  using MagickLib::FlattenImages;
  using MagickLib::FlipImage;
  using MagickLib::FlopImage;
  using MagickLib::FormatString;
  using MagickLib::FrameImage;
  using MagickLib::FrameInfo;
  using MagickLib::GammaImage;
  using MagickLib::GammaImage;
  using MagickLib::GaussianBlurImage;
  using MagickLib::GetCacheView;
  using MagickLib::GetCacheViewIndexes;
  using MagickLib::GetDrawInfo;
  using MagickLib::GetExceptionInfo;
  using MagickLib::GetGeometry;
  using MagickLib::GetImageAttribute;
  using MagickLib::GetImageAttribute;
  using MagickLib::GetImageBoundingBox;
  using MagickLib::GetImageDepth;
  using MagickLib::GetImageInfo;
  using MagickLib::GetImagePixels;
  using MagickLib::GetImageType;
  using MagickLib::GetIndexes;
  using MagickLib::GetMagickGeometry;
  using MagickLib::GetMagickInfo;
  using MagickLib::GetMagickRegistry;
  using MagickLib::GetNumberColors;
  using MagickLib::GetPageGeometry;
  using MagickLib::GetPixels;
  using MagickLib::GetQuantizeInfo;
  using MagickLib::GetTypeMetrics;
  using MagickLib::GreaterValue;
  using MagickLib::HSLTransform;
  using MagickLib::HeightValue;
  using MagickLib::ImageAttribute;
  using MagickLib::ImageInfo;
  using MagickLib::ImageInfoRegistryType;
  using MagickLib::ImageRegistryType;
  using MagickLib::ImageToBlob;
  using MagickLib::ImplodeImage;
  using MagickLib::IsGeometry;
  using MagickLib::IsSubimage;
  using MagickLib::LessValue;
  using MagickLib::LiberateMemory;
  using MagickLib::LocaleCompare;
  using MagickLib::MagickInfo;
  using MagickLib::MagickToMime;
  using MagickLib::MagnifyImage;
  using MagickLib::MapImage;
  using MagickLib::MatteFloodfillImage;
  using MagickLib::MedianFilterImage;
  using MagickLib::MinifyImage;
  using MagickLib::MissingDelegateError;
  using MagickLib::MissingDelegateWarning;
  using MagickLib::ModulateImage;
  using MagickLib::MontageInfo;
  using MagickLib::NegateImage;
  using MagickLib::NoiseType;
  using MagickLib::NormalizeImage;
  using MagickLib::OilPaintImage;
  using MagickLib::OpaqueImage;
  using MagickLib::OpenCacheView;
  using MagickLib::OptionError;
  using MagickLib::OptionWarning;
  using MagickLib::PercentValue;
  using MagickLib::PingBlob;
  using MagickLib::PingImage;
  using MagickLib::PointInfo;
  using MagickLib::PopImagePixels;
  using MagickLib::ProfileInfo;
  using MagickLib::PushImagePixels;
  using MagickLib::QuantizationError;
  using MagickLib::QuantizeImage;
  using MagickLib::QuantizeInfo;
  using MagickLib::QueryColorDatabase;
  using MagickLib::RGBTransformImage;
  using MagickLib::RaiseImage;
  using MagickLib::ReacquireMemory;
  using MagickLib::ReadImage;
  using MagickLib::RectangleInfo;
  using MagickLib::RectangleInfo;
  using MagickLib::ReduceNoiseImage;
  using MagickLib::RegisterMagickInfo;
  using MagickLib::RegistryType;
  using MagickLib::ResourceLimitError;
  using MagickLib::ResourceLimitWarning;
  using MagickLib::RollImage;
  using MagickLib::RotateImage;
  using MagickLib::SampleImage;
  using MagickLib::ScaleImage;
  using MagickLib::SegmentImage;
  using MagickLib::SetCacheThreshold;
  using MagickLib::SetCacheView;
  using MagickLib::SetClientName;
  using MagickLib::SetImage;
  using MagickLib::SetImageAttribute;
  using MagickLib::SetImageClipMask;
  using MagickLib::SetImageDepth;
  using MagickLib::SetImageInfo;
  using MagickLib::SetImageOpacity;
  using MagickLib::SetImagePixels;
  using MagickLib::SetImageType;
  using MagickLib::SetMagickInfo;
  using MagickLib::SetMagickRegistry;
  using MagickLib::ShadeImage;
  using MagickLib::SharpenImage;
  using MagickLib::ShaveImage;
  using MagickLib::ShearImage;
  using MagickLib::SignatureImage;
  using MagickLib::SizeBlob;
  using MagickLib::SolarizeImage;
  using MagickLib::SpreadImage;
  using MagickLib::SteganoImage;
  using MagickLib::StereoImage;
  using MagickLib::SwirlImage;
  using MagickLib::SyncCacheView;
  using MagickLib::SyncImage;
  using MagickLib::SyncImagePixels;
  using MagickLib::TextureImage;
  using MagickLib::ThresholdImage;
  using MagickLib::ThrowException;
  using MagickLib::TransformHSL;
  using MagickLib::TransformImage;
  using MagickLib::TransformRGBImage;
  using MagickLib::TransparentImage;
  using MagickLib::UndefinedException;
  using MagickLib::UndefinedRegistryType;
  using MagickLib::UnregisterMagickInfo;
  using MagickLib::UnsharpMaskImage;
  using MagickLib::ViewInfo;
  using MagickLib::WaveImage;
  using MagickLib::WidthValue;
  using MagickLib::WriteImage;
  using MagickLib::XNegative;
  using MagickLib::XServerError;
  using MagickLib::XServerWarning;
  using MagickLib::XValue;
  using MagickLib::YNegative;
  using MagickLib::YValue;
  using MagickLib::ZoomImage;

#endif // MAGICK_IMPLEMENTATION

}

#endif // Magick_Include_header
