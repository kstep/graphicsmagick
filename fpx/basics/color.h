//  ------------------------------------------------------------------------------------------------
//  MODULE    : Couleur
//  LANGUAGE  : C++
//  AUTHOR    : Adolfo VIDE
//  DATE    : 09/15/92
//  DESCRIPTION : Color classes
//  COMMENT   :
//  SCCSID      : @(#)color.h 1.1 11:49:57 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef Couleur_h
  #define Couleur_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include "b_types.h"
  #include "mac_comp.h"

  #include "npixel.h"

  // to avoid inclusion of ColorMatching.h
  class obj_ColorMatching;

//  Constants
//  ---------

  enum SpotType { Spot_None, Spot_Custom, Spot_PantoneProcess, Spot_PantoneInk, Spot_ToyoInk };
  
//  Classes declaration
//  -------------------

  struct Couleur;
  typedef Couleur* ptr_Couleur;
  typedef Couleur& ref_Couleur;
  
  struct CouleurTransparente;
  typedef CouleurTransparente* ptr_CouleurTransparente;
  typedef CouleurTransparente& ref_CouleurTransparente;
  
  struct CouleurCMYKtransparente;
  typedef CouleurCMYKtransparente* ptr_CouleurCMYKtransparente;
  typedef CouleurCMYKtransparente& ref_CouleurCMYKtransparente;
  
  typedef short ComposanteC;
  typedef float GCR;
  typedef unsigned16 Canal;
    
  struct CouleurRGB;
  typedef CouleurRGB* ptr_CouleurRGB;
  typedef CouleurRGB& ref_CouleurRGB;
  
  struct CouleurCMYK;
  typedef CouleurCMYK* ptr_CouleurCMYK;
  typedef CouleurCMYK& ref_CouleurCMYK;

  typedef NPixel PixelUG;
  typedef PixelUG* ptr_PixelUG;
  typedef PixelUG& ref_PixelUG;

  #define COL_INVALID -32768
  
//  Classes definition
//  ------------------

  struct Couleur {
  
                Couleur();
                Couleur(const Couleur& col);
                Couleur(const Pixel& pixel);
                Couleur(const NPixel& pixel);
                Couleur(ComposanteC r, ComposanteC g, ComposanteC b);
                Couleur(ComposanteC y, ComposanteC m, ComposanteC c, ComposanteC k);
    
                operator Pixel() const;
                operator NPixel() const;
    
        ComposanteC   rouge;
        ComposanteC   vert;
        ComposanteC   bleu;
        
        ComposanteC   cyan;
        ComposanteC   magenta;
        ComposanteC   jaune;
        ComposanteC   noir;
  
  private:
  
    static  GCR       gcrCourant;
  };
  
  // CAUTION: PARTS OF THE CODE MAKE AN ASSUMPTION ON THE SIZE OF THIS CLASS
  struct CouleurTransparente {
        short presence; // normalise a 0x1000
        short rouge;    // normalise a 0x1000
        short vert;   // normalise a 0x1000
        short bleu;   // normalise a 0x1000
  };
  
  
  // CAUTION: PARTS OF THE CODE MAKE AN ASSUMPTION ON THE SIZE OF THIS CLASS
  struct CouleurCMYKtransparente {
                CouleurCMYKtransparente();
                CouleurCMYKtransparente (short c, short m, short y, short k, short presence);

        void      Clear ();
        void      operator*=(const CouleurCMYKtransparente& a);


        short   presence; // normalise a 0x1000
        short   c;
        short   m;
        short   y;
        short   k;
  };
  
  // CAUTION: PARTS OF THE CODE MAKE AN ASSUMPTION ON THE SIZE OF THIS CLASS
  struct CouleurRGB {
  
                CouleurRGB();
                CouleurRGB(Coef c);
                CouleurRGB(ComposanteC r, ComposanteC g, ComposanteC b);
                CouleurRGB(const Couleur& col);
    
        Coef      rouge;
        Coef      vert;
        Coef      bleu;
        
        Boolean     operator==(const CouleurRGB& a) const;
        Boolean     operator!=(const CouleurRGB& a) const;
        CouleurRGB    operator+(const CouleurRGB& a) const;
        void      operator+=(const CouleurRGB& a);
        CouleurRGB    operator-(const CouleurRGB& a) const;
        void      operator-=(const CouleurRGB& a);
        CouleurRGB    operator*(short x) const;
        void      operator*=(short x);
        CouleurRGB    operator/(short x) const;
        void      operator/=(short x);
        
        void      Average (const CouleurRGB&  a, const CouleurRGB&  b);
        void      Average (const CouleurRGB&  a, const CouleurRGB&  b,
                     const CouleurRGB&  c, const CouleurRGB&  d);
  };
  
  // CAUTION: PARTS OF THE CODE MAKE AN ASSUMPTION ON THE SIZE OF THIS CLASS
  struct CouleurCMYK {
  
                CouleurCMYK();
                CouleurCMYK(Canal c, Canal m, Canal y, Canal k);
                CouleurCMYK(const Couleur& col);
    
        Boolean     operator ==(const CouleurCMYK& cmyk) const;
        Boolean     operator !=(const CouleurCMYK& cmyk) const;
    
        Canal     c;
        Canal     m;
        Canal     y;
        Canal     k;
  };



//  Inline methods
//  --------------

  inline Couleur::Couleur()
    : rouge(0),
      vert(0),
      bleu(0),
      cyan(COL_INVALID),
      magenta(COL_INVALID),
      jaune(COL_INVALID),
      noir(COL_INVALID)
  
  {
  }

  inline Couleur::Couleur(const Couleur& col)  
  {
    BlockMove (&col, this, sizeof *this);
  }

  inline Couleur::Couleur(const Pixel& pixel)
    : cyan(COL_INVALID),
      magenta(COL_INVALID),
      jaune(COL_INVALID),
      noir(COL_INVALID)
  {
    rouge = pixel.rouge;
    vert  = pixel.vert;
    bleu  = pixel.bleu;
  }
  
  inline Couleur::Couleur(const NPixel& pixel)
    : cyan(COL_INVALID),
      magenta(COL_INVALID),
      jaune(COL_INVALID),
      noir(COL_INVALID)
  {
    rouge = pixel.Red();
    vert  = pixel.Green();
    bleu  = pixel.Blue();
  }
  
  inline Couleur::operator Pixel() const  
  {
    Pixel pixel;
    
    pixel.rouge = (unsigned char)rouge;
    pixel.vert  = (unsigned char)vert;
    pixel.bleu  = (unsigned char)bleu;
    pixel.alpha = (unsigned char)0;
    
    return pixel;
  }
  
  inline Couleur::operator NPixel() const
  {
    NPixel  pixel;
    
    pixel.Red()   = (unsigned char)rouge;
    pixel.Green() = (unsigned char)vert;
    pixel.Blue()  = (unsigned char)bleu;
    pixel.Alpha() = (unsigned char)0;
    
    return pixel;
  }
  

  inline CouleurCMYKtransparente::CouleurCMYKtransparente ()
    : presence(0x1000), c(0), m(0), y(0), k(0)
  {
  }

  inline CouleurCMYKtransparente::CouleurCMYKtransparente (short c, short m, short y, short k, short presence)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->k = k;
    this->presence = presence;
  }

  inline void CouleurCMYKtransparente::Clear ()
  {
    c = 0;
    m = 0;
    y = 0;
    k = 0;
    presence = 0x1000;
  }

  inline void CouleurCMYKtransparente::operator*=(const CouleurCMYKtransparente& a)
  {
    long  cmp = 0x1000 - a.presence;

    c = (short)((a.presence * c + cmp * a.c) >> 12);
    m = (short)((a.presence * m + cmp * a.m) >> 12);
    y = (short)((a.presence * y + cmp * a.y) >> 12);
    k = (short)((a.presence * k + cmp * a.k) >> 12);

    presence = (presence * a.presence) >> 12;
  }

  inline CouleurRGB::CouleurRGB(Coef c) : rouge(c), vert(c), bleu(c)
  
  {
  }
  
  inline CouleurRGB::CouleurRGB(const Couleur& col)
  
  {
    rouge = col.rouge;
    vert  = col.vert;
    bleu  = col.bleu;
  }
  
  inline CouleurRGB::CouleurRGB(Coef r, Coef g, Coef b) : rouge(r), vert(g), bleu(b)
  
  {
  }

  inline Boolean CouleurRGB::operator==(const CouleurRGB& a) const
  
  {
    return ( (a.rouge == rouge) && (a.vert == vert) && (a.bleu == bleu) );
  }
  
  inline Boolean CouleurRGB::operator!=(const CouleurRGB& a) const
  
  {
    return ! ((*this) == a);
  }
  
  inline CouleurRGB CouleurRGB::operator+(const CouleurRGB& a) const
  
  {
    CouleurRGB b;
    
    b.rouge = rouge + a.rouge;
    b.vert  = vert  + a.vert;
    b.bleu  = bleu  + a.bleu;
    
    return b;
  }
  
  inline void CouleurRGB::operator+=(const CouleurRGB& a)
  
  {
    rouge += a.rouge;
    vert  += a.vert;
    bleu  += a.bleu;
  }
  
  inline CouleurRGB CouleurRGB::operator-(const CouleurRGB& a) const
  
  {
    CouleurRGB b;
    
    b.rouge = rouge - a.rouge;
    b.vert  = vert  - a.vert;
    b.bleu  = bleu  - a.bleu;
    
    return b;
  }
  
  inline void CouleurRGB::operator-=(const CouleurRGB& a)
  
  {
    rouge = rouge - a.rouge;
    vert  = vert  - a.vert;
    bleu  = bleu  - a.bleu;
  }
  
  inline CouleurRGB CouleurRGB::operator*(short x) const
  
  {
    CouleurRGB b;
    
    b.rouge = (ComposanteC)(((long)rouge * x) >> 12);
    b.vert  = (ComposanteC)(((long)vert  * x) >> 12);
    b.bleu  = (ComposanteC)(((long)bleu  * x) >> 12);
    
    return b;
  }
  
  inline void CouleurRGB::operator*=(short x)
  
  { 
    rouge = (ComposanteC)(((long)rouge * x) >> 12);
    vert  = (ComposanteC)(((long)vert  * x) >> 12);
    bleu  = (ComposanteC)(((long)bleu  * x) >> 12);
  }
  
  inline CouleurRGB CouleurRGB::operator/(short x) const
  
  {
    CouleurRGB b;
    
    b.rouge = (ComposanteC)(((long)rouge << 12)/ x);
    b.vert  = (ComposanteC)(((long)vert  << 12)/ x);
    b.bleu  = (ComposanteC)(((long)bleu  << 12)/ x);
    
    return b;
  }
  
  inline void CouleurRGB::operator/=(short x)
  
  {
    rouge = (ComposanteC)(((long)rouge << 12)/ x);
    vert  = (ComposanteC)(((long)vert  << 12)/ x);
    bleu  = (ComposanteC)(((long)bleu  << 12)/ x);
  }
  
  inline void CouleurRGB::Average(const CouleurRGB&  a, const CouleurRGB&  b)
  
  {
    rouge = (a.rouge + b.rouge) >> 1;
    vert  = (a.vert  + b.vert ) >> 1;
    bleu  = (a.bleu  + b.bleu ) >> 1;
  }

  inline void CouleurRGB::Average(const CouleurRGB&  a, const CouleurRGB&  b,
                  const CouleurRGB&  c, const CouleurRGB&  d)
  
  {
    rouge = (a.rouge + b.rouge + c.rouge + d.rouge) >> 2;
    vert  = (a.vert  + b.vert  + c.vert  + d.vert ) >> 2;
    bleu  = (a.bleu  + b.bleu  + c.bleu  + d.bleu ) >> 2;
  }

  inline CouleurCMYK::CouleurCMYK ()
  {
      c = m = y = k = 0;
  }

  inline CouleurCMYK::CouleurCMYK (Canal c, Canal m, Canal y, Canal k)
  {
      this->c = c;
      this->m = m;
      this->y = y;
      this->k = k;
  }

  inline CouleurCMYK::CouleurCMYK (const Couleur& col)
  
  {
    c = col.cyan;
    m = col.magenta;
    y = col.jaune;
    k = col.noir;
  }
  
  inline Boolean CouleurCMYK::operator ==(const CouleurCMYK& cmyk) const
  {
      return (c == cmyk.c) && (m == cmyk.m) && (y == cmyk.y) && (k == cmyk.k);
  }

  inline Boolean CouleurCMYK::operator !=(const CouleurCMYK& cmyk) const
  {
      return ! (*this == cmyk);
  }


//  Exported C functions
//  --------------------

extern void Interpole16CouleurRGB ( const CouleurRGB &q0, const CouleurRGB &q1, 
                  const CouleurRGB &q2, const CouleurRGB &q3, 
                  ptr_CouleurRGB R);

extern void Interpole64CouleurRGB ( const CouleurRGB &qNW, const CouleurRGB &qNE, 
                  const CouleurRGB &qSW, const CouleurRGB &qSE, 
                  ptr_CouleurRGB R);

extern Boolean IsColorDispersion (ptr_CouleurRGB tile, long size);


//  ------------------------------------------------------------------------------------------------
  #endif // Couleur_h
//  ------------------------------------------------------------------------------------------------
