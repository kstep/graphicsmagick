/*
  FILE  : NPixel.h

  CLASSES : NColor, NPixel

  AUTHOR  : Nicolas ROMANTZOFF

  PURPOSE : Provide a native class for pixels : aRGB on MOTOROLA, BGRa on INTEL...

  USAGE : Shouldn't have any .cpp with it...

  SCCSID      : @(#)npixel.h  1.1 11:50:03 18 Dec 1996
  ----------------------------------------------------------------------------
   Copyright (c) 1999 Digital Imaging Group, Inc.
   For conditions of distribution and use, see copyright notice
   in Flashpix.h
  ----------------------------------------------------------------------------
*/
#ifndef NPixel_h
#define NPixel_h

  #include "b_types.h"
  #include "pixel.h"

//  Classes declaration
//  -------------------

  class NPixel;
    typedef NPixel* ptr_NPixel;

//  Classes definition
//  ------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////
  #ifdef __bigEndian
    struct NColor {
      unsigned8   alpha;
      unsigned8   red;
      unsigned8   green;
      unsigned8   blue;
    };
  #else
    struct NColor {
      unsigned8   blue;
      unsigned8   green;
      unsigned8   red;
      unsigned8   alpha;
    };
  #endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
  class NPixel  {
    public:
                NPixel(const unsigned32 color = 0);
                NPixel(const unsigned8 red, const unsigned8 green, const unsigned8 blue, const unsigned8 alpha = 0);
                NPixel(const NColor& col);
                NPixel(const NPixel& pix);
                NPixel(const Pixel& pix);
          NPixel&   operator=(const NPixel& pix);
          NPixel&   operator=(const NColor& col);
          NPixel&   operator=(const unsigned32 color);

                operator unsigned32(void) const;
                operator NColor(void) const;
                operator Pixel(void) const;


          unsigned8 Alpha(void) const;
          unsigned8 Red(void) const;
          unsigned8 Green(void) const;
          unsigned8 Blue(void) const;

          unsigned8&  Alpha(void);
          unsigned8&  Red(void);
          unsigned8&  Green(void);
          unsigned8&  Blue(void);

      union {
        NColor    itsStruct;
        unsigned32  itsColor;
      };
  };
  
  
  // Inlines
  // -------

  // Swap a long from big to little endian or vice-versa
  // Added by Peter Hughes 21 May 1996
  #define SWAP4(x) ( (((x)>>24) & 0x000000FF) | (((x)>>8) & 0x0000FF00) | \
             (((x)<<8) & 0x00FF0000) | (((x)<<24) & 0xFF000000) )

  inline  NPixel::NPixel(const unsigned32 color)
  { itsColor = color; }

  inline  NPixel::NPixel(const unsigned8 red, const unsigned8 green, const unsigned8 blue, const unsigned8 alpha)
  { itsStruct.red = red; itsStruct.green = green; itsStruct.blue = blue; itsStruct.alpha = alpha; }

  inline  NPixel::NPixel(const NColor& col)
  { itsStruct = col; }

  inline  NPixel::NPixel(const NPixel& pix)
  { itsColor = pix.itsColor; }

  inline  NPixel::NPixel(const Pixel& pix)
  { 
    register unsigned32 newColor = *((unsigned32 *)(&pix));
#ifdef __bigEndian
    itsColor = newColor;                    // Simple copy
#else
    itsColor = SWAP4(newColor);                 // Bytes swapped copy
#endif
  }

  inline  NPixel&   NPixel::operator= (const NPixel& pix)
  { itsColor = pix.itsColor; return *this; }

  inline  NPixel&   NPixel::operator= (const NColor& col)
  { itsStruct = col; return *this; }

  inline  NPixel&   NPixel::operator= (const unsigned32 color)
  { itsColor = color; return *this; }

  inline  NPixel::operator unsigned32(void) const
  { return itsColor; }

  inline  NPixel::operator NColor(void) const
  { return itsStruct; }

  inline  NPixel::operator Pixel(void) const
  { 
    Pixel pix;
#ifdef __bigEndian
    *((unsigned32 *)(&pix)) = itsColor;             // Simple copy
#else
    *((unsigned32 *)(&pix)) = SWAP4(itsColor);          // Bytes swapped copy
#endif
    return pix;
  }

  inline unsigned8 NPixel::Alpha(void) const
  { return itsStruct.alpha; }

  inline unsigned8 NPixel::Red(void) const
  { return itsStruct.red; }

  inline unsigned8 NPixel::Green(void) const
  { return itsStruct.green; }

  inline unsigned8 NPixel::Blue(void) const
  { return itsStruct.blue; }

  inline unsigned8& NPixel::Alpha(void) 
  { return itsStruct.alpha; }

  inline unsigned8& NPixel::Red(void) 
  { return itsStruct.red; }

  inline unsigned8& NPixel::Green(void) 
  { return itsStruct.green; }

  inline unsigned8& NPixel::Blue(void) 
  { return itsStruct.blue; }

#endif
