//  ------------------------------------------------------------------------------------------------
//  MODULE    : Chaine
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : Jeudi 15 Octobre 1992
//  DESCRIPTION : 
//  COMMENTAIRE :
//  SCCSID      : @(#)b_string.cpp  1.1 11:49:56 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #include "b_string.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include <math.h>
  #include <string.h>
  #include <stdio.h>
  #include <stdlib.h>

#ifdef macintosh
  #include "graphic.h"
#endif 

//  Constants
//  ---------

//  #define ESPACE  "\p "

//  Variables
//  ---------

  char  lpDecimalSeparator = '.';
  char  lpThousandSeparator = ',';
  
//  ------------------------------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment Chaine
  #endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Private C functions
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methods
//  ------------------------------------------------------------------------------------------------

Chaine::Chaine()

{
  lng = 0;
}

Chaine::Chaine(const Chaine& laChaine)

{
  long size = laChaine.Longueur()+1;
  size = ((unsigned long) size > sizeof(Chaine) ? sizeof(Chaine) : size);
  BlockMove(&laChaine,this,size); 
}

  
Chaine::Chaine(const unsigned char *const pStr)

{
  BlockMove(pStr,this,*pStr+1);
  car[*pStr] = '\0';
}

Chaine::Chaine(float reel, short nbDec)

{
  short i;
  short nb;
  float arrondi = (float)0.5;
  unsigned char chiffre;
  unsigned char inutile;
  long intPart = 0;
  long decPart = 0;
  
  lng = 0;
  if (nbDec > 9)
    nbDec = 9;
    
  if (reel < 0) {
    reel = -reel;
    car[lng++] = '-';
  }
  
  for (nb=nbDec;nb--;)
    arrondi /= 10;
  reel += arrondi;
  for (i=0;reel>=1;i++,reel /= (float)10.0)
    if (i > 32)
      break;
  if (i) {
    while (i--) {
      reel *= 10;
      car[lng++] = (chiffre = (unsigned char)reel) + '0';
      reel -= chiffre;
      intPart = intPart*10 + chiffre;
    }
  } else {
    car[lng++] = '0';
  }
  inutile = lng;
  car[lng++] = lpDecimalSeparator;
  while (nbDec--) {
    reel *= 10;
    car[lng++] = (chiffre = (unsigned char)reel) + '0';
    reel -= chiffre;
    decPart = decPart*10 + chiffre;
    if (chiffre)
      inutile = lng;
  }
  lng = inutile;
  // Avoid the "negative 0" case
  if ((intPart == 0) && (decPart == 0)) {
    car[0] = '0';
    lng = 1;
  }
  car[lng] = '\0';
}

Chaine::Chaine(float reel, short nbDec, unsigned char decSep)
{
  short i;
  short nb;
  float arrondi = (float)0.5;
  unsigned char chiffre;
  unsigned char inutile;
  long intPart = 0;
  long decPart = 0;
  
  lng = 0;
  if (nbDec > 9)
    nbDec = 9;
    
  if (reel < 0) {
    reel = -reel;
    car[lng++] = '-';
  }
  
  for (nb=nbDec;nb--;)
    arrondi /= 10;
  reel += arrondi;
  for (i=0;reel>=1;i++,reel /= (float)10.0)
    if (i > 32)
      break;
  if (i) {
    while (i--) {
      reel *= 10;
      car[lng++] = (chiffre = (unsigned char)reel) + '0';
      reel -= chiffre;
      intPart = intPart*10 + chiffre;
    }
  } else {
    car[lng++] = '0';
  }
  inutile = lng;
  car[lng++] = decSep;
  while (nbDec--) {
    reel *= 10;
    car[lng++] = (chiffre = (unsigned char)reel) + '0';
    reel -= chiffre;
    decPart = decPart*10 + chiffre;
    if (chiffre)
      inutile = lng;
  }
  lng = inutile;
  // Avoid the "negative 0" case
  if ((intPart == 0) && (decPart == 0)) {
    car[0] = '0';
    lng = 1;
  }
  car[lng] = '\0';
}

Chaine::Chaine(const CStringHolder& s)  
{
  size_t size = strlen(s.GetString());
  size = (size > MAX_CHAINE ? MAX_CHAINE : size);
  this->lng = size;
  BlockMove(s.GetString(),this->car,size);  
  this->car[size]='\0';
}
                  
Chaine::operator float() const
{
  long  i;
  float reel = (float)0;
  float decimale = (float)1;
  Boolean negatif = FALSE;
  
  for(i=0;car[i]==' ';)
    if (++i>=lng)
      goto fin;
  if (car[i] == '-') {
    negatif = TRUE;
    if (++i>=lng)
      goto fin;
  } else if (car[i] == '+')
    if (++i>=lng)
      goto fin;
  while ((car[i] >= '0') && (car[i] <= '9')) {
    reel *= (float)10;
    reel += car[i] - '0';
    if (++i>=lng)
      goto fin;
  }
  if ((car[i] == ',') || (car[i] == '.')) {
    if (++i>=lng)
      goto fin;
    while ((car[i] >= '0') && (car[i] <= '9')) {
      decimale *= 10;
      reel *= 10;
      reel += car[i] - '0';
      if (++i>=lng)
        goto fin;
    }
  }
fin:
  if (negatif)
    reel = -reel;
  if (decimale > 1)
    reel /= decimale;
  return reel;
}

ref_Chaine Chaine::operator+= (const Chaine& s)
{
  if (s.lng)
    if (s.lng + lng > MAX_CHAINE) {
      BlockMove(s.car,car+lng,MAX_CHAINE-lng);
      lng = MAX_CHAINE;
    } else {
      BlockMove(s.car,car+lng,s.lng);
      lng += s.lng;
    }
  return *this;
}

ref_Chaine Chaine::operator+= (const unsigned char* s)
{
  if (s[0])
    if (s[0] + lng > MAX_CHAINE) {
      BlockMove(s+1,car+lng,MAX_CHAINE-lng);
      lng = MAX_CHAINE;
    } else {
      BlockMove(s+1,car+lng,s[0]);
      lng += s[0];
    }
  return *this;
}

ref_Chaine Chaine::operator+= (const CStringHolder& s)
{
  size_t len = strlen(s.GetString());
  
  if (len > 0)
    if (len + lng > MAX_CHAINE) {
      BlockMove(s.GetString(),car+lng,MAX_CHAINE-lng);
      lng = MAX_CHAINE;
    } else {
      BlockMove(s.GetString(),car+lng,len);
      lng += len;
    }
  return *this;
}

Chaine operator+ (const Chaine& s1, const Chaine& s2)
{
  Chaine s = s1;
  s += s2;
  return s;
}

Chaine operator+ (const Chaine& s1, const unsigned char* s2)
{
  Chaine s = s1;
  s += s2;
  return s;
}

Chaine operator+ (const unsigned char* s1, const Chaine& s2)
{
  Chaine s;
  s += s1;
  s += s2;
  return s;
}

Chaine operator+ (const Chaine& s1, const CStringHolder& s2)
{
  Chaine s = s1;
  s += s2;
  return s;
}

Chaine operator+ (const CStringHolder& s1, const Chaine& s2)
{
  Chaine s;
  s += s1;
  s += s2;
  return s;
}

Chaine::operator long() const
{
  // note: float is an operator override.
  // This takes a string, uses the float override,
  // then converts to long
  float nb = *this;
    
  return (long)nb;
}
  
// If 'this' contains a meta sequence of ^0, ^1 or ^2, then insert <replaceStr>
//   in the position specified by 'numParam'
void Chaine::PlaceParametre(const Chaine& replaceStr, short numParam )
{
#ifndef macintosh
  // Look for the correct parameter
  char * toSearch = "^0";
  if (numParam == 1) 
    toSearch[1] = '1';
  else if (numParam == 2) 
    toSearch[1] = '2';
    
  // Search
  const char * foundStr = strstr((const char*)Texte(), toSearch);
  
  // If found, build the string with replacment
  if (foundStr != NULL)
  {
    char modifiedStr[2*256];
    int  nbUnchangedStartingChars;
    
    // Copy unmodified start of string *this
    nbUnchangedStartingChars = foundStr - (const char*)Texte();
    strncpy(modifiedStr, (const char*)Texte(), nbUnchangedStartingChars);
    modifiedStr[nbUnchangedStartingChars] = '\0';
    
    // Replace "^i" by replaceStr
    strcat(modifiedStr, (const char*)replaceStr.Texte());
    
    // Copy unmodified end of string *this
    strcat(modifiedStr, foundStr+strlen(toSearch));

    // Copy modifiedStr to *this
    *this = Chaine(CStringHolder(modifiedStr));
  }

#else

  Handle localString = NewHandle(lng);
  Handle localReplace = NewHandle(replaceStr.Longueur());

  BlockMove(car,*localString,lng);
  BlockMove((Ptr)replaceStr.Texte(),*localReplace,replaceStr.Longueur());
  // replace the correct parameter
  if (numParam == 1) 
    ReplaceText(localString,localReplace,(unsigned char*) "\p^1");
  else if (numParam == 2) 
    ReplaceText(localString,localReplace,(unsigned char*) "\p^2");
  else 
    ReplaceText(localString,localReplace,(unsigned char*) "\p^0");
  lng = (unsigned char)GetHandleSize(localString);
  if (lng)
    BlockMove(*localString,car,lng);
  DisposHandle(localString);
  DisposHandle(localReplace);
  
#endif
}


#ifdef macintosh

void Chaine::TruncChaine(GrafPtr theGraphicPointer, unsigned char numberOfCharacters)
// this can be used only for constant character width fonts 
{
  short size;
  
  size = TextWidth("a",0,1);
  TruncString(short(numberOfCharacters)*size, *this, truncEnd);
  
  numberOfCharacters++;
  if (Longueur() < numberOfCharacters) {
    memset(&car[lng], ' ', size_t(numberOfCharacters) - size_t(lng));
    lng = numberOfCharacters;
  }
}

void Chaine::JustifyChaineRight(GrafPtr theGraphicPointer, long numberOfCharacters)
// this can be used only for constant character width fonts 
{ 
  Chaine temp;
  if (Longueur() < numberOfCharacters)      // if the lenght is lower than numberOfCharacters
  {                     // add the right number of blanks of the left
    size_t blankWidth = size_t(numberOfCharacters) - size_t(lng);
    memset(temp.Texte(), ' ', blankWidth);
    BlockMove(this->Texte(), (temp.Texte()) + blankWidth, lng);
    temp.lng += blankWidth + lng;
    lng += blankWidth;
    *this = temp;
  }
  else {                    // else 
  short size;                 // trunc the chaine to the required numberOfCharacters
  numberOfCharacters++;           // truncChaine truncs at numberOfCharacters-1 and adds ...

  size = TextWidth("a",0,1);          // at numberOfCharacters
  TruncString(short(numberOfCharacters)*size, *this, truncEnd);
  }
}

#endif // macintosh

void Chaine::TruncChaine(unsigned char numberOfCharacters)
// this can be used only for constant character width fonts 
{
  if (Longueur() >= numberOfCharacters)
    lng = numberOfCharacters;
  else {
    memset(&car[lng], ' ', size_t(numberOfCharacters) - size_t(lng));
    lng = numberOfCharacters;
  }
}

Chaine63::Chaine63()
{
  lng = 0;
}
  
Chaine63::Chaine63(const unsigned char *const pStr)
{
  BlockMove(pStr,this,*pStr+1);
  car[*pStr] = '\0';
}

Chaine63::Chaine63(float reel, short nbDec)
{
  short i;
  short nb;
  float arrondi = (float)0.5;
  unsigned char chiffre;
  unsigned char inutile;
  
  lng = 0;
  if (nbDec > 9)
    nbDec = 9;
    
  if (reel < 0) {
    reel = -reel;
    car[lng++] = '-';
  }
  
  for (nb=nbDec;nb--;)
    arrondi /= 10;
  reel += arrondi;
  for (i=0;reel>=1;i++,reel /= (float)10.0)
    if (i > 32)
      break;
  if (i) {
    while (i--) {
      reel *= 10;
      car[lng++] = (chiffre = (unsigned char)reel) + '0';
      reel -= chiffre;
    }
  } else {
    car[lng++] = '0';
  }
  inutile = lng;
  car[lng++] = lpDecimalSeparator;
  while (nbDec--) {
    reel *= 10;
    car[lng++] = (chiffre = (unsigned char)reel) + '0';
    reel -= chiffre;
    if (chiffre)
      inutile = lng;
  }
  lng = inutile;
}

Chaine63::Chaine63(const CStringHolder& s)  
{
  size_t size = strlen(s.GetString());
  size = (size > 63 ? 63 : size);
  this->lng = size;
  BlockMove(s.GetString(),this->car,size);  
}
                  
Chaine63::~Chaine63() 
{
}

Chaine63::operator float() const
{
  long  i;
  float reel = (float)0;
  float decimale = (float)1;
  Boolean negatif = FALSE;
  
  for(i=0;car[i]==' ';)
    if (++i>=lng)
      goto fin;
  if (car[i] == '-') {
    negatif = TRUE;
    if (++i>=lng)
      goto fin;
  } else if (car[i] == '+')
    if (++i>=lng)
      goto fin;
  while ((car[i] >= '0') && (car[i] <= '9')) {
    reel *= 10;
    reel += car[i] - '0';
    if (++i>=lng)
      goto fin;
  }
  if ((car[i] == ',') || (car[i] == '.')) {
    if (++i>=lng)
      goto fin;
    while ((car[i] >= '0') && (car[i] <= '9')) {
      decimale *= 10;
      reel *= 10;
      reel += car[i] - '0';
      if (++i>=lng)
        goto fin;
    }
  }
fin:
  if (negatif)
    reel = -reel;
  if (decimale > 1)
    reel /= decimale;
  return reel;
}

ref_Chaine63 Chaine63::operator+= (const Chaine63& s)
{
  if (s.lng)
    if (s.lng + lng > 63) {
      BlockMove(s.car,car+lng,63-lng);
      lng = 63;
    } else {
      BlockMove(s.car,car+lng,s.lng);
      lng += s.lng;
    }
  return *this;
}

ref_Chaine63 Chaine63::operator+= (const unsigned char* s)
{
  if (s[0])
    if (s[0] + lng > 63) {
      BlockMove(s+1,car+lng,63-lng);
      lng = 63;
    } else {
      BlockMove(s+1,car+lng,s[0]);
      lng += s[0];
    }
  return *this;
}

ref_Chaine63 Chaine63::operator+= (const CStringHolder& s)
{
  size_t len = strlen(s.GetString());
  
  if (len > 0)
    if (len + lng > 63) {
      BlockMove(s.GetString(),car+lng,63-lng);
      lng = 63;
    } else {
      BlockMove(s.GetString(),car+lng,len);
      lng += len;
    }
  return *this;
}

Chaine63 operator+ (const Chaine63& s1, const Chaine63& s2)
{
  Chaine63 s = s1;
  s += s2;
  return s;
}

Chaine63 operator+ (const Chaine63& s1, const unsigned char* s2)
{
  Chaine63 s = s1;
  s += s2;
  return s;
}

Chaine63 operator+ (const unsigned char* s1, const Chaine63& s2)
{
  Chaine63 s;
  s += s1;
  s += s2;
  return s;
}

Chaine63 operator+ (const Chaine63& s1, const CStringHolder& s2)
{
  Chaine63 s = s1;
  s += s2;
  return s;
}

Chaine63 operator+ (const CStringHolder& s1, const Chaine63& s2)
{
  Chaine63 s;
  s += s1;
  s += s2;
  return s;
}

#ifdef macintosh

void Chaine63::PlaceParametre(const Chaine63& replaceStr, short numParam )
{
#ifndef macintosh
  // Look for the correct parameter
  char * toSearch = "^0";
  if (numParam == 1) 
    toSearch[1] = '1';
  else if (numParam == 2) 
    toSearch[1] = '2';
    
  // Search
  char * foundStr = strstr(Texte(), toSearch);
  
  // If found, build the string with replacment
  if (foundStr != NULL)
  {
    char modifiedStr[2*256];
    int  nbUnchangedStartingChars;
    
    // Copy unmodified start of string *this
    nbUnchangedStartingChars = foundStr - Texte();
    strncpy(modifiedStr, (const char*)Texte(), nbUnchangedStartingChars);
    modifiedStr[nbUnchangedStartingChars] = '\0';
    
    // Replace "^i" by replaceStr
    strcat(modifiedStr, replaceStr.Texte());
    
    // Copy unmodified end of string *this
    strcat(modifiedStr, foundStr+strlen(toSearch));

    // Copy modifiedStr to *this
    *this = Chaine63(CStringHolder(modifiedStr));
  }

#else

  Handle localString = NewHandle(lng);
  Handle localReplace = NewHandle(replaceStr.Longueur());

  BlockMove(car,*localString,lng);
  BlockMove((Ptr)replaceStr.Texte(),*localReplace,replaceStr.Longueur());
  // replace the correct parameter
  if (numParam == 1) 
    ReplaceText(localString,localReplace,(unsigned char*) "\p^1");
  else if (numParam == 2) 
    ReplaceText(localString,localReplace,(unsigned char*) "\p^2");
  else 
    ReplaceText(localString,localReplace,(unsigned char*) "\p^0");
  lng = (unsigned char)GetHandleSize(localString);
  if (lng)
    BlockMove(*localString,car,lng);
  DisposHandle(localString);
  DisposHandle(localReplace);
  
#endif
}

void Chaine63::TruncChaine(GrafPtr theGraphicPointer, unsigned char numberOfCharacters)
// this can be used only for constant character width fonts 
{
  short size;
  
  size = TextWidth("a",0,1);
  TruncString(short(numberOfCharacters)*size, *this, truncEnd);
  
//  while (Longueur() < numberOfCharacters+1){
//    *this += ESPACE;
//  }
  numberOfCharacters++;
  if (Longueur() < numberOfCharacters) {
    memset(&car[lng], ' ', size_t(numberOfCharacters) - size_t(lng));
    lng = numberOfCharacters;
  }
}

void Chaine63::JustifyChaineRight(GrafPtr /*theGraphicPointer*/, long /*numberOfCharacters*/)
{
}

#endif // macintosh

void Chaine63::TruncChaine(unsigned char numberOfCharacters)
// this can be used only for constant character width fonts 
{
  if (Longueur() >= numberOfCharacters)
    lng = numberOfCharacters;
  else {
    memset(&car[lng], ' ', size_t(numberOfCharacters) - size_t(lng));
    lng = numberOfCharacters;
  }
}


//  ------------------------------------------------------------------------------------------------
//  Exported C functions
//  ------------------------------------------------------------------------------------------------

#ifdef macintosh
void LoadInternationalSeparators()

{
  Intl0Hndl itl0Resource = (Intl0Hndl)GetIntlResource(0);
  
  if (itl0Resource) {
    if ((*itl0Resource)->decimalPt == '.' || (*itl0Resource)->decimalPt == ',')
      lpDecimalSeparator = (*itl0Resource)->decimalPt;
    lpThousandSeparator = (*itl0Resource)->thousSep;
  }
}
#endif // macintosh
