//+-------------------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents: DocFile Time Support
//
//--------------------------------------------------------------------------

#ifndef __TIME16_HXX__
#define __TIME16_HXX__
#include "../../h/storage.h"
#include <time.h>

// Time type
typedef FILETIME TIME_T;

STDAPI_(void) FileTimeToTimeT(const FILETIME *pft, time_t *ptt);
STDAPI_(void) TimeTToFileTime(const time_t *ptt, FILETIME *pft);

#endif
