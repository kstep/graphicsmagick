/*   RAL-CGM Interpreter header file:  @(#) cgmufont.h  version 3.1
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
 * Description: structures and variables used by the font system.
 *
 * Modification log:
 *
 *  10 Sep 91 CDS  Split of  this utility stuff from cgmfont.h
 */

#ifndef CGMUFONT_H
#define CGMUFONT_H

     /*  Include the file that is needed before this one.  */

#include "cgmtext.h"
#include "cgmfont.h"

     /*  Declarations for where things are held.  */

#define ONDISK FALSE
#define INMEMORY TRUE

#define CSETINFO INMEMORY
#define FONTINFO INMEMORY

#define CSETNAMES

#define DESNAMES

#define SRCNAMES

     /*  Declare the possible values for the posture value  */

enum f_postures
   {
     P_NA,                            /* not applicable  */
     P_UPRIGHT,
     P_OBLIQUE,
     P_BSOBLIQUE,
     P_ITALIC,
     P_BSITALIC,
     P_OTHER
   };

     /*  Declare the possible values for the weight value  */

enum f_weights
   {
     W_NA,
     W_ULTRALIGHT,
     W_EXTRALIGHT,
     W_LIGHT,
     W_SEMILIGHT,
     W_MEDIUM,
     W_SEMIBOLD,
     W_BOLD,
     W_EXTRABOLD,
     W_ULTRABOLD
   };

     /*  Declare the possible values for the propwidth value  */

enum f_propwidths
   {
     X_NA,
     X_ULTRACONDENSED,
     X_EXTRACONDENSED,
     X_CONDENSED,
     X_SEMICONDENSED,
     X_MEDIUM,
     X_SEMIEXPANDED,
     X_EXPANDED,
     X_EXTRAEXPANDED,
     X_ULTRAEXPANDED
   };

     /*  Declare the possible values for the structure value  */

enum f_structures
   {
     S_NA,
     S_SOLID,
     S_OUTLINE
   };

     /*  Data held about each ISO character set, as read from the
         "isosets.d" data file.
     */

struct isocharset
{
     int registration,            /*  ISO registration number  */
         sublow, subhigh,         /*  lower and upper limits of subcode  */
         setsize;                 /*  size of character set (94 or 96) */
     char escseq[6];              /*  escape sequence, eg "4/2"  */
     int fudge;                   /*  gross character set classification  */
#ifdef CSETNAMES
     char *name;                  /*  optional name  */
#endif
   };

struct fudgecset
   {
     int number;
     char *name;
   };

     /*  Data structures that finally build up into an ISO font definition */

     /*  First the ISO standard format for a date.  */

struct iso8824
   {
     int year, month, day;
   };

     /*  Now the data version  */

struct isodataversion
   {
     int major, minor;
     struct iso8824 timestamp;
   };

     /*  Now the font design group values  */

struct isodsngroup
   {
     int class, subclass, group;
   };

     /*  Now the variants attributes that group together  */

struct isovariant
   {
     enum f_postures posture;
     enum f_weights weight;
     enum f_propwidths propwidth;
     enum f_structures structure;
   };

     /*  Now the full font description  */

struct isofont
   {
     char *fontname;
     Index fontid;
     struct isodataversion dataversion;
     int standardversion;
     struct source *dsnsource;
     char *fontfamily;
     int familyid;
     struct isovariant variant;
     struct isodsngroup dsngroup;
     int glyphcount, *glyphs;
     Index cset;
     long dsnsize, minsize, maxsize;
   };

     /*  Description of a font family  */

struct family
   {
     char *name;
     int number;
     struct isodsngroup dsngroup;
   };

struct alias
   {
     char *name;
     struct family *realfamily;
   };

struct font
   {
     char *fontname;
     int fontid;
     int familyid;
     struct isovariant variant;
     struct isodsngroup dsngroup;
     Index cset;
   };

struct source
   {
     char *name;
     int number;
   };

struct design
   {
     char *classname, *subclassname, *groupname;
     struct isodsngroup dsngroup;
   };

struct modifier
   {
     int value;
     char *name;
   };

struct xfnames
   {
      struct xfnames *next;    /* pointer to next name */
      struct xfnames *prev;    /* pointer to previous name */
      struct isofont *isobits; /* isofont info for this font */
   };
typedef struct xfnames FNTXnames;

struct xfont_list
   {
      char *gen_name;              /* generic font name */
      int  num_siz;                /* number of sizes for font */
      FNTXnames *names;            /* list of font names */
      struct xfont_list *pnext;     /* pointer to next generic font */
   };
typedef struct xfont_list FNTXfont_list;

#endif /* End of CGMUFONT_H */
