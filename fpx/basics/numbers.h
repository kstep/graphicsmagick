//  ------------------------------------------------------------------------------------------------
//  MODULE    : Numbers
//  LANGAGE   : C++
//  CREATEUR  : Laurent SABORET
//  DATE    : Monday 21 November 1994
//  DESCRIPTION : Common functions about numbers
//  COMMENTAIRE : Created from PlutonCommun.h
//  SCCSID      : @(#)numbers.h 1.1 11:50:04 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef Numbers_h
  #define Numbers_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

//  Constantes
//  ----------

  #ifndef PI
    #define PI ((long double)3.141592653588)
  #endif

//  Declarations des classes
//  ------------------------

//  Definitions des classes
//  -----------------------

//  Fonctions 'inline'
//  ------------------

  // Convert a float into integers
  
  inline unsigned char FLOAT_TO_UCHAR(float x)
  {
    return  (unsigned char) (x + 0.5);
  }
  
  inline int32 FLOAT_TO_INT32(float x)
  {
    if (x >= 0)
      return  (int32) (x + 0.5);
    else
      return  (int32) (x - 0.5);
  }
  
  inline long FLOAT_TO_LONG(float x)
  {
    if (x >= 0)
      return  (long) (x + 0.5);
    else
      return  (long) (x - 0.5);
  }
  
  inline short FLOAT_TO_SHORT(float x)
  {
    if (x >= 0)
      return  (short) (x + 0.5);
    else
      return  (short) (x - 0.5);
  }
  
  
  // Valeur absolue
  
  inline long ABS(long x)
  {
    return  ( (x >= 0) ? x : (-x) );
  }
  
  inline short ABS(short x)
  {
    return  ( (x >= 0) ? x : (-x) );
  }
  
  inline float ABS(float x)
  {
    return  ( (x >= 0) ? x : (-x) );
  }
  
  
  // Are 2 floats equals ? 
  // Algo : Est-ce-que |(a-b)/a| <= epsilon ?
  inline Boolean FLOAT_EGAL(float a, float b)
  {
    if (ABS(a) <= 1.0E-5)
      return  ( ABS( b ) <= 1.0E-5 );
    else if (ABS(b) <= 1.0E-5)
      return  ( ABS( a ) <= 1.0E-5 );
    else
      return  ( ABS( (float)(a - b) ) <= 1.0E-3 * ABS(a) );
  }
  
  // Is a float equal to zero ?
  inline Boolean  FLOAT_NUL(float a)  
  {
    return  ( (a <= 1.0E-5) && (a >= -1.0E-5) );
  }
  
  
  // Swap 2 variables
  
  inline void SWAP(long& a, long& b)
  {
    long  tmp;
    
    tmp = a;
    a   = b;
    b   = tmp;
  }
  
  inline void SWAP(short& a, short& b)
  {
    short tmp;
    
    tmp = a;
    a   = b;
    b   = tmp;
  }
  
  inline void SWAP(float& a, float& b)
  {
    float tmp;
    
    tmp = a;
    a   = b;
    b   = tmp;
  }
  
  inline void SWAP(Boolean& a, Boolean& b)
  {
    Boolean tmp;
    
    tmp = a;
    a   = b;
    b   = tmp;
  }
  
  
  // Sign
  #define SGN(x)    ( ((x) >= 0) ? 1 : (-1) )
  
  
  // Maximun
  
  inline long MAX(long x, long y)
  {
    return  ( (x >= y) ? x : y );
  }
  
  inline short MAX(short x, short y)
  {
    return  ( (x >= y) ? x : y );
  }
  
  inline float MAX(float x, float y)
  {
    return  ( (x >= y) ? x : y );
  }
  
  inline double MAX(double x, double y)
  {
    return  ( (x >= y) ? x : y );
  }
  
  inline unsigned char MAX(unsigned char a, unsigned char b)
  {
    return (a >= b) ? a : b;
  }
  
  
  // Minimum
  
  inline short MIN(short a, short b)
  {
    return (a <= b) ? a : b;
  }
  
  inline long MIN(long a, long b)
  {
    return (a <= b) ? a : b;
  }
  
  inline float MIN(float a, float b)
  {
    return (a <= b) ? a : b;
  }
  
  inline unsigned char MIN(unsigned char a, unsigned char b)
  {
    return (a <= b) ? a : b;
  }
  

//  Fonctions 'extern'
//  ------------------

//  Variables 'extern'
//  ------------------

//  ------------------------------------------------------------------------------------------------
  #endif // Numbers_h
//  ------------------------------------------------------------------------------------------------
