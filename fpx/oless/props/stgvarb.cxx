//+-------------------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents:   C++ Base wrapper for PROPVARIANT.
//
//--------------------------------------------------------------------------

#include "pch.cxx"
#include <ctype.h>

/* right now only US ansi support */
EXTERN_C 
STDAPI_(UINT) GetACP(VOID)
{ return 1252; }  /* Latin 1 (US, Western Europe) */

#if DBGPROP

BOOLEAN
IsUnicodeString(WCHAR const *pwszname, ULONG cb)
{
    if (cb != 0)
    {
  ULONG i, cchDoubleAnsi, cchNull;

  cchNull = cchDoubleAnsi = 0;
  for (i = 0; pwszname[i] != (OLECHAR)'\0'; i++)
  {
      if ((char) pwszname[i] == '\0' || (char) (pwszname[i] >> 8) == '\0')
      {
    cchNull++;
    if (i > 8 && cchDoubleAnsi > (3*i)/4)
    {
        // BUGBUG: This is a heuristic that should NOT be left in
        // the build for long (even the checked build).
        //PROPASSERT(!"IsUnicodeString: Suspicious string: double Ansi chars");
        //return(FALSE);
        return(TRUE);
    }
      }
      else
      if (isprint((char) pwszname[i]) && isprint((char) (pwszname[i] >> 8)))
      {
    cchDoubleAnsi++;
      }
  }
  if (cchNull < i/4)
  {
      // BUGBUG: This is a heuristic that should NOT be left in the
      // build for long (even the checked build).

      //BUGBUG: cscdrt stringizes GUIDs, leaving lots of high bytes set.
      //The DRT needs to change to avoid case mapping problems.
      //Until then, return TRUE
      //PROPASSERT(!"IsUnicodeString: Suspicious string: too few zero-extended Ansi chars");
      //return(FALSE);
  }
        // If cb isn't MAXULONG we verify that cb is at least as
        // big as the string.  We can't check for equality, because
        // there are some property sets in which the length field
        // for a string may include several zero padding bytes.
        
        PROPASSERT(cb == MAXULONG || (i + 1) * sizeof(WCHAR) <= cb);
    }
    return(TRUE);
}


BOOLEAN
IsAnsiString(CHAR const *pszname, ULONG cb)
{
    if (cb != 0)
    {
  ULONG i;

  for (i = 0; pszname[i] != '\0'; i++)
  {
  }
  if (i == 1 && isprint(pszname[0]) &&
      ((ULONG) &pszname[8] & 0xfff) == ((ULONG) pszname & 0xfff) &&
      isprint(pszname[2]) && pszname[3] == '\0' &&
      isprint(pszname[4]) && pszname[5] == '\0' &&
      isprint(pszname[6]) && pszname[7] == '\0')
  {
      // BUGBUG: This is a heuristic that should NOT be left in the
      // build for long (even the checked build).
      PROPASSERT(!"IsAnsiString: Suspicious string: looks like Unicode");
      return(FALSE);
  }
        // If cb isn't MAXULONG we verify that cb is at least as
        // big as the string.  We can't check for equality, because
        // there are some property sets in which the length field
        // for a string may include several zero padding bytes.
        
  PROPASSERT(cb == MAXULONG || i + 1 <= cb);
    }
    return(TRUE);
}
#endif
 
