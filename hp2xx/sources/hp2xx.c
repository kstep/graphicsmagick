/*
   Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.
   Parts Copyright (c) 1998-2002 Martin Kroeker  All rights reserved.
   
   Distributed by Free Software Foundation, Inc.

This file is part of HP2xx.

HP2xx is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the GNU General Public License, Version 2 or later, for full details.

Everyone is granted permission to copy, modify and redistribute
HP2xx, but only under the conditions described in the GNU General Public
License.  A copy of this license is supposed to have been
given to you along with HP2xx so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/** hp2xx : A converter of HPGL files into popular raster and vector formats
 **
 **	xx =  mf (MetaFont), eps (PostScript), pcl (HP-PCL Level3),
 **	      pre(view), pcx (PaintBrush-Format), img (GEM),
 **	      pic (ATARI bitmap), pbm (Portable Bitmap), ...
 **
 ** (c) 1992, 1993  Heinz W. Werntges and the HP2xx team
 **
 ** Author:
 **
 **	HWW	Heinz W. Werntges  (werntges@convex.rz.uni-duesseldorf.de)
 **		Fruchtstr. 2,  40223 Duesseldorf, Germany
 **
 ** New maintainer since version 3.3 and (c) 1999:
 **     MK      Dr. Martin Kroeker (mk@daveg.com, 
 **                                 martin@ruby.chemie.uni-freiburg.de)
 **             Zum Markwald 6, D-63165 Muehlheim, Germany                    
 **
 ** Amiga stuff & PBM & ILBM by:
 **
 **	CHL	Claus H. Langhans (Claus_Langhans@wildcat.fido.de)
 **		Unter den Nussbaeumen 19, 65779 Kelkheim (Ts.), Germany
 **
 **
 ** Atari stuff & IMG by:
 **
 **	NM	Norbert Meyer (sorry - no email)
 **		Reinhold-Schneider-Str. 83, 40595 Duesseldorf
 **
 **	AS	Andreas Schwab (schwab@ls5.informatik.uni-dortmund.de)
 **
 **	JE	Joern Eggers (II040EG@DTHAC11.bitnet)
 **
 ** OS/2 Full-Screen previewer by:
 **
 **	HSz	Host Szillat (szillat@iaas-berlin.d400.de)
 **
 ** X11 previewer by:
 **
 **	MSch	Michael Schoene (michael@nero.uni-bonn.de)
 **		Karolinger Str. 81,  40223 Duesseldorf, Germany
 **
 **
 ** Many thanks to all the other fine people who supported HP2xx by testing,
 ** correcting, porting, suggesting improvements, and otherwise spending
 ** their time on HP2xx, especially:
 **
 ** -- Michael Schmitz
 **  & Gerhard Steger 	 for their invaluable help with VAX/VMS,
 ** -- Juergen Gross     for access to a HP 9000 machine
 ** -- Roland Emmerich   for proofreading, beta-testing, and his HGC support
 ** -- Jon Gillian       for valuable suggestions
 ** -- Lawrence Lowe     for various improvements and contributions
 **-------------------------------------------------------------------------
 **/

/** hp2xx.c:
 **
 ** 91/01/12  V 1.00  HWW  PC version
 ** 91/01/17  V 1.01  HWW  Disk swapping added
 ** 91/01/19  V 1.02  HWW  Reorganized: mf mode revived, pcx mode added
 ** 91/01/29  V 1.03  HWW  SUN portation; HP-specials added
 ** 91/01/31  V 1.04  HWW  (-F) FF added
 ** 91/02/01  V 1.05  HWW  Flag -S added (Deskjet specials)
 ** 91/02/15  V 1.06  HWW  stdlib.h & VAX_C supported
 ** 91/02/19  V 2.01  HWW  file unchanged, but new package release!
 ** 91/06/09  V 2.10  HWW  (beta) More options; options x & y changed to o & O
 ** 91/06/15  V 2.11  HWW  (beta) VGA option added; PCX bug (on SUN) removed
 ** 91/06/20  V 2.12  HWW  Rotate (-r) option & Copyright added
 ** 91/06/20  V 2.12b HWW  Minor reformatting
 ** 91/06/29  V 2.13a HWW  PostScript support
 ** 91/08/28  V 2.14  HWW  ATARI 32K format added (prelim.)
 ** 91/10/15  V 2.14c HWW  atari & stad formats; hwtools.h eliminated
 ** 91/10/20  V 2.14d HWW  ATARI-->pic (packing removed)
 ** 91/10/24  V 2.15  HWW  ATARI-->pic stable now; "LT;" & "LT0;" supported
 ** 91/11/22  V 2.16b HWW  "SPn;" support
 ** 92/01/12  V 2.17  HWW  HPGL scanner improved, to_ps() debugged, ps --> eps,
 **			  More portabel code (ATARI acknowledged)
 ** 92/02/06  V 2.18e HWW  Better parser, IMG & various previews supported,
 **			  preview control via -h, -w; -W removed!
 ** 92/02/21  V 2.19d HWW  LB etc. supported, PG; (option -P) added
 ** 92/03/03  V 2.20c HWW  Modes added: -m epic,em,pac(ATARI) / LB bugs fixed
 ** 92/04/15  V 2.20d HWW  Width & height treated symmetrically now
 ** 92/04/28  V 2.20e HWW  -m pre: -d, -D effective again, default 75; PBM added
 ** 92/05/02  V 2.21a HWW  -m pre is default now. New opt: -l logfile
 ** 92/05/19  V 3.00c HWW  Color support (VGA preview), multiple input files
 ** 92/05/25  V 3.00f HWW  Color support (more modes)
 ** 92/06/09  V 3.01d HWW  XT,YT,TL,SM supported; ATARI update acknowledged;
 **			   Mode list added for easier expansion; debugged
 ** 92/10/20  V 3.02a HWW  LT now fully supported
 ** 92/11/08  V 3.02b HWW  Page range allowed
 ** 92/12/10  V 3.02c HWW  Mode "cad" added; DJ version: bug fix extended;
 ** 92/12/12  V 3.02d HWW  2 bug fixes for option -f
 ** 92/12/16  V 3.03a HWW  Option -t (+truesize) added
 ** 92/12/27  V 3.03b HWW  -S int (for Deskjet color support)
 ** 93/01/02  V 3.10a HWW  Source brushed up; ready for distribution
 ** 93/02/08  V 3.12a HWW  Support of Joern Eggers' cs mode added (ATARI only)
 **			   HAS_X11, HAS_SUNVIEW: added/modified
 ** 93/04/13  V 3.12b HWW  UC added (A. Treindl), small changes; CMYK added,
 **			   New previewer selection scheme // NEW RELEASE 3.1.2
 ** 93/04/25  V 3.12c HWW  Minor bug fixes (to_pcl.c, hpgl.c / Eric)
 ** 93/05/20  V 3.12d HWW  LT1 adjustment; '--version' option added (GNU std)
 ** 93/07/20  V 3.12e HWW  SilentWait() modified (UNIX) according to A. Bagge,
 **			   new OS/2 previewer by HSz, TIFF compression for PCL mode
 ** 93/09/03  V 3.13a HWW  Wait() renamed to NormalWait() (name collision on VAX),
 **			   Option -P : Option parser modified; -m hpgl added
 ** 93/09/22  V 3.13b HWW  Fix in autoset_outfile_name()
 ** 94/01/01  V 3.14a HWW  Additions by L. Lowe
 ** 94/02/14  V 3.20b HWW  Re-structured to facilitate multiple user interfaces
 ** Separate additions to HWW's V 3.20, assembled in this release  by MK:
 ** 94/03/23          EB   Mode "gpt", Gnuplot, added by Emmanuel Bigler
 ** 94/08/08          IMP  Mode "xfig" added by Ian MacPhedran
 ** 97/11/29          MS   Mode "png" added by Michael Schmitz
 ** 99/09/01  V 3.30a MK   CA,CS,IW,SA,SS,NP,PC,PW,PE and charsets1-8 added
 ** 99/09/12  V3.30a2 MK   RO,PS,EW added, bugfixes for IW,PC,PW,EA
 ** 99/10/01  V 3.30  MK   RO/PS changes
 ** 99/12/01  V 3.31  MK   fixes for RO,PS,LB,DI,DR, completed PE 
 ** 00/02/06          MK   fixes for scaling and PE; allow overriding of PC/PW
 ** 00/02/12  V 3.32  MK   Mode "tiff" added by M.Liberi (changed to use ZIP
 **                        instead of patented LZW compression; needs libtiff)
 ** 00/02/26          MK   Mode "escp" (Epson Esc/P2 printer language)
 **/

char	*VERS_NO = "3.4.2";
char	*VERS_DATE = "02/04/15";
char	*VERS_COPYRIGHT = "(c) 1991 - 1994 (V3.20) Heinz W. Werntges";
#if defined(AMIGA)
char	*VERS_ADDITIONS =
	"\tAmiga additions (V 2.00) by Claus Langhans (92/12/16)\n";
#elif defined (ATARI)
char	*VERS_ADDITIONS =
	"\tAtari additions (V 2.10) by N. Meyer / J. Eggers / A. Schwab  (93/01/xx)\n";
#else
char	*VERS_ADDITIONS = "                                (c) 1999 - 2002 Martin Kroeker\n";
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bresnham.h"
#include "pendef.h"
#include "hp2xx.h"



/**
 ** When adding your special mode, add a line here.
 ** Please note the alphabetical order (and keep it).
 ** Also see the definition of "hp2xx_mode" in hp2xx.h.
 **/

mode_list  ModeList[] =
{
	{XX_CAD,	"cad"},	/* LaTeX: TeXcad compatible output	*/
#ifdef	ATARI
	{XX_CS,		"cs"},	/* LaTeX using \special{...} for C. Strunk's TeX	*/
#endif
	{XX_DXF,	"dxf"}, /* AutoCAD DXF format			*/
	{XX_EM,		"em"},	/* LaTeX using \special{em:...}		*/
#ifdef EMF
	{XX_EMF,	"emf"}, /* Microsoft Enhanced Metafile		*/
	{XX_EMP,	"emp"}, /* Microsoft EMF printing		*/
#endif
	{XX_EPIC,	"epic"},/* LaTeX using epic.sty macros		*/
	{XX_EPS,	"eps"},	/* Encapulated PostScript		*/
#ifdef EPSON
        {XX_ESC2,       "esc2"}, /* Epson Esc/P2 printer language       */
#endif
	{XX_FIG,        "fig"}, /* FIG 3.1 Drawing Files                */
	{XX_GPT,        "gpt"}, /* gnuplot vector ascii format          */
	{XX_HPGL,	"hpgl"},/* Simplified HP-GL			*/
#ifdef	AMIGA
	{XX_ILBM,	"ilbm"},/* Special AMIGA format			*/
#endif
	{XX_IMG,	"img"},	/* Digital Research IMG raster format	*/
#ifdef	JPG
	{XX_JPG,	"jpg"},	/* Joint Photography Expert Group JPEG  */
#endif
	{XX_MF,		"mf"},	/* Metafont source output		*/
	{XX_PBM,	"pbm"},	/* Portable Bitmap			*/
	{XX_PCL,	"pcl"},	/* HP-PCL Level 5 printer code		*/
	{XX_PCX,	"pcx"},	/* Paintbrush's PCX raster format	*/
#ifdef	PIC_PAC
	{XX_PAC,	"pac"},	/* for ATARI, e.g. used by StaD		*/
	{XX_PIC,	"pic"},	/* for ATARI. Try to replace by IMG	*/
#endif
#ifdef USEPDF
        {XX_PDF,        "pdf"}, /* Portable Document Format       */
#endif
#ifdef PNG
        {XX_PNG,        "png"}, /* Portable Network Graphics            */
#endif
	{XX_PRE,	"pre"},	/* DEFAULT: Preview on screen		*/
        {XX_RGIP,       "rgip"},/* Uniplex RGIP vector format           */
	{XX_SVG,	"svg"}, /* W3C Scalable vector graphics		*/
#ifdef TIF
        {XX_TIFF,        "tiff"}, /* Tagged image file format            */
#endif
	{XX_TERM,	""},	/* Dummy: List terminator		*/
};



void	print_supported_modes(void)
{
int	i;

  Eprintf ("%s", ModeList[0].modestr);
  for (i=1; ModeList[i].mode != XX_TERM; i++)
	Eprintf (",%s", ModeList[i].modestr);
  Eprintf ("\n");
}




void	Send_version(void)
{
  Eprintf ("\n%s\n%s\n%s\n%s\n%s\n",
  "hp2xx is free software and you are welcome to distribute copies of it",
  "  under certain conditions. There is absolutely no warranty for hp2xx!",
  "For full details, read file COPYING (shipped along with this package),",
  "  or write to:\t\tFree Software Foundation, Inc.",
  "\t\t\t675 Mass Ave, Cambridge, MA 02139, USA");

  Eprintf ("\n%s\tV %s  (%s)   %s\n%s",
	"HP2xx:\tA HPGL converter (xx = mf, eps, pcl, pcx, img ...)\n",
	VERS_NO, VERS_DATE, VERS_COPYRIGHT, VERS_ADDITIONS);
}



void	usage_msg (const GEN_PAR *pg, const IN_PAR *pi, const OUT_PAR *po)
{
if (pg->quiet)
	return;

Send_version();

#ifdef ATARI
  Eprintf ("Usage:\tdouble-click on HP2XX.TTP, type parameters into\n");
  Eprintf ("\tcommand-line with following syntax:\n");
  Eprintf ("\t[options] [hpglfile]\n");
  Eprintf ("\n\t(if command-line offers not enough space for all parameters\n");
  Eprintf ("\tuse a command-line interpreter for starting HP2xx)\n");
#else
  Eprintf ("Usage:\thp2xx [options] [file1 [file2 ...]]\n");
#endif	/* ATARI */

  Eprintf ("\tUnix: Filter usage (.. | hp2xx -q -f- [options] | ..) ok\n");
NormalWait();

  Eprintf ("\nOpt fmt   defaults\tComment:\n");
  Eprintf (
"---------------------------------------------------------------------------\n");
  Eprintf ("-m strg   %s\t\tMode. Valid mode strings are:\n\t\t\t", pg->mode);
print_supported_modes();
  
  Eprintf ("-n        %s\t\tno filling of polygons (ignore FP commands)\n",
  	FLAGSTATE(pg->nofill));
  
  Eprintf ("-f strg   (auto gen.)\tName of output file ('-' = to stdout)\n");
  Eprintf ("-l strg   (stderr)\tName of log file\n");
  Eprintf ("-p strg   %1d%1d%1d%1d%1d%1d%1d%1d\tPensize(s) (in 1/10 mm (mf,ps) or dots (rest)).\n",
        pt.width[1], pt.width[2], pt.width[3], pt.width[4],
        pt.width[5], pt.width[6], pt.width[7], pt.width[8]);
  Eprintf ("\t\t\t\"strg\" must consist of 1 to 8 digits '0'-'9'\n");
  Eprintf ("-c strg   %1d%1d%1d%1d%1d%1d%1d%1d\tPen color(s) (see manual for details).\n",
        pt.color[1], pt.color[2], pt.color[3], pt.color[4],
        pt.color[5], pt.color[6], pt.color[7], pt.color[8]);
  Eprintf ("-P n:n    %d:%d\t\tPage range (0:0 = all pages).\n",
	pi->first_page, pi->last_page);
  Eprintf ("-q        %s\t\tQuiet mode (no diagnostics)\n",
	FLAGSTATE(pg->quiet));
  Eprintf ("-r float%5.1f\t\tRotation angle [deg]. -r90 = landscape\n",
	pi->rotation);
  Eprintf ("-s strg   %s\tName of swap file\n", pg->swapfile);

  Eprintf ("\nBitmap controls:\n");
  Eprintf ("-d int    %d\t\tDPI value for x or x&y, if -D unused.\n",
	po->dpi_x);
  Eprintf ("-D int    %d\t\tDPI value for y ONLY\n", po->dpi_x);
	 /* x, not y! */
NormalWait();

  Eprintf ("\nPCL-exclusive options:\n");
  Eprintf ("-i         %s\tPre-initialize printer\n", FLAGSTATE (po->init_p));
  Eprintf ("-F         %s\tSend a FormFeed at end\n", FLAGSTATE(po->formfeed));
  Eprintf ("-S int     %d\tUse Deskjet special commands (0=off, 1=B/W, 3=CMY, 4=CMYK)\n",
	po->specials);
  Eprintf ("NOTE:    \tOnly valid for -d: 300/150/100/75; -D invalid!\n");

  Eprintf ("\nPCL / PostScript / Preview options:\n");
  Eprintf ("-o float %5.1f\tX offset [mm] of picture\n", pi->xoff);
  Eprintf ("-O float %5.1f\tY offset [mm] of picture\n", pi->yoff);
  Eprintf ("-C           \tFit picture into center of (-a/-h/-w) rectangle\n");

  Eprintf ("\nTiff-exclusive options:\n");
  Eprintf ("-S int     %d\tUse Tiff Compression Format (0/1=None, 2=RLE, 3=G3Fax, 4=G4Fax, 5=LZW, 6=OJpeg, 7=Jpeg, 8=Deflate)\n",
	po->specials);

  Eprintf ("\nSize controls:\n");

  Eprintf ("-a float %5.1f\tAspect factor (x/y correction). Valid: > 0.0\n",
	pi->aspectfactor);
  Eprintf ("-h float %5.1f\tHeight [mm] of picture\n",pi->height);
  Eprintf ("-w float %5.1f\tWidth  [mm] of picture\n",pi->width );
  Eprintf ("-t         %s\tShow true HPGL size. Disables -a -h -w !\n",
	FLAGSTATE (pi->truesize));
  Eprintf ("-x float   -\tManual HPGL-coord range presetting: x0\n");
  Eprintf ("-X float   -\tManual HPGL-coord range presetting: x1\n");
  Eprintf ("-y float   -\tManual HPGL-coord range presetting: y0\n");
  Eprintf ("-Y float   -\tManual HPGL-coord range presetting: y1\n");

#ifdef DOS
  Eprintf ("\n-V int   %d\tVGA mode byte (decimal). Change at own risk!\n",
		po->vga_mode);
#endif

NormalWait();

  Eprintf ("Corresponding long options:\n\n");
  Eprintf ("hp2xx   [--mode] [--colors] [--pensizes] [--pages] [--quiet]\n");
  Eprintf ("\t[--nofill] [--width] [--height] [--aspectfactor] [--truesize]\n");
  Eprintf ("\t[--x0] [--x1] [--y0] [--y1]\n");
  Eprintf ("\t[--xoffset] [--yoffset] [--center]\n");
  Eprintf ("\t[--DPI] [--DPI_x] [--DPI_y]\n");
  Eprintf ("\t[--outfile] [--logfile] [--swapfile]\n");
  Eprintf ("\t[--PCL_formfeed] [--PCL_init] [--PCL_Deskjet]\n");
#ifdef DOS
  Eprintf ("\t[--VGAmodebyte]");
#endif
  Eprintf ("\t[--help] [--version]\n");


#ifdef PURE_C
  Eprintf("\nPress RETURN key\n");
getchar();
#endif
}




void	reset_par (IN_PAR *pi)
/**
 ** Reset some parameter struct elements which may have been changed
 ** by action() to their defaults
 **/
{
  pi->x0	=  1e10;	/* HP7550A's range is about     */
  pi->x1	= -1e10;	/* [-2^24, 2^24], so we're safe */
  pi->y0	=  1e10;
  pi->y1	= -1e10;
}




void	preset_par (GEN_PAR *pg, IN_PAR *pi, OUT_PAR *po)
/**
 ** Pre-set constant parameter struct elements with reasonable defaults
 **/
{
int	i;

  pi->aspectfactor = 1.0;
  pi->center_mode  = FALSE;
  pi->height	= 200.0;
  pi->width	= 200.0;
  pi->xoff	= 0.0;
  pi->yoff	= 0.0;
  pi->truesize	= FALSE;
  pi->hwcolor	= FALSE;
  pi->hwsize	= FALSE;
  pi->rotation	= 0.0;
  pi->in_file	= "";
  pi->hd	= NULL;
  pi->first_page= 0;
  pi->last_page	= 0;

  po->init_p	= FALSE;
  po->formfeed	= FALSE;
  po->specials	= 0;
  po->dpi_x	= 75;
  po->dpi_y	= 0;
  po->vga_mode	= 18;		/* 0x12: VGA 640x480, 16 colors */
  po->picbuf	= NULL;
  po->outfile	= "";

  pg->logfile	= "";
  pg->swapfile	= "hp2xx.swp";
  pg->mode	= "pre";
  pg->td	= NULL;
  pg->xx_mode	= XX_PRE;
  pg->nofill	= FALSE;
  pg->quiet	= FALSE;
  pg->maxpensize= 1;		/* in pixel or 1/10 mm		*/
  pg->maxcolor	= 1;		/* max. color index		*/
  pg->maxpens=8;
  pg->is_color	= FALSE;

  pt.width[0] = 0;		/* in pixel or 1/10 mm		*/
  pt.color[0] = xxBackground;
  for (i=1; i<=NUMPENS; i++) {
	pt.width[i]	= 1;	/* in pixel or 1/10 mm		*/
        pt.color[i]	= xxForeground;
  }

  set_color_rgb(xxBackground,255,255,255);
  set_color_rgb(xxForeground,  0,  0,  0);
  set_color_rgb(xxRed,       255,  0,  0);
  set_color_rgb(xxGreen     ,  0,255,  0);
  set_color_rgb(xxBlue      ,  0,  0,255);
  set_color_rgb(xxCyan      ,  0,255,255);
  set_color_rgb(xxMagenta   ,255,  0,255);
  set_color_rgb(xxYellow    ,255,255,  0);

  reset_par (pi);
}




void	Send_Copyright(void)
/**
 **	Remnant of older (non-GNU) releases. Leave here if you like
 **/
{
/*
static	unsigned char msg[] =
	{0xaf,0xa8,0xcd,0xd5,0x97,0xdd,0xdd,0x9f,
	 0x85,0x8d,0xc6,0x8c,0x85,0xed,0x8b,0x85,
	 0xf2,0xc0,0xd7,0xcb,0xd1,0xc2,0xc0,0xd6,
	 0xaf,0xa8,0xa5};
unsigned char	*p;

  p = msg;
  while (*p!=0xa5)
	Eprintf("%c", (*p++ ^ 0xa5));
  exit  (COPYNOTE);
*/
exit(-1);
}




void	autoset_outfile_name(
		const char* mode, const char *in_name,
		char** outfile)
{
int	len, i;

  if (**outfile=='-')	/* If output explicitly to stdout:		*/
	return;		/*    then nothing's to do here			*/

  if (isascii(**outfile) && strlen(*outfile) >0){/* If this looks like an output file name:	*/
	return;		/*    Just accept it! Add validity check later?	*/
}
  if (*in_name == '-')	/* If input from stdin				*/
	len = 0;
  else
	len = strlen(in_name);

  if (len == 0)		/* If input from stdin:				*/
  {			/*    then supply a default file name		*/
	*outfile="hp2xx.out";
	return;
  }

#if 0
  if (strcmp(mode,"pre") == 0)
	return;		/* If preview mode:				*/
			/*    then output file name is unused		*/
#endif
  for (i=len-1; i; i--)	/* Search for (last) '.' char in path		*/
	if (in_name[i] == '.')
		break;
  i++;

/**
 ** We can assume a valid inp_name file name here.
 ** If not, the following fopen() in main() will fail
 ** and no harm will be done by an incorrect output file name.
 **/

  if ((*outfile = malloc(len+2+strlen(mode))) == NULL)
  {
	Eprintf("Error: No mem for output file name!\n");
	PError ("autoset_outfile_name");
	exit   (ERROR);
  }
  strcpy(*outfile, in_name);

  if (strcmp(mode,"pre") == 0)
	return;		/* If preview mode:				*/
			/*    then file name is used only for window title */

  if (i==1 || len-i > 3) /* No or non-DOS extension: Add mode string	*/
  {
	strcat(*outfile, ".");
	strcat(*outfile, mode);	/* Mode string is extension!	*/
  }
  else
	strcpy(*outfile+i, mode);	/* Replace extension	*/
}



/**************************************************************************
 **
 ** cleanup_x ():
 **
 ** Call these functions to close & remove the temp. and input file
 **	as well as to free the raster picture buffer.
 ** Calling is ok even if the buffer is already freed or the files
 **	are already closed, so just call them when in doubt.
 **/

void
cleanup_g (GEN_PAR *pg)
{
  if (pg != NULL && pg->td != NULL)
  {
	fclose (pg->td);
	pg->td = NULL;
  }

#if defined(DOS) && defined (GNU)
/**
 ** GNU libc.a (DJ's DOS port) bug fix:
 **/
  unlink ("hp2xx.$$$");
#endif
}


void
cleanup_i (IN_PAR *pi)
{
  if (pi != NULL && pi->hd != NULL)
  {
	if (pi->hd != stdin)
		fclose (pi->hd);
	pi->hd = NULL;
  }
}


void
cleanup_o (OUT_PAR *po)
{
  if (po != NULL && po->picbuf != NULL)
  {
	free_PicBuf (po->picbuf);
	po->picbuf = NULL;
  }
}




void
cleanup (GEN_PAR *pg, IN_PAR *pi, OUT_PAR *po)
{
  cleanup_g (pg);
  cleanup_i (pi);
  cleanup_o (po);
}




/**************************************************************************
 **
 ** HPGL_to_TMP ():
 **
 ** This call opens a single HP-GL input file, scans and interprets
 ** its commands, and writes elementary move/draw commands into
 ** a temporary file.
 **	The input file is closed after returning, but the temp. file
 ** is kept open. You may re-use it multiple times. Close it finally!
 **	Calling this function invalidates later processing stages like
 ** the picture buffer.
 **/

int
HPGL_to_TMP (GEN_PAR *pg, IN_PAR *pi)
{
  /**
   ** Clean up previous leftovers (if any)
   **/

  cleanup_g (pg);
  cleanup_i (pi);

  /**
   ** Open HP-GL input file. Use stdin if selected.
   **/

  if (*pi->in_file == '-')
	pi->hd = stdin;
  else
	if ((pi->hd=fopen (pi->in_file, READ_BIN)) == NULL)
	{
		PError("hp2xx");
		return ERROR;
	}

  /**
   ** Open temporary intermediate file.
   **
   ** GNU libc.a (DJ's DOS port) bug fix (part 1 of 2):
   **	 tmpfile() does not seem to work!
   ** See code below for part 2/2 (removing hp2xx.$$$).
   ** NOTE:
   **	If program terminates abnormally, delete hp2xx.$$$ manually!!
   **/

#if defined(DOS) && defined (GNU)
  if ((pg->td = fopen("hp2xx.$$$","w+b")) == NULL)
#elif defined(AMIGA)
  if ((pg->td = fopen("t:hp2xx.tmp","w+b")) == NULL)
#else
  if ((pg->td = tmpfile()) == NULL)
#endif	/** !DOS && GNU	**/
  {
	PError("hp2xx -- opening temporary file");
	return ERROR;
  }

  /**
   ** Convert HPGL data into compact temporary binary file, and obtain
   ** scaling data (xmin/xmax/ymin/ymax in plotter coordinates)
   **/

  read_HPGL (pg, pi);

  if (pi->hd != stdin)
  {
	fclose (pi->hd);
	pi->hd = NULL;
  }
  return 0;
}



/**************************************************************************
 **
 ** TMP_to_VEC ():
 **
 ** Vector mode conversions.
 **	Call any number of times as long as the temp. file is opened.
 **
 ** Returns:
 **	ERROR	if error
 **	1	if mode not found
 **	0 	if successfully processed
 **/

int	TMP_to_VEC (const GEN_PAR *pg, const OUT_PAR *po)
{
  if (pg->td == NULL)
	return ERROR;

  rewind (pg->td);		/* Rewind temp file for re-reading	*/

  switch (pg->xx_mode)
  {
    case XX_MF:
	to_mftex(pg, po, 0);
	return 0;

    case XX_EM:
	to_mftex(pg, po, 1);
	return 0;

    case XX_EPIC:
	to_mftex(pg, po, 2);
	return 0;

    case XX_CAD:
	to_mftex(pg, po, 3);
	return 0;

#ifdef	ATARI
    case XX_CS:
	to_mftex(pg, po, 4);
	return 0;
#endif

#ifdef EMF       /*BAF */
    case XX_EMF:
        to_emf  (pg, po);
        return 0;
    case XX_EMP:
        to_emp  (pg, po);
         return 0;
    case XX_PRE:
        to_emw  (pg, po);
         return 0;
#endif           /*end BAF*/

    case XX_GPT:
        to_mftex(pg, po, 6);
        return 0;
        
    case XX_HPGL:
	to_mftex(pg, po, 5);
	return 0;

    case XX_DXF:
    	to_mftex(pg, po, 7);
    	return 0;
 
    case XX_SVG:
    	to_mftex(pg, po, 8);
    	return 0;
    	
    case XX_EPS:
	to_eps	(pg, po);
	return 0;
#ifdef USEPDF
    case XX_PDF:
	to_pdf	(pg, po);
	return 0;
#endif
    case XX_RGIP:
	to_rgip	(pg, po);
	return 0;

    case XX_FIG:
        to_fig (pg, po);
        return 0;
        
    default:
	return 1;
  }
}



/**************************************************************************
 **
 ** TMP_to_BUF ():
 **
 ** Rasterization into a memory buffer
 **	A pre-requisite for all raster formats. Call any number of times
 **		as long as the temp. file is opened.
 **	An internal raster picture buffer is maintained in memory
 **		by this call.
 **	The picture buffer is kept allocated after this call so that
 **		the time-consuming rasterization is needed only once.
 **		De-allocate at program end or when not needed anymore!
 **
 ** Returns:
 **	ERROR	if error
 **	0 	if successfully processed
 **/

int	TMP_to_BUF (const GEN_PAR *pg, OUT_PAR *po)
{
int	n_rows, n_cols;

  if (pg->td == NULL)
	return ERROR;

  rewind (pg->td);		/* Rewind temp file for re-reading	*/

  cleanup_o (po);

/**
 ** 1) Allocate virtual plotter area
 **/

  size_PicBuf (pg, po, &n_rows, &n_cols);

  if ((po->picbuf = allocate_PicBuf (pg, n_rows, n_cols)) == NULL)
  {
	Eprintf("Fatal error: cannot allocate %d*%d picture buffer\n",
		n_rows, n_cols);
	return ERROR;
  }

/**
 ** Read vectors from temporary file and plot them in memory buffer
 **/

  tmpfile_to_PicBuf (pg, po);

  return 0;
}



/**************************************************************************
 **
 ** BUF_to_RAS ():
 **
 ** Conversion of raster picture buffer into the final output format.
 **
 ** Returns:
 **	ERROR	if error
 **	1 	if output mode not found
 **	0 	if successfully processed
 **/

int	BUF_to_RAS (const GEN_PAR *pg, OUT_PAR *po)
{
  if (po->picbuf == NULL)
	return ERROR;

  switch (pg->xx_mode)
  {
	case XX_PCL:		/* HP PCL Level 3	*/
		return PicBuf_to_PCL (pg, po);

	case XX_PCX:		/* Paintbrush PCX	*/
		return PicBuf_to_PCX (pg, po);

#ifdef	PIC_PAC			/*   To be phased out!	*/
	case XX_PIC:		/* ATARI 32K format	*/
		return PicBuf_to_PIC (pg, po);

	case XX_PAC:		/* ATARI STaD format	*/
		return PicBuf_to_PAC (pg, po);
#endif

#ifdef	AMIGA
	case XX_ILBM:		/* AMIGA IFF-ILBM fmt	*/
		return PicBuf_to_ILBM(pg, po);
#endif

	case XX_IMG:		/* GEM's IMG format	*/
		return PicBuf_to_IMG (pg, po);
#ifdef JPG
	case XX_JPG:           /* JPEG image format */
		return PicBuf_to_JPG (pg, po);
#endif
	case XX_PBM:		/* Portable BitMap fmt	*/
		return PicBuf_to_PBM (pg, po);
#ifdef PNG
        case XX_PNG:            /* Portable Network fmt */
               return PicBuf_to_PNG (pg, po);
#endif

#ifdef TIF
        case XX_TIFF:            /* Tagged image file fmt */
               return PicBuf_to_TIF (pg, po);
#endif
#ifdef EPSON
	case XX_ESC2:		/* Epson Esc/P2	*/
		return PicBuf_to_ESCP2 (pg, po);
#endif
/**
 ** Previewers (depending on hardware platform):
 **/
	case XX_PRE:
#if   defined(HAS_DOS_DJGR)
		return PicBuf_to_DJ_GR (pg, po);
#elif defined(HAS_DOS_HGC)
		return PicBuf_to_HGC	(pg, po);
#elif defined(HAS_DOS_VGA)
		return PicBuf_to_VGA	(pg, po);
#elif defined(HAS_OS2_EMX)
		return PicBuf_to_OS2	(pg, po);
#elif defined(HAS_OS2_PM)
		return PicBuf_to_PM	(pg, po);
#elif defined(HAS_UNIX_X11)
		return PicBuf_to_X11	(pg, po);
#elif defined(HAS_UNIX_SUNVIEW)
		return PicBuf_to_Sunview(pg, po);
#elif defined(ATARI)
		return PicBuf_to_ATARI	(pg, po);
#elif defined(AMIGA)
		return PicBuf_to_AMIGA	(pg, po);
#elif defined(VAX)
		return PicBuf_to_UIS	(pg, po);
#else
		return PicBuf_to_Dummy	();
#endif	/* defined(...)	*/
default:
	return 1;
  }
}



/**********************************************************************
 **
 **  My intention is to facilitate the implementation of various
 **  user interfaces including e.g. a Windows front-end.
 **
 **  Note that e.g. in Windows programs there is a "win_main()" function
 **  instead of "main()", and that waiting for user acknowledgement
 **  or diagnostic output must not use stdin/out/err...
 **
 **  In a separate file, one of possibly several versions of
 **  user interface dependend functions + local utility functions
 **  is supplied. The traditional version can be found in "std_main.c"
 **
 **	Function	Tradiditional equivalent
 **
 **	EPrintf(...)	fprintf(stderr, ...)
 **	PError		perror
 **	SilentWait	-
 **	NormalWait	-
 **	...		main
 **/
