/*  RAL-CGM Interpreter header file: @(#) cgmpts.h  version 3.2
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the author should be marked as
 * such.
 *
 * K E V Palmen @ Rutherford Appleton Laboratory (kevp@uk.ac.rl.ib)
 *
 * Description:  CGM Points Handling Utilities
 *
 * Modification log:
 *
 *   3 Sep 92 KEVP Created
 *  28 Sep 92 KEVP Added PTSdefclip to set default clipping rectangle.
 *  29 Sep 92 KEVP Added PTSvdcextget to get VDC extent.
 *  16 Nov 92 KEVP Added PTSlist and PTSfree to handle stored points.
 *                 Moved include files to cgmpts.c
 */

#ifndef CGMPTS_H
#define CGMPTS_H

/*  Include all files on which this is dependent.  */
#include "cgmtypes.h"
#include "cgmpar.h"

#ifdef CGMPTS_C
#define Extern
#else
#define Extern extern
#endif

#ifdef PROTO
Extern void
/* Get clipping rectangle */
       PTSclipget( Enum *, Rect *),

/* Set default clipping rectangle */
       PTSdefclip ( void ),

/* Free stored points made by PTSlist */
       PTSfree ( void ),

/* Get points from driver arguments */
       PTSget( Long , Long *, Float *, Point *, Enum * ),

/* Clip and Output a polyline using device line drawing function */
       PTSlineclip( Long , Point *, Rect ,
                    void (*)(Long, Point *) ),  /* linedraw function */

/* Clip and Output a polymarker using device line drawing function */
       PTSmarker ( Long , Point *, Float, Markeratt *, Enum,
                    void (*)(Long, Point *) ),  /* linedraw function */

/* Get VDC extent */
       PTSvdcextget (Rect * );

Extern Point*
/* Get points from arguments, store them and return pointer to store */
       PTSlist( Long, Long *, Float *, Enum **);
#else
Extern void PTSclipget(), PTSdefclip(), PTSfree(), PTSget(),
            PTSlineclip(), PTSmarker(), PTSvdcextget();

Extern Point* PTSlist();
#endif

#undef Extern
#endif /* CGMPTS_H */
