//---------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents: functions exported from docfile.cxx
//
//---------------------------------------------------------------

#ifndef __DFENTRY_HXX__
#define __DFENTRY_HXX__

#include "dfmsp.hxx"

#ifndef _UNICODE
HRESULT DfOpenStorageOnILockBytes(ILockBytes *plkbyt,
                                  IStorage *pstgPriority,
                                  DWORD grfMode,
                                  SNB snbExclude,
                                  DWORD reserved,
                                  IStorage **ppstgOpen,
                                  CLSID *pcid);
#else
HRESULT DfOpenStorageOnILockBytesW(ILockBytes *plkbyt,
                                   IStorage *pstgPriority,
                                   DWORD grfMode,
                                   SNBW snbExclude,
                                   DWORD reserved,
                                   IStorage **ppstgOpen,
                                   CLSID *pcid);
#endif

#endif // #ifndef __DFENTRY_HXX__
