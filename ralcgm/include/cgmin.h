/*  RAL-CGM Interpreter header file: @(#) cgmin.h  version 3.3
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
 * Description: CGM input module external structures and variables
 *
 * Modification log:
 *
 *   2 Aug 91 RTP  Add SCCS id
 *  12 Aug 94 KEVP Add cgmfinished
 *   1 Mar 95 KEVP Add cgmoverrun
 *  22 Mar 95 KEVP Add cgmquiet
 */

#ifndef CGMIN_H
#define CGMIN_H

/*  Include all files on which this is dependent.  */

#include "cgmmach.h"
#include "cgmtypes.h"
#include "cgmstruc.h"

/*  Include file that all modules using this file will need.  */

#include "cgmerr.h"

/*  external variables needed by reader   */

  extern const struct char_defaults chardef;
  extern struct char_defaults mfichar, curichar;
  extern const struct text_defaults textdef;
  extern struct text_defaults mfitext, curitext;
  extern const struct bin_defaults bindef;
  extern struct bin_defaults  mfibin, curibin;
  extern const struct defaults commondef;
  extern struct defaults mf, cur;
  extern const struct attributes defatt;
  extern struct attributes mfatt, curatt;

  extern char *cgmversion;
  extern Enum cgmverno;
  extern Enum cgmstate, cgmprof;
  extern Logical cgmcharsub, cgmterm, cgmlist, cgmEOF, cgmfinished,
                 cgmverbose, cgmquiet;
  extern Logical cgmralgks, cgmralbin;

#ifdef EBCDIC
  extern Logical cgminnative;
  extern char cgmascii[], cgmebcdic[];
#endif

#ifdef FIXED_BUFFER
  extern long pint[], *pimax;
  extern float preal[], *prmax;
  extern char str[], *strmax;
#else
  extern long *pint, *pimax;
  extern float *preal, *prmax;
  extern char *str, *strmax;
#endif

#ifdef CGMINPUT_C
#define Extern
#else
#define Extern extern
#endif

/*  Reallocation of Variable buffer space */

#ifdef PROTO
  Extern long  *CGMialloc( long*, long );
  Extern float *CGMralloc( float*, long );
  Extern char  *CGMsalloc( char*, long );
#else
  Extern long  *CGMialloc( );
  Extern float *CGMralloc( );
  Extern char  *CGMsalloc( );
#endif

/*  Random Access Function  */

#ifndef CGMLIB
#ifdef PROTO
  extern Code CGMframe( Code );
#ifdef VMS
  extern void CGMvaxst();
#endif
#else
  extern Code CGMframe();
#ifdef VMS
  extern void CGMvaxst();
#endif
#endif
#endif /* Not for CGMLIB */

#ifdef PROTO
/*  Function to set Current ASF values for all encodings */

  Extern void CGMsetasf( long * );

/*  Set structure values to defaults */

  Extern void CGMdefaults( Code, Enum );

  extern FILE *CGMfopen( char*, char, Enum);

#else

  Extern void CGMsetasf();
  Extern void CGMdefaults();
  extern FILE *CGMfopen();
#endif

  extern long *null;

  extern Code st_start, st_term;

/*  Variables for random frame access    */

  extern Logical cgmrandom;
  extern long cgmnext, cgmpresent, cgmabort, cgmoverrun;

#ifdef FONTSYSTEM
/*  Structures for Font list and character set list */

  extern struct cgmfont cgmfonts;
  extern struct cgmcset cgmcsets;

/*  Font storing routine */

#ifdef PROTO
  extern void FNTflist ( long *, long *, char *, struct cgmfont *);
  extern void FNTclist ( long *, long *, char *, struct cgmcset *);
#else
  extern void FNTflist ( );
  extern void FNTclist ( );
#endif
#endif

#undef Extern
#endif  /*  end of cgmin.h */
