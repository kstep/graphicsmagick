//  ------------------------------------------------------------------------------------------------
//  MODULE    : CorrectLut
//  LANGUAGE  : C++
//  AUTHOR    : Laurent Saboret
//  DATE    : Monday, February 21st 1994
//  DESCRIPTION : Lookup table for a RGB pixel
//  COMMENT   :
//  SCCSID      : @(#)corr_lut.h  1.1 11:46:38 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef CorrectLut_h
  #define CorrectLut_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include  "pixel.h"
  #include  "npixel.h"

//  Constants
//  ---------
  typedef unsigned char Lut[256];

//  Classes declarations
//  --------------------

  // not to include Fichier.h
  class Fichier;
  typedef Fichier* ptr_Fichier;
  typedef Fichier& ref_Fichier;

  struct CorrectLut;
  typedef CorrectLut* ptr_CorrectLut;
  typedef CorrectLut& ref_CorrectLut;

//  Classes definitions
//  -------------------

  // A CmykCorrectLut is a lookup table for a RGB pixel
  struct CorrectLut {

  public:
          // Creates Identity LUT
                CorrectLut();           
          // Creates a classic LUT
                CorrectLut(const Lut r, const Lut g, const Lut b);  
          // The 'rgb' LUT is applied on each channel, after the 3 other luts
                CorrectLut(const Lut rgb, const Lut r, const Lut g, const Lut b);
                
          // Copy
                CorrectLut(const CorrectLut& toCopy);
          CorrectLut& operator=(const CorrectLut& toCopy);
                
          // Apply a color correction to a pixel
          Pixel   operator()(const Pixel& pixIn) const;
          NPixel    operator()(const NPixel& pixIn) const;
          
          // Get the lookup table's description
          Boolean   IsActive() const;
          void    GetLuts(Lut r, Lut g, Lut b) const;
          
          // Compute a*b
      friend  CorrectLut  operator*(const CorrectLut& a, const CorrectLut& b);

      virtual void    Save(ref_Fichier file);
      virtual void    Load(ref_Fichier file, long version);
    
  private:
          Boolean   active;       // If false, *this is the Identity LUT
          Lut     red;
          Lut     green;
          Lut     blue;
  };


//  Functions 'inline'
//  ------------------

  inline Boolean CorrectLut::IsActive() const
  {
    return active;
  }
  
  // Apply a color correction to a BIG ENDIAN pixel
  inline Pixel CorrectLut::operator()(const Pixel& pixIn) const
  {
    if (active)
    {
      Pixel  pixOut;
    
      pixOut.alpha = pixIn.alpha;
      pixOut.rouge = red[pixIn.rouge];
      pixOut.vert  = green[pixIn.vert];
      pixOut.bleu  = blue[pixIn.bleu];
      
      return pixOut;
    }
    else
    {
      return pixIn;
    }
  }
  
  // Apply a color correction to a NATIVE pixel
  inline NPixel CorrectLut::operator()(const NPixel& pixIn) const
  {
    if (active) 
    {
      return NPixel(red[pixIn.Red()], green[pixIn.Green()], blue[pixIn.Blue()], pixIn.Alpha());
    } 
    else
    {
      return pixIn;
    } 
  }
  
//  Functions 'extern'
//  ------------------

//  Variables 'extern'
//  ------------------

//  ------------------------------------------------------------------------------------------------
  #endif // CorrectLut_h
//  ------------------------------------------------------------------------------------------------
