/*   RAL-CGM Interpreter module:  @(#) cgminput.c  version 3.2
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
 * Description:  Various Utilities for CGM Input routines
 *
 * Maintenance Log:
 *  25 Sep 90 RTP  Original version created from cgminput.c
 *                 for routines needed by cgmlib
 *  12 Dec 90 RTP  Add DEBUG listing of CGM settings
 *  14 Dec 90 RTP  Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *  11 Feb 91 RTP  Add 'bytes needed' to ERR messages
 *  14 May 91 RTP  Add ANSI declarations
 *   2 Aug 91 RTP  Add SCCS id
 *  25 Sep 91 RTP  Remove default colour prec and extent setting
 *  12 Nov 92 RTP  Add check for default clip rectangle
 *                 Change argument to CGMialloc etc to include size
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 */

#define CGMINPUT_C
#include "cgmin.h"

extern struct char_defaults curchar;
extern struct text_defaults curtext;
extern struct bin_defaults curbin;

/**************************************************** CGMdefaults ******/

#ifdef PROTO
void CGMdefaults( Code c, Enum type )
#else
void CGMdefaults( c, type )

/*  Set default values for CGM structures */

Code c;
Enum type;
#endif
{

   switch (c)
   {
      case BEGMF:    /*  Set Metafile defaults */
            cur = commondef;
            mf = commondef;
            curatt = defatt;
            mfatt = defatt;
            switch ( type )
            {
               case BINARY:
                  mfibin = bindef;
                  curibin = bindef;
                  break;
               case NATIVE:
               case CHARACTER:
                  mfichar = chardef;
                  curichar = chardef;
                  break;
               case CLEAR_TEXT:
                  mfitext = textdef;
                  curitext = textdef;
                  break;
            }
            break;

      case MFDESC:   /*  First Picture store picture defaults */
            mf = cur;
            mfatt = curatt;
            switch ( type )
            {
               case BINARY:
                  mfibin = curibin;
                  break;
               case NATIVE:
               case CHARACTER:
                  mfichar = curichar;
                  break;
               case CLEAR_TEXT:
                  mfitext = curitext;
                  break;
            }
            break;

      case ENDPIC:   /*  restore Picture defaults  */
            cur = mf;
            curatt = mfatt;
            switch ( type )
            {
               case BINARY:
                  curibin = mfibin;
                  break;
               case NATIVE:
               case CHARACTER:
                  curichar = mfichar;
                  break;
               case CLEAR_TEXT:
                  curitext = mfitext;
                  break;
            }
            break;

      case VDCEXT:  /* Check VDC dependent defaults */
            if ( cur.vdc_type == REAL )
            {
               float xext, yext, size;

               xext = FABS(cur.vdc_extent.b.x.real - cur.vdc_extent.a.x.real);
               yext = FABS(cur.vdc_extent.b.y.real - cur.vdc_extent.a.y.real);
               size = ( xext > yext ? 0.01 * xext : 0.01 * yext );

               if ( !curatt.linewidth_flag )
                  curatt.line_width.real = ( cur.linewidth_mode == ABSOLUTE
                                           ? 0.1 * size : 1.0 );
               if ( !curatt.markersize_flag )
                  curatt.mark_size.real = ( cur.markersize_mode == ABSOLUTE
                                          ? size  : 1.0 );
               if ( !curatt.edgewidth_flag )
                  curatt.edge_width.real = ( cur.edgewidth_mode == ABSOLUTE
                                          ? 0.1 * size : 1.0 );
               if ( !curatt.charheight_flag )
                      curatt.char_height.real = size;
               if ( !curatt.cliprect_flag )
               {
                      cur.clip_rect.a.x.real = cur.vdc_extent.a.x.real;
                      cur.clip_rect.b.x.real = cur.vdc_extent.b.x.real;
                      cur.clip_rect.a.y.real = cur.vdc_extent.a.y.real;
                      cur.clip_rect.b.y.real = cur.vdc_extent.b.y.real;
               }
            }
            else
            {
               long xext, yext, size;

               xext = abs(cur.vdc_extent.b.x.intr - cur.vdc_extent.a.x.intr);
               yext = abs(cur.vdc_extent.b.y.intr - cur.vdc_extent.a.y.intr);
               size = ( xext > yext ? xext / 100 : yext / 100 );

               if ( !curatt.linewidth_flag )
                  curatt.line_width.intr = ( cur.linewidth_mode == ABSOLUTE
                                           ? size / 10 : 1 );
               if ( !curatt.markersize_flag )
                  curatt.mark_size.intr = ( cur.markersize_mode == ABSOLUTE
                                          ? size  : 1 );
               if ( !curatt.edgewidth_flag )
                  curatt.edge_width.intr = ( cur.edgewidth_mode == ABSOLUTE
                                          ? size / 10 : 1 );
               if ( !curatt.charheight_flag )
                      curatt.char_height.intr = size;
               if ( !curatt.cliprect_flag )
               {
                      cur.clip_rect.a.x.intr = cur.vdc_extent.a.x.intr;
                      cur.clip_rect.b.x.intr = cur.vdc_extent.b.x.intr;
                      cur.clip_rect.a.y.intr = cur.vdc_extent.a.y.intr;
                      cur.clip_rect.b.y.intr = cur.vdc_extent.b.y.intr;
               }
            }
            break;

      default:
            break;
   }
#ifdef DEBUG_ATT
   fprintf(stderr,"Current Defaults settings:\n");
   fprintf(stderr,"  Max colour index %d\n", cur.max_colind);
   fprintf(stderr,"  VDC type         %d\n", cur.vdc_type);
   fprintf(stderr,"  Transparency     %d\n", cur.transparency);
   fprintf(stderr,"  Clip indicator   %d\n", cur.clip_ind);
   fprintf(stderr,"  Colour Mode      %d\n", cur.color_mode);
   fprintf(stderr,"  Scale Mode       %d\n", cur.scale_mode);
   fprintf(stderr,"  Scale Factor     %f\n", cur.scale_factor);
   fprintf(stderr,"  Line Width Mode  %d\n", cur.linewidth_mode);
   fprintf(stderr,"  Marker Size Mode %d\n", cur.markersize_mode);
   fprintf(stderr,"  Edge Width Mode  %d\n", cur.edgewidth_mode);
   fprintf(stderr,"  Background Colour (%d %d %d)\n", cur.back.red,
                                   cur.back.green, cur.back.blue);
   fprintf(stderr,"  Auxilary Colour   (%d %d %d)\n", cur.aux.red,
                                   cur.aux.green, cur.aux.blue);
   if (cur.vdc_type == REAL)
      fprintf(stderr,"  VDC Extent        (%f,%f) (%f,%f)\n",
                      cur.vdc_extent.a.x.real, cur.vdc_extent.a.y.real,
                      cur.vdc_extent.b.x.real, cur.vdc_extent.b.y.real);
   else
      fprintf(stderr,"  VDC Extent        (%d,%d) (%d,%d)\n",
                      cur.vdc_extent.a.x.intr, cur.vdc_extent.a.y.intr,
                      cur.vdc_extent.b.x.intr, cur.vdc_extent.b.y.intr);
   if (cur.vdc_type == REAL)
      fprintf(stderr,"  Clip Rectangle    (%f,%f) (%f,%f)\n",
                      cur.clip_rect.a.x.real, cur.clip_rect.a.y.real,
                      cur.clip_rect.b.x.real, cur.clip_rect.b.y.real);
   else
      fprintf(stderr,"  Clip Rectangle    (%d,%d) (%d,%d)\n",
                      cur.clip_rect.a.x.intr, cur.clip_rect.a.y.intr,
                      cur.clip_rect.b.x.intr, cur.clip_rect.b.y.intr);

   fprintf(stderr,"\nCurrent Attribute Settings:\n");
   fprintf(stderr,"  Line index        %d\n", curatt.line_ind);
   fprintf(stderr,"  Line type         %d\n", curatt.line_type);
   fprintf(stderr,"  Line width        %f (%d)\n", curatt.line_width.real,
                                      curatt.line_width.intr);
   if ( cur.color_mode == DIRECT )
      fprintf(stderr,"  Line colour      (%d %d %d)\n", curatt.line.red,
                                   curatt.line.green, curatt.line.blue);
   else
      fprintf(stderr,"  Line colour       %d\n", curatt.line.index);

   fprintf(stderr,"  Marker index      %d\n", curatt.mark_ind);
   fprintf(stderr,"  Marker type       %d\n", curatt.mark_type);
   fprintf(stderr,"  Marker size       %f (%d)\n", curatt.mark_size.real,
                                      curatt.mark_size.intr);
   if ( cur.color_mode == DIRECT )
      fprintf(stderr,"  Marker colour    (%d %d %d)\n", curatt.marker.red,
                                   curatt.marker.green, curatt.marker.blue);
   else
      fprintf(stderr,"  Marker colour     %d\n", curatt.marker.index);

   fprintf(stderr,"  Text index        %d\n", curatt.text_ind);
   fprintf(stderr,"  Text font index   %d\n", curatt.text_font);
   fprintf(stderr,"  Text precision    %d\n", curatt.text_prec);
   fprintf(stderr,"  Text path         %d\n", curatt.text_path);
   fprintf(stderr,"  Character expansion %f\n", curatt.char_exp);
   fprintf(stderr,"  Character spacing   %f\n", curatt.char_space);
   if ( cur.color_mode == DIRECT )
      fprintf(stderr,"  Text colour      (%d %d %d)\n", curatt.text.red,
                                   curatt.text.green, curatt.text.blue);
   else
      fprintf(stderr,"  Text colour       %d\n", curatt.text.index);
   fprintf(stderr,"  Character height    %f (%d)\n",
             curatt.char_height.real, curatt.char_height.intr );
   if (cur.vdc_type == REAL)
      fprintf(stderr,"  Character orientation  (%f,%f) (%f,%f)\n",
                       curatt.char_up.x.real, curatt.char_up.y.real,
                       curatt.char_base.x.real, curatt.char_base.y.real);
   else
      fprintf(stderr,"  Character orientation  (%d,%d) (%d,%d)\n",
                       curatt.char_up.x.intr, curatt.char_up.y.intr,
                       curatt.char_base.x.intr, curatt.char_base.y.intr);
   fprintf(stderr,"  Text path         %d\n", curatt.text_path);
   fprintf(stderr,"  Text align        %d (%f) %d (%f)\n",
                      curatt.text_halign, curatt.text_hcont,
                      curatt.text_valign, curatt.text_vcont );
   fprintf(stderr,"  Character set     %d\n", curatt.char_set);
   fprintf(stderr,"  Alternate character set %d\n", curatt.altchar_set);

   fprintf(stderr,"  Fill index        %d\n", curatt.fill_ind);
   fprintf(stderr,"  Interior style    %d\n", curatt.int_style);
   fprintf(stderr,"  Hatch index       %d\n", curatt.hatch_ind);
   fprintf(stderr,"  Pattern index     %d\n", curatt.pat_ind);
   if (cur.vdc_type == REAL)
   {
      fprintf(stderr,"  Fill Reference Point  (%f,%f)\n",
               curatt.fill_ref.x.real, curatt.fill_ref.y.real);
      fprintf(stderr,"  Pattern Size      (%f,%f) (%f,%f)\n",
               curatt.pat_size.a.x.real, curatt.pat_size.a.y.real,
               curatt.pat_size.b.x.real, curatt.pat_size.b.y.real);
   }
   else
   {
      fprintf(stderr,"  Fill Reference Point  (%d,%d)\n",
                         curatt.fill_ref.x.intr, curatt.fill_ref.y.intr);
      fprintf(stderr,"  Pattern Size      (%d,%d) (%d,%d)\n",
               curatt.pat_size.a.x.intr, curatt.pat_size.a.y.intr,
               curatt.pat_size.b.x.intr, curatt.pat_size.b.y.intr);
   }
   if ( cur.color_mode == DIRECT )
      fprintf(stderr,"  Fill colour      (%d %d %d)\n", curatt.fill.red,
                                   curatt.fill.green, curatt.fill.blue);
   else
      fprintf(stderr,"  Fill colour       %d\n", curatt.fill.index);

   fprintf(stderr,"  Edge index        %d\n", curatt.edge_ind);
   fprintf(stderr,"  Edge type         %d\n", curatt.edge_type);
   fprintf(stderr,"  Edge width        %f (%d)\n", curatt.edge_width.real,
                                      curatt.edge_width.intr);
   if ( cur.color_mode == DIRECT )
      fprintf(stderr,"  Edge colour      (%d %d %d)\n", curatt.edge.red,
                                   curatt.edge.green, curatt.edge.blue);
   else
      fprintf(stderr,"  Edge colour       %d\n", curatt.edge.index);

   {
      int i;

      fprintf(stderr,"  ASFs ");
      for ( i=0; i<ASFS; i++) fprintf(stderr," %d", curatt.asf[i]);
      fprintf(stderr,"\n");
   }
#endif
   return;

}

/****************************************************** CGMsetasf ******/

#ifdef PROTO
void CGMsetasf( long *pi )
#else
void CGMsetasf( pi )
long *pi;
#endif
{
   long n, num;
   Int type, value, k, l;

   num = *pi++;
   for ( n = ZERO; n < num ; n++ )
   {
      type = *pi++;
      value = *pi++;
      if ( type < ASFS )  curatt.asf[ type ] = value;
      else
      {
         switch(type)
         {
            case 506:     /*  all edge  */
               k = 15;
               l = 17;
               break;
            case 507:     /*  all fill  */
               k = 11;
               l = 14;
               break;
            case 508:    /*  all text  */
               k = 6;
               l = 10;
               break;
            case 509:    /*  all marker  */
               k = 3;
               l = 5;
               break;
            case 510:    /*  all line  */
               k = ZERO;
               l = 2;
               break;
            case 511:    /*  all  */
               k = ZERO;
               l = 17;
               break;
            default:    /* no action  */
               k = 1;
               l = ZERO;
               break;
         }

/* Set Current values */

         for (; k <= l ; curatt.asf[k++] = value)
         ;
      }
   }
   return;
}


#ifdef VAR_BUFFER
/****************************************************** CGMialloc ******/

#ifdef PROTO
long *CGMialloc( long *pi, long size)
#else
long *CGMialloc( pi, size )
long *pi;
long size;
#endif
{
   long pt = pi - pint;
   long buffsize;
   char mess[80];

   buffsize = pimax - pint;
   if ( size && (size+pt) > buffsize ) buffsize = size + pt;
   else buffsize += ARRAY_MAX;
   pint = (long *) realloc( (char*)pint, (size_t)buffsize*sizeof(long) );
   if ( pint == NULL )
   {
      sprintf(mess,"%ud bytes needed", buffsize*sizeof(long) );
      exit( CGMerror ( "CGMialloc", ERR_NOMEMORY, FATAL, mess) );
   }
   pimax = pint + buffsize;
#ifdef DEBUG
   DMESS "New Integer Buffer: %x to %x %d bytes\n", pint, pimax, buffsize);
#endif

   return (pint + pt);
}
/****************************************************** CGMialloc ******/

#ifdef PROTO
float *CGMralloc( float *pr, long size)
#else
float *CGMralloc( pr, size )
float *pr;
long size;
#endif
{
   long pt = pr - preal, buffsize;
   char mess[80];

   buffsize = prmax - preal;
   if ( size && (size+pt) > buffsize ) buffsize = size + pt;
   else buffsize += ARRAY_MAX;
   preal = (float *) realloc( (char*)preal, (size_t)buffsize*sizeof(float) );
   if ( preal == NULL )
   {
      sprintf(mess,"%ud bytes needed", buffsize*sizeof(float) );
      exit ( CGMerror ( "CGMralloc", ERR_NOMEMORY, FATAL, mess) );
   }
   prmax = preal + buffsize;
#ifdef DEBUG
   DMESS "New Real Buffer: %x %ud bytes\n", preal, buffsize);
#endif

   return (preal + pt);
}
/****************************************************** CGMsalloc ******/

#ifdef PROTO
char *CGMsalloc( char *ps, long size )
#else
char *CGMsalloc( ps, size)
char *ps;
long size;
#endif
{
   long pt = ps - str, buffsize;
   char mess[80];

   buffsize = strmax - str;
   if ( size && (size+pt) > buffsize ) buffsize = size + pt;
   else buffsize += ARRAY_MAX;
   str = (char *) realloc( (char*) str, (size_t)buffsize * sizeof(char) );
   if ( str == NULL )
   {
      sprintf(mess,"%ud bytes needed", buffsize* sizeof(char) );
      exit ( CGMerror ( "CGMsalloc", ERR_NOMEMORY, FATAL, NULLSTR) );
   }
   strmax = str + buffsize;
#ifdef DEBUG
   DMESS "New String Buffer: %x %ud bytes\n", str, buffsize);
#endif

   return (str + pt);
}
#endif
