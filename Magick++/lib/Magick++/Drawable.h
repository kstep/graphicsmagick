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
    Coordinate ( void )
      : _x(0),
        _y(0)
      { }
    Coordinate ( double x_, double y_ )
      : _x(x_),
        _y(y_)
      { }
    ~Coordinate ()
      { }

    void   x ( double x_ )
      {
        _x = x_;
      }
    double x ( void ) const
      {
        return _x;
      }

    void   y ( double y_ )
      {
        _y = y_;
      }
    double y ( void ) const
      {
        return _y;
      }

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

    // Assignment operator
    const DrawableBase& operator= (const DrawableBase& original_ );

    // Print object to stream
    friend std::ostream& operator<< (std::ostream& stream_,
                                     const DrawableBase& drawable_);

    // Support a polymorphic print-to-stream operator
    virtual void print (std::ostream& stream_) const = 0;

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
    friend std::ostream& operator<< (std::ostream& stream_,
                                     const Drawable& drawable_);
  private:
    DrawableBase* dp;
  };

  //
  // Base class for all drawable path elements for use with
  // DrawablePath
  //
  class PathBase
  {
  public:
    // Constructor
    PathBase ( void );

    // Destructor
    virtual ~PathBase ( void );

    // Assignment operator
    const PathBase& operator= (const PathBase& original_ );

    // Print object to stream
    friend std::ostream& operator<< (std::ostream& stream_,
                                     const PathBase& drawable_);

    // Support a polymorphic print-to-stream operator
    virtual void print (std::ostream& stream_) const = 0;

    // Return polymorphic copy of object
    virtual PathBase* copy() const = 0;
  };

  //
  // Representation of a drawable path element surrogate object to
  // manage drawable path elements so they may be passed as a list to
  // DrawablePath.
  //
  class Path
  {
  public:
    // Constructor
    Path ( void );

    // Construct from PathBase
    Path ( const PathBase& original_ );

    // Destructor
    ~Path ( void );

    // Copy constructor
    Path ( const Path& original_ );

    // Assignment operator
    Path& operator= (const Path& original_ );

    // Print object to stream
    friend std::ostream& operator<< (std::ostream& stream_,
                                     const Path& drawable_);
  private:
    PathBase* dp;
  };

  //
  // Drawable Objects
  //

  // Affine (scaling, rotation, and translation)
  class DrawableAffine  : public DrawableBase
  {
  public:
    DrawableAffine ( double sx_, double sy_,
                     double rx_, double ry_,
                     double tx_, double ty_ )
      : _sx(sx_),
        _sy(sy_),
        _rx(rx_),
        _ry(ry_),
        _tx(tx_),
        _ty(ty_)
      {
      }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableAffine(*this);
      }

  protected:
    double _sx;
    double _sy;
    double _rx;
    double _ry;
    double _tx;
    double _ty;
  };

  // Angle (drawing angle)
  class DrawableAngle  : public DrawableBase
  {
  public:
    DrawableAngle ( double angle_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _x;
    double _y;
    double _width;
    double _height;
    const std::string _image;
  };

  // Line
  class DrawableLine : public DrawableBase
  {
  public:
    DrawableLine ( double startX_, double startY_,
                   double endX_, double endY_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _x;
    double _y;
    PaintMethod _paintMethod;
  };

  // Drawable Path
  class DrawablePath : public DrawableBase
  {
  public:
    DrawablePath ( const std::list<Magick::Path> &path_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    std::list<Magick::Path> _path;
  };

  // Point
  class DrawablePoint : public DrawableBase
  {
  public:
    DrawablePoint ( double x_, double y_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    std::list<Magick::Coordinate> _coordinates;
  };

  // Pop Graphic Context
  class DrawablePopGraphicContext : public DrawableBase
  {
  public:
    DrawablePopGraphicContext ( void )
      { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const
      {
        return new DrawablePopGraphicContext(*this);
      }

  private:
  };

  // Push Graphic Context
  class DrawablePushGraphicContext : public DrawableBase
  {
  public:
    DrawablePushGraphicContext ( void )
      { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const
      {
        return new DrawablePushGraphicContext(*this);
      }

  private:
  };

  // Rectangle
  class DrawableRectangle : public DrawableBase
  {
  public:
    DrawableRectangle ( double upperLeftX_, double upperLeftY_,
                        double lowerRightX_, double lowerRightY );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _upperLeftX;
    double _upperLeftY;
    double _lowerRightX;
    double _lowerRightY;
  };

  // Apply Rotation
  class DrawableRotation : public DrawableBase
  {
  public:
    DrawableRotation ( double angle_ )
      : _angle( angle_ )
      {
      }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableRotation(*this);
      }

  private:
    double _angle;
  };

  // Round Rectangle
  class DrawableRoundRectangle : public DrawableBase
  {
  public:
    DrawableRoundRectangle ( double centerX_, double centerY_,
                             double width_, double hight_,
                             double cornerWidth_, double cornerHeight_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

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
  class DrawableScaling : public DrawableBase
  {
  public:
    DrawableScaling ( double x_, double y_ )
      : _x(x_),
        _y(y_)
      {
      }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableScaling(*this);
      }

  private:
    double _x;
    double _y;
  };

  // Apply Skew in X direction
  class DrawableSkewX : public DrawableBase
  {
  public:
    DrawableSkewX ( double angle_ )
      : _angle(angle_)
      {
      }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableSkewX(*this);
      }

  private:
    double _angle;
  };

  // Apply Skew in Y direction
  class DrawableSkewY : public DrawableBase
  {
  public:
    DrawableSkewY ( double angle_ )
      : _angle(angle_)
      {
      }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableSkewY(*this);
      }

  private:
    double _angle;
  };


  // Stroke antialias
  class DrawableStrokeAntialias : public DrawableBase
  {
  public:
    DrawableStrokeAntialias ( bool flag_ )
      : _flag(flag_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableStrokeAntialias(*this);
      }

  private:
    bool _flag;
  };

  // Stroke color
  class DrawableStrokeColor : public DrawableBase
  {
  public:
    DrawableStrokeColor ( const Color &color_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    Color _color;
  };

  // stroke-dash value FIXME!

  // Stroke opacity
  class DrawableStrokeOpacity : public DrawableBase
  {
  public:
    DrawableStrokeOpacity ( double opacity_ )
      : _opacity(opacity_)
      {
      }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableStrokeOpacity(*this);
      }

  private:
    double _opacity;
  };

  // Stroke width
  class DrawableStrokeWidth : public DrawableBase
  {
  public:
    DrawableStrokeWidth ( double width_ );

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

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
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ DrawableBase* copy() const;

  private:
    double _x;
    double _y;
    std::string _text;
  };

  // Apply Translation
  class DrawableTranslation : public DrawableBase
  {
  public:
    DrawableTranslation ( double x_, double y_ )
      : _x(x_),
        _y(y_)
      {
      }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableTranslation(*this);
      }

  private:
    double _x;
    double _y;
  };

  // Text antialias
  class DrawableTextAntialias : public DrawableBase
  {
  public:
    DrawableTextAntialias ( bool flag_ )
      : _flag(flag_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/
    DrawableBase* copy() const
      {
        return new DrawableTextAntialias(*this);
      }

  private:
    bool _flag;
  };

  //
  // Path Element Classes To Support DrawablePath
  //
  class PathArcArgs
  {
  public:
    PathArcArgs( double radiusX_, double radiusY_,
                 double xAxisRotation_, bool largeArcFlag_,
                 bool sweepFlag_, double x_, double y_ )
      : _radiusX(radiusX_),
        _radiusY(radiusY_),
        _xAxisRotation(xAxisRotation_),
        _largeArcFlag(largeArcFlag_),
        _sweepFlag(sweepFlag_),
        _x(x_),
        _y(y_) { }

   friend std::ostream& operator<<( std::ostream& stream_,
                                    const PathArcArgs& args_ );

  private:
    double	_radiusX;	// X radius
    double	_radiusY;	// Y radius
    double	_xAxisRotation;	// Rotation relative to X axis
    bool	_largeArcFlag;	// Draw longer of the two matching arcs
    bool	_sweepFlag;	// Draw arc matching clock-wise rotation
    double	_x;		// End-point X
    double	_y;		// End-point Y
  };

  // Path Arc (Elliptical Arc)
  class PathArcAbs : public PathBase
  {
  public:
    // Draw a single arc segment
    PathArcAbs ( const Magick::PathArcArgs &coordinates_ )
      : _coordinates(1,coordinates_) { }

    // Draw multiple arc segments
    PathArcAbs ( const std::list<Magick::PathArcArgs> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathArcAbs(*this);
    }

  private:
    std::list<Magick::PathArcArgs> _coordinates;
  };
  class PathArcRel : public PathBase
  {
  public:
    // Draw a single arc segment
    PathArcRel ( const Magick::PathArcArgs &coordinates_ )
      : _coordinates(1,coordinates_) { }

    // Draw multiple arc segments
    PathArcRel ( const std::list<Magick::PathArcArgs> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathArcRel(*this);
    }

  private:
    std::list<Magick::PathArcArgs> _coordinates;
  };

  // Path Closepath
  class PathClosePath : public PathBase
  {
  public:
    PathClosePath ( void )
      { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathClosePath(*this);
    }

  private:

  };

  //
  // Curveto (Cubic Bezier)
  //
  class PathCurvetoArgs
  {
  public:
    PathCurvetoArgs( double x1_, double y1_,
                     double x2_, double y2_,
                     double x_, double y_ )
      : _x1(x1_),
        _y1(y1_),
        _x2(x2_),
        _y2(y2_),
        _x(x_),
        _y(y_)
      {
      }
    
    friend std::ostream& operator<<( std::ostream& stream_,
                                     const PathCurvetoArgs& args_ );
  private:
    double _x1;
    double _y1;
    double _x2;
    double _y2;
    double _x;
    double _y;
  };
  class PathCurvetoAbs : public PathBase
  {
  public:
    // Draw a single curve
    PathCurvetoAbs ( const Magick::PathCurvetoArgs &args_ )
      : _args(1,args_) { }

    // Draw multiple curves
    PathCurvetoAbs ( const std::list<Magick::PathCurvetoArgs> &args_ )
      : _args(args_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ 
    PathBase* copy() const
      {
        return new PathCurvetoAbs(*this);
      }

  private:
    std::list<Magick::PathCurvetoArgs> _args;
  };
  class PathCurvetoRel : public PathBase
  {
  public:
    // Draw a single curve
    PathCurvetoRel ( const Magick::PathCurvetoArgs &args_ )
      : _args(1,args_) { }

    // Draw multiple curves
    PathCurvetoRel ( const std::list<Magick::PathCurvetoArgs> &args_ )
      : _args(args_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ 
    PathBase* copy() const
      {
        return new PathCurvetoRel(*this);
      }

  private:
    std::list<Magick::PathCurvetoArgs> _args;
  };
  class PathSmoothCurvetoAbs : public PathBase
  {
  public:
    // Draw a single curve
    PathSmoothCurvetoAbs ( const Magick::Coordinate &coordinates_ )
      : _coordinates(1,coordinates_) { }

    // Draw multiple curves
    PathSmoothCurvetoAbs ( const std::list<Magick::Coordinate> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ 
    PathBase* copy() const
      {
        return new PathSmoothCurvetoAbs(*this);
      }

  private:
    std::list<Magick::Coordinate> _coordinates;
  };
  class PathSmoothCurvetoRel : public PathBase
  {
  public:
    // Draw a single curve
    PathSmoothCurvetoRel ( const Magick::Coordinate &coordinates_ )
      : _coordinates(1,coordinates_) { }

    // Draw multiple curves
    PathSmoothCurvetoRel ( const std::list<Magick::Coordinate> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ 
    PathBase* copy() const
      {
        return new PathSmoothCurvetoRel(*this);
      }

  private:
    std::list<Magick::Coordinate> _coordinates;
  };

  //
  // Quadratic Curveto (Quadratic Bezier)
  //
  class PathQuadraticCurvetoArgs
  {
  public:
    PathQuadraticCurvetoArgs( double x1_, double y1_,
                              double x_, double y_ )
      : _x1(x1_),
        _y1(y1_),
        _x(x_),
        _y(y_)
      {
      }
    
    friend std::ostream& operator<<( std::ostream& stream_,
                                     const PathQuadraticCurvetoArgs& args_ );
  private:
    double _x1;
    double _y1;
    double _x;
    double _y;
  };
  class PathQuadraticCurvetoAbs : public PathBase
  {
  public:
    // Draw a single curve
    PathQuadraticCurvetoAbs ( const Magick::PathQuadraticCurvetoArgs &args_ )
      : _args(1,args_) { }

    // Draw multiple curves
    PathQuadraticCurvetoAbs ( const std::list<Magick::PathQuadraticCurvetoArgs> &args_ )
      : _args(args_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ 
    PathBase* copy() const
      {
        return new PathQuadraticCurvetoAbs(*this);
      }

  private:
    std::list<Magick::PathQuadraticCurvetoArgs> _args;
  };
  class PathQuadraticCurvetoRel : public PathBase
  {
  public:
    // Draw a single curve
    PathQuadraticCurvetoRel ( const Magick::PathQuadraticCurvetoArgs &args_ )
      : _args(1,args_) { }

    // Draw multiple curves
    PathQuadraticCurvetoRel ( const std::list<Magick::PathQuadraticCurvetoArgs> &args_ )
      : _args(args_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ 
    PathBase* copy() const
      {
        return new PathQuadraticCurvetoRel(*this);
      }

  private:
    std::list<Magick::PathQuadraticCurvetoArgs> _args;
  };
  class PathSmoothQuadraticCurvetoAbs : public PathBase
  {
  public:
    // Draw a single curve
    PathSmoothQuadraticCurvetoAbs ( const Magick::Coordinate &coordinate_ )
      : _coordinates(1,coordinate_) { }

    // Draw multiple curves
    PathSmoothQuadraticCurvetoAbs ( const std::list<Magick::Coordinate> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ 
    PathBase* copy() const
      {
        return new PathSmoothQuadraticCurvetoAbs(*this);
      }

  private:
    std::list<Magick::Coordinate> _coordinates;
  };
  class PathSmoothQuadraticCurvetoRel : public PathBase
  {
  public:
    // Draw a single curve
    PathSmoothQuadraticCurvetoRel ( const Magick::Coordinate &coordinate_ )
      : _coordinates(1,coordinate_) { }

    // Draw multiple curves
    PathSmoothQuadraticCurvetoRel ( const std::list<Magick::Coordinate> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ 
    PathBase* copy() const
      {
        return new PathSmoothQuadraticCurvetoRel(*this);
      }

  private:
    std::list<Magick::Coordinate> _coordinates;
  };

  //
  // Path Lineto
  //
  class PathLinetoAbs : public PathBase
  {
  public:
    // Draw to a single point
    PathLinetoAbs ( const Magick::Coordinate& coordinate_  )
      : _coordinates(1,coordinate_) { }

    // Draw to multiple points
    PathLinetoAbs ( const std::list<Magick::Coordinate> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathLinetoAbs(*this);
    }

  private:
    std::list<Magick::Coordinate> _coordinates;
  };
  class PathLinetoRel : public PathBase
  {
  public:
    // Draw to a single point
    PathLinetoRel ( const Magick::Coordinate& coordinate_  )
      : _coordinates(1,coordinate_) { }

    // Draw to multiple points
    PathLinetoRel ( const std::list<Magick::Coordinate> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathLinetoRel(*this);
    }

  private:
    std::list<Magick::Coordinate> _coordinates;
  };

  // Path Horizontal Lineto
  class PathLinetoHorizontalAbs : public PathBase
  {
  public:
    PathLinetoHorizontalAbs ( double x_ )
      : _x(x_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathLinetoHorizontalAbs(*this);
    }

  private:
    double _x;
  };
  class PathLinetoHorizontalRel : public PathBase
  {
  public:
    PathLinetoHorizontalRel ( double x_ )
      : _x(x_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathLinetoHorizontalRel(*this);
    }

  private:
    double _x;
  };

  // Path Vertical Lineto
  class PathLinetoVerticalAbs : public PathBase
  {
  public:
    PathLinetoVerticalAbs ( double y_ )
      : _y(y_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathLinetoVerticalAbs(*this);
    }

  private:
    double _y;
  };
  class PathLinetoVerticalRel : public PathBase
  {
  public:
    PathLinetoVerticalRel ( double y_ )
      : _y(y_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathLinetoVerticalRel(*this);
    }

  private:
    double _y;
  };

  // Path Moveto
  class PathMovetoAbs : public PathBase
  {
  public:
    // Simple moveto
    PathMovetoAbs ( const Magick::Coordinate &coordinate_ )
      : _coordinates(1,coordinate_) { }

    // Moveto followed by implicit linetos
    PathMovetoAbs ( const std::list<Magick::Coordinate> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathMovetoAbs(*this);
    }

  private:
    std::list<Magick::Coordinate> _coordinates;
  };
  class PathMovetoRel : public PathBase
  {
  public:
    // Simple moveto
    PathMovetoRel ( const Magick::Coordinate &coordinate_ )
      : _coordinates(1,coordinate_) { }

    // Moveto followed by implicit linetos
    PathMovetoRel ( const std::list<Magick::Coordinate> &coordinates_ )
      : _coordinates(coordinates_) { }

    // Support a polymorphic print-to-stream operator
    /*virtual*/ void print (std::ostream& stream_) const;

    // Return polymorphic copy of object
    /*virtual*/ PathBase* copy() const {
      return new PathMovetoRel(*this);
    }

  private:
    std::list<Magick::Coordinate> _coordinates;
  };

} // namespace Magick

#endif // Drawable_header
