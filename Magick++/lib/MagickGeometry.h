// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Geometry Definition
//
// Representation of an ImageMagick geometry specification
// X11 geometry specification plus hints

#if !defined (Geometry_header)
#define Geometry_header

#include <string>
#include <iostream>

namespace Magick
{

  class Geometry;

  // Compare two Geometry objects regardless of LHS/RHS
  int operator == ( const Magick::Geometry& left_, const Magick::Geometry& right_ );
  int operator != ( const Magick::Geometry& left_, const Magick::Geometry& right_ );
  int operator >  ( const Magick::Geometry& left_, const Magick::Geometry& right_ );
  int operator <  ( const Magick::Geometry& left_, const Magick::Geometry& right_ );
  int operator >= ( const Magick::Geometry& left_, const Magick::Geometry& right_ );
  int operator <= ( const Magick::Geometry& left_, const Magick::Geometry& right_ );

  class Geometry
  {
    friend class Image;
  public:
    
    Geometry ( unsigned int width_,
		     unsigned int height_,
		     unsigned int xOff_ = 0,
		     unsigned int yOff_ = 0,
		     bool xNegative_ = false,
		     bool yNegative_ = false );
    Geometry ( const std::string geometry_ );
    Geometry ( const char * geometry_ );
    Geometry ( );
    ~Geometry ( void );
    
    // Width
    void          width ( unsigned int width_ );
    unsigned int  width ( void ) const;
    
    // Height
    void          height ( unsigned int height_ );
    unsigned int  height ( void ) const;
    
    // X offset from origin
    void          xOff ( unsigned int xOff_ );
    int           xOff ( void ) const;
    
    // Y offset from origin
    void          yOff ( unsigned int yOff_ );
    int           yOff ( void ) const;
    
    // Sign of X offset negative? (X origin at right)
    void          xNegative ( bool xNegative_ );
    bool          xNegative ( void ) const;
    
    // Sign of Y offset negative? (Y origin at bottom)
    void          yNegative ( bool yNegative_ );
    bool          yNegative ( void ) const;
    
    // Width and height are expressed as percentages
    void          percent ( bool percent_ );
    bool          percent ( void ) const;

    // Resize without preserving aspect ratio (!)
    void          aspect ( bool aspect_ );
    bool          aspect ( void ) const;
    
    // Resize if image is greater than size (>)
    void          greater ( bool greater_ );
    bool          greater ( void ) const;
    
    // Resize if image is less than size (<)
    void          less ( bool less_ );
    bool          less ( void ) const;
    
    // Does object contain valid geometry?
    void          isValid ( bool isValid_ );
    bool          isValid ( void ) const;
    
    // Set via geometry string
    const Geometry& operator = ( const std::string &geometry_ );
    const Geometry& operator = ( const char * geometry_ );
    
    // Return geometry string
    operator std::string() const;
    
    // print object to a stream
    friend std::ostream& operator<<(std::ostream& stream_, const Geometry& geometry_);
    
  private:
    unsigned int  _width;
    unsigned int  _height;
    unsigned int  _xOff;
    unsigned int  _yOff;
    bool          _xNegative;
    bool          _yNegative;
    bool          _isValid;
    bool          _percent;        // Interpret width & height as percentages (%)
    bool          _aspect;         // Force exact size (!)
    bool          _greater;        // Re-size only if larger than geometry (>)
    bool          _less;           // Re-size only if smaller than geometry (<)
  };
} // namespace Magick

#endif // Geometry_header
