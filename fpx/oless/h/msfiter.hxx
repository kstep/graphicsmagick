//+-------------------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Classes:    CMSFIterator - Main iterator class for MSF
//
//  Functions:  None.
//
//--------------------------------------------------------------------------

#ifndef __MSFITER_HXX__
#define __MSFITER_HXX__

#include "msf.hxx"

class CDirectory;

//+-------------------------------------------------------------------------
//
//  Class:      CMSFIterator
//
//  Purpose:    Iterator object provided by multi-stream
//
//  Interface:  See below
//
//--------------------------------------------------------------------------

class CMSFIterator
{
    public:
        inline CMSFIterator(CDirectory *pdir, SID sidChild);
        SCODE GetNext(STATSTGW *pstat);
  SCODE BufferGetNext(SIterBuffer *pib);
        inline SCODE Rewind();

        inline void  Release();

    private:
        CDirectory *_pdir;
  SID _sidChildRoot;
        CDfName _dfnCurrent;
};

inline CMSFIterator::CMSFIterator(CDirectory *pdir, SID sidChild)
{
    _pdir = pdir;
    _sidChildRoot = sidChild;
}

//+-------------------------------------------------------------------------
//
//  Member:     CMSFIterator::Rewind, public
//
//  Synposis:   Reset iterator to first position.
//
//  Effects:    Modifies _sidCurrent
//
//  Arguments:  Void.
//
//  Returns:    S_OK
//
//  Notes:
//
//---------------------------------------------------------------------------

inline SCODE CMSFIterator::Rewind()
{
    msfDebugOut((DEB_TRACE,"In CMSFIterator::Rewind()\n"));
    _dfnCurrent.Set((WORD)0, (BYTE *)NULL);
    msfDebugOut((DEB_TRACE,"Leaving CMSFIterator::Rewind()\n"));
    return S_OK;
}

//+-------------------------------------------------------------------------
//
//  Method:     CMSFIterator::Release, public
//
//  Synopsis:   Release this MSFIterator instance
//
//  Arguments:  None.
//
//  Returns:    Void.
//
//--------------------------------------------------------------------------

inline void CMSFIterator::Release()
{
    delete this;
}

#endif  //__MSFITER_HXX__
