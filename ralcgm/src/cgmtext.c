/*   RAL-CGM Interpreter module:  @(#) cgmtext.c  version 3.5
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
 * Authors: William K Lui, Predrag L Popovic, Chris D Seelig.
 *
 * Description: A set of routines to handle full CGM text attributes.
 *              Functions contained: TXTaccinf(),
 *                                   TXTalign(),
 *                                   TXTfree(),
 *                                   TXTgwidth(),
 *                                   TXTgheight(),
 *                                   TXTxform(),
 *                                   TXTrestrict(),
 * Maintenance Log:
 *
 *  26 Jul 90 CDS  TXTaccinf: original version stabilized.
 *  01 Aug 90 PLP  xform: original version stabilized.
 *  02 Aug 90 WKL  TXTalign: original version stabilized.
 *  12 Aug 90 PLP  Changed calling sequences for TXTalign
 *                 TXTaccinf; corrected LEFT path origin
 *                 calculation and several small omissions
 *                 in TXTalign. Commented throughout.
 *  13 Aug 90 WKL  Returned Apend Text flag back to TXTaccinf's
 *                 calling sequence.
 *  15 Aug 90 PLP  TXTgwidth and TXTgheight now made external;
 *                 added extents to the list of TXTalign's arguments.
 *  17 Aug 90 RTP  Change include files and declare ftnames & txttot[a-f].
 *  17 Aug 90 PLP  Removed unused locals; sorted out malloc, calloc,
 *                 realloc and free's argument typecasting. Changed
 *                 TXTxform to take an input array of 3 by 2 doubles
 *                 and store the matrix in it.
 *  18 Aug 90 PLP  Introduced skip2digit() and skipdigits.
 *  19 Aug 90 PLP  Introduced error handler and CGMpath, fixed
 *                 problems with skip2digit() and skipdigits. Where
 *                 applicable added default cases in switch statements;
 *                 also added (for now dummy) new text methods and
 *                 a dummy function for abbreviating long .afm filenames.
 *  19 Aug 90 PLP  Wrote a proper abbrev function.
 *  19 Aug 90 PLP  Renamed abbrev() to alias() and added freeing of the
 *                 aliased name.
 *  27 Aug 90 PLP  Completed change to using cgmatt.c and cgmfont.c;
 *                 subsequently removed TXTvdc2flt and tmpFNTmatch, now
 *                 obsolete.
 *  06 Sep 90 PLP  Declared all local functions static, added cgmfont.h.
 *  15 Sep 90 CDO  Replaced malloc calls to calloc (1, ...) calls to stop
 *                 some compilers complaining about boundary problems when
 *                 space is being reserved for a non-char variable.
 *  17 Sep 90 PLP  Moved the PostScript dependent functions (alias(),
 *                 getpsfd(), readafm(), skip2digit() and skipdigits()) to
 *                 the PostScript driver. Changed getpsfd() to PSgetfd.
 *  18 Sep 90 PLP  Put conditional compilation statements around PSgetfd.
 *  19 Sep 90 PLP  Sorted out functions and variables declarations.
 *  12 Oct 90 CDS  Made TXTaccinf produce error if it doesn't find a valid
 *                 text system.
 *  15 Oct 90 CDS  Changed TXTxform to use type Tmatrix, since this was
 *                 the only routine to use *double not *float.
 *                 Changed reference by pointer to reference as array,
 *                 to make more readable, this has NO effect on performance
 *                 since all subscripts are constant.
 *  26 Oct 90 RTP  Store string as ASCII even if machine is EBCDIC
 *   2 Nov 90 NMH  Added calling of Xw_getfd to TXTaccinf if XW defined.
 *                 Conditional definition of prototype added to cgmtext.h
 *   7 Dec 90 RTP  Change float arguments to Point where appropriate
 *  11 Feb 91 RTP  Add extra Debug messages
 *  14 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change calloc calls to MALLOC and cast to (size_t) for
 *                 malloc
 *  20 May 91 RTP  Use FREE macro instead of free()
 *  19 Jun 91 RTP  Change define name to CGMTEXT_C
 *  25 Jun 91 CDS  TXTxform: Cut out all the needless calculation.
 *  25 Jun 91 CDS  TXTxform: Documented Local variable usage.
 *   2 Aug 91 RTP  Add SCCS id
 *  26 Jun 92 MAS  Add the new routine TXTrestrict to process restricted
 *                 text properly.
 *   6 Oct 94 PMG  Fix wrong sign when dealing with P_LEFT in calculating character origin
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *   6 Feb 95 KEVP Add check of validity of characters in text string
 *  28 Feb 95 KEVP TXTxform: Check character orientation vectors
 *
 */

#define CGMTEXT_C
#include "cgmout.h"
#include "cgmtext.h"
#include "cgmfont.h"

static char *func="cgmtext";

typedef struct res_text {
   Textitem   *ptr;
   Logical    minimum;
} Restxt_item;

#ifdef PROTO
   static Logical scale_spc_exp( Textitem*, double, double, Logical );
   static Textitem *clip_text( Textitem*, double, double, int );
   static void font_load( Textitem* );
   static double calc_scale( Restxt_item*, int, double, double );
   static int size_check( Textitem*, double*, double*, double, double, float *);
#else
   static Logical scale_spc_exp();
   static Textitem *clip_text();
   static void font_load();
   static double calc_scale();
   static int size_check();
#endif

#ifdef PROTO
Textitem *TXTaccinf( long num, char *str, struct textatt *ta,
                     Enum *prefmethods )
#else
Textitem *TXTaccinf( num, str, ta, prefmethods )
long num;
char *str;
struct textatt *ta;
Enum *prefmethods;
#endif
/*
 *    TXTaccinf(): Function to store a string, reserve a structure
 *                 for it and fill-in all the necessary attributes
 *                 and font data information. Links the structure
 *                 into a list where apliccable.
 *
 *                 IN:  Append text flag.
 *                      String item to be stored.
 *                      Pointer to the attributes structure.
 *                      Array of preferred font methods.
 *                 OUT: None.
 *                 RETURN VALUE: Pointer to the head of the list.
 *
 *                 Author: CDS
 *
 * ALGORITHM
 * ---------
 *
 * Allocate memory needed for one Textitem structure. Link the structure
 * into the list if necessary, and fill-in with available information
 * (text attributes and font data).
 *
 * LOCALS
 * ------
 * fonts;
 * func
 * head
 * methods;
 * new
 * tmp
 * tail
 *
 */

{
   static Textitem *head=NULL, *tail=NULL;
   Textitem *new, *tmp;
   char *func="TXTaccinf", *cp;
   Enum methods[2];
   Index fonts[2];
   Int ichar;
   Logical string_error=FALSE; /* Flag to prevent repeated error messages */

   /* Get memory for this item's structure */
   new=(Textitem *)MALLOC( 1, sizeof(Textitem) );

   /* Exit if insufficient memory */
   if (new==NULL)
      exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));

   /* Make this item last item in the list */
   new->next=NULL;

   /*
      Initialize head and tail if this is the first item,
      link properly if not.
   */
   if (head==NULL)
   {
      head=new;
      tail=new;
   }
   else
   {
      tail->next=new;
      tail=new;
   }

   /* Allocate memory for and store the string. */
   new->str=(char *)MALLOC (1,(strlen(str)+1) );

   /* Exit if insufficient memory */
   if (new->str==NULL)
      exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));

   (void)strcpy(new->str,str);


   /* Check validity of characters in string */
   for( cp = new->str; *cp != '\0'; cp++ )
   {
#ifdef EBCDIC   /* Translate to ASCII on EBCDIC machines */
      *cp = cgmascii[(unsigned int)*cp];
#endif
      ichar = (unsigned short)(*cp) - 0x20;
      if( 0 > ichar || ichar >= MAX_FONT_CHARS )
      {
         if(!string_error) CGMerror (func, ERR_NPCINSTR, ERROR, str);
         string_error = TRUE;
         *cp = ' ';
      }
   }


   /* Fill details from the text attributes structure */
   new->text_font=ta->fontindex;
   new->text_prec=ta->precision;
   new->char_exp=ta->expansion;
   new->char_space=ta->spacing;
   new->colourindex=ta->colindex;
   new->text_col.red=ta->col.red;
   new->text_col.green=ta->col.green;
   new->text_col.blue=ta->col.blue;
   new->char_height=ta->height;
   new->text_hcont=ta->hcont;
   new->text_vcont=ta->vcont;

   /* Fill in shared attributes */
   new->aux_colourindex=ta->shared->auxindex;
   new->aux_col.red=ta->shared->aux.red;
   new->aux_col.green=ta->shared->aux.green;
   new->aux_col.blue=ta->shared->aux.blue;
   new->colour_mode=ta->shared->colmode;
   new->transparency=ta->shared->transparency;

   /*
      Call routine to get the text method, and font number.
   */

   (void)FNTmatch(ta, prefmethods, methods, fonts);
   new->text_method = methods[0];
   new->altext_method = methods[1];
   new->text_font = fonts[0];
   new->altext_font = fonts[1];

   /* Depending on font method used, call appropriate font enquiry routine */
#ifdef DEBUG
   (void) fprintf(stderr, "Font method: %d\n", new->text_method);
#endif
   switch (new->text_method)
   {
#ifdef XW
      case HDW:
        Xw_getfd(new);
      break;
#endif
#ifdef HERSHEY
      case HER:
         HERgetfd(new);
      break;
#endif
#ifdef BEZIER
      case BEZ:
         BEZgetfd(new);
      break;
#endif
#ifdef POSTSCRIPT
      case PS:
         PSgetfd(new);
      break;
#endif
#ifdef TEX
      case TEX:
          /* awaiting decision */
      break;
#endif
      default:
          /* Should do same thing as for hardware!
             But for the moment, tell people
             we didn't find a valid textstyle!!         */
      (void) CGMerror(func,ERR_TXTSYSNFND,ERROR,NULLSTR);
      break;
   }
   if (num == FINAL)
   {
      tmp = head;
      head = tail = NULL;
      return tmp;
   }
   /* All done - exit TXTaccinf() */
   return head;
}

#ifdef PROTO
void TXTalign ( Textitem *txtlist, Point txtp, double horext, double vertext )
#else
void TXTalign ( txtlist, txtp, horext, vertext )
Textitem *txtlist;
Point txtp;
double horext, vertext;
#endif
/*
 *    TXTalign(): function to process the text list and insert
 *                the information on origin coordinates (in untransformed
 *                VDCs) for each character of each string in the list.
 *                NB: By 'untransformed' we here mean that both character
 *                    orientation and device transformations have not been
 *                    performed.
 *
 *                IN:  Pointer to the head of the list
 *                     Alignment point's X coordiante
 *                     Alignment point's Y coordinate
 *                     The entire text's horizontal extent (untransformed VDC)
 *                     The entire text's vertical extent (untransformed VDC)
 *                OUT: None
 *                RETURN VALUE: None.
 *
 *                Author: WKL/PLP
 *
 * Maintenance Log:
 *
 *  19 Nov 91 AMR  Implemented Continuous, Vertical and Horizontal Text.
 *
 *
 * ALGORITHM
 * ---------
 *
 * Go through the text list and, asuming "normal" character orientation
 * (i.e. like in this text) but respecting all other text attributes,
 * work out the origins of all characters.
 *
 * LOCALS:
 * ------
 * a_rbot
 * a_rth
 * a_rtop
 * activeitem
 * af_rbot
 * af_rth
 * af_rtop
 * al_rbot
 * al_rth
 * al_rtop
 * ch1
 * cp
 * h1
 * i
 * lastbot
 * lastheight
 * lastorigin
 * lastspace
 * lastwidth
 * ntxtx
 * ntxty
 * theight
 * twid
 * txtlp
 * xadj
 * yadj
 *
 */

{
   int i;
   float twid, theight, xadj, yadj, h1, ch1;
   float lastbot, lastheight, lastorigin, lastspace, lastwidth;
   float a_rth, a_rbot, a_rtop;
   float af_rth, af_rbot, af_rtop;
   float al_rth, al_rbot, al_rtop;
   float ntxtx, ntxty;
   char *cp;
   Textitem *txtlp ,*activeitem;

#ifdef DEBUG
     fprintf(stderr,"TXTalign: (%f,%f) %f %f\n", txtp.x, txtp.y,
                      horext, vertext);
#endif
   txtlp = txtlist;

   /* pick up the width of bounding box  */
   twid = horext;

   /* pick up the height of bounding box */
   theight = vertext;

   /*
     For left and right paths, find the X displacement from the alignment
     point to the X origin of the first character in the first string.
     For up and down paths, horizontal alignment is done to the
     centre-line of the text, so we want to find the displacement
     from the alignment point to the centre-line.
   */
   switch (curatt.text_halign)
   {
      case NORMHORIZ: /* xadj is 0 for all paths */
           xadj = 0.0;
           break;

      case LEFT:  /* left */
           switch (curatt.text_path)
           {
               case P_LEFT:
                   /*
                      we need to substract width of the first character
                      here, but it will be more convenient to do it later.
                    */
                    xadj = twid;
                    break;
               case P_RIGHT:
                    xadj = 0.0 ;
                    break;
               case P_UP:
               case P_DOWN:
                    xadj = twid/2.0;
                    break;
           }
           break;

      case CTR:  /* centre */
           switch (curatt.text_path)
           {
               case P_LEFT:
                    /*
                      we need to substract width of the first character
                      here, but it will be more convenient to do it later.
                     */
                    xadj = twid/2.0;
                    break;
               case P_RIGHT:
                    xadj = -twid/2.0;
                    break;
               case P_UP:
               case P_DOWN:
                    xadj = 0.0;
                    break;
           }
           break;

      case RIGHT:  /* right */
           switch (curatt.text_path)
           {
               case P_LEFT:
                    /*
                      we need to substract width of the first character
                      here, but it will be more convenient to do it later.
                     */
                    xadj = 0.0;
                    break;
               case P_RIGHT:
                    xadj = -twid;
                    break;
               case P_UP:
               case P_DOWN:
                    xadj = -twid/2.0;
                    break;
           }
           break;

      case CONTHORIZ:  /* continuous horizontal - needs to be checked*/
           xadj = -twid * curatt.text_hcont;
           break;
   }

   /*
     Get the adjustment value for the Y coordinate of the
     alignment point, such that we eventually arrive at the
     base line of the first character in the text, regardless
     of the path and alignment.
   */
   switch (curatt.text_path)
   {
      case P_LEFT:
      case P_RIGHT:
           /*
             For left and right, vertical alignment is dictated by the
             "active" item, i.e. the item with biggest value for character
             height. Here we find it.
            */
           h1 = 0;
           txtlp = txtlist ;
           while (txtlp!=NULL)
           {
              ch1 = txtlp->char_height;
              if (ch1>h1)
                    { h1 = ch1; activeitem = txtlp;}
              txtlp = txtlp->next;
           }

           /* Get relevant font data of the active item */
           a_rth = activeitem->char_height;
           a_rbot = activeitem->rbot * a_rth;
           a_rtop = activeitem->rtop * a_rth;

           /* Now go down to the lower left corner of the bounding box */
           switch (curatt.text_valign)
           {
              case TOP:  /* top */
                   yadj = - theight ;
                   break;
              case CAP:  /* cap */
                   yadj = - theight + a_rtop ;
                   break;
              case HALF:  /* half */
                   yadj = - a_rth/2.0 - a_rbot;
                   break;
              case NORMVERT: /* normal vertical */
              case BASE:  /* base */
                   yadj = - a_rbot;
                   break;
              case BOTTOM:  /* bottom */
                  yadj = 0.0;
                  break;
              case CONTVERT:  /* continuous vertical - needs checking*/
                    yadj = -theight * curatt.text_vcont;
                    break;
           }

           /* Having found the bottom line, move up to the base line */
           yadj = yadj + a_rbot;
           break;

      case P_UP:
      case P_DOWN:
           /* For up and down, first and last character are important */
           af_rth = txtlist->char_height;
           af_rbot = txtlist->rbot * af_rth;
           af_rtop = txtlist->rtop * af_rth;

           txtlp = txtlist ;
           while (txtlp->next)
               txtlp=txtlp->next;
           al_rth = txtlp->char_height;
           al_rbot = txtlp->rbot * al_rth;
           al_rtop = txtlp->rtop * al_rth;

           switch ( curatt.text_valign)
           {
              case NORMVERT: /* normal vertical */
                   if (curatt.text_path == P_UP)
                      yadj = 0.  ;
                   else     /* down */
                      yadj = - af_rtop   - af_rth;
                   break;

              case TOP:  /* top */
                   if (curatt.text_path == P_UP)
                      yadj = - theight  + af_rbot  ;
                   else     /* down */
                      yadj = - af_rtop   - af_rth;
                   break;

              case CAP:  /* cap */
                   if (curatt.text_path == P_UP)
                      yadj = - theight + al_rtop + af_rbot  ;
                   else
                      yadj =  - af_rth;
                   break;

              case HALF:  /* half */
                   if (curatt.text_path == P_UP)
                      yadj = -(theight-(al_rtop+al_rth/2.)
                             -(af_rbot+af_rth/2.)) / 2. - af_rth/2.;
                   else
                      yadj = (theight-(af_rtop+af_rth/2.)
                               -(al_rbot+af_rth/2.)) / 2. - af_rth/2.0;
                   break;

              case BASE:  /* base */
                   if (curatt.text_path == P_UP)
                      yadj = 0.;
                   else
                      yadj = theight - al_rbot - af_rtop - af_rth;
                   break;

              case BOTTOM:  /* bottom */
                   if (curatt.text_path == P_UP)
                      yadj = af_rbot;
                   else
                      yadj = theight - af_rtop - af_rth;
                   break;

              case CONTVERT:  /* continuous vertical - needs checking*/
                   yadj = -theight * curatt.text_vcont;
                   break;
           }
           break;
  }


/*
   Calculate the "adjusted" alignment point, i.e. for left and right
   get the origin of the first character; for up and down get the
   base line and centre-line intersection of the first character .
 */
   ntxtx = txtp.x + xadj;
   ntxty = txtp.y + yadj;

/* Now work out the origin of each character and in every string. */
   txtlp = txtlist;
   while (txtlp)
   {
      txtlp->origin = (Point *)MALLOC ( strlen(txtlp->str), sizeof(Point) );
      i=0;
      for(cp = txtlp->str; *cp!='\0'; cp++)
      {
         switch (curatt.text_path)
         {
            case P_RIGHT:
                 /* formula :
                 First character of first string:
                             x = ntxtx;
                             y = ntxty;
                 Then:
                             x += height * (exp*normalized_width + spacing)
                             y = ntxty
                 */

                 /* if first character of first string x= ntxtx */
                 if((i==0)&&(txtlp == txtlist))
                 {
                    lastorigin =  ntxtx;
                    lastwidth =  0.;
                    lastspace =  0.;
                 }
                 txtlp->origin[i].x = lastorigin + lastwidth + lastspace;
                 lastorigin = txtlp->origin[i].x;
                 lastwidth = txtlp->char_height *
                             txtlp->char_exp *
                             txtlp->rwd[(int)(*cp)-32];
                 lastspace = txtlp->char_height *
                             txtlp->char_space;
                 txtlp->origin[i].y = ntxty;
                 break;

            case P_LEFT:
                 /*
                    First ever:
                               x = nxtx-height*(expansion*normalized_width);
                               y = ntxty;
                    Then:
                               x - = height* (exp*normwid + spacing);
                               y = ntxty;
                 */

                 /* if first character of str x = ntxtx - width(1st) */

                 if((i==0)&&(txtlp == txtlist))
                 {
                    lastorigin = ntxtx;
                    lastspace = 0.;
                 }
		                                    /*  PMG change sign of lastspace */
                 txtlp->origin[i].x = lastorigin -
                                     txtlp->char_height *
                                     txtlp->char_exp *
                                     txtlp->rwd[(int)(*cp)-32] - lastspace;
                 lastorigin = txtlp->origin[i].x;
                 lastspace = txtlp->char_space * txtlp->char_height;

                 txtlp->origin[i].y = ntxty;
                 break;

            case P_UP:
                 /*
                   First ever:
                              x = ntxtx - half_of_current_width;
                              y = ntxty;
                   Then:
                              x = ntxtx - half_of_current_width;
                              y += current_height + top + spacing + bot;
                 */

                 txtlp->origin[i].x = ntxtx -
                                     txtlp->char_height * txtlp->char_exp *
                                     (txtlp->rwd[(int)(*cp)-32])/2.0;

                 if((i==0)&&(txtlp == txtlist))
                 {
                    lastorigin =  ntxty - txtlp->char_height * txtlp->rbot;
                    lastheight =  0.;
                 }

                 txtlp->origin[i].y = lastorigin + lastheight +
                                     txtlp->char_height *
                                     txtlp->rbot;
                 lastorigin = txtlp->origin[i].y;
                 lastheight = txtlp->char_height * (1 + txtlp->rtop) ;
                 break;

            case P_DOWN:
                 /*
                   First ever:
                             x = ntxtx - half_of_current_width;
                             y = ntxty;
                   Then:
                             x = x;
                             y -= bot - spacing - top - height;
                  */

                 txtlp->origin[i].x = ntxtx - txtlp->char_height *
                                     txtlp->char_exp *
                                     (txtlp->rwd[(int)(*cp)-32])/2.0;

                 /* if first character of str y = ntxty */
                 if((i==0)&&(txtlp == txtlist))
                 {
                    lastorigin =  ntxty +
                                  txtlp->char_height *
                                  (1 + txtlp->rtop);
                    lastbot =  0.;
                 }
                 txtlp->origin[i].y = lastorigin - lastbot -
                                     txtlp->char_height *
                                     (1 + txtlp->rtop);
                 lastorigin = txtlp->origin[i].y;
                 lastbot = txtlp->char_height * txtlp->rbot ;
         }
         i++;
      }
      txtlp = txtlp -> next;
   }
   /* return from TXTalign */
   return;
}

#ifdef PROTO
void  TXTfree( Textitem *txtlist )
#else
void  TXTfree( txtlist )
Textitem *txtlist;
#endif
/*
 *    TXTfree(): Function which frees the memory allocated while
 *               processing text.
 *
 *               IN: Pointer to the head of a text list.
 *               OUT: None.
 *               RETURN VALUE: None.
 *
 *               Author: WKL
 *
 * ALGORITHM
 * ---------
 *
 * First free the memory for the string and the origins. Then free the
 * memory for the structure itself.
 *
 *  Maintenance Log:
 *
 *  25 Jun 91 CDS  Corrected possibility of unfreed storage.
 *  25 Jun 91 CDS  Documented Local variable usage.
 *
 */

{
/*
 * LOCALS
 * ------
 */
   Textitem *txtlistp1;     /* temp pointer used for struct position */

   /* Work down the list */
   while (txtlist!=NULL)
   {
      /* Store the pointer to the structure */
      txtlistp1 = txtlist;

      /* Move the item pointer on */
      txtlist = txtlist->next;

      /* Free string and origins first */
      FREE(txtlistp1->str);
      FREE(txtlistp1->origin);
      /* Free the structure */
      FREE(txtlistp1);
   }

   return;
}

#ifdef PROTO
double TXTgwidth( Textitem *txtlist )
#else
double TXTgwidth( txtlist )
Textitem *txtlist;
#endif
/*
 *   TXTgwidth(): Function to calculate the untransformed horizontal text
 *                 extent of each substring and the entire text.
 *
 *                 IN: Pointer to the text list.
 *                 OUT: None.
 *                 RETURN VALUE: Untransformed horizontal text extent.
 *
 *                 Author: WKL/PLP
 *
 * ALGORITHM
 * ---------
 *
 * We presume "normal" character orientation and just add the widths,
 * and spacings, or return maximum width, depending on the text path.
 * As we go along, width of individual strings, or text substrings, are
 * stored in their text items.
 *
 * LOCALS
 * ------
 * cp
 * cwid
 * width
 *
 */

{
   float cwid, width;
   char *cp;

   width = 0.0;
   /* Walk through the list and process each character */
   while (txtlist)
   {
      switch (curatt.text_path)
      {
        case P_RIGHT :
        case P_LEFT  :
             /*
               For left and right paths, take total width, add spacing and
               multiply by number of characters in each string. Make
               ammendments for the last character of the last string
               later.
             */
             cwid = 0. ;
             /* Loop through the string */
             for(cp = txtlist->str; *cp!='\0'; cp++)
                cwid += txtlist->rwd[(int)(*cp)-32];
             cwid = (cwid * txtlist->char_exp +
                    strlen(txtlist->str) * txtlist->char_space)
                    * txtlist->char_height;

             /* Take out one spacing for the last string */
             if (txtlist->next == NULL)
                cwid -= txtlist->char_space * txtlist->char_height;

             /* Accumulate */
             width += cwid;
             break;
        case P_UP    :
        case P_DOWN  :
             /* For up and down, just return the maximum width */
             cwid = txtlist->rmaxwd*txtlist->char_exp * txtlist->char_height ;
             if (cwid>width)
                width = cwid;
             break;
      }

      /* Store each string's extent into the structure */
      txtlist->tbxwid = cwid;

      txtlist = txtlist->next;
   }
   return width;
}

#ifdef PROTO
double TXTgheight ( Textitem *txtlist )
#else
double TXTgheight ( txtlist )
Textitem *txtlist;
#endif
/*
 *   TXTgheight(): Function to calculate the untransformed vertical text
 *                 extent of each substring and the entire text.
 *
 *                 IN: Pointer to the text list.
 *                 OUT: None.
 *                 RETURN VALUE: Untransformed vertical text extent.
 *
 *                 Author: WKL/PLP
 *
 * ALGORITHM
 * ---------
 *
 * We presume "normal" character orientation and just add the heights,
 * and spacings, or return maximum height, depending on the text path.
 * As we go along, height of individual strings, or text substrings, are
 * stored in their text items.
 *
 * LOCALS
 * ------
 * cheight
 * height
 *
 */

{
   float height , cheight;

   height = 0.0;
   /* Walk through the list and process each character */
   while (txtlist!=NULL)
   {
      switch (curatt.text_path)
      {
        case P_RIGHT :
        case P_LEFT  :
             /* For left and right paths, just return the maximum height */
             cheight = (1 + txtlist->rtop + txtlist->rbot) *
                       txtlist->char_height;
             if (cheight>height)
                height = cheight;
             break;
        case P_UP    :
        case P_DOWN  :
             /*
               For up and down paths, take total height, add spacing and
               multiply by number of characters in each string. Make
               ammendments for the last character of the last string
               later.
             */
             cheight = (1 +txtlist->rtop + txtlist->rbot+ txtlist->char_space)
                       * strlen(txtlist->str);
             cheight *=  txtlist->char_height;

             /* Take out one spacing for the last string */
             if (txtlist->next==NULL)
                cheight -= txtlist->char_space * txtlist->char_height;
             /* Accumulate */
             height += cheight;
             break;
      }

      /* Store each string's extent into the structure */
      txtlist->tbxheight = cheight;

      txtlist = txtlist->next;
   }
   return height ;
}

#ifdef PROTO
void TXTxform( Point txtp, Tmatrix matrix )
#else
void TXTxform( txtp, matrix )
Point txtp;
Tmatrix matrix;
#endif
/*
 *    TXTxform(): Function to calculate the character orientation transform.
 *
 *                 IN: Alignment point coordinates.
 *                     Array name  of 3 by 2 doubles.
 *                 OUT: None (but called by reference).
 *                 RETURN VALUE: None.
 *
 *                 Author: WKL/PLP
 *
 * ALGORITHM
 * ---------
 *
 * We do 3 transformations:
 *    a. translate the alignment point to the origin.
 *    b. do any rotation and scaling requested.
 *    c. translate back.
 * Transformation matrix per each step is calculated and then the 3
 * matrices are concatenated. The resulting matrix parameters are stored
 * into the input array by reference.
 *
 *  Maintenance Log:
 *
 *  25 Jun 91 CDS  Cut out all the needless calculation.
 *  25 Jun 91 CDS  Documented Local variable usage.
 *  28 Feb 95 KEVP Add check for zero length orientation vectors and
 *                 use local copies of these vectors.
 *
 */

{
/*
 * LOCALS
 * ------
 */
  Vector ora,     /* character base vector */
         orb;     /* character up vector */
  double modua,   /* length of char base vector, used in calc of cosa & sina */
         modub,   /* length of char up vector, used in calc of cosb & sinb */
         sina,    /* sin of angle of base vector */
         sinb,    /* sin of angle of up vector */
         cosa,    /* cos of angle of base vector */
         cosb,    /* cos of angle of up vector */
         e1,      /* x-shift to take text pt to origin and back */
         f1;      /* y-shift to take text pt to origin and back */

/*
      resulting transformation = transform_1 * transform_2 * transform_3,
      where:
      transform_3 = translation of the alignment point to the origin.
      transform_2 = shearing
      transform_1 = translation of the alignment point back;
*/
      e1 = txtp.x;
      f1 = txtp.y;
/*
      get char_up, char_base and calculate the relevant angles.
*/
      if(cur.vdc_type == REAL)
      {
         ora.x = curatt.char_base.x.real;
         ora.y = curatt.char_base.y.real;
         orb.x = curatt.char_up.x.real;
         orb.y = curatt.char_up.y.real;
      }
      else
      {
         ora.x = curatt.char_base.x.intr;
         ora.y = curatt.char_base.y.intr;
         orb.x = curatt.char_up.x.intr;
         orb.y = curatt.char_up.y.intr;
      }

      modua = SQRT( ora.x*ora.x + ora.y*ora.y );
      modub = SQRT( orb.x*orb.x + orb.y*orb.y );

      if(REQUAL(modua,0.0))  /* Check for zero base vector */
      {
          CGMerror (func,ERR_CHBASEZERO,ERROR,NULLSTR);
          sina = 0.0;  cosa = 1.0;
      }
      else  /* Normalize base vector */
      {
         sina = ora.y / modua;
         cosa = ora.x / modua;
      }

      if(REQUAL(modub,0.0))  /* Check for zero up vector */
      {
          CGMerror (func,ERR_CHUPZERO,ERROR,NULLSTR);
          sinb = 1.0;  cosb = 0.0;
      }
      else  /* Noramalize up vector */
      {
         sinb = orb.y / modub;
         cosb = orb.x / modub;
      }


      /*** This is the real bit of the transformation,
              i.e the rotate, scale, skew etc           */
      matrix[0][0] = cosa;
      matrix[0][1] = sina;
      matrix[1][0] = cosb;
      matrix[1][1] = sinb;

      /*** This is the shift to, then from the origin.
           NOTE: use of negated e1 & f1, transformed by the above
                 part of the matrix to shift back to original position */
      matrix[2][0] = e1 - e1*cosa - f1*cosb;
      matrix[2][1] = f1 - e1*sina - f1*sinb;

#ifdef DEBUG
      { int i,j;
         fprintf(stderr, "Transformation Matrix:\n");
         for ( i=0; i<3; i++ )
         {
            for ( j=0; j<3; j++)
               fprintf(stderr, " %f", matrix[i][j] );
            fprintf(stderr,"\n");
         }
      }
#endif

      return;
}

/*
 *    TXTrestrict(): Function to manipulate the complete linked-list
 *                   text structure in order to reduce it's overall
 *                   width and height to be within those supplied.
 *
 *
 *                 IN:  Pointer to first Textitem
 *                      Maximum Width
 *                      Maximum Height
 *                      Pointer to text attribute structure
 *
 *                 OUT: None.
 *                 RETURN VALUE: TRUE if there is any text left after clipping,
 *                               FALSE otherwise ( The text list will have been
 *                               deallocated from memory, this may cause
 *                               problems to the calling routine.)
 *                 Author: MAS
 *
 * ALGORITHM
 * ---------
 *
 * There is more detail in the comments below, but basically an initial
 * attempt is made to scale the fonts down to the correct size, either by
 * direct scaling of character height or by picking a smaller size of the
 * same font. Should this not be enough, character spacing and/or expansion
 * are reduced (possibly to minimum). After that, several more passes of
 * (psuedo)scaling are made, and in the last resort, the text is clipped
 * with the right-most characters being lost.
 *
 * LOCALS
 * ------
 * items         (number of Textitems in the complete list.)
 * item          (pointer to an array holding the Textitems and a flag.)
 * curr_item     (loop counter, offset into 'item' array.)
 * guilty        (flags whether it is width/height/both that exceed.)
 * font
 * twidth        (current actual text width, returned from TXTgwidth.)
 * theight       (current actual text height, from TXTgheight.)
 * scale         (the scale factor required for restriction.)
 * chr_hgt
 * grain         (granularity, used to map VDCs to actual pixels, etc.)
 * method
 * tptr          (temporary pointer to a Textitem.)
 * changed       (flags a change during the scaling operation.)
 * restricted    (flags when restriction is achieved.)
 * pass          (loop counter)
 * func
 *
 */

#ifdef PROTO
Textitem *TXTrestrict( Textitem *txtlist, double rwidth, double rheight, struct textatt *ta)
#else
Textitem *TXTrestrict( txtlist, rwidth, rheight, ta)
Textitem *txtlist;
double rwidth, rheight;
struct textatt *ta;
#endif

{  int curr_item, pass, items, guilty, font;
   double twidth, theight;
   float scale, chr_hgt, grain;
   Enum method;
   Textitem  *tptr;
   Restxt_item  *item;
   Logical changed, restricted;
   char *func="TXTrestrict";

#define TEXT_FITS         0
#define TEXT_TOO_HIGH     1
#define TEXT_TOO_WIDE     2
#define TEXT_BOTH_TOO_BIG 3

/* Firstly, make sure that everything is drawn at 'STROKE' precision.
*/

   tptr=txtlist;
   while (tptr!=NULL)
   {   tptr->text_prec=2;
       tptr=tptr->next;
   }

/* Now check to see if current size already fits the restriction
   required, if this is true then do nothing but return now.
*/

   guilty=size_check(txtlist, &twidth, &theight, rwidth, rheight, &scale);
   if (guilty==TEXT_FITS) return txtlist;

/* We are going to create an array structures that contain information about
   each substring, but first we need to count how many there are
*/

   for (items=0,tptr=txtlist  ; (tptr!=NULL) ; items++)
       tptr=tptr->next;

/* Allocate enough memory to contain the array of 'res_text' structures.
*/

   item=(Restxt_item *) MALLOC(items, sizeof (struct res_text));

/* Now initialize the structure to contains two things, a pointer to the
   substring and a flag as to whether it is already at it's minimum size.
*/

   tptr=txtlist;
   for (curr_item=0; (curr_item<items) ; curr_item++)
   {   item[curr_item].ptr=tptr;
       item[curr_item].minimum=FALSE;
       tptr=tptr->next;
   }

/* Choose the 'grain' value - this is the mapping between character height
   in VDCs and in pixels/points (whatever is appropriate for the device.)
*/

   grain=ta->shared->ygrain;
   grain=(grain < ta->shared->xgrain) ? grain : ta->shared->xgrain;

/* What follows is the main bulk of the restriction code. The idea is to
   look at the current size of the text, and determine a scale factor by
   which to make it just fit the restriction box. We attempt to scale each
   substring bu this factor, either by direct scaling for scalable fonts,
   or for systems like 'X' that have fixed alternate sizes, choose an
   appropriate smaller size. Where this is not available, just choose the
   minimum that is. After a pass of all the substrings, check the current
   size again and repeat. There are 3 conditions that halt this...

     (1) The text now fits the restriction.
     (2) All the substrings are at minimum size (so go on to clip, etc.)
     (3) The maximum no. of passes have been made (a safety precaution
         to avoid infinite looping.)
*/

   for (pass=0; (pass<15) ; pass++)
   {

/* Calculate the scale factor required, but multiply it by 0.99 so that
   it always underestimates a little, this reduces the number of
   passes made.
*/

      changed=FALSE;
      restricted=TRUE;

      scale=calc_scale(item, items, rwidth, rheight);
      if (scale>0.999) break;


 /* For each item, call the routine which attempts the scale. If font ids,
   method, etc. change then load up the details of the new font.
*/

      for (curr_item=0; (curr_item<items); curr_item++)
      {   tptr=(item+curr_item)->ptr;
          if (tptr!=NULL)
          {

/* Get the current font, method and character height of this item.
*/
              method = tptr->text_method;
              font   = tptr->text_font;
              chr_hgt = tptr->char_height;

/* Attempt to scale it, and flag whether a change occurs.
*/

              if (FNTscale( &method, &font, &chr_hgt , scale, grain)==TRUE)
                 changed=TRUE;
              else
              {   restricted=FALSE;
                  (item+curr_item)->minimum=FALSE;
              }

              tptr->char_height = chr_hgt;

/* If the font or method changes, then we load up the new font to get it's details.
*/
              if ((font != tptr->text_font) || (method != tptr->text_method))
              {   tptr->text_method = method;
                  tptr->text_font   = font;
                  font_load(tptr);
              }
          }
      }

/* After the first pass ONLY, we also make an attempt to achieve restriction by
   reducing character spacing (and expansion if appropriate) that is larger than
   normal back to a size that will just fit, or in the worst case, minimum.
   Horizontal text paths may use spacing or expansion, vertical ones can only
   use spacing.
*/
      if (restricted==TRUE) break;

      if (pass==0)
      {
         guilty=size_check(txtlist, &twidth, &theight, rwidth, rheight, &scale);
         switch (curatt.text_path)
         {

           case P_UP   :
           case P_DOWN :
                          if (guilty&TEXT_TOO_HIGH)
                          {  restricted=scale_spc_exp(txtlist, theight, rheight, TRUE);
                             if ((guilty==TEXT_TOO_HIGH)&&(restricted==TRUE)) restricted=TRUE;
                          }
                break;

           case P_RIGHT:
           case P_LEFT :
                         if (guilty&TEXT_TOO_WIDE)
                         {  restricted=scale_spc_exp(txtlist, twidth, rwidth, FALSE);
                            if ((guilty==TEXT_TOO_WIDE)&&(restricted==TRUE)) restricted=TRUE;
                         }
                break;
          }
      }

      if ((changed==FALSE)||(restricted==TRUE)) break;
   }

/* Deallocate the temporary structures, now that we're finished with them.
*/

   FREE(item);

/* If the restricted flag was set somewhere in the previous loop, then we may exit now.
*/

   if (restricted==TRUE) return txtlist;

/* Otherwise, a last check that restriction has been achieved.
*/

   guilty=size_check(txtlist, &twidth, &theight, rwidth, rheight, &scale);
   if (guilty==TEXT_FITS) return txtlist;

/* Last resort is to clip the text to fit the box - this involves clipping
   the individual characters if possible, but if at least one character is
   too big in the direction perpendicular to the text path then the only
   solution is to clip all of the text. In doing so, the Textitems are
   deallocated from memory, this is the reason why we return a FALSE if all
   items are removed, so that the routines calling us know not to do anything
   more with that Text structure.
*/

/* Note that there is another possibility left open, as well as choosing
   smaller sizes of the same font, we could also choose another font (or
   even font method), but this requires that you know what fonts look like
   relative to each other. This would probably entail setting up a table
   of alternative fonts for each font, which would be especially complex
   for a system like 'X', because that takes it's info direct from the
   server rather than a method file, and it's rare that the font set up
   on any two servers is the same.
*/

   return (clip_text(txtlist, rwidth, rheight, guilty));
}




/*
 *    TXTcopy():   Function to make a copy of a whole linked list
 *                 text structure, so that the original can then
 *                 be altered safely.
 *
 *                 IN:  Pointer to the first Textitem structure.
 *                 OUT: None.
 *                 RETURN VALUE: Pointer to the head of the new copy.
 *
 *                 Author: MAS
 *
 * ALGORITHM
 * ---------
 *
 * Allocate memory needed for one Textitem structure. Link the structure
 * into the list if necessary, and fill-in with available information
 * (text attributes and font data).
 *
 * LOCALS
 * ------
 * head
 * tail
 * new
 * func
 */

#ifdef PROTO
Textitem *TXTcopy( Textitem *txtlist )
#else
Textitem *TXTcopy( txtlist )
Textitem *txtlist;
#endif

{
   Textitem *head, *tail;
   Textitem *new;
   char *func="TXTcopy";

   head=NULL;
   tail=NULL;

   while (txtlist!=NULL)
   {
      new=(Textitem *)MALLOC( 1, sizeof(Textitem) );

      if (new==NULL)
      exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));

/* Copy the whole structure.
*/
      *new = *txtlist;
      new->next=NULL;

/* Reserve memory for, and copy, the string pointed to by
   this Textitem.
*/
      new->str=(char *)MALLOC (1,(strlen(txtlist->str)+1) );
      if (new->str==NULL)
      exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));

      (void)strcpy(new->str,txtlist->str);

/* Link into list correctly.
*/
      if (head==NULL)
      {
          head=new;
          tail=new;
      }
      else
      {
         tail->next=new;
         tail=new;
      }
      txtlist = txtlist->next;
   }

   return head;
}



     /*-----------------------------------------------------------------*
      |                                                                 |
      |      scale_spc_exp()                                            |
      |                                                                 |
      |      Evaluates and scales character spacing (and expansion)     |
      |        IN:  Address of first Textitem,                          |
      |             Current size in direction to be restricted          |
      |             Restricted size required                            |
      |             A logical flag, TRUE if text path is vertical,      |
      |                FALSE if it is horizontal.                       |
      |        OUT: None.                                               |
      |        RETURN: TRUE if text has been restricted,                |
      |                FALSE otherwise.                                 |
      |                                                                 |
      *-----------------------------------------------------------------*/


#ifdef PROTO
static Logical scale_spc_exp( Textitem *txtlist, double tsize, double rsize, Logical isvertical)
#else
static Logical scale_spc_exp( txtlist, tsize, rsize, isvertical)
Textitem *txtlist;
double tsize, rsize;
Logical isvertical;
#endif

{
   Textitem *tptr;
   float ssize,esize,temp;
   char *cp;
   int nc;
   double scale;
   Logical restricted;

   ssize = esize = 0.0;
   restricted=TRUE;
   tptr=txtlist;

   while (tptr!=NULL)
   {

/* Sum up size due only to excess spacing
*/
          if (tptr->char_space > 0.0 )
          {  nc=strlen(tptr->str);
             if (tptr->next==NULL) --nc;
             ssize += nc * tptr->char_space * tptr->char_height;
          }

/* Sum up size due only to excess expansion (only if
   a horizontal text path.)
*/
          if ((tptr->char_exp > 1.0) && (isvertical==FALSE))
          {   temp=0.0;
              for(cp = tptr->str; *cp!='\0'; cp++)
              temp  += tptr->rwd[(int)(*cp)-32];
              esize += temp * (tptr->char_exp -1.0) * tptr->char_height;
          }

          tptr = tptr->next;
   }

/* Evaluate scaling to apply.
*/
  if (isvertical==FALSE) scale=1.0-((tsize-rsize)/(ssize+esize));
                    else scale=1.0-((tsize-rsize)/ssize);

/* If it's less than zero, scale to zero but flag
   restriction not achieved.
*/
  if (scale<0.0)
     {   scale=0.0;
         restricted=FALSE;
     }

/* Now apply the calculated scale factor to each item with
   excess spacing/expansion.
*/
  tptr = txtlist;
  while (tptr!=NULL)
  {
      if (tptr->char_space > 0.0)
         tptr->char_space *= scale;

      if ((tptr->char_exp > 1.0)&&(isvertical==FALSE))
         tptr->char_exp = ((tptr->char_exp-1.0)*scale)+1.0;
      tptr = tptr->next;
  }
  return (restricted);
}


     /*-----------------------------------------------------------------*
      |                                                                 |
      |      clip_text()                                                |
      |                                                                 |
      |      clips text at the character that overflows the size box    |
      |      given, deallocating the extra text that is clipped.        |
      |        IN:  Address of first Textitem,                          |
      |             Width & height of restriction box                   |
      |             The 'guilty-party' returned from size_check         |
      |        OUT: None                                                |
      |        RET: Pointer to first item, or NULL if it's all been     |
      |             clipped.                                            |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static Textitem *clip_text( Textitem *txtlist, double rwidth, double rheight, int guilty)
#else
static Textitem *clip_text( txtlist, rwidth, rheight, guilty)
Textitem *txtlist;
double rwidth, rheight;
int guilty;
#endif


{
   Textitem *tptr=txtlist, *prev_tptr=NULL;
   Logical clip=FALSE;
   float size,size2;
   char *cp;

   size = 0.0;

   while ((tptr!=NULL)&&(clip==FALSE))
   {
      cp= tptr->str;
      switch (curatt.text_path)
      {
        case P_RIGHT :
        case P_LEFT  :

/* If text path horizontal & text is too high, clip EVERYTHING.
*/
             if (guilty&1)
             {   clip=TRUE;
                 break;
             }
/* Otherise, sum up width until it exceeds the restriction width.
*/
             for(; *cp!='\0'; cp++)
             {
                 if (tptr->next !=NULL)
                    {   size2 = (((tptr->rwd[(int)(*cp)-32])* tptr->char_exp) +
                                tptr->char_space);
                    }
                 else
                    {   size2 = (tptr->rwd[(int)(*cp)-32])* tptr->char_exp;
                        if (*(cp+1)!='\0') size2+=tptr->char_space;
                    }
                 size += size2*tptr->char_height;
                 if (size>rwidth)
                    {   clip=TRUE;
                        break;
                    }
             }
             break;

        case P_UP    :
        case P_DOWN  :

/* If text path vertical & text is too wide, clip EVERYTHING.
*/
            if (guilty&2)
             {   clip=TRUE;
                 break;
             }

/* Otherise, sum up height until it exceeds the restriction height.
*/
             for(; *cp!='\0'; cp++)
             {
                 size+=(1 +tptr->rtop + tptr->rbot+ tptr->char_space)
                        * tptr->char_height;
                 if (size>rheight)
                    {   clip=TRUE;
                        break;
                    }
             }
             break;
      }

      if (clip==FALSE)
      {  prev_tptr=tptr;
         tptr = tptr->next;
      }
   }

   if (clip==TRUE)
      {   if (cp==tptr->str)
             {   if (prev_tptr==NULL)
                    {   TXTfree(txtlist);  /* All text clip */
                        return NULL;
                    }
                 else
                    {   prev_tptr->next=NULL;
                        TXTfree(tptr);
                        return txtlist;   /* Whole item clip */
                    }
             }
          else                            /* Partial item clip */
             {   *cp='\0';
                 if (tptr->next != NULL) TXTfree(tptr->next);
                 tptr->next=NULL;
                 return txtlist;
             }
      }
   else return txtlist;
}


     /*-----------------------------------------------------------------*
      |                                                                 |
      |      font_load()                                                |
      |                                                                 |
      |      Calls the appropraite function to update the information   |
      |      held within a Textitem about the font it is using.         |
      |        IN:  Address of Textitem                                 |
      |        OUT: None  RETURN: None                                  |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void font_load( Textitem *txtlist)
#else
static void font_load( txtlist )
Textitem *txtlist;

#endif

{
   switch (txtlist->text_method)
   {
#ifdef XW
      case HDW:
        Xw_getfd(txtlist);
      break;
#endif
#ifdef HERSHEY
      case HER:
         HERgetfd(txtlist);
      break;
#endif
#ifdef BEZIER
      case BEZ:
         BEZgetfd(txtlist);
      break;
#endif
#ifdef POSTSCRIPT
      case PS:
         PSgetfd(txtlist);
      break;
#endif
#ifdef TEX
      case TEX:
          /* awaiting decision */
      break;
#endif
      default:
          /* Should do same thing as for hardware!
             But for the moment, tell people
             we didn't find a valid textstyle!!         */
      (void) CGMerror(func,ERR_TXTSYSNFND,ERROR,NULLSTR);
      break;
   }
}




     /*-----------------------------------------------------------------*
      |                                                                 |
      |      size_check()                                               |
      |                                                                 |
      |      Evaluates the current width & height of all text held in   |
      |      the text structure, compares it against the given width    |
      |      and height and calculates the scale factor needed to       |
      |      reduce the current size to the size given.                 |
      |        IN:  Address of first Textitem                           |
      |        OUT: Current width & height of the text.                 |
      |        RET: An integer 'guilty party' - reflects whether it is  |
      |             width, height or both that are too big              |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static int size_check( Textitem *txtlist, double *width, double *height, double rwidth, double rheight,
                       float *scale)
#else
static int size_check( txtlist, width, height, rwidth, rheight, scale)
Textitem *txtlist;
double *width, *height, rwidth, rheight;
float *scale;
#endif

{   double scale1,scale2;
    *width=TXTgwidth(txtlist);
    *height=TXTgheight(txtlist);
    scale1=rwidth/ *width;
    scale2=rheight/ *height;
    *scale=(scale1 < scale2)? scale1 : scale2;
    if (rwidth>=*width)
       return (rheight>=*height) ?       TEXT_FITS : TEXT_TOO_HIGH ;
    else return (rheight>=*height) ? TEXT_TOO_WIDE : TEXT_BOTH_TOO_BIG ;
}





     /*-----------------------------------------------------------------*
      |                                                                 |
      |      calc_scale()                                               |
      |                                                                 |
      |      Evaluates the scale factor required to reduce the width    |
      |      and height of the text list supplied to the restricted     |
      |      values given, taking into account that some of the Text    |
      |      items may already be at minimum size.                      |
      |        IN:  Address of the array holding Textitem information   |
      |             Number of items in that array.                      |
      |             Width & height to restrict to.                      |
      |        OUT: None.                                               |
      |        RETURN: Scale factor to apply to character height        |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static double calc_scale( Restxt_item *item, int items, double rwidth, double rheight)
#else
static double calc_scale( item, items, rwidth, rheight)
Restxt_item *item;
double rwidth, rheight;
int items;
#endif

{  double scale1,scale2;
   Textitem *txtlist;
   float size, cwid, cheight;
   char *cp;
   int i;

   size = 0.0;

/* Evaluate the scale factor needed to reduce width.
*/

   for ( i=0 ; (i<items) ; i++)
   {
      txtlist=(item+i)->ptr;
      switch (curatt.text_path)
      {
        case P_RIGHT :
        case P_LEFT  :

             cwid = 0.0 ;

/* Evaluate the width of this item */

             for(cp = txtlist->str; *cp!='\0'; cp++)
                cwid += txtlist->rwd[(int)(*cp)-32];
             cwid = (cwid * txtlist->char_exp +
                    strlen(txtlist->str) * txtlist->char_space)
                    * txtlist->char_height;

/* Ignore spacing on the end of the last string.
*/
             if (txtlist->next == NULL)
                cwid -= txtlist->char_space * txtlist->char_height;

/* If this item is minimum size, subtract it from the restriction
   width, otherise add it to the current width.
*/
             if ( (item+i)->minimum==TRUE) rwidth -= cwid;
             else                     size += cwid;

             break;

        case P_UP    :
        case P_DOWN  :


/* Take the width to be the largest width of any single item.
*/
             cwid = txtlist->rmaxwd*txtlist->char_exp * txtlist->char_height ;
             if (cwid>size)
                size = cwid;
             break;
      }
   }

/* The scale factor for width =
     (Restricted width required - Width of all minimum sized items) /
                    (Width of all non-minimum sized items.)
*/

   scale1= (rwidth/size);


/* Now evaluate scale factor for height.
*/

   size = 0.0;
   for ( i=0 ; (i<items) ; i++)
   {
      txtlist=(item+i)->ptr;
      switch (curatt.text_path)
      {
        case P_RIGHT :
        case P_LEFT  :

/* For horizontal paths, take largest height.
*/
             cheight = (1 + txtlist->rtop + txtlist->rbot) *
                       txtlist->char_height;
             if (cheight>size)
                size = cheight;
             break;

        case P_UP    :
        case P_DOWN  :

/* Vertical paths sum up height of each character.
*/
             cheight = (1 +txtlist->rtop + txtlist->rbot+ txtlist->char_space)
                       * strlen(txtlist->str);
             cheight *=  txtlist->char_height;

/* Ignore spacing on the end of the last string.
*/
             if (txtlist->next==NULL)
                cheight -= txtlist->char_space * txtlist->char_height;

/* If this item is minimum size, subtract it from the restriction
   height, otherise add it to the current height.
*/
             if ( (item+i)->minimum==TRUE) rheight -=cheight;
             else                     size += cheight;

             break;
      }
   }

/* The scale factor for height  =
      (Restricted height required - Height of all minimum sized items) /
                     (Height of all non-minimum sized items.)
*/
   scale2= (rheight/size) ;

/* Choose the minimum of the two scale factors and return it.
*/
   return (scale1 < scale2) ? scale1 : scale2;
}
