//  ----------------------------------------------------------------------------
//  MODULE    : FlashPixUtils.cpp
//  LANGUAGE  : C++
//  AUTHORS   : Nicolas ROMANTZOFF, Philippe BOSSUT
//  DATE    : Friday, July 7th 1995
//  DESCRIPTION : Names, IDs and Types of things as they are defined in the FPX spec:
//                FPX Image Specification, Final Draft, Eastman Kodak company, 
//                December 7 1995
//  COMMENT   : EVERYTHING you could find in the spec should be found here
//          and only here as a define. 
//  SCCSID      : @(#)fpxutils.cpp  1.1 11:59:23 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ----------------------------------------------------------------------------
  #include "fpxutils.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include <stdio.h>
  
#ifndef OLE_COMMUN_H
  #include "olecomm.h"
#endif

//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Functions 'intern'
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods
//  ----------------------------------------------------------------------------


//  ----------------------------------------------------------------------------
//   Functions "extern'
//  ----------------------------------------------------------------------------


void  GetSummaryInfoName  (char * buffer)
{
  if (buffer)
    sprintf (buffer, FMT_SummaryInfoName, StreamPrefix);
}

void  GetImageContentName  (char * buffer)
{
  if (buffer)
    sprintf (buffer, FMT_ImageContentName, StreamPrefix);
}

void  GetImageInfoName  (char * buffer)
{
  if (buffer)
    sprintf (buffer, FMT_ImageInfoName, StreamPrefix);
}

void  GetResolutionName (char * buffer, unsigned long resNum)
{
  if (buffer)
    sprintf (buffer, FMT_ResolutionName, (int) resNum);
}

void  GetSubImageHdrName (char * buffer, unsigned long imgNum)
{
  if (buffer)
    sprintf (buffer, FMT_SubImageHdrName, (int) imgNum);
}

void  GetSubImageDataName (char * buffer, unsigned long imgNum)
{
  if (buffer)
    sprintf (buffer, FMT_SubImageDataName, (int) imgNum);
}

void  GetSupportDataName (char * buffer, unsigned long imgNum)
{
  if (buffer)
    sprintf (buffer, FMT_SupportDataName, (int) imgNum);
}

void  GetICCProfileName (char * buffer, unsigned short imgNum)
{
  if (buffer)
    sprintf (buffer, FMT_ICCProfiles, imgNum);
}

void  GetCustomLinkName  (char * buffer)
{
  if (buffer)
    sprintf (buffer, FMT_CustomLink, StreamPrefix);
}

void  GetGlobalInfoName (char * buffer)
{
  if (buffer)
    sprintf (buffer, FMT_GlobalInfo, StreamPrefix);
}

void  GetImageDescName  (char * buffer, unsigned long index)
{
  if (buffer)
    sprintf (buffer, FMT_ImageDescription, StreamPrefix, (int) index);
}

void  GetImageStoreName (char * buffer, unsigned long index)
{
  if (buffer)
    sprintf (buffer, FMT_ImageStore, (int) index);
}

void  GetTransformName  (char * buffer, unsigned long index)
{
  if (buffer)
    sprintf (buffer, FMT_Transform, StreamPrefix, (int) index);
}

void  GetOperationName  (char * buffer, unsigned long index)
{
  if (buffer)
    sprintf (buffer, FMT_Operation, StreamPrefix, (int) index);
}


void  GetExtensionName  (char * buffer)
{
  if (buffer)
    sprintf (buffer, FMT_Extension, StreamPrefix);
}


/* FOR TEST & DEBUG ONLY
GUID  GetGuid()
{
  static short num = 0;
  GUID nextGuid = {0xd1167eb0,0xb8ba,0x11ce,0xaa,0x0f,0x00,0xaa,0x00,0x32,0xcb,0xa8};
  nextGuid.Data1 += num;
  num++;
  
  return nextGuid;
}
*/

