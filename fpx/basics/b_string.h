//  ------------------------------------------------------------------------------------------------
//  MODULE    : Chaine
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : Jeudi 15 Octobre 1992
//  DESCRIPTION :
//  COMMENTAIRE :
//  SCCSID      : @(#)b_string.h  1.1 11:49:56 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
#ifndef Chaine_h
#define Chaine_h
#ifndef ApplCommun_h
#  include    "common_a.h"
#endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#include "pasconv.h"
#include "mac_comp.h"

#include <memory.h>
#include <string.h>

#ifdef macintosh
#  include <OSUtils.h>
#  include <Packages.h>
#  include <TextUtils.h>
#endif


//  Constants
//  ---------

#define MAX_CHAINE  255 // It has to be fixed at 255 for backwards compatibility with files that may contain Chaine

//  Classes declaration
//  -------------------

class Chaine;
typedef Chaine* ptr_Chaine;
typedef Chaine& ref_Chaine;

class ChaineFic;
typedef ChaineFic* ptr_ChaineFic;
typedef ChaineFic& ref_ChaineFic;

class Chaine63;
typedef Chaine63* ptr_Chaine63;
typedef Chaine63& ref_Chaine63;

struct CStringHolder;
typedef CStringHolder* ptr_CStringHolder;
typedef CStringHolder& ref_CStringHolder;
  
//  Classes definition
//  ------------------

// Pascal string (MAX_CHAINE char max)
class Chaine {
public:
  Chaine();
  Chaine(long entier);
  Chaine(float reel, short nbDec);
  Chaine(float reel, short nbDec, unsigned char decSep);
  Chaine(const unsigned char *const pStr);
  Chaine(const CStringHolder& s);
  Chaine(const Chaine& laChaine);
  Chaine(const Chaine63& laChaine);
  operator unsigned char*() const;
  operator long() const;
  operator float() const;
                
  friend  inline int operator== (const Chaine& s1, const Chaine& s2);
  friend  inline int operator!= (const Chaine& s1, const Chaine& s2);
    
  ref_Chaine    operator+= (const Chaine& s);
  ref_Chaine    operator+= (const unsigned char* s);
  ref_Chaine    operator+= (const CStringHolder& s);
  friend  Chaine  operator+ (const Chaine& s1, const Chaine& s2);
  friend  Chaine  operator+ (const Chaine& s1, const unsigned char* s2);
  friend  Chaine  operator+ (const unsigned char* s1, const Chaine& s2);
  friend  Chaine  operator+ (const Chaine& s1, const CStringHolder& s2);
  friend  Chaine  operator+ (const CStringHolder& s1, const Chaine& s2);
                
  unsigned char         Longueur() const;
  void                  SubstChar(unsigned char oldC, unsigned char newC);
  void                  SubstChars(const Chaine& oldChars, unsigned char newC);
  Ptr                   Texte() const;
  
  void      TruncChaine(unsigned char numberOfCharacters);

#ifdef macintosh
  void      TruncChaine(GrafPtr theGraphicPointer, unsigned char numberOfCharacters);
  void      JustifyChaineRight(GrafPtr theGraphicPointer, long numberOfCharacters);
#endif
  void      PlaceParametre(const Chaine& remplacement, short numParam = 0);
        
private:
  unsigned char lng;
  unsigned char car[MAX_CHAINE];
};
  
  
// File name as a Pascal string
class ChaineFic : public Chaine {
public:
  ChaineFic();
  ChaineFic(const Str63 s63);
  ChaineFic(const Chaine& laChaine);

  friend  inline  int   operator== (const ChaineFic& s1, const ChaineFic& s2);
  friend  inline  int   operator!= (const ChaineFic& s1, const ChaineFic& s2);
};
  
// Pascal string (63 char max)
class Chaine63 {
public:
  Chaine63();
  Chaine63(long entier);
  Chaine63(float reel, short nbDec);
  Chaine63(const unsigned char *const pStr);
  Chaine63(const CStringHolder& s);
  Chaine63(const Chaine& laChaine);

  ~Chaine63();
                
  operator unsigned char*() const;
  operator long() const;
  operator float() const;
                
  friend  inline  int   operator== (const Chaine63& s1, const Chaine63& s2);
  friend  inline  int   operator!= (const Chaine63& s1, const Chaine63& s2);

  ref_Chaine63  operator+= (const Chaine63& s);
  ref_Chaine63  operator+= (const unsigned char* s);
  ref_Chaine63  operator+= (const CStringHolder& s);
  friend  Chaine63    operator+ (const Chaine63& s1, const Chaine63& s2);
  friend  Chaine63    operator+ (const Chaine63& s1, const unsigned char* s2);
  friend  Chaine63    operator+ (const unsigned char* s1, const Chaine63& s2);
  friend  Chaine63    operator+ (const Chaine63& s1, const CStringHolder& s2);
  friend  Chaine63    operator+ (const CStringHolder& s1, const Chaine63& s2);
                
  unsigned char Longueur() const;
  Ptr       Texte() const;
  
  void      TruncChaine(unsigned char numberOfCharacters);
#ifdef macintosh
  void      TruncChaine(GrafPtr theGraphicPointer, unsigned char numberOfCharacters);
  void      JustifyChaineRight(GrafPtr theGraphicPointer, long numberOfCharacters);
#endif
  void      PlaceParametre(const Chaine63& remplacement, short numParam = 0);
        
private:
  unsigned char lng;
  unsigned char car[63];
    
};

  
  // C String
struct CStringHolder {

  CStringHolder (const unsigned char* cString);
  CStringHolder (const char* cString);
                
  const char* GetString() const;
                
private:
  const char*   cString;
};


//  Inline methods
//  --------------

inline Chaine::Chaine(long entier)
{
  NumToString(entier, (unsigned char*)this);
}
  
inline Chaine::operator unsigned char*() const
{
  return (unsigned char*)this;
}
  
inline unsigned char Chaine::Longueur() const
{
  return lng;
}
  
inline void Chaine::SubstChar(unsigned char oldC, unsigned char newC)
// Replaces all occurences of oldC with newC in Chaine
{ 
  unsigned char *s;
  while ((s = (unsigned char *) memchr(car, oldC, lng)) != NULL) *s = newC;
}
  
inline void Chaine::SubstChars(const Chaine& oldChars, unsigned char newC)
  // Replaces all occurences of all characters in oldChars with newC in Chaine
{ 
  unsigned char *s;
  const unsigned char *c = oldChars.car;
  for (int i=oldChars.lng ; i>0 ; i-- , c++)
    while ((s = (unsigned char *) memchr(car, *c, lng)) != NULL) *s = newC;
}
  
inline Ptr Chaine::Texte() const
{
  Chaine* non_const_this = (Chaine*)this;
  non_const_this->car[lng] = '\0';
    
  return (Ptr)car;
}
  
inline int operator== (const Chaine& s1, const Chaine& s2)
{
  return (PascalStrcmp (s1, s2) == 0);
}
  
inline int operator!= (const Chaine& s1, const Chaine& s2)
{
  return (PascalStrcmp (s1, s2) != 0);
}

inline ChaineFic::ChaineFic()
{
}
  
inline ChaineFic::ChaineFic(const Str63 s63)
{
  BlockMove(s63,this,*s63+2); 
}
  
inline ChaineFic::ChaineFic(const Chaine& laChaine)
{
  BlockMove(&laChaine,this,laChaine.Longueur()+2);  
}
  
inline int operator== (const ChaineFic& s1, const ChaineFic& s2)
{
  return (PascalStrcmp (s1, s2) == 0);
}
  
inline int operator!= (const ChaineFic& s1, const ChaineFic& s2)
{
  return (PascalStrcmp (s1, s2) != 0);
}
  
inline Chaine63::Chaine63(long entier)
{
  NumToString(entier, (unsigned char*)this);
}
  
inline Chaine63::operator unsigned char*() const
{
  return (unsigned char*)this;
}
  
inline Chaine63::Chaine63(const Chaine& laChaine)
{
  long size = laChaine.Longueur()+1;
  size = (size > (long) sizeof(Chaine63) ? sizeof(Chaine63) : size);
  BlockMove(&laChaine,this,size); 
}
  
inline Chaine63::operator long() const
{
  long nb;
    
  StringToNum(*this,&nb);
  return nb;
}
  
inline unsigned char Chaine63::Longueur() const
{
  return lng;
}
  
inline Ptr Chaine63::Texte() const
{
  // LS 06/28/96: add a final \0
  Chaine63* non_const_this = (Chaine63*)this;
  non_const_this->car[lng] = '\0';
    
  return (Ptr)car;
}
  
inline int operator== (const Chaine63& s1, const Chaine63& s2)
{
  return (PascalStrcmp (s1, s2) == 0);
}
  
inline int operator!= (const Chaine63& s1, const Chaine63& s2)
{
  return (PascalStrcmp (s1, s2) != 0);
}
  
inline Chaine::Chaine(const Chaine63& laChaine)
{
  long size = laChaine.Longueur()+2;  //+2 because of length field AND '/0' in string itself
  size = (size > (long) sizeof(Chaine) ? sizeof(Chaine) : size);
  BlockMove(&laChaine,this,size); 
}
  
inline CStringHolder::CStringHolder (const char* theCString)
  : cString ((const char*) theCString)
{
}
  
inline CStringHolder::CStringHolder (const unsigned char* theCString)
  : cString ((const char*) theCString)
{
}

inline const char* CStringHolder::GetString() const
{
  return cString;
}
  
  
//  Exported C functions
//  --------------------

#ifdef macintosh
extern void LoadInternationalSeparators();
#endif


//  Global variables
//  ----------------

// GDN - we want this in all frameworks, don't we?
extern char lpDecimalSeparator;
extern char lpThousandSeparator;


//  ------------------------------------------------------------------------------------------------
#endif // Chaine_h
//  ------------------------------------------------------------------------------------------------
