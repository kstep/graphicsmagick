/*  RAL-CGM Interpreter header file: @(#) cgmfile.h  version 3.1
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
 * R T Platon @ Rutherford Appleton Laboratory (rtp@uk.ac.rl.ib)
 *
 * Description:  CGM Random access external functions
 *
 * Modification log:
 *
 *   2 Aug 91 RTP  Add SCCS id
 */

#ifndef CGMFILE_H
#define CGMFILE_H

#ifdef CGMFILE_C
#define Extern
#else
#define Extern extern
#endif

#ifdef PROTO

/* Work out Data path name */
   Extern char *CGMpath ( char*, char* );

/* Open a file */
   Extern FILE *CGMfopen( char*, char, Enum );

/* The three following routines when referring to a file, mean the path,
   filename and extension parts of the file. */

/* Modify the extension part of a file */
   Extern void CGMmodfextn( char*, char* );

/* Modify the filename part of a file */
   Extern void CGMmodfname( char*, char* );

/* Finds the position and length of the filename within the file string */

   Extern unsigned short CGMgetfname(char *fname, char **fnptr);

#else
   Extern char *CGMpath ();
   Extern FILE *CGMfopen();
   Extern void CGMmodfextn();
   Extern void CGMmodfname();
   Extern unsigned short CGMgetfname();
#endif

#undef Extern
#endif /* CGMFILEH */
