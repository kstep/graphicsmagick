/*   RAL-CGM Interpreter module:  @(#) cgmher.c  version 3.1
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
 * Description:   HERSHEY text system
 *
 *  Provides comprehensive facilities for output of text using
 *  Hershey line definitions of the characters.
 *
 *  Original author:  K E V Palmen (RAL) [ actually of the Bezier system ]
 *
 *  C Translator:     C D Osland (RAL)
 *
 *  Translation to C: August 1988
 *
 *  Almost total rewriten by C D Seelig around August 1990
 *
 *  Maintenance log:
 *
 *   1 Dec 90 CDS Stabilise Release 1 version
 *   4 Dec 90 RTP Remove int return from HERfds, change name to
 *                 HERgetfd and add ANSI prototyping
 *   7 Dec 90 RTP Use Point types where appropriate
 *  17 Dec 90 RTP Change algorithm for handling 'SignChar's for Cray
 *  25 Apr 90 CDS Changed frees to #defed FREE
 *  30 Apr 91 CDS Added New character flag to line routine calls. This
 *                allows drivers to optimize output better.
 *  20 May 91 CDS Moved hershey dependant bits from HERtext to HerDraw,
 *                Moved character refernce point handling to genher.
 *  25 May 91 CDS Corrected bug in HerLoad which caused wrong fonts to
 *                be used in majority of cases!!.
 *  21 Jun 91 CDS Added code to expand bin file name, which is no longer
 *                in the expanded form in directory file.
 *  11 Sep 91 CDS Changed to use utility specific header CGMUHER.H
 *  11 Sep 91 CDS Changed to call general error messages instead of
 *                Bezier ones!
 *  02 Oct 91 CDS Added checks to all memory allocations!
 *  03 Oct 91 CDS Corrected possible bug in HerOpen
 *  20 Nov 92 CDS Added check to prevent needless work for space characters
 *  27 Nov 92 CDS Changed ifdef ANSI to ifdef PROTO for function prototypes
 *
 */

#define CGMHER_C

#include "cgmtext.h"
#define CGMHER_C
#include "cgmuher.h"
#include "cgmerr.h"
#include "cgmfile.h"

static char *func = "CGMher";

     /*  HerOpen:
      |  ========
      |
      |  Open and intialise the Hershey emulation system.
      |
     */

#ifdef PROTO
static void HerOpen( void )
#else
static void HerOpen()
#endif

{
    int nread;
    char *dirname;
    FILE *dirfile;

    /***** If system is already open then return straight away */
    if (H_state == HerOPEN) return;

    /***** Open directory file */
    dirname = CGMpath("fontdir",herext);
    dirfile = CGMfopen(dirname, F_READ, BINARY);
    if ( dirfile == NULL)
    {   /* Exit on the spot if open failed */
        CGMerror(func, ERR_OPENFILE, FATAL, dirname);
        (void)FREE(dirname);
        return;
    }

    /*****  Read in the directory, so there is an in-core list of all
            available fonts.                                           */
    H_dir=(HerDir *)calloc(MaxFonts+1,sizeof(HerDir));
    if (H_dir == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    nread = fread( H_dir, sizeof(*H_dir), MaxFonts+1, dirfile);
    if (ferror(dirfile))
    {
        (void) CGMerror(func, ERR_FNTNOTOPEN, ERROR, dirname);
        FREE(dirname); FREE(H_dir);
        return;
    }

#ifdef DEBUG
    {
        HerDir *i;
        for(i=H_dir;i->number!=-1;i++)
        fprintf(stderr,"Dir entry\nCode %d\n Fontname %s\nFontfile %s\n\n",
                i->number,i->name,i->filename);
    }
#endif

    /***** Realloc, note there is no check on this as it always reduces the
           amount of memory being used                                       */
    H_dir=(HerDir *)realloc(H_dir, nread*sizeof(HerDir));
    if ( nread == 0)
    {   /* No Hershey fonts, give up here */
        (void) CGMerror(func,ERR_NOFONTS,ERROR,dirname);
        FREE(H_dir);
        fclose(dirfile);
        FREE(dirname);
        return;
    }
    fclose(dirfile);
    FREE(dirname);

    /*****  Mark the system as ready to go  */
    H_state = HerOPEN;
}

     /*  HERtext:
      |  ========
      |
      |  Draw a given text string using the Hershey emulation system.
      |
     */

#ifdef PROTO
void HERtext( Textitem *txtit, Point txtp,
              void (*linedraw)( int, Point*, Tmatrix, int ) )
#else

void HERtext(txtit, txtp, linedraw)

Textitem *txtit;
Point txtp;
void (*linedraw)();
#endif

{
    register char *c;       /*  Pointer to current character in txtit */
    register Point
          *pt;              /*  pointers into array of centres from txtitem  */
    int stat,               /*  return status from called routine  */
        ichar;              /*  character value of character to be printed  */

    float xscale,
          yscale;

    Tmatrix tl2vmat,        /*  textline to vdc transformation matrix  */
            xmat;           /*  total transformation matrix  */


    /*****  Make sure that the Hershey system is initialised  */
    if (H_state!=HerOPEN)
    {
        HerOpen();
        if (H_state!=HerOPEN)
        {
            (void) CGMerror(func,ERR_DBUNAV,ERROR,NULLSTR);
            return;
        }
    }

    /*****  Make sure the right font is loaded  */
    if ((H_font==NULL) || (txtit->text_font!=H_font->number))
    {
        stat=(txtit->text_font==0)?HerLoad(DefHerFnt)
                                  :HerLoad(txtit->text_font);
        /***** unable to load requested font, use the default one */
        if (stat==-1)
        { /***** unable to load  default font, return with error
                 and close Hershey system                        */
            H_state=HerCLOSED;
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
    yscale = (txtit->char_height) / (H_font->height);
    xscale = (txtit->char_exp) * yscale;

    /*****  Calculate what we can of the final transformation matrix  */
    xmat[0][0]=tl2vmat[0][0]*xscale;
    xmat[1][0]=tl2vmat[1][0]*yscale;
    xmat[0][1]=tl2vmat[0][1]*xscale;
    xmat[1][1]=tl2vmat[1][1]*yscale;

    for (; *c!='\0' ; c++, pt++)
    {
        ichar = (int) *c - 32;

        /* if character is space save a lot of time and skip it! */
        if (ichar==0) continue;

        /*****  Finish calculation of the final transformation matrix  */
        xmat[2][0]=tl2vmat[0][0]*(pt->x)+tl2vmat[1][0]*(pt->y)+tl2vmat[2][0];
        xmat[2][1]=tl2vmat[0][1]*(pt->x)+tl2vmat[1][1]*(pt->y)+tl2vmat[2][1];

        HerDraw(ichar, xmat, linedraw);

    }
}

     /*  HerLoad:
      |  =======
      |
      |  Load a new font from disk.
      |
     */

#ifdef PROTO
static int HerLoad(Index font)
#else
static int HerLoad(font)

Index font;
#endif

{
    FILE *binfile;
    char *binname, mess[80];
    int i;
    HerDir  *dir;
    HerFlist *fontptr;

#ifdef DEBUG
    fprintf(stderr,"Trying to find font %d\n",font);
#endif

    /*****  Look for the font in the font list  *****/
    for(fontptr=H_fontlist;fontptr!=NULL;fontptr=fontptr->next)
    {
        if (font==fontptr->hfont->number)
        {   /***** EXIT POINT , if font is in list set it to current font
                   and then return                                         */
            H_font=fontptr->hfont;
            H_char=fontptr->hchar;
            H_def=fontptr->hdef;
#ifdef DEBUG
            fprintf(stderr,"Found font %d in Fontlist\n",font);
#endif
            return(0);
        }
    }

    /***** and now in the font directory  *****/
    for (dir = H_dir;(dir->number!=-1) && (dir->number!=font);dir++)
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
    fontptr = (HerFlist *)calloc(1,sizeof(HerFlist));
    if (fontptr == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);

    /***** Read in the font header information - only first 5 */
    fontptr->hfont = (HerFont *)calloc(1,sizeof(HerFont));
    if (fontptr->hfont == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    i = fread(fontptr->hfont,sizeof(HerFont),1,binfile);
    sprintf(mess,"%s\n",dir->filename);
    if ( (i != 1) ||
         (fontptr->hfont->number != font) ||
         (fontptr->hfont->charids > MaxChars) ||
         (fontptr->hfont->defs    > MaxDefs) )
    {
        FREE(fontptr->hfont);
        FREE(fontptr);
        (void) CGMerror(func,ERR_FILCOR,ERROR,mess);
        return(-1);
    }
    fontptr->hchar=(HerChar *)calloc(fontptr->hfont->charids,sizeof(HerChar));
    if (fontptr->hchar == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    i = fread(fontptr->hchar,sizeof(HerChar),fontptr->hfont->charids,binfile);
    if (i != fontptr->hfont->charids)
    {
        FREE(fontptr->hchar);
        FREE(fontptr->hfont);
        FREE(fontptr);
        (void) CGMerror(func,ERR_FILCOR,ERROR,mess);
        return(-1);
    }

    fontptr->hdef = (HerDef *)calloc(fontptr->hfont->defs,sizeof(HerDef));
    if (fontptr->hdef == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    i = fread(fontptr->hdef,sizeof(HerDef), fontptr->hfont->defs,   binfile);
    if (i != fontptr->hfont->defs)
    {
        FREE(fontptr->hdef);
        FREE(fontptr->hchar);
        FREE(fontptr->hfont);
        FREE(fontptr);
        (void) CGMerror(func,ERR_FILCOR,ERROR,mess);
        return(-1);
    }

    H_font=fontptr->hfont;
    H_char=fontptr->hchar;
    H_def=fontptr->hdef;

    fontptr->next=H_fontlist;
    H_fontlist=fontptr;

    if (++H_numfonts> MaxResFonts)
    {
        /* find end of fontlist */
        while(fontptr->next->next!=NULL)
            fontptr=fontptr->next;

        /* FREE storage for last font */
        FREE(fontptr->next->hdef);
        FREE(fontptr->next->hchar);
        FREE(fontptr->next->hfont);
        FREE(fontptr->next);

        /* set last but ones next to NULL */
        fontptr->next=NULL;
        --H_numfonts;
    }
#ifdef DEBUG
    fprintf(stderr,"Loaded font %d in Fontlist\n",font);
#endif

    fclose(binfile);
    return(0);
}

     /*  HERgetfd:
     |   =======
     |
     |   Put the font details into a Textitem data structure.
     |
     */

#ifdef PROTO
void HERgetfd( Textitem *txtit )
#else
void HERgetfd(txtit)

Textitem *txtit;
#endif

{
    register int i;        /*  looping variable  */
    int stat;              /*  return status from function calls  */
    float maxwidth;        /*  width of widest character  */
    char *mess="HERSHEY";



#ifdef DEBUG
    fprintf(stderr,"Getting font details for font %d \n",txtit->text_font);
#endif

    if (H_state!=HerOPEN)
    {
        HerOpen();
        if (H_state!=HerOPEN)
        {
            (void) CGMerror(func,ERR_DBUNAV,ERROR,mess);
            return;
        }
    }

    /*****  Make sure the right font is loaded  */

    if ((H_font==NULL) || (txtit->text_font!=H_font->number))
    {
        stat=(txtit->text_font==0)?HerLoad(DefHerFnt)
                                  :HerLoad(txtit->text_font);
        /***** unable to load requested font, use the default one */
        if (stat==-1)
        { /***** unable to load  default font, return with error
                 and close Hershey system                        */
            H_state=HerCLOSED;
            (void) CGMerror(func,ERR_DBUNAV,ERROR,mess);
            return;
        }
    }

    maxwidth=0;
    for(i=0;i<(H_font->charids);i++)
    {
        txtit->rwd[i]=(float)(H_char[i].width)/H_font->height;
        maxwidth=(txtit->rwd[i]>maxwidth)?txtit->rwd[i]
                                         :maxwidth;
    }

    txtit->rmaxwd=maxwidth/H_font->height;
    txtit->rbot=(float)(H_font->bottom)/H_font->height;
    txtit->rtop=(float)(H_font->top-H_font->cap)/H_font->height;

    return;
}

     /*  HerDraw:
      |  ======
      |
      |  Expand the character, and draw it using supplied line function
      |
     */

#ifdef PROTO
static void HerDraw(int ichar, Tmatrix xmat,
                   void (*linedraw)(int, Point *, Tmatrix, int))
#else
static void HerDraw(ichar, xmat, linedraw)

int ichar;               /*  number of character to draw  */
void (*linedraw)();      /*  device routine to do drawing  */
Tmatrix xmat;            /*  transformation matrix  */
#endif
{
    register HerDef
           *cp;          /*  pointers into transformed control point list  */

    register Point
          *pbx;          /*  pointer into target array  */
    Point bex[MaxExp];   /*  target array  */
    HerDef *her;         /*  pointer to transformed character definition  */
    int newchar=TRUE;    /*  boolean newcharacter flag  */

    /*****  get character component by component  */
    her = &H_def[H_char[ichar].location];

    cp = her;
    pbx = bex;

    while (TRUE)
    {
        if (cp->x == (SignChar)-64)
        {
            /*****  Call up the graphics system to do the drawing */
            linedraw (pbx-bex, bex, xmat, newchar);
            pbx=bex;
            newchar = FALSE;
            if (cp->y == (SignChar)-64)
                /***** end of char  */
                break;
            /***** end of polyline  */
            cp++;
            continue;
        }
        /***** Put pt in target array*/
        pbx->x = (float)(cp->x);
        (pbx++)->y = (float)((cp++)->y);
    }

    return;
}
