/*
   Copyright (c) 1991 - 1993 Heinz W. Werntges.  All rights reserved.
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

/** to_mf.c: 	Converter to Metafont and misc. TeX formats
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
 **/

#include <stdio.h>
#include <stdlib.h>
#include "bresnham.h"
#include "hp2xx.h"



extern	float	xmin, xmax, ymin, ymax;



void	to_mftex (PAR *p, FILE *td, int mode)
{
PlotCmd		cmd;
HPGL_Pt		pt1;
float		coord2mm;
FILE		*md;
int		pensize, chars_out = 0, max_chars_out = 210, np = 1;
char		*ftype, *scale_cmd, *pen_cmd,
		*poly_start, *poly_next, *poly_last, *poly_end,
		*draw_dot, *exit_cmd;

  switch (mode)
  {
    case 0:	/* Metafont mode	*/
	ftype		= "METAFONT";
	scale_cmd	= "mode_setup;\nbeginchar(\"Z\",%4.3fmm#,%4.3fmm#,0);\n";
	pen_cmd		= "pickup pencircle scaled 0.%1dmm;\n";
	poly_start	= "draw(%4.3fmm,%4.3fmm)";
	poly_next	= "--(%4.3fmm,%4.3fmm)";
	poly_last	= "--(%4.3fmm,%4.3fmm);\n";
	poly_end	= ";\n";
	draw_dot	= "drawdot(%4.3fmm,%4.3fmm);\n";
	exit_cmd	= "endchar;\nend;\n";
	break;
    case 1:	/* TeX (em-Specials) mode	*/
	ftype		= "emTeX-specials";
	scale_cmd	= "\\unitlength1mm\n\\begin{picture}(%4.3f,%4.3f)\n";
	pen_cmd		= "\\special{em:linewidth 0.%1dmm}\n";
	poly_start	= "\\put(%4.3f,%4.3f){\\special{em:moveto}}\n";
	poly_next	= "\\put(%4.3f,%4.3f){\\special{em:lineto}}\n";
	poly_last	= poly_next;
	poly_end	= "";
	draw_dot	= "\\put(%4.3f,%4.3f){\\makebox(0,0)[cc]{.}}\n";
	exit_cmd	= "\\end{picture}\n";
	break;
    case 2:	/* TeX (epic) mode	*/
	ftype		= "TeX (epic)";
	scale_cmd	= "\\unitlength1mm\n\\begin{picture}(%4.3f,%4.3f)\n";
	pen_cmd		= "\\linethickness{0.%1dmm}\n";
	poly_start	= "\\drawline(%4.3f,%4.3f)";
	poly_next	= "(%4.3f,%4.3f)";
	poly_last	= "(%4.3f,%4.3f)\n";
	poly_end	= "\n";
	draw_dot	= "\\put(%4.3f,%4.3f){\\picsquare}\n";
	exit_cmd	= "\\end{picture}\n";
	break;
    case 3:	/* TeXcad (\emline-Macros) mode	*/
	ftype		= "TeXcad compatible";
	scale_cmd	= "\\unitlength=1mm\n\\begin{picture}(%4.3f,%4.3f)\n";
	pen_cmd		= "\\special{em:linewidth 0.%1dmm}\n\\linethickness{ 0.%1dmm}\n";
	poly_start	= "\\emline{%4.3f}{%4.3f}{%d}";
	poly_next	= "{%4.3f}{%4.3f}{%d}\n";
	poly_last	= poly_next;
	poly_end	= "";
	draw_dot	= "\\put(%4.3f,%4.3f){\\makebox(0,0)[cc]{.}}\n";
	exit_cmd	= "\\end{picture}\n";
	break;
  }

  if (!p->quiet)
	fprintf(stderr,"\n\n- Writing %s code to \"%s\"\n", ftype,
		*p->outfile == '-' ? "stdout" : p->outfile);

  if (p->is_color)
	fprintf(stderr, "\nWARNING: MF/TeX modes ignore colors!\n");

  if (*p->outfile != '-')
  {
	if ((md = fopen(p->outfile, "w")) == NULL)
	{
		perror("hp2xx (mf/tex)");
		exit(ERROR);
	}
  }
  else
	md = stdout;

  fprintf(md,"%% %s code in %s, created by hp2xx\n",
	ftype, p->outfile);
  fprintf(md, scale_cmd, p->width, p->height);

  pensize = p->pensize[p->pen];
  if (pensize != 0)
	if (mode == 3)
		fprintf(md, pen_cmd, pensize, pensize);
	else
		fprintf(md, pen_cmd, pensize);

  /* Factor transforming the coordinate values into millimeters: */
  coord2mm = p->height / (ymax-ymin);


  while ((cmd = PlotCmd_from_tmpfile()) != EOF)
	switch (cmd)
	{
	  case NOP:
		break;

	  case SET_PEN:
		if ((p->pen = fgetc(td)) == EOF)
		{
			perror("Unexpected end of temp. file: ");
			exit (ERROR);
		}
		pensize = p->pensize[p->pen];
		if (pensize != 0)
		{
			if (chars_out)	/* Finish up old polygon */
			{
				fprintf(md, poly_end);
				chars_out = 0;
			}
			if (mode == 3)
				fprintf(md, pen_cmd, pensize, pensize);
			else
				fprintf(md, pen_cmd, pensize);
		}
		break;

	  case MOVE_TO:
		HPGL_Pt_from_tmpfile (&pt1);
		if (pensize == 0 || mode == 3)
			break;
		if (chars_out)		/* Finish up old polygon */
			fprintf(md, poly_end);
		chars_out =  fprintf(md, poly_start,
			(pt1.x-xmin) * coord2mm, (pt1.y-ymin) * coord2mm);
		break;

	  case DRAW_TO:
		if (mode == 3)	/* Needs special treatment: no polygons!	*/
		{
			chars_out =  fprintf(md, poly_start,
			  (pt1.x-xmin) * coord2mm, (pt1.y-ymin) * coord2mm, np++);
			HPGL_Pt_from_tmpfile (&pt1);
			chars_out += fprintf(md, poly_next,
			  (pt1.x-xmin) * coord2mm, (pt1.y-ymin) * coord2mm, np++);
			break;
		}

		HPGL_Pt_from_tmpfile (&pt1);
		if (pensize == 0)
			break;
		if (chars_out > max_chars_out)
					/* prevent overlong lines */
		{
			fprintf(md, poly_last,
			  (pt1.x-xmin) * coord2mm, (pt1.y-ymin) * coord2mm);
			chars_out =  fprintf(md, poly_start,
			  (pt1.x-xmin) * coord2mm, (pt1.y-ymin) * coord2mm);
		} else
			chars_out += fprintf(md, poly_next,
			  (pt1.x-xmin) * coord2mm, (pt1.y-ymin) * coord2mm);
		break;

	  case PLOT_AT:
		HPGL_Pt_from_tmpfile (&pt1);
		if (chars_out)		/* Finish up old polygon */
		{
			fprintf(md, poly_end);
			chars_out = 0;
		}
		if (pensize == 0)
			break;
		fprintf(md, draw_dot,
			(pt1.x-xmin) * coord2mm, (pt1.y-ymin) * coord2mm);
		break;

	  default:
		fprintf(stderr,"Illegal cmd in temp. file!");
		exit (ERROR);
	}


  if (chars_out)			/* Finish up old polygon */
  {
	fprintf(md, poly_end);
	chars_out = 0;
  }

  fprintf(md, exit_cmd);		/* Add file trailer	*/

  if (md != stdout)
	fclose(md);

  if (!p->quiet)
	fputc ('\n', stderr);
}
