/*
   Copyright (c) 1991 - 1994 Heinz W. Werntges.  
   Parts Copyright (c) 1995 Emmanuel Bigler, (c) 1999 Martin Kroeker.
   All rights reserved. Distributed by Free Software Foundation, Inc.

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

/** to_tvec.c:    Converter to misc. TeX-oriented vector formats:
 **              Metafont, several TeX formats, simple HP-GL
 **
 ** 91/01/19  V 1.00  HWW  Derived from HPtoGF.c
 ** 91/02/10  V 1.01  HWW  "zaehler" removed
 ** 91/02/15  V 1.02  HWW  stdlib.h supported
 ** 91/02/18  V 1.03  HWW  some int's changed to float's
 ** 91/06/09  V 1.04  HWW  New options added; some simplifications done
 ** 91/10/15  V 1.05  HWW  ANSI_C
 ** 91/11/20  V 1.06  HWW  Many changes for "SPn;" support
 ** 92/01/13  V 1.06a HWW  debugged
 ** 92/02/27  V 1.07b HWW  TeX modes added (epic, emTeX specials)
 ** 92/05/17  V 1.07c HWW  Output to stdout if outfile == '-'
 ** 92/05/19  V 1.07d HWW  Warning if color mode
 ** 92/12/10  V 1.08a HWW  CAD (TeXcad) mode added
 ** 92/12/12  V 1.08b HWW  Info line now interprets outfile=='-' as "stdout"
 ** 93/04/12  V 1.08c HWW  Fix for J. Post's report re: \emline
 ** 93/09/01  V 1.09a HWW  Mode 5 (Simple HP-GL) added
 ** 93/12/30  V 1.09b HWW  Mode 5: Pen number preserved
 ** 94/02/14  V 1.10a HWW  Adapted to changes in hp2xx.h
 ** 95/03/23  V 1.11  E.B. new mode 6 gnuplot ascii format
 **/

#include <stdio.h>
#include <stdlib.h>

#ifdef ATARI
#include <string.h>
#include <ctype.h>
#endif

#include "bresnham.h"
#include "pendef.h"
#include "hp2xx.h"




int
to_mftex (const GEN_PAR *pg, const OUT_PAR *po, int mode)
{

PlotCmd         cmd;
HPGL_Pt         pt1;
float           xcoord2mm, ycoord2mm;
FILE            *md = NULL;
int             pensize, pencolor,pen_no, chars_out = 0, max_chars_out = 210;
int		np = 1, err = 0;
char            *ftype="", *scale_cmd="", *pen_cmd="",
		*poly_start="", *poly_next="", *poly_last="", *poly_end="",
		*draw_dot="", *exit_cmd="";
#ifdef ATARI
int             i;
FILE            *csfile;
char            *csname, *pos1, *pos2, *special_cmd, *tempch;
HPGL_Pt         old_pt;
#endif


  switch (mode)
  {
    case 0:     /* Metafont mode        */
	ftype           = "METAFONT";
	scale_cmd       = "mode_setup;\nbeginchar(\"Z\",%4.3fmm#,%4.3fmm#,0);\n";
	pen_cmd         = "pickup pencircle scaled 0.%1dmm;\n";
	poly_start      = "draw(%4.3fmm,%4.3fmm)";
	poly_next       = "--(%4.3fmm,%4.3fmm)";
	poly_last       = "--(%4.3fmm,%4.3fmm);\n";
	poly_end        = ";\n";
	draw_dot        = "drawdot(%4.3fmm,%4.3fmm);\n";
	exit_cmd        = "endchar;\nend;\n";
	break;
    case 1:     /* TeX (em-Specials) mode       */
	ftype           = "emTeX-specials";
	scale_cmd       = "\\unitlength1mm\n\\begin{picture}(%4.3f,%4.3f)\n";
	pen_cmd         = "\\special{em:linewidth 0.%1dmm}\n";
	poly_start      = "\\put(%4.3f,%4.3f){\\special{em:moveto}}\n";
	poly_next       = "\\put(%4.3f,%4.3f){\\special{em:lineto}}\n";
	poly_last       = poly_next;
	poly_end        = "";
	draw_dot        = "\\put(%4.3f,%4.3f){\\makebox(0,0)[cc]{.}}\n";
	exit_cmd        = "\\end{picture}\n";
	break;
    case 2:     /* TeX (epic) mode      */
	ftype           = "TeX (epic)";
	scale_cmd       = "\\unitlength1mm\n\\begin{picture}(%4.3f,%4.3f)\n";
	pen_cmd         = "\\linethickness{0.%1dmm}\n";
	poly_start      = "\\drawline(%4.3f,%4.3f)";
	poly_next       = "(%4.3f,%4.3f)";
	poly_last       = "(%4.3f,%4.3f)\n";
	poly_end        = "\n";
	draw_dot        = "\\put(%4.3f,%4.3f){\\picsquare}\n";
	exit_cmd        = "\\end{picture}\n";
	break;
    case 3:     /* TeXcad (\emline-Macros) mode */
	ftype           = "TeXcad compatible";
	scale_cmd       = "\\unitlength=1mm\n\\begin{picture}(%4.3f,%4.3f)\n";
	pen_cmd         = "\\special{em:linewidth 0.%1dmm}\n\\linethickness{ 0.%1dmm}\n";
	poly_start      = "\\emline{%4.3f}{%4.3f}{%d}";
	poly_next       = "{%4.3f}{%4.3f}{%d}%%\n";
		/* %% = Fix for John Post's bug report	*/
	poly_last       = poly_next;
	poly_end        = "";
	draw_dot        = "\\put(%4.3f,%4.3f){\\makebox(0,0)[cc]{.}}\n";
	exit_cmd        = "\\end{picture}\n";
	break;
#ifdef ATARI
    case 4:      /* CS-Graphics specials for ATARI TeX */
	ftype           = "CS-TeX specials";
	scale_cmd       = "\\unitlength1mm\n\\begin{draw}{%4.3f}{%4.3f}{%s}\n";
	special_cmd     = "\\put(0,0){\\special{CS!i %s}}\n";
	pen_cmd         = "w 0.%1dmm\n";
	poly_start      = "%4.3f %4.3f l ";
	poly_next       = "%4.3f %4.3f\n";
	poly_last       = poly_next;
	poly_end        = "";
	draw_dot        = "\\put(%4.3f,%4.3f){\\makebox(0,0)[cc]{.}}\n";
	exit_cmd        = "\\end{draw}\n";
	break;
#endif
    case 5:     /* HP-GL mode */
	ftype           = "Simple HP-GL";
	scale_cmd       = "";
	pen_cmd         = "SP%1d;";
	poly_start      = "PA;PU%g,%g;";
	poly_next       = "PD%g,%g;";
	poly_last       = poly_next;
	poly_end        = "";
	draw_dot        = "PU%g,%g;PD%g,%g;PU;";
	exit_cmd        = "";
	break;
      case 6:     /* gnuplot ascii input from HP-GL mode */
  	ftype           = "gnuplot ASCII";
  	scale_cmd       = "";
  	pen_cmd         = "#SP1\n";	/* Not fully implemented!! */
  	poly_start      = "#PA\n\n#PU\n%g  %g\n";
  	poly_next       = "#PD\n%g  %g\n";
  	poly_last       = poly_next;
  	poly_end        = "";
  	draw_dot        = "\n#PU\n%g  %g\n#PD\n%g  %g\n\n#PU\n\n";
  	exit_cmd        = "";
  	break;
	case 7:	/* DXF */
	ftype		= "DXF";
	scale_cmd	= "  0\nSECTION\n  2\nENTITIES\n  0\n";
	pen_cmd		= ""; /*FIXME*/
	poly_start	= "LINE\n  8\n0\n 10\n%g\n 20\n%g\n 30\n0.0\n";
	poly_next	= " 11\n%g\n 21\n%g\n 31\n0.0\n  0\n";
	poly_last	= poly_next;
	poly_end	= "";
	draw_dot	= ""; /*FIXME*/
	exit_cmd	= "ENDSEC\n  0\nEOF\n";
	break;
    case 8: /* SVG */
	ftype		= "SVG";
	scale_cmd	= "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n<svg width=\"100%%\" height=\"100%%\" viewBox=\"0 0 %4.3f %4.3f\" xmlns=\"http://www.w3.org/2000/svg\"><g>\n";
	pen_cmd		= "</g><g style=\"stroke:rgb(%d,%d,%d); fill:none; stroke-width:%d\" >\n";
	poly_start	= "<path d=\"M %4.3f, %4.3f \n";
	poly_next	= "	L %4.3f, %4.3f \n";
	poly_last	= "	L %4.3f, %4.3f \n\" />\n";
	poly_end	= "\" />\n";
	draw_dot	= "<path d=\"M %4.3f,%4.3f L %4.3f %4.3f\" />\n";
	exit_cmd	= "</g>\n</svg>\n";
	break;
}

#ifdef ATARI
  if (mode == 4)
  {
	csname = (char *)calloc(strlen(p->outfile)+5, sizeof(char));
	tempch = (char *)calloc(strlen(p->outfile)+5, sizeof(char));
	strcpy(csname, p->outfile);

	for (i=0; i<strlen(csname); i++)
		csname[i] = (isupper(csname[i]) ? tolower(csname[i]) : csname[i]);

	strcpy(tempch, csname);
  }
#endif


  if (*po->outfile != '-')
  {

#ifdef ATARI
	if (mode == 4)
	{
		pos1 = strrchr(csname, 46);
		pos2 = strrchr(csname, 92);

		if (pos1 != NULL && pos1 > pos2)
			pos1[0] = 0;

		strcat(csname, ".tex\0");

		if (!strcmp(csname, tempch))
		{
			pos1 = strrchr(tempch, 46); *pos1 = 0;
			strcat(tempch, ".csg\0");
		}


		if ((csfile = fopen(csname, "a")) == NULL ||
		    (md     = fopen(tempch, "w")) == NULL)
		{
			free(csname);
			free(tempch);
			PError("hp2xx mf/tex");
			err = ERROR;
			goto MF_exit;
		}

	}
	else
	{
		csfile = stdout;

#endif
		if ((md = fopen(po->outfile, "w")) == NULL)
		{
			PError("hp2xx (mf/tex)");
			err = ERROR;
			goto MF_exit;
		}
#ifdef ATARI
	}
#endif

  }
  else
  {
	md = stdout;
#ifdef ATARI
	csfile = stdout;
#endif
  }

#ifdef ATARI
  if (!pg->quiet)
	if (mode == 4)
		Eprintf ("\n\n- Writing %s code to \"%s\"\n", ftype,
			*po->outfile == '-' ? "stdout" : tempch);
	else
		Eprintf ("\n\n- Writing %s code to \"%s\"\n", ftype,
			*po->outfile == '-' ? "stdout" : po->outfile);
#else
  if (!pg->quiet)
	Eprintf ("\n\n- Writing %s code to \"%s\"\n", ftype,
		*po->outfile == '-' ? "stdout" : po->outfile);
#endif

  if (pg->is_color)
	Eprintf ( "\nWARNING: MF/TeX modes ignore colors!\n");

#ifdef ATARI
  if (mode == 4)
  {
	if (!pg->quiet)
	  Eprintf ( "- TEX-Input file is \"%s\"\n", csname);

	pos1 = strchr(tempch, 92);
	while (pos1 != NULL)
	{
		*pos1 = 47; pos1 = strchr(tempch, 92);
	}

	fprintf(csfile,"%% %s code in %s, created by hp2xx\n",
		ftype, tempch);
	fprintf(csfile, scale_cmd, po->width, po->height, tempch);
	fprintf(csfile, special_cmd, tempch);
	fprintf(md, "CS-Graphics V 1\nr\nu 1mm\n");
	free(csname);
	free(tempch);
  }
  else
  {
#endif
	if (mode < 5 )
	{
		fprintf(md,"%% %s code in %s, created by hp2xx\n",
			ftype, po->outfile);
		fprintf(md, scale_cmd, po->width, po->height);
	}

	if (mode == 7 ) fprintf(md, scale_cmd);
	if (mode == 8 ) fprintf(md, scale_cmd, po->width, po->height);
#ifdef ATARI
  }
#endif

  pen_no  = DEFAULT_PEN_NO;
  pensize = pt.width[pen_no];
                                                                                                 
  if (pensize != 0)
	switch (mode) {
	   case 3:
		fprintf(md, pen_cmd, pensize, pensize);
		break;
	   case 5:
		fprintf(md, pen_cmd, pen_no);
		break;
	   case 7:
	   	break;
	   case 8:
	   	fprintf(md, pen_cmd, 0,0,0,pensize);
		break;
	   default:
		fprintf(md, pen_cmd, pensize);
		break;
	}

  if (mode == 5)
  {
	xcoord2mm = 1.0;
	ycoord2mm = 1.0;
  }
  else
  {
	  /* Factor transforming the coordinate values into millimeters: */
	  xcoord2mm = po->width  / (po->xmax - po->xmin);
	  ycoord2mm = po->height / ( po->ymax - po->ymin);
  }

  if (mode ==8) {
  	xcoord2mm *= 2.83;
  	ycoord2mm *= 2.83;
  	}

  while ((cmd = PlotCmd_from_tmpfile()) != CMD_EOF)
	switch (cmd)
	{
	  case NOP:
		break;

	  case SET_PEN:
		if ((pen_no = fgetc(pg->td)) == EOF)
		{
			PError("Unexpected end of temp. file: ");
			err = ERROR;
			goto MF_exit;
		}
		pensize = pt.width[pen_no];
		if (pensize != 0)
		{
			if (chars_out)  /* Finish up old polygon */
			{
				fprintf(md, poly_end);
				chars_out = 0;
			}
			switch (mode) {
			   case 3:
				fprintf(md, pen_cmd, pensize, pensize);
				break;
			   case 5:
				fprintf(md, pen_cmd, pen_no);
				break;
			   case 8:
			        pencolor=pt.color[pen_no];
			   	fprintf(md, pen_cmd, pt.clut[pencolor][0],
			   	pt.clut[pencolor][1],pt.clut[pencolor][2],
			   	pensize);
			        break;	
			   default:
				fprintf(md, pen_cmd, pensize);
				break;
			}
		}
		break;

          case DEF_PW:
                if(load_pen_width_table(pg->td)<0) {
                    PError("Unexpected end of temp. file");
                    err = ERROR;
                    goto MF_exit;
                }
  pensize = pt.width[pen_no];
  if (pensize != 0)
	switch (mode) {
	   case 3:
		fprintf(md, pen_cmd, pensize, pensize);
		break;
	   case 5:
		fprintf(md, pen_cmd, pen_no);
		break;
	   case 8:
		break;
	   default:
		fprintf(md, pen_cmd, pensize);
		break;
	}
	break;
	  case MOVE_TO:
		HPGL_Pt_from_tmpfile (&pt1);
		if (pensize == 0 || mode == 3 || mode == 4 || mode == 7 )
			break;
		if (chars_out)          /* Finish up old polygon */
			fprintf(md, poly_end);
    if (mode == 8) pt1.y = po->ymax -pt1.y;

		chars_out =  fprintf(md, poly_start,
			(pt1.x - po->xmin) * xcoord2mm,
			(pt1.y - po->ymin) * ycoord2mm);

		break;

	  case DRAW_TO:
		if (mode == 3 || mode == 7)  /* Needs special treatment: no polygons!        */
		{
			chars_out =  fprintf(md, poly_start,
			  (pt1.x - po->xmin) * xcoord2mm,
			  (pt1.y - po->ymin) * ycoord2mm, np++);
			HPGL_Pt_from_tmpfile (&pt1);
			chars_out += fprintf(md, poly_next,
			  (pt1.x - po->xmin) * xcoord2mm,
			  (pt1.y - po->ymin) * ycoord2mm, np++);
			break;
		}
#ifdef ATARI
		else if (mode == 4)
		{
			old_pt = pt1;
			chars_out = fprintf(md, poly_start,
			  (old_pt.x - po->xmin) * xcoord2mm,
			  (old_pt.y - po->ymin) * ycoord2mm, np++);
			HPGL_Pt_from_tmpfile (&pt1);
			chars_out += fprintf(md, poly_next,
			  (pt1.x - old_pt.x) * xcoord2mm,
			  (pt1.y - old_pt.y) * ycoord2mm, np++);
			old_pt = pt1;
			break;
		}
#endif

		HPGL_Pt_from_tmpfile (&pt1);
    if (mode == 8) pt1.y = po->ymax -pt1.y;

		if (pensize == 0)
			break;


		if (chars_out > max_chars_out)
					/* prevent overlong lines */
		{
			fprintf(md, poly_last,
			  (pt1.x - po->xmin) * xcoord2mm,
			  (pt1.y - po->ymin) * ycoord2mm);
			chars_out =  fprintf(md, poly_start,
			  (pt1.x - po->xmin) * xcoord2mm,
			  (pt1.y - po->ymin) * ycoord2mm);
		} else
			chars_out += fprintf(md, poly_next,
			  (pt1.x - po->xmin) * xcoord2mm,
			  (pt1.y - po->ymin) * ycoord2mm);
		break;

	  case PLOT_AT:
		HPGL_Pt_from_tmpfile (&pt1);
		if (chars_out)          /* Finish up old polygon */
		{
			fprintf(md, poly_end);
			chars_out = 0;
		}
		if (pensize == 0)
			break;

#ifdef ATARI
		if (mode == 4)
			fprintf(csfile, draw_dot,
				(pt1.x - po->xmin) * xcoord2mm,
				(pt1.y - po->ymin) * ycoord2mm);

		else
#endif
			if (mode == 5 || mode == 8)
				fprintf(md, draw_dot,
					(pt1.x - po->xmin) * xcoord2mm,
					(pt1.y - po->ymin) * ycoord2mm,
					(pt1.x + 1.0 - po->xmin) * xcoord2mm,
					(pt1.y - po->ymin) * ycoord2mm);
			else
				fprintf(md, draw_dot,
					(pt1.x - po->xmin) * xcoord2mm,
					(pt1.y - po->ymin) * ycoord2mm);
		break;

	  case DEF_PC:
		     if(load_pen_color_table(pg->td)<0) {
                    PError("Unexpected end of temp. file");
                    err = ERROR;
                    goto MF_exit;
                	}

			Eprintf("Warning, no PC support in vector modes!");
			break;
	  default:
		Eprintf ("Illegal cmd in temp. file!");
		err = ERROR;
		goto MF_exit;
	}


  if (chars_out)			/* Finish up old polygon */
  {
	fprintf(md, poly_end);
	chars_out = 0;
  }

#ifdef ATARI
  if (mode == 4)
	fprintf(csfile, exit_cmd);
  else
#endif
	fprintf(md, exit_cmd);		/* Add file trailer     */


MF_exit:

  if (md != stdout && md != NULL)
	fclose(md);
#ifdef ATARI
  if (csfile != stdout && csfile != NULL)
	fclose(csfile);
#endif

  if (!pg->quiet)
	Eprintf ("\n");
  return err;
}
