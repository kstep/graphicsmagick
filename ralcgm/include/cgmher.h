/*   RAL-CGM Interpreter header file:  @(#) cgmher.h  version 3.1
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the authors.
 *
 * It is not in the public domain.
 *
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the authors should be marked as
 * such.
 *
 * Description: Definitions of all shared variables in the Hershey text
 *              system
 *
 *  Maintenance log:
 *
 *  20 May 91 CDS Removed character refernce point from struct herchar
 *                as this is now handled within genher.
 *  11 Sep 91 CDS Removed utility specific bits to leave only 'user'
 *                bits.
 *  16 Nov 92 CDS Changed ifdef ANSI to ifdef PROTO for function prototypes
 *
 */
#ifndef CGMHER_H
#define CGMHER_H

#ifdef CGMHER_C
#define Extern
#else
#define Extern extern
#endif

#ifdef PROTO

Extern void HERtext( Textitem *, Point ,
                     void (*)( int, Point*, Tmatrix, int ) ),
            HERgetfd( Textitem *txtit);

#else

Extern void HERtext(),             /*  Output the text  */
            HERgetfd();            /*  Text details query routine  */

#endif

#undef Extern

#endif
