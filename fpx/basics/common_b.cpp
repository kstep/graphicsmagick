//  ------------------------------------------------------------------------------------------------
//  MODULE    : PlutonCommun
//  LANGAGE   : C++
//  CREATEUR  : Laurent SABORET
//  DATE    : Mercredi 22 Septembre 1993
//  DESCRIPTION : 
//  COMMENTAIRE :
//  SCCSID      : @(#)common_b.cpp  1.1 11:49:58 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #include "common.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include "common_b.h"
  #include "numbers.h"

//  Constantes
//  ----------

//  Variables
//  ---------

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Fonctions internes
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methodes
//  ------------------------------------------------------------------------------------------------


//
// La classe PositionMv
//

// Rq : for an unknown reason, CW C++ 1.1 (DR4) doesn't accept this function in PlutonCommun.h
PositionMv::PositionMv(const PositionMv& p)
{
  h = p.h;
  v = p.v;
}
  
int operator ==(const PositionMv& p1, const PositionMv& p2)
{
  return (((p1.h - p2.h) < 0.0001) && ((p1.h - p2.h) > -0.0001) && ((p1.v - p2.v) < 0.0001) && ((p1.v - p2.v) > -0.0001));  
}

PositionMv operator +(const PositionMv& p1, const PositionMv& p2)
{
  PositionMv  retour;
  
  retour.h = p1.h + p2.h;
  retour.v = p1.v + p2.v;
  return retour;  
}

PositionMv operator -(const PositionMv& p1, const PositionMv& p2)
{
  PositionMv  retour;
  
  retour.h = p1.h - p2.h;
  retour.v = p1.v - p2.v;
  return retour;  
}

PositionMv operator *(const PositionMv& p, double a)
{
  PositionMv  retour;
  
  retour.h = (float) ((double) p.h * a);
  retour.v = (float) ((double) p.v * a);
  return retour;  
}

// Rq : for an unknown reason, CW C++ 1.1 (DR4) doesn't accept this function in PlutonCommun.h
PositionMv operator -(const PositionMv& p)
{
  return p * (-1);
}
  
// Rq : for an unknown reason, CW C++ 1.1 (DR4) doesn't accept this function in PlutonCommun.h
PositionMv operator *(double a, const PositionMv& p)
{
  return p * a;
}

PositionMv operator /(const PositionMv& p, double a)
{
  PositionMv  retour;
  
  retour.h = (float) ((double) p.h / a);
  retour.v = (float) ((double) p.v / a);
  return retour;  
}

// Rq : for an unknown reason, CW C++ 1.1 (DR4) doesn't accept this function in PlutonCommun.h
PositionMv::operator Position()
{
  Position p;
  p.h = (short)(h < -32000 ? -32000 : (h > 32000 ? 32000 : h));
  p.v = (short)(v < -32000 ? -32000 : (v > 32000 ? 32000 : v));
  return p;
}

// Rq : for an unknown reason, CW C++ 1.1 (DR4) doesn't accept this function in PlutonCommun.h
PositionMv& PositionMv::operator =(const PositionMv& positionMv)
{
  this->h = positionMv.h;
  this->v = positionMv.v;

  return *this;
}
  
// Rend le vecteur unitaire. Renvoie false si le vecteur est nul.
Boolean    PositionMv::Normalise()
{
  double  norme = Norme();
  
  if (norme == 0.0)
  {
    return false;
  }
  else
  {
    (*this) = (*this) / norme;
    
    if (h == 1.0)
      v = (float) 0.0;
    else if (v == 1.0)
      h = (float) 0.0;
    
    return true;
  }
}

double     Distance(const PositionMv& p1, const PositionMv& p2)
{
  PositionMv  p1p2 = p2 - p1;
  
  return  p1p2.Norme();
}


//
// La classe VecteurMv
//

// Renvoie H, projection de p sur la droite portee par le segment
// H appartient ou pas au segment.
//                               -> ->      -> ->
// *abscisse_curviligne recoit ||p1 H|| / ||p1 p2|| (dans [0,1] si H appartient au segment)
PositionMv  VecteurMv::ProjeteSurDroite(const PositionMv& p, float* abscisse_curviligne)
{
  double    norme_p1p2;         // Norme du vecteur p1p2
  PositionMv  u;              // Vecteur directeur unitaire de p1 vers p2
  PositionMv  p1p;            // Vecteur p1p
  PositionMv  H;
  double    p1p_scalaire_u;       // (p1p | u)
  
  u = p2 - p1;
  norme_p1p2 = u.Norme();
  u = u / norme_p1p2;

  p1p = p - p1; 
  p1p_scalaire_u = (p1p | u);
  
  H = p1 + p1p_scalaire_u * u;
  *abscisse_curviligne = (float) (p1p_scalaire_u / norme_p1p2);
  
  return H;
}

// Renvoie H, projection de p sur le segment. 
// H appartient toujours au segment.
//                               -> ->      -> ->
// *abscisse_curviligne recoit ||p1 H|| / ||p1 p2|| (dans [0,1])
PositionMv  VecteurMv::ProjeteSurSegment(const PositionMv& p, float* abscisse_curviligne)
{
  PositionMv  H;
  
  H = ProjeteSurDroite(p, abscisse_curviligne);
  
  // Force H a appartenir au segment
  if (*abscisse_curviligne < 0)
  {
    H = p1;
    *abscisse_curviligne = (float) 0.0;
  }
  else if (*abscisse_curviligne > 1)
  {
    H = p2;
    *abscisse_curviligne = (float) 1.0;
  }
          
  return H;
}

// Calcule l'intersection avec la droite (point, vecteur)
IntersectionVMV VecteurMv::IntersecteDroite(const PositionMv& pointDroite, 
                      const PositionMv& vecteurDroite,
                      ptr_PositionMv pointTrouve)
{
  float a1, b1, c1, a2, b2, c2, determinant;
  
  // Transforme les 2 droites sous la forme "a*x + by = c"
  a1 = p1.v - p2.v;
  b1 = p2.h - p1.h;
  c1 = p1.v*p2.h - p1.h*p2.v;
  
  a2 = vecteurDroite.v;
  b2 = - vecteurDroite.h;
  c2 = pointDroite.h*vecteurDroite.v - pointDroite.v*vecteurDroite.h;
  
  // Calcule le determinant du systeme de 2 equations
  determinant = a1*b2 - a2*b1;
  
  // Cas particulier : si le determinant est nul
  if (FLOAT_NUL(determinant))
  {
    if (FLOAT_NUL(a1*c2 - a2*c1))
      return VMV_UneDroite;         // les 2 droites sont confondues
    else
      return VMV_Vide;            // les 2 droites sont paralleles
  }
  
  // Inverse le systeme
  pointTrouve->h = (b2*c1 - b1*c2) / determinant;
  pointTrouve->v = (a1*c2 - a2*c1) / determinant;
  
  // L'intersection des 2 droites appartient-elle au segment ?
  if ( ((*pointTrouve)-p1 | (*pointTrouve)-p2) <= 0 )
    return VMV_UnPoint;
  else
    return VMV_Vide;      
}


//
// La classe RectangleMv
//

RectangleMv::RectangleMv(const PositionMv& coin1, const PositionMv& coin2)
{
  if (coin1.h <= coin2.h)
  {
    coin_inf.h = coin1.h;
    coin_sup.h = coin2.h;
  }
  else
  {
    coin_inf.h = coin2.h;
    coin_sup.h = coin1.h;
  }
  
  if (coin1.v <= coin2.v)
  {
    coin_inf.v = coin1.v;
    coin_sup.v = coin2.v;
  }
  else
  {
    coin_inf.v = coin2.v;
    coin_sup.v = coin1.v;
  }
}

RectangleMv::RectangleMv(float x1, float y1, float x2, float y2)
{
  if (x1 <= x2)
  {
    coin_inf.h = x1;
    coin_sup.h = x2;
  }
  else
  {
    coin_inf.h = x2;
    coin_sup.h = x1;
  }
  
  if (y1 <= y2)
  {
    coin_inf.v = y1;
    coin_sup.v = y2;
  }
  else
  {
    coin_inf.v = y2;
    coin_sup.v = y1;
  }
}

RectangleMv operator +(const RectangleMv& r1, const RectangleMv& r2)

{
  RectangleMv r;
  
  r = r1;

  if (r.coin_inf.h > r2.coin_inf.h) r.coin_inf.h = r2.coin_inf.h;
  if (r.coin_inf.v > r2.coin_inf.v) r.coin_inf.v = r2.coin_inf.v;
  if (r.coin_sup.h < r2.coin_sup.h) r.coin_sup.h = r2.coin_sup.h;
  if (r.coin_sup.v < r2.coin_sup.v) r.coin_sup.v = r2.coin_sup.v;

  return r;
}

RectangleMv& RectangleMv::operator +=(const RectangleMv& r)

{
  if (coin_inf.h > r.coin_inf.h) coin_inf.h = r.coin_inf.h;
  if (coin_inf.v > r.coin_inf.v) coin_inf.v = r.coin_inf.v;
  if (coin_sup.h < r.coin_sup.h) coin_sup.h = r.coin_sup.h;
  if (coin_sup.v < r.coin_sup.v) coin_sup.v = r.coin_sup.v;

  return *this;
}

// Indique si l'intersection de 2 rectangles est non vide
Boolean RectangleMv::Intersecte(const RectangleMv& r) const

{
  Boolean retour;
  
  // r est-il a gauche de this ?
  if (r.coin_inf.h > this->coin_sup.h)
    retour = false;
    
  // a droite ?
  else if (r.coin_sup.h < this->coin_inf.h)
    retour = false;
    
  // plus haut ?
  else if (r.coin_sup.v < this->coin_inf.v)
    retour = false;
    
  // plus bas ?
  else if (r.coin_inf.v > this->coin_sup.v)
    retour = false;
    
  else
    retour = true;
    
  return retour;
}

//  ------------------------------------------------------------------------------------------------
//  Fonctions externes
//  ------------------------------------------------------------------------------------------------

