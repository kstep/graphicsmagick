//+--------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents: CDocFileIterator header file
//
//  Classes:  CDocFileIterator
//
//---------------------------------------------------------------

#ifndef __ITER_HXX__
#define __ITER_HXX__

#include "h/piter.hxx"

class CMSFIterator;
class CStgHandle;

//+--------------------------------------------------------------
//
//  Class:  CDocFileIterator (dfi)
//
//  Purpose:  Derive a new iterator that remembers what DocFile it
//    came from
//
//  Interface:  Same as PDocFileIterator
//
//---------------------------------------------------------------

class CDocFileIterator : public PDocFileIterator
{
public:
    CDocFileIterator(void);
    SCODE Init(CStgHandle *ph);
    virtual ~CDocFileIterator(void);
    
    virtual SCODE GetNext(STATSTGW *pstatstg);
    virtual SCODE BufferGetNext(SIterBuffer *pib);
    virtual void Release(void);

private:    
    CMSFIterator *_pi;
};

#endif
