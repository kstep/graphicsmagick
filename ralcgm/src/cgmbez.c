/*   RAL-CGM Interpreter module:  @(#) cgmbez.c  version 3.1
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
 * Description:   BEZIER text system
 *
 *  Provides comprehensive facilities for output of text using
 *  Bezier curve definitions of the characters.
 *
 * Author:  Chris D Seelig
 *
 * Maintenance log:
 *
 *   3 Dec 90 RTP Add ANSI Prototyping
 *   7 Dec 90 RTP Use Point types where appropriate
 *  14 Dec 90 RTP Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *  25 Apr 90 CDS Changed frees to #defed FREE
 *   1 May 91 CDS Added newcharacter flag to line drawing routines, this
 *                doesnt work at the mo (see comments in code) but keeps
 *                Bezier in line with Hershey and does stop drivers from
 *                updating matrices more often than neccassary.
 *   1 May 91 CDS Optimized BezArc a bit, and got rid of BezBlend.
 *  16 May 91 CDS Replaced BezExp with BezDraw, which moves functionality
 *                out of BEZtext in readyness for combined text utilities!
 *  16 May 91 CDS Removed character reference adjustment to genbez.
 *  25 May 91 CDS Corrected bug in BezLoad which caused wrong fonts to
 *                be used in majority of cases!!.
 *  21 Jun 91 CDS Added code to expand bin file name, which is no longer
 *                in the expanded form in directory file.
 *  11 Sep 91 CDS Changed to use new header file CGMUBEZ.H
 *                Changed some Bezier specific error messages to be
 *                more general
 *  02 Oct 91 CDS Added checks to all memory allocations!
 *  03 Oct 91 CDS Corrected possible bug in BezOpen
 *  16 Nov 92 CDS Changed ifdef ANSI to ifdef PROTO in function prototypes
 */

#include "cgmtext.h"
#define CGMBEZ_C
#include "cgmubez.h"
#include "cgmerr.h"
#include "cgmfile.h"

static char *func = "CGMbez";

     /*  BezOpen:
      |  ========
      |
      |  Open and intialise the Bezier emulation system.
      |
     */

#ifdef PROTO
static void BezOpen( void )
#else
static void BezOpen()
#endif

{
    int i, n, ibc, nread;
    char *dirname;
    FILE *dirfile;

    /***** If system is already open then return straight away */
    if (B_state == BezOPEN) return;

    /*****  Open the font directory  */
    dirname = CGMpath("fontdir",bezext);
    dirfile = CGMfopen(dirname,F_READ,BINARY);
    if (dirfile == NULL)
    {   /* Exit on the spot if open failed */
        (void) CGMerror(func, ERR_OPENFILE, FATAL, dirname);
        FREE(dirname);
        return;
    }

    /*****  Read in the directory, so there is an in-core list of all
            available fonts.                                           */
    B_dir=(BezDir *)calloc(MaxFonts+1,sizeof(BezDir));
    if (B_dir == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    nread = fread( B_dir, sizeof(BezDir), MaxFonts+1, dirfile);
    if (ferror(dirfile))
    {
        (void) CGMerror(func, ERR_FILCOR, ERROR, dirname);
        FREE(dirname); FREE(B_dir);
        return;
    }

#ifdef DEBUG
    {
        BezDir *i;
        for(i=B_dir;i->number!=(Index)-1;i++)
        fprintf(stderr,"Dir entry\nCode %ld\n Fontname %s\nFontfile %s\n\n",
                i->number,i->name,i->filename);
    }
#endif

    /* Realloc, note there is no check on this as it always reduces the
       amount of memory being used                                     */
    B_dir=(BezDir *)realloc(B_dir, nread*sizeof(BezDir));
    if ( nread == 0)
    {   /* No Bezier fonts, give up here */
        (void) CGMerror(func,ERR_NOFONTS,ERROR,dirname);
        FREE(B_dir);
        fclose(dirfile);
        FREE(dirname);
        return;
    }
    fclose(dirfile);
    FREE(dirname);

    /*  Calculate the binomial coefficients used by the algorithm  */
    for (i=0;i<MaxCoef;i++) for (n=0;n<MaxCoef;n++) B_coef[n][i] = 0;

    B_coef[0][0] = 1;

    for (n=1;n<MaxCoef;n++)
        for (i=0;i<n+1;i++)
        {
            ibc = B_coef[n-1][i];
            B_coef[n][i]   = B_coef[n][i]   + ibc;
            B_coef[n][i+1] = B_coef[n][i+1] + ibc;
        }

    /*****  Mark the system as ready to go  */
    B_state = BezOPEN;
}

     /*  BEZtext:
      |  ========
      |
      |  Draw a given text string using the Bezier emulation system.
      |
      */

#ifdef PROTO
void BEZtext( Textitem *txtit, struct textatt *txtatt, Point txtp,
              void (*linedraw)(int, Point *, Tmatrix, int) )
#else
void BEZtext(txtit, txtatt, txtp, linedraw)

Textitem *txtit;
struct textatt *txtatt;
Point txtp;
void (*linedraw)();
#endif

{
    int ichar,              /*  ascii number of character  */
        stat;               /*  return status from function call  */

    Point *pt;              /*  pointers into array of centres from txtitem  */

    float xscale,
          yscale;           /*  scaling factors for matrix transformation  */

    Tmatrix tl2vmat,        /*  textline to vdc transformation matrix  */
            xmat;           /*  total transformation matrix  */

    char *c;                /*  Pointer to current character in txtit */

    /*****  Make sure that the Bezier system is initialised  */
    if (B_state!=BezOPEN)
    {
        BezOpen();
        if (B_state!=BezOPEN)
        {
            (void) CGMerror(func,ERR_DBUNAV,ERROR,NULLSTR);
            return;
        }
    }

    /*****  Make sure the right font is loaded  */
    if ((B_font==NULL) || (txtit->text_font!=B_font->number))
    {
        stat=(txtit->text_font==0)?BezLoad(DefBezFnt)
                                  :BezLoad(txtit->text_font);
        /***** unable to load requested font, use the default one */
        if (stat==-1)
        { /***** unable to load  default font, return with error
                 and close Bezier system                        */
            B_state=BezCLOSED;
            (void) CGMerror(func,ERR_DBUNAV,ERROR,NULLSTR);
            return;
        }
    }

    /*****  Process each character in turn  */
    c = txtit->str;
    pt = txtit->origin;

    /*****  get text line matrix */
    TXTxform(txtp, tl2vmat);

    /***** Scaling factor due to co-ordinate system change
           same for entire substring                       */
    yscale = (txtit->char_height) / (B_font->height);
    xscale = (txtit->char_exp) * yscale;

    /*****  Calculate what we can of the final transformation matrix  */
    xmat[0][0]=tl2vmat[0][0]*xscale;
    xmat[1][0]=tl2vmat[1][0]*yscale;
    xmat[0][1]=tl2vmat[0][1]*xscale;
    xmat[1][1]=tl2vmat[1][1]*yscale;

    for (; *c!='\0' ; c++, pt++)
    {
        ichar = (int) *c - 32;

        /*****  Finish calculation of the final transformation matrix  */
        xmat[2][0]=tl2vmat[0][0]*(pt->x)+tl2vmat[1][0]*(pt->y)+tl2vmat[2][0];
        xmat[2][1]=tl2vmat[0][1]*(pt->x)+tl2vmat[1][1]*(pt->y)+tl2vmat[2][1];

#ifdef DEBUG
        fprintf(stderr,"calling BEzDraw\n");
#endif
        /*****  Call the text drawing routines  */
        BezDraw(ichar, xmat, txtatt, linedraw);
    }
}

     /*  BezLoad:
      |  =======
      |
      |  Load a new font from disk.
      |
      */

#ifdef PROTO
static int BezLoad( Index font )
#else
static int BezLoad(font)

Index font;
#endif

{
    FILE *binfile;
    char *binname, mess[80];
    int i;
    BezDir  *dir;
    BezFlist *fontptr;

#ifdef DEBUG
    fprintf(stderr,"Trying to find font %d\n",font);
#endif

    /*****  Look for the font in the font list  *****/
    for(fontptr=B_fontlist;fontptr!=NULL;fontptr=fontptr->next)
    {
        if (font==fontptr->bfont->number)
        {   /***** EXIT POINT , if font is in list set it to current font
                   and then return                                         */
            B_font=fontptr->bfont;
            B_char=fontptr->bchar;
            B_def=fontptr->bdef;
#ifdef DEBUG
            fprintf(stderr,"Found font %d in Fontlist\n",font);
#endif
            return(0);
        }
    }

    /***** and now in the font directory  *****/
    for (dir = B_dir;(dir->number!=-1) && (dir->number!=font);dir++)
        ;

    if (dir->number==-1)
    {
        sprintf(mess,"%d",font);
        (void) CGMerror(func,ERR_FNTNOTSUP,WARNING,mess);
        return(-1);
    }
#ifdef DEBUG
    fprintf(stderr,"Found font %d in directory\n",font);
#endif

    /***** Open the font file */
    binname=CGMpath(dir->filename,binext);
#ifdef DEBUG
    fprintf(stderr,"opening file %s\n",binname);
#endif
    binfile = CGMfopen(binname, F_READ, BINARY);
    if (binfile == NULL)
    {
        (void) CGMerror(func,ERR_FNTNOTOPEN,ERROR,dir->filename);
        return(-1);
    }

    /***** Allocate memory for new entry in Fontlist */
    fontptr = (BezFlist *)calloc(1,sizeof(BezFlist));
    if (fontptr == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);

    /***** Read in the font header information - only first 5 */
    fontptr->bfont = (BezFont *)calloc(1,sizeof(BezFont));
    if (fontptr->bfont == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    i = fread(fontptr->bfont,sizeof(BezFont),1,binfile);
    sprintf(mess,"%s\n",dir->filename);
    if ( (i != 1) ||
         (fontptr->bfont->number != font) ||
         (fontptr->bfont->charids > MaxChars) ||
         (fontptr->bfont->defs    > MaxDefs) )
    {
        FREE(fontptr->bfont);
        FREE(fontptr);
        (void) CGMerror(func,ERR_FILCOR,ERROR,mess);
        return(-1);
    }
    fontptr->bchar=(BezChar *)calloc(fontptr->bfont->charids,sizeof(BezChar));
    if (fontptr->bchar == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    i = fread(fontptr->bchar,sizeof(BezChar),fontptr->bfont->charids,binfile);
    if (i != fontptr->bfont->charids)
    {
        FREE(fontptr->bchar);
        FREE(fontptr->bfont);
        FREE(fontptr);
        (void) CGMerror(func,ERR_FILCOR,ERROR,mess);
        return(-1);
    }

    fontptr->bdef = (BezDef *)calloc(fontptr->bfont->defs,sizeof(BezDef));
    if (fontptr->bdef == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    i = fread(fontptr->bdef,sizeof(BezDef), fontptr->bfont->defs,   binfile);
    if (i != fontptr->bfont->defs)
    {
        FREE(fontptr->bdef);
        FREE(fontptr->bchar);
        FREE(fontptr->bfont);
        FREE(fontptr);
        (void) CGMerror(func,ERR_FILCOR,ERROR,mess);
        return(-1);
    }

    B_font=fontptr->bfont;
    B_char=fontptr->bchar;
    B_def=fontptr->bdef;

    fontptr->next=B_fontlist;
    B_fontlist=fontptr;

    if (++B_numfonts> MaxResFonts)
    {
        /* find end of fontlist */
        while(fontptr->next->next!=NULL)
            fontptr=fontptr->next;

        /* FREE storage for last font */
        FREE(fontptr->next->bdef);
        FREE(fontptr->next->bchar);
        FREE(fontptr->next->bfont);
        FREE(fontptr->next);

        /* set last but ones next to NULL */
        fontptr->next=NULL;
        --B_numfonts;
    }
#ifdef DEBUG
    fprintf(stderr,"Loaded font %d in Fontlist\n",font);
#endif

    fclose(binfile);
    return(0);
}

     /*  BEZgetfd:
     |   =======
     |
     |   Put the font details into a Textitem data structure.
     |
     */

#ifdef PROTO
void BEZgetfd( Textitem *txtit )
#else
void BEZgetfd(txtit)

Textitem *txtit;
#endif

{
  int stat,                /*  return status from function call  */
      i;                   /*  looping variable  */
  float maxwidth;          /*  width of widest character  */

#ifdef DEBUG
  fprintf(stderr,"Getting font details for font %d \n",txtit->text_font);
#endif

    if (B_state!=BezOPEN)
    {
        BezOpen();
        if (B_state!=BezOPEN)
        {
            (void) CGMerror(func,ERR_DBUNAV,ERROR,NULLSTR);
            return;
        }
    }

    /*****  Make sure the right font is loaded  */

    if ((B_font==NULL) || (txtit->text_font!=B_font->number))
    {
        stat=(txtit->text_font==0)?BezLoad(DefBezFnt)
                                  :BezLoad(txtit->text_font);
        /***** unable to load requested font, use the default one */
        if (stat==-1)
        { /***** unable to load  defaultfont, return with error
                 and close bezier system                        */
            B_state=BezCLOSED;
            (void) CGMerror(func,ERR_DBUNAV,ERROR,NULLSTR);
            return;
        }
    }

    maxwidth=0;
    for(i=0;i<(B_font->charids);i++)
    {
        txtit->rwd[i]=(B_char[i].width)/B_font->height;
        maxwidth=(B_char[i].width>maxwidth)?B_char[i].width
                                           :maxwidth;
    }

    txtit->rmaxwd=maxwidth/B_font->height;
    txtit->rbot= -(B_font->bottom)/B_font->height;
    txtit->rtop=(B_font->top-B_font->cap)/B_font->height;

    return;
}

/*  BezArc:
|   =======
|
|   Derive a Bezier curve from its control points and put into
|   the Bezier Expanded Character definition arrays.
|
*/

#ifdef PROTO
static void BezArc( int ncurve, int ncontrol, BezDef *bez, Point *pbx)
#else
static void BezArc(ncurve, ncontrol, bez, pbx)

int         ncontrol,        /*  number of Bezier control points  */
            ncurve;          /*  number of Bezier curve points  */

Point       *pbx;            /*  Bezier - expanded definition  */

BezDef      *bez;            /*  control points  */
#endif

{
    int j;
    BezDef *bzc;

    for (j=1;j<ncurve;j++)
    {
        register double
               *pbu, *pbv, tmp;
        register int
            *bcoef;         /*  pointer into binomial coefficients array  */
        int i;
        double u, v,
               px=0.0, py=0.0,
               bu[8],bv[8],
               blend;


        u = (double) j / (double) ncurve;
        v = 1.0-u;

        pbu=bu; pbv=bv+ncontrol-1;
        *pbv=1.0; *pbu=1.0;

        while(pbv>bv)
        {
            tmp = *(pbu) * u;
            *(++pbu) = tmp;
            tmp = *(pbv) * v;
            *(--pbv) = tmp;
        }

        pbu=bu; pbv=bv;
        bcoef = B_coef[ncontrol-1];
        for (bzc=bez,i=0;i<ncontrol;i++)
        {
            blend = (double) (*bcoef++) * (*pbu++) * (*pbv++);
            px += bzc->x * blend;
            py += (bzc++)->y * blend;
        }

        pbx->x   = px;
        (pbx++)->y = py;
    }
}

     /*  BezDraw:
         ======

         Expand the character, and draw it using supplied line function

     */

#ifdef PROTO
static void BezDraw(int ichar, Tmatrix xmat,struct textatt *txtatt,
                   void (*linedraw)(int, Point *, Tmatrix, int))
#else
static void BezDraw(ichar, xmat, txtatt, linedraw)

int ichar;               /*  number of character to draw  */
struct textatt *txtatt;  /*  current text attributes  */
void (*linedraw)();      /*  device routine to do drawing  */
Tmatrix xmat;            /*  transformation matrix  */
#endif
{
    Point *pbx,          /*  pointer into target array  */
          bex[MaxExp];   /*  target array  */
    BezDef *bez,         /*  pointer to transformed character definition  */
           *lp=NULL,
           *cp=NULL,     /*  pointers into transformed control point list  */
           *cpend,       /*  pointer to end of transformed point list  */
           *loc;         /*  pointers to bezier character defintition  */
    double prc = 0.9;
    int ncontrol,        /*  number of control points in bezier curve */
        ncurve,          /*  number of points in expanded curve */
        nbez,            /*  number of points in bezier definition  */
        newchar=TRUE;    /*  boolean newcharacter flag  */
    Tmatrix imat;        /*  identity matirx  */
    float x, y;          /*  used in point transformation  */

    /*****  set up identitiy matrix */
    imat[0][0]=1.0; imat[1][0]=0.0; imat[2][0]=0.0;
    imat[0][1]=0.0; imat[1][1]=1.0; imat[2][1]=0.0;

    /*****  get character component by component  */
    loc = &B_def[B_char[ichar].location];
    nbez = B_char[ichar].numcomp;

    bez = (BezDef *)calloc(nbez, sizeof(BezDef));
    if (bez == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    cp = bez;
    cpend = bez+nbez;

    /*****  get co-ordinates, transform them and plug into arrays
            for the bezier expansion routine.             */
    for(;cp<cpend;loc++, cp++)
    {
        x = (loc->x);
        y = (loc->y);

        cp->x=x*xmat[0][0]+y*xmat[1][0]+xmat[2][0];
        cp->y=x*xmat[0][1]+y*xmat[1][1]+xmat[2][1];
        cp->flag=loc->flag;
    }

    cp=bez;
    pbx=bex;

    while (cp<cpend)
    {
        switch(ncontrol=(cp->flag))
        {
            case 1:  /* Just another pt  */
#ifdef DEBUG2
                fprintf(stderr,"Bezier pt %d\n",ncontrol);
#endif
                break;

            case 2:  /* Start of new element  */
#ifdef DEBUG2
                fprintf(stderr,"Bezier new comp pt %d\n",ncontrol);
#endif

                /*****  Draw the finished component first,
                        then reset target array and start again  */

                /*****  Call graphics system to do the drawing from arrays,
                        if less than 3 cant fill so don't send it. */
                if (pbx-bex > 2)
                {
                    linedraw(pbx-bex, bex, imat, newchar);
                    newchar = FALSE;
                }

                /***** Start from begining of output array again  */
                pbx=bex;

                break;

            default:  /* Last pt (lp) was start of an n pt bezier curve  */
#ifdef DEBUG2
                fprintf(stderr,"Bezier curve pt %d\n",ncontrol);
                /***** If lp NULL here theres an error in the definition  */
                if (lp==NULL)
                {
                    fprintf(stderr,"Error in bezier definition\n");
                    return;
                }
#endif

                /***** Expand the curve and put it in the target arrays  */

                ncurve=(int)(prc*BezNest(ncontrol, lp, txtatt))+1;
                if (ncurve < ncontrol) ncurve = ncontrol;

                if (pbx-bex+ncurve > MaxExp)
                {
                    (void)CGMerror(func, ERR_CURVE2BIG, WARNING, NULLSTR);
                    return;
                }

                BezArc(ncurve, ncontrol, lp, pbx);

                cp += ncontrol-2;
                pbx += ncurve-1;

                break;
        }

        /***** Put pt in target array*/
        pbx->x = cp->x;
        (pbx++)->y = cp->y;
        lp=cp++;
    }

    /*****  Call graphics system to do the drawing from arrays,
            if less than 3 cant fill so don't send it. */
    if (pbx-bex > 2)
        linedraw(pbx-bex, bex, imat, newchar);
    FREE(bez);

    return;
}

     /*  BezNest:
         =======

         Calculate the nominal point allowance for the Bezier curve.

     */

#ifdef PROTO
static double BezNest( int ncontrol, BezDef *bez, struct textatt *txtatt )
#else
static double BezNest(ncontrol, bez, txtatt)

int ncontrol;            /*  number of control points  */
BezDef *bez;             /*  Bezier control points  */
struct textatt *txtatt;
#endif

{
    int i;

    double xxi, yyi,              /*  current control point  */
           total = 0.0;           /*  sum of squares  */

    for (i=1;i<ncontrol;i++)
    {
        xxi = txtatt->shared->xgrain * (bez[i].x - bez[i-1].x);
        yyi = txtatt->shared->ygrain * (bez[i].y - bez[i-1].y);
        total += xxi*xxi + yyi*yyi;
    }

    return (double) SQRT( (ncontrol-1) * SQRT(total) / 3.0 );
}
