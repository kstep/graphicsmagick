/*   RAL-CGM Interpreter module:  @(#) cgmatt.c  version 3.1
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied,distributed, altered or used, but not sold
 * for profit or incorporated into any product except under licence from
 * the authors.
 *
 * It is not in the public domain.
 *
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the authors should be marked as
 * such.
 */

/*
 *   Author:     C D Osland (RAL)
 *   Date:       23 - 25 August 1990
 *
 * Modification log :
 *
 *  12 Oct 90 RTP  Remove colour table entries add Bundled RGB values
 *  17 Dec 90 RTP  Add check for valid line, edge and marker types
 *   5 Apr 91 AMR  Add checks for vdc type when setting line and marker widths.
 *  14 May 91 RTP  Add ANSI declarations and function prototypes
 *  19 Jun 91 RTP  Add header file cgmatt.h to include function declarations
 *   2 Aug 91 RTP  Add SCCS id
 */

/*
 *   This module contains utilities that copies the current attributes
 *   for each of the CGM pipelines (line, marker, text, fill, edge)
 *   into their individual data structures.  The aspect source flags
 *   VDC type and specification mode flags are used to determine which
 *   values are copied.
 *
 *   The module provides the following entry points:
 *
 *   ATTline(int, Linebundle[],
 *           int, Lineatt *)
 *
 *           processes line attributes
 *
 *   ATTmarker(int, Markerbundle[],
 *             int, Markeratt *)
 *
 *           processes marker attributes
 *
 *   ATTtext(int, Textbundle[],
 *           int, Textatt *)
 *
 *           processes text attributes
 *
 *   ATTfill(int, Fillbundle[],
 *           int, Fillatt *)
 *
 *           processes fill attributes except for edge attributes
 *
 *   ATTedge(int, Edgebundle[],
 *           int, Edgeatt *)
 *
 *           processes edge attributes
 */


#define CGMATT_C

#include "cgmout.h"
#include "cgmatt.h"

#define SAFEB(index) (index < 1 ? 0 : index > nb ? 0 : index - 1)
#define SAFEC(index) (index < 0 ? 1 : index > nc ? 0 : index)
#define COLRFACT( x, col ) ( (Posint) (x.col * \
           (Float)(cur.max_rgb.col-cur.min_rgb.col) + cur.min_rgb.col ) )

/* Internal static routine */
#ifdef PROTO
   static void set_shared(int, Sharedatt *);
#else
   static void set_shared();
#endif

/***************************************************** ATTline *********/
#ifdef PROTO
void ATTline( int nb, Linebundle *lb, int nc, Lineatt *la)
#else
void ATTline( nb, lb, nc, la )

int nb, nc;
Linebundle *lb;
Lineatt *la;
#endif

{
   Index index = SAFEB(curatt.line_ind);

   if (curatt.asf[0] == BUNDLED)
      la->type = lb[index].type;
   else
      la->type = curatt.line_type;
   if ( la->type <= 0 || la->type > 5 ) la->type = 1;

   if (curatt.asf[1] == BUNDLED)
      la->width = lb[index].width;
   else
      if (cur.vdc_type == REAL || cur.linewidth_mode == SCALED)
        la->width = curatt.line_width.real;
      else
        la->width = curatt.line_width.intr;

   la->widthmode = cur.linewidth_mode;

   if (curatt.asf[2] == BUNDLED)
      if (cur.color_mode == INDEXED)
         la->colindex = lb[index].colindex;
      else
      {
         la->col.red   = COLRFACT( lb[index].col, red );
         la->col.green = COLRFACT( lb[index].col, green );
         la->col.blue  = COLRFACT( lb[index].col, blue );
      }
   else
      if (cur.color_mode == INDEXED)
         la->colindex = SAFEC(curatt.line.index);
      else
      {
         la->col.red   = curatt.line.red;
         la->col.green = curatt.line.green;
         la->col.blue  = curatt.line.blue;
      }

   set_shared(nc, la->shared);

   return;
}

/***************************************************** ATTmarker *******/
#ifdef PROTO
void ATTmarker( int nb, Markerbundle *mb, int nc, Markeratt *ma)
#else
void ATTmarker( nb, mb, nc, ma )

int nb, nc;
Markerbundle *mb;
Markeratt *ma;
#endif

{
   Index index = SAFEB(curatt.mark_ind);

   if (curatt.asf[3] == BUNDLED)
      ma->type = mb[index].type;
   else
      ma->type = curatt.mark_type;
   if ( ma->type <= 0 || ma->type > 5 ) ma->type = 3;

   if (curatt.asf[4] == BUNDLED)
      ma->size = mb[index].size;
   else
      if (cur.vdc_type == REAL || cur.markersize_mode == SCALED)
        ma->size = curatt.mark_size.real;
      else
        ma->size = curatt.mark_size.intr;

   ma->sizemode =  cur.markersize_mode;

   if (curatt.asf[5] == BUNDLED)
      if (cur.color_mode == INDEXED)
         ma->colindex = mb[index].colindex;
      else
      {
         ma->col.red   = COLRFACT( mb[index].col, red );
         ma->col.green = COLRFACT( mb[index].col, green );
         ma->col.blue  = COLRFACT( mb[index].col, blue );
      }
   else
      if (cur.color_mode == INDEXED)
         ma->colindex = SAFEC(curatt.marker.index);
      else
      {
         ma->col.red   = curatt.marker.red;
         ma->col.green = curatt.marker.green;
         ma->col.blue  = curatt.marker.blue;
      }

   set_shared(nc, ma->shared);

   return;
}

/***************************************************** ATTtext *********/
#ifdef PROTO
void ATTtext( int nb, Textbundle *tb, int nc, Textatt *ta)
#else
void ATTtext( nb, tb, nc, ta )

int nb, nc;
Textbundle *tb;
Textatt *ta;
#endif

{
   Index index = SAFEB(curatt.text_ind);

   if (curatt.asf[6] == BUNDLED)
      ta->fontindex = tb[index].font;
   else
      ta->fontindex = curatt.text_font;

   if (curatt.asf[7] == BUNDLED)
      ta->precision = tb[index].precision;
   else
      ta->precision = curatt.text_prec;

   if (curatt.asf[8] == BUNDLED)
      ta->expansion = tb[index].expansion;
   else
      ta->expansion = curatt.char_exp;

   if (curatt.asf[9] == BUNDLED)
      ta->spacing = tb[index].spacing;
   else
      ta->spacing = curatt.char_space;

   if (curatt.asf[10] == BUNDLED)
      if (cur.color_mode == INDEXED)
         ta->colindex = tb[index].colindex;
      else
      {
         ta->col.red   = COLRFACT( tb[index].col, red );
         ta->col.green = COLRFACT( tb[index].col, green );
         ta->col.blue  = COLRFACT( tb[index].col, blue );
      }
   else
      if (cur.color_mode == INDEXED)
         ta->colindex = SAFEC(curatt.text.index);
      else
      {
         ta->col.red   = curatt.text.red;
         ta->col.green = curatt.text.green;
         ta->col.blue  = curatt.text.blue;
      }


   ta->halign = curatt.text_halign;
   ta->valign = curatt.text_valign;
   ta->hcont = curatt.text_hcont;
   ta->vcont = curatt.text_vcont;

   ta->path = curatt.text_path;

   if (cur.vdc_type == REAL)
   {
      ta->xbase = curatt.char_base.y.real;
      ta->ybase = curatt.char_base.y.real;
      ta->xup = curatt.char_up.x.real;
      ta->yup = curatt.char_up.y.real;
      ta->height = curatt.char_height.real;
   }
   else
   {
      ta->xbase = curatt.char_base.y.intr;
      ta->ybase = curatt.char_base.y.intr;
      ta->xup = curatt.char_up.x.intr;
      ta->yup = curatt.char_up.y.intr;
      ta->height = curatt.char_height.intr;
   }

   set_shared(nc, ta->shared);
   return;
}

/***************************************************** ATTfill *********/
#ifdef PROTO
void ATTfill( int nb, Fillbundle *fb, int nc, Fillatt *fa )
#else
void ATTfill( nb, fb, nc, fa )

int nb, nc;
Fillbundle *fb;
Fillatt *fa;
#endif

{
   Index index = SAFEB(curatt.fill_ind);

   if (curatt.asf[11] == BUNDLED)
      fa->style = fb[index].style;
   else
      fa->style = curatt.int_style;

   if (curatt.asf[12] == BUNDLED)
      if (cur.color_mode == INDEXED)
         fa->colindex = fb[index].colindex;
      else
      {
         fa->col.red   = COLRFACT( fb[index].col, red );
         fa->col.green = COLRFACT( fb[index].col, green );
         fa->col.blue  = COLRFACT( fb[index].col, blue );
      }
   else
      if (cur.color_mode == INDEXED)
         fa->colindex = SAFEC(curatt.fill.index);
      else
      {
         fa->col.red   = curatt.fill.red;
         fa->col.green = curatt.fill.green;
         fa->col.blue  = curatt.fill.blue;
      }

   if (curatt.asf[14] == BUNDLED)
      fa->pattern = fb[index].pattern;
   else
      fa->pattern = curatt.pat_ind;

   if (curatt.asf[13] == BUNDLED)
      fa->hatch = fb[index].hatch;
   else
      fa->hatch = curatt.hatch_ind;

   set_shared(nc, fa->shared);

   return;
}

/***************************************************** ATTedge *********/
#ifdef PROTO
void ATTedge( int nb, Edgebundle *eb, int nc, Edgeatt *ea)
#else
void ATTedge( nb, eb, nc, ea )

/**********************
*   AMR - Should vdc type be checked before setting edge width when
*         width individual ??????
**********************/

int nb, nc;
Edgebundle *eb;
Edgeatt *ea;
#endif

{
   Index index = SAFEB(curatt.edge_ind);

   if (curatt.asf[15] == BUNDLED)
      ea->type = eb[index].type;
   else
      ea->type = curatt.edge_type;
   if ( ea->type <= 0 || ea->type > 5 ) ea->type = 1;

   if (curatt.asf[16] == BUNDLED)
      ea->width = eb[index].width;
   else
      if (cur.vdc_type == REAL || cur.edgewidth_mode == SCALED)
        ea->width = curatt.edge_width.real;
      else
        ea->width = curatt.edge_width.intr;

   ea->widthmode = cur.edgewidth_mode;

   if (curatt.asf[17] == BUNDLED)
      if (cur.color_mode == INDEXED)
         ea->colindex = eb[index].colindex;
      else
      {
         ea->col.red   = COLRFACT( eb[index].col, red );
         ea->col.green = COLRFACT( eb[index].col, green );
         ea->col.blue  = COLRFACT( eb[index].col, blue );
      }
   else
      if (cur.color_mode == INDEXED)
         ea->colindex = SAFEC(curatt.edge.index);
      else
      {
         ea->col.red   = curatt.edge.red;
         ea->col.green = curatt.edge.green;
         ea->col.blue  = curatt.edge.blue;
      }

   set_shared(nc, ea->shared);

   return;
}

/***************************************************** set_shared ******/
#ifdef PROTO
static void set_shared( int nc, Sharedatt *ps )
#else
static void set_shared( nc, ps )

int nc;
Sharedatt *ps;
#endif

{
   if (cur.color_mode == INDEXED)
      ps->auxindex = SAFEC(cur.aux.index);
   else
   {
      ps->aux.red   = cur.aux.red;
      ps->aux.green = cur.aux.green;
      ps->aux.blue  = cur.aux.blue;
   }

   ps->transparency = cur.transparency;
   ps->colmode = cur.color_mode;

   return;
}
