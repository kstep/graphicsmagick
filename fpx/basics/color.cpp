//  ------------------------------------------------------------------------------------------------
//  MODULE    : Couleur
//  LANGUAGE  : C++
//  AUTHOR    : Adolfo VIDE
//  DATE    : 09/15/92
//  DESCRIPTION : Color classes
//  COMMENT   :
//  SCCSID      : @(#)color.cpp 1.1 11:49:57 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #include "fpx_color.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifdef LP_USE_SEP_COLOR   // We don't want to link with the separator in the IVUE Tool Kit or in Shasta

  #include "ColorConvertors.h"
  #include "ColorMatching.h"

#endif

//  Constants
//  ----------

//  Variables
//  ---------

  GCR Couleur::gcrCourant = (float)0.5;

//  ------------------------------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment Couleur
  #endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Internal Functions 
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methods
//  ------------------------------------------------------------------------------------------------

CouleurRGB::CouleurRGB()
{
  // null
}

Couleur::Couleur(ComposanteC r, ComposanteC g, ComposanteC b)
  : rouge(r),
    vert(g),
    bleu(b),
    cyan(COL_INVALID),
    magenta(COL_INVALID),
    jaune(COL_INVALID),
    noir(COL_INVALID)
  
{
}

Couleur::Couleur(ComposanteC y, ComposanteC m, ComposanteC c, ComposanteC k)
  : rouge(COL_INVALID),
    vert(COL_INVALID),
    bleu(COL_INVALID),
    cyan(c),
    magenta(m),
    jaune(y),
    noir(k)
{
}

//  ------------------------------------------------------------------------------------------------
//  External Functions 
//  ------------------------------------------------------------------------------------------------

// CAUTION : similar interpolation functions are still defined in different
// places of the code (Masque.cp and MasqueK4.cp). They could certainly be
// somewhat generalized and optimized, eventually, rewritten in assembler code.

// R[16] is the result organized in 4 lines of 4 pixels.

void Interpole16CouleurRGB (const CouleurRGB &q0, const CouleurRGB &q1, 
              const CouleurRGB &q2, const CouleurRGB &q3, 
              ptr_CouleurRGB R)
{
  R[0] = q0;
  R[10].Average(q0,q1,q2,q3);
  R[2].Average(q0,q1);
  R[8].Average(q0,q2);
  R[1].Average(q0,R[2]);
  R[4].Average(q0,R[8]);
  R[15].Average(R[10],q3);
  R[7].Average(R[10],q1);
  R[13].Average(R[10],q2);
  R[3].Average(R[2],q1);
  R[12].Average(R[8],q2);
  R[5].Average(q0,R[10]);
  R[6].Average(R[2],R[10]);
  R[9].Average(R[8],R[10]);
  R[11].Average(R[7],R[15]);
  R[14].Average(R[13],R[15]);
}

// R[64] is the result. CAUTION : the resulting array is organized in 4 sub-tiles of 4x4
// and not 8 lines of 8 pixels…

void Interpole64CouleurRGB (const CouleurRGB &qNW, const CouleurRGB &qNE, 
              const CouleurRGB &qSW, const CouleurRGB &qSE, 
              ptr_CouleurRGB R)
{
  CouleurRGB  qN, qS, qE, qW, qC;

  // Compute the intermediate 4x4 internal vertices into the 8x8 square
  qN.Average(qNW,qNE);  // North
  qW.Average(qNW,qSW);  // West
  qE.Average(qNE,qSE);  // East
  qS.Average(qSW,qSE);  // South
  qC.Average(qN,qS);    // Center

  Interpole16CouleurRGB (qNW, qN,  qW,  qC,  R);      // NW sub-tile
  Interpole16CouleurRGB (qN,  qNE, qC,  qE,  R + 16);   // NE sub-tile
  Interpole16CouleurRGB (qW,  qC,  qSW, qS,  R + 32);   // SW sub-tile
  Interpole16CouleurRGB (qC,  qE,  qS,  qSE, R + 48);   // SE sub-tile
}

// Check the color dispersion into a buffer.
// Explanation of the algorithm :
// In binary computation, the distance between two values x1 and x2 is proportionnal to (x1 ^ x2).
// We would like to know the maximum distance in a set of points (…xi…). The idea is to make
// the logical sum (|) of all the differences (xj ^ xi). It is easy to demonstrate the following equation:

//                    |(xj^xi) = (|xj) ^ (&xi)

// So we can cumulate "|"'s in a variable and "&"'s in another and make the "^" at the end.
// Because we are looking for a criteria, we can check the intermediate result at each point and break
// the loop as soon as the distance exceed a given threshold value.
// For colors, we make the same computation on each channel and add ("|"'s) the results.

Boolean IsColorDispersion (ptr_CouleurRGB tile, long size)
{
#define IncLargeBuf   3
#define IncSmallBuf   1
#define ThresholdBuf  (5*IncLargeBuf)
#define DispLimit   ~0x00FF

  long IncPix = (size < ThresholdBuf ? IncSmallBuf : IncLargeBuf);
  
  // Declare and initialize accumulators
  unsigned short maxRed = 0x0000;
  unsigned short minRed = 0xFFFF;
  unsigned short maxGrn = 0x0000;
  unsigned short minGrn = 0xFFFF;
  unsigned short maxBlu = 0x0000;
  unsigned short minBlu = 0xFFFF;
  
  while (size > 0) {
  
    // Accumulate values
    maxRed |= tile->rouge;
    minRed &= tile->rouge;
    maxGrn |= tile->vert;
    minGrn &= tile->vert;
    maxBlu |= tile->bleu;
    minBlu &= tile->bleu;
    
    // Check intermediate dispersion and exit with true if dispersion exceeds limit
    if (((maxRed ^ minRed) | (maxGrn ^ minGrn) | (maxBlu ^ minBlu)) & DispLimit)
      return TRUE;
      
    // Next pixel
    size -= IncPix;
    tile += IncPix;
  }
  
  // Exit with false if the dispersion criteria has not been challenged
  return FALSE;
}


