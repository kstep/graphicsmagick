// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Definition of Drawable (Graphic objects)
//

#if !defined(Drawable_header)
#define Drawable_header

#include <string>
#include <iostream>
#include <list>

#include <MagickColor.h>
#include <MagickGeometry.h>
#include <MagickInclude.h>

namespace Magick
{
  //
  // Representation of an x,y coordinate
  //
  class Coordinate
  {
    friend std::ostream& operator<<(std::ostream& stream_, const Coordinate& coordinate_);
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
    void point ( Coordinate coordinate_ );
    
    // Line
    void line ( double startX_, double startY_,
		double endX_, double endY_ );
    void line ( Coordinate startCoordinate_,
		Coordinate endCoordinate_ );
    
    // Rectangle
    void rectangle ( double upperLeftX_, double upperLeftY_,
		     double lowerRightX_, double lowerRightY );
    void rectangle ( Coordinate upperLeftCoordinate_,
		     Coordinate lowerRightCoordinate_ );
    
    // Filled Rectangle
    void fillRectangle ( double upperLeftX_, double upperLeftY_,
			 double lowerRightX_, double lowerRightY );
    void fillRectangle ( Coordinate upperLeftCoordinate_,
			 Coordinate lowerRightCoordinate_ );
    
    // Circle
    void circle ( double originX_, double originY_,
		  double perimX_, double perimY_ );
    void circle ( Coordinate originCoordinate_,
		  Coordinate perimCoordinate_ );
    
    // Filled Circle
    void fillCircle ( double originX_, double originY_,
		      double perimX_, double perimY_ );
    void fillCircle ( Coordinate originCoordinate_,
		      Coordinate perimCoordinate_ );
    
    // Ellipse
    void ellipse ( double originX_, double originY_, 
		   double width_, double height_,
		   double arcStart_, double arcEnd_ );
    void ellipse ( Coordinate originCoordinate_, 
		   double width_, double height_,
		   double arcStart_, double arcEnd_ );
    
    // Filled Ellipse
    void fillEllipse ( double originX_, double originY_, 
		       double width_, double height_,
		       double arcStart_, double arcEnd_ );
    void fillEllipse ( Coordinate originCoordinate_, 
		       double width_, double height_,
		       double arcStart_, double arcEnd_ );
    
    // Polygon (Coordinate list must contain at least three members)
    void polygon ( const std::list<Coordinate> &coordinates_ );
    
    // Filled Polygon (vectors_ is number coordinates)
    void fillPolygon ( const std::list<Coordinate> &coordinates_ );
    
    // Colorize at point using PaintMethod
    void color ( double x_, double y_,
		 PaintMethod paintMethod_ );
    void color ( Coordinate coordinate_,
		 PaintMethod paintMethod_ );
    
    // Change pixel matte value to transparent using PaintMethod
    void matte ( double x_, double y_,
		 PaintMethod paintMethod_ );
    void matte ( Coordinate coordinate_,
		 PaintMethod paintMethod_ );
    
    // Draw text at point
    void text ( double x_, double y_,
		std::string text_ );
    void text ( Coordinate coordinate_,
		std::string text_ );
    
    // Draw image at point
    void image ( double x_, double y_,
		 const std::string &image_ );
    void image ( Coordinate coordinate_,
		 const std::string &image_ );
    
    //
    // Following accessors are for internal use only
    //
    std::string primitive ( void ) const;
    
  private:
    
    // This is the string normally passed by -draw, e.g. "circle
    // +100+100 +200+200"
    std::string _primitive;
  };
} // namespace Magick
#endif // Drawable_header
