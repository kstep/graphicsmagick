 /*    CGMTopaz.c

       Routine to interpret CGM elements into data structures suitable for
       the Topaz system.  Calls tpz_ routines for everthing that is
       highly specific to the Topaz.

       Author:  C D Osland

       Origination date: August 1988

       Update log: Mar 89:  CDO  * #include removed as now in mach.h
                                 * everything bar CGMOtopaz split off
                                   into OutTopaz.c
                                 * reaction to clip rectangle and clip
                                   indicator added
                                 * colour range support added
                                 * background colour support added
                                 * escapes, messages and application data
                                   elements noted and counted
                                 * code added to react to ASF changes

 */

#define DRIVER
#define INTERPRET

#include <tpz/topaz_type.h>

#include "/usr/cdo/cgm/mach.h"
#include "/usr/cdo/cgm/cgm.h"

#include "vid.def.h"

#include "vid.cgm.h"
#include "vid.col.h"
#include "vid.mon.h"
#include "vid.opt.h"
#include "vid.vdc.h"

/*  Declare entry points in this module  */

void CGMTopaz();

/*  Declare RALcol routine  */

extern void RALcol();

/*  Declare tpz_attrib routine that are in CGMattrib.c  */

extern void tpz_attrib();

/*  Declare tpz_* routines that are in OutTopaz.c  */

extern void tpz_init(),
            tpz_bpic(),
            tpz_bframe(),
            tpz_eframe(),
            tpz_close(),

            tpz_setvdc(),
            tpz_patt(),
            tpz_cmode(),
            tpz_cset(),

            tpz_line(),
            tpz_marker(),
            tpz_text(),
            tpz_fill(),
            tpz_cells(),
            tpz_gdp(),
            tpz_rect(),

            tpz_merge(),
            tpz_view(),
            tpz_dclut();
extern
Colval      tpz_tocol();

void CGMTopaz (c, pi, pr, str)

Code c;
long *pi;
float *pr;
char *str;

{
   static cont_list = FALSE;
   int i, j, major, num;

#ifdef DEBUG
   fprintf(stderr,"Code into CGMOtopaz : %x\n",c);
#endif

   major = c>>8;
   num = *pi++;

   if ((t_picnumber < t_picstart) && (major != 0x30) && (major != 0x31)) return;

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */

         switch (c)
         {
                      /*   Polyline element:
                       |
                       |   num = number of points ( or -num )
                       |   *pi/*pr  = points list
                       |   If no of points is negative still more to come
                       */

            case LINE:
                 if ( ! cont_list ) tpz_attrib(LINE);
                 cont_list = ( num < 0 );
                 tpz_line (num, pi, pr);
                 break;

                      /*   Disjoint Polyline element:
                       |
                       |   num = number of points ( or -num )
                       |   *pi/*pr  = points list
                       |   If no of points is negative still more to come
                       |   NOTE: pairs must fit in a single buffer
                       */

            case DISJTLINE:
                 if ( ! cont_list ) tpz_attrib(LINE);
                 cont_list = ( num < 0 );
                 for (i = 0; i < abs(num); i += 2, *pi+=4, *pr+=4)
                    tpz_line (2, pi, pr );
                 break;

                      /*   Polymarker element:
                       |
                       |     num = number of markers ( or -num )
                       |     *pi/*pr  = points list
                       |     if no of points is negative still more to come
                       */

            case MARKER:
                 if ( ! cont_list ) tpz_attrib(MARKER);
                 cont_list = ( num < 0 );
                 tpz_marker (num, pi, pr);
                 break;

                      /*   Text element:
                       |
                       |   num     = final or not final
                       |   *pi/*pr = starting point
                       |   str[]   = Text
                       */

            case TEXT:
                 tpz_attrib(TEXT);
                 tpz_text (c, pi, pr, str, num);
                 break;

                      /*   Restricted Text element:
                       |
                       |   num         = final or not final
                       |   *pi/*pr     = width as VDC
                       |   *pi/*pr+1   = height as VDC
                       |   *pi/*pr+2/3 = starting point
                       |   str[]       = Text
                       */

            case RESTRTEXT:
                 tpz_attrib(TEXT);
                 tpz_text (c, pi, pr, str, num);
                 break;

                      /*   Append Text element:
                       |
                       |   num    = final or not final
                       |   str[]  = Text
                       */

            case APNDTEXT:
                 tpz_attrib(TEXT);
                 tpz_text (c, pi, pr, str, num);
                 break;

                      /*   Polygon element:
                       |
                       |   num = number of points ( or -num )
                       |   arr[1>] = points list
                       |   if no of points is negative still more to come
                       */

            case POLYGON:
                 if ( ! cont_list )
                  { tpz_attrib(POLYGON);
                    tpz_attrib(EDGEINDEX);
                  }
                 cont_list = ( num < 0 );
                 tpz_fill (num, pi, pr, NOSET);
                 break;

                      /*   Polygon Set element:
                       |
                       |   num = number of points ( or -num )
                       |   *pi/*pr = points set list
                       |   if no of points is negative still more to come
                       */

            case POLYGONSET:
                 if ( ! cont_list )
                  { tpz_attrib(POLYGON);
                    tpz_attrib(EDGEINDEX);
                  }
                 cont_list = ( num < 0 );
                 tpz_fill (num, pi, pr, SET);
                 break;

                      /* Cell Array element:
                       |
                       | num   = No of points
                       | arr[1-2]   =  point P
                       | arr[3-4]   =  point Q
                       | arr[5-6]   =  point R
                       | arr[7].i   =  nx
                       | arr[8].i   =  ny
                       | arr[9].i   =  Local Colour precision
                       | arr[10].i onwards   =  colour list
                       | (arr[1].i onwards if continuation )
                       */

            case CELLARRAY:
       /*          j = (cont_list ? 1 : 10);    */
                 cont_list = ( num < 0 );
                 tpz_cells (num, pi, pr);
                 break;

                      /*    GDP element:
                       |
                       |    num = no of points
                       |    arr[1].i = GDP type
                       |    arr[2>] = points list
                       |    str      = data record
                       */

            case GDP:
                 j = *pi++;
                 if (j == GKSARC ) tpz_attrib(LINE);  /* RALGKS arc */
                 else
                  { tpz_attrib(POLYGON);
                    tpz_attrib(EDGEINDEX);
                  }
                 tpz_gdp (j, pi, pr, (type == GKSPIE) + (type == GKSCHORD)*2 );
                 break;

                      /*  Rectangle element:
                       |
                       |  iarr[0-3] = two corner points
                       */

            case RECT:
                 tpz_attrib(POLYGON);
                 tpz_attrib(EDGEINDEX);
                 tpz_rect (pi, pr);
                 break;

            default:
                 fprintf(stderr," Invalid Graphics Primitive %x\n", c);
                 break;
          }
          break;

       case 0x30:  /*  Delimiter Elements  */

          switch (c)
          {

            case BEGMF:                /*  Begin Metafile   */
                 tpz_init();           /*  initialise Graphics  */
                 break;

            case BEGPIC:               /*  Begin Picture Descriptor   */
                 strcpy(t_picname,str);
                 t_picnumber++;
                 tpz_bpic();
                 break;

            case BEGPICBODY:           /*  Begin Picture Body  */
                 tpz_bframe();
                 break;

            case ENDPIC:               /*  End  Picture    */
                 tpz_eframe();
                 if (t_picnumber == t_picend)
                  { tpz_close();
                    printf("Output stopped by user at frame %d\n",t_picnumber);
                    exit(0);
                  }
                 break;

            case ENDMF:                /*  End Metafile   */
                 tpz_close();          /*  close Graphics  */
                 break;

            default:
                 fprintf(stderr," Invalid Delimiter %x", c);
          }
          break;


          case 0x31:  /* Metafile Descriptor Elements  */
                      /* Values set in Reader in structure ( cur. )
                       | probably no action here
                       */
          switch (c)
          {
             case MFVERSION:           /*  Metafile version   */
                  t_mfversion = num;
                  printf("Metafile version number %d\n",t_mfversion);
                  break;

             case MFDESC:              /*  Metafile Description  */
                  strcpy(t_mfdesc,str);
                  printf("Metafile description: %s\n",t_mfdesc);
                  break;

             case VDCTYPE:             /*  VDC Type   */

             case INTEGERPREC:         /*  Integer Precision  */

             case REALPREC:            /*  Real Precision   */

             case INDEXPREC:           /*  Index Precision   */
                  break;

             case COLRPREC:            /*  Colour Precision  */
                  if (cur.col_bits > 8)
                   { fprintf(stderr,"Colour precision (%d) too high: ",
                             cur.col_bits);
                     fprintf(stderr,"only 8 bits will be used\n");
                     t_colshift = cur.col_bits - 8;
                   }
                  break;

             case COLRINDEXPREC:       /*  Colour Index Precision  */
                  if (cur.colind_bits > COLOUR_BITS)
                   { fprintf(stderr,"Colour index precision (%d) too high\n",
                             cur.colind_bits);
                     fprintf(stderr,"Indices above %d will fail\n",
                             COLOUR_TAB);
                   }
                  break;

             case MAXCOLRINDEX:        /*  Maximum Colour Index  */
                  if (cur.max_colind > COLOUR_TAB)
                   { fprintf(stderr,"Maximum colour index (%d) too high\n",
                             cur.max_colind);
                     fprintf(stderr,"Only indices up to %d will be serviced\n",
                             COLOUR_TAB);
                   }
                  break;

             case COLRVALUEEXT:        /*  Colour value extent  */
                  t_Rrange = cur.max_rgb.red   - cur.min_rgb.red;
                  t_Grange = cur.max_rgb.green - cur.min_rgb.green;
                  t_Brange = cur.max_rgb.blue  - cur.min_rgb.blue;
                  break;

             case MFELEMLIST:          /*  Metafile element List  */

             case BEGMFDEFAULTS:       /* Begin Metafile defaults Replacement*/

             case ENDMFDEFAULTS:       /* End Metafile defaults Replacement */

             case FONTLIST:            /*  Font List   */

             case CHARSETLIST:         /*  Character set list  */

             case CHARCODING:          /*  Character coding Announcer  */
                 break;
            }
            break;

          case 0x32:  /* Picture Descriptor Elements  */

            switch (c)
            {
              case SCALEMODE:          /*  Scaling Mode   */
                  if (cur.scale_mode == METRIC)
                     fprintf(stderr,"Scaling mode METRIC will be ignored\n");
                  break;

              case COLRMODE:           /*  Colour Selection Mode */

              case LINEWIDTHMODE:      /*  Line width Specification  */

              case MARKERSIZEMODE:     /*  Marker size Specification  */

              case EDGEWIDTHMODE:      /*  Edge width Specification  */
                 break;

              case VDCEXT:             /*  VDC Extent    */
                                       /*  Reacted to in tpz_bframe  */
                 break;

              case BACKCOLR:           /*  Background Colour  */
                 t_background = tpz_tocol(cur.back.red,
                                          cur.back.green,
                                          cur.back.blue);
                 break;

              default:
                 fprintf(stderr," Invalid Picture Descriptor Element: %x");

             }
             break;

          case 0x33:  /* Control Elements  */
            switch(c)
            {
              case VDCINTEGERPREC:     /* VDC Integer Precision  */

              case VDCREALPREC:        /* VDC Real Precision  */

              case AUXCOLR:            /* Auxiliary Colour  */

              case TRANSPARENCY:       /* Transparency  */
                 break;

              case CLIPRECT:           /* Clip Rectangle  */
                 if (cur.vdc_type == REAL)
                  { t_clx1 = convx(cur.clip_rect.a.x.real);
                    t_clx2 = convx(cur.clip_rect.b.x.real);
                    t_cly1 = convx(cur.clip_rect.a.y.real);
                    t_cly2 = convx(cur.clip_rect.b.y.real);
                  }
                 else
                  { t_clx1 = convx((float) cur.clip_rect.a.x.intr);
                    t_clx2 = convx((float) cur.clip_rect.b.x.intr);
                    t_cly1 = convx((float) cur.clip_rect.a.y.intr);
                    t_cly2 = convx((float) cur.clip_rect.b.y.intr);
                   }
                 t_clipchanged = TRUE;
                 break;

              case CLIP:               /* Clip Indicator  */
                 t_clip = (cur.clip_ind == ON);
                 t_clipchanged = TRUE;
                 break;

              default:
                 fprintf(stderr," Invalid Control Element: %x\n");

             }
             break;

         case 0x34:  /* Circles and Ellipses  */
                     /* Handle as GDPs  */

            switch(c)
            {
                        /*      Circle element:
                         |
                         |      arr[0-1]  = centre of circle
                         |      arr[2]    = radius
                         */

              case CIRCLE:
                   tpz_attrib(POLYGON);
                   tpz_attrib(EDGEINDEX);
                   tpz_gdp (CIRCLE, pi, pr, 0);
                   break;

                        /* Circular Arc  3 point
                         |
                         |      arr[0-1]  = starting point
                         |      arr[2-3]  = intermediate point
                         |      arr[4-5]  = ending point
                         */

              case ARC3PT:
                   tpz_attrib(LINE);
                   tpz_gdp (ARC3PT, pi, pr, 0 );
                   break;

                        /* Circular Arc  3 point close
                         |
                         |      arr[0-1]  = starting point
                         |      arr[2-3]  = intermediate point
                         |      arr[4-5]  = endong point
                         |      arr[6].i  = close type ( pie - 0 ; chord - 1 )
                         */

              case ARC3PTCLOSE:
                   tpz_attrib(EDGEINDEX);
                   tpz_attrib(POLYGON);
                   tpz_gdp (ARC3PTCLOSE, pi, pr, *(pi+6)+1 );
                   break;

                        /* Circle Arc centre
                         |
                         |      arr[0-1]  = centre point
                         |      arr[2]    = DX_start
                         |      arr[3]    = DY_start
                         |      arr[4]    = DX_end
                         |      arr[5]    = DY_end
                         |      arr[6]    = radius
                         */

              case ARCCTR:
                   tpz_attrib(LINE);
                   tpz_gdp (ARCCTR, pi, pr, 0);
                   break;

                        /* Circle Arc centre close
                         |
                         |      arr[0-1]  = centre point
                         |      arr[2]    = DX_start
                         |      arr[3]    = DY_start
                         |      arr[4]    = DX_end
                         |      arr[5]    = DY_end
                         |      arr[6]    = radius
                         |      arr[7].i  = close type ( pie - 0 ; chord - 1 )
                         */

              case ARCCTRCLOSE:
                   tpz_attrib(POLYGON);
                   tpz_attrib(EDGEINDEX);
                   tpz_gdp (ARCCTRCLOSE, pi, pr, *(pi+7)+1 );
                   break;

                        /* Ellipse
                         |
                         |    arr[0-1]  = centre point
                         |    arr[2-3]  = endpoint of 1st conjugate diameter
                         |    arr[4-5]  = endpoint of 2nd conjugate diameter
                         */

              case ELLIPSE:
                   tpz_attrib(POLYGON);
                   tpz_attrib(EDGEINDEX);
                   tpz_gdp (ELLIPSE, pi, pr, 0);
                   break;

                        /* Elliptical Arc
                         |
                         |     arr[0-1]  = centre point
                         |     arr[2-3]  = endpoint of 1st conjugate diameter
                         |     arr[4-5]  = endpoint of 2nd conjugate diameter
                         |     arr[6]    = DX_start
                         |     arr[7]    = DY_start
                         |     arr[8]    = DX_end
                         |     arr[9]    = DY_end
                         */

              case ELLIPARC:
                   tpz_attrib(LINE);
                   tpz_gdp (ELLIPARC, pi, pr, 0);
                   break;

                        /* Elliptical Arc close
                         |
                         |    arr[0-1]  = centre point
                         |    arr[2-3]  = endpoint of 1st conjugate diameter
                         |    arr[4-5]  = endpoint of 2nd conjugate diameter
                         |    arr[6]    = DX_start
                         |    arr[7]    = DY_start
                         |    arr[8]    = DX_end
                         |    arr[9]    = DY_end
                         |    arr[10]= close type
                         */

              case ELLIPARCCLOSE:
                   tpz_attrib(POLYGON);
                   tpz_attrib(EDGEINDEX);
                   tpz_gdp (ELLIPARCCLOSE, pi, pr, *(pi+10)+1 );
                   break;

              default:
                   fprintf(stderr," Invalid Output Element: %x\n");

             }
             break;

          case 0x35:  /* Attributes  */

            switch(c)
            {
              case LINEINDEX:       /*  Line Bundle index  */
                 att.line_index = TRUE;
                 break;

              case LINETYPE:       /*  Line Type   */
                 att.line_type = TRUE;
                 break;

              case LINEWIDTH:       /*  Line Width */
                 att.line_width = TRUE;
                 break;

              case LINECOLR:       /*  Line Colour  */
                 att.line_color = TRUE;
                 break;

              case MARKERINDEX:       /*  Marker Bundle index  */
                 att.mark_index = TRUE;
                 break;

              case MARKERTYPE:       /*  Marker Type   */
                 att.mark_type = TRUE;
                 break;

              case MARKERSIZE:       /*  Marker Size */
                 att.mark_size = TRUE;
                 break;

              case MARKERCOLR:       /*  Marker Colour  */
                 att.mark_color = TRUE;
                 break;

              case TEXTINDEX:       /*  Text Bundle index  */
                 att.text_index = TRUE;
                 break;

              case TEXTFONTINDEX:       /*  Text Font Index  */
                 att.text_font = TRUE;
                 break;

              case TEXTPREC:       /*  Text Precision   */
                 att.text_prec = TRUE;
                 break;

              case CHAREXPAN:       /*  Character Expansion Factor  */
                 att.char_expan = TRUE;
                 break;

              case CHARSPACE:       /*  Character Spacing  */
                 att.char_space = TRUE;
                 break;

              case TEXTCOLR:       /*  Text Colour   */
                 att.text_color = TRUE;
                 break;

              case CHARHEIGHT:       /*  Character Height   */
                 att.char_height = TRUE;
                 break;

              case CHARORI:       /*  Character Orientation */
                 att.char_orient = TRUE;
                 break;

              case TEXTPATH:       /*  Text Path */
                 att.text_path = TRUE;
                 break;

              case TEXTALIGN:       /*  Text Alignment */
                 att.text_align = TRUE;
                 break;

              case CHARSETINDEX:      /*  Character Set Index */
                 att.char_set = TRUE;
                 break;

              case ALTCHARSETINDEX:   /*  Altern Character Set Index */
                 att.altchar_set = TRUE;
                 break;

              default:
                 fprintf(stderr," Invalid Attribute: %x\n");

             }
             break;

          case 0x36:  /* More Attributes  */
            switch(c)
            {
              case FILLINDEX:       /*  Fill Bundle index  */
                 att.fill_index = TRUE;
                 break;

              case INTSTYLE:       /*  Interior Style  */
                 att.int_style = TRUE;
                 break;

              case FILLCOLR:       /*  Fill Colour */
                 att.fill_color = TRUE;
                 break;

              case HATCHINDEX:       /*  Hatch Index  */
                 att.hatch_ind = TRUE;
                 break;

              case PATINDEX:       /*  Pattern Index  */
                 att.pat_ind = TRUE;
                 break;

              case EDGEINDEX:       /*  Edge Bundle index  */
                 att.edge_index = TRUE;
                 break;

              case EDGETYPE:       /*  Edge Type  */
                 att.edge_type = TRUE;
                 break;

              case EDGEWIDTH:       /*  Edge Width */
                 att.edge_width = TRUE;
                 break;

              case EDGECOLR:       /*  Edge Colour  */
                 att.edge_color = TRUE;
                 break;

              case EDGEVIS:       /*  Edge Visibility  */
                 att.edge_vis = TRUE;
                 break;

              case FILLREFPT:       /*  Fill Reference Point  */
                 att.fill_ref = TRUE;
                 break;

              case PATTABLE:       /*  Pattern Table  */
                 tpz_patt();
                 break;

              case PATSIZE:       /*  Pattern Size  */
                 att.pat_size = TRUE;
                 break;

              case COLRTABLE:       /*  Colour Table  */
                 tpz_cset();
                 t_colchanged = TRUE;
                 break;

              case ASF:       /*  Aspect source flags  */
                 for (i=0; i<num; i++)
                  { int type;
                    type = *pi++;
                    *pi++;
                    switch (type)
                     { case 0: case 1: case 2: case 510:
                               att.line_index = TRUE;
                               break;

                       case 3: case 4: case 5: case 509:
                               att.mark_index = TRUE;
                               break;

                       case 6: case 7: case 8: case 9: case 10: case 508:
                               att.text_index = TRUE;
                               break;

                       case 11: case 12: case 13: case 14: case 507:
                               att.fill_index = TRUE;
                               break;

                       case 15: case 16: case 17: case 506:
                               att.edge_index = TRUE;
                               break;

                       case 511:
                               att.line_index = att.mark_index =
                               att.text_index = att.fill_index =
                               att.edge_index = TRUE;
                               break;

                       default: fprintf(stderr,"Unknown ASF %d\n",type);
                     }
                  }
                 break;

              default:
                 fprintf(stderr," Invalid Attribute: %x\n");

             }
             break;

          case 0x37:  /* Escape And External Elements  */
            switch (c)
            {
              case ESCAPE:         /*  Escape  */
#ifdef DEBUG
                fprintf(stderr,"Escape element %d ignored\n",num);
#endif
                t_nescapes++;
                break;

              case MESSAGE:         /*  Message  */
                fprintf(stderr,"Message ignored - %s\n",str);
                t_nmessages++;
                break;

              case APPLDATA:         /*  Application Data  */
                fprintf(stderr,"Application data record %d ignored\n",num);
                t_nappldata++;
                break;

              default:
                fprintf(stderr," Invalid Escape/External Element: %x", c);
            }
            break;

        default:
                fprintf(stderr," Invalid Element: %x", c);
     }

     return;
  }
