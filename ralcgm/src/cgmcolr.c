/*   RAL-CGM Interpreter module:  @(#) cgmcolr.c  version 3.4
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
 * K E V palmen @ Rutherford Appleton Laboratory (kevp@uk.ac.rl.ib)
 *
 * Description:
 *
 * Maintenance Log:
 *   3 Sep 92 KEVP Original version
 *   2 Nov 92 RTP  Make new entry check conditional, correct realloc
 *                 for colour_defd and correct non-PROTO argument
 *  14 Jul 93 KEVP Modified for Metafile Defaults Replacement.
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *    2 Sep 94 KEVP Set colour_table to NULL when freed
 *    5 Oct 94 KEVP Free entire colour table if cgmfinished.
 */

#define CGMCOLR_C

#include "cgmout.h"
#include "cgmcolr.h"

/* statics for CGM Colour table */
typedef struct { Index ind; RGBcolour rgb; } Colrtab;
static Colrtab *colour_table = NULL;  /* colour table (start) */
static Index ncol_max;       /* length of colour table */
static Index ncol_fixed;     /* number of fixed entries */
static Index ncol_defined;   /* number of defined entries */
static Index ncol_ignore;    /* limit of search done by COLRtab */

#define INITLEN 64
/****************************************************** COLRput *****/

#ifdef PROTO
void COLRput ( Long nc, Long *pi )
#else
void COLRput ( nc, pi )

/*   put 'nc' colours starting at 'first' into the CGM colour table */

Long nc;              /* Number of Colours being put in */
Long *pi;             /* Index of first colour and RGB values */
#endif


{
   Index first,   /* Index of first colour */
         i,       /* Current colour index in loop */
         j;       /* Loop index, not used inside loop */
   Long newlen = 0;  /* New value for Colour table length */
   Long ncshort;     /* Minimum lengthening of colour table */
   Logical replace=FALSE; /* Replacing previous entries of same index */
   RGBcolour *col; /* Current RGB colour */

   first = *pi++;
#ifdef DEBUG
   DMESS "Set colour table: %d\n", nc);
#endif
/* Ensure that colour table is able receive entries */
   if(colour_table == NULL)
   {  /* Colour table needs initialising */
      if(nc > INITLEN)
      {
        newlen = nc;
      }
      else
      {
        newlen = INITLEN;
      }
      colour_table = (Colrtab *)MALLOC(newlen,sizeof(Colrtab));
      ncol_fixed   = 0L;
      ncol_defined = 0L;
      ncol_max = newlen;
      ncol_ignore = 0L;
   }
   else
   {
      if(ncol_defined + nc > ncol_max)
      { /* Entries won't fit in vacant spaces */
         ncol_ignore = ncol_fixed;
         ncshort = 0;
         for(i=first; i<first+nc; i++)
         { /* Count new entries that can not replace previous */
            if(COLRtab(i) == NULL) ncshort++;
         }
         ncol_ignore = 0L;
         if(ncol_defined + ncshort > ncol_max)
         { /* Longer colour table needed - length multiple of INITLEN */
            newlen = nc + ncol_defined;
            newlen = INITLEN*((newlen-1)/INITLEN + 1);
            colour_table = (Colrtab *)realloc((void *)colour_table,
                              (size_t)(newlen*sizeof(Colrtab)));
            ncol_max = newlen;
         }
         else
         {  /* We replace last preceeding entry of same index */
            replace = TRUE;
         }

#ifdef DEBUG
   DMESS "Colour table extended to: %d\n", newlen);
#endif
      }
   }
   if(colour_table == NULL)  /* Check that colour table is OK.*/
   {
      (void) CGMerror ("COLRput", ERR_NOMEMORY, ERROR, NULLSTR);
      return;
   }

/* Set new entries */
   if(replace)
   { /* We replace last entry of same index with new */
      ncol_ignore = ncol_fixed;
      for(i=first; i<first+nc; i++)
      {
         col = COLRtab(i);
         if(col != NULL)
         {
            col->red   = (Posint)*pi++;
            col->green = (Posint)*pi++;
            col->blue  = (Posint)*pi++;
         }
         else
         {  /* No changeable Entry of Current Index Found */
            colour_table[ncol_defined].ind = i;
            colour_table[ncol_defined].rgb.red   = (Posint)*pi++;
            colour_table[ncol_defined].rgb.green = (Posint)*pi++;
            colour_table[ncol_defined].rgb.blue  = (Posint)*pi++;
            ncol_defined++;
         }
      }
      ncol_ignore = 0L;
   }
   else
   {  /* We add all new entries to end */
      for(i=first; i<nc+first; i++)
      {
         colour_table[ncol_defined].ind = i;
         colour_table[ncol_defined].rgb.red   = (Posint)*pi++;
         colour_table[ncol_defined].rgb.green = (Posint)*pi++;
         colour_table[ncol_defined].rgb.blue  = (Posint)*pi++;
         ncol_defined++;
      }
   }

   if(cgmstate == MF_DEFAULTS)
   {  /* We are in Metafile Defaults Replacement: Fix the entries */
      ncol_fixed = ncol_defined;
   }

   return;
}
/****************************************************** COLRtab ****/

#ifdef PROTO
RGBcolour *COLRtab ( Index index )
#else
RGBcolour *COLRtab ( index )

/*   returns pointer to the last 'index' entry of the CGM colour table,
     if that colour has been defined, else returns NULL  */

Index index;          /* Colour Index */
#endif


{
    Index itab;  /* Place in Colour Table */

    if(colour_table == NULL) return NULL;

    for(itab=ncol_defined-1; itab>=ncol_ignore; itab--)
    {
       if(index == colour_table[itab].ind)
       {
          return &(colour_table[itab].rgb);
       }
    }
    return NULL;
}
/****************************************************** COLRfree *****/

#ifdef PROTO
void COLRfree( void )
#else
void COLRfree()

/*   free space allocated in the CGM colour table
     no longer needed, because picture or metafile has ended */

#endif


{
   if(colour_table == NULL) return;

   switch (cgmstate)
   {
       case PIC_CLOSED:
          if(cgmfinished) ncol_fixed = 0; /* Unfix colours, if at end of cgm */
          ncol_defined = ncol_fixed;   /* Free changeable colours */
          if(ncol_defined == 0L)
          {
               FREE(colour_table);
               colour_table = NULL;
          }
          break;
       case MF_CLOSED:
          FREE(colour_table);      /* Free the whole colour table */
          colour_table = NULL;
          break;
       default:
          break;
   }
   return;
}
