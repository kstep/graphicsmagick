//  ------------------------------------------------------------------------------------------------
//  MODULE      : FIErrors
//  LANGAGE     : C and C++
//  CREATOR     : Laurent SABORET 04/02/96, from FITypes.h by Laurent ALBERT
//  DESCRIPTION : Define the OSErr type and error codes
//  COMMENT   : Mustn't include ApplCommun.h
//  SCCSID      : @(#)b_error.h 1.1 11:49:54 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
#ifndef FIErrors_h
#define FIErrors_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifndef FITypes_h
  #include  "b_types.h"
#endif

#include  <errno.h>
    
#ifdef macintosh
  #include  <Errors.h>  
#endif


//  Constants
//  ---------

// Let's define Macintosh's errors on other machines :
#ifndef macintosh

enum {
  noErr = 0,        /*All is well*/
  fnfErr = -43,     /*File not found*/
  closErr = -24,      /*I/O System Errors*/
  memFullErr = -108,    /*not enough memory in heap zone*/
  eofErr = -39,     /* end of file error*/
  dskFulErr = -34,    /* end of file error*/
  writErr = -20,      /* write error*/
  badSectionErr = -451, /* bad section error*/
  envVersTooBig = -5502,  /* bigger than call can handle error*/
  badFidErr = -1307,    /*file id is dangling or doesn't match with the file number*/
  readErr = -19,      /* read error*/
  openErr = -23     /*I/O System Errors*/
};

#endif // #ifndef macintosh

//  ------------------------------------------------------------------------------------------------
#endif // FIErrors_h
//  ------------------------------------------------------------------------------------------------
