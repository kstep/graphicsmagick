//  ------------------------------------------------------------------------------------------------
//  MODULE    : Geometrie
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : Mercredi 16 Septembre 1992
//  DESCRIPTION :
//  COMMENTAIRE :
//  SCCSID      : @(#)geometry.h  1.1 11:50:02 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef Geometrie_h
  #define Geometrie_h
  #ifndef Commun_h
    #include  "common.h"
  #endif // Commun_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------
  
//  Constantes
//  ----------

  enum typ_Clipping { clip_Normal, clip_Clippe, clip_Complemente };
  enum typ_Operation { op_Soustraction=0, op_Addition=1, op_Alpha_Composite=2};
  enum TypePositionnement {T = 1, Sh, Sv, S, TR, TH, TRA, TAn, Gen, FourPoints};

//  Declarations des classes
//  ------------------------

  struct TransfoPerspective;
  typedef TransfoPerspective* ptr_TransfoPerspective;
  typedef TransfoPerspective& ref_TransfoPerspective;
  
  class IntegralTransfo;
  typedef IntegralTransfo* ptr_IntegralTransfo;
  typedef IntegralTransfo& ref_IntegralTransfo;

  struct IntegralPosition;
  typedef IntegralPosition* ptr_IntegralPosition;
  typedef IntegralPosition& ref_IntegralPosition;
  
  struct Int12Position;
  typedef Int12Position* ptr_Int12Position;
  typedef Int12Position& ref_Int12Position;


  struct Bezier;
  typedef Bezier* ptr_Bezier;
  typedef Bezier& ref_Bezier;
  
  // pour ne pas inclure FichierLP.h
  class FichierLP;
  typedef FichierLP* ptr_FichierLP;
  typedef FichierLP& ref_FichierLP;

//  Definitions des classes
//  -----------------------

//
// In 3D homogeneous coordinates, transformation of a plane into a 3D space is expressed as :
//
//                  a c e x0
//                  b d f y0    (see [FOL] p.216 - Eq 5.45)
//                  g h i z0
//                  0 0 0 1
//
// This general transformation can be simplified in our case taking into account the following model :
//
//    - we will use a perspective projection of center 0 on a z=d plane (see [FOL] p.254 - Fig 6.42)
//
//    - we can fix arbitrary d=1 so the projection matrix will be
//                  1 0 0 0
//                  0 1 0 0   (see [FOL] p.254 - Eq 6.3)
//                  0 0 1 0
//                  0 0 1 0
//    so a (x,y,z,1) point becomes (x,y,z,z) which is equivalent in homogeneous coordinates to
//    (x/z, y/z, 1, 1)
//
//    - because we use only flat 2D shapes and project onto z=1, we can consider every start shape
//    as belonging to this plane, so the homogeneous coordinates we use are reduced to the form
//    (x,y,1,1)
//
//    - using this reduction, it's easy to see that all z transforms can be turned into translation terms
//    so the general matrix becomes :
//                  a c 0 x0
//                  b d 0 y0
//                  g h 1 z0
//                  0 0 0 1
//
//    - using only points of the form (x,y,1,1), this matrix can be turned into :
//                  a c x0  0
//                  b d y0  0
//                  g h z0  0
//                  0 0 0 1
//
//    - we see the 4th dimension is useless in our case so a simple 3x3 matrix is enough
//
//    - because we use only this transformation to simulate a perspective view of a plane, we
//    don't need any z0 term and can fix it to 1, leaving the projection plane unchanged.
//    The final transformation matrix before projection is then :
//                  a c x0
//                  b d y0
//                  px  py  1
//
//  px and py are called the perspective coefficients. If they are equal to 0, there is no perspective and
//  the matrix is similar to the classical affine matrix transformation in 2D homogeneous coordinates.
//
// References :
//    [FOL] : Foley, Van Dam : Computer Graphics, second edition, fifth print
//

//
//  CAUTION :   TransfoPerspective are not affine transformations…
//        There are affine if and only if px = py = 0
//        See usage of IsAffine() and IsScaling()
//
// Important properties :
// ----------------------
//
//    - When multiplying a vector by a matrix, we always project onto z=1 after multiplication, dividing the
//    resulting coordinates by (px*x + py*y + 1).
//
//    - Consequence : the multiplication of a vector (getting rid of the translation factor) by a matrix is no more linear
//
//    - To maintain a "1" bottom right value, we always normalize matrices after multiplication
//
//    - The multiplication of matrices and vectors is associative : the combinaison still works…
//                P1*[P2(X)] = [P1*P2](X)
//      This is still true with normalized matrices multiplication
//


  struct TransfoPerspective {
  
    friend class IntegralTransfo;
        
                TransfoPerspective ();
                TransfoPerspective (const TransfoPerspective& Ta);
                TransfoPerspective (float a, float b, float c, float d, float x0 = 0, float y0 = 0, float px = 0, float py = 0);

        // Composition operators
        TransfoPerspective& operator =(const TransfoPerspective& transfoPerspective);
  friend    TransfoPerspective  operator *(const TransfoPerspective& T1, const TransfoPerspective& T2);
        TransfoPerspective& operator *=(const TransfoPerspective& T);
  friend    PositionMv    operator *(const TransfoPerspective& T, const PositionMv& point);
  
  // used only in obj_MasqueClippeKi::LocalUnderCrop
  friend    PositionMv    MultipMatrixVector(const TransfoPerspective& T, const PositionMv& point);
  
  
  friend    VecteurMv   operator *(const TransfoPerspective& T, const VecteurMv& vector);
        TransfoPerspective  Produit (const TransfoPerspective& Ta) const;
        
        // Inverse transformation
        void      Inverse();

        // Old affine transformation methods
        void      ForcerPosition (TypePositionnement type, VecteurMv& v1, VecteurMv& v2, VecteurMv& v3);
        void      ForcerPosition (TypePositionnement type, VecteurMv& v1, VecteurMv& v2, VecteurMv& v3, VecteurMv& v4);
        void      ForcerPosition (PositionMv& pA1, PositionMv& pB1, PositionMv& pA2, PositionMv& pB2, PositionMv& pA3, PositionMv& pB3);

        // Standard primitive transformations
        void      TransApplyTrans (const PositionMv& center, const TransfoPerspective& transfo);
        void      Translate (const VecteurMv& vector);
        void      Rotate (const PositionMv& center, float theta);
        void      Scale (const PositionMv& center, float sx, float sy);
          void      ShearHorizontal (const PositionMv& center, float sh);
          void      ShearVertical (const PositionMv& center, float sh);
        void      FlipHorizontal (const PositionMv& center);
        void      FlipVertical (const PositionMv& center);

        void      Translate (float dx, float dy);
        void      Rotate (float x0, float y0, float theta);
        void      Scale (float x0, float y0, float sx, float sy);
        void      ShearHorizontal (float x0, float y0, float sh);
        void      ShearVertical (float x0, float y0, float sh);
        void      FlipHorizontal (float x0, float y0);
        void      FlipVertical (float x0, float y0);
        
        // Perspective transformation
        void      Set4Points (const PositionMv& a0, const PositionMv& a1, const PositionMv& a2, const PositionMv& a3,
                      const PositionMv& b0, const PositionMv& b1, const PositionMv& b2, const PositionMv& b3);

        // Get Information
        Boolean     IsAffine();
        Boolean     IsScaling();
        Boolean     IsNull();
        Boolean     IsNonRotatedRectangle();
        
        // Get the translation, rotation, skew and scale which compose this matrix
        // (angles are given in radian between [-PI, PI])
        void      GetComponents(float* rotationAngle, float* skewAngle, float* scaleH,
                        float* scaleV, float* translationH, float* translationV,
                        float* perspectiveX = NULL, float* perspectiveY = NULL) const;
                      
        // Get the different matix components
        // Use carefully: Implemented to speed up renderings
        void      GetAffineMatrix(float* a, float* b, float* c, float* d, float* x0 = NULL, float* y0 = NULL) const;
        void      GetTranslation(float* translationH, float* translationV) const;
        void      GetPerspective(float* perspectiveX, float* perspectiveY) const;
                      
//
// In homogeneous coordinates, transformation is expressed as a 3x3 matrix :
//
//                  a c x0
//                  b d y0
//                  px  py  1
//
// See explanations above
//
    private:
        float     a;
        float     b;
        float     c;
        float     d;
        float     x0;
        float     y0;
        float     px;
        float     py;
  };


  // INTEGRAL COMPUTATION : TO BE USE FOR FAST ACCESS
  // In integral computing, dimensions are long used with 12 decimal bits instead of float
  
  // Integral transformation matrix used for fast computing : same thing than TransfoPerspective but integral "<<12"

  class IntegralTransfo {
  
  public:
                IntegralTransfo ();

      IntegralTransfo&  operator =(const TransfoPerspective& transfoPerspective);
  friend    Int12Position operator *(const IntegralTransfo& T, const PositionMv& point);
  friend    Int12Position operator *(const IntegralTransfo& T, const IntegralPosition& point);
  friend    Int12Position operator *(const IntegralTransfo& T, const Int12Position& point);
        
  protected:
  private:
        long      a;
        long      b;
        long      c;
        long      d;
        long      x0;
        long      y0;
        long      px;
        long      py;
  
  };

  // Integral point coordinates : typically used for screen or output page coordinates
  
  struct IntegralPosition {
                IntegralPosition ();
                IntegralPosition (long x, long y);

      // Caution : converting an Int12Position into an IntegralPosition will loose precision
        IntegralPosition& operator =(const Int12Position& point);
friend  inline  IntegralPosition  operator +(const IntegralPosition& p1, const IntegralPosition& p2);
friend  inline  IntegralPosition  operator -(const IntegralPosition& p1, const IntegralPosition& p2);
        IntegralPosition& operator +=(const IntegralPosition& p);
      
        long      x;
        long      y;
  };

  // Integral point coordinates with 12 decimal bits : same thing than IntegralPosition but "<<12"
  
  struct Int12Position {
                Int12Position ();

      // Caution : converting an IntegralPosition into an Int12Position will not recover precision
      Int12Position&  operator =(const IntegralPosition& point);
      Int12Position&  operator =(const PositionMv& point);
friend  inline  Int12Position operator +(const Int12Position& p1, const Int12Position& p2);
friend  inline  Int12Position operator -(const Int12Position& p1, const Int12Position& p2);
      Int12Position&  operator +=(const Int12Position& p);
      
        long      x;
        long      y;
  };

  // BEZIER :
  
  struct Bezier {
      
                Bezier() {suivant = NULL;}
      
        void      Init(float p0x, float  p0y, float p1x, float p1y, float p2x, float p2y, float p3x, float p3y);
    static  void      DetruitSuiteBezier(ptr_Bezier laBezier);
    static  ptr_Bezier    ReverseBezier(ptr_Bezier theBezier);
    
        
        float     p0x;                // The 4 points p0 to p3
        float     p0y;
        float     p1x;
        float     p1y;
        float     p2x;
        float     p2y;
        float     p3x;
        float     p3y;
        
        Boolean     d;                  // True if last point
        
        ptr_Bezier    suivant;              // Next Bezier segment
  };


//  Fonctions 'inline'
//  ------------------

inline IntegralPosition::IntegralPosition() { x = 0; y = 0; }

inline IntegralPosition::IntegralPosition (long x, long y) { this->x = x; this->y = y; }

inline Int12Position::Int12Position() { x = 0; y = 0; }

inline IntegralPosition& IntegralPosition::operator =(const Int12Position& point)
{
  x = point.x >> 12;
  y = point.y >> 12;
  return *this;
}

inline Int12Position& Int12Position::operator =(const IntegralPosition& point)
{
  x = point.x << 12;
  y = point.y << 12;
  return *this;
}

inline Int12Position& Int12Position::operator =(const PositionMv& point)
{
  x = (long)(point.h * 4096.0);
  y = (long)(point.v * 4096.0);
  return *this;
}

inline IntegralPosition operator +(const IntegralPosition& p1, const IntegralPosition& p2)
{ IntegralPosition p; p.x = p1.x + p2.x; p.y = p1.y + p2.y; return p; }

inline IntegralPosition operator -(const IntegralPosition& p1, const IntegralPosition& p2)
{ IntegralPosition p; p.x = p1.x - p2.x; p.y = p1.y - p2.y; return p; }

inline IntegralPosition& IntegralPosition::operator +=(const IntegralPosition& p)
{ x += p.x; y += p.y; return *this; }

inline Int12Position operator +(const Int12Position& p1, const Int12Position& p2)
{ Int12Position p; p.x = p1.x + p2.x; p.y = p1.y + p2.y; return p; }

inline Int12Position operator -(const Int12Position& p1, const Int12Position& p2)
{ Int12Position p; p.x = p1.x - p2.x; p.y = p1.y - p2.y; return p; }

inline Int12Position& Int12Position::operator +=(const Int12Position& p)
{ x += p.x; y += p.y; return *this; }

inline void TransfoPerspective::GetTranslation(float* translationH, float* translationV) const

{
  *translationH = x0, *translationV = y0;
}

//  Fonctions 'extern'
//  ------------------

// These functions are to accelerate computation of perspective matrix for 68K

#undef MC68000
#ifdef macintosh
  #ifndef powerc
    #define MC68000
  #endif
#endif

#ifndef MC68000

  typedef float Ratio;
  
  #define MakeRatio(x)  (x)
  #define FastDiv(x,y)  ((x)/(y))
  
#else

  #ifdef applec
    extern "C" long Div64(long x, long y);
  #else
    extern long Div64(long x, long y);
  #endif

  typedef long Ratio;
  
  #define MakeRatio(x)  ((x) * 0x100000)
  #define FastDiv(x,y)  Div64((x),(y))
  
#endif

//  Variables 'extern'
//  ------------------
  extern long tableProf[];
  
//  ------------------------------------------------------------------------------------------------
  #endif // Geometrie_h
//  ------------------------------------------------------------------------------------------------
