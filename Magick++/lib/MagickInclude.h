// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Inclusion of ImageMagick headers (with namespace magic)

#ifndef MagickInclude_header
#define MagickInclude_header

//
// Include ImageMagick headers into namespace "MagickLib". If
// MAGICK_IMPLEMENTATION is defined, include ImageMagick development
// headers.  This scheme minimizes the possibility of conflict with
// user code.
//

// If <clocale> is not included, then SPARC Works 5.0 blows up when
// including locale.h
#include <clocale>

namespace MagickLib
{

#undef PACKAGE
#undef VERSION

#include <magick/magick.h>
#ifdef MAGICK_IMPLEMENTATION
# include <magick/defines.h>
#endif // MAGICK_IMPLEMENTATION
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
  using MagickLib::BumpmapCompositeOp;
  using MagickLib::ReplaceCompositeOp;
  using MagickLib::ReplaceRedCompositeOp;
  using MagickLib::ReplaceGreenCompositeOp;
  using MagickLib::ReplaceBlueCompositeOp;
  using MagickLib::ReplaceMatteCompositeOp;
  
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
  
  // Filter types
  using MagickLib::FilterType;
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
  
  // Image types
  using MagickLib::ImageType;
  using MagickLib::UndefinedType;
  using MagickLib::BilevelType;
  using MagickLib::GrayscaleType;
  using MagickLib::PaletteType;
  using MagickLib::TrueColorType;
  using MagickLib::MatteType;
  using MagickLib::ColorSeparationType;
  
  // Interlace types
  using MagickLib::InterlaceType;
  using MagickLib::UndefinedInterlace;
  using MagickLib::NoInterlace;
  using MagickLib::LineInterlace;
  using MagickLib::PlaneInterlace;
  using MagickLib::PartitionInterlace;
  
  // Layer types
  using MagickLib::LayerType;
  using MagickLib::UndefinedLayer;
  using MagickLib::RedLayer;
  using MagickLib::GreenLayer;
  using MagickLib::BlueLayer;
  using MagickLib::MatteLayer;
  
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

//   // Bit gravity
//   enum GravityType
//   {
//     ForgetGravity,
//     NorthWestGravity,
//     NorthGravity,
//     NorthEastGravity,
//     WestGravity,
//     CenterGravity,
//     EastGravity,
//     SouthWestGravity,
//     SouthGravity,
//     SouthEastGravity,
//     StaticGravity
//   };

}

#endif // MagickInclude_header
