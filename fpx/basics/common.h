//	------------------------------------------------------------------------------------------------
//	MODULE		:	Commun
//	LANGAGE		:	C(++)
//	DESCRIPTION	:	Header included everywhere but by Application project and PlutonCommun.h
//	COMMENTAIRE	:	Contains global declarations.
//  SCCSID      : @(#)common.h	1.1 11:49:57 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
	#ifndef Commun_h
	#define Commun_h
//	------------------------------------------------------------------------------------------------

#ifndef FPXEXPORT
  #define FPXEXPORT extern __declspec(dllexport)
#endif
#ifndef JPEGEXPORT
  #define JPEGEXPORT extern __declspec(dllexport)
#endif

#ifndef ApplCommun_h
	#include	"common_a.h"
#endif

#ifndef PlutonCommun_h
	#include 	"common_b.h"
#endif

//	------------------------------------------------------------------------------------------------
	#endif // Commun_h
//	------------------------------------------------------------------------------------------------
