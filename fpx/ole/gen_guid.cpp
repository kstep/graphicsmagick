//  ----------------------------------------------------------------------------
//  MODULE    : Gen_guid 
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Thursday, May 2, 1996 
//  DESCRIPTION : Guid generation compatible Mac and Windows.
//  SCCSID      : @(#)gen_guid.cpp  1.1 11:53:26 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment Gen_guid
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef Debug_h
  #include "debug.h"
#endif

#ifdef _WINDOWS 
  #include "b_error.h"
#endif

#ifdef _UNIX
  #include <sys/time.h>
#endif

//  ----------------------------------------------------------------------------
  #include "gen_guid.h"
//  ----------------------------------------------------------------------------

//  Constants
//  ---------

#define UUID_LEN        128                 /* In bits */
#define UUID_VERSION    0x1000              /* DCE variant */
#define UUID_RESERVED   0x10
#define DEFAULT_ETHER_ADDR  "8:0:20:20:32:d0"   /* Historically significant... :-) */

//  Variables
//  ---------

#if   !defined(__unix)
typedef struct _timespec
{
  unsigned long tv_nsec;
  unsigned long tv_sec;
} timespec;
#endif

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

#if defined(macintosh) || defined(__unix)
static void GetENETAddress(unsigned char *ENETaddress)
{
  // MPPParamBlock thePB;
  short i;
  char linkAddr[7];
  char zoneName[34];
  // OSErr err;
  
  linkAddr[0] = 0x08;
  linkAddr[1] = 0x00;
  linkAddr[2] = 0x20;
  linkAddr[3] = 0x20;
  linkAddr[4] = 0x32;
  linkAddr[5] = 0xd0;
  // thePB.GAIINFO.version = 1;
  // thePB.GAIINFO.LAlength = 6;
  // thePB.GAIINFO.linkAddr = linkAddr;
  // thePB.GAIINFO.zoneName = zoneName;
  // err = PGetAppleTalkInfo(&thePB, 0);
  for(i=0;i<6;i++)
    *(ENETaddress+i) = linkAddr[i];
}
#endif

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------

#if defined(macintosh) || defined(_UNIX)
HRESULT GenGuid(CLSID *clsid)
{
  timespec ts;
  unsigned char ether_address[20];
  unsigned long loop, time_low, offset;
  unsigned short clock_sequence, time_mid, time_hi_and_version;
  unsigned char *uuid, clock_seq_hi, clock_seq_low; 

  offset = 0;

  uuid = (unsigned char *)clsid;

#if defined(macintosh)
  ts.tv_sec  = TickCount() * 60;
  ts.tv_nsec = ts.tv_sec   * 100000; 
#else
#  ifdef _UNIX
  {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
  }
#  endif
#endif

//  clock_gettime(CLOCK_REALTIME, &ts);

  /* Copy in TIME_LOW to uuid from nano seconds */
  time_low = ts.tv_nsec;
  BlockMove((unsigned char *)&time_low, (unsigned char *)(uuid + offset), sizeof(unsigned long));
  offset+=sizeof(unsigned long);

  /* Copy in TIME_MID to uuid from seconds */
  time_mid = (unsigned short)(ts.tv_sec & 0x0000ffff);
  BlockMove((unsigned char *)&time_mid, (unsigned char *)(uuid + offset), sizeof(unsigned short));
  offset+=sizeof(unsigned short);

  /* Copy in TIME_HI_AND_VERSION to uuid from seconds and version id */
  time_hi_and_version = (unsigned short)(ts.tv_sec >> 8);
  time_hi_and_version &= 0x0fff;
  time_hi_and_version |= UUID_VERSION;
  BlockMove((unsigned char *)&time_hi_and_version, (unsigned char *)(uuid + offset), sizeof(unsigned short));
  offset+=sizeof(unsigned short);

  /* Copy in CLOCK_SEQUENCE to uuid from seconds and version id */
  /* Always assume we have to reinit the clock sequence.
   * It's just easier this way... :-)
   */
  srand(ts.tv_nsec);    /* Seed random number generator with nanoseconds */
  clock_sequence = ((unsigned short)rand() % 16384);

  clock_seq_low = (unsigned char)(clock_sequence);

  clock_seq_hi = (unsigned char)(clock_sequence >> 8);
  clock_seq_hi &= 0x3f;
  clock_seq_hi |= UUID_RESERVED;
  BlockMove((unsigned char *)&clock_seq_hi, (unsigned char *)(uuid + offset), sizeof(unsigned char));
  offset+=sizeof(unsigned char);
  BlockMove((unsigned char *)&clock_seq_low, (unsigned char *)(uuid + offset), sizeof(unsigned char));
  offset+=sizeof(unsigned char);

  GetENETAddress(ether_address);

  for (loop = 0; loop < 6; loop++)
    *(uuid + offset + loop) = (unsigned char)ether_address[loop];
    
 return(S_OK);

}
#else
// The Windows version is of course better integrated...
HRESULT GenGuid(CLSID *clsid)
{
  return CoCreateGuid(clsid);
}
#endif // macintosh


//  - EOF ----------------------------------------------------------------------
