//  ------------------------------------------------------------------------------------------------
//  MODULE    : Geometrie
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : Mercredi 16 Septembre 1992
//  DESCRIPTION : 
//  COMMENTAIRE :
//  SCCSID      : @(#)geometry.cpp  1.1 11:50:01 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #include "geometry.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#include <stdlib.h>

  #include <math.h>
#ifdef applec
  #include <SANE.h>           // Define pi() function
#else
  #define   pi()    PI          // Define pi() to PI const.
#endif

#ifndef Debug_h
  #include "debug.h"
#endif

#ifndef Numbers_h
  #include "numbers.h"
#endif


//  Constantes
//  ----------

//  Variables
//  ---------

long tableProf[] = { 1, 5, 14, 39, 120, 409, 1498, 5723, 22364, 88413, 351582, 1402207 };

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Fonctions internes
//  ------------------------------------------------------------------------------------------------

// Compute the determinant of a square matrix of size width
// CAUTION : recursive function

static float Determinant (float* matrix, long width)

{
  float det = (float)0.0;
  if (width == 2)
    det = matrix[0]*matrix[3] - matrix[1]*matrix[2];
  else {
    float coef = (float)1.0;
    long subWidth = width-1;
    float* subMatrix = new float[subWidth*subWidth];
    if (subMatrix==NULL) {
      return (float) 0.0;
    }
    // The algorithm uses the development of the determinant along the first line
    for (long n = 0; n < width; n++) {
      if (!FLOAT_NUL(matrix[n])) {
        // Extract submatrix
        float* line = matrix + width;   // Skip the first line
        float* subLine = subMatrix;
        for (long i = 0; i < subWidth; i++)
          for (long j = 0; j < width; j++) {
            if (j == n)
              line++;         // Skip the nth column
            else
              *subLine++ = *line++; // Copy element in submatrix
          }
        // Compute det
        det += coef * matrix[n] * Determinant (subMatrix,subWidth);
      }
      coef *= (float)-1.0;
    }
    delete [] subMatrix;
  }
  return det;
}

//  ------------------------------------------------------------------------------------------------
//  Methodes
//  ------------------------------------------------------------------------------------------------

// Constructors of affine transformation matrix :
// ----------------------------------------------

// Default is Identity :

TransfoPerspective::TransfoPerspective ()

{
  a  = (float)1;
  b  = (float)0;
  c  = (float)0;
  d  = (float)1;
  x0 = (float)0;
  y0 = (float)0;
  px = (float)0;
  py = (float)0;
}

// Copy construction

TransfoPerspective::TransfoPerspective (const TransfoPerspective& Ta)

{
  a  = Ta.a;
  b  = Ta.b;
  c  = Ta.c;
  d  = Ta.d;
  x0 = Ta.x0;
  y0 = Ta.y0;
  px = Ta.px;
  py = Ta.py;
}

// Explicit construction

TransfoPerspective::TransfoPerspective (float a, float b, float c, float d, float x0, float y0, float px, float py)

{
  this->a  = a;
  this->b  = b;
  this->c  = c;
  this->d  = d;
  this->x0 = x0;
  this->y0 = y0;
  this->px = px;
  this->py = py;
}

// "=" operator

TransfoPerspective& TransfoPerspective::operator =(const TransfoPerspective& transfoPerspective)

{
  a  = transfoPerspective.a;
  b  = transfoPerspective.b;
  c  = transfoPerspective.c;
  d  = transfoPerspective.d;
  x0 = transfoPerspective.x0;
  y0 = transfoPerspective.y0;
  px = transfoPerspective.px;
  py = transfoPerspective.py;
  
  return *this;
}


// Various kind of matrix multiplication :
// ---------------------------------------

// Composition of affine transformation : use : T = T1 * T2;
// CAUTION : This function IS NOT commutative

TransfoPerspective operator*(const TransfoPerspective& T1, const TransfoPerspective& T2)
{ 
  return T1.Produit(T2);
}

// Composition of affine transformation : use : T1 *= T; (<=> T1 = T1 * T)
// CAUTION : This function IS NOT commutative

TransfoPerspective& TransfoPerspective::operator*=(const TransfoPerspective& T)
{ 
  *this = *this * T;
  return *this;
}

// Composition of affine transformation : use : R = T->Produit(Ta); (<=> R = T*Ta)

TransfoPerspective  TransfoPerspective::Produit (const TransfoPerspective& Ta) const
{
  TransfoPerspective TaResult;
  float norm  = px*Ta.x0 + py*Ta.y0 + 1;
  
  TaResult.a  = (a*Ta.a  + c*Ta.b  + x0*Ta.px) / norm;
  TaResult.b  = (b*Ta.a  + d*Ta.b  + y0*Ta.px) / norm;
  TaResult.c  = (a*Ta.c  + c*Ta.d  + x0*Ta.py) / norm;
  TaResult.d  = (b*Ta.c  + d*Ta.d  + y0*Ta.py) / norm;
  TaResult.x0 = (a*Ta.x0 + c*Ta.y0 + x0)       / norm;
  TaResult.y0 = (b*Ta.x0 + d*Ta.y0 + y0)       / norm;
  TaResult.px = (px*Ta.a + py*Ta.b + Ta.px)    / norm;
  TaResult.py = (px*Ta.c + py*Ta.d + Ta.py)    / norm;

  return TaResult;
}

// Transform point (Oxy) coordinates into (O'uv) coordinates and project on z=1 plane (normalize)

PositionMv operator*(const TransfoPerspective& T, const PositionMv& point)
{
  PositionMv result;
  float norm = T.px*point.h + T.py*point.v + 1;

  result.h = (T.a*point.h + T.c*point.v + T.x0) / norm;
  result.v = (T.b*point.h + T.d*point.v + T.y0) / norm;
  
  return result;
}



PositionMv  MultipMatrixVector(const TransfoPerspective& T, const PositionMv& point)
{
  #define EPS (float)0.001
  PositionMv result;
  float norm = T.px*point.h + T.py*point.v + (float)1;
    
    if ( (norm < (float)0) && (norm > (float)-1)  ) norm = EPS;
    
  result.h = (T.a*point.h + T.c*point.v + T.x0)/norm ;
  result.v = (T.b*point.h + T.d*point.v + T.y0)/norm ;
   
  return result;
}



// Transform vector (Oxy) coordinates into (O'uv) coordinates

VecteurMv operator*(const TransfoPerspective& T, const VecteurMv& vector)
{
  VecteurMv result(T*vector.p1, T*vector.p2);
  return result;
}

// Inversion of the transformation  :
// ----------------------------------

void TransfoPerspective::Inverse ()

{
  TransfoPerspective inverse;
  float det;
  
  det = 1 / (a*d - b*c);
  
  inverse.a  =  det * (d - py*y0);
  inverse.b  = -det * (b - px*y0);
  inverse.c  = -det * (c - py*x0);
  inverse.d  =  det * (a - px*x0);
  inverse.x0 =  det * (c*y0 - d*x0);
  inverse.y0 = -det * (a*y0 - b*x0);
  inverse.px =  det * (b*py - d*px);
  inverse.py = -det * (a*py - c*px);

  *this = inverse;  
}

// Modify transformation according to given positions :
// ----------------------------------------------------

void TransfoPerspective::ForcerPosition (TypePositionnement type,  VecteurMv& v1, VecteurMv& v2, VecteurMv& v3)
{
  VecteurMv v4;
  assert (type != FourPoints);
  ForcerPosition(type,v1,v2,v3,v4);
}

void TransfoPerspective::ForcerPosition (TypePositionnement type,  VecteurMv& v1, VecteurMv& v2, VecteurMv& v3, VecteurMv& v4)
{
  switch (type) {
    case T:
    case Sv:
    case Sh:
    case S:
    case TH:
    case TAn:
    case TR:
    case TRA:
      // All the old positionning options shouldn't be used
      assert(FALSE);
      break;
    case Gen:
      ForcerPosition(v1.p1, v1.p2, v2.p1, v2.p2, v3.p1, v3.p2);
      break;
    case FourPoints:
      Set4Points(v1.p1, v2.p1, v3.p1, v4.p1, v1.p2, v2.p2, v3.p2, v4.p2);
      break;
  }
} 

/*

    OLD POSITIONNING FUNCTIONS (Live Picture 1.3r5)

// Translation of a vector pApB in the page space : this is equal to a 
// translation of pBpA in the page space and then apply the transformation
// so T' = T * translat (see Translate())
void TransfoPerspective::ForcerPositionT (PositionMv& pA1, PositionMv& pB1)
{
  if (!(pA1==pB1)) {
    VecteurMv pA1B1(pA1,pB1);
    Translate (pA1B1);
  }
}

// Skewing
void TransfoPerspective::ForcerPositionS (PositionMv& pA1, PositionMv& pB1)
{
  PositionMv pA, pB;
  if (!(pA1==pB1)) {
    pA.h = pA1.h + pB1.v - pA1.v;
    pA.v = pA1.v - pB1.h + pA1.h;
    pB.h = pA1.h - pB1.v + pA1.v;
    pB.v = pA1.v + pB1.h - pA1.h;
    ForcerPosition (pA1, pA1, pB1, pB1, pA, pB);
  }
 }  
  
// 2 points repositionning : Translation o Rotation o Scaling
void TransfoPerspective::ForcerPositionTRA (PositionMv& pA1, PositionMv& pB1, PositionMv& pA2, PositionMv& pB2)
{
  if ((!(pA1==pB1)) || (!(pA2==pB2))) {
    // First Translate and Rotate
    ForcerPositionTR (pA1, pB1, pA2,  pB2);
    
    // Scale |pB1pB2|/|pA1pA2|
    PositionMv pB1B2 = pB2 - pB1;
    PositionMv pA1A2 = pA2 - pA1;
    float ratio = pB1B2.Norme() / pA1A2.Norme();
    Scale (pB1, ratio, ratio);
  }
}

// 2 points repositionning : Translation o Rotation
void TransfoPerspective::ForcerPositionTR (PositionMv& pA1, PositionMv& pB1, PositionMv& pA2, PositionMv& pB2)
{
  if ((!(pA1==pB1)) || (!(pA2==pB2))) {
    // Translate pA1 -> pB1
    ForcerPositionT(pA1,pB1);
    
    // Rotate (A1A2,B1,B1B2)
    float theta; 
    PositionMv pB1B2 = pB2 - pB1;
    PositionMv pA1A2 = pA2 - pA1;
    float sinTheta = pA1A2.h*pB1B2.v - pA1A2.v*pB1B2.h;
    float cosTheta = pA1A2.h*pB1B2.h + pA1A2.v*pB1B2.v;
    if (cosTheta)
      theta = atan(sinTheta/cosTheta);
    else
      theta = pi()/2;
    theta = (sinTheta > 0 ? theta : theta - pi());
    Rotate (pB1, theta);
  }
}

// 2 points repositionning : Translation o ScalingX o ScalingY
void TransfoPerspective::ForcerPositionTH (PositionMv& pA1, PositionMv& pB1, PositionMv& pA2, PositionMv& pB2)
{
  if ((!(pA1==pB1)) || (!(pA2==pB2))) {
    // Translate pA1 -> pB1
    ForcerPositionT(pA1,pB1);
    
    // Scale (pB1pB2)/(pA1pA2) in x and y independently
    PositionMv pB1B2 = pB2 - pB1;
    PositionMv pA1A2 = pA2 - pA1;
    float ratioX = pB1B2.h / pA1A2.h;
    float ratioY = pB1B2.v / pA1A2.v;
    Scale (pB1, ratioX, ratioY);
  }
}

void TransfoPerspective::ForcerPositionTAn (PositionMv& pA1, PositionMv& pB1, PositionMv& pA2, PositionMv& pB2)
{
  if ((!(pA1==pB1)) || (!(pA2==pB2))) {
    assert(FALSE);
  }
}
*/

void TransfoPerspective::ForcerPosition (PositionMv& pA1, PositionMv& pB1, PositionMv& pA2, PositionMv& pB2, PositionMv& pA3, PositionMv& pB3)
{

/*
VISU2 "Enter in ForcerPosition\n" FIN
VISU2 "pA1 : h = %8.2f, v = %8.2f -> pB1 : h = %8.2f, v = %8.2f\n", pA1.h, pA1.v, pB1.h, pB1.v FIN
VISU2 "pA2 : h = %8.2f, v = %8.2f -> pB2 : h = %8.2f, v = %8.2f\n", pA2.h, pA2.v, pB2.h, pB2.v FIN
VISU2 "pA3 : h = %8.2f, v = %8.2f -> pB3 : h = %8.2f, v = %8.2f\n", pA3.h, pA3.v, pB3.h, pB3.v FIN
*/
  if ((!(pA1==pB1)) || (!(pA2==pB2)) || (!(pA3==pB3))) {
    // The idea is to found the matrix transforming B1 into A1, B2 into A2 and B3 into A3
    // and then apply this transfo to the original transformation
    float a, b, c, d, x0, y0;
    
    float matrix[3][3];
    matrix[0][0] = pB1.h; matrix[0][1] = pB1.v; matrix[0][2] = (float)1;
    matrix[1][0] = pB2.h; matrix[1][1] = pB2.v; matrix[1][2] = (float)1;
    matrix[2][0] = pB3.h; matrix[2][1] = pB3.v; matrix[2][2] = (float)1;

    // Compute determinant
    float det = Determinant(&matrix[0][0],3);
    assert (!FLOAT_NUL(det));
  
    // Compute solutions for h components :
    float companionMatrix[3][3];
  
    // First column value : a
    BlockMove (matrix, companionMatrix, 3*3*sizeof(float));
    companionMatrix[0][0] = pA1.h; companionMatrix[1][0] = pA2.h; companionMatrix[2][0] = pA3.h;
    a = Determinant(&companionMatrix[0][0],3) / det;
  
    // Second column value : c
    BlockMove (matrix, companionMatrix, 3*3*sizeof(float));
    companionMatrix[0][1] = pA1.h; companionMatrix[1][1] = pA2.h; companionMatrix[2][1] = pA3.h;
    c = Determinant(&companionMatrix[0][0],3) / det;
  
    // Third column value : x0
    BlockMove (matrix, companionMatrix, 3*3*sizeof(float));
    companionMatrix[0][2] = pA1.h; companionMatrix[1][2] = pA2.h; companionMatrix[2][2] = pA3.h;
    x0 = Determinant(&companionMatrix[0][0],3) / det;

    // Same thing for v components : the matrix is unchanged, only the values to access are changed
    
    // First column value : b
    BlockMove (matrix, companionMatrix, 3*3*sizeof(float));
    companionMatrix[0][0] = pA1.v; companionMatrix[1][0] = pA2.v; companionMatrix[2][0] = pA3.v;
    b = Determinant(&companionMatrix[0][0],3) / det;
  
    // Second column value : d
    BlockMove (matrix, companionMatrix, 3*3*sizeof(float));
    companionMatrix[0][1] = pA1.v; companionMatrix[1][1] = pA2.v; companionMatrix[2][1] = pA3.v;
    d = Determinant(&companionMatrix[0][0],3) / det;
  
    // Third column value : y0
    BlockMove (matrix, companionMatrix, 3*3*sizeof(float));
    companionMatrix[0][2] = pA1.v; companionMatrix[1][2] = pA2.v; companionMatrix[2][2] = pA3.v;
    y0 = Determinant(&companionMatrix[0][0],3) / det;

    // In this case, the (Oxy) is always an affine transfo
    TransfoPerspective trans (a, b, c, d, x0, y0);
    
    // Combine with the original transfo
    *this *= trans;
  }
}

// Affine Primitives Transformations :
// -----------------------------------
// All primitives are defined through two ways : using vectors objects, using float values.

// Translation of origin O' of (O'uv) by (dx,dy) expressed in (Oxy)
void TransfoPerspective::Translate (const VecteurMv& vector)
{
  Translate (vector.p2.h - vector.p1.h, vector.p2.v - vector.p1.v);
  return;
}

void TransfoPerspective::Translate (float dx, float dy)
{
  // Translating origin in (dx,dy) cause coordinates to be translated by (-dx,-dy)
  TransfoPerspective trans((float)1.0, (float)0.0, (float)0.0, (float)1.0, -dx, -dy);
  *this *= trans;
  return;
}

// Apply transfo according to the center (x0,y0) expressed in (Oxy)
void TransfoPerspective::TransApplyTrans (const PositionMv& center, const TransfoPerspective& transfo)
{
  // The idea of postmultiplication is to modify (Oxy) plane before to 
  // apply the original transformation, i.e. :
  //    - Translate the center of transfo to the origin of (0xy) plane
  //    - Apply the transformation in the (Oxy) plane
  //    - Translate back the origin to the center of transfo
  //    - Combine this transfo to the original transformation
  
  TransfoPerspective centerToOrigin ((float)1, (float)0, (float)0, (float)1,-center.h,-center.v);
  TransfoPerspective originToCenter ((float)1, (float)0, (float)0, (float)1, center.h, center.v);
  TransfoPerspective centeredTransfo = originToCenter * (transfo * centerToOrigin);
  *this *= centeredTransfo;
  return;
}

// Rotation of angle theta according to the center (x0,y0) expressed in (Oxy)
void TransfoPerspective::Rotate (const PositionMv& center, float theta)
{
  // Rotate axes of (theta) will cause coordinates to be rotated by -(theta)
  TransfoPerspective rotate ((float)cos(theta), (float)-sin(theta), (float)sin(theta), (float)cos(theta), (float)0.0, (float)0.0);
  TransApplyTrans(center,rotate);
  return;
}

void TransfoPerspective::Rotate (float x0, float y0, float theta)
{
  PositionMv center (x0, y0);
  Rotate (center,theta);
  return;
}

// Scale image horizontal and vertical according to the center (x0,y0) expressed in (Oxy)
void TransfoPerspective::Scale (const PositionMv& center, float sx, float sy)
{
  // Scale axes of (sx,sy) will cause coordinates to be scaled (1/sx,1/sy)
  TransfoPerspective scale ((float)1.0/sx, (float)0.0, (float)0.0, (float)1.0/sy, (float)0.0, (float)0.0);
  TransApplyTrans(center,scale);
  return;
}

void TransfoPerspective::Scale (float x0, float y0, float sx, float sy)
{
  PositionMv center (x0, y0);
  Scale (center, sx, sy);
  return;
}

// Horizontal shear according to the center (x0,y0) expressed in (Oxy)
void TransfoPerspective::ShearHorizontal (const PositionMv& center, float sh)
{
  // Shear along x axe with (sh) will cause coordinates to be sheared backward with -(sh)
  TransfoPerspective shear ((float)1.0, (float)0.0, -sh, (float)1.0, (float)0.0, (float)0.0);
  TransApplyTrans(center,shear);
  return;
}

void TransfoPerspective::ShearHorizontal (float x0, float y0, float sh)
{
  PositionMv center (x0, y0);
  ShearHorizontal (center, sh);
  return;
}

// Vertical shear according to the center (x0,y0) expressed in (Oxy)
void TransfoPerspective::ShearVertical (const PositionMv& center, float sh)
{
  // Shear along y axe with (sh) will cause coordinates to be sheared backward with -(sh)
  TransfoPerspective shear ((float)1.0, -sh, (float)0.0, (float)1.0, (float)0.0, (float)0.0);
  TransApplyTrans(center,shear);
  return;
}

void TransfoPerspective::ShearVertical (float x0, float y0, float sh)
{
  PositionMv center (x0, y0);
  ShearVertical (center, sh);
  return;
}

// Horizontal flip according to the center (x0,y0) expressed in (Oxy)
void TransfoPerspective::FlipHorizontal (const PositionMv& center)
{
  TransfoPerspective flip ((float)-1.0, (float)0.0, (float)0.0, (float)1.0, (float)0.0, (float)0.0);
  TransApplyTrans(center,flip);
  return;
}

void TransfoPerspective::FlipHorizontal (float x0, float y0)
{
  PositionMv center (x0, y0);
  FlipHorizontal (center);
  return;
}

// Vertical flip according to the center (x0,y0) expressed in (Oxy)
void TransfoPerspective::FlipVertical (const PositionMv& center)
{
  TransfoPerspective flip ((float)1.0, (float)0.0, (float)0.0, (float)-1.0, (float)0.0, (float)0.0);
  TransApplyTrans(center,flip);
  return;
}

void TransfoPerspective::FlipVertical (float x0, float y0)
{
  PositionMv center (x0, y0);
  FlipVertical (center);
  return;
}





// General perspective transformation : determine the position in 3D space of an original rectangle to fit
// the perspective point of view set by the user
void TransfoPerspective::Set4Points (const PositionMv& a0, const PositionMv& a1, const PositionMv& a2, const PositionMv& a3, 
                const PositionMv& b0, const PositionMv& b1, const PositionMv& b2, const PositionMv& b3)

//
// Points are given in the current World coordinates system.
// ai are start points and bi end points.
//

{
  // The idea is to found the matrix transforming b0 into a0, b1 into a1, b2 into a2 and b3 into a3
  // and then apply this transfo to the original transformation
  
  // Set linear system
  /* This initializer is valid but not implemented in MPW compiler
  float matrix[8][8] = {
    {b0.h, b0.v, 1, 0, 0, 0, -b0.h*a0.h, -b0.v*a0.h},
    {b1.h, b1.v, 1, 0, 0, 0, -b1.h*a1.h, -b1.v*a1.h},
    {b2.h, b2.v, 1, 0, 0, 0, -b2.h*a2.h, -b2.v*a2.h},
    {b3.h, b3.v, 1, 0, 0, 0, -b3.h*a3.h, -b3.v*a3.h},
    {0, 0, 0, b0.h, b0.v, 1, -b0.h*a0.v, -b0.v*a0.v},
    {0, 0, 0, b1.h, b1.v, 1, -b1.h*a1.v, -b1.v*a1.v},
    {0, 0, 0, b2.h, b2.v, 1, -b2.h*a2.v, -b2.v*a2.v},
    {0, 0, 0, b3.h, b3.v, 1, -b3.h*a3.v, -b3.v*a3.v},
  };
  */
  
  float b[8]; 
  b[0] = a0.h;
  b[1] = a1.h;
  b[2] = a2.h;
  b[3] = a3.h;
  b[4] = a0.v;
  b[5] = a1.v;
  b[6] = a2.v;
  b[7] = a3.v;
                              

  float matrix[8][8];
  matrix[0][0] = b0.h;     matrix[0][1] = b0.v;     matrix[0][2] = (float)1; matrix[0][3] = (float)0; matrix[0][4] = (float)0; matrix[0][5] = (float)0; matrix[0][6] = -b0.h*a0.h; matrix[0][7] = -b0.v*a0.h;
  matrix[1][0] = b1.h;     matrix[1][1] = b1.v;     matrix[1][2] = (float)1; matrix[1][3] = (float)0; matrix[1][4] = (float)0; matrix[1][5] = (float)0; matrix[1][6] = -b1.h*a1.h; matrix[1][7] = -b1.v*a1.h;
  matrix[2][0] = b2.h;     matrix[2][1] = b2.v;     matrix[2][2] = (float)1; matrix[2][3] = (float)0; matrix[2][4] = (float)0; matrix[2][5] = (float)0; matrix[2][6] = -b2.h*a2.h; matrix[2][7] = -b2.v*a2.h;
  matrix[3][0] = b3.h;     matrix[3][1] = b3.v;     matrix[3][2] = (float)1; matrix[3][3] = (float)0; matrix[3][4] = (float)0; matrix[3][5] = (float)0; matrix[3][6] = -b3.h*a3.h; matrix[3][7] = -b3.v*a3.h;
  matrix[4][0] = (float)0; matrix[4][1] = (float)0; matrix[4][2] = (float)0; matrix[4][3] = b0.h;     matrix[4][4] = b0.v;     matrix[4][5] = (float)1; matrix[4][6] = -b0.h*a0.v; matrix[4][7] = -b0.v*a0.v;
  matrix[5][0] = (float)0; matrix[5][1] = (float)0; matrix[5][2] = (float)0; matrix[5][3] = b1.h;     matrix[5][4] = b1.v;     matrix[5][5] = (float)1; matrix[5][6] = -b1.h*a1.v; matrix[5][7] = -b1.v*a1.v;
  matrix[6][0] = (float)0; matrix[6][1] = (float)0; matrix[6][2] = (float)0; matrix[6][3] = b2.h;     matrix[6][4] = b2.v;     matrix[6][5] = (float)1; matrix[6][6] = -b2.h*a2.v; matrix[6][7] = -b2.v*a2.v;
  matrix[7][0] = (float)0; matrix[7][1] = (float)0; matrix[7][2] = (float)0; matrix[7][3] = b3.h;     matrix[7][4] = b3.v;     matrix[7][5] = (float)1; matrix[7][6] = -b3.h*a3.v; matrix[7][7] = -b3.v*a3.v;

  
  
  // the algorithm used to solve the system was taken from NUMERICAL RECIPES IN C page
  
  
  // first step LU decomposition of the matrix
   #define TINY (float)1.0e-20
   float db;
   int n= 8;    
   int indx[8];
   float* d = &db;
   int   i,imax,j,k;
   float big,dum,sum,temp;
   int   ii=-1,ip;
   
   float vv[8];
   *d = (float)1.0;
   
   for (i=0;i<=n-1;i++) {
   
      big = (float)0.0;
      for (j=0;j<=n-1;j++) 
         if ((temp=ABS(float(matrix[i][j]))) > big) big = temp;
      if (big == 0.0) exit(0);
      
      vv[i] = (float)1.0 / big;
   }
   
   for (j=0;j<=n-1;j++) {
      
      for (i=0;i<j;i++) {
         sum = matrix[i][j];
         for (k=0;k<i;k++) {
            sum -= matrix[i][k]*matrix[k][j];
         }   
         matrix[i][j] = sum;
      }
      big = (float)0.0;
      for (i=j;i<=n-1;i++) {
      
         sum = matrix[i][j];
         for (k=0;k<j;k++) {
            sum -= matrix[i][k]*matrix[k][j];
         }
         matrix[i][j] = sum;
         if ( (dum=vv[i]*ABS(float(sum))) >= big) {            
            big = dum;
            imax = i;
         }   
      }
      if (j != imax) {
      
         for (k=0;k<=n-1;k++) {
         
            dum = matrix[imax][k];
            matrix[imax][k] = matrix[j][k];
            matrix[j][k] = dum;   
         }
         *d = -(*d);
         vv[imax] = vv[j];
      }
      indx[j] = imax;

      if (matrix[j][j] == (float)0) matrix[j][j] = TINY;
      if (j != n-1) {
         
         dum = (float)1.0/(matrix[j][j]);
         for (i = j+1; i <= n-1;i++) {
            matrix[i][j] *= dum;
         }   
      }
   }
   
   
   ///////////////////////////////////////////////////////////////
   // second step : back-substitution
   
   for (i=0;i <= n-1; i++) {
      ip = indx[i];
      sum = b[ip];
      b[ip] = b[i];
      if (ii != -1) 
         for (j = ii;j<= i-1;j++) {
            sum -= matrix[i][j]*b[j];
         }   
      else if (sum) ii = i;
      b[i] = sum;   
   }
   
   for (i = n-1;i >= 0; i--) {   
      sum = b[i];
      for (j = i+1;j<=n-1;j++) {
         sum -= matrix[i][j]*b[j];
      }   
      b[i] = sum/matrix[i][i];
   }
        

  // Set the corresponding transfo
  TransfoPerspective trans (b[0], b[3], b[1], b[4], b[2], b[5], b[6], b[7]);
    
  // Combine with the original transfo
  *this *= trans;
}




//
// Get the translation, rotation, skew, scale and perspective which compose this matrix
// (angles are given in radian between [-PI, PI])
//
//  ALGO :
//
//  TransfoPerspective = translation * (rotation * (skew * (scale * perspective)), ie
//
//  ( a   c   x0 )   ( 1  0  tH )   ( cos(rot) -sin(rot)  0 )   ( 1  -sin(skew)  0 )   ( sH  0  1 )   ( 1  0  0 )
//  ( b   d   y0 ) = ( 0  1  tV ) * ( sin(rot)  cos(rot)  0 ) * ( 0   cos(skew)  0 ) * ( 0  sV  1 ) * ( 0  1  0 )
//  ( px  py  1  )   ( 0  0  1  )   (    0        0       1 )   ( 0      0       1 )   ( 0   0  1 )   ( Px Py 1 )
//
//    ( a   c   x0 )   ( sH * cos(rot) + tH * Px   -sV * sin(rot+skew) + tH * Py    tH )
//  ie  ( b   d   y0 ) = ( sH * sin(rot) + tV * Px    sV * cos(rot+skew) + tV * Py    tV )
//    ( px  py  1  )   (          Px                          Py            1  )
//
//

void TransfoPerspective::GetComponents(float* rotationAngle, float* skewAngle, float* scaleH,
                  float* scaleV, float* translationH, float* translationV,
                  float* perspectiveX, float* perspectiveY) const
{
  const double HALF_PI = PI / 2.0;
  double     rot, rot_plus_skew;
  double     a1, b1, c1, d1;

  // Get translation
  if (translationH != NULL)
    *translationH = x0;
  if (translationV != NULL)
    *translationV = y0;
  
  // Get perspective control values
  if (perspectiveX != NULL)
    *perspectiveX = px;
  if (perspectiveY != NULL)
    *perspectiveY = py;
  
  // Compute affines values without perspective influence
  a1 = a - x0*px;
  b1 = b - y0*px;
  c1 = c - x0*py;
  d1 = d - y0*py;

  // Get scale
  if (scaleH != NULL)
    *scaleH = (float) sqrt( a1*a1 + b1*b1 );
  if (scaleV != NULL)
    *scaleV = (float) sqrt( c1*c1 + d1*d1 );
    
  // Get rotation
  if (!FLOAT_NUL((float)a1))
    rot = atan2(b1, a1);                  // <=  tan(rot) = b1 / a1
  else
    rot = (b1 > 0) ? HALF_PI : -HALF_PI;
  if (rotationAngle != NULL)
    *rotationAngle = (float) rot;
  
  // Get skew
  if (skewAngle != NULL)
  {
    if (!FLOAT_NUL((float)d1))
      rot_plus_skew = atan2(-c1, d1);         // <=  tan(rot+skew) = -c1 / d1
    else
      rot_plus_skew = (c1 > 0) ? -HALF_PI : HALF_PI;
    *skewAngle = (float) (rot_plus_skew - rot);
    if (*skewAngle < -PI)
      *skewAngle += (float)2.0 * (float)PI;
  }
}

void TransfoPerspective::GetPerspective(float* perspectiveX, float* perspectiveY) const

{
  *perspectiveX = px;
  *perspectiveY = py;
}

void TransfoPerspective::GetAffineMatrix(float* a, float* b, float* c, float* d, float* x0, float* y0) const

{
  *a = this->a;
  *b = this->b;
  *c = this->c;
  *d = this->d;
  if (x0 != NULL)
    *x0 = this->x0;
  if (y0 != NULL)
    *y0 = this->y0;
}

Boolean TransfoPerspective::IsAffine()

{
  return (FLOAT_NUL(px) && FLOAT_NUL(py) ? TRUE : FALSE);
}

Boolean TransfoPerspective::IsScaling()

{
  return (FLOAT_NUL(px) && FLOAT_NUL(py) && FLOAT_NUL(b) && FLOAT_NUL(c) && FLOAT_EGAL(a, d) ? TRUE : FALSE);
}

Boolean TransfoPerspective::IsNull()

{
  return (FLOAT_NUL(a) && FLOAT_NUL(b) && FLOAT_NUL(c) && FLOAT_NUL(d) ? TRUE : FALSE);
}

Boolean TransfoPerspective::IsNonRotatedRectangle()

{
  return ((FLOAT_NUL(a) || FLOAT_NUL(b)) && (FLOAT_NUL(c) || FLOAT_NUL(d)) 
      && IsAffine() ? TRUE : FALSE);
}

// Constructors of integral affine transformation matrix :
// -------------------------------------------------------

// Default is Identity :

IntegralTransfo::IntegralTransfo ()

{
  a  = 1;
  b  = 0;
  c  = 0;
  d  = 1;
  x0 = 0;
  y0 = 0;
  px = 0;
  py = 0;
}

// "=" operator

IntegralTransfo& IntegralTransfo::operator =(const TransfoPerspective& transfoPerspective)

{
  long shift = 1 << 12;

  a  = (long)(transfoPerspective.a  * shift);
  b  = (long)(transfoPerspective.b  * shift);
  c  = (long)(transfoPerspective.c  * shift);
  d  = (long)(transfoPerspective.d  * shift);
  x0 = (long)(transfoPerspective.x0 * shift);
  y0 = (long)(transfoPerspective.y0 * shift);
  px = (long)(transfoPerspective.px * shift);
  py = (long)(transfoPerspective.py * shift);
  
  return *this;
}

// Various kind of composition :
// -----------------------------

// Composition with an integral coordinates

Int12Position operator*(const IntegralTransfo& T, const IntegralPosition& point)
{
  Int12Position result;
  long norm = ((T.px*point.x + T.py*point.y)>>12) + 1;

  result.x = (T.a*point.x + T.c*point.y + T.x0) / norm;
  result.y = (T.b*point.x + T.d*point.y + T.y0) / norm;

  return result;
}

// Composition with an integral coordinates shifted by 12

Int12Position operator*(const IntegralTransfo& T, const Int12Position& point)
{
  Int12Position result;
  long norm = ((T.px*point.x + T.py*point.y)>>24) + 1;

  result.x = (((T.a*point.x)>>12) + ((T.c*point.y)>>12) + T.x0) / norm;
  result.y = (((T.b*point.x)>>12) + ((T.d*point.y)>>12) + T.y0) / norm;

  return result;
}

// Composition with real coordinates

Int12Position operator*(const IntegralTransfo& T, const PositionMv& point)
{
  Int12Position result, interm;

  interm = point;
  result = T*interm;
  
  return result;
}


void Bezier::Init(float p0x, float  p0y, float p1x, float p1y, float p2x, float p2y, float p3x, float p3y)

{
  this->p0x = p0x;
  this->p0y = p0y;
  this->p1x = p1x;
  this->p1y = p1y;
  this->p2x = p2x;
  this->p2y = p2y;
  this->p3x = p3x;
  this->p3y = p3y;
  this->d = FALSE;
}


void Bezier::DetruitSuiteBezier(ptr_Bezier laBezier)

{
  ptr_Bezier bezier, bezier1;
  if (laBezier) {
    for (bezier = laBezier->suivant; bezier;) {
      bezier1 = bezier;
      bezier = bezier->suivant;
      delete bezier1;
    }
    delete laBezier;
  }
}

ptr_Bezier Bezier::ReverseBezier(ptr_Bezier theBezier) {
  ptr_Bezier result = 0;
  ptr_Bezier last_endpt = 0;
  for (ptr_Bezier ptr = theBezier; ptr; ptr = ptr->suivant) {
    ptr_Bezier rev_copy = new Bezier;
    if (rev_copy==NULL) {
      return (ptr_Bezier)NULL;
    }
    rev_copy->p0x = ptr->p3x;
    rev_copy->p0y = ptr->p3y;
    rev_copy->p1x = ptr->p2x;
    rev_copy->p1y = ptr->p2y;
    rev_copy->p2x = ptr->p1x;
    rev_copy->p2y = ptr->p1y;
    rev_copy->p3x = ptr->p0x;
    rev_copy->p3y = ptr->p0y;
    rev_copy->d = 0;
    if (!last_endpt) last_endpt = rev_copy;
    if (ptr->d) {
      if (last_endpt) {
        last_endpt->d = 1;
      }
      last_endpt = NULL;
    }
    rev_copy->suivant = result;
    result = rev_copy;
  }
  Bezier::DetruitSuiteBezier(theBezier);
  return result;
}

//  ------------------------------------------------------------------------------------------------
//  Fonctions externes
//  ------------------------------------------------------------------------------------------------

#ifdef __MWERKS__   // If CW 68000 (or more)
#ifdef MC68000      //

///////////////////////////////////////////
// This function computes x * 655336 / y //
///////////////////////////////////////////
asm long Div64(long x, long y)

{
    dc.w 0x202F   // move.l 4(SP),D0    ; move x to D0
    dc.w 0x0004
    dc.w 0x4AAF   // tst.l  8(SP)     ; test y
    dc.w 0x0008
    dc.w 0x6712   // beq.s  Overflow    ; if y is not 0
    dc.w 0x7200   // clr.l  D1        ; multiply x by 0x10000 in 64 bits
    dc.w 0x4C3C   // muls.l #$100000,D1:D0
    dc.w 0x0C01
    dc.w 0x0010
    dc.w 0x0000
    dc.w 0x4C6F   // divs.l 8(SP),D1:D0   ; divide the result by y in 64 bits
    dc.w 0x0C01
    dc.w 0x0008
    dc.w 0x4E75   // rts            ; return the result in D0

// Overflow:                    ; if y is 0
    dc.w 0x4A80   // tst.l  D0
    dc.w 0x6A08   // bpl.s  Positive    ; if x is negative
    dc.w 0x203C   // move.l #$80000000,D0 ; return the minimum value possible
    dc.w 0x8000
    dc.w 0x0000
    dc.w 0x4E75   // rts
  
// Positive:
    dc.w 0x203C   // move.l #$7FFFFFFF,D0 ; if y is positive return the maximum value possible
    dc.w 0x7FFF
    dc.w 0xFFFF
    dc.w 0x4E75   // rts
}

#endif // MC68000
#endif // __MWERKS__

