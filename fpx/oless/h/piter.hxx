//+--------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents: PDocFileIterator header file
//
//  Classes:  PDocFileIterator
//
//---------------------------------------------------------------

#ifndef __PITER_HXX__
#define __PITER_HXX__

//+--------------------------------------------------------------
//
//  Class:  PDocFileIterator (dfi)
//
//  Purpose:  DocFile iterator protocol
//
//  Interface:  See below
//
//---------------------------------------------------------------

class PDocFileIterator
{
public:
    virtual SCODE GetNext(STATSTGW *pstatstg) = 0;
    virtual SCODE BufferGetNext(SIterBuffer *pib) = 0;
    virtual void Release(void) = 0;
};

#endif
