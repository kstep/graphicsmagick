/*  RAL-CGM Interpreter header file: @(#) cgmxw.h  version 3.2
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the authors.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the authors should be marked as
 * such.
 *
 * R T Platon @ Rutherford Appleton Laboratory (rtp@uk.ac.rl.ib)
 *
 * Description: header file for X-Windows driver
 *
 * Modification log:
 *
 *  13 Sep 91 RTP  Add SCCS id
 */

/* built-in defaults   */

/* order of prefered text methods */
static Enum Xprefmeth[] = {HDW,
#ifdef HERSHEY
                           HER,
#endif
#ifdef BEZIER
                           BEZ,
#endif
                          (Enum)0};

#ifndef XWMENUFONT
#define XWMENUFONT      "*TIMES*NORMAL*12*"
#endif

#define XWCURSOR        XC_top_left_arrow


/*  general definitions                     */

#define  POLYCLOSE      2

#define  MAXCOL         65535

#define  SCREEN         0x01
#define  WORLD          0x02
#define  CLIPAREA       0x04
#define  VDCCHNG        0x08
#define  SETDFT         0x10


#define  HANORMAL       0
#define  HALEFT         1
#define  HACENTRE       2
#define  HARIGHT        3
#define  HACONT         4
#define  VANORMAL       0
#define  VATOP          1
#define  VACAP          2
#define  VAHALF         3
#define  VABASE         4
#define  VABOTTOM       5
#define  VACONT         6

#define  NOWAIT         0
#define  WAIT           1
#define  NOERROR        2

#define  ASIS           2
#define  BACKGROUND     1
#define  FOREGROUND     0

#define  XLINE          0
#define  XEDGE          1
#define  XTEXT          2
#define  XFILL          3
#define  XMARK          4

#define  XFNULL         0
#define  XFPLUS         1
#define  XFMINUS        2
#define  XFOTHER        3

#define  TERMLEN      100
#define  XMESGLEN     100


static Display *display;
static Window picture, root, Xw_groupwin;
static Pixmap back_pix;
static GC  gc_dflt, gc_line, gc_fill, gc_text, gc_edge;
static GC  gc_mark, gc_bound, gc_tile, gc_cell, gc_men;
static GC  gc_beztxt, gc_hertxt;
static XWindowAttributes attr;
static XGCValues gc_values;
static Colormap Xclmap, Xdfclmap;
static int screen, depth;
static Screen *scrn;
static Cursor  Xw_cursor;
static int  Xw_backing_store;
static int Xw_upmenu=FALSE;
static int Xw_waiting=TRUE, Xw_winwait=FALSE, Xw_error=FALSE, Xw_msgwait=FALSE;
static int Xw_firstmf=TRUE, Xw_upkb=FALSE, Xw_defpts=FALSE, Xfirst_pic,
           Xoldasf[18];
static int Xw_metric=TRUE;

/*  window parameters                          */

#define   XNBUTN       4
#define   XNBUTCHAR    2
#define   XNMENUITEM   7
#define   XNMENCHAR    7
#define   XKBMLEN      5
#define   XNKEYS      12
#define   XNKEYCHAR    1

struct Xzoom_rec
       { short x, y, x1, y1, x2, y2;
         float xscale, yscale;
       };
struct Xw_wij
       { Window win;
         char title[XNMENCHAR+1];
         Bool written;
         Pixmap pix;
         Bool pixback;
       };
struct Xwin_rec
       { struct Xwin_rec *next;
         int sc_x, sc_y;
         unsigned int sc_w, sc_h;
         int new_coords, newx, newy;
         unsigned int neww, newh;
         unsigned int topw,toph;
         float sc_rat;
         int offset_x, offset_y;
         int signx, signy;
         float wc_top, wc_bot, wc_left, wc_right;
         float wc_rat;
         float scale, dft_size;
         float tot_xscale, tot_xdisp, tot_yscale, tot_ydisp;
         float tot_xyscale;
         float pix_per_mm;
         int wc_or, first;
         struct Xzoom_rec zoom;
         struct Xw_wij top,  pi, b[XNBUTN+1], tx;
         char mesgbuff[XMESGLEN];
       };
static struct Xwin_rec *Xw_curwin=NULL, *Xw_zmwin, *Xw_winlist=NULL;
static Window Xw_menu_parent, Xw_kb_parent;
static int Xw_numwin=0;
static int Xw_zmtype=0;
static struct Xmen_rec
       { struct Xw_wij shell, m[XNMENUITEM+1];
         int w, h;
       } Xmenu;
static struct Xkb_rec
       { struct Xw_wij shell, tx;
         struct Xw_wij m[XNKEYS+1];
         int frame, w, h;
       } Xkb;

static struct Xwinparm
       { int wpack,hpack;
         int keyw,butw,menw,buth;
       } Xw_winparm;

/* coordinate translation macros                                 */

#define Xtranx(X)  ((X)*Xw_curwin->tot_xscale - Xw_curwin->tot_xdisp)
#define Xtrany(Y)  (-(Y)*Xw_curwin->tot_yscale + Xw_curwin->tot_ydisp)

/*  work area                                  */

union { long i[ARRAY_MAX];
        short s[ARRAY_MAX];
        float f[ARRAY_MAX];
        XPoint p[ARRAY_MAX];
        char c[ARRAY_MAX];
      } Xwork;
int amax_i, amax_s, amax_f,  amax_p, amax_c;

/*     terminal to be used     */

#define  TERMLEN      100
#define  XMESGLEN     100

char terminal[TERMLEN]="";


/*  clip parameters                            */

struct Xc {float left, right, top, bot;
           XRectangle area;
           int clip_ind;
          } ;

/*   colour tables                          */

struct Xcolrused {struct Xcolrused *next;
                  XColor ucol; };

#define  XDBCOLOR       0
#define  XDFCOLOR       1
#define  rgbtran(x,y)   Xcolourscale.y*(x-cur.min_rgb.y)

static int XNCOLOR=256, XDEFNCOLOR;
static int  monochrome, threepixes, can_set_colour;
static unsigned long Xw_black, Xw_white;
static struct   {float red,green,blue;} Xcolourscale;


static XColor  Xauxcolr, Xbackcolr, Xforecolr, Xdefbackcolr, Xdefforecolr;
static XColor  *Xcolrtab = NULL, *Xdefcolrtab;
static struct Xcolrused *Xalloccolr = NULL;

/*  line drawing parameters                    */

#define  XNLBUN         5
#define  XDLBUN         1
#define  XNDASH         5
#define  XDDASH         1
#define  XLMINW         1
#define  XLDFTW         1

struct Xlbun { float width;
               int type;
               int col;
             };
struct Xdlist { int type;
                char dash[10];
                int num;
              };
struct Xl { struct Xlbun *bundle;
          } ;

/*  edge drawing parameters                    */

#define  XNEBUN         5
#define  XDEBUN         1
#define  XEMINW         1
#define  XEDFTW         1

struct Xebun { float width;
               int type;
               int col;
             } ;
struct Xe { struct Xebun *bundle;
          } ;

/*  text drawing parameters                    */

#define  XNTBUN         3
#define  XDTBUN         1
#define  XDFONT         0
#define  XDEFFNT        1
#define  XMAXPART      15
#define  XFNTNMLEN     90


struct Xtbun { int font;
               float space;
               int col;
             } ;
struct Xtch { char ch;
              int dx,dy;
            } ;
struct Xts { struct Xts *next;
             int nchar;
             int gen_indx, part_indx;
             struct Xtch chstr[1];
           };
struct Xfnt { int gen_indx, part_indx;
            };
struct Xfnt_part { char name[XFNTNMLEN];
                   int width,height;
                   int lb,rb,as,de;
                 };
struct Xfnt_gen  { char name[XFNTNMLEN];
                   int npart, cur_part, init;
                   XFontStruct *fstr;
                   struct Xfnt_part part[XMAXPART];
                 };
struct Xt { struct Xtbun *bundle;
            int *fonts;
            float cur_space;
            int rstr_flag, rstr_w, rstr_h;
            int pos_x, pos_y, extx, exty;
            int gen_indx, part_indx;
            float delx,dely,signx,signy;
            int ha, va;
            int fi;
            struct Xts *sptr, *firstp, *lastp;
          } ;

/* text attribute structure for use with text utilities */
static struct textatt Xwtxtatt;
static struct sharedatt share;

/* fontlist from the X server */
static char **Xfnt_serv;
static Bool FNTXlist_called;
static int XNFONT;
struct X_curfont
         { int index;          /* current font index into Xfnt_serv list */
           XFontStruct *fstr;  /* font details of current font */
         };

static struct X_curfont Xw_font;

/*  fill area parameters                    */

#define  XNFBUN         7
#define  XDFBUN         1
#define  XNTILE        20
#define  XSTILE         9
#define  XDTILE         1
#define  XNSTIP         6
#define  XDSTIP         1
#define  XMINTILE      10
#define  XMAXTILE      80

struct Xfbun { int int_style;
               int hatch;
               int pattern;
               int col;
             } ;
struct Xftile { int nx;
                int ny;
                struct colour *col;
                Pixmap pm;
              } ;
struct Xf { struct Xfbun *bundle;
            int cur_style;
            int arcmode;
            Pixmap stipples[XNSTIP+1];
            struct Xftile tiles[XNTILE+1];
            struct Xftile deftiles[XNTILE+1];
          } ;

/*  marker parameters                       */

#define  XNMBUN         5
#define  XDMBUN         1
#define  XNMARK         5
#define  XDMARK         3
#define  XPOINTS        0
#define  XCIRCLE        1
#define  XLINES         2
#define  XMMINS         3
#define  XMDFTS         5

struct Xmbun { int type;
               float size;
               int col;
             } ;
struct Xml { int type;
             int npts;
             float dx[8], dy[8];
             XPoint p[8];
           } ;
struct Xm { struct Xmbun *bundle;
            int cur_size;
            int cur_type;
            struct Xml *list;
          } ;

/*   cell array                             */

static struct {int  nx,ny;
        double px,py,qx,qy,rx,ry;
        double delx1,dely1,delx2,dely2; } Xcell;
/*   change flags                           */

static struct { Logical line, mark, text, fill, edge;
         Logical def_line_width, def_mark_size, def_edge_width, def_char_height;
         Logical line_width, mark_size, edge_width, char_height; }Xflag ;

/*  default values                          */

static struct Xfnt_gen *Xfnt_supp;

static struct Xlbun Xdft_lbun[XNLBUN+1] = { { 1.0, 1, 1 },
                                     { 1.0, 1, 1 },
                                     { 1.0, 2, 2 },
                                     { 1.0, 3, 3 },
                                     { 1.0, 4, 4 },
                                     { 1.0, 5, 5 }
                                   };
static struct Xdlist Xdash_list[XNDASH+1] =
                  { {LineSolid,      "\004\004",                 2 },
                    {LineSolid,      "\004\004",                 2 },
                    {LineDoubleDash, "\014\006",                 2 },
                    {LineDoubleDash, "\004\006",                 2 },
                    {LineDoubleDash, "\014\004\006\004",         4 },
                    {LineDoubleDash, "\014\006\004\006\004\006", 6 }
                  };

static struct Xebun Xdft_ebun[XNEBUN+1] = { { 1.0, 1, 1 },
                                     { 1.0, 1, 1 },
                                     { 1.0, 2, 2 },
                                     { 1.0, 3, 3 },
                                     { 1.0, 4, 4 },
                                     { 1.0, 5, 5 }
                                   };

/* if these defaults are changed, change XWtext_bundle also */
static struct Xtbun Xdft_tbun[XNTBUN+1] = { { 1, 0.0, 1 },
                                     { 1, 0.0, 1 },
                                     { 2, 0.0, 2 },
                                     { 3, 0.0, 3 }
                                   };

/* text bundle for use with text utilities */
static struct textbundle XWtext_bundle[XNTBUN+1]={
                {1, CHAR, 1.0, 0.0, 1, 1.0, 1.0, 1.0},
                {1, CHAR, 1.0, 0.0, 1, 1.0, 1.0, 1.0},
                {2, CHAR, 1.0, 0.0, 2, 1.0, 1.0, 1.0},
                {3, CHAR, 1.0, 0.0, 3, 1.0, 1.0, 1.0},
                                     };

static struct Xfbun Xdft_fbun[XNFBUN+1] = { { HATCH,   1, 1, 1 },
                                     { HATCH,   1, 1, 1 },
                                     { HATCH,   2, 1, 2 },
                                     { HATCH,   3, 1, 3 },
                                     { HATCH,   4, 1, 4 },
                                     { HATCH,   5, 1, 5 },
                                     { HATCH,   6, 1, 6 },
                                     { PATTERN, 1, 1, 7 }
                                   };

#define  STIPSIZE      18
static int Xdft_stip[] = {
   16,16,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   16,16,
   0x00000000,0x00000000,0x11111111,0x11111111,
   0x00000000,0x00000000,0x00000000,0x00000000,
   0x00000000,0x00000000,0x11111111,0x11111111,
   0x00000000,0x00000000,0x00000000,0x00000000,
   0x00000000,0x00000000,0x11111111,0x11111111,
   0x00000000,0x00000000,0x00000000,0x00000000,
   0x00000000,0x00000000,0x11111111,0x11111111,
   0x00000000,0x00000000,0x00000000,0x00000000,
   16,16,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x00100010,0x00100010,
   15,15,
   0x00100001,0x00001000,0x01000010,0x00010000,
   0x10000100,0x00100001,0x00001000,0x01000010,
   0x00010000,0x10000100,0x00100001,0x00001000,
   0x01000010,0x00010000,0x10000100,0x00100001,
   0x00001000,0x01000010,0x00010000,0x10000100,
   0x00100001,0x00001000,0x01000010,0x00010000,
   0x10000100,0x00100001,0x00001000,0x01000010,
   0x00010000,0x10000100,
   15,15,
   0x00100001,0x00001000,0x00010000,0x10000100,
   0x00001000,0x01000010,0x10000100,0x00100001,
   0x01000010,0x00010000,0x00100001,0x00001000,
   0x00010000,0x10000100,0x00001000,0x01000010,
   0x10000100,0x00100001,0x01000010,0x00010000,
   0x00100001,0x00001000,0x00010000,0x10000100,
   0x00001000,0x01000010,0x10000100,0x00100001,
   0x01000010,0x00010000,
   16,16,
   0x00100010,0x00100010,0x11111111,0x11111111,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x11111111,0x11111111,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x11111111,0x11111111,
   0x00100010,0x00100010,0x00100010,0x00100010,
   0x00100010,0x00100010,0x11111111,0x11111111,
   0x00100010,0x00100010,0x00100010,0x00100010,
   18,18,
   0x10001010,0x00101000,0x10000000,0x00000100,0x00010000,0x01000000,
   0x10001010,0x00101000,0x10000000,0x01010001,0x01000101,0x00000000,
   0x00100000,0x10000010,0x00000000,0x01010001,0x01000101,0x00000000,
   0x10001010,0x00101000,0x10000000,0x00000100,0x00010000,0x01000000,
   0x10001010,0x00101000,0x10000000,0x01010001,0x01000101,0x00000000,
   0x00100000,0x10000010,0x00000000,0x01010001,0x01000101,0x00000000,
   0x10001010,0x00101000,0x10000000,0x00000100,0x00010000,0x01000000,
   0x10001010,0x00101000,0x10000000,0x01010001,0x01000101,0x00000000,
   0x00100000,0x10000010,0x00000000,0x01010001,0x01000101,0x00000000,
                  };

#define  TILESIZE     64
static unsigned char Xdft_tile[] = { 0,0,1,1,0,0,
/*  Pattern 1  */
    255,1,16,16,0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
 /*  Pattern 2  */
    255,2,16,16,0,
    1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* Pattern 3 */
    255,3,16,16,0,
    1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* Pattern 4 */
    255,4,16,16,0,
    1, 7, 7, 7, 7, 7, 7, 7, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 7, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 7, 1, 7, 7, 7, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 7, 1, 7, 1, 7, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 7, 1, 7, 7, 7, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 7, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 7, 7, 7, 7, 7, 7, 7, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 7, 7, 7, 7, 7, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 1, 7, 7, 7, 1, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 1, 7, 1, 7, 1, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 1, 7, 7, 7, 1, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 7, 7, 7, 7, 7, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* Pattern 5 */
    255,5,16,16,0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 4,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 4, 1, 4, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 4, 1, 4, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 4,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1 ,4, 1, 4, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 4, 1, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 4, 1, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 4, 1, 4, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*  Pattern 6  */
    255,6,16,16,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0,
    0, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 0,
    0, 7, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 0,
    0, 7, 6, 5, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 0,
    0, 7, 6, 5, 4, 3, 3, 3, 3, 3, 3, 4, 5, 6, 7, 0,
    0, 7, 6, 5, 4, 3, 2, 2, 2, 2, 3, 4, 5, 6, 7, 0,
    0, 7, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 0,
    0, 7, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 0,
    0, 7, 6, 5, 4, 3, 2, 2, 2, 2, 3, 4, 5, 6, 7, 0,
    0, 7, 6, 5, 4, 3, 3, 3, 3, 3, 3, 4, 5, 6, 7, 0,
    0, 7, 6, 5, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 0,
    0, 7, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 0,
    0, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 0,
    0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*  Pattern 7  */
    255,7,16,16,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 0, 0, 0, 0, 5, 5, 3, 5, 5, 5, 0, 0, 0, 0, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*  Pattern 8 */
    255,8,16,16,0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0,
    0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0,
    0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*  Pattern 9  */
    255,9,16,16,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
    0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
    0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0,
    0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0,
    0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0,
    0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0,
    0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0,
    0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0,
    0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

static struct Xmbun Xdft_mbun[XNMBUN+1] = { {1, 1.0, 1 },
                                     {1, 1.0, 1 },
                                     {2, 1.0, 2 },
                                     {3, 1.0, 3 },
                                     {4, 1.0, 4 },
                                     {5, 1.0, 5 }
                                   };
static struct Xml  Xdft_ml[XNMARK+1] =
        { { XPOINTS, 0,
            { 0.0 },
            { 0.0 },
            { {0,0}, {0,0} },
          },
          { XPOINTS, 0,
            { 0.0 },
            { 0.0 },
            { {0,0}, {0,0} },
          },
          { XLINES , 4,
            { 0.0, 0.0, -1.0, 1.0 },
            { 1.0, -1.0, 0.0, 0.0 },
            { {0,0}, {0,0} },
          },
          { XLINES , 8,
            { 0.0, 0.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 },
            { 1.0, -1.0, 0.0, 0.0, 1.0, -1.0, -1.0, 1.0 },
            { {0,0}, {0,0} },
          },
          { XCIRCLE, 0,
            { 0.0 },
            { 0.0 },
            { {0,0}, {0,0} },
          },
          { XLINES , 4,
            { 1.0, -1.0, -1.0, 1.0 },
            { 1.0, -1.0, 1.0, -1.0 },
            { {0,0}, {0,0} },
          }
        };

static struct Xl Xline = { Xdft_lbun };

static struct Xe Xedge = { Xdft_ebun };

static struct Xt Xtext = { Xdft_tbun, NULL, 0.0,
                    0, 0, 0,
                    0, 0, 0, 0,
                    0, 0,
                    0.0, 0.0, 0.0, 0.0,
                    0, 0, 0,
                    NULL, NULL, NULL };

static struct Xf Xfill = { Xdft_fbun, FillSolid, ArcPieSlice};

static struct Xm Xmark = { Xdft_mbun, 1, XDMARK, Xdft_ml };

static struct Xc Xclip = { 0.0, 1.0, 1.0, 0.0,
                    {0, 0, 0, 0}, OFF};

/*    text alignment tables   */

static int Xhatab[4] = {HALEFT, HARIGHT, HACENTRE, HACENTRE };
static int Xvatab[4] = {VABASE, VABASE , VABASE  , VATOP    };

static unsigned char Xdfcolr[768] = {
/*   GKS  default colours */

    0,   0,   0,   /* Background */
  255, 255, 255,   /* Foreground */
  255,   0,   0,   /* Red */
    0, 255,   0,   /* Green */
    0,   0, 255,   /* Blue */
  255, 255,   0,   /* Yellow */
    0, 255, 255,   /* Cyan */
  255,   0, 255,   /* Magenta */

 /*    8 Grey scales ( White to Black )  */

    255, 255, 255,  177, 177, 177,  158, 158, 158,  128, 128, 128,
    100, 100, 100,   78,  78,  78,   64,  64,  64,    0,   0,   0,

 /*    GKS colour maps from Versatec  16 - 255 */

   98, 192, 255,  139, 122, 255,    92,  70, 255,    51,   0, 243,
   28,   0, 133,   20,   0,  93,    14,   0,  65,    11,   0,  56,
    9,   0,  46,  128, 138, 255,    85,  85, 255,     0,   0, 255,
    0,   0, 168,    0,  13, 116,     0,   8,  76,     0,   8,  68,

  122, 139, 255,   80,  93, 255,     0,  59, 252,     0,  39, 168,
    0,  28, 122,    0,  18,  80,     0,  16,  70,   122, 149, 255,
   70, 108, 255,    0,  83, 230,     0,  61, 175,     0,  44, 128,
    0,  29,  85,    0,  26,  74,   116, 154, 255,    59, 118, 255,

    0,  97, 219,    0,  77, 182,     0,  56, 133,     0,  37,  89,
    0,  33,  79,  110, 159, 255,    43, 131, 255,     0, 112, 225,
    0,  92, 186,    0,  68, 138,     0,  46,  93,     0,  40,  80,
  177, 213, 255,  104, 173, 255,     0, 151, 250,     0, 136, 225,

    0, 120, 198,    0,  89, 147,     0,  59,  97,     0,  52,  86,
    0,  43,  70,    0, 186, 186,     0, 160, 147,     0, 142, 129,
    0, 121, 106,    0,  89,  78,     0,  62,  55,     0,  53,  46,
    0, 192, 160,    0, 160, 123,     0, 138, 105,     0, 119,  91,

    0,  89,  68,    0,  59,  44,     0,  53,  40,     0, 243, 169,
    0, 192, 134,    0, 160, 113,     0, 138,  97,     0, 118,  83,
    0,  89,  63,    0,  59,  41,     0,  51,  36,     0,  43,  30,
    0, 216, 115,    0, 175, 113,     0, 151, 102,     0, 138,  91,

    0, 104,  68,    0,  70,  46,     0,  61,  40,     0, 230,  74,
    0, 189, 110,    0, 171, 100,     0, 156,  92,     0, 116,  69,
    0,  80,  47,    0,  56,  33,   105, 238,   0,     0, 216,  46,
    0, 202,  65,    0, 189,  71,     0, 142,  53,     0,  97,  36,

    0,  83,  31,  161, 240,   0,    99, 225,   0,   120, 219,   0,
   92, 208,   0,   69, 156,   0,    46, 104,   0,    41,  93,   0,
  197, 243,   0,  172, 232,   0,   168, 227,   0,   160, 219,   0,
  122, 168,   0,   80, 110,   0,    70,  97,   0,   255, 251, 175,

  255, 250, 110,  255, 249,  70,   254, 248,   0,   253, 245,   0,
  189, 183,   0,  128, 124,   0,   113, 110,   0,    89,  86,   0,
  255, 209, 116,  255, 194,  97,   255, 191,  85,   255, 177,  65,
  195, 136,   0,  133,  92,   0,   116,  81,   0,   255, 190, 119,

  255, 163, 104,  255, 168,  89,   255, 160,  80,   195, 113,   0,
  133,  77,   0,  116,  67,   0,   255, 173, 122,   255, 154, 107,
  255, 142,  89,  255, 112,   0,   195,  86,   0,   128,  56,   0,
  113,  50,   0,  255, 162, 122,   255, 125, 104,   255, 100,  65,

  247,  80,   0,  186,  60,   0,   122,  39,   0,   110,  35,   0,
  255, 147, 124,  255,  98,  89,   255,  56,  43,   238,  44,   0,
  182,  34,   0,  122,  22,   0,   104,  19,   0,   255, 168, 170,
  255, 122, 128,  255,  70,  80,   250,   0,  38,   225,   0,  35,

  168,   0,  26,  110,   0,  17,   100,   0,  15,    80,   0,  12,
  255, 122, 147,  255,  70, 108,   247,   0,  83,   225,   0,  72,
  168,   0,  54,  110,   0,  35,    97,   0,  31,   255, 166, 197,
  255, 110, 170,  255,  43, 150,   232,   0, 134,   211,   0, 122,

  160,   0,  92,  104,   0,  60,    93,   0,  54,    76,   0,  44,
  255,  80, 204,  227,   0, 189,   199,   0, 165,   168,   0, 142,
  122,   0, 104,   85,   0,  72,    74,   0,  63,   255,  70, 225,
  205,   0, 213,  175,   0, 175,   141,   0, 147,   106,   0, 110,

   71,   0,  74,   62,   0,  65,   245,  43, 255,   193,   0, 227,
  153,   0, 195,  107,   0, 147,    80,   0, 110,    55,   0,  76,
   47,   0,  65,  216,  89, 255,   150,  43, 255,   118,   0, 216,
   80,   0, 147,   59,   0, 107,    38,   0,  70,    35,   0,  64,

  187, 110, 255,  122,  59, 255,    86,   0, 230,    55,   0, 147,
   38,   0, 100,   25,   0,  68,    22,   0,  60,   177, 177, 177,
  158, 158, 158,  128, 128, 128,   100, 100, 100,    78,  78,  78,
    0,   0,   0,   61,  50,  59,    52,  45,  62,    36,  61,  59,

  127, 118, 109,   91,  70,  72,    71,  57,  64,    79,  62,  64,
   28,  33,  32,   34,  31,  36,    31,   0,  22,   252, 182, 142,
  223, 145, 135,  191, 133, 119,   155, 100,  94,   137,  94,  90,
   64,  51,  53,   48,  41,  42,   255, 148, 116,   151,  96,  91
};

/* pointer to colour table for use with ATTtext routine */
static Colourentry *XWcolour_table=NULL;
