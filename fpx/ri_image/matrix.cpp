//  ----------------------------------------------------------------------------
//  MODULE    : CombinMat 
//  LANGUAGE  : C++ 
//  CREATOR   : Laurent SABORET 
//  CREAT. DATE : Tuesday, April 30, 1996 
//  DESCRIPTION : Matrix that mixes the 3 channels of an RGB pixel 
//  COMMENTS  : 
//  SCCSID      : @(#)matrix.cpp  1.1 11:46:40 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "matrix.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

CombinMat::CombinMat()
{
  int i, j;
  
  active = false;
  for (i=0; i<3; i++) {
    for (j=0; j<4; j++) {
      if (i == j)
        coef[i][j] = 1024;
      else
        coef[i][j] = 0;
    }
  }
}

CombinMat& CombinMat::operator =(const CombinMat& combinMat)

{
  int i, j;
  
  for (i=0; i<3; i++) {
    for (j=0; j<4; j++) {
      this->coef[i][j] = combinMat.coef[i][j];
    }
  }
  this->active = combinMat.active;
  
  return *this;
}

// Apply color combinaison to a BIG ENDIAN pixel
Pixel CombinMat::operator()(const Pixel& pixIn) const
{
  if (active) 
  {
    Pixel  pixOut;
  
    register unsigned char r, v, b;
    register long val;

    r = pixIn.rouge;
    v = pixIn.vert;
    b = pixIn.bleu;
    val = (coef[0][0]*r + coef[0][1]*v + coef[0][2]*b + coef[0][3]) >> 10;
    if (val < 0) val = 0;
    if (val > 0xFF) val = 0xFF;
    pixOut.rouge = (unsigned char) val;
    val  = (coef[1][0]*r + coef[1][1]*v + coef[1][2]*b + coef[1][3]) >> 10;
    if (val < 0) val = 0;
    if (val > 0xFF) val = 0xFF;
    pixOut.vert = (unsigned char) val;
    val  = (coef[2][0]*r + coef[2][1]*v + coef[2][2]*b + coef[2][3]) >> 10;
    if (val < 0) val = 0;
    if (val > 0xFF) val = 0xFF;
    pixOut.bleu = (unsigned char) val;
    pixOut.alpha = pixIn.alpha;
    return pixOut;
  } 
  else
    return pixIn;
}

// Apply color combinaison to a NATIVE pixel
NPixel CombinMat::operator()(const NPixel& pixIn) const
{
  if (active) 
  {
    NPixel  pixOut;
  
    register unsigned char r, v, b;
    register long val;

    r = pixIn.Red();
    v = pixIn.Green();
    b = pixIn.Blue();
    val = (coef[0][0]*r + coef[0][1]*v + coef[0][2]*b + coef[0][3]) >> 10;
    if (val < 0) val = 0;
    if (val > 0xFF) val = 0xFF;
    pixOut.Red() = (unsigned char) val;
    val  = (coef[1][0]*r + coef[1][1]*v + coef[1][2]*b + coef[1][3]) >> 10;
    if (val < 0) val = 0;
    if (val > 0xFF) val = 0xFF;
    pixOut.Green() = (unsigned char) val;
    val  = (coef[2][0]*r + coef[2][1]*v + coef[2][2]*b + coef[2][3]) >> 10;
    if (val < 0) val = 0;
    if (val > 0xFF) val = 0xFF;
    pixOut.Blue() = (unsigned char) val;
    pixOut.Alpha() = pixIn.Alpha();
    return pixOut;
  } 
  else
    return pixIn;
}

/* Compute a*b

   Algo:
    Mathematically, CombinMat matrix "a" is a 3x3 matrix Ma = ( coef[i][j] >> 10 ) for 0<=i<=2 and 0<=j<=2 
                        + a 3x1 vector Va = ( coef[i][3] >> 10 ) for 0<=i<=2
    For any pixel P, we have a(P) = Ma*P + Va
    
    So, a*b*P = Ma*(Mb*P + Vb) + Va = Ma*Mb*P + (Ma*Vb + Va)
*/
CombinMat operator*(const CombinMat& a, const CombinMat& b)

{
  if (!a.active)
  {
    return b;
  }
  else if (!b.active)
  {
    return a;
  }
  else
  {
    CombinMat ab;     // == a * b
    int     i, j;
  
    ab.active = true;
    
    for ( i=0 ; i < 3 ; i++ )
    {
      for ( j=0 ; j < 3 ; j++ )
        ab.coef[i][j] = (a.coef[i][0]*b.coef[0][j] + a.coef[i][1]*b.coef[1][j] + a.coef[i][2]*b.coef[2][j]) >> 10;
        
      ab.coef[i][3] = (((a.coef[i][0]*b.coef[0][3]) + (a.coef[i][1]*b.coef[1][3]) + (a.coef[i][2]*b.coef[2][3])) >> 10) + a.coef[i][3];
    }
    
    return ab;
  }
}

//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------



//  - EOF ----------------------------------------------------------------------
