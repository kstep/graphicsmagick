//  SCCSID      : @(#)pasconv.h 1.1 11:50:05 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

#ifndef pasconv_h
#define pasconv_h

#include "common_a.h"

#ifndef macintosh
  void NumToString (long x, unsigned char *s);
  long StringToNum (unsigned char *s, long *px);
#endif
  void PascalToC (const unsigned char *passtr, char *cstr);
  void CToPascal (const char *cstr, unsigned char *passtr);
  int PascalStrcmp (const unsigned char *a, const unsigned char *b);
//  char* c2pstr(char* cstr);

#endif  //pasconv_h
