//  ------------------------------------------------------------------------------------------------
//  MODULE    : PlutonCommun
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : September 16, 1992
//  DESCRIPTION : Floating point coordinates classes
//  COMMENTS  : Mustn't include Commun.h
//  SCCSID      : @(#)common_b.h  1.1 11:49:58 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef PlutonCommun_h
  #define PlutonCommun_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include "graphic.h"

  #include <math.h>
  
//  Constants
//  ---------

//  Classes declaration
//  -------------------

  typedef short Coef;

  typedef float CoordMV;
  typedef short CoordUG;
  
  struct PointMV;
  typedef PointMV* ptr_PointMV;
  typedef PointMV& ref_PointMV;
  
  struct PositionMv;
  typedef PositionMv* ptr_PositionMv;
  typedef PositionMv* tab_PositionMv;
  typedef PositionMv& ref_PositionMv;
  
  struct VecteurMv;
  typedef VecteurMv* ptr_VecteurMv;
  typedef VecteurMv& ref_VecteurMv;
  
  class   RectangleMv;
  typedef RectangleMv* ptr_RectangleMv;
  typedef RectangleMv& ref_RectangleMv;
  
  struct PointUG;
  typedef PointUG* ptr_PointUG;
  typedef PointUG& ref_PointUG;
  
  struct RectUG;
  typedef RectUG* ptr_RectUG;
  typedef RectUG& ref_RectUG;
  
//  Classes definition
//  ------------------

  struct PointMV {
  
    CoordMV x;
    CoordMV y;
  };
  
  struct PositionMv {
  
                  PositionMv();
                  PositionMv(float x, float y);
                  PositionMv(const PositionMv& p);
                  PositionMv(const Position& p);
                
                  operator Position();
                  
        PositionMv&     operator =(const PositionMv& positionMv);

        friend  int     operator ==(const PositionMv& p1, const PositionMv& p2);
      //  friend  int     operator !=(PositionMv p1, PositionMv p2);
    
        friend  PositionMv  operator +(const PositionMv& p1, const PositionMv& p2);
        friend  PositionMv  operator -(const PositionMv& p1, const PositionMv& p2);
        friend  PositionMv  operator *(const PositionMv& p, double a);
        friend  PositionMv  operator *(double a, const PositionMv& p);
        friend  PositionMv  operator /(const PositionMv& p, double a);
        friend  PositionMv  operator -(const PositionMv& p);

        friend  double    Distance(const PositionMv& p1, const PositionMv& p2);
    
  // Pour une PositionMv consideree comme un vecteur :

                  // Produit scalaire
        friend inline double operator |(const PositionMv& p1, const PositionMv& p2);
        double        Norme() const;
                  // Rend le vecteur unitaire. Renvoie false si le vecteur est nul.
        Boolean       Normalise();
    
  // Donnees internes :
    
        float         h;
        float         v;
  };
  
  
  // Les intersections possibles entre 2 droites :
  typedef enum { VMV_UnPoint, VMV_UneDroite, VMV_Vide } IntersectionVMV;

  struct VecteurMv {
                        
                PositionMv p1;
                PositionMv p2;

                VecteurMv();
                VecteurMv(const PositionMv& p1, const PositionMv& p2);
                VecteurMv(float x1, float y1, float x2, float y2);
                
        VecteurMv&    operator =(const VecteurMv& vecteurMv);

        friend inline int operator ==(const VecteurMv& v1, const VecteurMv& v2);

                // Renvoit H, projection de p sur la droite portee par le segment
                // H appartient ou pas au segment.
                //                               -> ->      -> ->
                // *abscisse_curviligne recoit ||p1 H|| / ||p1 p2||
        PositionMv    ProjeteSurDroite(const PositionMv& p, float* abscisse_curviligne);

                // Renvoit H, projection de p sur le segment. 
                // H appartient toujours au segment.
                //                               -> ->      -> ->
                // *abscisse_curviligne recoit ||p1 H|| / ||p1 p2||
        PositionMv    ProjeteSurSegment(const PositionMv& p, float* abscisse_curviligne);

                // Calcule l'intersection avec la droite (point, vecteur)
        IntersectionVMV IntersecteDroite(const PositionMv& pointDroite, 
                         const PositionMv& vecteurDroite,
                         ptr_PositionMv pointTrouve);
  
  };
  
  class RectangleMv {
  
  public:
                RectangleMv();
                RectangleMv(const PositionMv& coin1, const PositionMv& coin2);
                RectangleMv(float x1, float y1, float x2, float y2);
                
        RectangleMv&  operator =(const RectangleMv& r);

    friend inline Boolean operator ==(const RectangleMv& r1, const RectangleMv& r2);
  
  // "+" means union of r1 and r2 rectangles. usage : r = r1 + r2;
    friend  RectangleMv   operator +(const RectangleMv& r1, const RectangleMv& r2);
        RectangleMv&  operator +=(const RectangleMv& r);
    
        PositionMv    coin_inferieur() const;
        PositionMv    coin_superieur() const;

        Boolean     Intersecte(const RectangleMv& r) const;
        Boolean     Contient(const PositionMv& p) const;
    
  private:
                PositionMv coin_inf;    // We keep coin_inf.x <= coin_sup.x
                PositionMv coin_sup;    //     and coin_inf.y <= coin_sup.y
  
  };
  
  
#ifdef powerc
# pragma options align=mac68k
#endif
#ifdef __powerc
# pragma options align=mac68k
#endif
  struct PointUG : public Position {
  
                PointUG();
                PointUG(const Position& pt);
                PointUG(short x, short y);
                PointUG(const PointMV& pMV);
  
      friend  inline  int operator ==(const PointUG& p1, const PointUG& p2);
      friend  inline  int operator !=(const PointUG& p1, const PointUG& p2);
  };
  
  struct RectUG : public PRectangle {
  
                RectUG();
                RectUG(const PRectangle& rect);
                RectUG(const PointUG& p1, const PointUG& p2);
                RectUG(short x0, short y0, short x1, short y1);
  };
#ifdef powerc
# pragma options align=reset
#endif
#ifdef __powerc
# pragma options align=reset
#endif
  
  
//  Inline methods
//  --------------

  inline PointUG::PointUG()
  
  {
  }
  
  inline PointUG::PointUG(const Position& pt) : Position(pt)
  
  {
  }
    
  inline PointUG::PointUG(short x, short y) : Position(x, y)
  
  {
  }
  
  inline PointUG::PointUG(const PointMV& pMV)
  
  {
    h = (short)pMV.x;
    v = (short)pMV.y;
  }
  
  inline int operator ==(const PointUG& p1, const PointUG& p2)
  
  {
    return  (*((int32*)&p1) == *((int32*)&p2));     // Hazard! May be not portable!
  }
  
  inline int operator !=(const PointUG& p1, const PointUG& p2)
  
  {
    return  (*((int32*)&p1) != *((int32*)&p2));     // Hazard! May be not portable!
  }
  

  inline RectUG::RectUG()
  
  {
  }
  
  inline RectUG::RectUG(const PRectangle& rect) : PRectangle(rect)
  
  {
  }
  
  inline RectUG::RectUG(short x0, short y0, short x1, short y1) : PRectangle(x0, y0, x1, y1)
  
  {
  }
  
  inline RectUG::RectUG(const PointUG& p1, const PointUG& p2) : PRectangle(p1, p2)
  
  {
  }
  
  
  inline PositionMv::PositionMv()
  {
    h = (float)0.;
    v = (float)0.;
  }
  
  inline PositionMv::PositionMv(float x, float y)
  {
    h = x;
    v = y;
  }
  
  inline PositionMv::PositionMv(const Position& p)
  {
    h = p.h;
    v = p.v;
  }
  
  // Produit scalaire
  inline double operator |(const PositionMv& p1, const PositionMv& p2)
  {
    return  p1.h*p2.h + p1.v*p2.v;
  }
  
  inline double PositionMv::Norme() const
  {
    return  sqrt(*this | *this);
  }
  
  
  inline VecteurMv::VecteurMv()
  {
    p1.h = (float)0.;
    p1.v = (float)0.;
    p2.h = (float)0.;
    p2.v = (float)0.;
  }
  
  inline VecteurMv::VecteurMv(const PositionMv& p1, const PositionMv& p2)
  {
    this->p1 = p1;
    this->p2 = p2;
  }
  
  inline VecteurMv::VecteurMv(float x1, float y1, float x2, float y2)
  {
    this->p1.h = x1;
    this->p1.v = y1;
    this->p2.h = x2;
    this->p2.v = y2;
  }
  
  inline int operator ==(const VecteurMv& v1, const VecteurMv& v2)
  {
    return ((v1.p1 == v2.p1) && (v1.p2 == v2.p2));  
  }
  
  inline VecteurMv& VecteurMv::operator =(const VecteurMv& vecteurMv)
  
  {
    this->p1 = vecteurMv.p1;
    this->p2 = vecteurMv.p2;
    
    return *this;
  }


  inline RectangleMv::RectangleMv()
  {
    coin_inf.h = (float)0.;
    coin_inf.v = (float)0.;
    coin_sup.h = (float)0.;
    coin_sup.v = (float)0.;
  }
  
  inline Boolean operator ==(const RectangleMv& r1, const RectangleMv& r2)
  
  {
    return ((r1.coin_inf == r2.coin_inf) && (r1.coin_sup == r2.coin_sup));  
  }
  
  inline RectangleMv& RectangleMv::operator =(const RectangleMv& r)
  
  {
    this->coin_inf = r.coin_inf;
    this->coin_sup = r.coin_sup;
    
    return *this;
  }
  
  inline PositionMv RectangleMv::coin_inferieur() const
  
  {
    return coin_inf;
  }
  
  inline PositionMv RectangleMv::coin_superieur() const
  
  {
    return coin_sup;
  }
  
  inline Boolean RectangleMv::Contient(const PositionMv& p) const
  
  {
    return ( (p.h >= coin_inf.h) && (p.h <= coin_sup.h)
        && (p.v >= coin_inf.v) && (p.v <= coin_sup.v) );
  }


//  Inline C functions
//  ------------------

  inline Coef CoefPondere(Coef a, Coef b)
  
  {
    return (a*b) >> 8;
  }
  
  inline Coef CoefMoyenne(Coef a, Coef b)
  
  {
    return (a+b) >> 1;
  }
  
  inline Coef CoefComplemente(Coef a)
  
  {
    return 256 - a;
  }
  
  inline Coef CoefClipping(Coef a)
  
  {
    return (a < 0) ? 0 : ((a > 255) ? 255 : a);
  }
  

//  External C functions
//  --------------------


//  ------------------------------------------------------------------------------------------------
  #endif // PlutonCommun_h
//  ------------------------------------------------------------------------------------------------
