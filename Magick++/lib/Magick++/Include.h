// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Inclusion of ImageMagick headers (with namespace magic)

#ifndef Magick_Include_header
#define Magick_Include_header

// Have standard <iosfwd>
#define HAVE_IOSFWD 1
// Have <sys/types.h>
#define HAVE_SYS_TYPES_H 1

//
// Include ImageMagick headers into namespace "MagickLib". If
// MAGICK_IMPLEMENTATION is defined, include ImageMagick development
// headers.  This scheme minimizes the possibility of conflict with
// user code.
//

#if defined(__BORLANDC__)
//
// The following includes need to be specified to compile with Borland
// C++ Builder 4.0.
//
#include <vcl.h>
#endif // defined(__BORLANDC__)

#if defined(macintosh)
#  undef HAVE_SYS_TYPES_H
#  include <stat.mac.h>  // Needed for off_t
#endif // defined(macintosh)

// Needed for stdio FILE
#include <stdio.h>

// Needed for time_t
#include <time.h>

// Needed for off_t on Unix and Windows
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
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

namespace MagickLib
{
#undef PACKAGE
#undef VERSION

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
  using MagickLib::CompositeOperator;
  using MagickLib::UndefinedCompositeOp;
  using MagickLib::OverCompositeOp;
  using MagickLib::InCompositeOp;
  using MagickLib::OutCompositeOp;
  using MagickLib::AtopCompositeOp;
  using MagickLib::XorCompositeOp;
  using MagickLib::PlusCompositeOp;
  using MagickLib::MinusCompositeOp;
  using MagickLib::AddCompositeOp;
  using MagickLib::SubtractCompositeOp;
  using MagickLib::DifferenceCompositeOp;
  using MagickLib::MultiplyCompositeOp;
  using MagickLib::BumpmapCompositeOp;
  using MagickLib::CopyCompositeOp;
  using MagickLib::CopyRedCompositeOp;
  using MagickLib::CopyGreenCompositeOp;
  using MagickLib::CopyBlueCompositeOp;
  using MagickLib::CopyOpacityCompositeOp;
  using MagickLib::ClearCompositeOp;
  using MagickLib::DissolveCompositeOp;
  using MagickLib::DisplaceCompositeOp;
  using MagickLib::ModulateCompositeOp;
  using MagickLib::ThresholdCompositeOp;
  
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
  using MagickLib::PaletteType;
  using MagickLib::PaletteMatteType;
  using MagickLib::TrueColorType;
  using MagickLib::TrueColorMatteType;
  using MagickLib::ColorSeparationType;
  
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
  using MagickLib::AcquireImagePixels;
  using MagickLib::AcquireMemory;
  using MagickLib::AddNoiseImage;
  using MagickLib::AffineMatrix;
  using MagickLib::AllocateImage;
  using MagickLib::AnnotateImage;
  using MagickLib::AspectValue;
  using MagickLib::BlobError;
  using MagickLib::BlobToImage;
  using MagickLib::BlobWarning;
  using MagickLib::BlurImage;
  using MagickLib::BorderImage;
  using MagickLib::CacheError;
  using MagickLib::CacheWarning;
  using MagickLib::ChannelImage;
  using MagickLib::ChopImage;
  using MagickLib::CloneDrawInfo;
  using MagickLib::CloneImage;
  using MagickLib::CloneImageInfo;
  using MagickLib::CloneQuantizeInfo;
  using MagickLib::CloneString;
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
  using MagickLib::DespeckleImage;
  using MagickLib::DestroyDrawInfo;
  using MagickLib::DestroyImage;
  using MagickLib::DestroyImageInfo;
  using MagickLib::DestroyImages;
  using MagickLib::DestroyQuantizeInfo;
  using MagickLib::DispatchImage;
  using MagickLib::DisplayImages;
  using MagickLib::DrawImage;
  using MagickLib::DrawInfo;
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
  using MagickLib::GetImageDepth;
  using MagickLib::GetImageInfo;
  using MagickLib::GetImagePixels;
  using MagickLib::GetImageType;
  using MagickLib::GetIndexes;
  using MagickLib::GetMagickInfo;
  using MagickLib::GetNumberColors;
  using MagickLib::GetPixels;
  using MagickLib::GetQuantizeInfo;
  using MagickLib::GetTypeMetrics;
  using MagickLib::GreaterValue;
  using MagickLib::HSLTransform;
  using MagickLib::HeightValue;
  using MagickLib::ImageAttribute;
  using MagickLib::ImageInfo;
  using MagickLib::ImageToBlob;
  using MagickLib::ImplodeImage;
  using MagickLib::IsGeometry;
  using MagickLib::IsSubimage;
  using MagickLib::LessValue;
  using MagickLib::LiberateMemory;
  using MagickLib::LocaleCompare;
  using MagickLib::MagickInfo;
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
  using MagickLib::ParseImageGeometry;
  using MagickLib::PercentValue;
  using MagickLib::PingImage;
  using MagickLib::PopImagePixels;
  using MagickLib::PostscriptGeometry;
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
  using MagickLib::TransformHSL;
  using MagickLib::TransformImage;
  using MagickLib::TransformRGBImage;
  using MagickLib::TransparentImage;
  using MagickLib::UndefinedException;
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
