// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Definition of Options
//
// Options which may be applied to an image. These options are the
// equivalent of options supplied to ImageMagick utilities.
//
// This is an internal implementation class and is not part of the
// Magick++ API
//

#if !defined(Magick_Options_header)
#define Magick_Options_header

#include <string>
#include "Magick++/Color.h"
#include "Magick++/Geometry.h"
#include "Magick++/Drawable.h"
#include "Magick++/Include.h"

namespace Magick
{

  class Image;

  class Options
  {
    friend class Image;

  public:
    Options( void );
    Options( const Options& options_ );
    ~Options();

    // Remove pixel aliasing
    void            antiAlias( bool flag_ );
    bool            antiAlias( void );
    
    // Join images into a single multi-image file
    void            adjoin ( bool flag_ );
    bool            adjoin ( void ) const;
    
    // Image background color
    void            backgroundColor ( const Color &color_ );
    Color           backgroundColor ( void ) const;
    
    // Name of texture image to tile onto the image background
    void            backgroundTexture ( const std::string &backgroundTexture_ );
    std::string     backgroundTexture ( void ) const;
    
    // Image border color
    void            borderColor ( const Color &color_ );
    Color           borderColor ( void ) const;
    
    // Text bounding-box base color (default none)
    void            boxColor ( const Color &boxColor_ );
    Color           boxColor ( void ) const;
    
    // Colors within this distance are considered equal
    void            colorFuzz ( double fuzz_ );
    double          colorFuzz ( void ) const;
    
    // Compression type ( NoCompression, BZipCompression,
    // FaxCompression, JPEGCompression, LZWCompression,
    // RunlengthEncodedCompression, or ZipCompression )
    void            compressType ( CompressionType compressType_ );
    CompressionType compressType ( void ) const;
    
    // Vertical and horizontal resolution in pixels of the image
    void            density ( const Geometry &geomery_ );
    Geometry        density ( void ) const;

    // Image depth (8 or 16)
    void            depth ( unsigned int depth_ );
    unsigned int    depth ( void ) const;

    // Image filename to read or write
    void            fileName ( const std::string &fileName_ );
    std::string     fileName ( void ) const;

    // Color to use when filling drawn objects
    void            fillColor ( const Color &fillColor_ );
    Color           fillColor ( void ) const;

    // Rule to use when filling drawn objects
    void            fillRule ( const FillRule &fillRule_ );
    FillRule        fillRule ( void ) const;

    // Font name
    void            font ( const std::string &font_ );
    std::string     font ( void ) const;

    // Font point size
    void            fontPointsize ( double pointSize_ );
    double          fontPointsize ( void ) const;
    
    std::string     format ( void ) const;

    // Image interlace scheme
    void            interlaceType ( InterlaceType interlace_ );
    InterlaceType   interlaceType ( void ) const;
    
   // Image format to write or read
    void            magick ( const std::string &magick_ );
    std::string     magick ( void ) const;
    
    // Transparent color
    void            matteColor ( const Color &matteColor_ );
    Color           matteColor ( void ) const;

   // Write as a monochrome image
    void            monochrome ( bool monochromeFlag_ );
    bool            monochrome ( void ) const;

    // Preferred size and location of an image canvas.
    void            page ( const Geometry &pageSize_ );
    Geometry        page ( void ) const;

    // Pen texture image.
    void            penTexture ( const MagickLib::Image *penTexture_ );
    const MagickLib::Image* penTexture ( void  ) const;

    // Desired image quality factor
    void            quality ( unsigned int quality_ );
    unsigned int    quality ( void ) const;
    
    // Maximum number of colors to quantize to
    void            quantizeColors ( unsigned int colors_ );
    unsigned int    quantizeColors ( void ) const;
    
    // Colorspace to quantize in.
    void            quantizeColorSpace ( ColorspaceType colorSpace_ );
    ColorspaceType  quantizeColorSpace ( void ) const;
    
    // Dither image during quantization.
    void            quantizeDither ( bool ditherFlag_ );
    bool            quantizeDither ( void ) const;
    
    // Quantization tree-depth
    void            quantizeTreeDepth ( unsigned int treeDepth_ );
    unsigned int    quantizeTreeDepth ( void ) const;

    // Units of resolution to interpret density
    void            resolutionUnits ( ResolutionType resolutionUnits_ );
    ResolutionType  resolutionUnits ( void ) const;

    // Image size (required for raw formats)
    void            size ( const Geometry &geometry_ );
    Geometry        size ( void ) const;

    // enabled/disable stroke anti-aliasing
    void            strokeAntiAlias( bool flag_ );
    bool            strokeAntiAlias( void ) const ;

    // Color to use when drawing object outlines
    void            strokeColor ( const Color &strokeColor_ );
    Color           strokeColor ( void ) const;

    // Control the pattern of dashes and gaps used to stroke
    // paths. The strokeDashArray represents a list of numbers that
    // specify the lengths of alternating dashes and gaps in user
    // units. If an odd number of values is provided, then the list of
    // values is repeated to yield an even number of values.
    void            strokeDashArray ( const double* strokeDashArray_ );
    const double* strokeDashArray ( void ) const;

    // While drawing using strokeDashArray, specify distance into the dash
    // pattern to start the dash (default 0).
    void            strokeDashOffset ( double strokdDashOffset_ );
    double    strokeDashOffset ( void ) const;

    // Specify the shape to be used at the end of open subpaths when
    // they are stroked. Values of LineCap are UndefinedCap, ButtCap,
    // RoundCap, and SquareCap.
    void            strokeLineCap ( LineCap lineCap_ );
    LineCap         strokeLineCap ( void ) const;
    
    // Specify the shape to be used at the corners of paths (or other
    // vector shapes) when they are stroked. Values of LineJoin are
    // UndefinedJoin, MiterJoin, RoundJoin, and BevelJoin.
    void            strokeLineJoin ( LineJoin lineJoin_ );
    LineJoin        strokeLineJoin ( void ) const;

    // Specify miter limit. When two line segments meet at a sharp
    // angle and miter joins have been specified for 'lineJoin', it is
    // possible for the miter to extend far beyond the thickness of
    // the line stroking the path. The miterLimit' imposes a limit on
    // the ratio of the miter length to the 'stroke_width'. The default
    // value of this parameter is 4.
    void            strokeMiterLimit ( unsigned int miterLimit_ );
    unsigned int    strokeMiterLimit ( void ) const;

   // Stroke width for drawing vector objects (default one)
    void            strokeWidth ( double strokeWidth_ );
    double          strokeWidth ( void ) const;

    void            subImage ( unsigned int subImage_ );
    unsigned int    subImage ( void ) const;

    // Sub-frame number to return
    void            subRange ( unsigned int subRange_ );
    unsigned int    subRange ( void ) const;
    
    void            tileName ( const std::string &tileName_ );
    std::string     tileName ( void ) const;

    // Origin of coordinate system to use when annotating with text or drawing
    void            transformOrigin ( double tx_, double ty_ );

    // Reset transformation parameters to default
    void            transformReset ( void );

    // Rotation to use when annotating with text or drawing
    void            transformRotation ( double angle_ );

    // Scale to use when annotating with text or drawing
    void            transformScale ( double sx_, double sy_ );

    // Skew to use in X axis when annotating with text or drawing
    void            transformSkewX ( double skewx_ );

    // Skew to use in Y axis when annotating with text or drawing
    void            transformSkewY ( double skewy_ );

    // Return verbose information about an image, or an operation
    void            verbose ( bool verboseFlag_ );
    bool            verbose ( void ) const;
    
    void            view ( const std::string &view_ );
    std::string     view ( void ) const;

    // X11 display name
    void            x11Display ( const std::string &display_ );
    std::string     x11Display ( void ) const;
    
    //
    // Internal implementation methods.  Please do not use.
    //

    MagickLib::DrawInfo*       drawInfo( void );
    MagickLib::ImageInfo *     imageInfo( void );
    MagickLib::QuantizeInfo *  quantizeInfo( void );

    // Construct using raw structures
    Options( const MagickLib::ImageInfo* imageInfo_,
             const MagickLib::QuantizeInfo* quantizeInfo_,
             const MagickLib::DrawInfo* drawInfo_ );

  protected:

  private:
    
    // Assignment not supported
    Options operator= ( const Options& );

    MagickLib::ImageInfo*        _imageInfo;
    MagickLib::QuantizeInfo*     _quantizeInfo;
    MagickLib::DrawInfo*         _drawInfo;
  };
} // namespace Magick

//
// Inlines
//

inline void Magick::Options::antiAlias( bool flag_ )
{
  _imageInfo->antialias = static_cast<unsigned int>( flag_ );
}
inline bool Magick::Options::antiAlias( void )
{
  return static_cast<bool>(_imageInfo->antialias);
}

inline void Magick::Options::adjoin ( bool flag_ )
{
  _imageInfo->adjoin = static_cast<unsigned int>(flag_);
}
inline bool Magick::Options::adjoin ( void ) const
{
  return static_cast<bool>(_imageInfo->adjoin);
}

inline void Magick::Options::colorFuzz ( double fuzz_ )
{
  _imageInfo->fuzz = fuzz_;
}
inline double Magick::Options::colorFuzz ( void ) const
{
  return _imageInfo->fuzz;
}

inline void Magick::Options::compressType ( CompressionType compressType_ )
{
  _imageInfo->compression = compressType_;
}
inline Magick::CompressionType Magick::Options::compressType ( void ) const
{
  return static_cast<Magick::CompressionType>(_imageInfo->compression);
}

inline void Magick::Options::strokeDashOffset ( double strokeDashOffset_ )
{
  _drawInfo->dash_offset = strokeDashOffset_;
}
inline double Magick::Options::strokeDashOffset ( void ) const
{
  return _drawInfo->dash_offset;
}

inline void Magick::Options::depth ( unsigned int depth_ )
{
  _imageInfo->depth = depth_;
}
inline unsigned int Magick::Options::depth ( void ) const
{
  return _imageInfo->depth;
}

inline void Magick::Options::fontPointsize ( double pointSize_ )
{
  _imageInfo->pointsize = pointSize_;
  _drawInfo->pointsize = pointSize_;
}
inline double Magick::Options::fontPointsize ( void ) const
{
  return _imageInfo->pointsize;
}

inline void Magick::Options::interlaceType ( Magick::InterlaceType interlace_ )
{
  _imageInfo->interlace = interlace_;
}
inline Magick::InterlaceType Magick::Options::interlaceType ( void ) const
{
  return static_cast<Magick::InterlaceType>(_imageInfo->interlace);
}

inline void Magick::Options::monochrome ( bool monochromeFlag_ )
{
  _imageInfo->monochrome = monochromeFlag_;
}
inline bool Magick::Options::monochrome ( void ) const
{
  return static_cast<bool>(_imageInfo->monochrome);
}

inline void Magick::Options::quantizeColors ( unsigned int colors_ )
{
  _quantizeInfo->number_colors = colors_;
}
inline unsigned int Magick::Options::quantizeColors ( void ) const
{
  return _quantizeInfo->number_colors;
}

inline void Magick::Options::quantizeColorSpace ( Magick::ColorspaceType colorSpace_ )
{
  _quantizeInfo->colorspace = colorSpace_;
}
inline Magick::ColorspaceType Magick::Options::quantizeColorSpace ( void ) const
{
  return static_cast<Magick::ColorspaceType>(_quantizeInfo->colorspace);
}

inline void Magick::Options::quantizeDither ( bool ditherFlag_ )
{
  _imageInfo->dither = ditherFlag_;
  _quantizeInfo->dither = ditherFlag_;
}
inline bool Magick::Options::quantizeDither ( void ) const
{
  return static_cast<bool>(_imageInfo->dither);
}

inline void Magick::Options::quantizeTreeDepth ( unsigned int treeDepth_ )
{
  _quantizeInfo->tree_depth = treeDepth_;
}
inline unsigned int Magick::Options::quantizeTreeDepth ( void ) const
{
  return _quantizeInfo->tree_depth;
}

inline void Magick::Options::quality ( unsigned int quality_ )
{
  _imageInfo->quality = quality_;
}
inline unsigned int Magick::Options::quality ( void ) const
{
  return _imageInfo->quality;
}

inline void Magick::Options::strokeAntiAlias( bool flag_ )
{
  _drawInfo->stroke_antialias = flag_;
}
inline bool Magick::Options::strokeAntiAlias( void ) const
{
  return _drawInfo->stroke_antialias;
}

// Specify the shape to be used at the end of open subpaths when they
// are stroked. Values of LineCap are ButtCap, RoundCap, and
// SquareCap.
inline void Magick::Options::strokeLineCap ( Magick::LineCap lineCap_ )
{
  _drawInfo->linecap = lineCap_;
}
inline Magick::LineCap Magick::Options::strokeLineCap ( void ) const
{
  return _drawInfo->linecap;
}

// Specify the shape to be used at the corners of paths (or other
// vector shapes) when they are stroked.
inline void Magick::Options::strokeLineJoin ( Magick::LineJoin lineJoin_ )
{
  _drawInfo->linejoin = lineJoin_;
}
inline Magick::LineJoin Magick::Options::strokeLineJoin ( void ) const
{
  return _drawInfo->linejoin;
}

// miterLimit for drawing lines, circles, ellipses, etc.
inline void Magick::Options::strokeMiterLimit ( unsigned int miterLimit_ )
{
  _drawInfo->miterlimit = miterLimit_;
}
inline unsigned int Magick::Options::strokeMiterLimit ( void ) const
{
  return _drawInfo->miterlimit;
}

// Stroke width for drawing lines, circles, ellipses, etc.
inline void Magick::Options::strokeWidth ( double strokeWidth_ )
{
  _drawInfo->stroke_width = strokeWidth_;
}
inline double Magick::Options::strokeWidth ( void ) const
{
  return _drawInfo->stroke_width;
}

inline void Magick::Options::subImage ( unsigned int subImage_ )
{
  _imageInfo->subimage = subImage_;
}
inline unsigned int Magick::Options::subImage ( void ) const
{
  return _imageInfo->subimage;
}

inline void Magick::Options::subRange ( unsigned int subRange_ )
{
  _imageInfo->subrange = subRange_;
}
inline unsigned int Magick::Options::subRange ( void ) const
{
  return _imageInfo->subrange;
}

inline void Magick::Options::verbose ( bool verboseFlag_ )
{
  _imageInfo->verbose = verboseFlag_;
}
inline bool Magick::Options::verbose ( void ) const
{
  return static_cast<bool>(_imageInfo->verbose);
}

inline MagickLib::DrawInfo * Magick::Options::drawInfo( void )
{
  return _drawInfo;
}

inline MagickLib::ImageInfo * Magick::Options::imageInfo( void )
{
  return _imageInfo;
}

inline MagickLib::QuantizeInfo * Magick::Options::quantizeInfo( void )
{
  return _quantizeInfo;
}

#endif // Magick_Options_header
