/*   RAL-CGM Interpreter header file:  @(#) cgmbez.h  version 3.1
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
 * Description: Definitions of user callable routines
 *
 * Modification log:
 *
 *  11 Sep 91 CDS  Split of utility stuff to leave only 'user' bits.
 *  16 Nov 92 CDS  Changed ifdef ANSI to ifdef PROTO in function prototypes
 */

#ifndef CGMBEZ_H
#define CGMBEZ_H

#ifdef CGMBEZ_C
#define Extern
#else
#define Extern extern
#endif

#ifdef PROTO

     /*  The following routines are the only ones that are callable
      |  by the CGM system, so needs to obey the BEZxxx convention.
      */

Extern void BEZtext( Textitem *, struct textatt *, Point ,
                     void (*)(int, Point *, Tmatrix, int) ),
            BEZgetfd( Textitem *);

#else

Extern void BEZtext(),               /*  Output the text  */
            BEZgetfd();              /*  Text details query routine  */

#endif

#undef Extern

#endif
