/*   RAL-CGM Interpreter module:  @(#) cgmohpgl.c  version 3.3
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
 * C D Seelig @ Rutherford Appleton Laboratory (cds@uk.ac.rl.ib)
 *
 * Description:  Output driver for HPGL plotter files
 *
 * Author:  C D Seelig
 *
 * Arguments:     stream   FILE structure for output file
 *
 *                c        CGM element code (character form)
 *
 *                *pi      Pointer to Integer Array
 *
 *                *pr      Pointer to Real Array
 *
 *                *str     Pointer to string
 *
 * Maintenance Log:
 *    23/07/93 KEVP  Free colour table at ENDMF too (in HPGLclose)
 *
 *
 * Limitations+Improvements:
 *
 * Something funny with GDPs
 * Get file handling sorted out.
 * Something should be done about differing line widths
 * Add pen width correction to cellarray
 * Should handle more primitives with HPGL rather than utilities
 * Maybe make cellarray relative
 *
 * DONE (probably)
 * Needs restricted text putting in.
 * Pen selection needs looking at (sometimes draws with no pen)
 * Bits for A3/A4 selection need finishing
 * Scaling is a bit up the creak
 * Check colour matching
 * Check colour matching (again)
 * Look at scaling of markers
 * For fill have fill att if edges invis but edge att if edges vis
 * Have a look at clipping
 * Should add check for size of picture if metric scaling
 * Add manual/auto page feed options
 * Should maybe do something about background colour
 *
 */

#define CGMOHPGL_C

/*  Include relevant parameters and structures */

#include "cgmout.h"
#include "cgmtext.h"

/*  Include device specific variables and structures */

#include "cgmhpgl.h"
#include "cgmgdp.h"
#include "cgmcolr.h"
#ifdef BEZIER
#include "cgmbez.h"
#endif
#ifdef HERSHEY
#include "cgmher.h"
#endif

/* CODE START */

/**************************************************** CGMOhpgl **********/

#ifdef PROTO
void CGMOhpgl(FILE *stream, Code c, Long *pi, Float *pr, Char *str)
#else
void CGMOhpgl(stream, c, pi, pr, str)
FILE *stream;
Code c;
Long *pi;
Float *pr;
Char *str;
#endif

/*  Output Driver for HPGL driven devices */
{
   static Logical first = TRUE;
   static Logical outq = FALSE;
   register Long n, i, j, num;
   Code major;
   Enum close;
   Lpoint localpts[ARRAY_MAX];
   Textitem *txt_item;


   if ( c == (Code) EOF ) exit(0);
   major = c>>8;
   num = *pi++;

   if (!outq)
   {
      out = stream;
      outq = TRUE;
   }

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */

         switch (c)
         {
            case NONOP:
               break;

            case LINE:         /*  Polyline   */
#ifdef DEBUG_LINE
fprintf(stderr, "\nElement LINE\n");
#endif
               if ( first ) HPGLattrib(LINE);
               HPGLline(num, pi, pr, LINE);
               first = (num >= ZERO);
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
#ifdef DEBUG_LINE
fprintf(stderr, "\nElement DISJTLINE\n");
#endif
               if (first) HPGLattrib(LINE);
               for (i = 1; i <= HPGLABS(num); i += 2)
               {
                  HPGLline(2L, pi, pr, LINE);
                  pi += 4; pr += 4;
               }
               first = (num >= ZERO);
               break;

            case MARKER:         /*  Polymarker  */
               if (first) HPGLattrib(MARKER);
               HPGLmarker(num, pi, pr);
               first = (num >= ZERO);
               break;

            case TEXT:               /*  Text   */
               HPGLattrib(TEXT);
               if (cur.vdc_type == REAL)
               {
                  txtpos.x = (float)pr[0];
                  txtpos.y = (float)pr[1];
               }
               else
               {
                  txtpos.x=(float)pi[0];
                  txtpos.y=(float)pi[1];
               }
               rt_mode=FALSE;
               txt_item = TXTaccinf(num, str, &hpgltext, text_method);
               if ((Enum)num == FINAL) HPGLtext(txt_item);
               break;


            case RESTRTEXT:            /*  Restricted Text */
               HPGLattrib( TEXT );
               if ( cur.vdc_type == REAL )
               {
                  rt_wid=(double)pr[0];
                  rt_hgt=(double)pr[1];
                  txtpos.x=(float)pr[2];
                  txtpos.y=(float)pr[3];
               }
               else
               {
                  rt_wid=(double)pi[0];
                  rt_hgt=(double)pi[1];
                  txtpos.x=(float)pi[2];
                  txtpos.y=(float)pi[3];
               }
               rt_mode=TRUE;
               txt_item = TXTaccinf( num, str, &hpgltext, text_method );
               if ( (Enum)num == FINAL ) HPGLtext( txt_item );
               break;

            case APNDTEXT:           /*  Append Text   */
               HPGLattrib(TEXT);
               txt_item = TXTaccinf( num, str, &hpgltext, text_method );
               if ( (Enum)num == FINAL ) HPGLtext( txt_item );
               break;

            case POLYGON:         /*  Polygon   */
               if (curatt.edge_vis==ON)
               {
                  HPGLattrib(EDGEVIS);
                  HPGLline(num, pi, pr, EDGEVIS);
               }
               else
               {
                  HPGLattrib(POLYGON);
                  HPGLfill(num, pi, pr);
               }
               break;

            case POLYGONSET:         /*  Polygon Set  */
               HPGLattrib(POLYGON);
               (void) CGMerror(func, ERR_ELMNOTSUP, WARNING, "POLYGONSET");
               break;

            case CELLARRAY:         /*  Cell Array  */
               if (first)
               {
                  if (cur.vdc_type==REAL)
                  {
                     cell.p.x = (Ptype)*pr++;
                     cell.p.y = (Ptype)*pr++;
                     cell.q.x = (Ptype)*pr++;
                     cell.q.y = (Ptype)*pr++;
                     cell.r.x = (Ptype)*pr++;
                     cell.r.y = (Ptype)*pr++;
                     pi += 6;
                  }
                  else
                  {
                     cell.p.x = (Ptype)*pi++;
                     cell.p.y = (Ptype)*pi++;
                     cell.q.x = (Ptype)*pi++;
                     cell.q.y = (Ptype)*pi++;
                     cell.r.x = (Ptype)*pi++;
                     cell.r.y = (Ptype)*pi++;
                  }
                  cell.nx = *pi++;
                  cell.ny = *pi++;
                  pi++;              /* Ignore Local Precision */
#ifdef DEBUG_CELL
                  printf("\nCellArray parm.\n");
                  printf("cell.p.x:%f cell.p.y:%f\n",cell.p.x,cell.p.y);
                  printf("cell.q.x:%f cell.q.y:%f\n",cell.q.x,cell.q.y);
                  printf("cell.r.x:%f cell.r.y:%f\n",cell.r.x,cell.r.y);
#endif

                  HPGLattrib( CELLARRAY );
               }

/*  set up local Cell Array parameters here */

               HPGLcell(cell, pi, pr);
               first = (num >= ZERO);
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               j = *pi++;
               close = (cgmralgks && j == GKSPIE ? PIE :
                       (cgmralgks && j == GKSCHORD ? CHORD
                                                    : ANY));
               if ( cgmralgks )
               {
                  close = (j == GKSPIE ? PIE :
                          (j == GKSCHORD ? CHORD :
                           ANY));
                  HPGLgdp((Code) j, pi, pr, close);
               }
               else
               {
                  CGMerror(func, ERR_NOGDP, ERROR, NULLSTR);
               }
               break;

            case RECT:         /*  Rectangle   */
               HPGLrect(pi, pr);
               break;

            default:
               (void) sprintf( emess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVPRIM, ERROR, emess);
               break;
         }
         break;

      case 0x30:  /*  Delimiter Elements  */

         switch (c)
         {

            case BEGMF:         /*  Begin Metafile   */
               HPGLinit();
               break;

            case ENDMF:         /*  End Metafile   */
               HPGLclose();
               break;

            case BEGPIC:        /*  Begin Picture Descriptor   */
               HPGLnewpic(str);
               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               HPGLsetdef();
               HPGLattrib(BEGPIC);
               break;

            case ENDPIC:         /*  End  Picture    */
               newpic=FALSE;
               att_flag=TRUE;
               COLRfree(); /* free colour table */
               HPGLmenu();
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVDELIM, ERROR, emess );
         }
         break;


      case 0x31:  /* Metafile Descriptor Elements  */
         switch (c)
         {
            case MFVERSION:        /* Metafile version */
            case MFDESC:        /* Metafile Description */
            case VDCTYPE:        /* VDC Type */
            case INTEGERPREC:        /* Integer Precision */
            case REALPREC:        /* Real Precision */
            case INDEXPREC:        /* Index Precision */
            case COLRPREC:        /* Colour Precision */
            case COLRINDEXPREC: /* Colour Index Precision */
            case MAXCOLRINDEX:        /* Maximum Colour Index */
            case COLRVALUEEXT:        /* Colour value extent */
            case MFELEMLIST:        /* Metafile element List */
            case BEGMFDEFAULTS: /* Begin Metafile defaults Replacement */
            case ENDMFDEFAULTS: /* End Metafile defaults Replacement */
            case FONTLIST:        /* Font List */
            case CHARSETLIST:        /* Character set list */
            case CHARCODING:        /* Character coding Announcer */
               break;
         }
         break;

      case 0x32:  /* Picture Descriptor Elements  */
         switch (c)
         {
            case COLRMODE:             /*  Colour Selection Mode */
            case SCALEMODE:                     /*  Scaling Mode */
            case LINEWIDTHMODE:     /*  Line width Specification */
            case MARKERSIZEMODE:   /*  Marker size Specification */
            case EDGEWIDTHMODE:     /*  Edge width Specification */
            case VDCEXT:                          /*  VDC Extent */
               break;

            case BACKCOLR:                 /*  Background Colour */
              /* If indexed colour then background colour goes into map */
               if (cur.color_mode == INDEXED)
               {
                  Long col[3];

                  col[0] = 0;
                  col[1] = cur.back.red;
                  col[2] = cur.back.green;
                  col[3] = cur.back.blue;

                  COLRput(1, col);
               }

               backpen = 0;
               break;

            default:
               (void) sprintf (emess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVCONT, ERROR, emess);
         }
         break;

      case 0x33:  /* Control Elements  */

         switch(c)
         {
            case VDCINTEGERPREC:       /* VDC Integer Precision */
            case VDCREALPREC:             /* VDC Real Precision */
            case AUXCOLR:                   /* Auxiliary Colour */
            case TRANSPARENCY:                  /* Transparency */
            case CLIPRECT:                    /* Clip Rectangle */
               setclip(IFNEED);
               break;
            case CLIP:                        /* Clip Indicator */
               setclip(IFNEED);
               break;
            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVCONT, ERROR, emess);
         }
         break;

      case 0x34:  /* Circles and Ellipses  */

         switch(c)
         {
            case CIRCLE:       /* Circle */
               HPGLgdp(CIRCLE, pi, pr, ANY);
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               HPGLgdp(ARC3PT, pi, pr, ANY);
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               HPGLgdp(ARC3PTCLOSE, pi, pr, (Enum) *(pi+6) );
               break;

            case ARCCTR:       /* Circle Arc centre */
               HPGLgdp(ARCCTR, pi, pr, ANY);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               HPGLgdp(ARCCTRCLOSE, pi, pr, (Enum) *(pi+7) );
               break;

            case ELLIPSE:        /* Ellipse */
               HPGLgdp(ELLIPSE, pi, pr, ANY);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               HPGLgdp(ELLIPARC, pi, pr, ANY);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close */
               HPGLgdp(ELLIPARCCLOSE, pi, pr, (Enum) *(pi+10) );
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVOUT, ERROR, emess );
         }
         break;

      case 0x35:  /* Attributes */
         att_flag = TRUE;
         switch(c)
         {
            case CHARHEIGHT:           /* Character Height */

            case LINEWIDTH:                  /* Line Width */

            case MARKERSIZE:                /* Marker Size */

            case LINEINDEX:           /* Line Bundle index */

            case LINETYPE:                    /* Line Type */

            case LINECOLR:                  /* Line Colour */

            case MARKERINDEX:       /* Marker Bundle index */

            case MARKERTYPE:                /* Marker Type */

            case MARKERCOLR:              /* Marker Colour */

            case TEXTINDEX:           /* Text Bundle index */

            case TEXTFONTINDEX:         /* Text Font Index */

            case TEXTPREC:               /* Text Precision */

            case CHAREXPAN:  /* Character Expansion Factor */

            case CHARSPACE:           /* Character Spacing */

            case TEXTCOLR:                  /* Text Colour */

            case CHARORI:         /* Character Orientation */

            case TEXTPATH:                    /* Text Path */

            case TEXTALIGN:              /* Text Alignment */

            case CHARSETINDEX:      /* Character Set Index */

            case ALTCHARSETINDEX:  /* Altern Character Set Index */
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVATT, ERROR, emess );
               break;
         }
         break;

      case 0x36:  /* More Attributes  */
         att_flag = TRUE;
         switch(c)
         {
            case EDGEWIDTH:               /*  Edge Width */

            case FILLINDEX:       /*  Fill Bundle index  */

            case INTSTYLE:           /*  Interior Style  */

            case FILLCOLR:               /*  Fill Colour */

            case HATCHINDEX:            /*  Hatch Index  */

            case PATINDEX:            /*  Pattern Index  */

            case EDGEINDEX:       /*  Edge Bundle index  */

            case EDGETYPE:                /*  Edge Type  */

            case EDGECOLR:              /*  Edge Colour  */

            case EDGEVIS:           /*  Edge Visibility  */

            case FILLREFPT:    /*  Fill Reference Point  */

            case PATTABLE:            /*  Pattern Table  */

            case PATSIZE:              /*  Pattern Size  */
                  break;

            case  COLRTABLE:           /*  Colour Table  */
               COLRput(num, pi);
            break;


            case ASF:       /*  Aspect source flags  */
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVATT, ERROR, emess );
               break;

         }
         break;

      case 0x37:  /* Escape And External Elements  */
         switch (c)
         {
            case ESCAPE:         /*  Escape  */
               break;

            case MESSAGE:         /*  Message  */
               break;

            case APPLDATA:         /*  Application Data  */
               break;

            default:
               (void) sprintf ( emess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVESC, ERROR, emess);
               break;
         }
         break;

      default:
         (void) sprintf ( emess, "(Code: 0x%x)", c);
         (void) CGMerror ( func, ERR_INVELEM, ERROR, emess);
         break;
     }

     return;
  }

/********************************* HPGLoptions **************************/
#ifdef PROTO
void HPGLoptions(char *opts)
#else
void HPGLoptions(opts)
char *opts;
#endif
{
/*
In here should go things like number of pens, paper size, maybe
plotter number to make easy to choose. Maybe even pen colours.
*/
   int i, len = strlen(opts), pen_num, n;
   char pen_name[9], *pen_desc_str;
   float red, green, blue, width;

/*
 * Set up the pens from the default, or the environment variables
 * Note that command line options may change pens later on.
 */

   pen=(pen_spec *)malloc(sizeof(pen_spec)*(pens+1));

   for(pen_num=0; pen_num<=pens; pen_num++)
   {

      sprintf(pen_name, "CGMHPGL%-d", pen_num);
#ifdef DEBUG_PENS
fprintf(stderr, "\nLooking for envirionment var (%s)\n",pen_name);
#endif
      pen_desc_str = (char *)getenv( pen_name );
#ifdef DEBUG_PENS
fprintf(stderr, "\nFound pen desciption (%s)\n",pen_desc_str);
#endif
      if (pen_desc_str == NULL)
      {
         pen[pen_num] = def_pen[pen_num];
      }
      else
      {
         n = sscanf(pen_desc_str, "%f,%f,%f,%f", &red, &green, &blue, &width);
         switch(n)
         {
         case 4: /* width and 3 colours specified */
            pen[pen_num].width = width;
         case 3: /* 3 colours specified */
            if ( (0.0 <= red) && (red <= 1.0) &&
                   (0.0 <= green) && (green <= 1.0) &&
                   (0.0 <= blue) && (blue  <= 1.0))
            {
            pen[pen_num].colour.red = red;
            pen[pen_num].colour.green = green;
            pen[pen_num].colour.blue = blue;
            break;
         }
         default:
            sprintf(emess, "pen %2d\n", pen_num);
            (void)CGMerror(func, ERR_BADPENDEF, WARNING, emess);
            pen[pen_num] = def_pen[pen_num];
         }
      }
#ifdef DEBUG_PENS
fprintf(stderr, "\nPEN %2d : %f %f %f %f\n",
        pen_num,
        pen[pen_num].colour.red,
        pen[pen_num].colour.green,
        pen[pen_num].colour.blue,
        pen[pen_num].width);
#endif
   }

#ifdef DEBUG
fprintf(stderr, "\nHPGLoptions args are :- %s\n", opts);
#endif

   if ((opts!=NULL)&&(len!=0))
   {
#ifdef DEBUG_OPTIONS
      fprintf(stderr, "\nHPGLoptions: Options for this run are :- ");
#endif
      for (i=0; i<len; i++)
      {
#ifdef DEBUG_OPTIONS
         fprintf(stderr, "%c", opts[i]);
#endif
         switch (opts[i])
         {
         case '3':  /* Set papersize to A3 */
            size=A3;
            break;

         case '4':  /* Set papersize to A4 */
            size=A4;
            break;

         case 'P':  /* Set Paper to be Portrait */
         case 'p':
            orient = PORTRAIT;
            break;

         case 'L':  /* Set Paper to be Landscape */
         case 'l':
            orient = LANDSCAPE;
            break;

         case 'N':  /* Set not to put out printer control codes */
         case 'n':
            printcontrol = FALSE;
            break;

         case 'C':  /* Set to put out printer control codes */
         case 'c':
            printcontrol = TRUE;
            break;

         case 'A':  /* Set to automatic page feed */
         case 'a':
            paperchange = AUTO;
            break;

         case 'M':  /* Set to manual page feed */
         case 'm':
            paperchange = MANUAL;
            break;

         default:
          {
            char *t=" ";

            *t = opts[i];
            (void)CGMerror(func, ERR_INVOPT, ERROR, t);
            break;
          }
         }
#ifdef DEBUG_OPTIONS
         fprintf(stderr, "\n");
#endif
      }
   }

   return;
}

/********************************* HPGLattrib ***************************/

#ifdef PROTO
static void HPGLattrib(Code type)
#else
static void HPGLattrib(type)
Code type;
#endif
/*  Converts CGM Attribute 'type' to local Attribute  */
{
    static Logical last_prim = NONOP;
#ifdef DEBUG
    DMESS  "\nHPGLattrib: type 0x%x\n", type);
#endif

   /* if no attribute has changed and last primitive was of
    * the same type as this one then return */
   if (!att_flag && (last_prim==type))
      return;

   switch (type)
   {
      case LINE:
         ATTline(NLINEB, line_bundle, cur.max_colind, &hpglline);

         setcolour(hpglline.colindex, hpglline.col);
         sendlinestyle(hpglline.type);

         /* Set local attributes here  */
         break;

      case MARKER:
         ATTmarker(NMARKB, mark_bundle, cur.max_colind, &hpglmark);

         /* Set local attributes here */
         setcolour(hpglmark.colindex,  hpglmark.col);

         break;

      case TEXT:
         ATTtext( NTEXTB, text_bundle, cur.max_colind, &hpgltext );
         break;

      case POLYGON:  /*  Fill attributes  */
         ATTfill(NFILLB, fill_bundle, cur.max_colind, &hpglfill);
         ATTedge(NEDGEB, edge_bundle, cur.max_colind, &hpgledge);

         /* Set local attributes here        */
         if ( curatt.edge_vis == ON )
         {
            sendlinestyle(hpgledge.type);
            setcolour(hpgledge.colindex,  hpgledge.col);
         }
         else
         {
            sendlinestyle((Index)1);
            setcolour(hpglfill.colindex,  hpglfill.col);
         }

        break;

      case EDGEVIS:    /*  Edge attributes */
         ATTedge(NEDGEB,edge_bundle, cur.max_colind,&hpgledge);

         /* Set local attributes here        */

         sendlinestyle(hpglline.type);
         setcolour(hpgledge.colindex,  hpgledge.col);
         break;

      case CELLARRAY:  /*  Cell array - may affect texture & colour*/
          break;

      case BEGPIC:    /* Reset Picture attributes to default */
         break;

      default:
         (void) sprintf ( emess, "(Type: 0x%x)", type);
         (void) CGMerror ( func, ERR_INVATT, ERROR, emess );
         break;
   }

   last_prim = type;
   att_flag = FALSE;

   return;
}

/***************************************************** HPGLinit *********/

#ifdef PROTO
static void HPGLinit(void)
#else
static void HPGLinit()
#endif

/*  Initialise Variables and colour map */

{
   Index n;
/*
Ok in here are going to go all the things which are needed to set up the
printer, HPGLoption will already have been called so we know what
orientation, printer type etc we are working with.
*/
   hpglcuratt = hpgldefatt;

   hpglline.shared = &hpglshared;
   hpglmark.shared = &hpglshared;
   hpgltext.shared = &hpglshared;
   hpglfill.shared = &hpglshared;
   hpgledge.shared = &hpglshared;

   ext1.x=225; ext1.y=523;
   switch (orient+size)
   {
   case A4+LANDSCAPE:
      ext1.x=225; ext1.y=523;
      ext2.x=11040; ext2.y=7721;
      break;
   case A4+PORTRAIT:
      ext1.x=0; ext1.y=225;
      ext2.x=7198; ext2.y=11040;
      break;
   case A3+PORTRAIT:
      ext2.x=11040; ext2.y=15995;
      break;
   case A3+LANDSCAPE:
      ext2.x=15995; ext2.y=11040;
      break;
   default:
      fprintf(stderr,"Internal error bad option combination\n");
   }

   /* Find the widest pen */
   n = pens;
   while(--n)
       max_pen_width=(pen[n].width>max_pen_width)?pen[n].width:max_pen_width;
}


/*** Set local default attribute ***/
#ifdef PROTO
static void HPGLsetdef(void)
#else
static void HPGLsetdef()
#endif
{
   if (printcontrol)
   {
      fprintf(out, "\033.@;0:\n");
      fprintf(out, "\033.I81;;17:\n");
      fprintf(out, "\033.N;19:\n");
   }

   fprintf(out, "IN;\n");

   switch (orient+size)
   {
   case A4+LANDSCAPE:
      fprintf(out, "PS4;\n");
      fprintf(out, "RO;\n");
      break;
   case A4+PORTRAIT:
      fprintf(out, "PS4;\n");
      fprintf(out, "RO90;\n");
      break;
   case A3+PORTRAIT:
      fprintf(out, "PS0;\n");
      fprintf(out, "RO90;\n");
      break;
   case A3+LANDSCAPE:
      fprintf(out, "PS0;\n");
      fprintf(out, "RO;\n");
      break;
   default:
      fprintf(stderr,"Internal error bad option combination\n");
   }
   HPGLwindow();
   sendP1P2(P1, P2);
   setclip(FORCE);

  /* Send out rectangle of max size in background colour */
   if (backpen)
   {
      sendcolour(backpen);
      rect_r(P1, P2);
   }

   return;
}

#ifdef PROTO
static void setclip(int cond)
#else
static void setclip(cond)
int cond;
#endif
{
   if ( cur.clip_ind == ON )
   {
#ifdef DEBUG_CLIP
      printf("\nClipping is ON, and to clip rectangle\n");
#endif
      sendclip(cur.clip_rect, cond);
   }
   else  /* cur.clip_ind == OFF */
   {
#ifdef DEBUG_CLIP
      printf("\nClipping is OFF, and to VDC extent\n");
#endif
      sendclip(cur.vdc_extent, cond);
   }
}

#ifdef PROTO
static void HPGLwindow(void)
#else
static void HPGLwindow()
#endif
{
/*
 * For abstract scaling then we will keep the aspect ratio implied
 * by the VDC extent
 * For metric then set the whole kaboodle to millimeters
 */

   if ( cur.vdc_type == REAL )
   {
      Wx1 = cur.vdc_extent.a.x.real;
      Wy1 = cur.vdc_extent.a.y.real;
      Wx2 = cur.vdc_extent.b.x.real;
      Wy2 = cur.vdc_extent.b.y.real;
   }
   else
   {
      Wx1 = (Float) cur.vdc_extent.a.x.intr;
      Wy1 = (Float) cur.vdc_extent.a.y.intr;
      Wx2 = (Float) cur.vdc_extent.b.x.intr;
      Wy2 = (Float) cur.vdc_extent.b.y.intr;
   }

  /* Calculate the scaling factor used to get from VDC to plotter coords */
   if (cur.scale_mode == METRIC)
   {
     /*
      * If METRIC defined by only by scale factor and
      * must be stuck to in order to get the picture right
      */
      x_scale = y_scale = POINTS_PER_MM * cur.scale_factor;
   }
   else
   {
      Float abs_x_scale, abs_y_scale;

     /*
      * Work out scale factors as if not preserving aspect ratio
      * one of these will be correct, the other will be adjusted
      */
      x_scale = (Float)(ext1.x-ext2.x)/(Wx1-Wx2);
      y_scale = (Float)(ext1.y-ext2.y)/(Wy1-Wy2);

      abs_x_scale = HPGLABS(x_scale);
      abs_y_scale = HPGLABS(y_scale);

     /*
      * Find out which direction had the smallest scale factor this one
      * is correct, the other needs to match in value, but retain its
      * old sign
      */
      if (abs_x_scale<abs_y_scale)
         y_scale = (y_scale<0.0) ? -abs_x_scale : abs_x_scale;
      else
         x_scale = (x_scale<0.0) ? -abs_y_scale : abs_y_scale;
   }

   /* Now calculate the translation to get origin in right place */
   x_tran = (Float)ext1.x - x_scale*Wx1;
   y_tran = (Float)ext1.y - y_scale*Wy1;

   P1 = ext1;
   P2.x = (Lptype)XTRAN(Wx2);
   P2.y = (Lptype)YTRAN(Wy2);

  /* Check on paper size */
   if ((P2.x>ext2.x) || (P2.y>ext2.y))
   {
      /* Picture is bigger than paper give up */
      CGMerror(func, ERR_PICLGRPAP, FATAL, NULLSTR);
      exit(0);
   }

#ifdef DEBUG
fprintf(stderr, "\nMapping transform is :-  (%f, %f)\n", x_scale, 0.0);
fprintf(stderr, "                         (%f, %f)\n", 0.0, y_scale);
fprintf(stderr, "                         (%f, %f)\n\n", x_tran, y_tran);
#endif

   hpglshared.xgrain = x_scale;
   hpglshared.ygrain = y_scale;

   wvrat=(x_scale<y_scale)?x_scale:y_scale;

}



/***************************************************** HPGLnewpic *******/

#ifdef PROTO
static void HPGLnewpic(char *pic_name)
#else
static void HPGLnewpic(pic_name)
char *pic_name;
#endif

/*   New Frame processing started at first drawing element  */
{
   char name[80];

   newpic=TRUE;
   /* If file is not open then open new */
   if (out==NULL)
   {
      fprintf(stderr, "New file name\n");
      fscanf(stdin, "%s", name);
      out=fopen(name, "w");
      if (out==NULL)
         exit((int)CGMerror (func, ERR_OPENFILE, FATAL, NULLSTR));
      att_flag=TRUE;
   }
   return;
}

/***************************************************** HPGLmenu *********/

#ifdef PROTO
static void HPGLmenu(void)
#else
static void HPGLmenu()
#endif

/*  End of frame processing to select options */

{
#ifdef DEBUG
fprintf(stderr, "HPGLmenu\n");
#endif

#ifdef DEBUG_FILES
fprintf(stderr, "\nHPGLmenu: paper type is ");
if (paperchange==MANUAL)
   fprintf(stderr, "MANUAL\n");
else
   fprintf(stderr, "AUTO\n");
#endif
   /* If no automatic page throw then close file */
   if (paperchange==MANUAL)
   {
      sendcolour(0);
      fprintf(out, "PA%d,%d;\n", ext1.x, ext2.y);
      fprintf(out, "IN;\n");

      if (printcontrol)
         fprintf(out, "\033.R\n");

      fclose(out);
      out = NULL;
   }

   return;
}

/***************************************************** HPGLclose ********/

#ifdef PROTO
static void HPGLclose(void)
#else
static void HPGLclose()
#endif

/*  Close down the Graphics  */
{
/*
Ok here is the point to shove the paper out, and reinitialise the
printer
*/

   /* If automatic page throw then file needs closing now */
   if (paperchange==AUTO)
   {
      sendcolour(0);
      fprintf(out, "IN;\n");
      fprintf(out, "\033.R\n");
      fclose(out);
      out = NULL;
   }

   COLRfree();   /* Free Colour Table */

   return;
}

/***************************************************** HPGLmarker *******/

#ifdef PROTO
static void HPGLmarker(Long n, Long *pi, Float *pr)
#else
static void HPGLmarker(n, pi, pr)
Long n, *pi;
Float *pr;
#endif

/*   draws n markers starting at pi/pr  */
{
   Long i;
   Float msizex, msizey;
   Lpt lpt;

   /* find the length in world coordinate system */
   if ( hpglmark.sizemode == SCALED )
   {
      msizex=hpglmark.size;
      msizey=hpglmark.size;
   }
   else
   {
      msizex = (Lptype)100.0*(float)hpglmark.size/(Wx2-Wx1);
      msizey = 100.0*(float)hpglmark.size/(Wy2-Wy1);
   }

   fprintf(out, "SR%.4f,%.4f;", msizex, msizey);

   switch (hpglmark.type)
   {
      case 2:        /* "*" */
         fprintf(out, "SM%c;\n", MARKER_STAR);
         break;
      case 3:        /* "+" */
         fprintf(out, "SM%c;\n", MARKER_PLUS);
         break;
      case 5: /* "X" */
         fprintf(out, "SM%c;\n", MARKER_CROSS);
         break;
      case 1: /* "." */
         fprintf(out, "SM%c;\n", MARKER_DOT);
         break;
      case 4: /* "O" */
         fprintf(out, "SM%c;\n", MARKER_OH);
         break;
   }

   if ( cur.vdc_type == REAL )
   {
      lpt.x = (Lptype)XTRAN(*pr++);
      lpt.y = (Lptype)YTRAN(*pr++);
      compoints(NEW, "PA", lpt);
      for ( i=1; i<n; i++ )
      {
         lpt.x = (Lptype)XTRAN(*pr++);
         lpt.y = (Lptype)YTRAN(*pr++);
         compoints(CONT, "PA", lpt);
      }
   }
   else
   {
      lpt.x = (Lptype)XTRAN(*pi++);
      lpt.y = (Lptype)YTRAN(*pi++);
      compoints(NEW, "PA", lpt);
      for ( i=1; i<n; i++ )
      {
         lpt.x = (Lptype)XTRAN(*pi++);
         lpt.y = (Lptype)YTRAN(*pi++);
         compoints(CONT, "PA", lpt);
      }
   }

   compoints(END, "", lpt);
   fprintf(out, "SM;\n");


   return;
}

/***************************************************** HPGLtext *********/

#ifdef PROTO
static void HPGLtext(Textitem *txt)
#else
static void HPGLtext(txt)
Textitem *txt;
#endif

/*  draws TEXT according to current settings */
{
   Textitem *curtxt = txt;

   if (rt_mode)
      curtxt=TXTrestrict(curtxt, rt_wid, rt_hgt, &hpgltext);

   /*
    * If restricted has screwed up then dont print anything
    * I dont beleive that TXTrestrict should be able to mess up
    * to this extent, but at the moment it can! So we check it.
    */
   if (!curtxt)
      return;

   TXTalign(curtxt, txtpos, TXTgwidth(curtxt), TXTgheight(curtxt));

   while(curtxt != NULL)
   {
      setcolour(curtxt->colourindex, curtxt->text_col);

      switch(curtxt->text_method)
      {
#ifdef HERSHEY
      case HER:
         HERtext(curtxt, txtpos, HPGLtxline);
         break;
#endif
#ifdef BEZIER
      case BEZ:
         BEZtext(curtxt, &hpgltext, txtpos, HPGLtxfill);
         break;
#endif
      case TEX: /* not supported yet ! */
      case HDW: /* not supported yet ! */
      default:
         fprintf(stderr, "Internal error bad font system selected");
         break;
      }
      curtxt = curtxt->next;
   }

   TXTfree(txt);

   return;
}

/***************************************************** HPGLtxline *******/
#ifdef PROTO
static void HPGLtxline(Int n, Point *pt, Tmatrix matrix, int newchar)
#else
static void HPGLtxline(n, pt, matrix, newchar)
Int n;
Point *pt;
Tmatrix matrix;
int newchar;
#endif
{
   static Tmatrix newmat;
   Lpt *lptlst, *lpt, *lptend;
   Index style=1;
#ifdef DEBUG_TXLINE
   Long i;
#endif

#ifdef DEBUG_TXLINE
fprintf(stderr, "\nHPGLtxline: args are :- n=%ld\n", n);
fprintf(stderr, "\nHPGLtxline: coords are :-\n");
for(i=0;i<n;i++)
      fprintf(stderr, "(%f,%f)\n", pt[i].x, pt[i].y);
#endif

   lpt = lptlst = (Lpt *)MALLOC(n, sizeof(Lpt));
   if ( lpt==NULL ) CGMerror (func, ERR_NOMEMORY, FATAL, NULLSTR);
   lptend = lpt+n;

#ifdef DEBUG_TXLINE
fprintf(stderr, "\nHer xform is :-  (%lf, %lf)\n", matrix[0][0], matrix[0][1]);
fprintf(stderr, "                 (%lf, %lf)\n", matrix[1][0], matrix[1][1]);
fprintf(stderr, "                 (%lf, %lf)\n", matrix[2][0], matrix[2][1]);
#endif

   /*
    * If new character then we have a new transfomation to work out
    * and we need to set a new linestyle, since it may have changed
    */
   if (newchar)
   {
      sendlinestyle((Index)1);

      newmat[0][0] = matrix[0][0] * (Mtype)x_scale;
      newmat[1][0] = matrix[1][0] * (Mtype)x_scale;
      newmat[2][0] = matrix[2][0] * (Mtype)x_scale + (Mtype)x_tran;
      newmat[0][1] = matrix[0][1] * (Mtype)y_scale;
      newmat[1][1] = matrix[1][1] * (Mtype)y_scale;
      newmat[2][1] = matrix[2][1] * (Mtype)y_scale + (Mtype)y_tran;

#ifdef DEBUG_TXLINE
fprintf(stderr, "\nHer xform is :- (%lf, %lf)\n", newmat[0][0], newmat[0][1]);
fprintf(stderr, "                (%lf, %lf)\n", newmat[1][0], newmat[1][1]);
fprintf(stderr, "                (%lf, %lf)\n\n", newmat[2][0], newmat[2][1]);
#endif
   }

   /* If less than 2 points then forget it */
   if (n<2)
      return;

   while(lpt<lptend)
   {
       lpt->x = (Lptype)(pt->x *(Ptype)newmat[0][0]+
                         pt->y *(Ptype)newmat[1][0]+
                                (Ptype)newmat[2][0]);

       (lpt++)->y = (Lptype)(pt->x*(Ptype)newmat[0][1]+
                            (pt++)->y*(Ptype)newmat[1][1]+
                                   (Ptype)newmat[2][1]);
   }

   line_r(n ,lptlst);
   FREE(lptlst);

}

/***************************************************** HPGLtxfill *******/
#ifdef PROTO
static void HPGLtxfill(Int n, Point *pt, Tmatrix matrix, int newchar)
#else
static void HPGLtxfill(n, pt, matrix, newchar)
Int n;
Point *pt;
Tmatrix matrix;
int newchar;
#endif
{
   Tmatrix newmat;
   Lpt *lptlst, *lpt, *lptend;
   unsigned short style=1;

   lpt = lptlst = (Lpt *)MALLOC(n, sizeof(Lpt));
   if ( lpt==NULL ) CGMerror (func, ERR_NOMEMORY, FATAL, NULLSTR);
   lptend = lpt+n;

   /*
    * If new character then we have a new transfomation to work out
    * and we need to set a new linestyle, since it may have changed
    */
   if (newchar)
   {
      sendlinestyle((Index)1);

      newmat[0][0] = matrix[0][0] * (Mtype)x_scale;
      newmat[1][0] = matrix[1][0] * (Mtype)x_scale;
      newmat[2][0] = matrix[2][0] * (Mtype)x_scale + (Mtype)x_tran;
      newmat[0][1] = matrix[0][1] * (Mtype)y_scale;
      newmat[1][1] = matrix[1][1] * (Mtype)y_scale;
      newmat[2][1] = matrix[2][1] * (Mtype)y_scale + (Mtype)y_tran;

#ifdef DEBUG_TXLINE
fprintf(stderr, "\nHer xform is :- (%lf, %lf)\n", newmat[0][0], newmat[0][1]);
fprintf(stderr, "                (%lf, %lf)\n", newmat[1][0], newmat[1][1]);
fprintf(stderr, "                (%lf, %lf)\n\n", newmat[2][0], newmat[2][1]);
#endif
   }

   /* If less than 2 points then forget it */
   if (n<3)
      return;

   while(lpt<lptend)
   {
       lpt->x = (Lptype)(pt->x*(Ptype)newmat[0][0]+
                          pt->y*(Ptype)newmat[1][0]+
                                (Ptype)newmat[2][0]);

       (lpt++)->y = (Lptype)(pt->x*(Ptype)newmat[0][1]+
                            (pt++)->y*(Ptype)newmat[1][1]+
                                   (Ptype)newmat[2][1]);
   }

   fill_r(n ,lptlst);
   FREE(lptlst);

}

/***************************************************** HPGLgdp **********/

#ifdef PROTO
static void HPGLgdp(Code type, Long *pi, Float *pr, Enum close)
#else
static void HPGLgdp(type, pi, pr, close)
Code type;
Long *pi;
Float *pr;
Enum close;
#endif

/*  draws GDP 'type' with points starting at pi/pr
    for convenience CGM primitives are handled as GDPs  */
{
   Float *p;
   Long j, np, i;
   Logical fill, edge, GKSgdp, isarc, centreknown = FALSE;
   Point p1, p2, p3, p4, p5, centre, cdp1, cdp2;
   Double rad=0.0, ang, ang1, ang2;
   Enum vdctype;
   Lpoint *pt = localpts;

   centre.x = 0.0;
   centre.y = 0.0;
   ang = 0.0;

/*  work out if a RAL-GKS GDP  */

   GKSgdp = ( type == GKSCIRCLE || type == GKSARC ||
              type == GKSPIE    || type == GKSCHORD );

/*  work out if this is an arc  */

   isarc = ( type == GKSARC || type == ARC3PT ||
             type == ARCCTR || type == ELLIPARC );
/*  decide whether to draw fill area, edge or both and set attributes  */

   fill = ( ! isarc && curatt.int_style != EMPTY
                    && curatt.int_style != HOLLOW );
   edge = ( curatt.edge_vis == ON || curatt.int_style == HOLLOW );

  /* make sure area is visible */

   if ( ! (fill || edge || isarc) ) return;

  /*  At present ignore transformation of GKS GDPs */

   if (GKSgdp)
   {
       pi += 6; pr += 6;
   }

/*  get 5 points and convert to real values */

   if (cur.vdc_type == REAL)
   {
      p1.x = *pr++;  p1.y = *pr++;
      p2.x = *pr++;  p2.y = *pr++;
      p3.x = *pr++;  p3.y = *pr++;
      p4.x = *pr++;  p4.y = *pr++;
      p5.x = *pr++;  p5.y = *pr++;
   }
   else
   {
      p1.x =  *pi++; p1.y = *pi++;
      p2.x =  *pi++; p2.y = *pi++;
      p3.x =  *pi++; p3.y = *pi++;
      p4.x =  *pi++; p4.y = *pi++;
      p5.x =  *pi++; p5.y = *pi++;
   }

/*  Now work out significant points  */

   switch ( type )
   {
      case ARCCTR:
      case ARCCTRCLOSE:
            centre = p1;
            rad = p4.x;
            GDParc ( &centre, (double)rad, &p2, &p3, &ang, &p1, &p3 );
            centreknown = TRUE;

      case GKSARC:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PT:
      case ARC3PTCLOSE:
            if ( ! centreknown )
                  GDPcentre ( &centre, &rad, &ang, &p1, &p2, &p3);

            if ( REQUAL( ang, 0.0 ) )
            {
               /* Points are co-linear        */
               p2 = p3;
               type = LINE;

               break;
            }

            /* work out points */
            GDPcircle ( &centre, (Double)rad, (Double)ang,
                        &p1, (Double)wvrat, &np, pt, close );

           break;

      case GKSCIRCLE:
           centre = p1;
           rad = (Double)SQRT(( p1.x - p2.x ) * ( p1.x - p2.x ) +
                      ( p1.y - p2.y ) * ( p1.y - p2.y ) );
           GDPcircle ( &centre, (Double)rad, (Double)(2.0 * PI),
                       &p2, (Double)wvrat, &np, pt, close );
           break;

      case CIRCLE:
            centre = p1;
            rad = p2.x;
            p1.x += rad;
            GDPcircle ( &centre, (Double)rad, (Double)( 2 * PI ),
                        &p1, (Double)wvrat, &np, pt, close );
           break;

      case ELLIPSE:
           centre = p1;
           cdp1 = p2; cdp2 = p3;
           p4.x = 1.0; p4.y = 0.0;
           GDPellipse( &centre, &cdp1, &cdp2, &p4, &p4, (Double)wvrat,
                        &np, pt, close );
           break;

      case ELLIPARC:
      case ELLIPARCCLOSE:
           centre = p1;
           cdp1 = p2;        cdp2 = p3;
           if ( VEQUAL(p2.x * p3.y - p3.x * p2.y, 0.0) )
           {
              /* Points are co-linear        */
              p1 = p3;
              type = LINE;
              break;
           }
           GDPellipse( &centre, &cdp1, &cdp2, &p4, &p5, (Double)wvrat,
                        &np, pt, close);
           break;

      default:
           break;
   }

/* now draw primitives  */

   switch ( type )
   {
      case SOFTWARE:
      break;

      case CIRCLE:
      case ARC3PT:
      case ELLIPSE:
      case ARCCTR:
      case ELLIPARC:
      case GKSARC:
      case GKSCIRCLE:
           p = (Float *)malloc( 2*(size_t)(np-2)*sizeof(Float) );
           if ( p == NULL )
                CGMerror ("HPGLgdp", ERR_NOMEMORY, FATAL, NULLSTR);

           /* 1st and last pts are the centre, so ignore them */
           for (i=1; i<np-1; i++)
           {
              p[ 2*i-2 ] = pt[i].x;
              p[ 2*i-1 ] = pt[i].y;
           }
           np -= 2;
           break;

      case ARC3PTCLOSE:
      case ARCCTRCLOSE:
      case ELLIPARCCLOSE:
      case GKSPIE:
      case GKSCHORD:
           p = (Float *)malloc( 2*(size_t)np*sizeof(Float) );
           if ( p == NULL )
                CGMerror ("HPGLgdp", ERR_NOMEMORY, FATAL, NULLSTR);

           for (i=0; i<np; i++)
           {
              p[ 2*i ]   = pt[i].x;
              p[ 2*i+1 ] = pt[i].y;
           }
           break;

      case LINE:    /*         Points are co-linear  */
           p = (Float *)malloc( 4*sizeof(Float) );
           if ( p == NULL)
                CGMerror ("HPGLgdp", ERR_NOMEMORY, FATAL, NULLSTR);

           p[0] = p1.x; p[1] = p1.y;
           p[2] = p2.x; p[3] = p2.y;
           np = 2;

           break;

         break;
      default:
         break;
   }

   if (isarc)
   {
      HPGLattrib(LINE);
      HPGLline( np, pi, p, LINE);
   }
   else
   {
      if ( fill )
      {
         HPGLattrib(POLYGON);
         HPGLfill( np, pi, p);
      }
      if ( edge )
      {
         HPGLattrib(EDGEVIS);
         HPGLline( np, pi, p, EDGEVIS);
      }
   }
   FREE(p);

   return;
}

/********************************* HPGLline *****************************/
#ifdef PROTO
static void HPGLline(Long n, Long *pi, Float *pr , Code type)
#else
static void HPGLline(n, pi, pr, type)
Long n, *pi;
Float *pr;
Code type;
#endif
/*   draws n point polyline starting at pi/pr  */
{
   double w;  /* line width */
   unsigned short style;
   Lpt *lptlst, *lpt, *lptend;
#ifdef DEBUG_LINE
   Long i;
#endif

#ifdef DEBUG_LINE
fprintf(stderr, "\nHPGLline: args are :- n=%ld\n", n);
fprintf(stderr, "\nHPGLline: coords are :-\n");
for(i=0;i<n;i++)
   if (cur.vdc_type==INTEGER)
      fprintf(stderr, "(%d,%d)\n", pi[2*i], pi[2*i+1]);
   else
      fprintf(stderr, "(%f,%f)\n", pr[2*i], pr[2*i+1]);
#endif

   if ( n<2 ) return; /* not a line or edge */

   switch ( type )
   {
   case LINE:
      if ( hpglline.widthmode == SCALED )
         w = hpglline.width / wvrat; /* window scaled */
      else
        w = hpglline.width;

      break;

   case EDGEVIS:
      if ( hpgledge.widthmode == SCALED )
         w = hpgledge.width / wvrat; /* window scaled */
      else
         w = hpgledge.width;

      break;
   }

   /* Ok for now all transformation done before output */
   lpt = lptlst = (Lpt *)MALLOC(sizeof(Lpt),n);
   lptend = lpt+n;

   if ( cur.vdc_type == INTEGER )
      while(lpt<lptend)
      {
         lpt->x = (Lptype)XTRAN(*pi++);
         (lpt++)->y = (Lptype)YTRAN(*pi++);
      }
   else /* REAL vdc */
      while(lpt<lptend)
      {
         lpt->x = (Lptype)XTRAN(*pr++);
         (lpt++)->y = (Lptype)YTRAN(*pr++);
      }

   line_r(n, lptlst);

   return;
}

/*********************************** HPGLfill ***************************/

#ifdef PROTO
static void HPGLfill(Long n, Long *pi, Float *pr)
#else
static void HPGLfill(n, pi, pr)
Long n, *pi;
Float *pr;
#endif
{
   Index style=1;
   Lpt *lptlst, *lpt, *lptend;

   /* Ok for now all transformation done before output */
   lpt = lptlst = (Lpt *)MALLOC(sizeof(Lpt),n);
   lptend = lpt+n;

   if ( cur.vdc_type == INTEGER )
      while(lpt<lptend)
      {
         lpt->x = (Lptype)XTRAN(*pi++);
         (lpt++)->y = (Lptype)YTRAN(*pi++);
      }
   else /* REAL vdc */
      while(lpt<lptend)
      {
         lpt->x = (Lptype)XTRAN(*pr++);
         (lpt++)->y = (Lptype)YTRAN(*pr++);
      }

   sendlinestyle(style);
   fill_r(n, lptlst);

   return;
}

/********************************* HPGLrect *****************************/
#ifdef PROTO
static void HPGLrect(Long *pi, Float *pr)
#else
static void HPGLrect(pi, pr)
Long *pi;
Float *pr;
#endif
/*   draws rectangle parrallel to axis using point from pi/pr  */
{
   double w;  /* line width */
   Lpt lpt1, lpt2;

#ifdef DEBUG_RECT
   fprintf(stderr, "\nHPGLrect: coords are :-\n");
   if (cur.vdc_type==INTEGER)
      fprintf(stderr, "(%d,%d) (%d,%d)\n", pi[0], pi[1], pi[2], pi[3]);
   else
      fprintf(stderr, "(%f,%f) (%f,%f)\n", pr[0], pr[1], pr[2], pr[3]);
#endif

      if ( hpglline.widthmode == SCALED )
         w = hpglline.width / wvrat; /* window scaled */
      else
        w = hpglline.width;
      if ( hpgledge.widthmode == SCALED )
         w = hpgledge.width / wvrat; /* window scaled */
      else
         w = hpgledge.width;
   if (cur.vdc_type==INTEGER)
   {
      lpt1.x = (Lptype)XTRAN(*pi++);
      lpt1.y = (Lptype)YTRAN(*pi++);
      lpt2.x = (Lptype)XTRAN(*pi++);
      lpt2.y = (Lptype)YTRAN(*pi++);
   }
   else /* REAL vdc */
   {
      lpt1.x = (Lptype)XTRAN(*pr++);
      lpt1.y = (Lptype)YTRAN(*pr++);
      lpt2.x = (Lptype)XTRAN(*pr++);
      lpt2.y = (Lptype)YTRAN(*pr++);
   }

   rect_r(lpt1, lpt2);

   return;
}

/********************************* HPGLcell *****************************/
#ifdef PROTO
static void HPGLcell(Cell cell, Long *pi, Float *pr)
#else
static void HPGLcell(cell, pi, pr)
Cell cell;
Long *pi;
Float *pr;
#endif
/*   draws cell arrays */
{
   Lpt p, q, r;
   Lptype cell_startx, cell_starty, wid, x, y, wadj;
   Double prx, pry, rqx, rqy;
   Float mw;
   Index i, j, k, cur_pen;
   Double cp, mod_pr, mod_qr, perp_x_wid, perp_y_wid;
   RGBcolour col_dir, *incolour;
   Index col_ind, stripes;
   char *parr, *parr_end, *parr_cur, *parr_row_start;
   Logical instripe, *penused;

#ifdef DEBUG_CELL
   fprintf(stderr, "\nHPGLcell: coords are :-\n");
   fprintf(stderr, "(%f,%f) (%f,%f) (%f,%f)\n",
                      cell.p.x, cell.p.y,
                      cell.q.x, cell.q.y,
                      cell.r.x, cell.r.y);
#endif

   p.x=(Lptype)XTRAN(cell.p.x);
   p.y=(Lptype)YTRAN(cell.p.y);
   q.x=(Lptype)XTRAN(cell.q.x);
   q.y=(Lptype)YTRAN(cell.q.y);
   r.x=(Lptype)XTRAN(cell.r.x);
   r.y=(Lptype)YTRAN(cell.r.y);

  /*
   * Work out the width of the array, and hence the width of
   * each cell in the array
   */

  /* calculate cross product */

   prx = (Double)(r.x-p.x)/cell.nx;
   pry = (Double)(r.y-p.y)/cell.nx;
   rqx = (Double)(q.x-r.x)/cell.ny;
   rqy = (Double)(q.y-r.y)/cell.ny;

#ifdef DEBUG_CELL
   fprintf(stderr, "HPGLcell: cell vector PR (%lf,%lf)\n\
          cell vector RQ (%lf,%lf)\n",
           prx, pry, rqx, rqy);
#endif

   cp = HPGLABS(prx*rqy - pry*rqx);

  /* calculate modulus of vectors PR and QR */
   mod_pr = (Double)SQRT(prx*prx+pry*pry);
   mod_qr = (Double)SQRT(rqx*rqx+rqy*rqy);

#ifdef DEBUG_CELL
   fprintf(stderr, "HPGLcell: Cross product = %ld\n\
          length pf PR vector = %lf\n\
          length of RQ vector = %lf\n",
           cp, mod_pr, mod_qr);
#endif

  /* now we have enough to calculate the perpendicular distances */
   perp_x_wid = cp/mod_qr;
   perp_y_wid = cp/mod_pr;

  /* if the cells are too small produce warning and give up
     currently to small means less than 3 pen widths across
     the perpendicular */
   if (perp_x_wid<(3.0*max_pen_width*POINTS_PER_MM) ||
       perp_y_wid<(3.0*max_pen_width*POINTS_PER_MM))
   {
#ifdef DEBUG_CELL
      fprintf(stderr, "HPGLcell: Giving up due to size of cells\n");
#endif
      CGMerror(func, ERR_CELL2SMALL, WARNING, "Cell array abandoned");
     /* Plot outline of array */
      sendcolour((Index)1);
      fprintf(out, "PU%d,%d;PRPD%d,%d,%d,%d,%d,%d,%d,%d;PA;\n",
                   p.x, p.y,
                   (int)prx*cell.nx, (int)pry*cell.nx,
                   (int)rqx*cell.ny, (int)rqy*cell.ny,
                   (int)-prx*cell.nx, (int)-pry*cell.nx,
                   (int)-rqx*cell.ny, (int)-rqy*cell.ny);
      return;
   }

#ifdef DEBUG_CELL
   fprintf(stderr, "HPGLcell: getting memory for pen array\n");
#endif

  /* set up pen used array */
   penused = (Logical *)MALLOC(pens, sizeof(Logical));
  /* set up new colour array */
   parr=(char *)MALLOC(cell.nx*cell.ny, sizeof(char));
  /* If not enough memory then giveup on the cell array */
   if (parr==NULL || penused==0)
   {
#ifdef DEBUG_CELL
      fprintf(stderr, "HPGLcell: Giving up due to lack of memory\n");
#endif
      CGMerror(func, ERR_NOMEMORY, WARNING, "Cell array abandoned");
     /* Plot outline of array */
      sendcolour((Index)1);
      fprintf(out, "PU%d,%d;PRPD%d,%d,%d,%d,%d,%d,%d,%d;PA;\n",
                   p.x, p.y,
                   (int)prx*cell.nx, (int)pry*cell.nx,
                   (int)rqx*cell.ny, (int)rqy*cell.ny,
                   (int)-prx*cell.nx, (int)-pry*cell.nx,
                   (int)-rqx*cell.ny, (int)-rqy*cell.ny);
      return;
   }

#ifdef DEBUG_CELL
   fprintf(stderr, "HPGLcell: setting up pen array\n");
#endif

   for(cur_pen=1; cur_pen<=pens; penused[cur_pen++] = FALSE)
      ;

   parr_end = parr+cell.nx*cell.ny;
   if (cur.color_mode == INDEXED)
      for(parr_cur=parr;parr_cur<parr_end;parr_cur++)
      {
         col_ind = *pi++;
         incolour = COLRtab(col_ind);

        /* If index not defined then default index to stall number */
         if (incolour == NULL)
         {
            *parr_cur = ((col_ind<=pens)?col_ind:(Index)1);
            penused[(int)(*parr_cur)] = TRUE;
         }
         else
         {
            *parr_cur = get_pen_num(BACKMATCH, *incolour);
            penused[(int)(*parr_cur)] = TRUE;
         }
#ifdef DEBUG_CELL
         fprintf(stderr, "HPGLcell: Penmap %d\n", (int)*parr_cur);
#endif
      }
   else /* Direct colour */
      for(parr_cur=parr;parr_cur<parr_end;parr_cur++)
      {
         col_dir.red = *pi++;
         col_dir.green = *pi++;
         col_dir.blue = *pi++;
         *parr_cur = get_pen_num(BACKMATCH, col_dir);
         penused[(int)(*parr_cur)] = TRUE;
#ifdef DEBUG_CELL
         fprintf(stderr, "HPGLcell: Penmap %d\n", (int)*parr_cur);
#endif
      }

#ifdef DEBUG_CELL
   fprintf(stderr, "HPGLcell: starting to draw the array with %d pens\n",
           (int)pens);
#endif

  /*
   * Ok now for each pen
   */
   for(cur_pen=1; cur_pen<=pens; cur_pen++)
   {
     /* If nothing to draw in this pen then skip the lot */
      if (penused[cur_pen] == FALSE)
         continue;

     /* Select pen for this pass */
      sendcolour(cur_pen);

     /* Work out the stripes required for this pen */
      stripes = (int)(perp_y_wid/(pen[cur_pen].width*POINTS_PER_MM));

      if (stripes*pen[cur_pen].width*POINTS_PER_MM < perp_y_wid)
         stripes++;

     /* Work out pen width adjustment for this pen
      wadj = (Lptype)fceil(pen[cur_pen].width*POINTS_PER_MM/2.0);
      wadj = 0; */

     /* Set row start to start of array */
      parr_row_start = parr;

#ifdef DEBUG_CELL
      fprintf(stderr, "HPGLcell: Starting coverage for pen %d\n\
             Stripes per cell = %d\n",
             (int)cur_pen, (int)stripes);
#endif

     /* For each row of cells */
      for(j=0; j<cell.ny; j++)
      {

        /* For each stripe in row */
         for(k=0; k<stripes; k++)
         {
           /* save start of colour map for this row */
            parr_cur = parr_row_start;

           /* set to not be in stripe */
            instripe = FALSE;

           /* For each cell crossed by stripe */
            for(i=0; i<cell.nx; i++)
            {
              /* Calculate invariant part of X coords */
               cell_startx = p.x + i*prx + j*rqx;

              /* Calculate invariant part of Y coords */
               cell_starty = p.y + i*pry + j*rqy;

#ifdef DEBUG_CELL
               if (k==0)
                  fprintf(stderr, "HPGLcell: Cell (%d,%d) is colour %d\n\
          Instripe is %d\n\
          Stripe number is %d\n",
                          (int)i, (int)j, (int)*parr_cur,
                          (int)instripe,
                          (int)k);
#endif

               if (*parr_cur++ == cur_pen)
               {
                  if (instripe==FALSE)
                  {
#ifdef DEBUG_CELL
                     if (k==0)
                        fprintf(stderr, "HPGLcell: Starting stripe at (%d,%d)\n",
                                (int)i, (int)j);
#endif
                     x = (Lptype)(cell_startx + ((double)k*rqx/stripes));
                     y = (Lptype)(cell_starty + ((double)k*rqy/stripes));
                     fprintf(out, "PU%d,%d;", (int)x, (int)y);
                     instripe = TRUE;
                  }
#ifdef DEBUG_CELL
                  else
                  {
                     if (k==0)
                        fprintf(stderr, "HPGLcell: matching colour stripe goes on\n");
                  }
#endif
               }
               else
               {
                  if (instripe==TRUE)
                  {
#ifdef DEBUG_CELL
                     if (k==0)
                        fprintf(stderr, "HPGLcell: Ending stripe at (%d,%d)\n",
                                (int)i, (int)j);
#endif
                     x = (Lptype)(cell_startx + ((double)k*rqx/stripes));
                     y = (Lptype)(cell_starty + ((double)k*rqy/stripes));
                     fprintf(out, "PD%d,%d;\n", (int)x, (int)y);
                     instripe = FALSE;
                  }
#ifdef DEBUG_CELL
                  else
                  {
                     if (k==0)
                        fprintf(stderr, "HPGLcell: lack of stripe goes on\n");
                  }
#endif
               }
            }
            if (instripe)
            {
#ifdef DEBUG_CELL
               if (k==0)
                  fprintf(stderr, "HPGLcell: Ending stripe at (%d,%d)\n",
                          (int)i, (int)j);
#endif
               x = (Lptype)(cell_startx + prx +((double)k*rqx/stripes));
               y = (Lptype)(cell_starty + pry +((double)k*rqy/stripes));
               fprintf(out, "PD%d,%d;\n", (int)x, (int)y);
               instripe = FALSE;
            }
         }
         parr_row_start = parr_cur;
      }
      fprintf(out, "PU;\n");
   }

   FREE(parr);
   FREE(penused);

   return;
}

/****************************** setcolour ******************************/

#ifdef PROTO
static void setcolour(Index index, RGBcolour newcol)
#else
static void setcolour(index, newcol)
Index index;
RGBcolour newcol;
#endif
{
   RGBcolour *incolour;

#ifdef DEBUG_COLOUR
fprintf(stderr, "setcolour: entered\n");
#endif

/* If INDIRECT colour then get the RGB value */
   if (cur.color_mode == INDEXED)
   {
#ifdef DEBUG_COLOUR
fprintf(stderr, "setcolour: looking for indexed colour.\n");
#endif
      incolour = COLRtab(index);

      /* If index not defined then default index to stall number */
      if (incolour == NULL)
      {
#ifdef DEBUG_COLOUR
fprintf(stderr, "setcolour: defaulting index to stall number.\n");
#endif
         sendcolour((index<=pens)?index:(Index)1);
         return;
      }

      newcol = *incolour;
   }

   sendcolour(get_pen_num(NOBACKMATCH, newcol));

   return;
}

/****************************** get_pen_num ******************************/

#ifdef PROTO
static Index get_pen_num(int start, RGBcolour col_dir)
#else
static Index get_pen_num(start, col_dir)
int start;
RGBcolour col_dir;
#endif
{
   Index n,p;
   float diff, temp=4.0;
   float red, green, blue;
/* Now match to the nearest pen colour available */
   for(n=start; n<=pens; n++)
   {
      red = RGBNORM(col_dir,red);
      green = RGBNORM(col_dir,green);
      blue = RGBNORM(col_dir,blue);

      diff = (red - pen[n].colour.red) *
             (red - pen[n].colour.red) +
             (green - pen[n].colour.green) *
             (green - pen[n].colour.green) +
             (blue - pen[n].colour.blue) *
             (blue - pen[n].colour.blue);

     /*
      * If better than OR EQUAL to best previous match then
      * use this pen, equal means that picks pen in preference
      * to backgraound
      */
      if (diff <= temp)
      {
         p = n;
         temp = diff;
      }
   }
   return p;
}

#ifdef PROTO
static void line_r(Long n, Lpt *lptlst)
#else
static void line_r(n, lptlst)
Long n;
Lpt *lptlst;
#endif
{
   Lpt *lptend = lptlst+n, pt, prevpt;
#ifdef DEBUG_LINE
   Long i;
#endif

#ifdef DEBUG_LINE
fprintf(stderr, "line: coords are :-\n");
for(i=0;i<n;i++)
      fprintf(stderr, "(%d,%d)\n", lptlst[i].x, lptlst[i].y);
#endif

   fprintf(out, "PU%d,%d;PR;\n", prevpt.x=lptlst->x, prevpt.y=lptlst->y);

   pt.x = (++lptlst)->x - prevpt.x;
   pt.y = lptlst->y - prevpt.y;
   compoints(NEW, "PD", pt);
   prevpt.x=lptlst->x; prevpt.y=lptlst->y;

   while((++lptlst)<lptend)
   {
      pt.x = lptlst->x - prevpt.x;
      pt.y = lptlst->y - prevpt.y;
      compoints(CONT, "PD", pt);
      prevpt = *lptlst;
   }

   compoints(END, NULLSTR, pt);
   fprintf(out, "PA;PU;\n");

   return;
}

#ifdef PROTO
static void fill_r(Long n, Lpt *lptlst)
#else
static void fill_r(n, lptlst)
Long n;
Lpt *lptlst;
#endif
{
   Lpt *lptend = lptlst+n, pt, prevpt, stpt;
#ifdef DEBUG_FILL
   Long i;
#endif

#ifdef DEBUG_FILL
fprintf(stderr, "line: coords are :-\n");
for(i=0;i<n;i++)
      fprintf(stderr, "(%d,%d)\n", lptlst[i].x, lptlst[i].y);
#endif

   prevpt = *lptlst;
   stpt = prevpt;

   fprintf(out, "PU%d,%d;PR;\n", (int)prevpt.x, (int)prevpt.y);

   pt.x = (++lptlst)->x - prevpt.x;
   pt.y = lptlst->y - prevpt.y;
   compoints(NEW, "PD", pt);
   prevpt.x=lptlst->x; prevpt.y=lptlst->y;

   while((++lptlst)<lptend)
   {
      pt.x = lptlst->x - prevpt.x;
      pt.y = lptlst->y - prevpt.y;
      compoints(CONT, "PD", pt);
      prevpt = *lptlst;
   }

   /* test for closure of the area. if not closed then close it */
   if ((prevpt.x != stpt.x) ||
       (prevpt.y != stpt.y))
   {
#ifdef DEBUG_FILL
      fprintf(stderr, "fill_r: closing unclosed fill area\n");
      fprintf(stderr, "(%d,%d)\n", pt.x, pt.y);
#endif
      pt.x = stpt.x - prevpt.x;
      pt.y = stpt.y - prevpt.y;
      compoints(CONT, "PD", pt);
   }

   compoints(END, NULLSTR, stpt);
   fprintf(out, "PA;PU;\n");

   return;
}

/***************************** rect_r *********************************/
#ifdef PROTO
static void rect_r(Lpt lpt1, Lpt lpt2)
#else
static void rect_r(lpt1, lpt2)
Lpt lpt1, lpt2;
#endif
{
/*
 * Routine to draw CGM rectangle primitive.
 *
 * For now just edge the rectangle in an appropriate colour
 *
 */
#ifdef DEBUG_RECT
fprintf(stderr, "rect: coords are :-\n");
      fprintf(stderr, "(%d,%d)(%d,%d)\n", lpt1.x, lpt1.y, lpt2.x, lpt2.y);
#endif

   fprintf(out, "PU%d,%d;\n", lpt1.x, lpt1.y);
   fprintf(out, "ER%d,%d;\n", lpt2.x-lpt1.x, lpt2.y-lpt1.y);

   return;
}

/*
 * Functions for sending HPGL to the print file
 */
#ifdef PROTO
#else
#endif

/* Function to send out scaling points P1 and P2 to the file */
#ifdef PROTO
static void sendP1P2(Lpt p1, Lpt p2)
#else
static void sendP1P2(p1, p2)
Lpt p1, p2;
#endif
{
   fprintf(out, "IP%d,%d,%d,%d;\n", p1.x, p1.y, p2.x, p2.y);
}

/* Function to send out clipping limits */
#ifdef PROTO
static void sendclip(struct rect cr, int cond)
#else
static void sendclip(cr, cond)
struct rect cr;
int cond;
#endif
{
   Lptype cp1x, cp1y, cp2x, cp2y;
   static Lptype scp1x = 0, scp1y = 0, scp2x = 0, scp2y = 0;
#ifdef DEBUG_CLIP
   int changed=0;
#endif

   if (cur.vdc_type == REAL)
   {
#ifdef DEBUG_CLIP
   fprintf(stderr, "sendclip: Real VDC clip rect is (%f,%f) (%f,%f)\n",
                       cr.a.x.real, cr.a.y.real, cr.b.x.real, cr.b.y.real);
#endif
      cp1x=(Lptype)XTRAN(cr.a.x.real);
      cp1y=(Lptype)YTRAN(cr.a.y.real);
      cp2x=(Lptype)XTRAN(cr.b.x.real);
      cp2y=(Lptype)YTRAN(cr.b.y.real);
   }
   else
   {
#ifdef DEBUG_CLIP
   fprintf(stderr, "sendclip: Integer VDC clip rect is (%d,%d) (%d,%d)\n",
                       cr.a.x.intr, cr.a.y.intr, cr.b.x.intr, cr.b.y.intr);
#endif
      cp1x=(Lptype)XTRAN(cr.a.x.intr);
      cp1y=(Lptype)YTRAN(cr.a.y.intr);
      cp2x=(Lptype)XTRAN(cr.b.x.intr);
      cp2y=(Lptype)YTRAN(cr.b.y.intr);
   }

#ifdef DEBUG_CLIP
   fprintf(stderr, "sendclip: Clip rect is (%d,%d) (%d,%d)\n",
                       cp1x, cp1y, cp2x, cp2y);
#endif

#ifdef DEBUG_CLIP
   if (cp1x<P1.x ||
       cp1y<P1.y ||
       cp2x>P2.x ||
       cp2y>P2.y)
      changed=TRUE;
#endif

   if (cp1x<P1.x) cp1x = P1.x;
   if (cp1y<P1.y) cp1y = P1.y;
   if (cp2x>P2.x) cp2x = P2.x;
   if (cp2y>P2.y) cp2y = P2.y;

#ifdef DEBUG_CLIP
   if (changed)
   {
      fprintf(stderr, "sendclip: Clipping rectangle modified\n");
      fprintf(stderr, "sendclip: Clip rect is (%d,%d) (%d,%d)\n",
                       cp1x, cp1y, cp2x, cp2y);
   }
#endif

   if ((scp1x!=cp1x) ||
       (scp1y!=cp1y) ||
       (scp2x!=cp2x) ||
       (scp2y!=cp2y) ||
       (cond==FORCE))
   {
#ifdef DEBUG_CLIP
      fprintf(stderr, "sendclip: sending out new clip rectangle\n");
#endif
      fprintf(out, "IW%d,%d,%d,%d;\n", cp1x, cp1y, cp2x, cp2y);
      scp1x = cp1x;
      scp1y = cp1y;
      scp2x = cp2x;
      scp2y = cp2y;
   }
}

/* Function to send out new pen instruction */
#ifdef PROTO
static void sendcolour(Index n)
#else
static void sendcolour(n)
Index n;
#endif
{
#ifdef DEBUG_COLOUR
fprintf(stderr, "sendcolour: requesting pen %d.\n", (int)n);
#endif
   if (n==hpglcuratt.pen)
      return;

#ifdef DEBUG_COLOUR
fprintf(stderr, "sendcolour: changing to pen %d.\n", (int)n);
#endif
   if (n)
      fprintf(out, "SP%d;\n", (int)n);
   else
      fprintf(out, "SP;\n");

   hpglcuratt.pen=n;

   return;
}

/* Function to send out new  line style instruction */
#ifdef PROTO
static void sendlinestyle(Index n)
#else
static void sendlinestyle(n)
Index n;
#endif
{
#ifdef DEBUG
fprintf(stderr, "sendlinestyle: (%d)\n", (int)n);
#endif

   if (n==hpglcuratt.linetype)
      return;

   if (n==1)
      fprintf(out, "LT;\n");
   else
      fprintf(out, "LT%d;\n", (int)linestyle[n]);

   hpglcuratt.linetype=n;

   return;
}

/* Function to send out strings of points, with a command */
#ifdef PROTO
static void compoints(int status, char *com, Lpt p)
#else
static void compoints(status, com, p)
int status;
char *com;
Lpt p;
#endif
{
#define LINE_LEN 80
   static char *currcom;
   static int count;
   static char str[LINE_LEN], lineout[LINE_LEN+1], *lopt;
   int len;

   switch(status)
   {
   case NEW: /* new command to use */
      currcom = strcpy((char *)MALLOC(strlen(com)+1, sizeof(char)),com);
      sprintf(lineout, "%s%d,%d", currcom, p.x, p.y);
      len = strlen(lineout);
      count = LINE_LEN - len;
      lopt = lineout + len;
      break;
   case CONT: /* continuation of past command */
      (void)sprintf(str, ",%d,%d", p.x, p.y);
      len=strlen(str);
      if (count>(len+2))
      { /* tag the new point on to the list */
         (void)strcpy(lopt, str);
         count -= len;
         lopt += len;
      }
      else
      {
         fprintf(out, "%s;\n", lineout);
         sprintf(lineout, "%s%d,%d", currcom, p.x, p.y);
         len = strlen(lineout);
         count = LINE_LEN - len;
         lopt = lineout + len;
      }
      break;
   case END: /* end of command */
      fprintf(out, "%s;\n", lineout);
      FREE(currcom);
      currcom=NULL;
      break;
   default:
      fprintf(stderr, "Internal error bad call to compoints\n");
      break;
   }
}
