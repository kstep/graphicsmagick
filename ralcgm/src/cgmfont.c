/*   RAL-CGM Interpreter module:  @(#) cgmfont.c  version 3.9
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
 *   Date:       7 - 26 August 1990
 */

/*
 *   This module contains utilities that handle character sets and
 *   fonts according to the formalism of ISO standards 646, 2022, 2375
 *   and 9541.  Since ISO 9541 is as yet not a standard,
 *   things may change.
 *
 *   The module provides the following entry points:
 *
 *   FNTinit()
 *
 *      initializes the whole system, character sets and fonts;
 *
 *   FNTclose()
 *
 *      closes the whole system down, freeing all storage
 *
 *   FNTXlist(argc,argv)
 *
 *      initializes the RAS (hardware) list of implementation
 *      fonts from the list extracted by an X-windows client from
 *      the server.
 *
 *   FNTflist(count, ints,string,list)
 *
 *      unpacks the multi-string in `string' (as constructed by each of
 *      the input modules) and stores the result as fontlist `list';
 *
 *   FNTclist(count, ints,string,list)
 *
 *      unpacks the multi-string in `string' (as constructed by each of
 *      the input modules) and stores the result as character set `list';
 *
 *   FNTmatch(textatt,available,methods,fonts)
 *
 *      looks for a suitable method and font for each of the two
 *      character set indices and the single font index;
 *
 *   FNTdisplay()
 *
 *      provides a readable dump of the whole font and character set
 *      database.
 *
 *   FNTscale()
 *
 *      Scale fonts
 *
 *   Maintenance Log
 *   ===============
 *
 *  17 Aug 90 CDO  - all routines renamed in line with ralcgm naming
 *                   conventions;
 *                 - cgmtext.h added to includes;
 *                 - cgmferrs.h removed from includes and replaced by
 *                   cgmerr.h;
 *
 *  25 Aug 90 CDO  - input system complete; searching system still needs
 *                   work on it;
 *
 *  28 Aug 90 CDO  - added FNTXlist to allow the import of X-windows font
 *                   data into the system
 *
 *   3 Sep 90 CDO  - added FNTclose to free off all storage, so the system
 *                   can be well behaved under cgmlib;
 *                 - added checking of all malloc and callocs to ensure that
 *                   space was obtained;
 *                 - added checking of the limits of all dynamic arrays to
 *                   ensure that no more values were read in than there
 *                   was space for
 *  22 Oct 90 NMH  - added sorting of font list from X server
 *
 *  12 Nov 90 CDO  - removed temporary code for TESTXWF
 *                 - fixed duplicate use of 'i' variable in character
 *                   set list processing
 *                 - duplicate use of 'i' variable in FNTXlist fixed
 *                   (only a line warning, but confusing)
 *                 - when no CGM font list is given, first font from
 *                   first preferred method is returned, not 0.
 *                 - same logic applied if font index is out of range;
 *                   it could be argued that font index 1 should be
 *                   used instead, or that the font should not be
 *                   changed
 *                 - attempt to generate FNTmatch list from NMH's sorted
 *                   list
 *  13 Nov 90 RTP  Make FNTXlist conditional on XW
 *  26 Nov 90 NMH  FNTXlist now picking up X font list from server
 *  26 Nov 90 RTP  Add ANSI prototypes
 *                 Add check_size to check height for raster fonts
 *                 Add DMESS macro for error messages
 *                 Add different levels of DEBUG, remove debuglevel
 *   3 Dec 90 RTP  Return correct fontid from match_attrib
 *                 also add height check to match_attrib
 *                 Change request height to 'long'
 *                 Equate '-' to ' ' in match_string
 *   4 Dec 90 RTP  Ignore trailing spaces in match_string
 *  14 Jan 91 RTP  Set Default value for special fonts
 *                 Set charset for special fonts
 *                 Remove old matching algorithm
 *  14 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change calloc calls to use MALLOC and cast to (size_t)
 *  20 May 91 RTP  Replace free() with Macro FREE
 *   2 Aug 91 RTP  Add SCCS id
 *   5 Sep 91 RTP  Change match_substring to look for trailing space
 *  11 Sep 91 CDS  Changed to use utility specific header CGMUFONT.H
 *   4 Aug 92 MAS  Add FNTscale routine
 *  11 Aug 92 RTP  Correct structure in DEBUG statement
 *  27 Nov 92 CDS  Changed ifdef ANSI to ifdef PROTO for function prototypes
 *   8 Dec 92 CDS  (For NMH) Added correction to total number of fonts
 *                 if an invalid font is found.
 *  22 Sep 93 CDS  Corrected use of %d to read long ints in multiple places
 *  27 Oct 93 RTP  Change malloc to MALLOC throughout
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *  25 Jan 95 PMG  Add direct handling of Bezier and Hershey fonts by number
 */

#define CGMFONT_C

#include "cgmout.h"
#include "cgmfile.h"
#include "cgmatt.h"
#include "cgmufont.h"

struct request
{
   Index              cset[2];
   int                familyid;
   struct isovariant  var;
   struct isodsngroup dsn;
   long               height;
   Enum               rotation;
};

struct result
{
   Enum method[2];
   Index font[2];
};


#ifdef PROTO
static void proc_csets( void ),
            proc_typefaces( void ),
            proc_families( void ),
            proc_aliases( void ),
            proc_sfonts( void ),
            proc_sources( void ),
            proc_designs( void ),
            proc_reqs( void ),
            proc_methods( void );

static void read_rest( FILE*, char*, int );
static Index check_size( struct textatt*, Enum, struct isofont** );
static Logical strip_font( char*, char* ),
               match_string( char*, char* ),
               match_substring( char*, char* ),
               match_attrib( struct request, Enum*, struct result* );

#else
static void proc_csets(),
            proc_typefaces(),
            proc_families(),
            proc_aliases(),
            proc_sfonts(),
            proc_sources(),
            proc_designs(),
            proc_reqs(),
            proc_methods();

static void read_rest();
static Index check_size();

static Logical match_string(),
               match_substring(),
               match_attrib(),
               strip_font();
#endif

#ifdef FONTDISPLAY
#ifdef PROTO
static void disp_csets( void ),
            disp_fonts( void ),
            disp_isocsets( void ),
            disp_fudgesets( void ),
            disp_families( void ),
            disp_aliases( void ),
            disp_sfonts( void ),
            disp_sources( void ),
            disp_designs( void ),
            disp_rfonts( void ),
            disp_mfonts( void );
#else
static void disp_csets(),
            disp_fonts(),
            disp_isocsets(),
            disp_fudgesets(),
            disp_families(),
            disp_aliases(),
            disp_sfonts(),
            disp_sources(),
            disp_designs(),
            disp_rfonts(),
            disp_mfonts();
#endif
#endif

static struct isocharset **csets;
static int                ncsets;

static struct fudgecset  **fsets;
static int                nfsets;

static struct family     **families;
static int                nfamilies;

static struct alias      **aliases;
static int                naliases;

static struct font       **sfonts;
static int                nsfonts;

static struct source     **sources;
static int                nsources;

static struct design     **designs;
static int                ndesigns;

static struct font       **rfonts;
static int                nrfonts;

#define METHODS MAXMETHOD+1

static struct isofont    **mfonts[METHODS];
static int                nmfonts[METHODS];
static char                *mname[METHODS];

#define NUMWEIGHTS 11

static struct modifier

     weights[] = {
                   1, " Ultra Light",
                   2, " Extra Light",
                   4, " Demi Light",
                   4, " Semi Light",
                   3, " Light",
                   9, " Ultra Bold",
                   8, " Extra Bold",
                   6, " Demi Bold",
                   6, " Semi Bold",
                   7, " Bold",
                   5, " Medium"
                 };

#define NUMWIDTHS  12

static struct modifier

     widths[] =  {
                   1, " Ultra Condensed",
                   2, " Extra Condensed",
                   4, " Demi Condensed",
                   4, " Semi Condensed",
                   3, " Condensed",
                   3, " Narrow",
                   9, " Ultra Expanded",
                   8, " Extra Expanded",
                   6, " Demi Expanded",
                   6, " Semi Expanded",
                   7, " Expanded",
                   5, " Medium"
                 };

#define NUMVARIANTS 4

static struct modifier

     variants[]= {
                   2, " Oblique",
                   3, " Back Slant Oblique",
                   4, " Italic",
                   5, " Back Slant Italic"
                 };

#define RAS    HDW   /* alternative definition of hardware method */

static Logical started = FALSE;

static char *fntext = "fnt";

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      FNTinit()                                                  |
      |                                                                 |
      |      Initialize the font and character set system.              |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
void FNTinit( void )
#else
void FNTinit()
#endif

{
   if (started) return;

#ifdef DEBUG
   DMESS "Initializing font system\n");
#endif

   proc_csets();
   proc_typefaces();
   proc_reqs();
   proc_methods();

#ifdef DEBUG
   DMESS "Font system initialized\n");
#endif
   started = TRUE;
}


     /*-----------------------------------------------------------------*
      |                                                                 |
      |      FNTclose()                                                 |
      |                                                                 |
      |      Close down the font system.                                |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
void FNTclose( void )
#else
void FNTclose()
#endif

{
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      FNTflist()                                                 |
      |                                                                 |
      |      Store a font                  list as provided in the      |
      |      CGM header in a form that can be used later on.            |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
void FNTflist( long *inthead, long *ints, char *string, struct cgmfont *list)
#else
void FNTflist(inthead, ints, string, list)

long *inthead, *ints;
char *string;
struct cgmfont *list;
#endif
{
   int i;
   long l= *ints, *pi;
   char *ps, *pms, *func = "FNTflist";

   list->count = inthead - ints - 1;
#ifdef DEBUG
   DMESS "Count given is %d\n",list->count);
   for (i=0,pi=ints; i<list->count; i++,pi++)
      DMESS "Offset %d string %s\n",(i==0?0:*pi),string+(i==0?0:*pi));
#endif

     /*  Now allocate the space for the string and, separately,
         the space for the pointers to the substrings.
     */

   pms = (char*) MALLOC (1, l+1 );
   if (pms == (char *) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"flist (name list)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }

   list->names = (char **) MALLOC( list->count, sizeof(ps) );
   if (list->names == (char **) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"flist (structure)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }

   for (i=0,ps=string,pi=ints+1; i<list->count; i++,pi++)
   {
      strcpy(pms,ps);
      list->names[i] = pms;
      pms += strlen(ps) + 1;
      ps = string + *pi;
   }
   return;
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      FNTclist()                                                 |
      |                                                                 |
      |      Store a character set         list as provided in the      |
      |      CGM header in a form that can be used later on.            |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
void FNTclist( long *inthead, long *ints, char *string, struct cgmcset *list)
#else
void FNTclist(inthead, ints, string, list)

long *inthead, *ints;
char *string;
struct cgmcset *list;
#endif

{  int i;
   long  l= *ints, *pi;
   char *ps, *pms, *func = "FNTclist";

   list->count = (inthead-ints-1) / 2;
#ifdef DEBUG
   DMESS "Count given is %d\n",list->count);
   for (i=0,pi=ints; i<list->count; i++,pi+=2)
      DMESS "Offset %d Type %d String %s\n",
              (i==0?0:pi[0]), pi[1], string+(i==0?0:pi[0]) );
#endif

     /*  Now allocate the space for the string and, separately,
         the space for the pointers to the substrings.
     */

   pms = (char *) MALLOC (1, l+1 );
   if (pms == (char *) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"clist (name list)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }

   list->types = (int *) MALLOC( list->count, sizeof(int) );
   if (list->types == (int *) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"clist (type list)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }

   list->names = (char **) MALLOC( list->count, sizeof(ps) );
   if (list->names == (char **) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"clist (structure)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }

   for (i=0,ps=string,pi=ints+1; i<list->count; i++)
   {  strcpy(pms,ps);
      list->names[i] = pms;
      list->types[i] = *pi++;
      pms += strlen(ps) + 1;
      ps = string + *pi++;
   }
}


#ifdef XW
     /*-----------------------------------------------------------------*
      |                                                                 |
      |      FNTXlist()                                                 |
      |                                                                 |
      |      Initialize the HDW (hardware) list of implementatione      |
      |      fonts from the data provided when an X-windows client      |
      |      requests full font information from its server.            |
      |                                                                 |
      *-----------------------------------------------------------------*/
#ifdef PROTO
void FNTXlist( int argc, char **argv )
#else
void FNTXlist( argc, argv )

int argc;
char **argv;
#endif

{  int i;
   Index xfontid=0;
   char *func = "FNTXlist";
   int num_generic, num_total=argc;
   FNTXnames *pname, *new_pname;  /* for setting list of members of generic */
   FNTXfont_list *pxflp = NULL,   /* pointer to current generic font */
                 *pxflh,          /* pointer to head of list of generic fonts */
                 *pxflt,          /* pointer to tail of list of generic fonts */
                 *pxfls;          /* pointer for searching generic list */

    struct isofont *isofontp;
    struct isofont **pp;

     /*   We are passed an array of string, identical in structure to the
      |   fcount,flist arrangement used into C programs.  Each string is of
      |   the format:
      |
      |   -foundry-family-weight-style-width-styleid-pixelsize-pointsize...
      |   -xresolution-yresolution-spacing-avwidth-encoding
      |
      |   where the fine detail of the way in which this is recorded is
      |   detailed below.
      */

     /*  Process each string in turn, accumulating the results in the HDW
      |  (hardware) slot in the available fonts part of the font system.
      */

   if (!started) FNTinit();

   for (i=0; i<argc; i++)

   {  char *fontstring = argv[i], *string, **pstart, *start[14], *gen_name;
      int j, minuscount= 0, n;
      char *source,
           *family,
           *weight,
           *style,
           *width,
           *styleid,
           *pixelsize,
           *pointsize,
           *xresolution,
           *yresolution,
           *spacing,
           *avwidth,
           *encoding;
      int  zpixelsize = 0,
           zpointsize = 0,
           zxresolution = 0,
           zyresolution = 0,
           zavwidth = 0;
      char zstyle = 'R',
           zspacing = 'P';
      int lstr,isrc,ifam;
      struct isofont *pisof;

      string = (char*) MALLOC (1, strlen(fontstring)+1 );
      if (string == (char *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,ERROR,"X font list (string)");
         break;
      }

      strcpy(string,fontstring);
      lstr = strlen(string);

      for (j=minuscount=0,pstart=start; j<lstr; j++)
         if (string[j] =='-')
         {  minuscount++;
            if (minuscount > 13) break;
            string[j] = '\0';
            *pstart++ = string+j+1;
         }

      if (minuscount < 13)
      {  (void) CGMerror(func,ERR_XFONTLIST,ERROR,fontstring);
         continue;
      }

      source =      start[0];
      family =      start[1];
      weight =      start[2];
      style =       start[3];
      width =       start[4];
      styleid =     start[5];
      pixelsize =   start[6];
      pointsize =   start[7];
      xresolution = start[8];
      yresolution = start[9];
      spacing =     start[10];
      avwidth =     start[11];
      encoding =    start[12];

#ifdef DEBUG
      DMESS "X font (%s) %s\n",source,family);
      DMESS ".. weight %s style %s width %s style id %s\n",
             weight,style,width,styleid);
      DMESS ".. pixel size %s point size %s resolutions %s,%s\n",
             pixelsize, pointsize, xresolution, yresolution);
      DMESS ".. spacing %s average width %s encoding %s\n",
             spacing, avwidth, encoding);
#endif

      n = sscanf(pixelsize,"%d",&zpixelsize);
      n += sscanf(pointsize,"%d",&zpointsize);
      n += sscanf(xresolution,"%d",&zxresolution);
      n += sscanf(yresolution,"%d",&zyresolution);
      n += sscanf(avwidth,"%d",&zavwidth);

      if (n != 5)
      {  (void) CGMerror(func,ERR_XFONTLIST,ERROR,string);
         num_total--;
         continue;
      }

      n = 0;
      if (strlen(style) == 1)
         zstyle = toupper(style[0]);
      else
         n++;

      if (strlen(spacing) == 1)
         zspacing = toupper(spacing[0]);
      else
         n++;

      if (zstyle != 'R' &&
          zstyle != 'I' &&
          zstyle != 'O') n++;

      if (zspacing != 'M' &&
          zspacing != 'P' &&
          zspacing != 'C') n++;

/*
 store the derived info in an isofont structure
*/
      pisof = (struct isofont *)MALLOC( 1, sizeof(struct isofont) );
      if (pisof == (struct isofont *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL," (isofont structure)");
         exit(0);
      }
      pisof->fontid=xfontid++;
      pisof->fontname=family;
      pisof->dsnsource = (struct source *) NULL;
/* #ifdef SRCNAMES */
      for (isrc=0; isrc<nsources; isrc++)
      {  struct source *psrc = sources[isrc];
#ifdef DEBUG
         DMESS "Trying to matching source %s with %s\n",
                    source, psrc->name);
#endif
         if (match_string(source,psrc->name))
         {  pisof->dsnsource = psrc;
            break;
         }
      }
/* #endif */
      pisof->fontfamily = family;
      pisof->familyid=0;
      for (ifam=0; ifam<nfamilies;ifam++)
      {  struct family *pfam = families[ifam];
#ifdef DEBUG
         DMESS "Trying to match family %s to %s\n",
                  family, pfam->name);
#endif
         if (match_string(family,pfam->name))
         {  pisof->familyid = pfam->number;
            break;
         }
      }
      pisof->variant.posture= (zstyle == 'R' ? 1 :
                              zstyle == 'I' ? 4 :
                              zstyle == 'O' ? 2 : 0);
      pisof->variant.weight = (match_string(weight,"book") ? 4 :
                              match_string(weight,"demi") ? 6 :
                              match_string(weight,"medium") ? 5 :
                              match_string(weight,"bold") ? 7 :
                              match_string(weight,"light") ? 3 : 5);
      pisof->variant.propwidth = (match_string(width,"normal") ? 5 :
                                 match_string(width,"wide") ? 7 :
                                 match_string(width,"double wide") ? 9 :
                                 match_string(width,"narrow") ? 3 : 5);
      pisof->variant.structure = 1;
      pisof->dsngroup.class = 2;
      pisof->dsngroup.subclass = 4;
      pisof->dsngroup.group = 1;
      pisof->dsnsize = zpixelsize;
      pisof->minsize = zpixelsize;
      pisof->maxsize = zpixelsize;
      pisof->cset = 1;

/*
  determine generic name of font for use in temporary font list
*/
      gen_name = (char *) MALLOC (1, strlen(family)+5 );
      if(sprintf(gen_name,"%s%c%c%c%c",family,toupper(weight[0]),zstyle,
                toupper(width[0]),toupper(width[1])) ==0) n++;
#ifdef DEBUG
      DMESS "gen_name:%s\n",gen_name);
#endif

      if (n > 0)
      {  (void) CGMerror(func,ERR_XFONTLIST,ERROR,string);
         continue;
      }


/*
first font ??
*/
      if (pxflp == NULL)
      {  pxflp = (FNTXfont_list *)MALLOC( 1, sizeof(FNTXfont_list) );
/*
Exit if insufficient memory
*/
         if (pxflp==NULL)
         {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"(font_list structure)");
            exit(0);
         }
/*
set up first list entry
*/
         pxflp->pnext=NULL;
         pxflp->gen_name=gen_name;
         pxflp->num_siz=1;
         pname=(FNTXnames *)MALLOC( 1, sizeof(FNTXnames) );
/*
Exit if insufficient memory
*/
         if (pname==NULL)
         {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"names structure)");
             exit(0);
         }
         pxflp->names=pname;
         pname->next=NULL;
         pname->prev=NULL;
         pname->isobits= pisof;
         num_generic=1;
         pxflh=pxflt=pxflp;
      }
      else
/*
 same generic name as last time
*/
      if (!(strcmp(gen_name,pxflp->gen_name)))
      {  new_pname=(FNTXnames *)MALLOC( 1, sizeof(FNTXnames) );
/*
  Exit if insufficient memory
*/
         if (new_pname==NULL)
         {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"(names structure)");
            exit(0);
         }
         pname->next=new_pname;
         new_pname->prev=pname;
         pname=new_pname;
         pname->next=new_pname=NULL;
         pname->isobits= pisof;
         pxflp->num_siz++;
      }
      else
/*
 different generic name from last time
*/
      {  if(num_generic==1)
            pxfls=NULL;
         else
/*
 try matching generic name againts those already stored
*/
            for(pxfls=pxflh; pxfls!= NULL; pxfls=pxfls->pnext)
            {  if(!strcmp(gen_name,pxfls->gen_name))
/*
 found a match
*/
               {  pxflp=pxfls;
                  new_pname=(FNTXnames *)MALLOC(1,sizeof(FNTXnames));
/*
  Exit if insufficient memory
*/
                  if (new_pname==NULL)
                  {
                    (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                                    "(names structure)");
                    exit(0);
                  }
                  for(pname=pxflp->names; pname->next != NULL;
                                          pname=pname->next);
                  pname->next=new_pname;
                  new_pname->prev=pname;
                  pname=new_pname;
                  pname->next=new_pname=NULL;
                  pname->isobits= pisof;
                  pxflp->num_siz++;
                  break;
               }
            }
            if(pxfls==NULL)
/*
  no match found so it is a new generic name
*/
            {  pxfls = (FNTXfont_list *)MALLOC(1,sizeof(FNTXfont_list));
/*
 Exit if insufficient memory
*/
               if (pxfls==NULL)
               {
                 (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                                      "(font_list structure)");
                  exit(0);
               }
/*
  set up list entries
*/
               pxflp=pxflt;
               pxflp->pnext=pxfls;
               pxflp=pxfls;
               pxflp->pnext=NULL;
               pxflp->gen_name=gen_name;
               pxflp->num_siz=1;
               pname=(FNTXnames *)MALLOC(1,sizeof(FNTXnames));
/*
  Exit if insufficient memory
*/
               if (pname==NULL)
               {
                  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                                 "(font_name structure)");
                  exit(0);
               }
               pxflp->names=pname;
               pname->next=NULL;
               pname->prev=NULL;
               pname->isobits= pisof;
               pxflt=pxflp;
               num_generic++;
            }
      }
   }              /* for loop end */

#ifdef DEBUG
   DMESS "number of generic fonts is %d\n",num_generic);
#endif

/*
  sort sizes by bubble sort as it's a once off thing
*/

    for(pxflp=pxflh; pxflp!= NULL; pxflp=pxflp->pnext)
    {  FNTXnames  *pname1, *pname2, *pname_tmp;
       int ii,j,swapped;

       for(ii=0; ii<(pxflp->num_siz-1); ii++)
       {  pname1=pxflp->names;
          pname2=pname1->next;
          swapped=FALSE;
          for(j=1; j<(pxflp->num_siz-ii); j++)
          {  if(pname1->isobits->dsnsize > pname2->isobits->dsnsize)
/*
 throw all the pointers in the air and hope they land in the correct places!
 I mean swap two adjacent positions.
*/
             {  pname_tmp=pname1->prev;
                if(pname_tmp != NULL)
                  pname_tmp->next=pname2;
                else
                  pxflp->names=pname2;
                pname2->prev=pname_tmp;
                pname1->next = pname2->next;
                pname_tmp=pname2->next;
                if(pname_tmp != NULL)
                  pname_tmp->prev= pname1;
                pname2->next= pname1;
                pname1->prev= pname2;
                swapped=TRUE;
             }
             else
               pname1=pname1->next;

             if(pname1->next !=NULL )
               pname2=pname1->next;
          }
          if(!swapped) break;
       }
    }

/* print out the list */
#ifdef DEBUG
    for(pxflp=pxflh; pxflp!= NULL; pxflp=pxflp->pnext)
    {  DMESS "Generic name is :%s\n",pxflp->gen_name);
       DMESS "number in generic class is : %d\n",pxflp->num_siz);
       for(pname=pxflp->names; pname!=NULL; pname=pname->next)
       {  DMESS "font size is:%f\n",pname->isobits->dsnsize);
          DMESS "font name is:%s\n",pname->isobits->fontname);
       }
    }
#endif

     /*----------------------------------------------------------------------*
      |   From the list that has now been constructed, make up the list      |
      |   in the form in which FNTmatch will search it, attaching it to      |
      |   the HARDWARE slot in the list of available methods.                |
      *----------------------------------------------------------------------*/

    mname[RAS] = "X-windows";
    nmfonts[RAS] = num_total;
    mfonts[RAS] = pp = (struct isofont **)
                       MALLOC(nmfonts[RAS], sizeof(isofontp));
    if (pp == (struct isofont **) NULL)
    {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"X-font (list)");
       exit(0);
    }

    for(pxflp=pxflh; pxflp!= NULL; pxflp=pxflp->pnext)
    {
       for(pname=pxflp->names; pname!=NULL; pname=pname->next)
       {  FNTXnames *pfn;

          pfn=pname->prev;
          if (pfn != NULL)
            pname->isobits->minsize=pname->isobits->dsnsize;
          else
            pname->isobits->minsize=0;
          pfn=pname->next;
          if (pfn != NULL)
            pname->isobits->maxsize=pfn->isobits->dsnsize-1;
          else
            pname->isobits->maxsize=0;

          *pp++ = pname->isobits;
       }            /* end of loop over specific name  */
    }               /* end of loop over generic names  */


/*
 Free off some space
*/

    for(pxflp=pxflh; pxflp!= NULL; pxflp=pxflp->pnext)
       FREE(pxflp->gen_name);

}                   /* end of FNTXlist */
#endif


     /*-----------------------------------------------------------------*
      |                                                                 |
      |      FNTmatch()                                                 |
      |                                                                 |
      |      Find a match for the current font and character sets       |
      |      and return a pair of methods and fonts for the caller      |
      |      to use.                                                    |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
void FNTmatch(struct textatt *txtatt,
              Enum *available, Enum *methods, Index *fonts)
#else
void FNTmatch(txtatt, available, methods, fonts)

struct textatt *txtatt;
Enum *available, *methods;
Index *fonts;
#endif

/*  The overall scheme is as follows.  It assumes that the character set
    is irrelevant at present, but does check that the character sets
    requested are known to ISO.  This might also be done at CGM header
    time (a tidy-up that we can do in phase 2).  Note that all searches
    are case-independent.

    Input:

         We have a font index in `findex' and two character set indices
         in `[a]csindex'; these are in `textatt'.  We don't know if the text
         strings are in the inner or outer set (ie, which of the character
         set indices will actually be used) and don't care since this
         will be handled by the code that does the output; we assume that
         both character sets may be needed.

         We are also given - by the driver that calls this utility -
         a list of available methods, in order, terminated by 0.

    Output:

         A pair of methods (PostScript, Bezier etc..) and font ids as used
         by that system; a pair because the first deals with the primary
         character set and the second with the alternate character set.

    Search Order:

         1.  Verify that the character set indices are valid and search
             the list of ISO character sets to match the given escape
             sequence with those for each character set.  Return the
             index into the ISO list as the result; default 0 if no
             match of the cgm indices were out of range.

         2.  Search the list of `available' fonts (ie all fonts for each
             available method, in the order given) BY NAME; return if
             a direct match is found.

         3.  Search the list of `requestable' fonts (ie all fonts for each
             package for which there is a list) BY NAME.  If a match is
             found, extract the attributes of the requestable font and then
             search the `available' fonts for a match BY ATTRIBUTES.  Return
             the best match.

         4.  Remove any `source' string that may prefix the name; discard
             this information.

         5.  Search the `special fonts' list (which has the names of all
             fonts that look like family names but are actually names for
             specific fonts); if a match is found, extract the attributes
             of the font and then search the `available' fonts for a match
             BY ATTRIBUTES.  Return the best match.

         6.  Strip the name down to (hopefully) a recognizable family name
             by removing `variant' suffixes.  Keep the information removed.
             Search the `family' names and then the `alias' names for a
             match BY NAME.  If no match is found, return method 1 font 1.
             If a match is found, extract the attributes of the font and
             merge them with the infromation found by remving the `variant'
             strings.  Search the `available' fonts for a match BY ATTRIBUTES.
             Return the best match.

    Example:

         Let us have four test cases:

            1.  Font requested is `Palatino-Bold', (ie a PostScript font
                name, and PostScript is one of the allowed methods.

            2.  Font requested is `Palatino Bold', ie not a PostScript
                font name but a font that PostScript supports, and
                PostScript is one of the allowed methods.

            3.  Font requested is `Arrighi Italic' which is not a
                PostScript font; assume PostScript is not allowed.

            4.  Font requested is `HELV-BOLD', a name used by UNIRAS
                that is equivalent to `Helvetica Bold'; assume PostScript
                is not available.

         The first search is through the list of available methods,
         looking for an exact match on the original font name.

         Results:  Case 1:  hit
                   Case 2:  miss
                   Case 3:  miss
                   Case 4:  miss

         Therefore for case 1, method PostScript and the font id for
         `Palatino-Bold' is returned.

         Next we look through all the names of fonts that are known
         to have been used in CGMs by different graphics systems.
         This includes a list for UNIRAS.

         Results:  Case 2:  miss
                   Case 3:  miss
                   Case 4:  hit

         All attributes of the font (family, weight, propwidth and
         variant are extracted from the corresponding entry.

         The next operation is to strip the original font name of all
         modifiers like `italic' and `bold'.  These are held in the
         `variant', `weight' and `width' lists.  As these are stripped
         off, their effect (bolder-thinner etc..) is recorded so that
         they can then be reapplied later.  The result is a font family
         name.

         Results:  Case 2:  font family name  `Palatino'
                            weight            7 (=bold)
                            width             5 (=normal)
                            variant           0 (=regular)

                   Case 3:  font family name  `Arrighi'
                            weight            5 (=medium)
                            width             5 (=normal)
                            variant           1 (=italic)

         Now the font family name is looked for in the vast directory
         (and a small extension for aliases).

         Results:  Case 2:  Palatino found in entry 1276
                   Case 3:  Arrighi  found in entry 1701

         Note that if the font family name had been `Palatin' it would
         have been found in the alias file and the same value (1276)
         would be returned.

         We have now identified the font family but know that we have no
         implementation of it.  We therefore pick up the font attributes
         (as defined by ISO and recorded against each font in the
         database) and look for closest match in the implementation list.
         This has to take into account the modifiers that were removed
         so we could get the font family name.

         We therefore look again through the list of available fonts but
         this time we are looking for a match by attributes rather than
         by name.  The algorithm by which the search is prepared to[
         ignore more and more of the requested attributes will be
         refined in the light of experience.

*/

{  char *func = "FNTmatch";

   int i, j;
   int isocs[2], *pisocs, fudgecs[2], *pfudge;
   Index findex;
   char *cgmfontname, msg[80], fontname[256];
   struct modifier *pmod;
   struct isovariant stripvar;
   struct family *pfound;
#ifdef DEBUG
   Enum *pmethod;
#endif

   if (!started) FNTinit();

 /*  1.  Check that the CGM character set indices are within the range
  |      of the CGM character set list give; if so, search for the
  |      character set escape sequences in the
  |      list of registered escape sequences.
  */

#ifdef DEBUG
   DMESS "FNTmatch called\n");
   DMESS "  Textatt contents are:\n");
   DMESS "    Fontindex    %4d\n",txtatt->fontindex);
   DMESS "    Precision    %4d (%s)\n",txtatt->precision,
          txtatt->precision == STRING ? "STRING" :
          txtatt->precision == CHAR   ? "CHAR" : "STROKE");
   DMESS "    Expansion    %5.2g\n",txtatt->expansion);
   DMESS "    Spacing      %5.2g\n",txtatt->spacing);
   DMESS "    Colour index %4d RGB (%3d,%3d,%3d)\n",
          txtatt->colindex,
          txtatt->col.red,
          txtatt->col.green,
          txtatt->col.blue);
   DMESS "    Height       %5.2g\n",txtatt->height);
   DMESS "    Up-vector    %5.2g,%5.2g\n",txtatt->xup, txtatt->yup);
   DMESS "    Base-vector  %5.2g,%5.2g\n",txtatt->xbase, txtatt->ybase);
   DMESS "    Path         %4d (%s)\n",txtatt->path,
          txtatt->path == P_RIGHT ? "RIGHT" :
          txtatt->path == P_LEFT  ? "LEFT" :
          txtatt->path == P_UP    ? "UP" : "DOWN");
   if (txtatt->halign == CONTHORIZ)
      DMESS "    Horiz. align %4d (CONTHORIZ) %g\n",
          txtatt->halign, txtatt->hcont);
   else
      DMESS "    Horiz. align %4d (%s)\n",txtatt->halign,
          txtatt->halign == NORMHORIZ ? "NORMHORIZ" :
          txtatt->halign == LEFT      ? "LEFT" :
          txtatt->halign == CTR       ? "CTR" : "RIGHT");
   if (txtatt->valign == CONTVERT)
      DMESS "    Horiz. align %4d (CONTVERT) %g\n",
          txtatt->valign,txtatt->vcont);
   else
      DMESS "    Vert. align  %4d (%s)\n",txtatt->valign,
          txtatt->valign == NORMVERT ? "NORMVERT" :
          txtatt->valign == TOP       ? "TOP" :
          txtatt->valign == CAP       ? "CAP" :
          txtatt->valign == HALF      ? "HALF" :
          txtatt->valign == BASE      ? "BASE" : "BOTTOM");
   DMESS "    C.S. indices %4d, %4d\n",txtatt->csindex,
                                                 txtatt->acsindex);
   DMESS "  Sharedatt contents are:\n");
   DMESS "    Aux. colour  %4d RGB (%3d,%3d,%3d)\n",
          txtatt->shared->auxindex,
          txtatt->shared->aux.red,
          txtatt->shared->aux.green,
          txtatt->shared->aux.blue);
   DMESS "    Colour mode  %4d (%s)\n",txtatt->shared->colmode,
          txtatt->shared->colmode == INDEXED ? "INDEXED" : "DIRECT");
   DMESS "    Transparency %4d (%s)\n",txtatt->shared->transparency,
          txtatt->shared->transparency == YES_TRANS ? "ON" : "OFF");
   DMESS "    Granularity  %5.2g,%5.2g\n",
          txtatt->shared->xgrain, txtatt->shared->ygrain);
   pmethod = available;
   DMESS "  Available text methods: ");
   while ((int) *pmethod > 0)
      DMESS " %d",(int) *pmethod++);
   DMESS "\n");
#ifdef FONTDISPLAY
   FNTdisplay();
#endif
#endif

   if (cgmcsets.count > 0)
   {  Index csi[2], *pcsi;

      csi[0] = txtatt->csindex;
      csi[1] = txtatt->acsindex;

      for (i=0,pcsi=csi,pisocs=isocs,pfudge=fudgecs;
           i<2;
           i++,pcsi++,pisocs++,pfudge++)

      {  Index cgmcsi = *pcsi;   /*  get the CGM character set index  */
         struct isocharset *pcs;       /*  pointer into ISO CS array  */
         char *cgmcsseq;

          /*  Check range of CGM character set index  */

#ifdef DEBUG
         DMESS "  Processing character set number %d index %d\n",i,cgmcsi);
#endif
         *pisocs = -1;
         if ((cgmcsi < 0) || (cgmcsi > (cgmcsets.count-1)))
         {  sprintf(msg,"%d",cgmcsi);
            CGMERROR(func,ERR_CSETRANGE,ERROR,msg);
         }
         else
         {

          /*  Match the CGM character set escape sequence (which will
           |  be of the form `n/m') with one of the sequences in the
           |  ISO character set list.
           */

            cgmcsseq = cgmcsets.names[cgmcsi];
#ifdef DEBUG
            DMESS "Looking for match for sequence %s\n",cgmcsseq);
#endif
            for (j=0; j<ncsets; j++)

            {  *pisocs = -2;
                pcs = csets[j];
#ifdef DEBUG1
               DMESS "Looking at ISO %d esc %s name %s\n",
                       pcs->registration, pcs->escseq, pcs->name);
#endif
               if( match_string(pcs->escseq,cgmcsseq) )
               {  *pisocs = j;
                  *pfudge = pcs->fudge;
#ifdef DEBUG
                  DMESS "Match found for CGM CSET %s\n",cgmcsseq);
#endif
                  break;
               }
            }
            if (*pisocs == -2)
            {  sprintf(msg,"%d %s",cgmcsi,cgmcsseq);
               CGMERROR(func,ERR_CSETUNKNOWN,ERROR,msg);
            }
         }
         if (*pisocs < 0) *pisocs = 0;
      }

#ifdef DEBUG
      DMESS "  Character sets matched ISO sets %d and %d\n",
                        isocs[0],isocs[1]);
#endif
   }
   else
   {
          /*  No character sets defined, so default to first entry in
           |  ISO list.
           */

      isocs[0] = isocs[1] = fudgecs[0] = fudgecs[1] = 1;
#ifdef DEBUG
      DMESS "No CGM character sets defined, so easy match\n");
#endif
   }

     /*---------------------------------------------------------------*
      |  The two element array `isocs' now has the index into the     |
      |  ISO character set array for each of the requested            |
      |  character sets.  Therefore we know which character sets      |
      |  the requestor wanted.  Now to deal with fonts.               |
      *---------------------------------------------------------------*/

     /*  Set up default font index  */

    methods[0] = methods[1] = available[0];
    fonts[0] = fonts[1] =
              check_size( txtatt, methods[0], &mfonts[methods[0]][0] );

 /*  2.  Check that the font index supplied is within the range
  |      of the list supplied in the CGM header.
  */

   if (cgmfonts.count < 1)
   {
#ifdef RALFONTS
/* If no Fontlist check for valid Bezier/Hershey fonts */
      fonts[0] = fonts[1] = txtatt->fontindex;
      if ( fonts[0] > 199 && fonts[0] < 213 ) methods[0] = methods[1] = BEZ;
      else if ( fonts[0] > 100 && fonts[0] < 123 ) methods[0] = methods[1] = HER;
#endif
#ifdef DEBUG
         DMESS "No CGM font list, so returning method %d font %d\n",
              methods[0], fonts[0]);
#endif
      return;
   }

   findex = txtatt->fontindex;
   if ( findex < 1 || findex > cgmfonts.count )
   {  sprintf(msg,"%d",findex);
      CGMERROR(func,ERR_FONTRANGE,ERROR,msg);
      return;
   }

     /*  The index is within range, so we can pick up the name of the
      |  font from the cgm font list.
      */

   cgmfontname = cgmfonts.names[findex-1];

   /* If font name has been deleted, return with default font index */
   if(cgmfontname[0] == (char)0) return;

 /*  3.  Remove any `source' name from the requested font.  */

   strcpy(fontname,cgmfontname);

   for (i=0; i<nsources; i++)
   {
      struct source *psource = sources[i];

      if ( match_substring(psource->name,fontname) )
      {
         strcpy(fontname,cgmfontname+strlen(psource->name)+1);
#ifdef DEBUG2
         DMESS "Source string `%s' stripped leaving `%s'\n",
                 psource->name,fontname);
#endif
         break;
      }
   }

#ifdef DEBUG
   DMESS "  Name of font %d from CGM fontlist is %s\n",findex,fontname);
#endif

     /*---------------------------------------------------------------*
      |  The character pointer `fontname' now has the complete        |
      |  fontname as requested in the CGM; this may be as             |
      |  complicated as:                                              |
      |                                                               |
      |     Sempel Times New Roman Bold Italic Condensed              |
      |                                                               |
      |  For the present we deal with it whole.  When we strip out    |
      |  parts, it will get copied to `fontname'.                     |
      *---------------------------------------------------------------*/


     /*  4.  For each of the output methods in the `available'
      |      list - and in the order of that list - search for a match
      |      BY NAME between the requested font name and the name of
      |      the implemented font.  The `available' list should be
      |      terminated by a 0 entry.
      */

#ifdef DEBUG
   DMESS "  Searching the available fonts for `%s'\n",fontname);
#endif

/*  If font name is 'hardware' use default font and method */

   if ( match_string(fontname,"hardware") )
   {
#ifdef DEBUG
      DMESS "        ---> Match found - method %d font %d {%s}\n",
                   methods[0], fonts[0], fontname);
#endif
      return;
   }

   for (i=0; available[i] > (Enum)0; i++)
   {
      Enum method = available[i];
      int nfont;
      struct isofont **mfont;

      if( method < 1 || method > MAXMETHOD )
      {  sprintf(msg,"%d",method);
         CGMERROR(func,ERR_METHOD,ERROR,msg);
         continue;
      }

#ifdef DEBUG1
      DMESS "    Processing available method %d number %d title %s\n",
             i, method, mname[method]);
#endif

      mfont = mfonts[method];
      nfont = nmfonts[method];
#ifdef DEBUG1
      DMESS "      This method has %d fonts\n",nfont);
#endif

      for (j=0; j<nfont; j++)
      {
         struct isofont *pf;

         pf = mfont[j];

#ifdef DEBUG1
         DMESS "        Inspecting font %d (%s)\n",pf->fontid,pf->fontname);
#endif
         if( match_string(pf->fontname,fontname) )
         {  methods[0] = methods[1] = method;
            fonts[0] = fonts[1] = check_size( txtatt, method,
                                              &mfonts[method][j] );
#ifdef DEBUG
            DMESS "        ---> Match found - method %d font %d {%s} %s\n",
                   method, fonts[0], mname[method], fontname);
#endif
            return;
         }
      }
   }
#ifdef DEBUG
   DMESS "No match found with an implementation method\n");
#endif

     /*---------------------------------------------------------------*
      |  We now know that `fontname' is not one of the fonts          |
      |  implemented by any of the available methods, in so far       |
      |  as its name does not match any of the implementation         |
      |  font names.  The next possibility is that the name           |
      |  is known to be one of those of one the packages for which    |
      |  a list of fonts is known and has been classified.            |
     /*---------------------------------------------------------------*

     /*  5.  Search for the requested font in the lists of fonts that
      |      are known to be placed in CGM fontlists by various packages.
      |      If found, search the available fonts for a match by
      |      attributes.
      */

#ifdef DEBUG
   DMESS "Searching requestable fonts for: '%s'\n", fontname);
#endif
   for (i=0; i<nrfonts; i++)
   {
      struct font *prfont = rfonts[i];

#ifdef DEBUG1
      DMESS "  Requestable font: '%s'\n",prfont->fontname);
#endif

      if( match_string(prfont->fontname,fontname) )
      {
         struct request req;
         struct result  res;

#ifdef DEBUG
         DMESS "  Match to font %d %s\n",i,fontname);
#endif
         req.cset[0] = prfont->cset;
         req.cset[1] = prfont->cset;
         req.familyid = prfont->familyid;
         req.dsn = prfont->dsngroup;
         req.var = prfont->variant;
         req.height = txtatt->height * txtatt->shared->ygrain;
         req.rotation = 0;
         if( match_attrib(req, available, &res) )
         {  methods[0] = res.method[0];
            fonts[0] = res.font[0];
            methods[1] = res.method[1];
            fonts[1] = res.font[1];
#ifdef DEBUG
            DMESS "    -----> Match to method %d font %d\n",
                   methods[0],fonts[0]);
#endif
            return;
          }
      }
   }
#ifdef DEBUG
   DMESS "No match found with any requestable font\n");
#endif

     /*---------------------------------------------------------------*
      |  We now have a `fontname' that should start with a family     |
      |  name and may have suffixes after it for variants.  However,  |
      |  it may be one of the special font names that signify a       |
      |  fully resolved font rather than a variant.  If so, we have   |
      |  no need to strip off variant suffixes.                       |
     /*---------------------------------------------------------------*

     /*  6.  Search the list of `special fonts', searching the
      |      `available' fonts list if a match is found.
      */

#ifdef DEBUG
   DMESS "Searching special fonts for: '%s'\n", fontname);
#endif
   for (i=0; i<nsfonts; i++)
   {
      struct font *psfont = sfonts[i];

#ifdef DEBUG1
      DMESS "Testing for special font %s\n",psfont->fontname);
#endif
      if( match_string(psfont->fontname,fontname) )
      {
         struct request req;
         struct result  res;

#ifdef DEBUG
         DMESS "Match found - constructing request block\n");
#endif
         req.cset[0] = fudgecs[0];
         req.cset[1] = fudgecs[1];
         req.familyid = psfont->familyid;
         req.dsn = psfont->dsngroup;
         req.var = psfont->variant;
         req.height = txtatt->height * txtatt->shared->ygrain;
         req.rotation = 0;
         if( match_attrib(req, available, &res) )
         {  methods[0] = res.method[0];
            fonts[0] = res.font[0];
            methods[1] = res.method[1];
            fonts[1] = res.font[1];
#ifdef DEBUG
            DMESS "    -----> Match to method %d font %d\n",
                   methods[0],fonts[0]);
#endif
            return;
         }
      }
   }
#ifdef DEBUG
   DMESS "No match found with any special font\n");
#endif

     /*---------------------------------------------------------------*
      |  We now know that `fontname' is not the name of any of the    |
      |  available fonts, nor one of those known for a list of        |
      |  well-known packages, nor one of the special fonts that       |
      |  was searched for immediately above.  The only way we will    |
      |  now get a match is on the typeface family database.          |
      |  However, although `fontname' now starts with a family        |
      |  name, there may be suffixes indicating variants.             |
     /*---------------------------------------------------------------*

     /*  7.  Remove all variants from `fontname'; these may occur
      |      in any order and words like `bold' may be prefixed
      |      by others {like `extra') so the order of the `weights',
      |      `widths' and `variants' lists is carefully chosen so that
      |      (eg) `extra bold' is found before just `bold'.
      |      Note that the `space' in each text string in the same
      |      structures is deliberate - it ensures that the name
      |      is never matched at the beginning of the string and simplifies
      |      the code in `strip_font'.
      */

   stripvar.posture   = P_UPRIGHT;
   stripvar.weight    = W_MEDIUM;
   stripvar.propwidth = X_MEDIUM;
   stripvar.structure = S_SOLID;

#ifdef DEBUG
   DMESS "Looking for a variant\n");
#endif
   for (i=0, pmod=variants; i<NUMVARIANTS; i++, pmod++)
   {  if( strip_font(fontname,pmod->name) )
      {  stripvar.posture = (enum f_postures)pmod->value;
#ifdef DEBUG
         DMESS "Stripping %s (value %d) leaves %s\n",
                 pmod->name,pmod->value,fontname);
#endif
         break;
      }
   }

#ifdef DEBUG
   DMESS "Looking for a weight\n");
#endif
   for (i=0, pmod=weights; i<NUMWEIGHTS; i++, pmod++)
   {  if( strip_font(fontname,pmod->name) )
      {  stripvar.weight = (enum f_weights)pmod->value;
#ifdef DEBUG
         DMESS "Stripping %s (value %d) leaves %s\n",
                 pmod->name,pmod->value,fontname);
#endif
         break;
      }
   }

#ifdef DEBUG
   DMESS "Looking for a propwidth\n");
#endif
   for (i=0, pmod=widths; i<NUMWIDTHS; i++, pmod++)
   {  if( strip_font(fontname,pmod->name) )
      {  stripvar.propwidth = (enum f_propwidths)pmod->value;
#ifdef DEBUG
         DMESS "Stripping %s (value %d) leaves %s\n",
                 pmod->name,pmod->value,fontname);
#endif
         break;
      }
   }

     /*---------------------------------------------------------------*
      |  We should now have, in `fontname', a pure name of a typeface |
      |  family.  But for US copyright law, the task would therefore  |
      |  just be to search a unique list of typeface family names.    |
      |  However, in the US, the NAME (not the design) of a typeface  |
      |  is copyright, so people cheat by cloning the design and      |
      |  giving it a different name.  The task therefore splits       |
      |  into two:                                                    |
     /*---------------------------------------------------------------*

     /*  8.  Search the typeface family database for a match BY NAME
      |      on `fontname'.  If a match is found, just return the
      |      pointer to the matching `family' entry.
      */

#ifdef DEBUG
   DMESS "Looking through family list\n");
#endif
   pfound = (struct family *) NULL;

   for (i=0; i<nfamilies; i++)
   {
      struct family *pfamily = families[i];

      if( match_string(pfamily->name,fontname) )

      {  pfound = pfamily;
#ifdef DEBUG
         DMESS "Match to family index %d id %d name %s\n",
                 i, pfound->number, pfound->name);
#endif
         break;
      }
   }

     /*  9.  Search the alias family database for a match BY NAME
      |      on `fontname'.  If a match is found, just return the
      |      pointer to the corresponding `family' entry.
      */

#ifdef DEBUG
   DMESS "Finished with family list\n");
#endif
   if (pfound == (struct family *) NULL)

      for (i=0; i<naliases; i++)
      {
         struct alias *palias = aliases[i];

         if( match_string(palias->name,fontname) )

         {  pfound = palias->realfamily;
#ifdef DEBUG
            DMESS "Match to alias index %d id %d name %s\n",
                    i, pfound->number, pfound->name);
#endif
            break;
         }
      }

     /* 10.  If a match was found, call `match_font' to find the
      |      nearest available font.
      */

#ifdef DEBUG
   DMESS "Finished with alias list\n");
#endif
   if (pfound == (struct family *) NULL)
   {
      CGMERROR(func,ERR_NOFMATCH,ERROR,cgmfontname);
      /* Delete unusable name from font-list to save on repition */
      cgmfontname[0] = (char)0;
   }

   else

   {
      struct request req;
      struct result  res;

#ifdef DEBUG
      DMESS "Match found with family or alias\n");
#endif
      req.cset[0] = fudgecs[0];
      req.cset[1] = fudgecs[1];
      req.familyid = pfound->number;
      req.dsn = pfound->dsngroup;
      req.var = stripvar;
      req.height = txtatt->height * txtatt->shared->ygrain;
      req.rotation = 0;
      if( match_attrib(req, available, &res) )
      {  methods[0] = res.method[0];
         methods[1] = res.method[1];
         fonts[0] = res.font[0];
         fonts[1] = res.font[1];
#ifdef DEBUG
            DMESS "    -----> Match to method %d font %d\n",
                   methods[0],fonts[0]);
#endif
         return;
      }
   }
#ifdef DEBUG
   DMESS "No match found with anything\n");
#endif
}

#ifdef FONTDISPLAY
     /*-----------------------------------------------------------------*
      |                                                                 |
      |      FNTdisplay()                                               |
      |                                                                 |
      |      Print out the font and character set tables.               |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
void FNTdisplay( void )
#else
void FNTdisplay()
#endif

{
   disp_csets();
   disp_fonts();
   disp_isocsets();
   disp_fudgesets();
   disp_families();
   disp_aliases();
   disp_sfonts();
   disp_sources();
   disp_designs();
   disp_rfonts();
   disp_mfonts();
}

#ifdef PROTO
static void disp_csets( void )
#else
static void disp_csets()
#endif

{  int i;

     /*  Display the CGM character set list  */

   DMESS "\n\n\nCGM CHARACTER SETS:\n\n");

   if (cgmcsets.count < 1)
   {  DMESS "  No CGM character sets defined\n");
      return;
   }

   for (i=0; i<cgmcsets.count; i++)
      DMESS "  set %3d (index %i3) is type %1d `%s'\n",
                i+1,i,cgmcsets.types[i],cgmcsets.names[i]);
}

#ifdef PROTO
static void disp_fonts( void )
#else
static void disp_fonts()
#endif

{  int i;

     /*  Display the CGM font list  */

   DMESS "\n\n\nCGM FONTS:\n\n");

   if (cgmfonts.count < 1)
   {  DMESS "  No CGM fonts defined\n");
      return;
   }

   for (i=0; i<cgmfonts.count; i++)
      DMESS "  font %3d (index %3d) is `%s'\n",i+1,i,cgmfonts.names[i]);
}

#ifdef PROTO
static void disp_isocsets( void )
#else
static void disp_isocsets()
#endif

{  int i;

     /*  Display the ISO character sets  */

   DMESS "\n\n\nISO CHARACTER SETS:\n\n");

   if (ncsets < 1)
   {  DMESS "  No ISO character sets defined\n");
      return;
   }

   for (i=0; i<ncsets; i++)
   {  struct isocharset *cs = csets[i];

      DMESS "  set %3d (%2d:%2d) size %3d fudge %d sequence %s\n",
              cs->registration,
              cs->sublow,
              cs->subhigh,
              cs->setsize,
              cs->fudge,
              cs->escseq);
      DMESS "          %s\n", cs->name);
   }
}

#ifdef PROTO
static void disp_fudgesets( void )
#else
static void disp_fudgesets()
#endif

{  int i;

     /*  Display the fudge character sets  */

   DMESS "\n\n\nFUDGE CHARACTER SETS:\n\n");

   if (nfsets < 1)
   {  DMESS "  No fudge character sets defined\n");
      return;
   }

   for (i=0; i<nfsets; i++)
   {  struct fudgecset *fs = fsets[i];

      DMESS "  set %3d ::  %s\n",
              fs->number,
              fs->name);
   }
}

#ifdef PROTO
static void disp_families( void )
#else
static void disp_families()
#endif

{  int i;

     /*  Display the ISO family names  */

   DMESS "\n\n\nISO TYPEFACE FAMILIES:\n\n");

   if (nfamilies < 1)
   {  DMESS "  No typeface families defined\n");
      return;
   }

   for (i=0; i<nfamilies; i++)
   {  struct family *fam = families[i];

      DMESS "  family %4d design %2d.%2d.%2d ::  %s\n",
              fam->number,
              fam->dsngroup.class,
              fam->dsngroup.subclass,
              fam->dsngroup.group,
              fam->name);
   }
}

#ifdef PROTO
static void disp_aliases( void )
#else
static void disp_aliases()
#endif

{  int i;

     /*  Display the alias names  */

   DMESS "\n\n\nTYPEFACE ALIASES:\n\n");

   if (naliases < 1)
   {  DMESS "  No typeface aliases defined\n");
      return;
   }

   for (i=0; i<naliases; i++)
   {  struct alias *ali = aliases[i];

      DMESS "  alias  `%s' -> %s\n",
              ali->name,
              ali->realfamily->name);
   }
}

#ifdef PROTO
static void disp_sfonts( void )
#else
static void disp_sfonts()
#endif

{  int i;

     /*  Display the special fonts  */

   DMESS "\n\n\nSPECIAL FONTS:\n\n");

   if (nsfonts < 1)
   {  DMESS "  No special fonts defined\n");
      return;
   }

   for (i=0; i<nsfonts; i++)
   {  struct font *f = sfonts[i];

      DMESS "  font %4d family %4d var %d %d %d %d des %2d.%2d.%2d %d :: %s\n",
              f->fontid,
              f->familyid,
              f->variant.posture,
              f->variant.weight,
              f->variant.propwidth,
              f->variant.structure,
              f->dsngroup.class,
              f->dsngroup.subclass,
              f->dsngroup.group,
              f->cset,
              f->fontname);
   }
}

#ifdef PROTO
static void disp_sources( void )
#else
static void disp_sources()
#endif

{  int i;

     /*  Display the source names  */

   DMESS "\n\n\nSOURCE NAMES:\n\n");

   if (nsources < 1)
   {  DMESS "  No sources defined\n");
      return;
   }

   for (i=0; i<nsources; i++)
   {  struct source *src = sources[i];

      DMESS "  source %4d :: %s\n",
              src->number,
              src->name);
   }
}

#ifdef PROTO
static void disp_designs( void )
#else
static void disp_designs()
#endif

{  int i;

     /*  Display the ISO design names  */

   DMESS "\n\n\nISO DESIGN NAMES:\n\n");

   if (ndesigns < 1)
   {  DMESS "  No design codes defined\n");
      return;
   }

   for (i=0; i<ndesigns; i++)
   {  struct design *des = designs[i];

      DMESS "  design %2d.%2d.%2d (%s (%s (%s)))\n",
              des->dsngroup.class,
              des->dsngroup.subclass,
              des->dsngroup.group,
              des->classname,
              des->subclassname,
              des->groupname);
   }
}

#ifdef PROTO
static void disp_rfonts( void )
#else
static void disp_rfonts()
#endif

{  int i;

     /*  Display the requestable fonts  */

   DMESS "\n\n\nREQUESTABLE FONTS:\n\n");

   if (nrfonts < 1)
   {  DMESS "  No requestable fonts defined\n");
      return;
   }

   for (i=0; i<nrfonts; i++)
   {  struct font *f = rfonts[i];

      DMESS "  font %4d family %4d variant %d %d %d %d \
design %2d.%2d.%2d %2d\n                 %s\n",
              f->fontid,
              f->familyid,
              f->variant.posture,
              f->variant.weight,
              f->variant.propwidth,
              f->variant.structure,
              f->dsngroup.class,
              f->dsngroup.subclass,
              f->dsngroup.group,
              f->cset,
              f->fontname);
   }
}

#ifdef PROTO
static void disp_mfonts( void )
#else
static void disp_mfonts()
#endif

{  int i;

     /*  Display the available fonts  */

   DMESS "\n\n\nAVAILABLE FONTS:\n\n");

   for (i=1; i<=MAXMETHOD; i++)

   {  struct isofont **methp = mfonts[i];
      int j, nfonts = nmfonts[i];

      DMESS "  method number %2d fonts %4d name %s\n",
              i, nfonts, mname[i]);

      for (j=0; j<nfonts; j++)
      {  struct isofont *f = methp[j];

         DMESS "    font %4d family %4d var %d %d %d %d des %2d.%2d.%2d\n",
                 f->fontid,
                 f->familyid,
                 f->variant.posture,
                 f->variant.weight,
                 f->variant.propwidth,
                 f->variant.structure,
                 f->dsngroup.class,
                 f->dsngroup.subclass,
                 f->dsngroup.group);
         DMESS "              source %d (%s) :: %s\n",
                 f->dsnsource->number,
                 f->dsnsource->name,
                 f->fontname);
      }
   }
}
#endif

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_csets()                                               |
      |                                                                 |
      |      Process the ISO character list file.                       |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_csets( void )
#else
static void proc_csets()
#endif

     /*  This is a fairly simple routine that reads everything in
         from a single file to a structure large enough to hold pointers
         to as many character sets as there are and then allocates
         an array of the right size.  This saves the space for
         pointers at the cost of some extra one-off processing.
     */

{  FILE *fileid;
   char *filename;
   struct isocharset  *protocs, **cslist, *tempcsets[MAXCSET];
   struct fudgecset   *protofs, **fslist, *tempfsets[MAXFUDGE];
   char *func = "FNTinit(csets)";
   int i;

#ifdef DEBUG
   DMESS "Reading in CHARACTER SET file\n");
#endif
   ncsets = 0;
   cslist = tempcsets;

        /*  Generate the filename and open the file.  */

   filename = CGMpath("c_iso",fntext);
   fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
   if (fileid == NULL)
   {  CGMERROR (func, ERR_NOCSETS, ERROR, filename);
      FREE(filename);
      return;
   }
   FREE(filename);

        /*  Process each entry  */

   while (ncsets < (MAXCSET-1))
   {  int n;
      char name[80];
      struct isocharset cs, *csp;

        /*  First read the entry into local variables  */

      n = fscanf(fileid,"%d %d %d %d %s %d",
                      &cs.registration,
                      &cs.sublow,
                      &cs.subhigh,
                      &cs.setsize,
                      cs.escseq,
                      &cs.fudge);
      if (n < 6) break;
#ifdef CSETNAMES
      read_rest(fileid,cs.name=name,80);
#else
      while (fgetc(fileid) != '\n') ;
#endif

        /*  Now allocate a new 'isocharset' data structure and copy all
            the information into it.
        */

      ncsets++;
      csp = (struct isocharset *) MALLOC(1,sizeof(cs));
      if (csp == (struct isocharset *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                         "ISO charset (structure)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      *csp = cs;
#ifdef CSETNAMES
      csp->name = (char *)MALLOC (1, strlen(name)+1 );
      if (csp->name == (char *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"ISO charset (name)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      strcpy(csp->name,name);
#else
      csp->name = (char *) NULL;
#endif
#ifdef DEBUG1
      DMESS "Character set %s\n             %d %d %d %d %d %s\n",
              name,csp->registration,csp->sublow,csp->subhigh,
              csp->setsize,csp->fudge,csp->escseq);
#endif
      *cslist++ = csp;
   }

        /*  Close the isocharset file tidily.  */

   fclose(fileid);
#ifdef DEBUG
   DMESS "CHARACTER SET file read - %d entries found\n",ncsets);
#endif

        /*  Finally allocate an array large enough to hold all the pointers
            to the isocharset structures so that they can be accessed
            quickly.  */

   csets = (struct isocharset **) MALLOC(ncsets,sizeof(protocs));
   if (csets == (struct isocharset **) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"ISO charset (list)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }
   for (i=0; i<ncsets; i++)
      csets[i] = tempcsets[i];

     /*  Now do the same thing for the `fudge' character set list;
      |  this is just here as we wish to react in a systematic way
      |  to requests for non-roman character sets, but the full work
      |  required - to do it via a codepage system - demands effort
      |  far beyond what we have available.
      */

#ifdef DEBUG
   DMESS "Reading in FUDGE CHARACTER SET file\n");
#endif
   nfsets = 0;
   fslist = tempfsets;

        /*  Generate the filename and open the file.  */

   filename = CGMpath("c_fudge",fntext);
   fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
   if (fileid == NULL)
   {  CGMERROR (func, ERR_NOFSETS, ERROR, filename);
      FREE(filename);
      return;
   }
   FREE(filename);

        /*  Process each entry  */

   while (nfsets < (MAXFUDGE-1))
   {  int n;
      char name[80];
      struct fudgecset fs, *fsp;

        /*  First read the entry into local variables  */

      n = fscanf(fileid,"%d", &fs.number);
      if (n < 1) break;
      if (fs.number < 0) break;
      read_rest(fileid,fs.name=name,80);

        /*  Now allocate a new 'fudgecset' data structure and copy
            the information into it.
        */

      nfsets++;
      fsp = (struct fudgecset *) MALLOC(1,sizeof(fs));
      if (fsp == (struct fudgecset *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                         "fudge charset (structure)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      *fsp = fs;
      fsp->name = (char *)MALLOC (1, strlen(name)+1 );
      if (fsp->name == (char *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"fudge charset (name)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      strcpy(fsp->name,name);
#ifdef DEBUG1
      DMESS "Fudge character set %d %s\n",fsp->number,fsp->name);
#endif
      *fslist++ = fsp;
   }

        /*  Close the isocharset file tidily.  */

   fclose(fileid);
#ifdef DEBUG
   DMESS "FUDGE CHARACTER SET file read - %d entries found\n",nfsets);
#endif

        /*  Finally allocate an array large enough to hold all the pointers
            to the isocharset structures so that they can be accessed
            quickly.  */

   fsets = (struct fudgecset **) MALLOC(nfsets,sizeof(protofs));
   if (fsets == (struct fudgecset **) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"fudge charset (list)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }
   for (i=0; i<nfsets; i++)
      fsets[i] = tempfsets[i];
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_typefaces()                                           |
      |                                                                 |
      |      Process all the files associated with typeface families.   |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_typefaces( void )
#else
static void proc_typefaces()
#endif

{
     /*  This routine has rather more to do as it has to read in four
         files that are interlinked:

            family      - the list of family names and design group info
            alias       - alias names for families
            fonts       - family names that are really specific fonts
            source      - names of designers and foundries
            design      - the design group information
     */

#ifdef DEBUG
   DMESS "Processing TYPEFACE files\n");
#endif

   proc_families();
   proc_aliases();
   proc_sfonts();
   proc_sources();
   proc_designs();

#ifdef DEBUG
   DMESS "All TYPEFACE files processed\n");
#endif
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_families()                                            |
      |                                                                 |
      |      Process the names of all typeface families which are       |
      |      genuinely families - like Bodoni and unlike Univers.       |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_families( void )
#else
static void proc_families()
#endif

{  FILE *fileid;
   char *filename;
   int i;
   struct family **famlist, *protofam, *tempfams[MAXFAMILY];
   char *func = "FNTinit(families)";

#ifdef DEBUG
   DMESS "Reading in FAMILY file\n");
#endif
   nfamilies = 0;
   famlist = tempfams;

        /*  Generate the filename and open the file.  */

   filename = CGMpath("f_family",fntext);
   fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
   if (fileid == NULL)
   {  CGMERROR (func, ERR_NOFAMS, ERROR, "");
      FREE(filename);
      return;
   }
   FREE(filename);

     /*  Process each entry  */

   while (nfamilies < (MAXFAMILY-1))
   {  int n;
      char name[80];
      struct family fam, *famp;

        /*  First read the entry into local variables  */

      n = fscanf(fileid,"%d %d.%d.%d",
                      &fam.number,
                      &fam.dsngroup.class,
                      &fam.dsngroup.subclass,
                      &fam.dsngroup.group);
      if (n < 4) break;
      read_rest(fileid,name,80);

        /*  Now allocate a new 'isofamily' data structure and copy all[
            the information into it.
        */

      nfamilies++;
      famp = (struct family *) MALLOC(1,sizeof(fam));
      if (famp == (struct family *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"family (structure)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      *famp = fam;
      famp->name = (char *)MALLOC ( 1, strlen(name)+1 );
      if (famp->name == (char *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"family (name)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      strcpy(famp->name,name);
#ifdef DEBUG1
      DMESS "Font family %s\n            %d %d.%d.%d\n",
              famp->name,
              famp->number,
              famp->dsngroup.class,
              famp->dsngroup.subclass,
              famp->dsngroup.group);
#endif
      *famlist++ = famp;
   }

        /*  Close the isofamily file tidily.  */

   fclose(fileid);

        /*  Finally allocate an array large enough to hold all the pointers
            to the family structures so that they can be accessed
            quickly.  */

   families = (struct family **) MALLOC(nfamilies,sizeof(protofam));
   if (families == (struct family **) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,"family (list)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }
   for (i=0; i<nfamilies; i++)
      families[i] = tempfams[i];
#ifdef DEBUG
   DMESS "FAMILY file read - %d entries\n",nfamilies);
#endif

}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_aliases()                                             |
      |                                                                 |
      |      Process the names of typefaces that are just aliases       |
      |      of real families - eg Palatin for Paltino.                 |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_aliases( void )
#else
static void proc_aliases()
#endif

{  FILE *fileid;
   char *filename;
   int i;
   struct alias *aliasp, **aliaslist, *tempalias[MAXALIAS];
   char *func = "FNTinit(aliases)";

     /*  Now we process the alias file; this is simpler as
      |  it only has numbers and names, but the number has to be
      |  searched for in the family list and a pointer set so that
      |  when an alias is found the font information is directly
      |  accessible.
      */

#ifdef DEBUG
   DMESS "Reading in ALIAS file\n");
#endif
   naliases = 0;
   aliaslist = tempalias;

     /*  Generate the filename and open the file.  */

   filename = CGMpath("f_alias",fntext);
   fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
   FREE(filename);

     /*  If no alias file is found, just bypass the processing  */

   if (fileid != NULL)

        /*  Process each entry  */

   {  while (naliases < (MAXALIAS-1))
      {  int n, number;
#ifdef DEBUG
         int found;
#endif
         char name[80];
         struct alias alias;

           /*  First read the entry into local variables  */

         n = fscanf(fileid,"%d",&number);
         if (n < 1) break;
         if (number < 1) break;
         read_rest(fileid,name,80);
#ifdef DEBUG1
      DMESS "Family alias %d %s\n",number, name);
#endif

           /*  Now search through the isofamily lists to find the
            |  font family for which this is a synonym.
            */

#ifdef DEBUG
         found = FALSE;
#endif

         for (i=0; i<nfamilies; i++)
         {  struct family *famp = families[i];
#ifdef DEBUG1
            DMESS "Testing family %d number %d name %s\n",i,
                   famp->number,famp->name);
#endif
            if (number == famp->number)
            {  aliasp = (struct alias *) MALLOC(1,sizeof(alias));
               if (aliasp == (struct alias *) NULL)
               {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                                  "alias (structure)");
                  exit(0);         /* redundant, as CGMerror will do it  */
               }
               naliases++;
               aliasp->name = (char *)MALLOC ( 1, strlen(name)+1 );
               if (aliasp->name == (char *) NULL)
               {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "alias (name)");
                  exit(0);         /* redundant, as CGMerror will do it  */
               }
               strcpy(aliasp->name,name);
               aliasp->realfamily = famp;
#ifdef DEBUG
               found = TRUE;
#endif
               *aliaslist++ = aliasp;
#ifdef DEBUG1
               DMESS "Alias match found for %d %s in %d %s\n",
                      number, name, famp->number, famp->name);
#endif
               break;
            }
         }

#ifdef DEBUG
         if (!found)
            DMESS "No match found for alias %d %s\n", number, name);
#endif

      }

      fclose(fileid);

        /*  If any aliases were provided, now copy across the pointer
         |  array to a freshly malloced one, which is then remembered
         |  in `aliases'.
         */

      if (naliases > 0)

      {  aliases = (struct alias **) MALLOC(naliases,sizeof(aliasp));
         if (aliases == (struct alias **) NULL)
         {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "alias (list)");
            exit(0);         /* redundant, as CGMerror will do it  */
         }
         for (i=0; i<naliases; i++)
            aliases[i] = tempalias[i];
      }
   }
#ifdef DEBUG
   DMESS "ALIAS file read - %d matched entries\n",naliases);
#endif
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_sfonts()                                              |
      |                                                                 |
      |      Process the names of typefaces that are not just typeface  |
      |      families but fonts, in the sense of typefaces of a         |
      |      particular posture, weight, propwidth and structure.       |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_sfonts( void )
#else
static void proc_sfonts()
#endif

{  FILE *fileid;
   char *filename;
   int i;
   struct font *sfontp, **sfontlist, *tempsfont[MAXFONT];
   char *func = "FNTinit(fonts)";

     /*  Now we process the font file; this contains the names
      |  and attributes of all the fonts (like `Univers 45') which
      |  appear to be family names but which actually are explicit
      |  variants of a typeface family.
      */

#ifdef DEBUG
   DMESS "Reading in SPECIAL FONT file\n");
#endif

   nsfonts = 0;
   sfontlist = tempsfont;

     /*  Generate the filename and open the file.  */

   filename = CGMpath("f_font",fntext);
   fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
   FREE(filename);

     /*  If no font file is found, just bypass the processing  */

   if (fileid != NULL)

        /*  Process each entry  */

   {  while (nsfonts < (MAXFONT-1))
      {  int n;
         char name[80];
         struct font sfont;

           /*  First read the entry into local variables; the format
            |  of each entry is:
            |
            |     id          %d      identifier for the font
            |     dsngroup    %d.%d.%d  design group codes
            |     posture     %d        posture code (1-n)
            |     weight      %d        weight code (1-9)
            |     propwidth   %d        propwidth code (1-9)
            |     structure   %d        structure code (1-3)[
            |     name        %*s       font name
            |
            */

         n = fscanf(fileid,"%d %d %d.%d.%d %d %d %d %d",
                          &sfont.fontid,
                          &sfont.familyid,
                          &sfont.dsngroup.class,
                          &sfont.dsngroup.subclass,
                          &sfont.dsngroup.group,
                          &sfont.variant.posture,
                          &sfont.variant.weight,
                          &sfont.variant.propwidth,
                          &sfont.variant.structure);
         if (n < 9) break;
         if (sfont.fontid < 1) break;
         read_rest(fileid,name,80);
#ifdef DEBUG1
      DMESS "Special font %s\n             %d %d %d.%d.%d %d %d %d %d\n",
name,
                          sfont.fontid,
                          sfont.familyid,
                          sfont.dsngroup.class,
                          sfont.dsngroup.subclass,
                          sfont.dsngroup.group,
                          sfont.variant.posture,
                          sfont.variant.weight,
                          sfont.variant.propwidth,
                          sfont.variant.structure);
#endif
         sfont.fontname = (char *)MALLOC( 1, strlen(name)+1 );
         if (sfont.fontname == (char *) NULL)
         {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "special font (name)");
            exit(0);         /* redundant, as CGMerror will do it  */
         }
         strcpy(sfont.fontname,name);

         sfontp = (struct font *) MALLOC(1,sizeof(sfont));
         if (sfontp == (struct font *) NULL)
         {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                            "special font (structure)");
            exit(0);         /* redundant, as CGMerror will do it  */
         }
         *sfontp = sfont;
         nsfonts++;
         *sfontlist++ = sfontp;
      }

      fclose(fileid);

        /*  If any fonts were provided, now copy across the pointer
         |  array to a freshly malloced one, which is then remembered
         |  in `sfonts'.
         */

      if (nsfonts > 0)

      {  sfonts = (struct font **) MALLOC(nsfonts,sizeof(sfontp));
         if (sfonts == (struct font **) NULL)
         {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "special font (list)");
            exit(0);         /* redundant, as CGMerror will do it  */
         }
         for (i=0; i<nsfonts; i++)
            sfonts[i] = tempsfont[i];
      }
   }
#ifdef DEBUG
   DMESS "SPECIAL FONT file read - %d entries\n",nsfonts);
#endif
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_sources()                                             |
      |                                                                 |
      |      Process the names of suppliers of fonts; these may         |
      |      prefix the real font name - eg Stempel Arrighi.            |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_sources( void )
#else
static void proc_sources()
#endif

{  FILE *fileid;
   char *filename;
   int i;
   struct source  *protosrc, **srclist, *tempsrcs[MAXSOURCE];
   char *func = "FNTinit(sources)";

#ifdef DEBUG
   DMESS "Reading in SOURCES file\n");
#endif

   nsources = 0;
   srclist = tempsrcs;

        /*  Generate the filename and open the file.  */

   filename = CGMpath("f_source",fntext);
   fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
   FREE(filename);
   if (fileid == NULL)
      return;

     /*  Process each entry  */

   while (nsources < (MAXSOURCE-1))
   {  int n;
      char name[80];
      struct source src, *srcp;

        /*  First read the entry into local variables  */

      n = fscanf(fileid,"%d", &src.number);
      if (n < 1) break;
      if (src.number < 1) break;
      read_rest(fileid,src.name=name,80);
#ifdef DEBUG1
      DMESS "Font source %4d %s\n", src.number, name);
#endif

        /*  Now allocate a new 'source' data structure and copy
            the information into it.
        */

      nsources++;
      srcp = (struct source *) MALLOC(1,sizeof(src));
      if (srcp == (struct source *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "source (structure)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      *srcp = src;
      srcp->name = (char *)MALLOC (1, strlen(name)+1 );
      if (srcp->name == (char *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "source (name)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      strcpy(srcp->name,name);
      *srclist++ = srcp;
   }

        /*  Close the source file tidily.  */

   fclose(fileid);

        /*  Finally allocate an array large enough to hold all the pointers
         |  to the source structures so that they can be accessed quickly.
         */

   sources = (struct source **) MALLOC(nsources,sizeof(protosrc));
   if (sources == (struct source **) NULL)
   {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "source (list)");
      exit(0);         /* redundant, as CGMerror will do it  */
   }
   for (i=0; i<nsources; i++)
      sources[i] = tempsrcs[i];
#ifdef DEBUG
   DMESS "SOURCES file read - %d entries\n",nsources);
#endif
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_designs()                                             |
      |                                                                 |
      |      Process the names of design source codes so they can       |
      |      be used in debugging.                                      |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_designs( void )
#else
static void proc_designs()
#endif

{  FILE *fileid;
   char *filename, *classname, *subclassname;
   int i;
   struct design *protodes, **deslist, *tempdess[MAXDESIGN];
   char *func = "FNTinit(designs)";

#ifdef DEBUG
   DMESS "Reading in DESIGNS file\n");
#endif

   ndesigns = 0;
   deslist = tempdess;

        /*  Generate the filename and open the file.  */

   filename = CGMpath("f_dsn",fntext);
   fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
   FREE(filename);
   if (fileid == NULL)
      return;

     /*  Process each entry  */

   while (ndesigns < (MAXDESIGN-1))
   {  int n, classid=0, subclassid=0, groupid=0;
      char name[80], *holdname;
      struct design des, *desp;

        /*  First read the entry into local variables  */

      n = fscanf(fileid,"%d.%d.%d", &classid, &subclassid, &groupid);
      if (n < 1) break;
      read_rest(fileid,name,80);
#ifdef DEBUG1
      DMESS "Design file entry; count %d; value %d %d %d; name %s\n",
              n, classid, subclassid, groupid, name);
#endif

        /*  The design file has entries in three formats, categorized
            by the number of numbers:

               1   class name
               2   subclass name
               3   group name

            We therefore only allocate a new `design' structure when
            n == 3; in all cases we take a copy of the name.
        */

      holdname = (char *)MALLOC (1, strlen(name)+1 );
      if (holdname == (char *) NULL)
      {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "design (name)");
         exit(0);         /* redundant, as CGMerror will do it  */
      }
      strcpy(holdname,name);

      switch (n)

      {  case 1: classname = holdname;
                 break;

         case 2: subclassname = holdname;
                 break;

         case 3:
                 desp = (struct design *) MALLOC(1,sizeof(des));
                 if (desp == (struct design *) NULL)
                 {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                                    "design (structure)");
                    exit(0);         /* redundant, as CGMerror will do it  */
                 }
                 desp->dsngroup.class = classid;
                 desp->dsngroup.subclass = subclassid;
                 desp->dsngroup.group = groupid;
                 desp->classname = classname;
                 desp->subclassname = subclassname;
                 desp->groupname = holdname;
                 ndesigns++;
                 *deslist++ = desp;
#ifdef DEBUG1
                 DMESS "Font design %2d.%2d.%2d (%s (%s (%s)))\n",
                         desp->dsngroup.class,
                         desp->dsngroup.subclass,
                         desp->dsngroup.group,
                         desp->classname,
                         desp->subclassname,
                         desp->groupname);
#endif
      }
   }

        /*  Close the design file tidily.  */

   fclose(fileid);

        /*  Finally allocate an array large enough to hold all the pointers
         |  to the design structures so that they can be accessed quickly.
         */

   designs = (struct design **) MALLOC(ndesigns,sizeof(protodes));
   if (designs == (struct design **) NULL)
    {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "design (list)");
       exit(0);         /* redundant, as CGMerror will do it  */
    }
   for (i=0; i<ndesigns; i++)
      designs[i] = tempdess[i];
#ifdef DEBUG
   DMESS "DESIGNS file read - %d entries\n",ndesigns);
#endif
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_reqs()                                                |
      |                                                                 |
      |      Process all files of known requestable fonts.              |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_reqs( void )
#else
static void proc_reqs()
#endif

{  FILE *fileid, *rfile;
   char *filename;
   int i;
   struct font *rfontp, **rfontlist, *temprfont[MAXREQ];
   char *func = "FNTinit(rfonts)";

     /*  The routine reads through the list of packages stored in the
      |  `r_master' file.  This contains a list of packages - not in
      |  any particular order - that are known to the system and for
      |  which font information is available.
      |  Each line contains an identifier for the package
      |  and a filename from which the information about this package
      |  can be obtained.
      */

#ifdef DEBUG
   DMESS "Reading in REQUEST files\n");
#endif

   nrfonts = 0;
   rfontlist = temprfont;

   filename = CGMpath("r_master",fntext);
   rfile = CGMfopen(filename,F_READ,CLEAR_TEXT);
   if (rfile == NULL)
   {  CGMERROR (func,ERR_NOREQFONTS,FATAL,filename);
      FREE(filename);
      exit(0);
   }
   FREE(filename);

     /*  Process each line of the file to extract a deeper filename.  */

   while (nrfonts < (MAXREQ-1))

   {  int packid, n;
      char packfile[32], packtitle[80];

      n = fscanf(rfile,"%d %s",&packid,packfile);
      if (n < 2) break;
      if (packid < 1) break;
      read_rest(rfile,packtitle,80);
#ifdef DEBUG1
      DMESS "Package id %d name %s title %s\n",packid,packfile,packtitle);
#endif

        /*  Now we have the name of a file that contains the information
         |  about a particular output method, so we open up that file
         |  and process it.
         */

      filename = CGMpath(packfile,fntext);
      fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
      if (fileid == NULL)
      {  CGMERROR (func,ERR_PACKMISSING,ERROR,filename);
         FREE(filename);
         continue;             /*  skip this file but continue with others */
      }
#ifdef DEBUG
      else
         DMESS "Request file %s opened OK\n",filename);
#endif
      FREE(filename);

        /*  Now read in the information from the particular package
         |  font files.  This contains information about the font
         |  family, variant and design group.  This is sufficient for
         |  a match to be found with the implementation method fonts
         |  descriptions.
         |
         |  Each entry in the file is as follows:
         |
         |     id        %d       identifier by which font is known to `package'
         |     source    %d       nominal design authority
         |     family    %d       identifier of font family as in `family' file
         |     posture   %d       posture code (1-n)
         |     weight    %d       weight code (1-9)
         |     propwidth %d       propwidth code (1-9)
         |     structure %d       structure code (1-3)
         |     dsngroup  %d.%d.%d design code as in `dsn' file
         |     charset   %d       a simple idea of which character set;
         |                        see c_fudge.fnt for a list
         |     name      %*s      font name
         */

      while (nrfonts < (MAXREQ-1))

      {  struct font rfont;
         int m, dsn;
         char fn[80];

     /*  Read the entry into local variables  */

         m = fscanf(fileid,"%d %d %d %d %d %d %d %d.%d.%d %ld",
                         &rfont.fontid,
                         &dsn,
                         &rfont.familyid,
                         &rfont.variant.posture,
                         &rfont.variant.weight,
                         &rfont.variant.propwidth,
                         &rfont.variant.structure,
                         &rfont.dsngroup.class,
                         &rfont.dsngroup.subclass,
                         &rfont.dsngroup.group,
                         &rfont.cset);
#ifdef DEBUG1
         DMESS "Request fscan read %d values\n",n);
#endif
         if (m < 11) break;
         if (rfont.fontid < 0) break;
         read_rest(fileid,fn,80);
#ifdef DEBUG1
         DMESS "Font id %d source %d family %d: %s\n",
                 rfont.fontid,dsn,rfont.familyid,fn);
         DMESS "Attributes %d %d %d %d design group %d.%d.%d cset %d\n",
                 rfont.variant.posture, rfont.variant.weight,
                 rfont.variant.propwidth, rfont.variant.structure,
                 rfont.dsngroup.class, rfont.dsngroup.subclass,
                 rfont.dsngroup.group,rfont.cset);
#endif

#ifdef DESNAMES

          /*  Search the `source' list for a match between the design
           |  code supplied and those in the list.  If a match is found,
           |  link the `design source' field to the name of the design
           |  source.
           */

           /*  Add this code later  */
#else
         rfont.dsnsource = (struct source *) NULL;
#endif

          /*  Now allocate space for a permanent copy of the structure
           |  and copy it across; also add pointer to the structure into
           |  the (temporary) pointer list.
           */

         nrfonts++;
         rfontp = (struct font *) MALLOC(1,sizeof(rfont));
         if (rfontp == (struct font *) NULL)
          {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                             "requestable font (structure)");
             exit(0);         /* redundant, as CGMerror will do it  */
          }
         *rfontp = rfont;
         rfontp->fontname = (char *)MALLOC (1, strlen(fn)+1 );
         if (rfontp->fontname == (char *) NULL)
         {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                            "requestable font (name)");
            exit(0);         /* redundant, as CGMerror will do it  */
         }
         strcpy(rfontp->fontname,fn);
         *rfontlist++ = rfontp;
      }

      fclose(fileid);
   }

   fclose(rfile);

       /*  Now allocate space for a permanent copy of the list of pointers
        |  to all the (permanent) isofont structures.
        */

   rfonts  = (struct font **) MALLOC(nrfonts, sizeof(rfontp));
   if (rfonts == (struct font **) NULL)
    {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "requestable font (list)");
       exit(0);         /* redundant, as CGMerror will do it  */
    }
   for (i=0; i<nrfonts; i++)
      rfonts[i] = temprfont[i];
#ifdef DEBUG
   DMESS "REQUEST files read - %d entries\n",nrfonts);
#endif
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      proc_methods()                                             |
      |                                                                 |
      |      Process all files of applicable text output methods.       |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void proc_methods( void )
#else
static void proc_methods()
#endif

{  FILE *fileid, *mfile;
   char *filename, msg[80];
   int i;
   struct isofont *tmfonts[MAXFPERM], **ptmfonts, *isofontp;
   char *func = "FNTinit(methods)";

     /*  The routine reads through the list of methods stored in the
      |  `m_master' file.  This contains a list of methods - not in
      |  any particular order - that are available to the system as a
      |  whole (ie the superset of the methods used by all installed
      |  drivers.  Each line contains an identifier for the method -
      |  which must match those of HDW, PS, HER, TEX definitions -
      |  and a filename from which the information about this method
      |  can be obtained.
      */

#ifdef DEBUG
   DMESS "Reading in METHOD MASTER file\n");
#endif

   filename = CGMpath("m_master",fntext);
   mfile = CGMfopen(filename,F_READ,CLEAR_TEXT);
   if (mfile == NULL)
   {  CGMERROR (func,ERR_NOMETHODS,FATAL,filename);
      FREE(filename);
      exit(0);
   }
   FREE(filename);

     /*  Clear out the arrays in which subsequent data will be
      |  placed, so that any missing methods are correctly handled.
      */

   for (i=0; i<=MAXMETHOD; i++)
   {  mfonts[i] = (struct isofont **) NULL;
      mname[i] = (char *) NULL;
      nmfonts[i] = -1;
   }

     /*  Process each line; the method identifier (a number) is followed
      |  by the name of the file contained the list of that method's fonts
      |  and then the informal name of the method.
      */

   for (i=0; i<MAXMETHOD; i++)

   {  int method, n, ii;
      char methfile[32], methtitle[80];
      struct isofont **pp;

      n = fscanf(mfile,"%d",&method);
      if (n < 1) break;
      if (method < 1) break;
      n = fscanf(mfile,"%s",methfile);
      if (n < 1) break;
      read_rest(mfile,methtitle,80);
#ifdef DEBUG1
      DMESS "Method id %d name %s title %s\n",method,methfile,methtitle);
#endif

          /*  Now check that the method given is valid.  If not,
           |  continue with the next entry from the master file.
           */

      if ((method < 1) || (method > MAXMETHOD))
      {  sprintf(msg,"%d %s",method,methfile);
         CGMERROR(func,ERR_METHRANGE,ERROR,msg);
         continue;
      }

          /*  The method has been selected, so change its count to an initial
           |  value of zero and copy the method name into a new string.
           */

      nmfonts[method] = 0;
      mname[method] = (char *)MALLOC (1, strlen(methtitle)+1 );
      if (mname[method] == (char *) NULL)
       {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL, "method (name)");
          exit(0);         /* redundant, as CGMerror will do it  */
       }
      strcpy(mname[method],methtitle);
      ptmfonts = tmfonts;

          /*  Now we have the name of a file that contains the information
           |  about a particular output method, so we open up that file
           |  and process it.  Proceed to next method if the file cannot be
           |  opened.
           */

      filename = CGMpath(methfile,fntext);
      fileid = CGMfopen(filename,F_READ,CLEAR_TEXT);
      if (fileid == NULL)
      {  CGMERROR (func,ERR_NOMETHODS,ERROR,filename);
         FREE(filename);
         continue;
      }
#ifdef DEBUG
      DMESS "Method file %s for %s opened OK\n",methfile, methtitle);
#endif
      FREE(filename);

          /*  Now read in the information from the individual method
           |  font files.  This effectively contains the required
           |  information about the font in `isofont' notation, although
           |  at present the character set (glyph) information is held
           |  in a short form (one of a small number of pre-defined sets).
           |  Each entry in the file is as follows:
           |
           |     id        %d       identifier for font in `method'
           |     source    %d       identifier of source as in `source' file
           |     family    %d       identifier of family as in `family' file
           |     posture   %d       posture code (1-n)
           |     weight    %d       weight code (1-9)
           |     propwidth %d       propwidth code (1-9)
           |     structure %d       structure code (1-3)
           |     dsngroup  %d.%d.%d design code as in `dsn' file
           |     dsnsize   %d       design size
           |     minsize   %d       minimum size
           |     maxsize   %d       maximum size
           |     charset   %d       a simple idea of which character set:
           |     name      %*s      font name
           */

      while (nmfonts[method] < (MAXFPERM-1))

      {  struct isofont f, *pf;
         int m, source;
         char fn[80];

     /*  Read the entry into local variables  */

         m = fscanf(fileid,"%ld %d %d %d %d %d %d %d.%d.%d %ld %ld %ld %ld",
                         &f.fontid,
                         &source,
                         &f.familyid,
                         &f.variant.posture,
                         &f.variant.weight,
                         &f.variant.propwidth,
                         &f.variant.structure,
                         &f.dsngroup.class,
                         &f.dsngroup.subclass,
                         &f.dsngroup.group,
                         &f.dsnsize,
                         &f.minsize,
                         &f.maxsize,
                         &f.cset);
         if (m < 14) break;
         if (f.fontid < 0) break;
         read_rest(fileid,fn,80);
#ifdef DEBUG1
         DMESS "Font id %d source %d family %d: %s\n",
                 f.fontid,
                 source,
                 f.familyid,
                 fn);
         DMESS "Attributes %d %d %d %d design group %d.%d.%d\n",
                 f.variant.posture,
                 f.variant.weight,
                 f.variant.propwidth,
                 f.variant.structure,
                 f.dsngroup.class,
                 f.dsngroup.subclass,
                 f.dsngroup.group);
         DMESS "Design size %d (min %d max %d) character set %d\n",
                 f.dsnsize,
                 f.minsize,
                 f.maxsize,
                 f.cset);
#endif

          /*  Search the `source' list for a match between the source
           |  code supplied and those in the list.  If a match is found,
           |  link the `source' field to the name of the source.
           */

         f.dsnsource = (struct source *) NULL;

#ifdef SRCNAMES
         for (ii=0; ii<nsources; ii++)
         {  struct source *psrc = sources[ii];

#ifdef DEBUG1
            DMESS "Matching source %d with %d ?\n",
                    source, psrc->number);
#endif
            if (source == psrc->number)
            {  f.dsnsource = psrc;
               break;
            }
         }
#ifdef DEBUG
         if (f.dsnsource == (struct source *) NULL)
            DMESS "No match found for source %d\n",source);
#ifdef DEBUG1
         else
            DMESS "Matching source %d :: %s\n",
               f.dsnsource->number,
               f.dsnsource->name);
#endif
#endif
#endif

          /*  Search the font family list for a match with the supplied
           |  font family code and link the font family field if a match
           |  is found.
           */

         f.fontfamily = (char *) NULL;
         for (ii=0; ii<nfamilies; ii++)
         {  struct family *pfam = families[ii];
#ifdef DEBUG1
            DMESS "Matching family %d to %d ?\n",
                    f.familyid,pfam->number);
#endif
            if (f.familyid == pfam->number)
            {  f.fontfamily = pfam->name;
               break;
            }
         }
#ifdef DEBUG1
         if (f.fontfamily == ((char *) NULL))
            DMESS "No match found for font family %d\n",f.familyid);
         else
            DMESS "Method matches font family %s\n",f.fontfamily);
#endif

          /*  Now allocate space for a permanent copy of the structure
           |  and copy it across; also add pointer to the structure into
           |  the (temporary) pointer list.
           */

         f.fontname = (char *)MALLOC (1, strlen(fn)+1 );
         if (f.fontname == (char *) NULL)
          {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                             "available font (name)");
             exit(0);         /* redundant, as CGMerror will do it  */
          }
         strcpy(f.fontname,fn);
         pf = (struct isofont *) MALLOC(1,sizeof(f));
         if (pf == (struct isofont *) NULL)
          {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                             "available font (structure)");
             exit(0);         /* redundant, as CGMerror will do it  */
          }
         *pf = f;
         *ptmfonts++ = pf;
         nmfonts[method]++;
      }

      fclose(fileid);
      if (nmfonts[method] >= MAXFPERM)
         CGMERROR(func,ERR_XSFPERM,WARNING,"");

       /*  Now allocate space for a permanent copy of the list of pointers
        |  to all the (permanent) isofont structures.
        */

      mfonts[method] = pp = (struct isofont **)
                       MALLOC(nmfonts[method], sizeof(isofontp));
      if (pp == (struct isofont **) NULL)
       {  (void) CGMerror(func,ERR_NOFNTSPACE,FATAL,
                          "available font (list)");
          exit(0);         /* redundant, as CGMerror will do it  */
          }
      for (ii=0; ii<nmfonts[method]; ii++)
         pp[ii] = tmfonts[ii];
#ifdef DEBUG
      DMESS "METHOD file read for method %d - %d entries (%s)\n",
              method, nmfonts[method],mname[method]);
#endif
   }

   fclose(mfile);
#ifdef DEBUG
   DMESS "METHOD files all read in\n");
#endif
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      check_size()                                               |
      |                                                                 |
      |      For raster fonts adjusts fontid depending on character     |
      |      height.                                                    |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static Index check_size( struct textatt *txtatt, Enum method,
                         struct isofont **fm)
#else
static Index check_size( txtatt, method, fm)

struct textatt *txtatt;
Enum method;
struct isofont **fm ;
#endif
{
   long height;
   struct isofont *pf = *fm;

/*  Return '1' if method only has one font  */

   if ( nmfonts[method] < 1 ) return (Index) 1;

/*  Return fontid if font is scalable (dsnsize = 0) */

   if ( pf->dsnsize == 0 ) return ( pf->fontid );

/*  Loop through fonts until height if less than maxsize
    or maxsize is zero */

   height = txtatt->height * txtatt->shared->ygrain;

   while ( pf->maxsize && (height > pf->maxsize) ) pf = *(++fm);

   return ( pf->fontid );
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      read_rest()                                                |
      |                                                                 |
      |      Reads the rest of a line into a character variable.        |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static void read_rest( FILE *fileid, char *loc, int len)
#else
static void read_rest( fileid, loc, len )

FILE *fileid;
char *loc;
int len;
#endif

{  char *nm, c;
   int i;

   nm = loc;
   while ((c = getc(fileid)) == ' ') ;
   *nm++ = c;
   for (i=1; i<len; i++)
      if ((c = getc(fileid)) == '\n')
      {  *nm = '\0';
         break;
      }
      else
         *nm++ = c;
   return;
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      strip_font()                                               |
      |                                                                 |
      |      Strips a font of any weight, width or variant modifiers    |
      |      and returns the font family name and the attributes        |
      |      that have been stripped off.                               |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static Logical strip_font( char *font, char *string)
#else
static Logical strip_font( font, string )

char *font, *string;
#endif

{  int i, l = strlen(font) - strlen(string) + 1;

#ifdef DEBUG
   DMESS "Into strip_font with `%s' and `%s'\n",font,string);
#endif
   for (i=0; i<l; i++)

      if (match_substring(string,font+i))

      {  char work[256];

#ifdef DEBUG
         DMESS "Removing string at position %d\n",i);
#endif
         strncpy(work,font,i);
         work[i] = '\0';
         if (i<l)
            strcat(work,font+i+strlen(string));
         strcpy(font,work);
#ifdef DEBUG
         DMESS "New font string `%s' after removing `%s'\n",font,string);
#endif
         return TRUE;
      }
   return FALSE;
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      match_string()                                             |
      |                                                                 |
      |      Checks whether one string exactly matches another, but     |
      |      ignores the case of both.                                  |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static Logical match_string( char *a, char *b)
#else
static Logical match_string( a, b )

char *a, *b;
#endif

{  int i, l, la=strlen(a), lb= strlen(b);
   char *pa, *pb;

/*
   if ((l=strlen(a)) != strlen(b)) return FALSE;
*/
   l = ( la > lb ? lb : la );

   for (i=0, pa=a, pb=b; i<l; i++, pa++, pb++)
   {
      if ( *pa == '-' && *pb == ' ' ) continue;
      if ( *pa == ' ' && *pb == '-' ) continue;
      if (toupper(*pa) != toupper(*pb)) return FALSE;
   }

/* check if longer string has only trailing spaces */

   for ( ; i<la; i++) if ( *pa++ != ' ' ) return FALSE;
   for ( ; i<lb; i++) if ( *pb++ != ' ' ) return FALSE;

   return TRUE;
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      match_substring()                                          |
      |                                                                 |
      |      Checks whether the second string (the longer) contains     |
      |      the first string at its start.                             |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static Logical match_substring( char *a, char *b)
#else
static Logical match_substring( a, b )

char *a, *b;
#endif

{  int i, l = strlen(a);
   char *pa, *pb;

   for (i=0, pa=a, pb=b; i<l; i++, pa++, pb++)
   {
      if ( *pa == '-' && *pb == ' ' ) continue;
      if ( *pa == ' ' && *pb == '-' ) continue;
      if (toupper(*pa) != toupper(*pb)) return FALSE;
   }
   return ( *pb == ' ' ? TRUE : FALSE );
}

     /*-----------------------------------------------------------------*
      |                                                                 |
      |      match_attrib()                                             |
      |                                                                 |
      |      Search the `available' methods for an implementation       |
      |      font that matches the request.                             |
      |                                                                 |
      *-----------------------------------------------------------------*/

#ifdef PROTO
static Logical match_attrib(struct request req,
                            Enum *available, struct result *res)
#else
static Logical match_attrib( req, available, res )

struct request  req;
Enum           *available;
struct result  *res;
#endif

     /*   Congratulations on reading this far.  If you have been
      |   concentrating, you will know that this routine is the
      |   whole reason for this module to be written.  Until it is
      |   called, various degrees of cheating have been going on,
      |   honouring requests for fonts by name rather than by attribute.
      |   From the size (of the code, let alone the data) of this system,
      |   it should be obvious that matching by attributes is the real
      |   target.
      |
      |   This routine is called to achieve just that.  The request
      |   is stored in `req' and we will give back the result in `res'.
      |
      |   The potential complexity of this routine is in the fact that
      |   we have to allow a degree of fuzzy matching, or no match will
      |   ever be found.  The attributes of the font are principally
      |   stylistic:
      |
      |      actual font:      family
      |
      |      design code:      class
      |                        subclass
      |                        group
      |
      |      variant:          posture
      |                        weight
      |                        propwidth
      |                        structure
      |
      |   To this are added three mere implementation details:
      |
      |                        height
      |                        rotation
      |                        character set (2 off)
      |
      |   At present, `rotation' is always set to zero, but this should
      |   be fixed before dumb laser printers - that store the fonts at
      |   each of four rotations - are added to the devices serviced.
      |
      |   Also at present, the `character set' is a pale imitation of
      |   what it should be, given all the hard work we have done on the
      |   CGM character set list and the [alternate] character set index.
      |   For the present it is a crude value as documented in the
      |   c_fudge.fnt file.
      |
      |   The routine works by successively scanning the list of
      |   `available' fonts, each time calculating a score for the
      |   match to the `req' and each time changing the weighting
      |   factors for 9 of the above 11 attributes.  Character height
      |   and character set are dealt with separately.
      */

#define ALLPASS   9
#define STARTPASS 0
#define ENDPASS   9

{
   Index cset;
   int ics, ngood = 0;

#ifdef DEBUG
   DMESS "Matching attributes: \
fam %d var %d %d %d %d  dsn %d.%d.%d ht %d cset %d\n",
           req.familyid,
           req.var.posture,
           req.var.weight,
           req.var.propwidth,
           req.var.structure,
           req.dsn.class,
           req.dsn.subclass,
           req.dsn.group,
           req.height,
           req.cset[0] );
#endif

   for (ics = 0; ics < 2; ics++)
   {
      Enum *pmethod, bestmethod;
      long besterror;
      Index bestfont;

      cset = req.cset[ics];
      besterror = 99999;
      bestmethod = available[0];
      bestfont = mfonts[bestmethod][0]->fontid;
#ifdef DEBUG1
      DMESS "Default values: method %d font %d cset %d\n",
                                bestmethod, bestfont, cset );
#endif

      for (pmethod = available; *pmethod > (Enum)0; pmethod++)
      {
         Enum method = *pmethod;
         int nfont, i;
         struct isofont **mfont;

         if ( (nfont = nmfonts[method]) < 1 ) continue;
         mfont = mfonts[method];
#ifdef DEBUG
         DMESS "    method %d with %d fonts (%s)\n",
                method,nfont,mname[method]);
#endif
         for (i=0; i<nfont; i++)
         {
            struct isofont *pf = mfont[i];
            long error = 0;

#ifdef DEBUG1
         DMESS "    height %d  dsn size %d  cset %d (%d)\n",
                req.height, pf->dsnsize, pf->cset, cset);
#endif
            if ( pf->dsnsize ) /* fixed size font */
            {
               if ( req.height < pf->minsize ) continue;
               if ( pf->maxsize && req.height > pf->maxsize ) continue;
            }
            if (cset != pf->cset) continue;

            if (req.var.structure != pf->variant.structure)
               error += 1;

            if (req.familyid != pf->familyid)
               error += 2;

            if (req.dsn.group != pf->dsngroup.group)
               error += 4;

            if (req.dsn.subclass != pf->dsngroup.subclass)
               error += 8;

            error += 16 * abs((int)req.var.propwidth -
                              (int)pf->variant.propwidth);

            if (req.var.posture != pf->variant.posture)
               error += 256;

            error += 512 * abs((int)req.var.weight - (int)pf->variant.weight);

            if (req.dsn.class != pf->dsngroup.class)
               error += 8192;

#ifdef DEBUG
            DMESS "    Font %d (%s) error %d\n",
                    pf->fontid, pf->fontname, error);
#endif
            if (error < besterror)
            {
               besterror = error;
               bestmethod = method;
               bestfont = pf->fontid;
            }
         }
      }
      res->method[ics] = bestmethod;
      res->font[ics] = bestfont;
#ifdef DEBUG
      DMESS "    Best method: %d  Best font: %d\n", bestmethod, bestfont);
#endif
      ngood++;
   }

   if (ngood == 2)
      return TRUE;
   else
      return FALSE;
}                        /*  routine  */

#ifdef PROTO
Logical FNTscale ( Enum *method, int *fontid, float *cheight, float scale, float gran)
#else
Logical FNTscale ( method, fontid, cheight, scale, gran)
Enum *method;
int *fontid;
float *cheight, scale, gran;
#endif


{   float nheight = *cheight*scale;
    long height=(long)nheight*gran;
    struct isofont **fm,*pf;
    int j=0;

    do
    {
        fm = &mfonts[*method][j++];
        pf = *fm;
    }
    while (pf->fontid != *fontid);


/* If font is scalable (dsnsize = 0), scale it and return */

   if ( pf->dsnsize == 0 )
      {   *cheight=nheight;
          return TRUE;
      }

/* Otherwise, look for a point size that is equal or smaller than the new height
   If there is one, take it (and scale the char height accordingly), else
   return FALSE and alter char height to match smallest point size.
*/

   for (;;)
     {
          if ((pf->dsnsize <= height) || (pf->minsize==0)) break;
          pf = *(--fm);
      }

   *fontid=pf->fontid;

   if (pf->dsnsize > height)
   {   *cheight=(float)((pf->dsnsize)/gran);
       return FALSE;
   }
   else
   {   *cheight=nheight;
       return TRUE;
   }
}
