//  ------------------------------------------------------------------------------------------------
//  MODULE    : CorrectLut
//  LANGAGE   : C++
//  CREATEUR  : Laurent Saboret
//  DATE    : Lundi 21 FŽvrier 1994
//  DESCRIPTION : Lookup table for a RGB pixel
//  COMMENTAIRE :
//  SCCSID      : @(#)corr_lut.cpp  1.1 11:46:37 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #include "corr_lut.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifndef Fichier_h
  #include  "a_file.h"
#endif

//  Constantes
//  ----------

  const short   lut_NbContraintes = 3;      // For backward compatibility with LP 1.5§1

//  Variables
//  ---------

//  ------------------------------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment CorrectLut
  #endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Fonctions internes
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methodes
//  ------------------------------------------------------------------------------------------------

// Creates Identity LUT
CorrectLut::CorrectLut()
{
  active = FALSE;
}

// Creates a classic LUT
CorrectLut::CorrectLut(const Lut r, const Lut g, const Lut b)

{
  active = TRUE;
  
  // Copy LUTs
  BlockMove(r, red,   256);
  BlockMove(g, green, 256);
  BlockMove(b, blue,  256);
}

// The 'rgb' LUT is applied on each channel, after the 3 other luts.
CorrectLut::CorrectLut(const Lut rgb, const Lut r, const Lut g, const Lut b)

{
  int i;
  
  active = TRUE;
  
  // Convert 4 LUTs into 3 LUTs
  for ( i=0 ; i < 256 ; i++ ) 
  {
    red[i]   = rgb[ r[i] ];
    green[i] = rgb[ g[i] ];
    blue[i]  = rgb[ b[i] ];
  }
}

CorrectLut::CorrectLut(const CorrectLut& toCopy)

{
  active = toCopy.active;
  
  BlockMove(toCopy.red,   red,   256);
  BlockMove(toCopy.green, green, 256);
  BlockMove(toCopy.blue,  blue,  256);
}

CorrectLut& CorrectLut::operator=(const CorrectLut& toCopy)

{
  active = toCopy.active;
  
  BlockMove(toCopy.red,   red,   256);
  BlockMove(toCopy.green, green, 256);
  BlockMove(toCopy.blue,  blue,  256);
  
  return *this;
}


// Get the lookup table's description
void CorrectLut::GetLuts(Lut r, Lut g, Lut b) const

{
  int   i;
  
  if (r != NULL)
  {
    if (active) {
      BlockMove(red, r, 256);
    } else {
      for ( i=0 ; i<256 ; i++ )
        r[i] = i;
    }
  }
  
  if (g != NULL)
  {
    if (active) {
      BlockMove(green, g, 256);
    } else {
      for ( i=0 ; i<256 ; i++ )
        g[i] = i;
    }
  }
  
  if (b != NULL)
  {
    if (active) {
      BlockMove(blue, b, 256);
    } else {
      for ( i=0 ; i<256 ; i++ )
        b[i] = i;
    }
  }
}


// Compute a*b
CorrectLut  operator*(const CorrectLut& a, const CorrectLut& b)

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
    CorrectLut  ab;     // == a * b
    int     i;
  
    ab.active = TRUE;
    for ( i=0 ; i < 256 ; i++ )
    {
      ab.red[i]   = a.red[b.red[i]];
      ab.green[i] = a.green[b.green[i]];
      ab.blue[i]  = a.blue[b.blue[i]];
    }
    return ab;
  }
}


void CorrectLut::Save(ref_Fichier file)

{
  RGBColor  colorTarget={0,0,0};
  Boolean   valide=FALSE;
  int     i;

  file.Ecriture(active);
  if (active) 
  {
    file.EcritureTableau((unsigned char*)red,   256);
    file.EcritureTableau((unsigned char*)green, 256);
    file.EcritureTableau((unsigned char*)blue,  256);

    // For backward compatibility with LP 1.5§1
    for (i=0; i<2*lut_NbContraintes; i++)   
      file.Ecriture(colorTarget); 
    for (i=0; i<lut_NbContraintes; i++)
      file.Ecriture(valide);
    file.Ecriture((short)0);
  }
}

void CorrectLut::Load(ref_Fichier file, long)

{
  RGBColor  colorTarget={0,0,0};
  Boolean   valide=FALSE;
  short int influence=0;
  int     i;

  file.Lecture(&active);
  if (active) 
  {
    file.LectureTableau((unsigned char*)red,   256);
    file.LectureTableau((unsigned char*)green, 256);
    file.LectureTableau((unsigned char*)blue,  256);

    for (i=0; i<2*lut_NbContraintes; i++)     // Items unused after LP 1.5§1
      file.Lecture(&colorTarget);         // For backward compatibility
    for (i=0; i<lut_NbContraintes; i++)
      file.Lecture(&valide);
    file.Lecture(&influence);
  }
}

  
//  ------------------------------------------------------------------------------------------------
//  Fonctions externes
//  ------------------------------------------------------------------------------------------------

