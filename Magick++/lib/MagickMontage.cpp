// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Implementation of Montage
//

#define MAGICK_IMPLEMENTATION

#include <string>

#include <MagickMontage.h>
#include <MagickFunctions.h>

//
// Implementation of Montage
//

Magick::Montage::Montage ( void )
  : _backgroundColor(),
    _compose(UndefinedCompositeOp),
    _fileName(),
    _font(),
    _geometry(),
    _gravity(0),
    _label(),
    _pen(),
    _pointSize(0),
    _shadow(false),
    _texture(),
    _tile(),
    _title(),
    _transparentColor()
{
}

Magick::Montage::~Montage( void )
{
  // Nothing to do
}

void Magick::Montage::backgroundColor ( const Magick::Color &backgroundColor_ )
{
  _backgroundColor = backgroundColor_;
}
Magick::Color Magick::Montage::backgroundColor ( void ) const
{
  return _backgroundColor;
}

void Magick::Montage::compose ( Magick::CompositeOperator compose_ )
{
  _compose = compose_;
}
Magick::CompositeOperator Magick::Montage::compose ( void ) const
{
  return _compose;
}

void Magick::Montage::fileName( std::string fileName_ )
{
  _fileName = fileName_;
}
std::string Magick::Montage::fileName( void ) const
{
  return _fileName;
}

void Magick::Montage::font ( std::string font_ )
{
  _font = font_;
}
std::string Magick::Montage::font ( void ) const
{
  return _font;
}

void Magick::Montage::geometry ( const Geometry &geometry_ )
{
  _geometry = geometry_;
}
Magick::Geometry Magick::Montage::geometry ( void ) const
{
  return _geometry;
}

void Magick::Montage::gravity ( unsigned int gravity_ )
{
  _gravity = gravity_;
}
unsigned int Magick::Montage::gravity ( void ) const
{
  return _gravity;
}

// Apply as attribute to all images before doing montage
void Magick::Montage::label( std::string label_ )
{
  _label = label_;
}
std::string Magick::Montage::label( void ) const
{
  return _label;
}

void Magick::Montage::penColor ( const Color &pen_ )
{
  _pen = pen_;
}
Magick::Color Magick::Montage::penColor ( void ) const
{
  return _pen;
}

void Magick::Montage::pointSize ( unsigned int pointSize_ )
{
  _pointSize = pointSize_;
}
unsigned int Magick::Montage::pointSize ( void ) const
{
  return _pointSize;
}

void Magick::Montage::shadow ( bool shadow_ )
{
  _shadow = shadow_;
}
bool Magick::Montage::shadow ( void ) const
{
  return _shadow;
}

void Magick::Montage::texture ( std::string texture_ )
{
  _texture = texture_;
}
std::string Magick::Montage::texture ( void ) const
{
  return _texture;
}

void Magick::Montage::tile ( const Geometry &tile_ )
{
  _tile = tile_;
}
Magick::Geometry Magick::Montage::tile ( void ) const
{
  return _tile;
}

void Magick::Montage::title ( std::string title_ )
{
  _title = title_;
}
std::string Magick::Montage::title ( void ) const
{
  return _title;
}

// Applied after the fact to montage with TransparentImage()
void Magick::Montage::transparentColor ( const Magick::Color &transparentColor_ )
{
  _transparentColor = transparentColor_;
}
Magick::Color Magick::Montage::transparentColor ( void ) const
{
  return _transparentColor;
}

void Magick::Montage::updateMontageInfo ( MagickLib::MontageInfo &montageInfo_ ) const
{
  if ( _backgroundColor.isValid() )
    {
      Magick::CloneString( &montageInfo_.background_color, _backgroundColor );
    }

  if ( _compose != Magick::UndefinedCompositeOp )
    montageInfo_.compose = _compose;

  if ( _fileName.length() != 0 )
    {
      _fileName.copy( montageInfo_.filename, MaxTextExtent - 1 );
      montageInfo_.filename[ _fileName.length() ] = 0; // null terminate
    }

  if ( _font.length() != 0 )
    Magick::CloneString( &montageInfo_.font, _font );

  if ( _geometry.isValid() )
    Magick::CloneString( &montageInfo_.geometry, _geometry );

  if ( _gravity != 0 )
    montageInfo_.gravity = _gravity;

  if ( _pen.isValid() )
    Magick::CloneString( &montageInfo_.pen, _pen );

  if ( _pointSize != 0 )
    montageInfo_.pointsize = _pointSize;

  if ( _shadow == true )
    montageInfo_.shadow = True;

  if ( _texture.length() != 0 )
    Magick::CloneString( &montageInfo_.texture, _texture );

  if ( _tile.isValid() )
    Magick::CloneString( &montageInfo_.tile, _tile );

  if ( _title.length() != 0 )
    Magick::CloneString( &montageInfo_.title, _title );
}


//
// Implementation of MontageFramed
//

Magick::MontageFramed::MontageFramed ( void )
  : _borderColor(),
    _borderWidth(0),
    _frame(),
    _matteColor()
{
}

/* virtual */ Magick::MontageFramed::~MontageFramed ( void )
{
  // Nothing to do
}

void Magick::MontageFramed::borderColor ( const Magick::Color &borderColor_ )
{
  _borderColor = borderColor_;
}
Magick::Color Magick::MontageFramed::borderColor ( void ) const
{
  return _borderColor;
}

void Magick::MontageFramed::borderWidth ( unsigned int borderWidth_ )
{
  _borderWidth = borderWidth_;
}
unsigned int Magick::MontageFramed::borderWidth ( void ) const
{
  return _borderWidth;
}

void Magick::MontageFramed::frameGeometry ( const Magick::Geometry &frame_ )
{
  _frame = frame_;
}
Magick::Geometry Magick::MontageFramed::frameGeometry ( void ) const
{
  return _frame;
}

void Magick::MontageFramed::matteColor ( const Magick::Color &matteColor_ )
{
  _matteColor = matteColor_;
}
Magick::Color Magick::MontageFramed::matteColor ( void ) const
{
  return _matteColor;
}

/* virtual */ void  Magick::MontageFramed::updateMontageInfo ( MagickLib::MontageInfo &montageInfo_ ) const
{
  // Do base updates
  Montage::updateMontageInfo ( montageInfo_ );

  if ( _borderColor.isValid() )
    Magick::CloneString( &montageInfo_.border_color, _borderColor );

  if ( _borderWidth != 0 )
    montageInfo_.border_width = _borderWidth;

  if ( _frame.isValid() )
    Magick::CloneString( &montageInfo_.frame, _frame );

  if ( _matteColor.isValid() )
    Magick::CloneString( &montageInfo_.matte_color, _matteColor );
}
