//  ------------------------------------------------------------------------------------------------
//  MODULE    : Graphique
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : October 15, 1992
//  DESCRIPTION : Geometical classes with integer coordinates
//  COMMENTAIRE :
//  SCCSID      : @(#)graphic.cpp 1.1 11:50:02 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
  #include "graphic.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

//  Constantes
//  ----------

//  Variables
//  ---------

//  ------------------------------------------------------------------------------------------------
//  Internal C functions
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methods of the class Position
//  ------------------------------------------------------------------------------------------------

Position::Position()

{
}

Position::Position(short x, short y)

{
  v = y;
  h = x;
}

// Is Position inside a rectangle?
Boolean Position::Interne(short x0, short y0, short x1, short y1) const

{
  return ((h >= x0) && (h <= x1) && (v >= y0) && (v <= y1));
}

Position Position::operator +=(const Position& p)

{
  this->h += p.h;
  this->v += p.v;
  return *this;
}

Position Position::operator -=(const Position& p)

{
  this->h -= p.h;
  this->v -= p.v;
  return *this;
}

int operator ==(const Position& p1, const Position& p2)

{
  return (p1.h==p2.h && p1.v==p2.v);
}

int operator !=(const Position& p1, const Position& p2)

{
  return !(p1.h==p2.h && p1.v==p2.v);
}

Position operator +(Position p1, const Position& p2)

{
  p1 += p2;
  return p1;
}

Position operator -(Position p1, const Position& p2)

{
  p1 -= p2;
  return p1;
}


//  ------------------------------------------------------------------------------------------------
//  Methods of the class PRectangle
//  ------------------------------------------------------------------------------------------------

PRectangle::PRectangle()

{
}

// Height
short PRectangle::Hauteur() const

{
  return bottom - top;
}

// Width
short PRectangle::Largeur() const

{
  return right - left;
}


// Add to origin by amount p
PRectangle PRectangle::operator +=(const Position& p)

{
  this->left  += p.h;
  this->right += p.h;
  this->top += p.v;
  this->bottom  += p.v;
  return *this;
}

// Subtract from origin by amount p
PRectangle PRectangle::operator -=(const Position& p)

{
  this->left  -= p.h;
  this->right -= p.h;
  this->top -= p.v;
  this->bottom  -= p.v;
  return *this;
}

PRectangle::PRectangle(short x0, short y0, short x1, short y1)

{
  if (x0 < x1) {
    left = x0;
    right = x1;
  } else {
    left = x1;
    right = x0;
  }
  if (y0 < y1) {
    top = y0;
    bottom = y1;
  } else {
    top = y1;
    bottom = y0;
  }
}

PRectangle::PRectangle(const Position& p1, const Position& p2)

{
  if (p1.h < p2.h) {
    left = p1.h;
    right = p2.h;
  } else {
    left = p2.h;
    right = p1.h;
  }
  if (p1.v < p2.v) {
    top = p1.v;
    bottom = p2.v;
  } else {
    top = p2.v;
    bottom = p1.v;
  }
}

Boolean PRectangle::Contient(const Position& p) const

{
  return ( (p.h >= left) && (p.h < right) && (p.v >= top) && (p.v < bottom) );
}

Boolean PRectangle::IsEmpty()
{
  return ((top >= bottom) || (left >= right));
}

#if defined(_WINDOWS) || defined(_UNIX)
int operator ==(const PRectangle &r1, const PRectangle &r2)
#else
int operator ==(const PRectangle &const r1, const PRectangle &const r2)
#endif

{
  return (r1.left==r2.left && r1.right==r2.right
      && r1.top==r2.top && r1.bottom==r2.bottom);
}

#if defined(_WINDOWS) || defined(_UNIX)
int operator !=(const PRectangle &r1, const PRectangle &r2)
#else
int operator !=(const PRectangle &const r1, const PRectangle &const r2)
#endif

{
  return !(r1.left==r2.left && r1.right==r2.right
      && r1.top==r2.top && r1.bottom==r2.bottom);
}

// Union of two rectangles
void PRectangle::Union(const PRectangle& rect)

{
  if (rect.left < left) left = rect.left;
  if (rect.right > right) right = rect.right;
  if (rect.top < top) top = rect.top;
  if (rect.bottom > bottom) bottom = rect.bottom;
}

// Union of two rectangles
PRectangle PRectangle::operator |=(const PRectangle& r)

{
  Union( r );
  return *this;
}

// Union of two rectangles
#if defined(_WINDOWS) || defined(_UNIX)
PRectangle operator |(const PRectangle &r1, const PRectangle &r2)
#else
PRectangle operator |(const PRectangle& const r1, const PRectangle& const r2)
#endif

{
  PRectangle  rr = r1;
  rr |= r2;
  return rr;
}

// Intersection of two rectangles
void PRectangle::Intersection(const PRectangle& rect)

{
  if (rect.left > left) left = rect.left;
  if (rect.right < right) right = rect.right;
  if (rect.top > top) top = rect.top;
  if (rect.bottom < bottom) bottom = rect.bottom;
}

// Intersection of two rectangles
PRectangle PRectangle::operator &=(const PRectangle& r)

{
  Intersection( r );
  return *this;
}

// Intersection of two rectangles
#if defined(_WINDOWS) || defined(_UNIX)
PRectangle operator &(const PRectangle &r1, const PRectangle &r2)
#else
PRectangle operator &(const PRectangle &const r1, const PRectangle &const r2)
#endif

{
  PRectangle  rr = r1;
  rr &= r2;
  return rr;
}
