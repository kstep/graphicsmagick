// This may look like C code, but it is really -*- C++ -*-
//
// Definition of Montage class used to specify montage options.
//

#if !defined(Montage_header)
#define Montage_header

#include <string>
#include <MagickColor.h>
#include <MagickGeometry.h>
#include <MagickInclude.h>

//
// Basic (Un-framed) Montage
//
namespace Magick
{
  class Montage
  {
  public:
    Montage( void );
    virtual ~Montage( void );
    
    void              backgroundColor ( const Color &backgroundColor_ );
    Color             backgroundColor ( void ) const;
    
    void              compose ( CompositeOperator compose_ );
    CompositeOperator compose ( void ) const;
    
    void              fileName( std::string fileName_ );
    std::string       fileName( void ) const;
    
    void              font ( std::string font_ );
    std::string       font ( void ) const;
    
    void              geometry ( const Geometry &geometry_ );
    Geometry          geometry ( void ) const;
    
    void              gravity ( unsigned int gravity_ );
    unsigned int      gravity ( void ) const;
    
    // Apply as attribute to all images before montage
    void              label( std::string label_ );
    std::string       label( void ) const;
    
    void              penColor ( const Color &pen_ );
    Color             penColor ( void ) const;
    
    void              pointSize ( unsigned int pointSize_ );
    unsigned int      pointSize ( void ) const;
    
    void              shadow ( bool shadow_ );
    bool              shadow ( void ) const;
    
    void              texture ( std::string texture_ );
    std::string       texture ( void ) const;
    
    void              tile ( const Geometry &tile_ );
    Geometry          tile ( void ) const;
    
    void              title ( std::string title_ );
    std::string       title ( void ) const;
    
    // Apply to montage with TransparentImage()
    void              transparentColor ( const Color &transparentColor_ );
    Color             transparentColor ( void ) const;

    //
    // Implementation methods/members
    //

    // Update elements in existing MontageInfo structure
    virtual void      updateMontageInfo ( MagickLib::MontageInfo &montageInfo_ ) const;
        
  protected:

  private:
    Color             _backgroundColor;   // Color that thumbnails are composed on
    CompositeOperator _compose;           // Composition algorithm to use (e.g. ReplaceCompositeOp)
    std::string       _fileName;          // Filename to save montages to
    std::string       _font;              // Label font
    Geometry          _geometry;          // Thumbnail width & height plus border width & height
    unsigned int      _gravity;           // Thumbnail position (e.g. SouthWestGravity)
    std::string       _label;             // Thumbnail label (applied to image prior to montage)
    Color             _pen;               // Color for text annotations
    unsigned int      _pointSize;         // Font point size
    bool              _shadow;            // Enable drop-shadows on thumbnails
    std::string       _texture;           // Background texture image
    Geometry          _tile;              // Thumbnail rows and colmns
    std::string       _title;             // Montage title
    Color             _transparentColor;  // Transparent color
  };

  //
  // Montage With Frames (Extends Basic Montage)
  //
  class MontageFramed : public Montage
  {
  public:
    MontageFramed ( void );
    /* virtual */ ~MontageFramed ( void );
    
    void           borderColor ( const Color &borderColor_ );
    Color          borderColor ( void ) const;
    
    void           borderWidth ( unsigned int borderWidth_ );
    unsigned int   borderWidth ( void ) const;
    
    void           frameGeometry ( const Geometry &frame_ );
    Geometry       frameGeometry ( void ) const;
    
    void           matteColor ( const Color &matteColor_ );
    Color          matteColor ( void ) const;

    //
    // Implementation methods/members
    //

    // Update elements in existing MontageInfo structure
    /* virtual */ void updateMontageInfo ( MagickLib::MontageInfo &montageInfo_ ) const;
    
  protected:

  private:
    
    Color          _borderColor;	// Frame border color
    unsigned int   _borderWidth;	// Pixels between thumbnail and surrounding frame
    Geometry       _frame;		// Frame geometry (width & height frame thickness)
    Color          _matteColor;		// Frame foreground color
  };
} // namespace Magick

#endif // Montage_header
