//+--------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents: CStrList header
//
//  Classes:  CStrList
//
//---------------------------------------------------------------

#ifndef __STRLIST_HXX__
#define __STRLIST_HXX__

#include "drt.hxx"

struct SStrEntry
{
    SStrEntry *pseNext, *psePrev;
    union
    {
  void *pv;
  unsigned long dw;
    } user;
    OLECHAR atc[1];   // Actually contains the whole string
};

class CStrList
{
public:
    CStrList(void);
    ~CStrList(void);
    
    SStrEntry *Add(OLECHAR *ptcs);
    void Remove(SStrEntry *pse);
    SStrEntry *Find(OLECHAR *ptcs);
    void Empty(void);
    
    SStrEntry *GetHead(void) { return _pseHead; }
    
private:
    SStrEntry *_pseHead;
};

#endif // #ifndef __STRLIST_HXX__
