//-------------------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents:   MSF header class
//
//  Classes:    CMSFHeader
//
//--------------------------------------------------------------------------

#ifndef __HEADER_HXX__
#define __HEADER_HXX__

#include "msf.hxx"
#include "storagep.h"

#define HDR_NOFORCE  0x0000
#define HDR_FORCE    0x0001
#define HDR_ALL      0x0002

struct SPreHeader : protected SStorageFile
{
protected:
  USHORT    _uMinorVersion;
  USHORT    _uDllVersion;
        USHORT          _uByteOrder;

  USHORT          _uSectorShift;
        USHORT          _uMiniSectorShift;

  USHORT          _usReserved;
  ULONG           _ulReserved1;
  ULONG           _ulReserved2;
    
        FSINDEX   _csectFat;
        SECT      _sectDirStart;

        DFSIGNATURE _signature;

        ULONG           _ulMiniSectorCutoff;

        SECT    _sectMiniFatStart;
        FSINDEX   _csectMiniFat;

        SECT    _sectDifStart;
        FSINDEX   _csectDif;
};


const USHORT CSECTFATREAL = (HEADERSIZE - sizeof(SPreHeader)) / sizeof(SECT);

const USHORT CSECTFAT = CSECTFATREAL;

class CMSFHeader: private SPreHeader
{
public:

    CMSFHeader(USHORT uSectorShift);
    
    SCODE Validate() const;
    
    inline USHORT GetMinorVersion() const;
    inline USHORT GetDllVersion() const;
  
    inline SCODE SetFatLength(const FSINDEX cFatSect);
    inline FSINDEX GetFatLength() const;

    inline SCODE SetMiniFatLength(const FSINDEX cFatSect);
    inline FSINDEX GetMiniFatLength() const;

    inline SCODE SetDirStart(const SECT sect);
    inline SECT GetDirStart() const;

    inline SCODE SetFatStart(const SECT sect);
    inline SECT GetFatStart()  const;

    inline SCODE SetMiniFatStart(const SECT sect);
    inline SECT GetMiniFatStart()  const;

    inline SCODE SetDifStart(const SECT sect);
    inline SECT  GetDifStart() const;

    inline SCODE SetDifLength(const FSINDEX cFatSect);
    inline FSINDEX GetDifLength() const;

    inline SECT GetFatSect(const FSINDEX oSect) const;
    inline SCODE SetFatSect(const FSINDEX oSect, const SECT sect);

    inline USHORT GetSectorShift() const;
    inline USHORT GetMiniSectorShift() const;

    inline ULONG  GetMiniSectorCutoff() const;

    inline DFSIGNATURE GetCommitSig() const;
    inline void SetCommitSig(const DFSIGNATURE sig);

    // whether machine is using diff order from file format
    inline BOOL DiffByteOrder() const;
    inline void ByteSwap();

private:
    SECT  _sectFat[CSECTFAT];

    SCODE SetSig(const BYTE *pbSig);
};


inline SCODE CMSFHeader::SetFatLength(const FSINDEX cFatSect)
{
    msfDebugOut((DEB_TRACE,"In CMSFHeader::SetFatLength(%lu)\n",cFatSect));
    _csectFat = cFatSect;
    msfDebugOut((DEB_TRACE,"Out CMSFHeader::SetFatLength()\n"));
    return S_OK;
}

inline FSINDEX CMSFHeader::GetFatLength() const
{
    return _csectFat;
}

inline SCODE CMSFHeader::SetMiniFatLength(const FSINDEX cFatSect)
{
    msfDebugOut((DEB_TRACE,"In CMSFHeader::SetMiniFatLength(%lu)\n",cFatSect));
    _csectMiniFat = cFatSect;
    msfDebugOut((DEB_TRACE,"Out CMSFHeader::SetMiniFatLength()\n"));
    return S_OK;
}

inline FSINDEX CMSFHeader::GetMiniFatLength() const
{
    return(_csectMiniFat);
}

inline SCODE CMSFHeader::SetDirStart(const SECT sectNew)
{
    _sectDirStart = sectNew;
    return S_OK;
}

inline SECT CMSFHeader::GetDirStart() const
{
    return _sectDirStart;
}

inline SCODE CMSFHeader::SetFatStart(const SECT sectNew)
{
    _sectFat[0] = sectNew;
    return S_OK;
}

inline SECT CMSFHeader::GetFatStart() const
{
    return _sectFat[0];
}

//+-------------------------------------------------------------------------
//
//  Member:     CMSFHeader::SetMiniFatStart
//
//  Synopsis:   Sets minifat's first sector's index
//
//  Arguments:  [sectNew]   -- sector index
//
//  Returns:    S_OK (necessary?)
//
//  Modifies:   _sectMiniFatStart
//
//--------------------------------------------------------------------------

inline SCODE CMSFHeader::SetMiniFatStart(const SECT sectNew)
{
    _sectMiniFatStart = sectNew;
    return S_OK;
}

//+-------------------------------------------------------------------------
//
//  Member:     CMSFHeader::GetMiniFatStart
//
//  Synopsis:   Gets minifat's first sector's index
//
//  Returns:    minifat's first sector's index
//
//--------------------------------------------------------------------------

inline SECT CMSFHeader::GetMiniFatStart() const
{
    return(_sectMiniFatStart);
}

inline SCODE CMSFHeader::SetDifStart(const SECT sectNew)
{
    _sectDifStart = sectNew;
    return S_OK;
}

inline SECT CMSFHeader::GetDifStart() const
{
    return _sectDifStart;
}

inline SECT CMSFHeader::GetFatSect(const FSINDEX oSect) const
{
    msfAssert(oSect < CSECTFAT);
    return _sectFat[oSect];
}

inline SCODE CMSFHeader::SetFatSect(const FSINDEX oSect, const SECT sect)
{
    msfAssert(oSect < CSECTFAT);
    _sectFat[oSect] = sect;
    return S_OK;
}

inline SCODE CMSFHeader::SetDifLength(const FSINDEX cFatSect)
{
    _csectDif = cFatSect;
    return S_OK;
}


inline FSINDEX CMSFHeader::GetDifLength() const
{
    return _csectDif;
}


inline USHORT CMSFHeader::GetSectorShift() const
{
    return _uSectorShift;
}


inline DFSIGNATURE CMSFHeader::GetCommitSig() const
{
    return _signature;
}

inline void CMSFHeader::SetCommitSig(const DFSIGNATURE sig)
{
    _signature = sig;
}

inline USHORT CMSFHeader::GetMiniSectorShift() const
{
    return _uMiniSectorShift;
}

inline ULONG CMSFHeader::GetMiniSectorCutoff() const
{
    return _ulMiniSectorCutoff;
}

inline USHORT CMSFHeader::GetMinorVersion() const
{
    return _uMinorVersion;
}

inline USHORT CMSFHeader::GetDllVersion() const
{
    return _uDllVersion;
}

// we always store Litte Endian on disk
#define DISK_BYTE_ORDER 0xFFFE

// whether machine is using diff order than file format
inline BOOL CMSFHeader::DiffByteOrder() const
{   
    // _uByteOrder stores what the machine will see
    // when it reads in a Little Endian 0xFFFE on disk
#ifndef _GENERATE_REVERSE_
    return _uByteOrder != DISK_BYTE_ORDER;
#else
    return _uByteOrder == DISK_BYTE_ORDER;
#endif
}

inline void CMSFHeader::ByteSwap() 
{
    if (DiffByteOrder())
    {
  ::ByteSwap(& _uMinorVersion);
  ::ByteSwap(& _uDllVersion);
  // note: _uByteOrder should not be swapped so that we know
  //       which Endian the machine is operating in.
  ::ByteSwap(& _uSectorShift);
  ::ByteSwap(& _uMiniSectorShift);

  ::ByteSwap(& _usReserved);
  ::ByteSwap(& _ulReserved1);
  ::ByteSwap(& _ulReserved2);
    
  ::ByteSwap(& _csectFat);
  ::ByteSwap(& _sectDirStart);
  
  ::ByteSwap(& _signature);
  
  ::ByteSwap(& _ulMiniSectorCutoff);

  ::ByteSwap(& _sectMiniFatStart);
  ::ByteSwap(& _csectMiniFat);

  ::ByteSwap(& _sectDifStart);
  ::ByteSwap(& _csectDif);

        // swapping the Fat table
        for (int cnt=0; cnt < CSECTFAT; cnt++)
            ::ByteSwap( &(_sectFat[cnt]) );
    }
}

const ULONG OACCESS = 0xFFFFFF00;
const ULONG OREADLOCK = OACCESS + 1;
const ULONG CREADLOCKS = 16;
const ULONG OUPDATE = OREADLOCK + CREADLOCKS + 1;
const ULONG OOPENLOCK = OUPDATE + 1;
const ULONG COPENLOCKS = 20;
const ULONG OOPENREADLOCK = OOPENLOCK;
const ULONG OOPENWRITELOCK = OOPENLOCK + COPENLOCKS;
const ULONG OOPENDENYREADLOCK = OOPENWRITELOCK + COPENLOCKS;
const ULONG OOPENDENYWRITELOCK = OOPENDENYREADLOCK + COPENLOCKS;


#endif //__HEADER_HXX__





