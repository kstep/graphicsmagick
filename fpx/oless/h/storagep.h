//+-------------------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents:   Internal storage information
//
//--------------------------------------------------------------------------

#ifndef __STORAGEP_H__
#define __STORAGEP_H__

// The byte combination that identifies that a file is a storage of
// some kind

const BYTE SIGSTG[] = {0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1};
const BYTE CBSIGSTG = sizeof(SIGSTG);

// The first portion of a storage file
struct SStorageFile
{
    BYTE  abSig[CBSIGSTG];    //  Signature
    CLSID _clid;        //  Class Id
};

#endif

