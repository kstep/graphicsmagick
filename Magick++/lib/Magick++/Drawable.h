// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Definition of Drawable (Graphic objects)
//

#if !defined(Drawable_header)
#define Drawable_header

#include <string>
#include <list>


#include "Magick++/Color.h"
#include "Magick++/Geometry.h"
#include "Magick++/Include.h"

namespace Magick
{
  //
  // Representation of an x,y coordinate
  //
  class Coordinate
  {
    friend std::ostream& operator<<( std::ostream& stream_,
				     const Coordinate& coordinate_ );
  public:
    Coordinate ( void );
    Coordinate ( double x_, double y_ );
    ~Coordinate ();

    void   x ( double x_ );
    double x ( void ) const;

    void   y ( double y_ );
    double y ( void ) const;

  private:
    double _x;
    double _y;
  };

  //
  // Representation of a drawable object
  //
  class Drawable
  {
  public:
    // Constructor
    Drawable ( void );
    
    // Destructor
    ~Drawable ( void );
    
    //
    // Drawable Objects
    //
    
    // Point
    void point ( double x_, double y_ );
    void point ( const Coordinate &coordinate_ );
    
    // Line
    void line ( double startX_, double startY_,
		double endX_, double endY_ );
    void line ( const Coordinate &startCoordinate_,
		const Coordinate &endCoordinate_ );
    
    // Rectangle
    void rectangle ( double upperLeftX_, double upperLeftY_,
		     double lowerRightX_, double lowerRightY );
    void rectangle ( const Coordinate &upperLeftCoordinate_,
		     const Coordinate &lowerRightCoordinate_ );
    
    // Filled Rectangle
    void fillRectangle ( double upperLeftX_, double upperLeftY_,
			 double lowerRightX_, double lowerRightY );
    void fillRectangle ( const Coordinate &upperLeftCoordinate_,
			 const Coordinate &lowerRightCoordinate_ );
    
    // Circle
    void circle ( double originX_, double originY_,
		  double perimX_, double perimY_ );
    void circle ( const Coordinate &originCoordinate_,
		  const Coordinate &perimCoordinate_ );
    
    // Filled Circle
    void fillCircle ( double originX_, double originY_,
		      double perimX_, double perimY_ );
    void fillCircle ( const Coordinate &originCoordinate_,
		      const Coordinate &perimCoordinate_ );
    
    // Ellipse
    void ellipse ( double originX_, double originY_, 
		   double width_, double height_,
		   double arcStart_, double arcEnd_ );
    void ellipse ( const Coordinate &originCoordinate_, 
		   double width_, double height_,
		   double arcStart_, double arcEnd_ );
    
    // Filled Ellipse
    void fillEllipse ( double originX_, double originY_, 
		       double width_, double height_,
		       double arcStart_, double arcEnd_ );
    void fillEllipse ( const Coordinate &originCoordinate_, 
		       double width_, double height_,
		       double arcStart_, double arcEnd_ );
    
    // Polygon (Coordinate list must contain at least three members)
    void polygon ( const std::list<Magick::Coordinate> &coordinates_ );
    
    // Filled Polygon (vectors_ is number coordinates)
    void fillPolygon ( const std::list<Magick::Coordinate> &coordinates_ );

    // Bezier curve (Coordinate list must contain at least three members)
    void bezier ( const std::list<Magick::Coordinate> &coordinates_ );
    
    // Colorize at point using PaintMethod
    void color ( double x_, double y_,
		 PaintMethod paintMethod_ );
    void color ( const Coordinate &coordinate_,
		 PaintMethod paintMethod_ );
    
    // Change pixel matte value to transparent using PaintMethod
    void matte ( double x_, double y_,
		 PaintMethod paintMethod_ );
    void matte ( const Coordinate &coordinate_,
		 PaintMethod paintMethod_ );
    
    // Draw text at point
    void text ( double x_, double y_,
		std::string text_ );
    void text ( const Coordinate &coordinate_,
		std::string text_ );
    
    // Draw image at point
    void image ( double x_, double y_,
		 const std::string &image_ );
    void image ( const Coordinate &coordinate_,
		 const std::string &image_ );
    
    //
    // Following accessors are for internal use only
    //
    void        primitive ( const std::string &primitive_ );
    std::string primitive ( void ) const;
    
  private:

    // Common implementation of methods which take one Coordinate argument
    void one_arg_impl ( const char* command_,
			const Coordinate &coordinate_ );

    // Common implementation of methods which take two Coordinate arguments
    void two_arg_impl ( const char* command_,
			const Coordinate &startCoordinate_,
			const Coordinate &endCoordinate_ );

    // Common implementation of methods which take a list of Coordinates
    void list_arg_impl ( const char* command_,
			 const std::list<Magick::Coordinate> &coordinates_ );
    
    // This is the string normally passed by -draw, e.g. "circle
    // +100+100 +200+200"
    std::string _primitive;
  };
} // namespace Magick

//
// Inlines
//

//
// Coordinate class
//
inline Magick::Coordinate::Coordinate ( void )
  : _x(0),
    _y(0)
{
}

inline Magick::Coordinate::Coordinate ( double x_, double y_ )
  : _x(x_),
    _y(y_)
{
}

inline Magick::Coordinate::~Coordinate ()
{
  // Nothing to do
}

inline void Magick::Coordinate::x ( double x_ )
{
  _x = x_;
}

inline double Magick::Coordinate::x ( void ) const
{
  return _x;
}

inline void Magick::Coordinate::y ( double y_ )
{
  _y = y_;
}

inline double Magick::Coordinate::y ( void ) const
{
  return _y;
}

//
// Drawable class
//

// Constructor
inline Magick::Drawable::Drawable( void )
  : _primitive()
{
  // All components are self-initializing
}

// Destructor
inline Magick::Drawable::~Drawable( void )
{
  // Nothing to do
}

// Completed draw primitive (for ImageMagick)
inline void Magick::Drawable::primitive ( const std::string &primitive_ )
{
  _primitive = primitive_;
}
inline std::string Magick::Drawable::primitive ( void ) const
{
  // cout << _primitive << endl;
  return _primitive;
}


// Point
inline void Magick::Drawable::point ( const Magick::Coordinate &coordinate_ )
{
  one_arg_impl( "point", coordinate_ );
}
inline void Magick::Drawable::point ( double x_,
				      double y_ )
{
  point( Coordinate( x_, y_ ) );
}

// Line
inline void Magick::Drawable::line ( const Magick::Coordinate &startCoordinate_,
				     const Magick::Coordinate &endCoordinate_ )
{
  two_arg_impl( "line", startCoordinate_, endCoordinate_ );
}
inline void Magick::Drawable::line ( double startX_,
				     double startY_,
				     double endX_,
				     double endY_ )
{
  line( Coordinate( startX_, startY_ ), Coordinate( endX_, endY_ ) );
}

// Rectangle
inline void Magick::Drawable::rectangle ( const Magick::Coordinate &upperLeftCoordinate_,
					  const Magick::Coordinate &lowerRightCoordinate_ )
{
  two_arg_impl( "rectangle", upperLeftCoordinate_, lowerRightCoordinate_ );
}
inline void Magick::Drawable::rectangle ( double upperLeftX_,
					  double upperLeftY_,
					  double lowerRightX_,
					  double lowerRightY )
{
  rectangle( Coordinate( upperLeftX_, upperLeftY_ ),
	     Coordinate( lowerRightX_, lowerRightY ) );
}

// Filled Rectangle
inline void Magick::Drawable::fillRectangle ( const Magick::Coordinate &upperLeftCoordinate_,
					      const Magick::Coordinate &lowerRightCoordinate_ )
{
  two_arg_impl( "fillRectangle", upperLeftCoordinate_, lowerRightCoordinate_ );
}
inline void Magick::Drawable::fillRectangle ( double upperLeftX_,
					      double upperLeftY_,
					      double lowerRightX_,
					      double lowerRightY )
{
  fillRectangle( Coordinate( upperLeftX_, upperLeftY_ ),
		 Coordinate( lowerRightX_, lowerRightY ) );
}

// Circle
inline void Magick::Drawable::circle ( const Magick::Coordinate &originCoordinate_,
				       const Magick::Coordinate &perimCoordinate_ )
{
  two_arg_impl( "circle", originCoordinate_, perimCoordinate_ );
}
inline void Magick::Drawable::circle ( double originX_,
				       double originY_,
				       double perimX_,
				       double perimY_ )
{
  circle( Coordinate( originX_, originY_ ),
	  Coordinate( perimX_, perimY_ ) );
}

// Filled Circle
inline void Magick::Drawable::fillCircle ( const Magick::Coordinate &originCoordinate_,
					   const Magick::Coordinate &perimCoordinate_ )
{
  two_arg_impl( "fillCircle", originCoordinate_, perimCoordinate_ );
}
inline void Magick::Drawable::fillCircle ( double originX_,
					   double originY_,
					   double perimX_,
					   double perimY_ )
{
  fillCircle( Coordinate( originX_, originY_ ),
	      Coordinate( perimX_, perimY_ ) );
}

// Ellipse
inline void Magick::Drawable::ellipse ( double originX_,
					double originY_, 
					double width_,
					double height_,
					double arcStart_,
					double arcEnd_ )
{
  ellipse( Coordinate( originX_, originY_ ),
	   width_, height_,
	   arcStart_, arcEnd_ );
}

// Filled Ellipse
inline void Magick::Drawable::fillEllipse ( double originX_,
					    double originY_, 
					    double width_,
					    double height_,
					    double arcStart_,
					    double arcEnd_ )
{
  fillEllipse( Coordinate( originX_, originY_ ),
	       width_, height_,
	       arcStart_, arcEnd_ );
}

// Polygon (Coordinate list must contain at least three members)
inline void Magick::Drawable::polygon ( const std::list<Magick::Coordinate> &coordinates_ )
{
  list_arg_impl( "polygon", coordinates_ );
}

// Filled Polygon (Coordinate list must contain at least three members)
inline void Magick::Drawable::fillPolygon ( const std::list<Magick::Coordinate> &coordinates_ )
{
  list_arg_impl( "fillPolygon", coordinates_ );
}

// Bezier curve (Coordinate list must contain at least three members)
inline void Magick::Drawable::bezier ( const std::list<Magick::Coordinate> &coordinates_ )
{
  list_arg_impl( "bezier", coordinates_ );
}

// Colorize at point using PaintMethod
inline void Magick::Drawable::color ( double x_, double y_,
				      Magick::PaintMethod paintMethod_ )
{
  color( Coordinate( x_, y_ ), paintMethod_ );
}

// Change pixel matte value to transparent using PaintMethod
inline void Magick::Drawable::matte ( double x_, double y_,
				      Magick::PaintMethod paintMethod_ )
{
  matte( Coordinate( x_, y_ ), paintMethod_ );
}

// Draw text at point
inline void Magick::Drawable::text ( double x_,
				     double y_,
				     std::string text_ )
{
  text( Coordinate( x_, y_), text_ );
}

// Draw image at point
inline void Magick::Drawable::image ( double x_,
				      double y_,
				      const std::string &image_ )
{
  image( Coordinate( x_, y_), image_ );
}


#endif // Drawable_header
