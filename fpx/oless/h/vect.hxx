//-------------------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents: Vector common types
//
//  Classes:  CVectBits -- Bit fields for vectors
//
//--------------------------------------------------------------------------

#ifndef __VECT_HXX__
#define __VECT_HXX__

#include "page.hxx"

//+-------------------------------------------------------------------------
//
//  Class:      CVectBits (vb)
//
//  Purpose:    Structure for Vector flags.
//
//  Interface:
//
//  Notes:
//
//--------------------------------------------------------------------------

struct CVectBits
{
    BYTE    full:1;
    USHORT    firstfree;

    inline CVectBits();
};

//+-------------------------------------------------------------------------
//
//  Method:     CVectBits::CVectBits, public
//
//  Synopsis:   CVectBits default constructor.
//
//  Notes:
//
//--------------------------------------------------------------------------

inline CVectBits::CVectBits()
{
    full = 0;
    firstfree = 0;
}


inline CVectBits * GetNewVectBits(ULONG ulSize)
{
    msfAssert(ulSize > 0);
    CVectBits *pfb = NULL;

    if (ulSize <= (_HEAP_MAXREQ / sizeof(CVectBits)))
    {
        pfb = new CVectBits[(MAXINDEXTYPE)ulSize];
    }
    return pfb;
}


//+-------------------------------------------------------------------------
//
//  Class:      CPagedVector (pv)
//
//  Purpose:    *Finish This*
//
//  Interface:
//
//  Notes:
//
//--------------------------------------------------------------------------

class CPagedVector
{
public:
    inline CPagedVector(const SID sid);

    SCODE Init(CMStream *pms, ULONG ulSize);

    ~CPagedVector();

    void Empty(void);


    SCODE Resize(ULONG ulSize);

    SCODE Flush(void);

    SCODE GetTable(const ULONG iTable, DWORD dwFlags, void **ppmp);
    inline void ReleaseTable(const ULONG iTable);

    inline void SetSect(const ULONG iTable, const SECT sect);

    inline CVectBits * GetBits(const ULONG iTable);

    inline void ResetBits(void);

    SCODE SetDirty(ULONG iTable);
    inline void ResetDirty(ULONG iTable);

    inline void FreeTable(ULONG iTable);

    inline CMStream * GetParent(void) const;
    inline void SetParent(CMStream *pms);

private:
    CMSFPageTable * _pmpt;
    const SID _sid;

    ULONG _ulSize;          //  Amount in use
    ULONG _ulAllocSize;     //  Amount allocated

    CMStream *_pmsParent;

    CMSFPage **_amp;
    CVectBits *_avb;
#ifdef _MSC_VER
#pragma warning(disable:4512)
// since there is a const member, there should be no assignment operator
#endif // _MSC_VER
};
#ifdef _MSC_VER
#pragma warning(default:4512)
#endif // _MSC_VER


inline CPagedVector::CPagedVector(const SID sid)
: _pmpt(NULL),
  _sid(sid),
  _ulSize(0),
  _ulAllocSize(0),
  _pmsParent(NULL),
  _amp(NULL),
  _avb(NULL)
{
}

#endif //__VECT_HXX__
