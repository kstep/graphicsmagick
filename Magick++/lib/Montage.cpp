// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Implementation of Montage
//

#define MAGICK_IMPLEMENTATION

#include <string>

#include "Magick++/Montage.h"
#include "Magick++/Functions.h"
#include "Magick++/Include.h"

Magick::Montage::Montage ( void )
  : _backgroundColor("#ffffff"),
    _compose(OverCompositeOp),
    _fileName(),
    _fill("#000000ff"),
    _font(),
    _geometry("120x120+4+3>"),
    _gravity(CenterGravity),
    _label(),
    _pointSize(12),
    _shadow(false),
    _stroke(),
    _texture(),
    _tile("6x4"),
    _title(),
    _transparentColor()
{
}

Magick::MontageFramed::MontageFramed ( void )
  : _borderColor("#dfdfdf"),
    _borderWidth(0),
    _frame(),
    _matteColor("#bdbdbd")
{
}

void Magick::Montage::updateMontageInfo ( MontageInfo &montageInfo_ ) const
{

  memset(&montageInfo_,0,sizeof(MontageInfo));

  // background_color
  montageInfo_.background_color = _backgroundColor;
  // border_color
  montageInfo_.border_color = Color();
  // border_width
  montageInfo_.border_width = 0;
  // filename
  _fileName.copy( montageInfo_.filename, MaxTextExtent - 1 );
  montageInfo_.filename[ _fileName.length() ] = 0; // null terminate
  // fill
  montageInfo_.fill = _fill;
  // font
  if ( _font.length() != 0 )
    Magick::CloneString( &montageInfo_.font, _font );
  else
    LiberateMemory(reinterpret_cast<void **>(&montageInfo_.font));
  // frame
  LiberateMemory(reinterpret_cast<void **>(&montageInfo_.frame));
  // geometry
  if ( _geometry.isValid() )
    Magick::CloneString( &montageInfo_.geometry, _geometry );
  else
    LiberateMemory(reinterpret_cast<void **>(&montageInfo_.geometry));
  // gravity
  montageInfo_.gravity = _gravity;
  // matte_color
  montageInfo_.matte_color = Color();
  // pointsize
  montageInfo_.pointsize = _pointSize;
  // shadow
  montageInfo_.shadow = static_cast<int>(_shadow);
  // signature (validity stamp)
  montageInfo_.signature = MagickSignature;
  // stroke
  montageInfo_.stroke = _stroke;
  // texture
  if ( _texture.length() != 0 )
    Magick::CloneString( &montageInfo_.texture, _texture );
  else
    LiberateMemory(reinterpret_cast<void **>(&montageInfo_.texture));
  // tile
  if ( _tile.isValid() )
    Magick::CloneString( &montageInfo_.tile, _tile );
  else
    LiberateMemory(reinterpret_cast<void **>(&montageInfo_.tile));
  // title
  if ( _title.length() != 0 )
    Magick::CloneString( &montageInfo_.title, _title );
  else
    LiberateMemory(reinterpret_cast<void **>(&montageInfo_.title));
}

//
// Implementation of MontageFramed
//

/* virtual */ void
Magick::MontageFramed::updateMontageInfo ( MontageInfo &montageInfo_ ) const
{
  // Do base updates
  Montage::updateMontageInfo ( montageInfo_ );

  // border_color
  montageInfo_.border_color = _borderColor;
  // border_width
  montageInfo_.border_width = _borderWidth;
  // frame
  if ( _frame.isValid() )
    Magick::CloneString( &montageInfo_.frame, _frame );
  else
    LiberateMemory(reinterpret_cast<void **>(&montageInfo_.frame));
  // matte_color
  montageInfo_.matte_color = _matteColor;
}
