// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Definition of Drawable (Graphic objects)
//

#if !defined(Drawable_header)
#define Drawable_header

#include "Magick++/Include.h"

#include <string>
#include <list>
#include <utility>
#include <iostream>
#include "Magick++/Color.h"
#include "Magick++/Geometry.h"

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
  // Base class for all drawable objects
  //
  class DrawableBase
  {
  public:
    // Constructor
    DrawableBase ( void );

    // Destructor
    virtual ~DrawableBase ( void );

    // Copy constructor
//     DrawableBase ( const DrawableBase& original_ );

    // Assignment operator
    const DrawableBase& operator= (const DrawableBase& original_ );

    // Print object to stream
    friend ostream& operator<< (ostream& stream_,
                                const DrawableBase& drawable_);

    // Support a polymorphic print-to-stream operator
    virtual void print (ostream& stream_) const = 0;

    // Return polymorphic copy of object
    virtual DrawableBase* copy() const = 0;
  };

  //
  // Representation of a drawable surrogate object to manage drawable objects
  //
  class Drawable
  {
  public:
    // Constructor
    Drawable ( void );

    // Construct from DrawableBase
    Drawable ( const DrawableBase& original_ );

    // Destructor
    ~Drawable ( void );

    // Copy constructor
    Drawable ( const Drawable& original_ );

    // Assignment operator
    Drawable& operator= (const Drawable& original_ );

    // Print object to stream
    friend ostream& operator<< (ostream& stream_,
                                const Drawable& drawable_);
  private:
    DrawableBase* dp;
  };

  //
  // Drawable Objects
  //

  // Affine (scaling, rotation, and translation)
  class DrawableAffine  : public DrawableBase
  {
  public:
    DrawableAffine ( double scaleX_, double scaleY_,
                     double rotationX_, double rotationY_,
                     double translationX_, double translationY_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _scaleX;
    double _scaleY;
    double _rotationX;
    double _rotationY;
    double _translationX;
    double _translationY;
  };

  // Angle (drawing angle)
  class DrawableAngle  : public DrawableBase
  {
  public:
    DrawableAngle ( double angle_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;
  private:
    double _angle;
  };

  // Arc
  class DrawableArc : public DrawableBase
  {
  public:
    DrawableArc ( double startX_, double startY_,
                  double endX_, double endY_,
                  double startDegrees_, double endDegrees_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _startX;
    double _startY;
    double _endX;
    double _endY;
    double _startDegrees;
    double _endDegrees;
  };

  // Bezier curve (Coordinate list must contain at least three members)
  class DrawableBezier : public DrawableBase
  {
  public:
    DrawableBezier ( const std::list<Magick::Coordinate> &coordinates_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    std::list<Magick::Coordinate> _coordinates;
  };

  // Circle
  class DrawableCircle : public DrawableBase
  {
  public:
    DrawableCircle ( double originX_, double originY_,
                     double perimX_, double perimY_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _originX;
    double _originY;
    double _perimX;
    double _perimY;
  };

  // Colorize at point using PaintMethod
  class DrawableColor : public DrawableBase
  {
  public:
    DrawableColor ( double x_, double y_,
                    PaintMethod paintMethod_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _x;
    double _y;
    PaintMethod _paintMethod;
  };

  // Decoration (text decoration)
  class DrawableTextDecoration : public DrawableBase
  {
  public:
    DrawableTextDecoration ( DecorationType decoration_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    DecorationType _decoration;
  };

  // Ellipse
  class DrawableEllipse : public DrawableBase
  {
  public:
    DrawableEllipse ( double originX_, double originY_, 
                      double width_, double height_,
                      double arcStart_, double arcEnd_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _originX;
    double _originY; 
    double _width;
    double _height;
    double _arcStart;
    double _arcEnd;
  };

  // Specify drawing fill color
  class DrawableFillColor : public DrawableBase
  {
  public:
    DrawableFillColor ( const Magick::Color &color_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    Magick::Color _color;
  };

  // Specify drawing fill opacity
  class DrawableFillOpacity : public DrawableBase
  {
  public:
    DrawableFillOpacity ( double opacity_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _opacity;
  };

  // Specify text font
  class DrawableFont : public DrawableBase
  {
  public:
    DrawableFont ( const std::string &font_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    std::string _font;
  };

  // Specify text positioning gravity
  class DrawableGravity : public DrawableBase
  {
  public:
    DrawableGravity ( GravityType gravity_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    GravityType _gravity;
  };

  // Draw image at point, scaled to size specified by width and height
  class DrawableCompositeImage : public DrawableBase
  {
  public:
    DrawableCompositeImage ( double x_, double y_,
                             double width_, double height_,
                             const std::string &image_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _x;
    double _y;
    double _width;
    double _height;
    const std::string &_image;
  };

  // Line
  class DrawableLine : public DrawableBase
  {
  public:
    DrawableLine ( double startX_, double startY_,
                   double endX_, double endY_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _startX;
    double _startY;
    double _endX;
    double _endY;
  };

  // Change pixel matte value to transparent using PaintMethod
  class DrawableMatte : public DrawableBase
  {
  public:
    DrawableMatte ( double x_, double y_,
                    PaintMethod paintMethod_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _x;
    double _y;
    PaintMethod _paintMethod;
  };

  // path ???? FIXME!

  // Point
  class DrawablePoint : public DrawableBase
  {
  public:
    DrawablePoint ( double x_, double y_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _x;
    double _y;
  };

  // Text pointsize
  class DrawablePointSize : public DrawableBase
  {
  public:
    DrawablePointSize ( double pointSize_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _pointSize;
  };

  // Polygon (Coordinate list must contain at least three members)
  class DrawablePolygon : public DrawableBase
  {
  public:
    DrawablePolygon ( const std::list<Magick::Coordinate> &coordinates_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    std::list<Magick::Coordinate> _coordinates;
  };

  // Polyline (Coordinate list must contain at least three members)
  class DrawablePolyline : public DrawableBase
  {
  public:
    DrawablePolyline ( const std::list<Magick::Coordinate> &coordinates_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    std::list<Magick::Coordinate> _coordinates;
  };

  // pop graphic-context  FIXME!

  // push graphic-context FIXME!

  // Rectangle
  class DrawableRectangle : public DrawableBase
  {
  public:
    DrawableRectangle ( double upperLeftX_, double upperLeftY_,
                        double lowerRightX_, double lowerRightY );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _upperLeftX;
    double _upperLeftY;
    double _lowerRightX;
    double _lowerRightY;
  };

  // Apply Rotation
  class DrawableRotation : public DrawableAffine
  {
  public:
    DrawableRotation ( double angle_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;
  };

  // Round Rectangle
  class DrawableRoundRectangle : public DrawableBase
  {
  public:
    DrawableRoundRectangle ( double centerX_, double centerY_,
                             double width_, double hight_,
                             double cornerWidth_, double cornerHeight_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _centerX;
    double _centerY;
    double _width;
    double _hight;
    double _cornerWidth;
    double _cornerHeight;
  };

  // Apply Scaling
  class DrawableScaling : public DrawableAffine
  {
  public:
    DrawableScaling ( double x_, double y_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;
  };

  // Apply Skew in X direction
  class DrawableSkewX : public DrawableAffine
  {
  public:
    DrawableSkewX ( double angle_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;
  };

  // Apply Skew in Y direction
  class DrawableSkewY : public DrawableAffine
  {
  public:
    DrawableSkewY ( double angle_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;
  };

  // Stroke color
  class DrawableStrokeColor : public DrawableBase
  {
  public:
    DrawableStrokeColor ( const Color &color_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    Color _color;
  };

  // stroke-antialias stroke_antialias FIXME!

  // stroke-dash value FIXME!

  // Stroke opacity
  class DrawableStrokeOpacity : public DrawableBase
  {
  public:
    DrawableStrokeOpacity ( double opacity_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _opacity;
  };

    // Stroke width
  class DrawableStrokeWidth : public DrawableBase
  {
  public:
    DrawableStrokeWidth ( double width_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _width;
  };

  // Draw text at point
  class DrawableText : public DrawableBase
  {
  public:
    DrawableText ( double x_, double y_,
                   const std::string &text_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _x;
    double _y;
    std::string _text;
  };

  // Apply Translation
  class DrawableTranslation : public DrawableAffine
  {
  public:
    DrawableTranslation ( double x_, double y_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;
  };

  // text-antialias text_antialias FIXME!

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

#endif // Drawable_header
