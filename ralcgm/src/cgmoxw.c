/*   RAL-CGM Interpreter module:  @(#) cgmoxw.c  version 3.4
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
 * Description: Output driver for X Window system release 11
 *
 * Author:  A R Mayhook
 *
 * Maintenance Log:
 *
 *   3 Jul 89 ARM  First Version
 *  25 Jul 89 ARM  Widgets
 *  20 Oct 89 RTP  Convert to use new pointers for input lists
 *  30 Jan 90 ARM  Version not using Widgets
 *  17 Aug 90 RTP  Add error handler Routine
 *  31 Aug 90 NMH  Added VMS bits and use of text utilities
 *  12 Sep 90 RTP  Change GDP routine names and change all
 *                 struct parameters to pointers
 *  17 Sep 90 CDO  Added (char *) casts to various getenv calls
 *  10 Oct 90 NMH  Added reaction to -g RAL GDP option
 *  to 2 Nov 90    Added use of attribute checking routine ATTtext
 *                 for use with text utilities.
 *                 Added inquiring available font list from X
 *                 server instead of using external file.
 *                 Revamped Text drawing so that all aligment is
 *                 done by text utilities EVEN for hardware text.
 *                 STRING text done simpler than CHAR and STROKE.
 *                 Hardware font used is given by Font match
 *                 system so the external alias file is no longer
 *                 required. Do not react to fontlist entry.
 *                 Added determination of screen resolution for use
 *                 in request for available fonts ie only get
 *                 those targetted at screen in use. Also only
 *                 ask for ISO character encodings.
 *  21 Nov 90 NMH  Hershey text method added. Text methods now
 *                 supported are X fonts, Hershey and Bezier.
 *  28 Nov 90 NMH  Moved setup of fontlist in Xw_init to within the
 *                 first call to Xw_init only section so that it
 *                 isn't repeated at the start of each metafile.
 *                 Added response to NONOP element.
 *   7 Dec 90 RTP  Use Point types where appropriate
 *   7 Dec 90 NMH  Changed do_not_propagate_mask to NoEventMask in
 *                 Xw_crwin for XCreateWindow. Was = -1
 *  10 Dec 90 NMH  Fixed use of Point types by adding them to Xw_beztxt and
 *                 Xw_hertxt and Xw_stktxt.
 *  11 Dec 90 NMH  Removed check on screen resolution in requesting fonts.
 *                 This is not necessary as we are only interested in pixel
 *                 sizes. The correct fonts are only required for correct
 *                 point size fonts.
 *  14 Dec 90 RTP  Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *   7 Jan 91 NMH  Change index values for RESTRTEXT text pos to 2 & 3
 *  15 Mar 91 RTP  Change cgmstate to Enum type
 *   1 May 91 CDS  Added new character flag to routines Xw_beztxt, Xw_hertxt,
 *                 Xw_stktxt. This flag is not used by the X driver
 *                 and is included for compatibility only.
 *  16 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change calloc calls to use MALLOC macro and cast to
 *                 (size_t) where necessary
 *  22 Jul 91 NMH  Changed window title to be ralcgm version from cgmview.
 *                 added messages when getting font info
 *                 added aborting/aborted messages
 *  31 Jul 91 NMH  Moved freeing pointer to a window to after calling
 *                 XDestroyWindow with it and not before !!!
 *                 added use of pixmap for main picture backing store if no
 *                 backing store on server.
 *   2 Aug 91 NMH  Only initialize font system when required rather than always
 *                 at startup.
 *                 Respond to all expose events in picture window not just the
 *                 last. Only respond to the last expose for other windows.
 *  13 Sep 91 RTP  Add SCCS id
 *                 Change Xsync to XSync
 *  25 Sep 91 RTP  Remove names from prototypes for consistency with headers
 *                 Add break after GKSCIRCLE
 *   8 Oct 91 RTP  Fix bug 35 - Cell Arrays with integer VDCs not working
 *  24 Aug 92 RTP  Correct points selection for RALGKS Circle GDP
 *   5 Nov 92 RTP  Correct function argument errors reported by Cray
 *  27 Oct 92 CDS  Add check to prevent overflow in 'calculator' display.
 *  15 Jan 93 RTP  Add #define CGMOXW_C to avoid including X11 Headers
 *  20 Apr 93 RTP  Change xmin & ymin in Xw_setwin to float to handle GKS
 *                 Add Xw_metric as flag to obey metric scaling mode
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *
 */

/*
      Limitations

     1)  The following not supported
             ESCAPE
             APPLICATION DATA
      2)  The FILL REFERENCE POINT is fixed at the top left hand
             corner of the window

      If font matching system selects a Hardware font:
      3)  CHARACTER ORIENTATION is supported with the restriction
             that characters are always plotted upright
      4)  CHARACTER HEIGHT is supported by selecting the nearest size
             available from a fixed set of supported raster fonts. Done by
             the font match system NOT by this driver.


 */

#define CGMOXW_C
#include "cgmout.h"

#include "cgmtext.h"
#include "cgmatt.h"
#include "cgmfont.h"
#include "cgmxw.h"                       /*  Local variables for X11  */

#include "cgmgdp.h"
#ifdef BEZIER
#include "cgmbez.h"
#endif
#ifdef HERSHEY
#include "cgmher.h"
#endif
                               /*  X11  driver functions   */
#ifdef PROTO
void CGMOXw(Code, Long*, Float*, char*);
                                               /* Main driver routine */
void Xw_options(char*),                            /* Parse X options */
     Xw_getfd (Textitem*);
                         /* get hardware font info for text utilities */
static void
     Xw_init(long*),                        /* Initialisation routine */
     Xw_close(long),                           /* Close down Graphics */
     Xw_windows(struct Xwin_rec *),                 /* Create Windows */
     Xw_firstp( void ),                         /* first picture init */
     Xw_newpic(long*, char*),                    /* Start a new frame */
     Xw_attrib(short, Code),                        /* Set Attributes */
     Xw_line(int, Long*, Float* ),                 /* Draw a polyline */
     Xw_marker(int, Long*, Float* ),              /* Draw polymarkers */
     Xw_txtdraw(Textitem*, Point),       /* front end of text drawing */
     Xw_beztxt(int, Point*, Tmatrix, int ),
                                  /* front end to Bezier text drawing */
     Xw_hertxt(int, Point*, Tmatrix, int ),
                                 /* front end to Hershey text drawing */
     Xw_stktxt(int, Point*, Tmatrix, int, int),
           /* stroke text drawing routine for Hershey and Bezier text */
     Xw_hdwtxt(Textitem*, Point),               /* Draw hardware text */
     Xw_setgran( void ), /* Set screen granularity for the text utils */
     Xw_cell( Long*, Float* ),                    /* Draw cell arrays */
     Xw_gdp(short, Long*, Float*, Code),                  /* Draw gdp */
     Xw_poly(int, Long*, Float*, Enum, Code),                 /* fill */
     Xw_segment(int, Long*, Float* ),               /* disjoint lines */
     Xw_inmenu( void ),                               /* init menu    */
     Xw_inkb( void ),                                 /* init kb      */
     Xw_set_default( void ),             /* reset defaults for BEGPIC */
     Xw_pattab( Long* ),                /* add entries to pattern tab */
     Xw_maketile(int),                  /* make a tile from a pattern */
     Xw_copycolr(XColor*, struct colour *),          /* copy rgb vals */
     Xw_setwin(int),                         /* set window parameters */
     Xw_resize(Window, struct Xwin_rec *, XEvent*),
                                                  /* get window parms */
     Xw_butn(Window, struct Xwin_rec *, XEvent*),
                                                      /* zoom buttons */
     Xw_newview(Window, struct Xwin_rec *),            /* new windows */
     Xw_outmesg(char*, char*),                     /* message to user */
     Xw_cmnd(Window, struct Xwin_rec *, XEvent*),
                                                    /* cmd button rtn */
     Xw_kb(Window, char*, struct Xwin_rec *, XEvent*),
                                                    /*  kb button rtn */
     Xw_menu(Window, struct Xwin_rec *, XEvent*),
                                                   /* menu button rtn */
     Xw_dispatch(XEvent*),                        /* event dispatcher */
     Xw_setwsiz(Window),                   /* set parms for menu text */
     Xw_basics( void ),                 /* open display and get parms */
     Xw_event(int),                          /* process window events */
     Xw_fillpix(Pixmap, unsigned long);
                      /* fill a pixmap. used for pixmap backing store */
static unsigned long
     Xw_getpix(XColor*, int);                  /* get pixel for colour*/
static Window
     Xw_crwin(Window,int,int,unsigned int,
              unsigned int,unsigned long, unsigned long );
                                                  /* Create Windows   */

#else
void CGMOXw();                 /* Main driver routine            */
void Xw_options(),             /* Parse X options                */
     Xw_getfd();    /* get hardware font info for text utilities */
static void
     Xw_init(),                /* Initialisation routine         */
     Xw_close(),               /* Close down Graphics            */
     Xw_windows(),             /* Create Windows                 */
     Xw_firstp(),              /* fist picture init              */
     Xw_newpic(),              /* Start a new frame              */
     Xw_attrib(),              /* Set Attributes                 */
     Xw_line(),                /* Draw a polyline from Data Area */
     Xw_marker(),              /* draw polymarkers               */
     Xw_txtdraw(),             /* front end text routine         */
     Xw_beztxt(),            /* front end to Bezier text drawing */
     Xw_hertxt(),           /* front end to Hershey text drawing */
     Xw_stktxt(),
      /* stroke text drawing routine for Hershey and Bezier text */
     Xw_hdwtxt(),                          /* Draw hardware text */
     Xw_setgran(),
            /* Set the screen granularity for the text utilities */
     Xw_cell(),                /* Draw cell arrays               */
     Xw_gdp(),                 /* draw GDPs                      */
     Xw_poly(),                /* polygon fill                   */
     Xw_segment(),             /* disjoint lines                 */
     Xw_inmenu(),              /* init menu                      */
     Xw_inkb(),                /* init kb                        */
     Xw_set_default(),         /* reset defaults for BEGPIC      */
     Xw_pattab(),              /* add entries to pattern table   */
     Xw_maketile(),            /* generate a tile from a pattern */
     Xw_copycolr(),            /* copy and scale rgb values      */
     Xw_setwin(),              /* set window parameters          */
     Xw_resize(),              /* get window parameters          */
     Xw_butn  (),              /* zoom button processing         */
     Xw_newview(),             /* new set of windows             */
     Xw_outmesg(),             /* message to user                */
     Xw_cmnd(),                /* commmand button processing     */
     Xw_kb(),                  /* kb button rtn                  */
     Xw_menu(),                /* menu button processing         */
     Xw_dispatch(),            /* event dispatcher               */
     Xw_setwsiz(),             /* set parms for menu text        */
     Xw_basics(),              /* open display and get parms     */
     Xw_event(),               /* process window events          */
     Xw_fillpix();             /* fill a backing store pixmap    */
static unsigned long
     Xw_getpix();              /* Convert colour to pixel        */
static Window
     Xw_crwin();               /* Create Windows                 */

extern char  *getenv();

#endif

/* Local Variables */
static Point Xw_txt;
static char *func="CGMoxw", mess[48];
static int rt_mode;
static double rt_wid, rt_hgt;

/**************************************************** CGMOXw ********/

#ifdef PROTO
void CGMOXw ( Code c, Long *pi, Float *pr, char *str )
#else
void CGMOXw ( c, pi, pr, str )

/*  Outputs Metafile Element 'c' to the Xwindow system       */

Code c;
Long *pi;
Float *pr;
char *str;
#endif
{
   static Logical first = TRUE, newpic= TRUE;
   static long pic = ZERO;
   static char pic_name[32];
   register int i, j, maj;
   long num;
   XColor *p;
   int fnt_meths[2], fnt_fonts[2];
   Textitem *titem;
   Enum temp_text_bun;
   Index temp_font_id;

#ifdef DEBUG
   DMESS "\n  Op code: 0x%x : ", c);
#endif

   if (c == (Code) EOF) return;

   if ( cgmstate == PIC_OPEN )
   {
     Xw_event(NOWAIT);
     if (cgmabort) return;
   }

   maj = c>>8;
   num = *pi++;

   switch (maj)
   {
      case 0x00:               /* Graphics Primitives  */

         if ( newpic )
         {
            Xw_newpic(&pic, pic_name);
            newpic = FALSE;
         }

         switch (c)
         {
            case LINE:         /*  Polyline   */
               if ( first ) Xw_attrib(XLINE,c);
               Xw_line ( (int) num, pi, pr );
               first = ( num >= ZERO );
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
               if ( first ) Xw_attrib(XLINE,c);
               Xw_segment ( (int) num, pi, pr);
               first = ( num >= ZERO );
               break;

            case MARKER:         /*  Polymarker  */
               if ( first ) Xw_attrib(XMARK,c);
               Xw_marker ( (int) num, pi, pr);
               first = ( num >= ZERO );
               break;

/* textpoint */
            case TEXT:         /*  Text   */
               if(!FNTXlist_called)
                 {
                   char copy_msg[XMESGLEN];

                   strcpy(copy_msg,Xw_curwin->mesgbuff);
                   sprintf(Xw_curwin->mesgbuff,
                   "Initializing font system with font list from X server....");
                   XClearWindow(display,Xw_curwin->tx.win);
                   XDrawString(display,Xw_curwin->tx.win,gc_men,
                          Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
                          Xw_curwin->mesgbuff,strlen(Xw_curwin->mesgbuff));
                   XSync(display,FALSE);

                   FNTXlist(XNFONT,Xfnt_serv); /* init fontmatch with X fonts */
                   FNTXlist_called = TRUE;

                   strcpy(Xw_curwin->mesgbuff,copy_msg);
                   XClearWindow(display,Xw_curwin->tx.win);
                   XDrawString(display,Xw_curwin->tx.win,gc_men,
                          Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
                          Xw_curwin->mesgbuff,strlen(Xw_curwin->mesgbuff));
                 }

               if (cur.vdc_type == REAL)
                 {
                  Xw_txt.x = pr[0];
                  Xw_txt.y = pr[1];
                 }
               else
                 {
                  Xw_txt.x = (float) pi[0];
                  Xw_txt.y = (float) pi[1];
                 }
               rt_mode=0;
               Xw_setgran();
               ATTtext(XNTBUN, XWtext_bundle, XNCOLOR, &Xwtxtatt);
               titem = TXTaccinf(num, str, &Xwtxtatt, Xprefmeth);
               if (num == FINAL) Xw_txtdraw(titem, Xw_txt);
               break;

            case RESTRTEXT:         /*  Restricted Text */
               if(!FNTXlist_called)
                 {
                   char copy_msg[XMESGLEN];

                   strcpy(copy_msg,Xw_curwin->mesgbuff);
                   sprintf(Xw_curwin->mesgbuff,
                   "Initializing font system with font list from X server....");
                   XClearWindow(display,Xw_curwin->tx.win);
                   XDrawString(display,Xw_curwin->tx.win,gc_men,
                          Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
                          Xw_curwin->mesgbuff,strlen(Xw_curwin->mesgbuff));
                   XSync(display,FALSE);

                   FNTXlist(XNFONT,Xfnt_serv); /* init fontmatch with X fonts */
                   FNTXlist_called = TRUE;

                   strcpy(Xw_curwin->mesgbuff,copy_msg);
                   XClearWindow(display,Xw_curwin->tx.win);
                   XDrawString(display,Xw_curwin->tx.win,gc_men,
                          Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
                          Xw_curwin->mesgbuff,strlen(Xw_curwin->mesgbuff));
                 }


               if (cur.vdc_type == REAL)
                 {
                  Xw_txt.x = pr[2];
                  Xw_txt.y = pr[3];
                  rt_wid= (double)pr[0];
                  rt_hgt= (double)pr[1];
                 }
               else
                 {
                  Xw_txt.x = (float) pi[2];
                  Xw_txt.y = (float) pi[3];
                  rt_wid= (double)pi[0];
                  rt_hgt= (double)pi[1];
                 }
               rt_mode=1;
               Xw_setgran();
               ATTtext(XNTBUN, XWtext_bundle, XNCOLOR, &Xwtxtatt);
               titem = TXTaccinf(num, str, &Xwtxtatt, Xprefmeth);
               if (num == FINAL) Xw_txtdraw(titem, Xw_txt);
               break;

            case APNDTEXT:         /*  Append Text   */
               Xw_setgran();
               ATTtext(XNTBUN, XWtext_bundle, XNCOLOR, &Xwtxtatt);
               titem = TXTaccinf(num, str, &Xwtxtatt, Xprefmeth);
               if (num == FINAL) Xw_txtdraw(titem, Xw_txt);
               break;

            case POLYGON:         /*  Polygon   */
               Xw_poly ( (int)num, pi, pr, NOSET, c);
               break;

            case POLYGONSET:         /*  Polygon Set  */
               Xw_poly ( (int)num, pi, pr, SET, c);
               break;

            case CELLARRAY:           /*  Cell Array  */
               Xw_cell( pi-1, pr);
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               if(cgmralgks)  /*  only recognise RAL GKS GDP's   */
                 Xw_gdp ( (short)*pi, pi+1, pr, c);
               else
               {
                 (void) sprintf ( mess, "(type: %d)", (short)*pi);
                 (void) CGMerror ( func, ERR_NOGDP, ERROR, mess);
               }
               break;

            case RECT:         /*  Rectangle   */
               Xw_gdp (RECT, pi, pr, c);
               break;

            case NONOP:        /*  No op or NULL */
               break;
            default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVPRIM, ERROR, mess);
               break;
         }
         break;

      case 0x30:  /*  Delimiter Elements  */

         switch (c)
         {

            case BEGMF:         /*  Begin Metafile   */
               /* Make sure shared attributes are being picked up */
               Xwtxtatt.shared = &share;
               Xw_init(&pic);
               Xfirst_pic = TRUE;
               Xflag.line_width = Xflag.mark_size = TRUE;
               Xflag.edge_width = Xflag.char_height = TRUE;
               break;

            case ENDMF:         /*  End Metafile   */
               Xw_close(pic);
               break;

            case BEGPIC:        /*  Begin Picture Descriptor   */
               Xw_outmesg(str,"");
               if (Xfirst_pic)
               {
                  Xflag.def_line_width = Xflag.line_width;
                  Xflag.def_mark_size = Xflag.mark_size;
                  Xflag.def_edge_width = Xflag.edge_width;
                  Xflag.def_char_height = Xflag.char_height;
                  Xw_firstp();
                  Xfirst_pic = FALSE;
               }
               Xw_set_default();
               Xflag.line = Xflag.mark =  TRUE;
               Xflag.edge = Xflag.fill = TRUE;
               Xflag.line_width = Xflag.def_line_width;
               Xflag.mark_size = Xflag.def_mark_size;
               Xflag.edge_width = Xflag.def_edge_width;
               Xflag.char_height = Xflag.def_char_height;
               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               newpic = TRUE;   /*  defer processing till first draw */
               break;

            case ENDPIC:         /*  End  Picture    */
               Xw_event(WAIT);
               break;

           default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVDELIM, ERROR, mess);
               break;
         }
         break;


      case 0x31:  /* Metafile Descriptor Elements  */
         switch (c)
         {
            case MFVERSION:         /*  Metafile version   */

            case MFDESC:         /*  Metafile Description  */

            case VDCTYPE:         /*  VDC Type   */

            case INTEGERPREC:         /*  Integer Precision  */

            case REALPREC:         /*  Real Precision   */

            case INDEXPREC:         /*  Index Precision   */

            case COLRPREC:         /*  Colour Precision */

            case COLRINDEXPREC:    /*  Colour Index Precision  */
               break;

            case MAXCOLRINDEX:     /*  Maximum Colour Index  */
               if (cur.max_colind >= XNCOLOR)
               {
                  p = (XColor *) MALLOC (  cur.max_colind, sizeof(XColor) );
                  for (i=0; i<cur.max_colind; i++)
                  {
                     if (i < XNCOLOR) p[i] = Xcolrtab[i];
                     else p[i] = Xbackcolr;
                  }
                  FREE(Xcolrtab);
                  Xcolrtab = p;
               }
               XNCOLOR = cur.max_colind;
               break;

            case COLRVALUEEXT:     /*  Colour value extent  */
               Xcolourscale.red = 65535.0/
                          ( (float) (cur.max_rgb.red-cur.min_rgb.red));
               Xcolourscale.green = 65535.0/
                         ((float) (cur.max_rgb.green-cur.min_rgb.green));
               Xcolourscale.blue = 65535.0/
                          ( (float) (cur.max_rgb.blue-cur.min_rgb.blue));
               break;

            case MFELEMLIST:       /*  Metafile element List  */

            case BEGMFDEFAULTS: /* Begin Metafile defaults Replacement*/
               break;

            case ENDMFDEFAULTS: /* End Metafile defaults Replacement */
               break;

            case FONTLIST:         /*  Font List  */
               break;

            case CHARSETLIST:  /*  Character set list  */

            case CHARCODING:   /*  Character coding Announcer  */
               break;

           default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVMFDESC, ERROR, mess);
               break;
           }
         break;

      case 0x32:  /* Picture Descriptor Elements  */
         switch (c)
         {
            case SCALEMODE:         /*  Scaling Mode   */
                 Xw_setwin(WORLD);
                 break;

            case COLRMODE:         /*  Colour Selection Mode */
                 break;

            case LINEWIDTHMODE:    /*  Line width Specification  */

            case MARKERSIZEMODE:   /*  Marker size Specification  */

            case EDGEWIDTHMODE:     /*  Edge width Specification  */
               break;

            case VDCEXT:         /*  VDC Extent    */
               Xw_setwin(WORLD);
               Xflag.line = Xflag.mark =  TRUE;
               Xflag.edge = Xflag.fill = TRUE;
               break;

            case BACKCOLR:       /*  Background Colour  */
              Xw_copycolr(&Xbackcolr,&cur.back);
              Xbackcolr.pixel = -1;
              Xw_getpix(&Xbackcolr,ASIS);
              XSetWindowBackground(display,picture,Xbackcolr.pixel);
              XClearWindow(display,picture);
              if((Xw_backing_store != Always))
                Xw_fillpix(back_pix,Xbackcolr.pixel);
              Xcolrtab[0] = Xbackcolr;
              break;

            default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVPDESC, ERROR, mess);
         }
         break;

      case 0x33:  /* Control Elements  */
         switch(c)
         {
            case VDCINTEGERPREC:       /* VDC Integer Precision  */

            case VDCREALPREC:       /* VDC Real Precision  */
                break;

            case AUXCOLR:       /* Auxiliary Colour  */
               Xw_copycolr(&Xauxcolr,&cur.aux);
               Xauxcolr.pixel = -1;
               Xflag.line = Xflag.mark = TRUE;
               Xflag.fill =  TRUE;
               break;

            case TRANSPARENCY:       /* Transparency  */
               Xflag.fill = Xflag.line = TRUE;
               break;

            case CLIPRECT:       /* Clip Rectangle  */
               Xw_setwin(CLIPAREA);
               if ( cur.clip_ind == ON)
               {
                  Xflag.line = Xflag.mark = TRUE;
                  Xflag.fill =  Xflag.edge = TRUE;
               }
               break;

            case CLIP:       /* Clip Indicator  */
               if ( cur.clip_ind != Xclip.clip_ind)
               {
                  Xw_setwin(CLIPAREA);
                  Xflag.line = Xflag.mark = TRUE;
                  Xflag.fill =  Xflag.edge = TRUE;
               }
               break;

            default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVCONT, ERROR, mess);
         }
         break;

      case 0x34:  /* Circles and Ellipses  */

         if ( newpic )
         {
            Xw_newpic(&pic, pic_name);
            newpic = FALSE;
         }

         switch(c)
         {
            case CIRCLE:       /* Circle      */
               Xw_gdp (CIRCLE, pi, pr, c);
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               Xw_gdp (ARC3PT, pi, pr, c);
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               Xw_gdp (ARC3PTCLOSE, pi, pr, c);
               break;

            case ARCCTR:       /* Circle Arc centre */
               Xw_gdp (ARCCTR, pi, pr, c);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               Xw_gdp (ARCCTRCLOSE, pi, pr, c);
               break;

            case ELLIPSE:       /* Ellipse    */
               Xw_gdp (ELLIPSE, pi, pr, c);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               Xw_gdp (ELLIPARC, pi, pr, c);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close*/
               Xw_gdp (ELLIPARCCLOSE, pi, pr, c);
               break;

            default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVOUT, ERROR, mess);
         }
         break;

      case 0x35:  /* Attributes  */
         switch(c)
         {

            case LINEWIDTH:       /*  Line Width */
               Xflag.line_width = FALSE;
            case LINEINDEX:       /*  Line Bundle index  */
            case LINETYPE:       /*  Line Type   */
            case LINECOLR:       /*  Line Colour  */
               Xflag.line = TRUE;
               break;

            case MARKERSIZE:       /*  Marker Size */
               Xflag.mark_size = FALSE;
            case MARKERINDEX:       /*  Marker Bundle index  */
            case MARKERTYPE:       /*  Marker Type   */
            case MARKERCOLR:       /*  Marker Colour  */
               Xflag.mark = TRUE;
               break;

            case CHARHEIGHT:    /*  Character Height   */
               Xflag.char_height = FALSE;
            case TEXTPATH:       /*  Text Path */
            case TEXTALIGN:       /*  Text Alignment */
            case TEXTINDEX:       /*  Text Bundle index  */
            case TEXTFONTINDEX:       /*  Text Font Index  */
            case TEXTCOLR:       /*  Text Colour   */
            case CHARORI:       /*  Character Orientation */
            case CHARSPACE:       /*  Character Spacing  */
            case TEXTPREC:       /*  Text Precision   */
            case CHAREXPAN:   /*  Character Expansion Factor */
            case CHARSETINDEX:      /*  Character Set Index  */
            case ALTCHARSETINDEX: /*  Altern Character Set Index */
               break;

            default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVATT, ERROR, mess);
               break;
         }
         break;

      case 0x36:  /* More Attributes  */
         switch(c)
         {
            case FILLINDEX:       /*  Fill Bundle index  */
            case INTSTYLE:       /*  Interior Style  */
            case FILLCOLR:       /*  Fill Colour */
            case HATCHINDEX:       /*  Hatch Index  */
            case PATINDEX:       /*  Pattern Index  */
               Xflag.fill = TRUE;
               break;

            case EDGEWIDTH:       /*  Edge Width */
               Xflag.edge_width = FALSE;
            case EDGEINDEX:       /*  Edge Bundle index  */
            case EDGETYPE:       /*  Edge Type  */
            case EDGECOLR:       /*  Edge Colour  */
               Xflag.edge = TRUE;
               break;

            case EDGEVIS:       /*  Edge Visibility  */

            case FILLREFPT:       /*  Fill Reference Point  */
               break;

            case PATTABLE:       /*  Pattern Table  */
               Xw_pattab( pi-1 );
               break;

            case PATSIZE:       /*  Pattern Size  */
               for (i=0; i<=XNTILE; i++)
               {
                  if (Xfill.tiles[i].pm != -1)
                  {
                     XFreePixmap(display,Xfill.tiles[i].pm);
                     Xfill.tiles[i].pm = -1;
                  }
               }
               break;

            case COLRTABLE:       /*  Colour Table  */
               i = *pi++;    /*  Colour Index */
               for (j = 0; j < num && i < XNCOLOR; i++,j++)
               {
                  Xcolrtab[i].pixel = -1;
                  Xcolrtab[i].red = rgbtran(*pi++, red);
                  Xcolrtab[i].green = rgbtran(*pi++, green);
                  Xcolrtab[i].blue = rgbtran(*pi++, blue);
               }
               break;

            case ASF:       /*  Aspect source flags  */
               for (i=0; i<ASFS; i++)
               {
                  if (curatt.asf[i] != Xoldasf[i])
                  switch (i)
                  {
                     case 0: case 1: case 2:
                        Xflag.line = TRUE;
                     case 3: case 4: case 5:
                        Xflag.mark = TRUE;
                     case 6: case 9: case 10:
                     case 11: case 12: case 13: case 14:
                        Xflag.fill = TRUE;
                     case 15: case 16: case 17:
                        Xflag.edge = TRUE;
                  }
                  Xoldasf[i] = curatt.asf[i];
               }
               break;

            default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVATT, ERROR, mess);
               break;

         }
         break;

      case 0x37:  /* Escape And External Elements  */
         switch (c)
         {
            case ESCAPE:         /*  Escape  */
               break;

            case MESSAGE:         /*  Message  */
               if (num == ACTION)
               {
                  Xw_outmesg(str,"WAITING");
                  Xw_msgwait = TRUE;
                  Xw_event(NOERROR);
               }
               else Xw_outmesg(str,"");
               break;

            case APPLDATA:      /*  Application Data  */
               break;

            default:
               (void) sprintf ( mess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVESC, ERROR, mess);
               break;
         }
         break;

      default:
         (void) sprintf ( mess, "(code: 0x%x)", c);
         (void) CGMerror ( func, ERR_INVELEM, ERROR, mess);
         break;
     }

     return;
  }

/***************************************************** Xw_options ******/
#ifdef PROTO
void Xw_options( char *opts )
#else
void Xw_options( opts )
char *opts;
#endif

{
   int i, len = strlen(opts);

   if ( opts == NULL | !len ) return;

   for ( i=0; i < len; i++ )
   {
      switch ( toupper( opts[i] ) )
      {
         case 'M':  /*  Metric mode */
           Xw_metric = TRUE;
           break;
         case 'S':  /*  Scaled mode */
           Xw_metric = FALSE;
           break;
         default:
           break;
      }
   }
   return;
}

/**************************************************** XW_ATTRIB *******/

#ifdef PROTO
static void Xw_attrib ( short type, Code c)
#else
static void Xw_attrib (type,c)

/*  Updates X11 GCs from attributes  */

short type;
Code c;
#endif

{

    register short j;
    int i, n, sc_width, linetype, fstyle;
    int size, font, ngen, npart, diff, mindiff, pixht;
    float width;
    unsigned long forepix, backpix;
    Pixmap tile, stipple;
    XColor *colrptr, tempcolr;

#ifdef DEBUG
    DMESS  "sgi_attrib: type 0x%x\n", type);
#endif

    switch (type)
    {
       case XLINE:

          n = curatt.line_ind;
          curatt.line_ind = ( n>XNLBUN ) ? XDLBUN : n;

          if ( !Xflag.line) return;

          if (curatt.asf[1])
              sc_width = Xline.bundle[curatt.line_ind].width * XLDFTW;
          else
          {
             if (cur.linewidth_mode == SCALED)
                   sc_width = curatt.line_width.real * XLDFTW;
             else
             {
                if (cur.vdc_type == REAL)
                       width = curatt.line_width.real;
                else width = curatt.line_width.intr;
                sc_width = width * Xw_curwin->tot_xyscale;
             }
          }
          sc_width = sc_width <= XLMINW ? XLMINW : sc_width;

          if (curatt.asf[0]) n = Xline.bundle[curatt.line_ind].type;
          else n = curatt.line_type;
          n = n>XNDASH ? XDDASH : n;
          linetype = Xdash_list[n].type;
          if (linetype == LineDoubleDash && cur.transparency == ON)
             linetype = LineOnOffDash;

          if (curatt.asf[2])
          {
             j = Xline.bundle[curatt.line_ind].col;
             colrptr = &Xcolrtab[j];
          }
          else
          {
             if (cur.color_mode == INDEXED)
                colrptr = &Xcolrtab[curatt.line.index];
             else
             {
                tempcolr.pixel = -1;
                Xw_copycolr(&tempcolr,&curatt.line);
                colrptr = &tempcolr;
             }
          }
          forepix = Xw_getpix(colrptr,FOREGROUND);
          backpix = Xw_getpix(&Xauxcolr,BACKGROUND);

          XSetForeground(display, gc_line, forepix);
          XSetBackground(display, gc_line, backpix);
          XSetLineAttributes(display, gc_line,
                                 sc_width, linetype,
                                 CapButt, JoinMiter);
          XSetDashes(display, gc_line, 0,
                     Xdash_list[n].dash, Xdash_list[n].num);

          if (Xclip.clip_ind == ON)
               XSetClipRectangles(display, gc_line, 0, 0,
                                 &Xclip.area, 1, Unsorted);
          else XSetClipMask(display,gc_line,None);
          Xflag.line = FALSE;
          break;


      case XMARK:

          n = curatt.mark_ind;
          curatt.mark_ind = ( n>XNMBUN ) ? XDMBUN : n;

          if ( !Xflag.mark) return;

          if (curatt.asf[3]) n = Xmark.bundle[curatt.mark_ind].type;
          else n = curatt.mark_type;
          Xmark.cur_type = n > XNMARK ? XDMARK : n;

          if (curatt.asf[4])
              Xmark.cur_size =Xmark.bundle[curatt.mark_ind].size * XMDFTS;
          else
          {
             if (cur.markersize_mode == SCALED)
                   Xmark.cur_size = curatt.mark_size.real * XMDFTS;
             else
             {
                if (cur.vdc_type == REAL)
                        size = curatt.mark_size.real;
                else  size = curatt.mark_size.intr;
                Xmark.cur_size = size * Xw_curwin->tot_xyscale;
             }
          }

          if (Xmark.cur_size < XMMINS) Xmark.cur_size = XMMINS;
          if (Xmark.cur_size%2 == 0) Xmark.cur_size++;  /* must be odd */

          if (curatt.asf[5])
          {
             j = Xmark.bundle[curatt.mark_ind].col;
             colrptr = &Xcolrtab[j];
          }
          else
          {
             if (cur.color_mode == INDEXED)
                colrptr = &Xcolrtab[curatt.marker.index];
             else
             {
                tempcolr.pixel = -1;
                Xw_copycolr(&tempcolr,&curatt.marker);
                colrptr = &tempcolr;
             }
          }
          forepix = Xw_getpix(colrptr,FOREGROUND);

          XSetForeground(display, gc_mark, forepix);

          if (Xclip.clip_ind == ON)
               XSetClipRectangles(display, gc_mark, 0, 0,
                                 &Xclip.area, 1, Unsorted);
          else XSetClipMask(display,gc_mark,None);
          Xflag.mark = FALSE;
          break;

      case XTEXT:
/* text nolonger handled this way, all done by Text Utilities */
          break;

      case XFILL:  /*  Fill attributes  */

          n = curatt.fill_ind;
          curatt.fill_ind = ( n>XNFBUN ) ? XDFBUN : n;

          if ( !Xflag.fill) return;

          if (curatt.asf[11])
             Xfill.cur_style = Xfill.bundle[curatt.fill_ind].int_style;
          else
             Xfill.cur_style = curatt.int_style;
          switch (Xfill.cur_style)
          {
             case PATTERN:
                  if (curatt.asf[14])
                         n = Xfill.bundle[curatt.fill_ind].pattern;
                  else   n = curatt.pat_ind;
                  n = n>XNTILE ? XDTILE : n;
                  fstyle = FillTiled;
                  if (Xfill.tiles[n].col == NULL) n = XDTILE;
                  if (Xfill.tiles[n].pm == -1) Xw_maketile(n);
                  tile = Xfill.tiles[n].pm;
                  break;
               case HATCH:
                  if (curatt.asf[13])
                         n = Xfill.bundle[curatt.fill_ind].hatch;
                  else   n = curatt.hatch_ind;
                  n = n>XNSTIP ? XDSTIP : n;
                  if (cur.transparency == ON) fstyle = FillStippled;
                  else fstyle = FillOpaqueStippled;
                  stipple = Xfill.stipples[n];
                  break;
               case HOLLOW:
               case EMPTY:
                  if (cur.transparency == OFF)
                    { fstyle = FillTiled;
                      if (Xfill.tiles[0].pm == -1) Xw_maketile(0);
                      tile = Xfill.tiles[0].pm;
                    }
                  else fstyle = FillSolid;
                  break;
               case SOLID:
               default:
                  fstyle = FillSolid;
                  break;
          }


          if (curatt.asf[12])
          {
             j = Xfill.bundle[curatt.fill_ind].col;
             colrptr = &Xcolrtab[j];
          }
          else
          {
             if (cur.color_mode == INDEXED)
                colrptr = &Xcolrtab[curatt.fill.index];
             else
             {
                tempcolr.pixel = -1;
                Xw_copycolr(&tempcolr,&curatt.fill);
                colrptr = &tempcolr;
             }
          }
          forepix = Xw_getpix(colrptr,FOREGROUND);
          backpix = Xw_getpix(&Xauxcolr,BACKGROUND);

     /*  setting tile/stipple origin doesn't seem to work
          if (cur.vdc_type == REAL)
          {
             ts_x = Xtranx(curatt.fill_ref.x.real);
             ts_y = Xtrany(curatt.fill_ref.y.real);
          }
          else
          {
             ts_x = Xtranx(curatt.fill_ref.x.intr);
             ts_y = Xtrany(curatt.fill_ref.y.intr);
          }

          XSetTSOrigin(display,gc_fill,ts_x,ts_y);
    */
          XSetArcMode(display,gc_fill,Xfill.arcmode);
          XSetForeground(display, gc_fill, forepix);
          XSetForeground(display, gc_bound, forepix);
          XSetBackground(display, gc_fill, backpix);
          XSetFillStyle(display,gc_fill,fstyle);
          if (fstyle == FillTiled) XSetTile(display,gc_fill,tile);
          if ((fstyle == FillStippled)||(fstyle == FillOpaqueStippled))
                     XSetStipple(display,gc_fill,stipple);

          if (Xclip.clip_ind == ON)
          {
             XSetClipRectangles(display, gc_fill, 0, 0,
                                 &Xclip.area, 1, Unsorted);
             XSetClipRectangles(display, gc_bound, 0, 0,
                                 &Xclip.area, 1, Unsorted);
          }
          else
          {
             XSetClipMask(display,gc_fill,None);
             XSetClipMask(display,gc_bound,None);
          }

          Xflag.fill = FALSE;
          break;


      case XEDGE:    /*  Edge attributes */

          n = curatt.edge_ind;
          curatt.edge_ind = ( n>XNEBUN ) ? XDEBUN : n;

          if ( !Xflag.edge) return;

          if (curatt.asf[16])
              width = Xedge.bundle[curatt.edge_ind].width* XEDFTW;
          else
          {
             if (cur.edgewidth_mode == SCALED)
                   sc_width = curatt.edge_width.real * XEDFTW;
             else
             {
                if (cur.vdc_type == REAL)
                       width = curatt.edge_width.real;
                else width = curatt.edge_width.intr;
                sc_width = width * Xw_curwin->tot_xyscale;
             }
          }
          sc_width = sc_width<XEMINW ? XEMINW : sc_width;

          if (curatt.asf[15]) n = Xedge.bundle[curatt.edge_ind].type;
          else n = curatt.edge_type;
          n = n>XNDASH ? XDDASH : n;
          linetype = Xdash_list[n].type;
          if (linetype == LineDoubleDash && cur.transparency == ON)
             linetype = LineOnOffDash;

          if (curatt.asf[17])
          {
             j = Xedge.bundle[curatt.edge_ind].col;
             colrptr = &Xcolrtab[j];
          }
          else
          {
             if (cur.color_mode == INDEXED)
                colrptr = &Xcolrtab[curatt.edge.index];
             else
             {
                tempcolr.pixel = -1;
                Xw_copycolr(&tempcolr,&curatt.edge);
                colrptr = &tempcolr;
             }
          }
          forepix = Xw_getpix(colrptr,FOREGROUND);
          backpix = Xw_getpix(&Xauxcolr,BACKGROUND);

          XSetForeground(display, gc_edge, forepix);
          XSetBackground(display, gc_edge, backpix);
          XSetLineAttributes(display, gc_edge,
                                 sc_width, linetype,
                                 CapButt, JoinMiter);
          XSetDashes(display, gc_edge, 0,
                     Xdash_list[n].dash, Xdash_list[n].num);

          if (Xclip.clip_ind == ON)
               XSetClipRectangles(display, gc_edge, 0, 0,
                                 &Xclip.area, 1, Unsorted);
          else XSetClipMask(display,gc_edge,None);
          Xflag.edge = FALSE;
         break;


      default:
         (void) sprintf ( mess, "(code: 0x%x)", c);
         (void) CGMerror ( func, ERR_INVATT, ERROR, mess);
         break;
   }

   return;
}

/**************************************************** XW_NEWPIC ******/

#ifdef PROTO
static void Xw_newpic( long *pic, char *pic_name)
#else
static void Xw_newpic(pic, pic_name)

/*   New Frame processing started at first drawing element  */

long *pic;
char *pic_name;
#endif
{

#ifdef DEBUG
     DMESS "Xw_newpic: pic no %d '%s' \n",
                              *pic, pic_name);
#endif


   Xw_event(NOWAIT);
   if ( Xw_curwin->new_coords) Xw_setwin(SCREEN);

   if (Xflag.line_width  && (cur.linewidth_mode == ABSOLUTE))
     if (cur.vdc_type == REAL)
          curatt.line_width.real = 0.1 * Xw_curwin->dft_size;
     else
     {
        curatt.line_width.intr = 0.1 * Xw_curwin->dft_size;
        if (curatt.line_width.intr < 1) curatt.line_width.intr = 1;
     }
   if (Xflag.edge_width  && (cur.edgewidth_mode == ABSOLUTE))
     if (cur.vdc_type == REAL)
          curatt.edge_width.real = 0.1 * Xw_curwin->dft_size;
     else
     {
        curatt.edge_width.intr = 0.1 * Xw_curwin->dft_size;
        if (curatt.edge_width.intr < 1) curatt.edge_width.intr = 1;
     }
   if (Xflag.mark_size  && (cur.markersize_mode == ABSOLUTE))
     if (cur.vdc_type == REAL)
          curatt.mark_size.real = Xw_curwin->dft_size;
     else
     {
        curatt.mark_size.intr = Xw_curwin->dft_size;
        if (curatt.mark_size.intr < 1) curatt.mark_size.intr = 1;
     }
   if (Xflag.char_height)
     if (cur.vdc_type == REAL)
          curatt.char_height.real = Xw_curwin->dft_size;
     else
     {
        curatt.char_height.intr = Xw_curwin->dft_size;
        if (curatt.char_height.intr < 1) curatt.char_height.intr = 1;
     }

   XClearWindow(display,picture);
   if((Xw_backing_store != Always))
        Xw_fillpix(back_pix,Xcolrtab[0].pixel);

#ifdef DEBUG
     DMESS " Window: %f %f %f %f\n",
                 Xw_curwin->wc_left, Xw_curwin->wc_right,
                 Xw_curwin->wc_bot, Xw_curwin->wc_top );
     DMESS " Screen: %d %d %d %d\n",
                 Xw_curwin->sc_x, Xw_curwin->sc_y,
                 Xw_curwin->sc_w, Xw_curwin->sc_h);
     DMESS " Aspect: %f %f %f \n",
                 Xw_curwin->wc_rat, Xw_curwin->sc_rat, Xw_curwin->scale);
#endif

   return;
}

/*************************************************** XW_POLY *********/

#ifdef PROTO
static void Xw_poly ( int n, Long *pi, Float *pr, Enum set, Code c )
#else
static void Xw_poly ( n, pi, pr, set, c)

/*   draws polygons from n points starting at *pi/*pr  */

int n;
Long *pi;
Float *pr;
Enum set;
Code c;
#endif
{
   static Logical first=TRUE, del_pt=FALSE;
   short cont, j, k;
   long prevtype, lasttype, curtype;
   int dx1, dy1, dx2, dy2, dirn;
   static int m, closept, n_edge, last_vis, boundary, edge, fill;
   static XPoint *edgep;

#ifdef DEBUG
   DMESS  "Xw_poly: %d points  set: %d\n", n, set);
#endif

   cont = ( n < ZERO );
   n = abs ( n );

   if ( first )
   {
      m = closept = 0;
      n_edge = -1;
      last_vis = FALSE;
      if (set) edgep = (XPoint *) MALLOC( ARRAY_MAX, sizeof(XPoint) );
      Xw_attrib(XFILL,c);
      boundary = (Xfill.cur_style == HOLLOW);
      edge = (curatt.edge_vis == ON);
      if ( ( Xfill.cur_style == EMPTY || Xfill.cur_style == HOLLOW )
            && ( cur.transparency == ON) )  fill = FALSE;
      else fill = TRUE;
   }

   for ( k=0; k<n; k++, m++)
   {
      if (m >= amax_p-2)
      {
         (void) CGMerror("Xw_poly",ERR_BIGPOINTS, ERROR, NULLSTR);
         break;
      }
      if ( cur.vdc_type == INTEGER )
      {
         Xwork.p[m].x =  Xtranx(*pi++);
         Xwork.p[m].y =  Xtrany(*pi++);
      }
      else
      {
         Xwork.p[m].x =  Xtranx(*pr++);
         Xwork.p[m].y =  Xtrany(*pr++);
      }

      prevtype = lasttype;
      lasttype = curtype;
      curtype = *pi;

      if (m == 0) del_pt = FALSE;
      else
        if ( (Xwork.p[m].x == Xwork.p[m-1].x) &&
             (Xwork.p[m].y == Xwork.p[m-1].y)  )  del_pt = TRUE;
        else
          if (m == 1) del_pt = FALSE;
          else
          {
            dx1 = Xwork.p[m-1].x - Xwork.p[m-2].x;
            dy1 = Xwork.p[m-1].y - Xwork.p[m-2].y;
            dx2 = Xwork.p[m].x - Xwork.p[m-1].x;
            dy2 = Xwork.p[m].y - Xwork.p[m-1].y;
            dirn = (dx1*dx2 == 0) ? dy1*dy2 : dx1*dx2;
            if (dx1*dy2 != dx2*dy1) del_pt = FALSE;
            else
              if ( dirn < 0 ) del_pt = FALSE;
              else
                if (set)
                {
                  if (prevtype == lasttype) del_pt = TRUE;
                  else del_pt = FALSE;
                }
                else del_pt = TRUE;
           }


      if (del_pt)
      {
        Xwork.p[m-1].x = Xwork.p[m].x;
        Xwork.p[m-1].y = Xwork.p[m].y;
        lasttype = prevtype;
        m--;
        if (set && (n_edge >= 0))
        {
          if (edgep[n_edge].x == m)
          {
             n_edge--;
             last_vis = FALSE;
          }
          else
          {
             if (edgep[n_edge].x + edgep[n_edge].y == m+2)
             {
                edgep[n_edge].y--;
                last_vis = TRUE;
             }
          }
        }
      }

      if (set)
      {
         if (*pi == CLOSEVIS || *pi == VIS || boundary)
         {
            if (last_vis) edgep[n_edge].y++;
            else
            {
               n_edge++;
               edgep[n_edge].x = m;
               edgep[n_edge].y = 2;
             }
             last_vis = TRUE;
         }
         else last_vis = FALSE;
         if (*pi == CLOSEINVIS || *pi == CLOSEVIS || k == n-1)
         {
            if (closept != 0)
            {
               Xwork.p[++m].x = Xwork.p[closept].x;
               Xwork.p[m].y   = Xwork.p[closept].y;
            }
            Xwork.p[++m].x = Xwork.p[0].x;
            Xwork.p[m].y   = Xwork.p[0].y;
            closept = m+1;
            last_vis = FALSE;
         }
         pi++;
      }
   }


   if (!cont)
   {
      if (set)
      {
         if (closept == m) m--;
         if (fill)
           {
            XFillPolygon(display,picture,gc_fill,
                       Xwork.p,m,Complex,CoordModeOrigin);
            if((Xw_backing_store != Always))
              XFillPolygon(display,back_pix,gc_fill,
                       Xwork.p,m,Complex,CoordModeOrigin);
           }
         if (boundary)
         {
            for (j=0; j<=n_edge; j++)
            {
               k = edgep[j].x;
               m = edgep[j].y;
               XDrawLines(display,picture,gc_bound,
                         &Xwork.p[k],m,CoordModeOrigin);
               if((Xw_backing_store != Always))
                  XDrawLines(display,back_pix,gc_bound,
                         &Xwork.p[k],m,CoordModeOrigin);
            }
         }
         else
         {
            Xw_attrib(XEDGE,c);
            for (j=0; j<=n_edge; j++)
            {
               k = edgep[j].x;
               m = edgep[j].y;
               XDrawLines(display,picture,gc_edge,
                         &Xwork.p[k],m,CoordModeOrigin);
               if((Xw_backing_store != Always))
                  XDrawLines(display,back_pix,gc_edge,
                         &Xwork.p[k],m,CoordModeOrigin);
            }
         }
         FREE(edgep);
      }
      else
      {
         if (fill)
           {
            XFillPolygon(display,picture,gc_fill,
                       Xwork.p,m,Complex,CoordModeOrigin);
            if((Xw_backing_store != Always))
               XFillPolygon(display,back_pix,gc_fill,
                       Xwork.p,m,Complex,CoordModeOrigin);
           }
         Xwork.p[m] = Xwork.p[0];
         if (boundary)
           {
            XDrawLines(display,picture,gc_bound,
                         Xwork.p,m+1,CoordModeOrigin);
            if((Xw_backing_store != Always))
               XDrawLines(display,back_pix,gc_bound,
                         Xwork.p,m+1,CoordModeOrigin);
           }
         else if (edge)
              {
                 Xw_attrib(XEDGE,c);
                 XDrawLines(display,picture,gc_edge,
                         Xwork.p,m+1,CoordModeOrigin);
                 if((Xw_backing_store != Always))
                    XDrawLines(display,back_pix,gc_edge,
                         Xwork.p,m+1,CoordModeOrigin);
              }
      }
  }

  first = (!cont);

  return;
}

/*************************************************** XW_LINE *********/

#ifdef PROTO
static void Xw_line ( int n, Long *pi, Float *pr)
#else
static void Xw_line ( n, pi, pr)

/*   draws n point polyline starting at *pi/*pr  */

int n;
Long *pi;
Float *pr;
#endif
{
   int cont, k, m;
   static Logical first=TRUE, del_pt=FALSE;
   int dx1, dy1, dx2, dy2, dirn;

#ifdef DEBUG
   DMESS  "Xw_line: %d points\n", n);
#endif

   cont = ( n < ZERO );
   n = abs ( n );
   if ( first && ( n<2 )) return;

   if (first) m = 0;
   else m = 1;

   for ( k=0; k<n; k++,m++)
   {
      if ( cur.vdc_type == INTEGER )
      {
         Xwork.p[m].x =  Xtranx(*pi++);
         Xwork.p[m].y =  Xtrany(*pi++);
      }
      else
      {
         Xwork.p[m].x =  Xtranx(*pr++);
         Xwork.p[m].y =  Xtrany(*pr++);
      }

      if (m == 0) del_pt = FALSE;
      else
        if ( (Xwork.p[m].x == Xwork.p[m-1].x) &&
             (Xwork.p[m].y == Xwork.p[m-1].y)  )  del_pt = TRUE;
        else
          if (m == 1) del_pt = FALSE;
          else
          {
            dx1 = Xwork.p[m-1].x - Xwork.p[m-2].x;
            dy1 = Xwork.p[m-1].y - Xwork.p[m-2].y;
            dx2 = Xwork.p[m].x - Xwork.p[m-1].x;
            dy2 = Xwork.p[m].y - Xwork.p[m-1].y;
            dirn = (dx1*dx2 == 0 ? dy1*dy2 : dx1*dx2);
            if (dx1*dy2 != dx2*dy1) del_pt = FALSE;
            else
              if ( dirn < 0 ) del_pt = FALSE;
              else del_pt = TRUE;
          }


      if (del_pt)
      {
        Xwork.p[m-1].x = Xwork.p[m].x;
        Xwork.p[m-1].y = Xwork.p[m].y;
        m--;
      }
   }
   XDrawLines(display,picture,gc_line,&Xwork.p[0],
              m,CoordModeOrigin);
   if((Xw_backing_store != Always))
      XDrawLines(display,back_pix,gc_line,&Xwork.p[0],
              m,CoordModeOrigin);
   if (cont)
   {
      Xwork.p[0].x =  Xwork.p[m-1].x;
      Xwork.p[0].y =  Xwork.p[m-1].y;
   }
   first = (!cont);

   return;
}

/*************************************************** XW_SEGMENT ******/

#ifdef PROTO
static void Xw_segment( int n, Long *pi, Float *pr)
#else
static void Xw_segment( n, pi, pr)

/*   draws n point disjoint polyline starting at *pi/*pr  */

int n;
Long *pi;
Float *pr;
#endif
{
   static Logical first = TRUE, saved = FALSE;
   int cont, j, k, jodd;

#ifdef DEBUG
   DMESS  "Xw_line: %d points\n", n );
#endif

   cont = ( n < ZERO );
   n = abs ( n );
   if ( first && ( n<2 )) return;

   if ( cur.vdc_type == INTEGER )
   {
      for ( k=0; k<n; k++ )
      {
         Xwork.p[k+1].x =  Xtranx(*pi++);
         Xwork.p[k+1].y =  Xtrany(*pi++);
      }
   }
   else
   {
      for ( k=0; k<n; k++ )
      {
         Xwork.p[k+1].x =  Xtranx(*pr++);
         Xwork.p[k+1].y =  Xtrany(*pr++);
      }
   }

   if (saved) { k=0; j=n+1; }
   else       { k=1; j=n;   }
   jodd = (j%2 == 1);
   j /= 2;

   XDrawSegments(display,picture,gc_line,(XSegment *)&Xwork.p[k],j);
   if((Xw_backing_store != Always))
     XDrawSegments(display,back_pix,gc_line,(XSegment*)&Xwork.p[k],j);

   if (jodd)
   {
       Xwork.p[0].x =  Xwork.p[n].x;
       Xwork.p[0].y =  Xwork.p[n].y;
       saved = TRUE;
   }
   else saved = FALSE;

   first = !cont;
   return;
}

/**************************************************** XW_MARKER *******/

#ifdef PROTO
static void Xw_marker ( int n, Long *pi, Float *pr )
#else
static void Xw_marker ( n, pi, pr )

/*   draws n markers starting at *pi/*pr  */

int n;
Long *pi;
Float *pr;
#endif
{
   int j, k, m;
   int x, y, w, h, i;

#ifdef DEBUG
   DMESS  "Xw_marker: %d points\n", n);
#endif

   n = abs ( n );

   if ( cur.vdc_type == INTEGER )
   {
      for ( k=0; k<n; k++)
      {
         Xwork.p[k].x =  Xtranx(*pi++);
         Xwork.p[k].y =  Xtrany(*pi++);
      }
   }
   else
   {
      for ( k=0; k<n; k++)
      {
         Xwork.p[k].x =  Xtranx(*pr++);
         Xwork.p[k].y =  Xtrany(*pr++);
      }
   }

   m=Xmark.cur_type;
   switch (Xmark.list[m].type)
   {
      case XPOINTS:
          XDrawPoints(display,picture,gc_mark,&Xwork.p[0],
                      n,CoordModeOrigin);
          if((Xw_backing_store != Always))
             XDrawPoints(display,back_pix,gc_mark,&Xwork.p[0],
                      n,CoordModeOrigin);
          break;
      case XCIRCLE:
          for (i=0; i<n; i++)
          {
              x = Xwork.p[i].x - Xmark.cur_size/2;
              y = Xwork.p[i].y - Xmark.cur_size/2;
              w = h = Xmark.cur_size;
              XDrawArc(display,picture,gc_mark,x,y,w,h,0,360*64);
              if((Xw_backing_store != Always))
                 XDrawArc(display,back_pix,gc_mark,x,y,w,h,0,360*64);
          }
          break;
      case XLINES:
      default:
          h = (Xmark.cur_size-1)/2;
          for (i=0; i<n; i++)
          {
             k = Xmark.list[m].npts;
             for (j=0; j<k; j++)
             {
                w = floor( FABS( Xmark.list[m].dx[j]*h )+0.5 );
                w = Xmark.list[m].dx[j]>0.0 ? w : -w;
                Xmark.list[m].p[j].x = Xwork.p[i].x+w;
                w = floor( FABS( Xmark.list[m].dy[j]*h )+0.5 );
                w = Xmark.list[m].dy[j]>0.0 ? w : -w;
                Xmark.list[m].p[j].y = Xwork.p[i].y+w;
             }
             XDrawSegments(display,picture, gc_mark,
                     (XSegment*)&Xmark.list[m].p[0], k/2);
             if((Xw_backing_store != Always))
                XDrawSegments(display,back_pix, gc_mark,
                     (XSegment*)&Xmark.list[m].p[0], k/2);
          }
          break;
   }
   return;
}

/**************************************************** XW_GDP ***********/

#ifdef PROTO
static void Xw_gdp ( short type, Long *pi, Float *pr, Code c )
#else
static void Xw_gdp ( type, pi, pr, c )

/*  draws GDP 'type' with points starting at *pi/*pr
    for convenience CGM primitives are handled as GDPs  */

short type;
Long *pi;
Float *pr;
Code c;
#endif
{
   long j, np;
   Logical   fill, edge, line, boundary, GKSgdp;
   int close, xmin, ymin, inta1, inta2;
   int  w, h, shape, npts;
   Point p1, p2, p3, p4, p5;
   double rad, ang;
   Point centre, cdp1, cdp2;
   Lpoint pt[ARRAY_MAX];
   XPoint ends[3];
   double xd1, yd1, xd2, yd2, ang1, ang2, Xpi=PI;

#ifdef DEBUG
   DMESS  "Xw_gdp: type %d\n", type);
#endif


/*  work out closure type */


   if ( (type == GKSCHORD                   )  ||
        ((type == ARC3PTCLOSE)   && *(pi+6) )  ||
        ((type == ARCCTRCLOSE)   && *(pi+7) )  ||
        ((type == ELLIPARCCLOSE) && *(pi+10) ) )
   {
      if (Xfill.arcmode != ArcChord)
      {
          Xfill.arcmode = ArcChord;
          Xflag.fill = TRUE;
      }
      close = CHORD;
   }
   else if ( type == ARC3PTCLOSE   && ! *(pi+6)  ||
             type == ARCCTRCLOSE   && ! *(pi+7)  ||
             type == ELLIPARCCLOSE && ! *(pi+10) )
        {
           if (Xfill.arcmode != ArcPieSlice)
           {
              Xfill.arcmode = ArcPieSlice;
              Xflag.fill = TRUE;
           }
           close = PIE;
        }
        else
           close = NOCLOSE;


/* update fill attributes */

   Xw_attrib ( XFILL,c );

/*  work out if this is a line  */

   line = ( type == GKSARC      ||
            type == ARC3PT      ||
            type == ARCCTR      ||
            type == ELLIPARC );

   if (line)
   {
      Xw_attrib ( XLINE,c );
      fill = edge = boundary = FALSE;
   }
   else
   {
      if (  ( Xfill.cur_style == EMPTY || Xfill.cur_style == HOLLOW )
           && ( cur.transparency == ON) )  fill = FALSE;
      else fill = TRUE;

      boundary = ( Xfill.cur_style == HOLLOW);

      edge = ( Xfill.cur_style != HOLLOW && curatt.edge_vis == ON );
      if (edge) Xw_attrib ( XEDGE ,c);
   }

/*  work out if GKS gdp  */

   GKSgdp = ( type == GKSCIRCLE || type == GKSARC ||
              type == GKSPIE    || type == GKSCHORD );


#ifdef DEBUG
   DMESS "  GKS %d, edge %d, close %d, line %d, fill %d, boundary %d\n",
          GKSgdp, edge, close, line, fill, boundary);
#endif

/*  At present ignore transformation of GKS GDPs */

   if (GKSgdp)
       if (cur.vdc_type == REAL) pr += 6;
       else pi += 6;

/*  get 5 points and convert to real values */

   if (cur.vdc_type == REAL)
   {
      p1.x = *pr++;   p1.y = *pr++;
      p2.x = *pr++;   p2.y = *pr++;
      p3.x = *pr++;   p3.y = *pr++;
      p4.x = *pr++;   p4.y = *pr++;
      p5.x = *pr++;   p5.y = *pr++;
   }
   else
   {
      p1.x = *pi++;   p1.y = *pi++;
      p2.x = *pi++;   p2.y = *pi++;
      p3.x = *pi++;   p3.y = *pi++;
      p4.x = *pi++;   p4.y = *pi++;
      p5.x = *pi++;   p5.y = *pi++;
   }

#ifdef DEBUG

/*  Print type of GDP and parameters   */

#ifdef DEBUG
   DMESS "  GDP type: 0x%x  int style: %d\n", type, fill);
#endif
   switch ( type )
   {
      case GKSARC:
      case ARC3PT:
      case ARCCTR:
         DMESS "  Arc: (%f,%f) (%f,%f) (%f,%f)\n",
                                    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
         break;

      case GKSCHORD:
         DMESS "  Chord: (%f,%f) (%f,%f) (%f,%f)\n",
                                    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
         break;

      case GKSPIE:
         DMESS "  Pie: (%f,%f) (%f,%f) (%f,%f)\n",
                                    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
         break;

      case ARC3PTCLOSE:
      case ARCCTRCLOSE:
         if ( close == CHORD )
            DMESS "  Chord: (%f,%f) (%f,%f) (%f,%f)\n",
                                    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
         else
            DMESS "  Pie: (%f,%f) (%f,%f) (%f,%f)\n",
                                    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
         break;

      case GKSCIRCLE:
         DMESS "  Circle: (%f,%f) (%f,%f)\n",
                                    p1.x, p1.y, p2.x, p2.y);
         break;

      case CIRCLE:
         DMESS "  Circle: (%f,%f)  rad: %f\n",
                                    p1.x, p1.y, p2.x);
         break;

      case RECT:
         DMESS "  Rectangle: (%f,%f) (%f,%f)\n",
                                   p1.x, p1.y, p2.x, p2.y);
         break;

      case ELLIPSE:
         DMESS "  Ellipse: (%f,%f) (%f,%f) (%f,%f)\n",
                                   p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
         break;

      case ELLIPARCCLOSE:
      case ELLIPARC:
         DMESS
         "  Elliptic arc: (%f,%f) (%f,%f) (%f,%f) (%f,%f) (%f,%f) %d\n",
                 p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, p5.x, p5.y,
                 close);
         break;

      default:
         (void) sprintf ( mess, "(type: %d)", type);
         (void) CGMerror ( func, ERR_NOGDP, ERROR, mess);
         break;
   }
#endif

/*  Now work out significant points  */

   switch ( type )
   {
      case GKSARC:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PT:
      case ARC3PTCLOSE:
         GDPcentre ( &centre, &rad, &ang, &p1, &p2, &p3);
         xd1 = p1.x - centre.x;
         yd1 = p1.y - centre.y;
         xd2 = p3.x - centre.x;
         yd2 = p3.y - centre.y;
         ang1 =  atan2(yd1,xd1) ;
         ang2 =  ang1 + ang ;
         ang1 *=  180.0/Xpi ;
         ang2 *=  180.0/Xpi ;
         break;

      case ARCCTR:
      case ARCCTRCLOSE:
         centre = p1;
         xd1 = p2.x;
         yd1 = p2.y;
         xd2 = p3.x;
         yd2 = p3.y;
         ang1 = atan2(yd1,xd1);
         ang2 = atan2(yd2,xd2);
         rad = p4.x;
         p1.x = rad * cos(ang1) + centre.x;
         p1.y = rad * sin(ang1) + centre.y;
         p3.x = rad * cos(ang2) + centre.x;
         p3.y = rad * sin(ang2) + centre.y;
         ang1 *=  180.0/Xpi ;
         ang2 *= 180.0/Xpi ;
         ang2 = ang2<ang1 ? ang2+360.0 : ang2;
         break;


      case GKSCIRCLE:
         centre = p2;
         rad = SQRT( (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) );
         ang1 = 45.0;
         ang2 = ang1 + 360.0;
         break;

      case CIRCLE:
         centre = p1;
         rad = p2.x;
         ang1 = 45.0;
         ang2 = ang1 + 360.0;
         break;


      case RECT:
         if (Xw_curwin->wc_or == 0 || Xw_curwin->wc_or == 1)
               xmin = (p1.x < p2.x) ? Xtranx(p1.x) : Xtranx(p2.x);
         else  xmin = (p1.x > p2.x) ? Xtranx(p1.x) : Xtranx(p2.x);
         if (Xw_curwin->wc_or == 0 || Xw_curwin->wc_or == 2)
               ymin = (p1.y > p2.y) ? Xtrany(p1.y) : Xtrany(p2.y);
         else  ymin = (p1.y < p2.y) ? Xtrany(p1.y) : Xtrany(p2.y);
         w = (int)  FABS( (double)(p2.x - p1.x))*Xw_curwin->tot_xyscale;
         h = (int)  FABS( (double)(p2.y - p1.y))*Xw_curwin->tot_xyscale;
         Xwork.p[0].x=Xwork.p[1].x=Xwork.p[4].x=Xtranx(p1.x);
         Xwork.p[2].x=Xwork.p[3].x=Xtranx(p2.x);
         Xwork.p[0].y=Xwork.p[3].y=Xwork.p[4].y=Xtrany(p1.y);
         Xwork.p[1].y=Xwork.p[2].y=Xtrany(p2.y);
         break;

    case ELLIPSE:
         p4.x = p5.x = 1.0;
         p4.y = p5.y = 0.0;

      case ELLIPARC:
      case ELLIPARCCLOSE:
         centre = p1;
         cdp1   = p2;
         cdp2   = p3;
         GDPellipse(&centre, &cdp1, &cdp2, &p4, &p5, (Double)Xw_curwin->scale,
                            &np, pt, close);

      default:
         break;
   }


/* now draw primitives  */

   switch ( type )
   {

      case ARCCTR:
      case ARCCTRCLOSE:
      case GKSARC:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PT:
      case ARC3PTCLOSE:
      case GKSCIRCLE:
      case CIRCLE:
#ifdef DEBUG
            DMESS  "  Centre: %f,%f  %f %f,%f\n",
                                centre.x, centre.y, rad, ang1, ang2 );
#endif

/* set end-of-arc points if necessary   */

          if (close == PIE || close == CHORD)
          {
             ends[0].x = Xtranx(p1.x);
             ends[0].y = Xtrany(p1.y);
             if (close == CHORD)
             {
                ends[1].x = Xtranx(p3.x);
                ends[1].y = Xtrany(p3.y);
                npts = 2;
             }
             else
             {
                ends[2].x = Xtranx(p3.x);
                ends[2].y = Xtrany(p3.y);
                ends[1].x = Xtranx(centre.x);
                ends[1].y = Xtrany(centre.y);
                npts = 3;
             }
          }


          switch (Xw_curwin->wc_or)
          {
             case 1:
                  ang = -ang1;
                  ang1 = -ang2;
                  ang2 = ang;
                  break;
             case 2:
                  ang = 180.0 -ang1;
                  ang1 = 180.0 - ang2;
                  ang2 = ang;
                  break;
             case 3:
                  ang1 += 180.0;
                  ang2 += 180.0;
                  break;
             default :
                  break;
          }
          inta1 = 64.0*ang1;
          inta2 = 64.0*ang2 - inta1;

          xmin = Xtranx(centre.x) - Xw_curwin->tot_xyscale * rad;
          ymin = Xtrany(centre.y) - Xw_curwin->tot_xyscale * rad;
          w = h = (int)  2*rad*Xw_curwin->tot_xyscale;
         if (fill)
           {
            XFillArc(display,picture,gc_fill,
                            xmin,ymin,w,h,inta1,inta2);
            if((Xw_backing_store != Always))
               XFillArc(display,back_pix,gc_fill,
                            xmin,ymin,w,h,inta1,inta2);
           }
         if (edge)
         {
            XDrawArc(display,picture,gc_edge,xmin,ymin,w,h,inta1,inta2);
            if((Xw_backing_store != Always))
               XDrawArc(display,back_pix,gc_edge,xmin,ymin,w,h,inta1,inta2);
            if (close == PIE || close == CHORD)
              {
               XDrawLines(display,picture,gc_edge,
                            ends,npts,CoordModeOrigin);
               if((Xw_backing_store != Always))
                 XDrawLines(display,back_pix,gc_edge,
                            ends,npts,CoordModeOrigin);
              }
         }
         if (line)
           {
            XDrawArc(display,picture,gc_line,
                            xmin,ymin,w,h,inta1,inta2);
            if((Xw_backing_store != Always))
              XDrawArc(display,back_pix,gc_line,
                            xmin,ymin,w,h,inta1,inta2);
           }
         if (boundary)
         {
            XDrawArc(display,picture,gc_bound,
                      xmin,ymin,w,h,inta1,inta2);
            if((Xw_backing_store != Always))
               XDrawArc(display,back_pix,gc_bound,
                      xmin,ymin,w,h,inta1,inta2);
             if (close == PIE || close == CHORD)
               {
                XDrawLines(display,picture,gc_bound,
                             ends,npts,CoordModeOrigin);
                if((Xw_backing_store != Always))
                   XDrawLines(display,back_pix,gc_bound,
                             ends,npts,CoordModeOrigin);
               }
         }
         break;

      case RECT:
         npts = 4;
         if (fill)
           {
            XFillPolygon(display,picture,gc_fill,
                                    Xwork.p,npts,Convex,CoordModeOrigin);
            if((Xw_backing_store != Always))
              XFillPolygon(display,back_pix,gc_fill,
                                    Xwork.p,npts,Convex,CoordModeOrigin);
           }
         npts = 5;
         if (edge)
           {
            XDrawLines(display,picture,gc_edge,
                                    Xwork.p,npts,CoordModeOrigin);
            if((Xw_backing_store != Always))
              XDrawLines(display,back_pix,gc_edge,
                                    Xwork.p,npts,CoordModeOrigin);
           }
         if (boundary)
           {
            XDrawLines(display,picture,gc_bound,
                                    Xwork.p,npts,CoordModeOrigin);
            if((Xw_backing_store != Always))
               XDrawLines(display,back_pix,gc_bound,
                                    Xwork.p,npts,CoordModeOrigin);
           }
         break;

      case ELLIPSE:
      case ELLIPARC:
      case ELLIPARCCLOSE:
         for (j=0; j<np; j++)
         {
           Xwork.p[j].x = Xtranx(pt[j].x);
           Xwork.p[j].y = Xtrany(pt[j].y);
         }
         Xwork.p[np].x = Xwork.p[0].x;
         Xwork.p[np].y = Xwork.p[0].y;
         npts = np;
         if (line)
           {
            XDrawLines(display,picture,gc_line,
                           Xwork.p,npts,CoordModeOrigin);
            if((Xw_backing_store != Always))
              XDrawLines(display,back_pix,gc_line,
                           Xwork.p,npts,CoordModeOrigin);
           }
         if (fill)
         {
           if (close == PIE) shape = Nonconvex;
           else  shape = Convex;
           XFillPolygon(display,picture,gc_fill,
                           Xwork.p,npts,shape,CoordModeOrigin);
           if((Xw_backing_store != Always))
             XFillPolygon(display,back_pix,gc_fill,
                           Xwork.p,npts,shape,CoordModeOrigin);
         }
         npts = np+1;
         if (edge)
           {
            XDrawLines(display,picture,gc_edge,
                           Xwork.p,npts,CoordModeOrigin);
            if((Xw_backing_store != Always))
              XDrawLines(display,back_pix,gc_edge,
                           Xwork.p,npts,CoordModeOrigin);
           }
         if (boundary)
           {
            XDrawLines(display,picture,gc_bound,
                           Xwork.p,npts,CoordModeOrigin);
            if((Xw_backing_store != Always))
               XDrawLines(display,back_pix,gc_bound,
                           Xwork.p,npts,CoordModeOrigin);
           }
      default:
         break;
   }
   return;
}

/**************************************************** XW_SETWIN ********/

#ifdef PROTO
static void Xw_setwin( int type )
#else
static void Xw_setwin( type )
int type;
#endif
{
   double w, h, oldscale, sx, sy;
   float xmin, ymin;
                                               /* Xw_curwin->wc_or values

                                                      2  |  0
                                                    _____|_____
                                                         |
                                                      3  |  1
                                               */


   if (type & SCREEN)
   {
      if (Xw_curwin->new_coords)
      {
         Xw_curwin->sc_x   = Xw_curwin->newx;
         Xw_curwin->sc_y   = Xw_curwin->newy;
         Xw_curwin->sc_w   = Xw_curwin->neww;
         Xw_curwin->sc_h   = Xw_curwin->newh;
         Xw_curwin->new_coords = FALSE;
      }
   }
   if (type & CLIPAREA)
   {
      if (cur.vdc_type == REAL)
      {
         Xclip.left  = cur.clip_rect.a.x.real;
         Xclip.bot   = cur.clip_rect.a.y.real;
         Xclip.right = cur.clip_rect.b.x.real;
         Xclip.top   = cur.clip_rect.b.y.real;
      }
      else
      {
         Xclip.left  = cur.clip_rect.a.x.intr;
         Xclip.bot   = cur.clip_rect.a.y.intr;
         Xclip.right = cur.clip_rect.b.x.intr;
         Xclip.top   = cur.clip_rect.b.y.intr;
      }
   }

   if (type & WORLD)
   {
      if (cur.vdc_type == REAL)
      {
         Xw_curwin->wc_left   = cur.vdc_extent.a.x.real;
         Xw_curwin->wc_bot    = cur.vdc_extent.a.y.real;
         Xw_curwin->wc_right  = cur.vdc_extent.b.x.real;
         Xw_curwin->wc_top    = cur.vdc_extent.b.y.real;
      }
      else
      {
         Xw_curwin->wc_left   = cur.vdc_extent.a.x.intr;
         Xw_curwin->wc_bot    = cur.vdc_extent.a.y.intr;
         Xw_curwin->wc_right  = cur.vdc_extent.b.x.intr;
         Xw_curwin->wc_top    = cur.vdc_extent.b.y.intr;
      }
   }

   if ( (type & WORLD)  ||  (type & SCREEN) )
   {
      if (Xw_curwin->wc_right < Xw_curwin->wc_left)
      {
         Xw_curwin->wc_or = 2;
         Xw_curwin->signx = -1;
      }
      else
      {
         Xw_curwin->wc_or = 0;
         Xw_curwin->signx = 1;
      }
      if (Xw_curwin->wc_top < Xw_curwin->wc_bot)
      {
         Xw_curwin->wc_or++;
         Xw_curwin->signy = -1;
      }
      else Xw_curwin->signy = 1;

       w = FABS(Xw_curwin->wc_right - Xw_curwin->wc_left);
       h = FABS(Xw_curwin->wc_top - Xw_curwin->wc_bot);
       if (type & SETDFT)  Xw_curwin->dft_size = w>h ? 0.01*w : 0.01*h;
       oldscale = Xw_curwin->scale;

       Xw_curwin->wc_rat = h / w ;
       Xw_curwin->sc_rat = (float) Xw_curwin->sc_h / (float) Xw_curwin->sc_w;

       if (Xw_metric && cur.scale_mode == METRIC)
            Xw_curwin->scale = Xw_curwin->pix_per_mm * cur.scale_factor;
       else
       {
          if (Xw_curwin->wc_rat < Xw_curwin->sc_rat)
                 Xw_curwin->scale = Xw_curwin->sc_w / w;
          else
                 Xw_curwin->scale = Xw_curwin->sc_h / h;
       }

       if (type & VDCCHNG)
               Xw_curwin->dft_size *=  (oldscale / Xw_curwin->scale);

       Xw_curwin->offset_x = 0.5 * (Xw_curwin->sc_w - w * Xw_curwin->scale);
       if (Xw_curwin->offset_x < 0) Xw_curwin->offset_x = 0;
       Xw_curwin->offset_y = -0.5 * (Xw_curwin->sc_h - h * Xw_curwin->scale);
       if (Xw_curwin->offset_y > 0) Xw_curwin->offset_y *= 2;

       sx = Xw_curwin->scale * Xw_curwin->signx;
       sy = Xw_curwin->scale * Xw_curwin->signy;
       Xw_curwin->tot_xscale = sx * Xw_curwin->zoom.xscale;
       Xw_curwin->tot_yscale = sy * Xw_curwin->zoom.yscale;
       Xw_curwin->tot_xyscale = FABS( Xw_curwin->tot_xscale );
       Xw_curwin->tot_xdisp = ( sx * Xw_curwin->wc_left
                              - Xw_curwin->offset_x - Xw_curwin->zoom.x
                              ) * Xw_curwin->zoom.xscale;
       Xw_curwin->tot_ydisp = ( sy * Xw_curwin->wc_bot
                              + Xw_curwin->offset_y + Xw_curwin->zoom.y
                              + Xw_curwin->sc_h
                              ) * Xw_curwin->zoom.yscale;
   }

   if (type & CLIPAREA)
   {
       if (Xw_curwin->wc_or == 0 || Xw_curwin->wc_or == 1)
            xmin = Xclip.left < Xclip.right ? Xclip.left : Xclip.right;
       else xmin = Xclip.left > Xclip.right ? Xclip.left : Xclip.right;

       if (Xw_curwin->wc_or == 0 || Xw_curwin->wc_or == 2)
            ymin = Xclip.bot < Xclip.top ? Xclip.top : Xclip.bot;
       else ymin = Xclip.bot > Xclip.top ? Xclip.top : Xclip.bot;

       Xclip.area.x = Xtranx(xmin);
       Xclip.area.y = Xtrany(ymin);
       Xclip.area.width = FABS( Xclip.left-Xclip.right )
                           * Xw_curwin->tot_xyscale;
       Xclip.area.height = FABS( Xclip.top-Xclip.bot )
                           * Xw_curwin->tot_xyscale;

       Xclip.clip_ind = cur.clip_ind;
   }
   return;
}

/**************************************************** XW_SET_DEFAULT ***/

#ifdef PROTO
static void Xw_set_default( void )
#else
static void Xw_set_default()
#endif
{
   int i, j, size;
   struct Xcolrused *p, *q;

   XClearWindow(display,picture);
   if((Xw_backing_store != Always))
     Xw_fillpix(back_pix,Xcolrtab[0].pixel);

   if ( Xcolrtab != NULL ) FREE(Xcolrtab);
   XNCOLOR = XDEFNCOLOR;
   Xcolrtab = (XColor *) MALLOC ( XNCOLOR, sizeof(XColor) );

   for (i=0; i<XNCOLOR ; i++)
   {
     Xcolrtab[i] = Xdefcolrtab[i];
     Xcolrtab[i].pixel = -1;
   }

   if (Xw_numwin < 2)
   {
      for (p=Xalloccolr; p!=NULL;p=q)
      {
         q = p->next;
         if (can_set_colour) XFreeColors(display,Xclmap,&p->ucol.pixel,1,0);
         FREE(p);
      }
      Xalloccolr = NULL;
   }

   for (i=0; i<=XNTILE; i++)
   {
        if (Xfill.tiles[i].pm != -1)
             XFreePixmap(display,Xfill.tiles[i].pm);
        if (Xfill.tiles[i].col != NULL) FREE(Xfill.tiles[i].col);
        Xfill.tiles[i].pm = -1;
        Xfill.tiles[i].col = NULL;
        if (Xfill.deftiles[i].col != NULL)
        {
            size = Xfill.deftiles[i].nx * Xfill.deftiles[i].ny;
            Xfill.tiles[i].col = (struct colour *)
                           MALLOC ( size, sizeof(struct colour) );
            for (j=0; j<size; j++)
               Xfill.tiles[i].col[j] = Xfill.deftiles[i].col[j];
        }
   }

   Xbackcolr = Xdefbackcolr;
   Xw_getpix(&Xbackcolr,ASIS);

   XSetWindowBackground(display,picture,Xbackcolr.pixel);
   XClearWindow(display,picture);
   if((Xw_backing_store != Always))
     Xw_fillpix(back_pix,Xbackcolr.pixel);
   Xforecolr = Xdefforecolr;
   Xw_getpix(&Xforecolr,ASIS);
   Xcolrtab[0] = Xbackcolr;
   Xw_copycolr(&Xauxcolr,&cur.aux);
   Xauxcolr.pixel = -1;


   Xw_setwin(CLIPAREA+WORLD+SETDFT);

   Xclip.clip_ind = OFF;

   return;

}

/**************************************************** XW_INIT **********/

#ifdef PROTO
static void Xw_init( long *pic )
#else
static void Xw_init( pic )

/*  Initiate Graphics */

long *pic;
#endif
{
   unsigned long  gc_mask, allgc;
   int i, j, n, k, jw;
   unsigned int w;
   long *pint;
   char filename[256], *fnm;
   unsigned char *pch;
   Pixmap dummypm;
   FILE *stream;
   char Xfnt_patt[25];    /* font match pattern to use in request to server */
   int match_lim=1000;    /* maximum number of matches for XListFonts */

   if (Xw_firstmf)
   {
       Xw_curwin = (struct Xwin_rec *) MALLOC ( 1, sizeof(struct Xwin_rec) );
       Xw_curwin->zoom.x = Xw_curwin->zoom.y = 0;
       Xw_curwin->zoom.xscale = Xw_curwin->zoom.yscale = 1.0;
       sprintf(Xw_curwin->mesgbuff,"    0 :: select first frame");
       Xw_windows(Xw_curwin);
       XSync(display,FALSE);
/*
  get list of fonts from the server which have iso character sets
*/
       sprintf(Xfnt_patt,"*-iso*");
       if((Xfnt_serv=XListFonts(display,Xfnt_patt,match_lim,&XNFONT))==NULL)
          exit( CGMerror (func, ERR_NOFONT, FATAL, NULLSTR) );

       FNTXlist_called = FALSE;

       Xw_inmenu();
       Xw_inkb();


   }

   picture = Xw_curwin->pi.win;
   if((Xw_backing_store != Always))
     back_pix = Xw_curwin->pi.pix;

   Xcolourscale.red = 65535.0/((float)(cur.max_rgb.red-cur.min_rgb.red));
   Xcolourscale.green = 65535.0/((float)(cur.max_rgb.green-cur.min_rgb.green));
   Xcolourscale.blue = 65535.0/((float)(cur.max_rgb.blue-cur.min_rgb.blue));

   Xcolrtab = (XColor *) MALLOC ( XNCOLOR, sizeof(XColor) );

   for (i=0,pch=Xdfcolr; i<XNCOLOR; i++)
   {
       Xcolrtab[i].pixel = -1;
       Xcolrtab[i].red   = 257 * (*pch++);
       Xcolrtab[i].green = 257 * (*pch++);
       Xcolrtab[i].blue  = 257 * (*pch++);
   }
   Xdefbackcolr = Xcolrtab[0];
   Xdefforecolr = Xcolrtab[1];

   Xclmap = Xdfclmap;

   Xbackcolr = Xdefbackcolr;
   Xw_getpix(&Xbackcolr,ASIS);
   Xforecolr = Xdefforecolr;
   Xw_getpix(&Xforecolr,ASIS);
   Xauxcolr = Xbackcolr;

   XSetWindowBackground(display,picture,Xbackcolr.pixel);
   XClearWindow(display,picture);
   if((Xw_backing_store != Always))
     Xw_fillpix(back_pix,Xbackcolr.pixel);

   Xw_setwin(WORLD+SCREEN);

   amax_i = sizeof(Xwork)/sizeof(long);
   amax_s = sizeof(Xwork)/sizeof(short);
   amax_f = sizeof(Xwork)/sizeof(float);
   amax_p = sizeof(Xwork)/sizeof(XPoint);
   amax_c = sizeof(Xwork)/sizeof(char);

   allgc = ( 1L<<(GCLastBit+1) ) -1;
   gc_line = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_dflt,allgc,gc_line);

   gc_values.fill_style = FillSolid;
   gc_values.fill_rule  = EvenOddRule;
   gc_mask = GCFillStyle+GCFillRule;
   XChangeGC(display,gc_line,gc_mask,&gc_values);
   gc_edge = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_line,allgc,gc_edge);

   gc_cell = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_line,allgc,gc_cell);

   dummypm = XCreatePixmap(display,picture,10,10,1);
   gc_tile = XCreateGC(display,dummypm,0,&gc_values);
   XFreePixmap(display,dummypm);

   gc_text = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_line,allgc,gc_text);

   gc_beztxt = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_dflt,allgc,gc_beztxt);
   gc_values.fill_style = FillSolid;
   gc_values.fill_rule  = EvenOddRule;
   gc_mask = GCFillStyle+GCFillRule;
   XChangeGC(display,gc_beztxt,gc_mask,&gc_values);

   gc_hertxt = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_dflt,allgc,gc_hertxt);

   gc_fill = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_line,allgc,gc_fill);

   gc_bound = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_line,allgc,gc_bound);
   gc_values.line_width = 2;
   gc_values.line_style = LineSolid;
   gc_values.cap_style  = CapButt;
   gc_values.join_style = JoinMiter;
   gc_mask = GCLineWidth+GCLineStyle+GCCapStyle+GCJoinStyle;
   XChangeGC(display,gc_bound,gc_mask,&gc_values);

   gc_mark = XCreateGC(display,picture,0,&gc_values);
   XCopyGC(display,gc_fill,allgc,gc_mark);
   gc_values.line_width = 1;
   gc_mask = GCLineWidth;
   XChangeGC(display,gc_fill,gc_mask,&gc_values);


   for (i=0; i<ASFS; i++) Xoldasf[i] = curatt.asf[i];

   for (n=0; n<=XNTILE; n++)
   {
       Xfill.tiles[n].pm = -1;
       Xfill.tiles[n].col = NULL;
   }
   w = cur.color_mode;
   cur.color_mode = INDEXED;
   for (i=0,pch=Xdft_tile; i<=XSTILE; i++)
   {
       k = *(pch+1);
       n = *pch + 6;
       for (j=0,pint = &Xwork.i[0]; j<n; j++)
           *pint++ = *pch++ ;
       Xwork.i[0]++;
       Xw_pattab( &Xwork.i[0] );
       n = Xfill.tiles[k].nx * Xfill.tiles[k].ny;
       for (j=0; j<n;j++)
       {
           jw = Xfill.tiles[k].col[j].index;
           Xfill.tiles[k].col[j].red = Xcolrtab[jw].red;
           Xfill.tiles[k].col[j].green = Xcolrtab[jw].green;
           Xfill.tiles[k].col[j].blue = Xcolrtab[jw].blue;
       }
   }
   cur.color_mode = w;

   Xw_font.index=0;
   if(Xw_font.fstr != NULL)
      XFreeFont(display,Xw_font.fstr);
   Xw_font.fstr = NULL;

   if (Xw_firstmf) Xw_event(WAIT);
   Xw_firstmf = FALSE;
   return;
}

/**************************************************** XW_CRWIN *********/

#ifdef PROTO
static Window Xw_crwin( Window win, int x, int y, unsigned int w,
                 unsigned int h, unsigned long evmask, unsigned long atmask)
#else
static Window Xw_crwin(win,x,y,w,h,evmask,atmask)
Window win;
int x,y;
unsigned int w,h;
unsigned long evmask, atmask;
#endif
{
   Window newwin;
   XSetWindowAttributes setattr;
   unsigned long latmask;

   setattr.override_redirect = TRUE;
   setattr.do_not_propagate_mask = NoEventMask;
   setattr.backing_store = Xw_backing_store;
   setattr.save_under = True;
   setattr.border_pixel = Xw_black;
   setattr.background_pixel = Xw_white;
   latmask = atmask | (CWBackPixel + CWBorderPixel);

   newwin = XCreateWindow(display,win,x,y,w,h,1,depth,
                CopyFromParent,CopyFromParent,latmask,&setattr);
   if (evmask) XSelectInput(display,newwin,evmask);

   return newwin;
}

/**************************************************** XW_INMENU ********/

#ifdef PROTO
static void Xw_inmenu( void )
#else
static void Xw_inmenu()
#endif
{
   unsigned int w,h;
   int x,y,len,i;
   unsigned long evmask, atmask;


   x = y = 0;
   w = Xw_winparm.menw+2+2*Xw_winparm.wpack;
   h = XNMENUITEM*(Xw_winparm.buth+2)+(XNMENUITEM+1)*Xw_winparm.hpack;
   Xmenu.w = w+2;
   Xmenu.h = h+2;
   evmask = 0;
   atmask = CWOverrideRedirect + CWDontPropagate + CWSaveUnder;
   Xmenu.shell.win = Xw_crwin(Xw_curwin->top.win,x,y,w,h,evmask,atmask);
   Xmenu.shell.pixback = FALSE;
   Xw_menu_parent = Xw_curwin->top.win;

   x = Xw_winparm.wpack;
   y = Xw_winparm.hpack;
   w = Xw_winparm.menw;
   h = Xw_winparm.buth;
   evmask = ButtonReleaseMask + ButtonPressMask +
            LeaveWindowMask + EnterWindowMask   +
            ExposureMask;
   atmask = CWOverrideRedirect + CWDontPropagate + CWBackingStore;
   Xmenu.m[1].win = Xw_crwin(Xmenu.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xmenu.m[1].title,"Copy   ");
   Xmenu.m[1].written = FALSE;
   Xmenu.m[1].pixback = FALSE;

   y += h+2+Xw_winparm.hpack;
   Xmenu.m[2].win = Xw_crwin(Xmenu.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xmenu.m[2].title,"Bigger ");
   Xmenu.m[2].written = FALSE;
   Xmenu.m[2].pixback = FALSE;

   y += h+2+Xw_winparm.hpack;
   Xmenu.m[3].win = Xw_crwin(Xmenu.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xmenu.m[3].title,"Smaller");
   Xmenu.m[3].written = FALSE;
   Xmenu.m[3].pixback = FALSE;

   y += h+2+Xw_winparm.hpack;
   Xmenu.m[4].win = Xw_crwin(Xmenu.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xmenu.m[4].title,"Move   ");
   Xmenu.m[4].written = FALSE;
   Xmenu.m[4].pixback = FALSE;

   y += h+2+Xw_winparm.hpack;
   Xmenu.m[5].win = Xw_crwin(Xmenu.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xmenu.m[5].title,"Normal ");
   Xmenu.m[5].written = FALSE;
   Xmenu.m[5].pixback = FALSE;

   y += h+2+Xw_winparm.hpack;
   Xmenu.m[6].win = Xw_crwin(Xmenu.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xmenu.m[6].title,"Close  ");
   Xmenu.m[6].written = FALSE;
   Xmenu.m[6].pixback = FALSE;

   y += h+2+Xw_winparm.hpack;
   Xmenu.m[7].win = Xw_crwin(Xmenu.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xmenu.m[7].title,"Finish ");
   Xmenu.m[7].written = FALSE;
   Xmenu.m[7].pixback = FALSE;

   if (Xw_backing_store == Always)
   {
      for (i=1; i<= XNMENUITEM; i++)
      {
          len = strlen(Xmenu.m[i].title);
          XClearWindow(display,Xmenu.m[i].win);
          XDrawString(display,Xmenu.m[i].win,gc_men,
               Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
               Xmenu.m[i].title,len);
          Xmenu.m[i].written = (Xw_backing_store == Always);
      }
   }
   return;

}

/**************************************************** XW_INKB ********/

#ifdef PROTO
static void Xw_inkb( void )
#else
static void Xw_inkb()
#endif
{
   unsigned int w,h;
   int x,y,len,i;
   unsigned long evmask, atmask;
   char temp[XKBMLEN];


   x = y = 0;
   w = 3*(Xw_winparm.keyw+2)+4*Xw_winparm.wpack;
   h = 5*(Xw_winparm.buth+2)+6*Xw_winparm.hpack;
   Xkb.w = w+2;
   Xkb.h = h+2;
   evmask = 0;
   atmask = CWOverrideRedirect + CWDontPropagate + CWSaveUnder;
   Xkb.shell.win = Xw_crwin(Xw_curwin->top.win,x,y,w,h,evmask,atmask);
   Xkb.shell.pixback= FALSE;
   Xw_kb_parent = Xw_curwin->top.win;
   x = Xw_winparm.wpack;
   y = Xw_winparm.hpack;
   w -= 2*Xw_winparm.hpack+2;
   h = Xw_winparm.buth;
   evmask = ExposureMask;
   atmask = CWOverrideRedirect + CWDontPropagate + CWBackingStore;
   Xkb.tx.win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.tx.title," ");
   Xkb.tx.written = FALSE;
   Xkb.tx.pixback = FALSE;

   y += Xw_winparm.buth+2+Xw_winparm.hpack;
   evmask = ButtonReleaseMask + ButtonPressMask +
            LeaveWindowMask + EnterWindowMask   +
            ExposureMask;
   w = Xw_winparm.keyw;
   Xkb.m[1].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[1].title,"0");
   Xkb.m[1].written = FALSE;
   Xkb.m[1].pixback = FALSE;

   x += Xw_winparm.keyw+2+Xw_winparm.wpack;
   Xkb.m[2].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[2].title,"1");
   Xkb.m[2].written = FALSE;
   Xkb.m[2].pixback = FALSE;

   x += Xw_winparm.keyw+2+Xw_winparm.wpack;
   Xkb.m[3].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[3].title,"2");
   Xkb.m[3].written = FALSE;
   Xkb.m[3].pixback = FALSE;

   y += Xw_winparm.buth+2+Xw_winparm.hpack;
   x = Xw_winparm.wpack;
   Xkb.m[4].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   Xkb.m[4].written = FALSE;
   Xkb.m[4].pixback = FALSE;

   strcpy(Xkb.m[4].title,"3");
   x += Xw_winparm.keyw+2+Xw_winparm.wpack;
   Xkb.m[5].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   Xkb.m[5].written = FALSE;
   Xkb.m[4].pixback = FALSE;

   strcpy(Xkb.m[5].title,"4");
   x += Xw_winparm.keyw+2+Xw_winparm.wpack;
   Xkb.m[6].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[6].title,"5");
   Xkb.m[6].written = FALSE;
   Xkb.m[6].pixback = FALSE;

   y += Xw_winparm.buth+2+Xw_winparm.hpack;
   x = Xw_winparm.wpack;
   Xkb.m[7].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[7].title,"6");
   Xkb.m[7].written = FALSE;
   Xkb.m[7].pixback = FALSE;

   x += Xw_winparm.keyw+2+Xw_winparm.wpack;
   Xkb.m[8].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[8].title,"7");
   Xkb.m[8].written = FALSE;
   Xkb.m[8].pixback = FALSE;

   x += Xw_winparm.keyw+2+Xw_winparm.wpack;
   Xkb.m[9].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[9].title,"8");
   Xkb.m[9].written = FALSE;
   Xkb.m[9].pixback = FALSE;

   y += Xw_winparm.buth+2+Xw_winparm.hpack;
   x = Xw_winparm.wpack;
   Xkb.m[10].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[10].title,"9");
   Xkb.m[10].written = FALSE;
   Xkb.m[10].pixback = FALSE;

   x += Xw_winparm.keyw+2+Xw_winparm.wpack;
   Xkb.m[11].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[11].title,"C");
   Xkb.m[11].written = FALSE;
   Xkb.m[11].pixback = FALSE;

   x += Xw_winparm.keyw+2+Xw_winparm.wpack;
   Xkb.m[12].win = Xw_crwin(Xkb.shell.win,x,y,w,h,evmask,atmask);
   strcpy(Xkb.m[12].title,"D");
   Xkb.m[12].written = FALSE;
   Xkb.m[12].pixback = FALSE;



   if (Xw_backing_store == Always)
   {
       sprintf(temp,"%d",Xkb.frame);
       len = strlen(temp);
       XClearWindow(display,Xkb.tx.win);
       XDrawString(display,Xkb.tx.win,gc_men,
            Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
            temp,len);
       Xkb.tx.written = (Xw_backing_store == Always);
       for (i=1; i<= XNKEYS; i++)
       {
            len = strlen(Xkb.m[i].title);
            XClearWindow(display,Xkb.m[i].win);
            XDrawString(display,Xkb.m[i].win,gc_men,
                 Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
                 Xkb.m[i].title,len);
            Xkb.m[i].written = (Xw_backing_store == Always);
       }
   }
   return;
}

/**************************************************** XW_CLOSE ********/

#ifdef PROTO
static void Xw_close( long pic )
#else
static void Xw_close( pic )

/*  Close down the Graphics  */

long pic;
#endif
{
   int i;
   struct Xcolrused *p, *q;

   if (!Xfirst_pic)
       for (i=0; i<=XNSTIP; i++) XFreePixmap(display,Xfill.stipples[i]);

   for (i=0; i<=XNTILE; i++)
     {if (Xfill.tiles[i].pm != -1)
              XFreePixmap(display,Xfill.tiles[i].pm);
      Xfill.tiles[i].pm = -1;
      if (Xfill.tiles[i].col != NULL)
              FREE(Xfill.tiles[i].col);
      Xfill.tiles[i].col = NULL;
      if (Xfill.deftiles[i].col != NULL)
              FREE(Xfill.deftiles[i].col);
      Xfill.deftiles[i].col = NULL;
     }

   if (Xw_numwin <2)
   {
      for (p=Xalloccolr; p!= NULL; p=q)
      {
         q = p->next;
         if (can_set_colour) XFreeColors(display,Xclmap,&p->ucol.pixel,1,0);
         FREE(p);
      }
      Xalloccolr = NULL;
   }

   if (Xcolrtab != NULL)
   {
       FREE(Xcolrtab);
       Xcolrtab = NULL;
   }

   FREE(Xdefcolrtab);
   Xdefcolrtab = NULL;

   if(Xw_font.fstr != NULL)
   {
      XFreeFont(display,Xw_font.fstr);
      Xw_font.fstr = NULL;
   }

   XFreeGC(display,gc_line);
   XFreeGC(display,gc_text);
   XFreeGC(display,gc_beztxt);
   XFreeGC(display,gc_hertxt);
   XFreeGC(display,gc_edge);
   XFreeGC(display,gc_fill);
   XFreeGC(display,gc_mark);
   XFreeGC(display,gc_bound);
   XFreeGC(display,gc_tile);
   XFreeGC(display,gc_cell);

   return;
}


/**************************************************** XW_FIRSTP ********/

#ifdef PROTO
static void Xw_firstp( void )
#else
static void Xw_firstp()
#endif
{
   int i, k, temp, n, *pint, jw, jh, w, h, maxjw;
   unsigned long pix, mask = 15L<<28;
   XImage *image;
   char *p;

/*   get colour table defaults */

   Xdefcolrtab = (XColor *) MALLOC ( XNCOLOR, sizeof(XColor) );
   for (i=0; i<XNCOLOR; i++) Xdefcolrtab[i] = Xcolrtab[i];
   Xdefbackcolr = Xbackcolr;
   Xdefbackcolr.pixel = -1;
   Xdefforecolr = Xforecolr;
   Xdefforecolr.pixel = -1;
   XDEFNCOLOR = XNCOLOR;


/*   get pattern table defaults   */

   for (i=0; i<=XNTILE; i++)
   {
       Xfill.deftiles[i] = Xfill.tiles[i];
       if (Xfill.tiles[i].pm != -1)
          XFreePixmap(display,Xfill.tiles[i].pm);
       Xfill.tiles[i].pm = -1;
       Xfill.tiles[i].col = NULL;
   }


   p =  (char *) MALLOC ( STIPSIZE*STIPSIZE, sizeof(long) );
   image = XCreateImage(display,attr.visual,1,ZPixmap,
                        0,p,STIPSIZE,STIPSIZE,8,0);

   pint = Xdft_stip;
   for (i=0; i<=XNSTIP; i++)
   {
       w = *pint++;
       h = *pint++;
       maxjw = (w+7)/8;
       Xfill.stipples[i] = XCreatePixmap(display,picture,w,h,1);
       for (jh=0; jh<h; jh++)
       {
          for (jw=0,n=0; jw<maxjw; jw++)
          {
             temp = *pint++;
             for (k=0; k<8; k++)
             {
                pix = (temp & mask) == 0 ? 0 : 1;
                XPutPixel(image,n,jh,pix);
                temp <<= 4;
                if (++n == w) break;
             }
          }
       }
      XPutImage(display,Xfill.stipples[i],gc_tile,image,
                0,0,0,0,w,h);
   }
   XDestroyImage(image);

   XFreeGC(display,gc_tile);
   gc_tile = XCreateGC(display,picture,0,&gc_values);

   return;

}

/**************************************************** XW_COPYCOLR ******/

#ifdef PROTO
static void Xw_copycolr( XColor *to, struct colour *from )
#else
static void Xw_copycolr( to, from )
XColor *to;
struct colour *from;
#endif
{
   to->red = rgbtran(from->red,red);
   to->green = rgbtran(from->green,green);
   to->blue = rgbtran(from->blue,blue);
   return;
}


/**************************************************** XW_MAKETILE ******/

#ifdef PROTO
static void Xw_maketile( int indx )
#else
static void Xw_maketile( indx )
int indx;
#endif
{
   int n, i, j, jw, jh, nxpix, nypix, ndx, ndy, ngrpx, ngrpy;
   int xs, ys;
   double x1, x2, y1, y2;
   struct colour *colrp;
   XColor *colptr, tempcolr;
   char *p;
   XImage *image;
   unsigned long pix;

   if (cur.vdc_type == INTEGER)
   {
       x1 = curatt.pat_size.a.x.intr;
       y1 = curatt.pat_size.a.y.intr;
       x2 = curatt.pat_size.b.x.intr;
       y2 = curatt.pat_size.b.y.intr;
   }
   else
   {
       x1 = curatt.pat_size.a.x.real;
       y1 = curatt.pat_size.a.y.real;
       x2 = curatt.pat_size.b.x.real;
       y2 = curatt.pat_size.b.y.real;
   }

   nxpix = SQRT(x2*x2+y2*y2)*Xw_curwin->scale;
   ndx = Xfill.tiles[indx].nx;
   ngrpx = (nxpix+ndx/2)/ndx;
   if (ndx*ngrpx < XMINTILE) ngrpx = (XMINTILE+ndx-1)/ndx;
   if (ndx*ngrpx > XMAXTILE) ngrpx = XMAXTILE/ndx;
   nxpix = ndx * ngrpx;

   nypix = SQRT(x1*x1+y1*y1)*Xw_curwin->scale;
   ndy = Xfill.tiles[indx].ny;
   ngrpy = (nypix+ndy/2)/ndy;
   if (ndy*ngrpy < XMINTILE) ngrpy = (XMINTILE+ndy-1)/ndy;
   if (ndy*ngrpy > XMAXTILE) ngrpy = XMAXTILE/ndy;
   nypix = ndy * ngrpy;

   p = (char *) MALLOC ( nxpix*nypix, sizeof(long) );
   image = XCreateImage(display,attr.visual,depth,ZPixmap,
                        0,p,nxpix,nypix,8,0);

   for (n = -1,i=0; i<ndy; i++)
   {
      ys = i * ngrpy;
      for (j=0; j<ndx; j++)
      {
         n++;
         colrp = &Xfill.tiles[indx].col[n];
         tempcolr.pixel = -1;
         if (cur.color_mode == INDEXED)
                  colptr = &Xcolrtab[colrp->index];
         else
         {
            tempcolr.red = colrp->red;
            tempcolr.green = colrp->green;
            tempcolr.blue = colrp->blue;
            tempcolr.pixel = -1;
            colptr = &tempcolr;
         }
         pix = Xw_getpix(colptr,ASIS);
         xs = j * ngrpx;
         for (jw=0; jw<ngrpx; jw++)
            for (jh=0; jh<ngrpy; jh++)
                 XPutPixel(image,xs+jw,ys+jh,pix);
      }
   }

   Xfill.tiles[indx].pm =
              XCreatePixmap(display,picture,nxpix,nypix,depth);
   XPutImage(display,Xfill.tiles[indx].pm,gc_tile,image,
             0,0,0,0,nxpix,nypix);
   XDestroyImage(image);

   return;
}
/**************************************************** XW_GETPIX ********/

#ifdef PROTO
static unsigned long Xw_getpix( XColor *col, int type)
#else
static unsigned long Xw_getpix( col, type )
XColor *col;
int type;
#endif
{
   XColor tc;
   int i, j;
   int nearred=1000000, neargreen=1000000, nearblue=1000000;
   int nearall=1000000;
   struct Xcolrused *p, *bestred, *bestgreen, *bestblue, *bestall;

   tc = *col;
   tc.flags = tc.pixel = 0;
   if (monochrome)
     tc.red=tc.green=tc.blue=0.3*tc.red+0.59*tc.green+0.11*tc.blue;

   if (depth == 1 && type != ASIS)
   {
       if (type == BACKGROUND) tc.pixel = Xbackcolr.pixel;
       else tc.pixel = Xforecolr.pixel;
       col->pixel = tc.pixel;
       return col->pixel;
   }

   if (col->pixel != -1) return col->pixel;

   for (p=Xalloccolr; p!=NULL; p=p->next)
   {
       i = p->ucol.red - col->red;
       j = i = abs(i);
       if (i < nearred)
       {
           nearred = i;
           bestred = p;
       }
       i = p->ucol.green - col->green;
       j += i = abs(i);
       if (i < neargreen)
       {
           neargreen = i;
           bestgreen = p;
       }
       i = p->ucol.blue - col->blue;
       j += i = abs(i);
       if (i < nearblue)
       {
           nearblue = i;
           bestblue = p;
       }
       if (j == 0)
       {
           col->pixel = p->ucol.pixel;
           return col->pixel;
       }
       if (j < nearall)
       {
           nearall = j;
           bestall = p;
       }
   }

   if ( XAllocColor(display,Xclmap,&tc) )
   {
       col->pixel = tc.pixel;
       p = (struct Xcolrused *) MALLOC ( 1, sizeof(struct Xcolrused) );
       p->ucol = *col;
       p->next = Xalloccolr;
       Xalloccolr = p;
   }
   else
     if (threepixes)
            col->pixel = (bestred->ucol.pixel & attr.visual->red_mask)      |
                         (bestgreen->ucol.pixel & attr.visual->green_mask)  |
                         (bestblue->ucol.pixel & attr.visual->blue_mask)    ;
     else col->pixel = bestall->ucol.pixel;

   return col->pixel;
}

/**************************************************** XW_PATTAB *******/

#ifdef PROTO
static void Xw_pattab( Long *pi )
#else
static void Xw_pattab( pi )

/*  stores pattern table entry                           */

Long *pi;
#endif
{
   static int first=TRUE, abort, next, size;
   int cont, n, i, ind, row, col;

   cont = (*pi < 0);
   n = abs( (int) *pi++);
   if (first)
   {
       ind = *pi++;
       row = *pi++;
       col = *pi++;
       if (ind < 0 || ind > XNTILE) abort = TRUE;
       else
       {
          size = row * col;
          if (Xfill.tiles[ind].col != NULL)
             FREE(Xfill.tiles[ind].col);
          if (Xfill.tiles[ind].pm != -1)
             XFreePixmap(display,Xfill.tiles[ind].pm);
          Xfill.tiles[ind].col = (struct colour *)
                                 MALLOC ( size, sizeof(struct colour) );
          Xfill.tiles[ind].pm = -1;
          Xfill.tiles[ind].nx = row;
          Xfill.tiles[ind].ny = col;
          next = 0;
          pi++;
          abort = FALSE;
       }
  }

  if (!abort)
  {
     for ( i=0; next < size && i<n ; i++,next++)
        if (cur.color_mode == INDEXED)
        {
            Xfill.tiles[ind].col[next].index = *pi++;
        }
        else
        {
            Xfill.tiles[ind].col[next].red = rgbtran(*pi++,red);
            Xfill.tiles[ind].col[next].green = rgbtran(*pi++,green);
            Xfill.tiles[ind].col[next].blue = rgbtran(*pi++,blue);
        }
   }

   first = !cont;

   return;
}

/**************************************************** XW_CELL *********/

#ifdef PROTO
static void Xw_cell( Long *pi, Float *pr )
#else
static void Xw_cell( pi, pr )

/*  displays a cell array                                */

Long *pi;
Float *pr;
#endif
{
   static int first=TRUE, next1, next2;
   int cont, n, i, ind;
   struct colour tc;
   XColor tempcolr, *colptr;
   unsigned long pix;

   cont = (*pi < 0);
   n = abs( (int) *pi++);
   if (first)
   {
     if (cur.vdc_type == REAL)
     {
           Xcell.px = Xtranx(*pr++);
           Xcell.py = Xtrany(*pr++);
           Xcell.qx = Xtranx(*pr++);
           Xcell.qy = Xtrany(*pr++);
           Xcell.rx = Xtranx(*pr++);
           Xcell.ry = Xtrany(*pr++);
           pi += 6;
     }
     else
     {
           Xcell.px = Xtranx(*pi++);
           Xcell.py = Xtrany(*pi++);
           Xcell.qx = Xtranx(*pi++);
           Xcell.qy = Xtrany(*pi++);
           Xcell.rx = Xtranx(*pi++);
           Xcell.ry = Xtrany(*pi++);
     }
     Xcell.nx = *pi++;
     Xcell.ny = *pi++;
     pi++;
     Xcell.delx1 = (Xcell.rx-Xcell.px)/Xcell.nx;
     Xcell.dely1 = (Xcell.ry-Xcell.py)/Xcell.nx;
     Xcell.delx2 = (Xcell.qx-Xcell.rx)/Xcell.ny;
     Xcell.dely2 = (Xcell.qy-Xcell.ry)/Xcell.ny;
     next1 = next2 = 0;
  }

  for (i=0; i<n; i++,next1++)
  {
     if (next1 >= Xcell.nx)
     {
        next1 = 0;
        next2++;
     }
     if (cur.color_mode == INDEXED)
     {
        ind = (*pi>XNCOLOR) ? XDFCOLOR : *pi;
        colptr = &Xcolrtab[ind];
        pi++;
     }
     else
     {
          tc.red = *pi++;
          tc.green = *pi++;
          tc.blue = *pi++;
          Xw_copycolr(&tempcolr,&tc);
          tempcolr.pixel = -1;
          colptr = &tempcolr;
     }
     pix = Xw_getpix(colptr,ASIS);

      Xwork.p[0].x = Xcell.px +
                           next1 * Xcell.delx1 + next2 * Xcell.delx2;
      Xwork.p[0].y = Xcell.py +
                           next1 * Xcell.dely1 + next2 * Xcell.dely2;
      Xwork.p[1].x = Xcell.px +
                           (next1+1) * Xcell.delx1 + next2 * Xcell.delx2;
      Xwork.p[1].y = Xcell.py +
                           (next1+1) * Xcell.dely1 + next2 * Xcell.dely2;
      Xwork.p[2].x = Xcell.px +
                           (next1+1) * Xcell.delx1 + (next2+1) * Xcell.delx2;
      Xwork.p[2].y = Xcell.py +
                           (next1+1) * Xcell.dely1 + (next2+1) * Xcell.dely2;
      Xwork.p[3].x = Xcell.px +
                           next1 * Xcell.delx1 + (next2+1) * Xcell.delx2;
      Xwork.p[3].y = Xcell.py +
                           next1 * Xcell.dely1 + (next2+1) * Xcell.dely2;

      XSetForeground(display, gc_cell, pix);

      XFillPolygon(display,picture,gc_cell,
                   Xwork.p,4,Convex,CoordModeOrigin);
      if((Xw_backing_store != Always))
        XFillPolygon(display,back_pix,gc_cell,
                   Xwork.p,4,Convex,CoordModeOrigin);

  }

  first = !cont;

  return;
}


/**************************************************** XW_EVENT ********/

#ifdef PROTO
static void Xw_event( int type )
#else
static void Xw_event( type )
int type;
#endif
{
   XEvent evnt;

   while (XPending(display) != 0)
   {
       XNextEvent(display,&evnt);
       Xw_dispatch(&evnt);
   }

   if (type == NOWAIT) return;

   Xw_waiting = TRUE;
   Xw_error = FALSE;
   if(cgmabort)
     {
      Xw_outmesg("Aborted current frame.","");
      cgmabort = FALSE;
     }

   XBell(display,50);
   while ((XPending(display) != 0) || Xw_waiting || Xw_winwait)
   {
       XNextEvent(display,&evnt);
       Xw_dispatch(&evnt);
       if (Xw_error)
       {
           if (type == NOERROR) Xw_waiting = FALSE;
           else
           {
               Xw_outmesg("invalid reply","");
               Xw_error = FALSE;
           }
       }
   }
   return;
}

/**************************************************** XW_OUTMESG *********/

#ifdef PROTO
static void Xw_outmesg( char *msg, char *prefix)
#else
static void Xw_outmesg( msg, prefix )
char *msg, *prefix;
#endif
{
   int len, plen;
   char *p;

   if (prefix[0] == '\0') sprintf(Xw_curwin->mesgbuff,"%5d :: ",cgmpresent);
   else                sprintf(Xw_curwin->mesgbuff,"%.8s :: ",prefix);
   plen = strlen(Xw_curwin->mesgbuff);
   p = &Xw_curwin->mesgbuff[plen];
   strncpy(p,msg,XMESGLEN-plen);
   Xw_curwin->mesgbuff[XMESGLEN-1] = '\0';
   len = strlen(Xw_curwin->mesgbuff);
   XClearWindow(display,Xw_curwin->tx.win);
   XDrawString(display,Xw_curwin->tx.win,gc_men,
             Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
             Xw_curwin->mesgbuff,len);
   return;
}

/************************************************** XW_MENU ************/

#ifdef PROTO
static void Xw_menu( Window w, struct Xwin_rec *p, XEvent *ev )
#else
static void Xw_menu( w, p, ev )
Window w;
XEvent *ev;
struct Xwin_rec *p;
#endif
{
   struct Xwin_rec **q;
   Logical waitstate;
   int i, evmask= -1;

   if (ev->type == ButtonPress) return;

   Xw_zmtype = 0;
   waitstate = (cgmstate == PIC_CLOSED) && Xw_waiting;

   Xw_curwin = p;
   picture = Xw_curwin->pi.win;
   if((Xw_backing_store != Always))
     back_pix = Xw_curwin->pi.pix;

   if (w == Xmenu.m[7].win)
   {
     XCloseDisplay(display);
     exit(0);
   }

   if ( (w != Xmenu.m[6].win) && !waitstate ) return;
   if (w == Xmenu.m[5].win)
   {
       p->zoom.x = p->zoom.y = 0;
       p->zoom.xscale = p->zoom.yscale = 1.0;
       Xw_setwin(SCREEN+CLIPAREA);
       sscanf(p->mesgbuff,"%d",&cgmnext);
       Xw_waiting = FALSE;
   }
   else
     if (w == Xmenu.m[6].win)
     {
         Xw_numwin--;
         if (Xw_numwin == 0)
         {
             XCloseDisplay(display);
             exit(0);
         }
         XSelectInput(display,p->top.win,NoEventMask);
         while( XCheckWindowEvent(display,p->top.win,evmask,ev));
         XSelectInput(display,p->pi.win,NoEventMask);
         while( XCheckWindowEvent(display,p->pi.win,evmask,ev));
         XSelectInput(display,p->tx.win,NoEventMask);
         while( XCheckWindowEvent(display,p->tx.win,evmask,ev));
         for (i=1; i<=XNBUTN; i++)
         {
              XSelectInput(display,p->b[i].win,NoEventMask);
              while( XCheckWindowEvent(display,p->b[i].win,evmask,ev));
         }

         for (q= &Xw_winlist; *q!=p; q= &(*q)->next);
         *q = p->next;
         Xw_curwin = Xw_winlist;
         XReparentWindow(display,Xmenu.shell.win,
                          Xw_curwin->top.win,0,0);
         Xw_menu_parent = Xw_curwin->top.win;
         XReparentWindow(display,Xkb.shell.win,
                          Xw_curwin->top.win,0,0);
         Xw_kb_parent = Xw_curwin->top.win;
         if(p->pi.pixback)
            XFreePixmap(display,p->pi.pix);
         XDestroyWindow(display,p->top.win);
         FREE(p);
     }
     else
         if (w == Xmenu.m[1].win)  Xw_newview(w, p);
         else
         {
             Xw_zmwin = Xw_curwin;
             if (w == Xmenu.m[2].win) Xw_zmtype = 1;
             else
                if (w == Xmenu.m[3].win) Xw_zmtype = 2;
                else
                  if (w == Xmenu.m[4].win) Xw_zmtype = 3;
                  else ;
             Xw_defpts = TRUE;
         }

   XUnmapWindow(display,Xmenu.shell.win);
   Xw_upmenu = FALSE;

   return;
}

/**************************************************** XW_KB ************/

#ifdef PROTO
static void Xw_kb( Window w, char *cld, struct Xwin_rec *p, XEvent *ev )
#else
static void Xw_kb( w, cld, p, ev )
Window w;
char *cld;
struct Xwin_rec *p;
XEvent *ev;
#endif
{
   int len;
   char temp[XKBMLEN];

   if (ev->type == ButtonPress) return;
   Xw_curwin = p;

   picture = Xw_curwin->pi.win;
   if((Xw_backing_store != Always))
     back_pix = Xw_curwin->pi.pix;

   switch (*cld)
   {
      case 'C':
        Xkb.frame = 0;
        XClearWindow(display,Xkb.tx.win);
        break;

      case 'D':
        if (Xkb.frame != 0)
        {
           cgmnext = Xkb.frame;
           Xw_waiting = FALSE;
        }
        XUnmapWindow(display,Xkb.shell.win);
        Xw_upkb = FALSE;
        break;

      default:
        if (Xkb.frame<100000)
        {
           Xkb.frame *= 10;
           Xkb.frame += (*cld-'0');
           sprintf(temp,"%d",Xkb.frame);
           len = strlen(temp);
           XClearWindow(display,Xkb.tx.win);
           XDrawString(display,Xkb.tx.win,gc_men,
                Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,temp,len);
           break;
       }
    }
    return;
}


/************************************************** XW_BUTN ************/

#ifdef PROTO
static void Xw_butn( Window w, struct Xwin_rec *p, XEvent *ev )
#else
static void Xw_butn( w, p, ev )
Window w;
struct Xwin_rec *p;
XEvent *ev;
#endif
{
   short xmin, ymin, xmax, ymax, d;
   float s, s1;
   int xtop, ytop;
   XRectangle rect[1];

   if ((ev->type != ButtonPress) && (ev->type != ButtonRelease))
      return;

   Xw_curwin = p;

   picture = Xw_curwin->pi.win;
   if((Xw_backing_store != Always))
     back_pix = Xw_curwin->pi.pix;

   switch (ev->xbutton.button)
   {
     case Button1:
       if (ev->type == ButtonPress) return;
       if (Xw_upmenu || Xw_defpts) return;
       if (!Xw_waiting) return;

       if (!Xw_upkb)
       {
          xtop = ev->xbutton.x;
          ytop = ev->xbutton.y;

          if (xtop > p->neww-Xkb.w) xtop = p->neww-Xkb.w;
          if (ytop > p->newh-Xkb.h) ytop = p->newh-Xkb.h;

          Xw_upkb = TRUE;
          Xkb.frame = 0;
          XReparentWindow(display,Xkb.shell.win,p->top.win,xtop,ytop);
          Xw_kb_parent = p->top.win;
          XClearWindow(display,Xkb.tx.win);
          XMapRaised(display,Xkb.shell.win);
          XMapSubwindows(display,Xkb.shell.win);
          return;
       }
       else
       {
          XUnmapWindow(display,Xkb.shell.win);
          Xw_upkb = FALSE;
          return;
        }

     case Button2:
       if (ev->type == ButtonPress) return;
       if (Xw_upkb || Xw_defpts) return;
       if (!Xw_waiting) return;

       if (!Xw_upmenu)
       {
          xtop = ev->xbutton.x;
          ytop = ev->xbutton.y;

          if (xtop > p->neww-Xmenu.w) xtop = p->neww-Xmenu.w;
          if (ytop > p->newh-Xmenu.h) ytop = p->newh-Xmenu.h;

          Xw_upmenu = TRUE;
          XReparentWindow(display,Xmenu.shell.win,p->top.win,xtop,ytop);
          Xw_menu_parent = p->top.win;


          XMapRaised(display,Xmenu.shell.win);
          XMapSubwindows(display,Xmenu.shell.win);
          Xw_zmtype = 0;
          return;
       }
       else
       {

          XUnmapWindow(display,Xmenu.shell.win);
          Xw_upmenu = FALSE;
          return;
       }

     case Button3:
       if (ev->xbutton.button != Button3) return;
       if (Xw_upkb || Xw_upmenu) return;

       if (Xw_zmtype == 0) return;
       if (Xw_zmwin != p)
       {
          Xw_zmtype = 0;
          return;
       }

       if (ev->type == ButtonPress)
       {
          p->zoom.x1 = ev->xbutton.x;
          p->zoom.y1 = ev->xbutton.y;

          rect[0].x = ev->xbutton.x - 3;
          rect[0].y = ev->xbutton.y - 3;
          rect[0].width =  rect[0].height = 7;
          XSetForeground(display,gc_dflt,Xforecolr.pixel);
          XFillRectangles(display,picture,gc_dflt,rect,1);
          XSetForeground(display,gc_dflt,Xbackcolr.pixel);
          XDrawRectangles(display,picture,gc_dflt,rect,1);
       }
       else
       {
          Xw_defpts = FALSE;
          p->zoom.x2 = ev->xbutton.x;
          p->zoom.y2 = ev->xbutton.y;
          xmin = p->zoom.x1 < p->zoom.x2 ? p->zoom.x1 : p->zoom.x2;
          ymin = p->zoom.y1 < p->zoom.y2 ? p->zoom.y1 : p->zoom.y2;
          xmax = p->zoom.x1 > p->zoom.x2 ? p->zoom.x1 : p->zoom.x2;
          ymax = p->zoom.y1 > p->zoom.y2 ? p->zoom.y1 : p->zoom.y2;
          switch (Xw_zmtype)
          {
             case 1:
                if ((xmax == xmin) || (ymax == ymin)) break;
                s = ((float) p->neww)/((float) (xmax-xmin));
                s1 = ((float) p->newh)/((float) (ymax-ymin));
                s = s < s1 ? s : s1;
                d = -xmin;
                p->zoom.x += d/p->zoom.xscale;
                p->zoom.xscale *= s;
                d = -ymin;
                p->zoom.y += d/p->zoom.yscale;
                p->zoom.yscale *= s;
                break;

              case 2:
                if ((xmax == xmin) || (ymax == ymin)) break;
                s = ((float) (xmax-xmin))/((float) p->neww);
                s1 = ((float) (ymax-ymin))/((float) p->newh);
                s = s < s1 ? s : s1;
                d = xmin/s;
                p->zoom.x += d/p->zoom.xscale;
                p->zoom.xscale *= s;
                d = ymin/s;
                p->zoom.y += d/p->zoom.yscale;
                p->zoom.yscale *= s;
                break;

              case 3:
                p->zoom.x += ((double)(p->zoom.x2 - p->zoom.x1))/p->zoom.xscale;
                p->zoom.y += ((double)(p->zoom.y2 - p->zoom.y1))/p->zoom.yscale;
                break;

              default:
                break;
           }
           Xw_setwin(SCREEN+CLIPAREA);
           Xw_zmtype = 0;
           sscanf(p->mesgbuff,"%d",&cgmnext);
           Xw_waiting = FALSE;
         }
         break;

      default: break;
   }
   return;
}

/************************************************** XW_CMND ************/

#ifdef PROTO
static void Xw_cmnd( Window w, struct Xwin_rec *p, XEvent *ev )
#else
static void Xw_cmnd( w, p, ev )
Window w;
XEvent *ev;
struct Xwin_rec *p;
#endif
{
   int temp;
   Logical waitstate;

   if (ev->type == ButtonPress) return;

   if (Xw_msgwait)
   {
     Xw_waiting = FALSE;
     Xw_msgwait = FALSE;
     return;
   }

   if (Xw_upmenu || Xw_upkb || Xw_defpts) return;

   Xw_zmtype = 0;
   waitstate = Xw_firstmf | ((cgmstate == PIC_CLOSED) && Xw_waiting);

   if (w == p->b[4].win)
   {
     if (!waitstate && (p == Xw_curwin))
       {
        cgmabort = 1;
        Xw_outmesg("Aborting, skipping elements to end of frame....","");
       }

     return;
   }
   else
   {
     if (!waitstate) return;
     if (sscanf(p->mesgbuff,"%d",&temp) != 1) Xw_error = TRUE;
     else
     {
         if (w == p->b[3].win) cgmnext = temp;
         else if (w == p->b[1].win) cgmnext = ++temp;
              else cgmnext = --temp;
         Xw_waiting = FALSE;
     }
   }
   Xw_curwin = p;

   picture = Xw_curwin->pi.win;
   if((Xw_backing_store != Always))
     back_pix = Xw_curwin->pi.pix;

   return;
}

/************************************************ XW_RESIZE ************/

#ifdef PROTO
static void Xw_resize( Window wi, struct Xwin_rec *p, XEvent *ev )
#else
static void Xw_resize( wi, p, ev )
Window wi;
struct Xwin_rec *p;
XEvent *ev;
#endif
{
   unsigned int w,h;
   int x,y,xret,yret;
   Logical waitstate;
   Window rret;
   unsigned int wret,hret,bret,dret;
   XWindowAttributes wininfo;

   waitstate = (cgmstate == PIC_CLOSED) && Xw_waiting;

   if ((p->topw != ev->xconfigure.width) ||
       (p->toph != ev->xconfigure.height) )
   {
      p->topw = ev->xconfigure.width;
      p->toph = ev->xconfigure.height;
      p->neww = ev->xconfigure.width - 2*Xw_winparm.wpack - 2;
      p->newh = ev->xconfigure.height - 3*Xw_winparm.hpack - 4
                                      - Xw_winparm.buth;
      if (waitstate)
      {
         sscanf(p->mesgbuff,"%d",&cgmnext);
         Xw_waiting = FALSE;
      }
   }
   p->new_coords = TRUE;

   x = p->newx = Xw_winparm.wpack;
   y = p->newy = Xw_winparm.hpack;
   w = p->neww;
   h = p->newh;
   XMoveResizeWindow(display,p->pi.win,x,y,w,h);
   y = p->newh+2+2*Xw_winparm.hpack;
   w = Xw_winparm.butw;
   h = Xw_winparm.buth;
   XMoveResizeWindow(display,p->b[1].win,x,y,w,h);
   x += Xw_winparm.wpack+2+w;
   XMoveResizeWindow(display,p->b[2].win,x,y,w,h);
   x += Xw_winparm.wpack+2+w;
   XMoveResizeWindow(display,p->b[3].win,x,y,w,h);
   x += Xw_winparm.wpack+2+w;
   XMoveResizeWindow(display,p->b[4].win,x,y,w,h);
   x += Xw_winparm.wpack+2+w;
   w = p->topw-x-2-Xw_winparm.wpack;
   XMoveResizeWindow(display,p->tx.win,x,y,w,h);

   if ((Xw_menu_parent == p->top.win) && Xw_upmenu )
   {
       XGetGeometry(display,Xmenu.shell.win,
                        &rret,&xret,&yret,&wret,&hret,&bret,&dret);
       if (xret+wret > p->neww) xret = p->neww - wret;
       if (yret+hret > p->newh) yret = p->newh - hret;
       XMoveWindow(display,Xmenu.shell.win,xret,yret);
   }
   else if ((Xw_kb_parent == p->top.win) && Xw_upkb )
        {
           XGetGeometry(display,Xkb.shell.win,
                        &rret,&xret,&yret,&wret,&hret,&bret,&dret);
           if (xret+wret > p->neww) xret = p->neww - wret;
           if (yret+hret > p->newh) yret = p->newh - hret;
           XMoveWindow(display,Xkb.shell.win,xret,yret);
         }

   Xw_curwin = p;
   picture = Xw_curwin->pi.win;
/*
  destroy old pixmap and create a new pixmap of main picture window
  if using pixmap backing store.
*/
   if((Xw_backing_store != Always) && Xw_curwin->pi.pixback)
     {
      XFreePixmap(display,Xw_curwin->pi.pix);
      XGetWindowAttributes(display,Xw_curwin->pi.win,&wininfo);
      Xw_curwin->pi.pix = XCreatePixmap(display,Xw_curwin->pi.win,wininfo.width,
                    wininfo.height,wininfo.depth);
      Xw_curwin->pi.pixback = TRUE;
      back_pix = Xw_curwin->pi.pix;
      Xw_fillpix(back_pix,Xcolrtab[0].pixel);
     }


   Xw_setwin(SCREEN+CLIPAREA);

   return;
}

/*********************************************** XW_NEWVIEW ************/

#ifdef PROTO
static void Xw_newview( Window wi, struct Xwin_rec *oldwin )
#else
static void Xw_newview( wi, oldwin )
Window wi;
struct Xwin_rec *oldwin;
#endif
{
   struct Xwin_rec *newwin;
   int temp;

   newwin = (struct Xwin_rec *) MALLOC ( 1, sizeof(struct Xwin_rec) );
   newwin->zoom.x = newwin->zoom.y = 0;
   newwin->zoom.xscale = newwin->zoom.yscale = 1.0;
   strcpy(newwin->mesgbuff,oldwin->mesgbuff);
   Xw_windows(newwin);
   sscanf(newwin->mesgbuff,"%d",&temp);
   cgmnext = temp;
   Xw_waiting = FALSE;
   Xw_curwin = newwin;
   Xw_setwin(WORLD+SCREEN);

   picture = Xw_curwin->pi.win;
   XSetWindowBackground(display,picture,Xbackcolr.pixel);
   XClearWindow(display,picture);
   if((Xw_backing_store != Always))
     {
      back_pix = Xw_curwin->pi.pix;
      Xw_fillpix(back_pix,Xbackcolr.pixel);
     }

   return;
}

/*********************************************** XW_WINDOWS ************/

#ifdef PROTO
static void Xw_windows( struct Xwin_rec *win )
#else
static void Xw_windows( win )
struct Xwin_rec *win;
#endif
{
   unsigned int w, h;
   int x, y, i, len;
   unsigned long evmask, atmask;
   XWMHints wmhints;
   XSizeHints szhints;
   int argc;
   char *argv[3];
   XWindowAttributes wininfo;

   if (Xw_firstmf)
   {
       Xw_basics();
       Xw_setwsiz(Xw_groupwin);
   }

   win->pix_per_mm = ((double) WidthOfScreen(scrn))
                                   /WidthMMOfScreen(scrn);
   win->topw = WidthOfScreen(scrn);
   win->toph = HeightOfScreen(scrn);
   win->topw = win->toph = win->topw<win->toph ? 0.75*win->topw
                                               : 0.75*win->toph;

   argv[0] = "cgm";
   argv[1] = "-display";
   argv[2] = terminal;
   argc = 3;
   wmhints.flags = InputHint + StateHint + WindowGroupHint;
   wmhints.input = FALSE;
   wmhints.initial_state = NormalState;
   wmhints.window_group = Xw_groupwin;
   szhints.flags = PPosition + PSize + PMinSize;
   szhints.x = szhints.y = 0;
   szhints.width = win->topw;
   szhints.height = win->toph;
   szhints.min_width = XNBUTN*(Xw_winparm.butw+2) + 2 +
                       (XNBUTN+2)*Xw_winparm.wpack + Xw_winparm.menw;
   szhints.min_height = (XNMENUITEM+1)*(Xw_winparm.buth+2) +
                        (XNMENUITEM+4)*Xw_winparm.hpack + 6;
   evmask = StructureNotifyMask;
   atmask = CWDontPropagate;
   if (Xw_firstmf)
   {
       XSetStandardProperties(display,Xw_groupwin,cgmversion,"ralcgm",
              None,argv,argc,&szhints);
       XSetWMHints(display,Xw_groupwin,&wmhints);
   }

   win->top.win = Xw_crwin(root,0,0,win->topw,win->toph,evmask,atmask);
   win->top.pixback = FALSE;
   XSetStandardProperties(display,win->top.win,cgmversion,"ralcgm",
              None,argv,argc,&szhints);
   XSetWMHints(display,win->top.win,&wmhints);


   win->neww = win->topw - 2*Xw_winparm.wpack-2;
   win->newh = win->toph - 3*Xw_winparm.hpack-4-Xw_winparm.buth;
   win->newx = Xw_winparm.wpack;
   win->newy = Xw_winparm.hpack;
   win->new_coords = TRUE;
   evmask = ButtonPressMask + ButtonReleaseMask + ExposureMask;
   atmask = CWDontPropagate + CWOverrideRedirect +
            CWSaveUnder     + CWBackingStore;
   win->pi.win = Xw_crwin(win->top.win,win->newx,
                  win->newy,win->neww,win->newh,evmask,atmask);

   XGetWindowAttributes(display,win->pi.win,&wininfo);

/* create pixmap of main picture window if no backing store */
   if(Xw_backing_store != Always)
     {
      win->pi.pix = XCreatePixmap(display,win->pi.win,wininfo.width,
                    wininfo.height,wininfo.depth);
      win->pi.pixback = TRUE;
     }
   else
      win->pi.pixback = FALSE;


   w = Xw_winparm.butw;
   h = Xw_winparm.buth;
   x = Xw_winparm.wpack;
   y = 2*Xw_winparm.hpack+2+win->newh;
   evmask = ButtonReleaseMask + ButtonPressMask +
            EnterWindowMask + LeaveWindowMask +
            ExposureMask ;
   atmask = CWDontPropagate + CWOverrideRedirect + CWBackingStore;
   win->b[1].win = Xw_crwin(win->top.win,x,y,w,h,evmask,atmask);
   strcpy(win->b[1].title,"Nx");
   win->b[1].written = FALSE;
   win->b[1].pixback = FALSE;
   x += Xw_winparm.wpack+2+w;
   win->b[2].win = Xw_crwin(win->top.win,x,y,w,h,evmask,atmask);
   strcpy(win->b[2].title,"Pr");
   win->b[2].written = FALSE;
   win->b[2].pixback = FALSE;
   x += Xw_winparm.wpack+2+w;
   win->b[3].win = Xw_crwin(win->top.win,x,y,w,h,evmask,atmask);
   strcpy(win->b[3].title,"Cu");
   win->b[3].written = FALSE;
   win->b[3].pixback = FALSE;
   x += Xw_winparm.wpack+2+w;
   win->b[4].win = Xw_crwin(win->top.win,x,y,w,h,evmask,atmask);
   strcpy(win->b[4].title,"Ab");
   win->b[4].written = FALSE;
   win->b[4].pixback = FALSE;
   x += Xw_winparm.wpack+2+w;
   w = win->topw-x-Xw_winparm.wpack-2;
   evmask = ExposureMask;
   win->tx.win = Xw_crwin(win->top.win,x,y,w,h,evmask,atmask);
   win->tx.written = FALSE;
   win->tx.pixback = FALSE;

  if (Xw_backing_store == Always)
  {
      len = strlen(win->mesgbuff);
      XClearWindow(display,win->tx.win);
      XDrawString(display,win->tx.win,gc_men,
           Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
           win->mesgbuff,len);
      win->tx.written = !(Xw_backing_store == NotUseful);
      for (i=1; i<= XNBUTN; i++)
      {
           len = strlen(win->b[i].title);
           XClearWindow(display,win->b[i].win);
           XDrawString(display,win->b[i].win,gc_men,
               Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
               win->b[i].title,len);
           win->b[i].written = !(Xw_backing_store == NotUseful);
      }
  }

   win->first = TRUE;
   XMapRaised(display,win->top.win);
   XMapSubwindows(display,win->top.win);
   Xw_winwait = TRUE;
   XDefineCursor(display,win->pi.win,Xw_cursor);
   Xw_numwin++;
   win->next = Xw_winlist;
   Xw_winlist = win;
}

/******************************************************* XW_BASICS *****/

#ifdef PROTO
static void Xw_basics( void )
#else
static void Xw_basics()
#endif
{
  char *pchar;
  int i,j;
  XColor  colour;

  if ( terminal[0] == '\0')
  {
#ifdef VMS
     pchar = (char *)getenv("DECW$DISPLAY");
#else
     pchar = (char *)getenv("DISPLAY");
#endif
    if (pchar != NULL) strcpy(terminal,pchar);
#ifdef CMS
    else
    {
        exit ( CGMerror ( func, ERR_NOTERM, FATAL, NULLSTR ) );
    }
#else
#ifdef VMS
    else
    {
        (void) sprintf( mess,
           "(Use -a option or the DCL SET DISPLAY command)");
        exit( CGMerror ( func, ERR_NOTERM, FATAL, mess) );
    }
#else
    else gethostname(terminal, TERMLEN-3);
#endif
#endif
  }
  j = 0;
  for (i=strlen(terminal); i>0 ; i--)
  {
      if (terminal[i-1] == '.')
        j = i;
      else if( !isdigit(terminal[i-1]) )
        break;
  }
  if ( (i==0) || (terminal[i-1] != ':') )
  {
#ifdef VMS
    strcat(terminal,"::0");
#else
    strcat(terminal,":0");
#endif
    screen = 0;
  }
  else
  {
     if (j==0)
     {
        if( (terminal[i-1] = ':') && (!isdigit(terminal[i])) )
            strcat(terminal,"0");
        screen = 0;
     }
     else sscanf(&terminal[j],"%d",&screen);
  }

  if ( (display=XOpenDisplay(terminal)) == NULL)
  {
     exit( CGMerror (func, ERR_XOPENDISP, FATAL, terminal) );
  }

  scrn = ScreenOfDisplay(display,screen);
  depth = DefaultDepthOfScreen(scrn);
  root = RootWindow(display,screen);
  XGetWindowAttributes(display,root,&attr);
  Xdfclmap = attr.colormap;

  can_set_colour = attr.visual->class == DirectColor  ||
                   attr.visual->class == PseudoColor  ||
                   attr.visual->class == GrayScale    ;
  monochrome     = attr.visual->class == StaticGray   ||
                   attr.visual->class == GrayScale    ;
  threepixes     = attr.visual->class == TrueColor    ||
                   attr.visual->class == DirectColor  ;

  Xw_cursor = XCreateFontCursor(display,XWCURSOR);

  Xw_backing_store = DoesBackingStore(scrn);

  if (depth == 1)
  {
     Xw_black = 1;
     Xw_white = 0;
  }
  else
  {
       colour.red = colour.green = colour.blue = 0;
       colour.flags = DoRed+DoGreen+DoBlue;
       XAllocColor(display,Xdfclmap,&colour);
       Xw_black = colour.pixel;
       colour.red = colour.green = colour.blue = 65535;
       colour.flags = DoRed+DoGreen+DoBlue;
       XAllocColor(display,Xdfclmap,&colour);
       Xw_white = colour.pixel;
  }

  Xw_groupwin = XCreateSimpleWindow(display,root,0,0,10,10,1,0,0);

  return;
}

/**************************************************** XW_DISPATCH *****/

#ifdef PROTO
static void Xw_dispatch( XEvent *evnt )
#else
static void Xw_dispatch( evnt )
XEvent *evnt;
#endif
{
   Window rt,pa,*ch, topw, dispw;
   unsigned int nch;
   int i, len;
   struct Xwin_rec  *p;
   char temp[XKBMLEN];

   if ( (evnt->xany.window == Xmenu.shell.win) ||
        (evnt->xany.window == Xkb.shell.win) ) return;
   XQueryTree(display,evnt->xany.window,&rt,&topw,&ch,&nch);
   XFree((char*)ch);

   if (topw == Xmenu.shell.win)
       dispw = Xw_menu_parent;
   else if (topw == Xkb.shell.win)
       dispw = Xw_kb_parent;
   else dispw = topw;

   for (p=Xw_winlist; ; )
   {
       if (p->top.win == dispw) break;
       else if (p->top.win == evnt->xany.window)
            {
                topw = evnt->xany.window;
                break;
            }
            else if ((p = p->next) == NULL) return;
   }

   switch(evnt->type)
   {
       case ButtonPress:
       case ButtonRelease:
         if (topw == Xmenu.shell.win) Xw_menu(evnt->xany.window,p,evnt);
         else if (topw == Xkb.shell.win)
              {
                 for (i=1; Xkb.m[i].win!= evnt->xany.window; i++);
                 Xw_kb(evnt->xany.window,Xkb.m[i].title,p,evnt);
              }
              else if (evnt->xany.window == p->pi.win)
                       Xw_butn(evnt->xany.window,p,evnt);
                   else Xw_cmnd(evnt->xany.window,p,evnt);
         break;
       case EnterNotify:
         XSetWindowBorderWidth(display,evnt->xany.window,2);
         break;
       case LeaveNotify:
         XSetWindowBorderWidth(display,evnt->xany.window,1);
         break;
       case ConfigureNotify:
         if (evnt->xconfigure.window != evnt->xconfigure.event) break;
         Xw_resize(evnt->xconfigure.event,p,evnt);
         break;
       case MapNotify:
         if (evnt->xmap.window != evnt->xmap.event) break;
         if (evnt->xmap.window != p->top.win) break;
         Xw_winwait = FALSE;
         break;
       case Expose:
         if (topw == Xmenu.shell.win)
           {
             if ( evnt->xexpose.count != 0) break;
             for (i=1; i<= XNMENUITEM; i++)
               if (Xmenu.m[i].win == evnt->xexpose.window) break;
               len = strlen(Xmenu.m[i].title);
               XClearWindow(display,Xmenu.m[i].win);
               XDrawString(display,Xmenu.m[i].win,gc_men,
                      Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,
                      Xmenu.m[i].title,len);
               Xmenu.m[i].written = (Xw_backing_store == Always);
           }
         else if (topw == Xkb.shell.win)
           {
             if (evnt->xexpose.window == Xkb.tx.win)
               {
                if ( evnt->xexpose.count != 0) break;
                XClearWindow(display,Xkb.tx.win);
                sprintf(temp,"%d",Xkb.frame);
                len = strlen(temp);
                XDrawString(display,Xkb.tx.win,gc_men,
                    Xw_winparm.wpack,Xw_winparm.buth-Xw_winparm.hpack,temp,len);
                Xkb.tx.written = (Xw_backing_store == Always);
               }
             else
               {
                if ( evnt->xexpose.count != 0) break;
                for (i=1; i<= XNKEYS; i++)
                    if (Xkb.m[i].win == evnt->xexpose.window) break;
                len = strlen(Xkb.m[i].title);
                XClearWindow(display,Xkb.m[i].win);
                XDrawString(display,Xkb.m[i].win,gc_men,Xw_winparm.wpack,
                     Xw_winparm.buth-Xw_winparm.hpack,Xkb.m[i].title,len);
                Xkb.m[i].written = (Xw_backing_store == Always);
               }
           }
         else
           {
            if (evnt->xexpose.window == p->tx.win)
              {
               if ( evnt->xexpose.count != 0) break;
               len = strlen(p->mesgbuff);
               XClearWindow(display,p->tx.win);
               XDrawString(display,p->tx.win,gc_men,Xw_winparm.wpack,
                    Xw_winparm.buth-Xw_winparm.hpack,p->mesgbuff,len);
               p->tx.written = !(Xw_backing_store == NotUseful);
              }
            else if ((evnt->xexpose.window == p->pi.win) && p->pi.pixback)
              {
               XCopyArea (display,p->pi.pix,p->pi.win, gc_dflt,
                         evnt->xexpose.x,evnt->xexpose.y,
                         evnt->xexpose.width,evnt->xexpose.height,
                         evnt->xexpose.x,evnt->xexpose.y);
               XFlush(display);
              }
            else
              {
               if ( evnt->xexpose.count != 0) break;
               for (i=1; i<= XNBUTN; i++)
                 if (p->b[i].win == evnt->xexpose.window) break;
               len = strlen(p->b[i].title);
               XClearWindow(display,p->b[i].win);
               XDrawString(display,p->b[i].win,gc_men,Xw_winparm.wpack,
                    Xw_winparm.buth-Xw_winparm.hpack,p->b[i].title,len);
               p->b[i].written = !(Xw_backing_store == NotUseful);
              }
           }
         break;
       default:
         break;
   }
   return;
}

/**************************************************** XW_SETWSIZ ******/

#ifdef PROTO
static void Xw_setwsiz( Window win )
#else
static void Xw_setwsiz( win )
Window win;
#endif
{
   char *p;
   XFontStruct *fontstr;
   int m, n;
   unsigned long gc_mask, allgc;

   p = (char *)getenv("CGMMENUFONT");
   if (p != NULL) fontstr = XLoadQueryFont(display,p);
   else           fontstr = XLoadQueryFont(display,XWMENUFONT);
   if (fontstr == NULL)
   {
       exit ( CGMerror ( func, ERR_MENUFONT, FATAL, NULLSTR ) );
   }

   m = fontstr->max_bounds.width;
   if ((n = m/3) < 3) n = 3;
   Xw_winparm.wpack = n;
   Xw_winparm.keyw = XNKEYCHAR*m+2*n;
   Xw_winparm.butw = XNBUTCHAR*m+2*n;
   Xw_winparm.menw = XNMENCHAR*m+2*n;

   m = fontstr->max_bounds.ascent+fontstr->max_bounds.descent;
   if ((n = m/3) < 3) n = 3;
   Xw_winparm.hpack = n;
   Xw_winparm.buth = m+2*n;

   gc_dflt = XDefaultGC(display,screen);
   allgc = ( 1L<<(GCLastBit+1) ) -1;

   gc_men  = XCreateGC(display,win,0,&gc_values);
   XCopyGC(display,gc_dflt,allgc,gc_men);

   gc_values.foreground = Xw_black;
   gc_values.background = Xw_white;
   gc_mask = GCForeground+GCBackground;
   XChangeGC(display,gc_men,gc_mask,&gc_values);

   XSetFont(display,gc_men,fontstr->fid);
   XFree((char *)fontstr);

   return;
}

/**************************************************** XW_TXTDRAW ******/

#ifdef PROTO
static void Xw_txtdraw( Textitem *list, Point txtp )
#else
static void Xw_txtdraw( list, txtp )
Textitem *list;
Point txtp;
#endif
{
   Textitem *txtcp,*txtlp=list;
   XColor tempcolr, *colrptr;        /* for setting text colour */
   unsigned long forepix, backpix;   /* for setting text colour */
   int txt_indx;


   if (rt_mode) txtlp=TXTrestrict(txtlp, rt_wid, rt_hgt, &Xwtxtatt);

   if (txtlp!=NULL)
   {
      TXTalign(txtlp, txtp, TXTgwidth(txtlp), TXTgheight(txtlp));
      while (txtlp)
      {
          if ( cur.color_mode == INDEXED )
          {
              colrptr = &Xcolrtab[txtlp->colourindex];
              forepix = Xw_getpix(colrptr,FOREGROUND);
              backpix = Xw_getpix(&Xauxcolr,BACKGROUND);
          }
          else
          {
              tempcolr.pixel = -1;
              tempcolr.red = rgbtran(txtlp->text_col.red, red );
              tempcolr.green = rgbtran(txtlp->text_col.green, green );
              tempcolr.blue = rgbtran(txtlp->text_col.blue, blue );
              colrptr = &tempcolr;
              forepix = Xw_getpix(colrptr,FOREGROUND);
              backpix = Xw_getpix(&Xauxcolr,BACKGROUND);
          }

        switch (txtlp->text_method)
        {
#ifdef HERSHEY
           case HER:
               XSetForeground(display, gc_hertxt, forepix);
               XSetBackground(display, gc_hertxt, backpix);
               if (Xclip.clip_ind == ON)
                 XSetClipRectangles(display, gc_hertxt, 0, 0,
                                 &Xclip.area, 1, Unsorted);
               else XSetClipMask(display,gc_hertxt,None);
               HERtext(txtlp, txtp, Xw_hertxt);
               break;
#endif
#ifdef BEZIER
           case BEZ:
               XSetForeground(display, gc_beztxt, forepix);
               XSetBackground(display, gc_beztxt, backpix);
               if (Xclip.clip_ind == ON)
                 XSetClipRectangles(display, gc_beztxt, 0, 0,
                                  &Xclip.area, 1, Unsorted);
               else XSetClipMask(display,gc_beztxt,None);
               BEZtext(txtlp, &Xwtxtatt, txtp, Xw_beztxt);
               break;
#endif
           case HDW:
               XSetForeground(display, gc_text, forepix);
               XSetBackground(display, gc_text, backpix);
               if (Xclip.clip_ind == ON)
                 XSetClipRectangles(display, gc_text, 0, 0,
                                 &Xclip.area, 1, Unsorted);
               else XSetClipMask(display,gc_text,None);
               txt_indx = (txtlp->text_font > XNFONT) ? 0 : txtlp->text_font;
               if(Xw_font.index != txt_indx)
               {
                 if(Xw_font.fstr != NULL)
                    XFreeFont(display,Xw_font.fstr);
                  Xw_font.fstr = XLoadQueryFont(display,Xfnt_serv[txt_indx]);
                  Xw_font.index = txt_indx;
               }
               else
               {
                 if (Xw_font.fstr == NULL)
                    Xw_font.fstr = XLoadQueryFont(display,Xfnt_serv[txt_indx]);
               }
               XSetFont(display,gc_text,Xw_font.fstr->fid);
               Xw_hdwtxt(txtlp, txtp);
               break;
           case PS:
                /* no routine  */
           case TEX:
               /* No routine yet */
            default:
               break;
        }
        txtlp = txtlp->next;
      }
      TXTfree(list);
   }
   return;
}

/************************************************** Xw_beztxt **********
* Xw_beztxt() :output Bezier text
*
* Input Arguments:
*     n : number of points
*bezpts : pointer to x and y points for outlines
*   Mat : a 3 by 2 matrix that repersent the transformation for the
*         points.
*newchar: boolean newcharacter flag.
* Output Arguments:
*     None.
* Return value:
*     None.
************************************************************************/

#ifdef PROTO
static void Xw_beztxt( int n, Point *bezpts, Tmatrix mat, int newchar)
#else
static void Xw_beztxt( n, bezpts, mat, newchar )
int n;
Point *bezpts ;
Tmatrix mat;
int newchar;
#endif
{
    Xw_stktxt(n,bezpts,mat,0,newchar);
    return;
}

/****************************************************** Xw_hertxt ******
* Xw_hertxt() :output Hershey
*
* Input Arguments:
*     n : number of points
*   herpts : pointer to x and y points
*   Mat : a 3 by 2 matrix that repersent the transformation for the
*         points.
*newchar: boolean newcharacter flag.
* Output Arguments:
*     None.
* Return value:
*     None.
************************************************************************/

#ifdef PROTO
static void Xw_hertxt( int n, Point *herpts, Tmatrix mat, int newchar )
#else
static void Xw_hertxt( n, herpts, mat, newchar )
int n;
Point *herpts;
Tmatrix mat;
int newchar;
#endif
{
    Xw_stktxt(n,herpts,mat,1,newchar);
    return;
}

/********************************************************* Xw_stktxt ***
* Xw_stktxt() : draw stroke text output for Bezier or Hershey text
*
* Input Arguments:
*     n    : number of points
*   stkpts : pointer to x and y points
*   Mat    : a 3 by 2 matrix that repersent the transformation for the
*            points.
* methodfg : 0 for Bezier, 1 for Hershey
*  newchar : boolean newcharacter flag.
*
* Output Arguments:
*     None.
* Return value:
*     None.
************************************************************************/

#ifdef PROTO
static void Xw_stktxt( int n, Point *stkpts, Tmatrix mat,
                       int methodfg, int newchar)
#else
static void Xw_stktxt(n,stkpts,mat,methodfg,newchar)
int n;
int methodfg;
Point *stkpts;
Tmatrix mat;
int newchar;
#endif
{
   int i,j,k;
   float A,B,C,D,E,F;
   XColor tempcolr, *colrptr;        /* for setting text colour */
   unsigned long forepix, backpix;   /* for setting text colour */

   n=abs(n);
   if (n<2) return;

/* transform points list to X coords */

/*  Matrix passed into routine gives values a,b,c,d,e,f for a matrix A:

                      |a b 0|
                A ==> |c d 0|
                      |e f 1|

 this requires the points as a row vector V ==> [x y 1]

 the transformation of V by A gives the drawing points in VDC coordinates,

 ie:

              V' = V.A

 the points V' need to be transformed to the coordinates for the X window by
 another matrix B, ie:

              V'' = V.A.B

 Elsewhere in this driver this is performed by the macros Xtranx and Xtrany.
 The matrix of these macros (B) can be concatenated with the matrix passed into
 the routine (A) so that only one transformation (C) is required to convert
 Bezier of Hershey cooordinates into X coordinates.

 From the macros Xtranx and Xtrany the matrix B is:

       |Xw_curwin->tot_xscale              0                 0|
       |         0              -Xw_curwin->tot_yscale       0|
       |-Xw_curwin->tot_xdisp    Xw_curwin->tot_ydisp        1|


 so the matrix C is:

   |a*Xw_curwin->tot_xscale    -b*Xw_curwin->tot_yscale         0|
   |c*Xw_curwin->tot_xscale    -d*Xw_curwin->tot_yscale         0|
   |(e*Xw_curwin->tot_xscale-  (-f*Xw_curwin->tot_yscale         |
   |    Xw_curwin->tot_xdisp)    +Xw_curwin->tot_ydisp)         1|


 using this matrix:

         V'' = V.C
 or :

[x'' y'' 1] =

      [x y 1].|a*Xw_curwin->tot_xscale    -b*Xw_curwin->tot_yscale         0|
              |c*Xw_curwin->tot_xscale    -d*Xw_curwin->tot_yscale         0|
              |(e*Xw_curwin->tot_xscale-  (-f*Xw_curwin->tot_yscale         |
              |    Xw_curwin->tot_xdisp)    +Xw_curwin->tot_ydisp)         1|

 or
   x'' = Xw_curwin->tot_xscale(x*a+y*c+e)-Xw_curwin->tot_xdisp
   y'' = -Xw_curwin->tot_yscale(x*b+d*y+f)+Xw_curwin->tot_ydisp

 to reduce the number of computations calculate:
   A=Xw_curwin->tot_xscale*a
   B=-Xw_curwin->tot_yscale*b
   C=Xw_curwin->tot_xscale*c
   D=-Xw_curwin->tot_yscale*d
   E=Xw_curwin->tot_xscale*e-Xw_curwin->tot_xdisp
   F=-Xw_curwin->tot_yscale*f+Xw_curwin->tot_ydisp

so
   x''=Ax+Cy+E
   y''=Bx+Dy+F

*****************************************************************************/

   A = Xw_curwin->tot_xscale*mat[0][0];
   B = -Xw_curwin->tot_yscale*mat[0][1];
   C = Xw_curwin->tot_xscale*mat[1][0];
   D = -Xw_curwin->tot_yscale*mat[1][1];
   E = Xw_curwin->tot_xscale*mat[2][0]-Xw_curwin->tot_xdisp;
   F = -Xw_curwin->tot_yscale*mat[2][1]+Xw_curwin->tot_ydisp;

   for ( k=0; k<n; k++)
   {
     Xwork.p[k].x =stkpts[k].x*A + stkpts[k].y*C + E;
     Xwork.p[k].y =stkpts[k].x*B + stkpts[k].y*D + F;
   }

/* do the drawing */
   if(!methodfg)
     {
      XFillPolygon(display,picture,gc_beztxt,Xwork.p,n,
                                          Complex,CoordModeOrigin);
      if((Xw_backing_store != Always))
         XFillPolygon(display,back_pix,gc_beztxt,Xwork.p,n,
                                          Complex,CoordModeOrigin);

      XDrawLines(display,picture,gc_beztxt,Xwork.p,n,CoordModeOrigin);
      if((Xw_backing_store != Always))
         XDrawLines(display,back_pix,gc_beztxt,Xwork.p,n,CoordModeOrigin);

     }
   else
     {
      XDrawLines(display,picture,gc_hertxt,Xwork.p,n,CoordModeOrigin);
      if((Xw_backing_store != Always))
         XDrawLines(display,back_pix,gc_hertxt,Xwork.p,n,CoordModeOrigin);
     }
   return;
}

/****************************************************** Xw_getfd ************
 *     Xw_getfd():   Function which provides the font details for the text
 *                   utilities
 *
 *                  IN: Pointer to a Textitem structure.
 *                  OUT: None.
 *                  RETURN VALUE: None.
 *
 *                  Author: NMH
 *
 ****************************************************************************/

#ifdef PROTO
void Xw_getfd ( Textitem *thisitem )
#else
void Xw_getfd ( thisitem )
Textitem  *thisitem ;
#endif
{
   char *func="Xw_getfd";
   int maxwidth, maxdescent, maxascent, top, i;
   unsigned long capline;
   int txt_indx;
   float normal;
   XFontStruct *xfp;

   txt_indx = (thisitem->text_font > XNFONT) ? 0 : thisitem->text_font;
   if(Xw_font.index != txt_indx)
   {
      if(Xw_font.fstr !=NULL)
        XFreeFont(display,Xw_font.fstr);
      Xw_font.fstr = XLoadQueryFont(display,Xfnt_serv[txt_indx]);
      Xw_font.index = txt_indx;
   }
   else
   {
     if (Xw_font.fstr == NULL)
        Xw_font.fstr = XLoadQueryFont(display,Xfnt_serv[txt_indx]);
   }

   xfp=Xw_font.fstr;

/*
   get info before normalization
*/
   maxwidth= xfp->max_bounds.width;
   maxdescent = xfp->max_bounds.descent;
   maxascent = xfp->max_bounds.ascent;
   if(!XGetFontProperty(xfp,XA_CAP_HEIGHT,&capline))
     capline=maxascent;
   top = maxascent - (int)capline;

/*
   X font info is in pixels which needs to be normalized to CGM where base = 0
   and cap line = 1 * character height. Therefore divide by capline.
*/
   normal = (float ) capline;

/*
   store the values in textitem
*/
   thisitem->rmaxwd = (float)xfp->max_bounds.width/normal;
   thisitem->rbot = (float)xfp->max_bounds.descent/normal;
   thisitem->rtop = (float)top/normal;

/*
   Get the width and bounding box information for the first 95 characters,
   normalize and store in the textitem structure.
*/
   for (i=0;i<95;i++)
      thisitem->rwd[i] = (float)xfp->per_char[i].width/normal;

   return;
}


/********************************************************** Xw_setgran *****
 *     Xw_setgran():   Set the screen granularity fot the text utilities
 *
 ****************************************************************************/
#ifdef PROTO
static void Xw_setgran( void )
#else
static void Xw_setgran()
#endif
{
   XWindowAttributes Xwin_attrib;

  XGetWindowAttributes(display,Xw_curwin->pi.win,&Xwin_attrib);
  if(cur.vdc_type == REAL)
  {
     Xwtxtatt.shared->xgrain=(float)Xwin_attrib.width/
         (cur.vdc_extent.b.x.real-cur.vdc_extent.a.x.real)*Xw_curwin->zoom.xscale;
     Xwtxtatt.shared->ygrain=(float)Xwin_attrib.height/
         (cur.vdc_extent.b.y.real-cur.vdc_extent.a.y.real)*Xw_curwin->zoom.yscale;
  }
  else
  {
     Xwtxtatt.shared->xgrain=Xwin_attrib.width/
         (cur.vdc_extent.b.x.intr-cur.vdc_extent.a.x.intr)*Xw_curwin->zoom.xscale;
     Xwtxtatt.shared->ygrain=Xwin_attrib.height/
         (cur.vdc_extent.b.y.intr-cur.vdc_extent.a.y.intr)*Xw_curwin->zoom.yscale;
  }
  return;
}

/********************************************************** Xw_hdwtxt ****
*   draw hardware text characters in poistions given by text utilities
*
* Input Arguments:
*     list : Pointer to the head of the link list of text items
*     txtx : x coord for origin of text element
*     txty : y coord for origin of text element
* Output Arguments:
*     None.
* Return value:
*     None.
*
***************************************************************************/
#ifdef PROTO
static void Xw_hdwtxt( Textitem *txt, Point txtp)
#else
static void Xw_hdwtxt( txt, txtp )
Textitem *txt;
Point txtp;
#endif
{
   char *cp, ch[2];
   int i,gen_indx, part_indx, xp, yp;
   Tmatrix mat;
   float A, B, C, D, E, F;

   switch(txt->text_prec)
   {
       case STRING:   /*STRING: */
           xp=Xtranx(txt->origin[0].x);
           yp=Xtrany(txt->origin[0].y);
#ifdef EBCDIC
           for (cp=txt->str; *cp!='0'; cp++)
               *cp = cgmebcdic[(unsigned int) *cp];
#endif
           XDrawString(display,picture,gc_text,xp,yp,txt->str,strlen(txt->str));
           if((Xw_backing_store != Always))
             XDrawString(display,back_pix,gc_text,xp,yp,txt->str,
                                                strlen(txt->str));
           break;
       case CHAR:     /* CHAR:   */
       case STROKE:   /* STROKE: */
          TXTxform(txtp, mat);
/* for explanation of the following transformation see Xw_stktxt */
          A=Xw_curwin->tot_xscale*mat[0][0];
          B= -Xw_curwin->tot_yscale*mat[0][1];
          C=Xw_curwin->tot_xscale*mat[1][0];
          D= -Xw_curwin->tot_yscale*mat[1][1];
          E=Xw_curwin->tot_xscale*mat[2][0]-Xw_curwin->tot_xdisp;
          F= -Xw_curwin->tot_yscale*mat[2][1]+Xw_curwin->tot_ydisp;

/* loop through the string and draw each character */
          for(cp=txt->str, i=0;*cp !='\0'; cp++, i++)
          {
             xp=txt->origin[i].x*A + txt->origin[i].y*C + E;
             yp=txt->origin[i].x*B + txt->origin[i].y*D + F;
#ifdef EBCDIC
             ch[0]= cgmebcdic[(unsigned int) *cp];
#else
             ch[0]= *cp;
#endif
             ch[1]= '\0';
             XDrawString(display,picture,gc_text,xp,yp,ch,1);
             if((Xw_backing_store != Always))
                XDrawString(display,back_pix,gc_text,xp,yp,ch,1);
          }
          break;
   }
   return;
}

/********************************************************** Xw_fillpix ****
*   draw hardware text characters in poistions given by text utilities
*
* Input Arguments:
*     back_pix : id of pixmap to fill
*     colour   : pixel value to use for fill
* Output Arguments:
*     None.
* Return value:
*     None.
*
***************************************************************************/
#ifdef PROTO
static void Xw_fillpix ( Pixmap backing, unsigned long colour)
#else
static void Xw_fillpix ( backing,  colour)
Pixmap backing;
unsigned long colour;
#endif

{
 GC fillgc;
 XGCValues gc_vals;
 unsigned long gc_mask, allgc;
 Window rwin;
 int xret, yret;
 unsigned int width, height, borderw, depth;

 fillgc  = XCreateGC(display,back_pix,0,&gc_values);
 allgc = ( 1L<<(GCLastBit+1) ) -1;
 XCopyGC(display,gc_dflt,allgc,fillgc);
 gc_vals.fill_style = FillSolid;
 gc_mask = GCFillStyle;
 XChangeGC(display,fillgc,gc_mask,&gc_vals);
 XSetForeground(display, fillgc, colour);

 XGetGeometry(display,backing,&rwin,&xret,&yret,&width,&height,&borderw,&depth);

 XFillRectangle (display,backing,fillgc,0,0,width,height);

 XFreeGC(display,fillgc);
 return;
}
