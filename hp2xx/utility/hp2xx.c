/*
   Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.
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

/** std_main.c: Traditional user interface for hp2xx
 **
 ** 94/02/14  V 1.00  HWW  Derived from hp2xx.c
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif /* WIN32 */
#include "../sources/bresnham.h"
#include "../sources/pendef.h"
#include "../sources/hp2xx.h"
#include "../sources/getopt.h"


extern	mode_list	ModeList[];

static	short Logfile_flag = FALSE;




void
Eprintf(const char* fmt, ...)
{
va_list	ap;

  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
}



void
PError (const char* msg)
{
   perror (msg);
}




void
SilentWait (void)
{
char	dummy[80];
#ifdef UNIX
FILE	*tty;
#endif
/**
 ** Get anything typed including '\n' if stderr does NOT go to a file
 ** or else the user may be invisibly prompted.
 **
 ** According to a suggestion from A. Bagge, in UNIX pipe mode stdin
 ** will be replaced by /dev/tty.
 **/
  if (!Logfile_flag)
  {
#ifdef UNIX
	if ((tty = fopen("/dev/tty","r")) != NULL)
	{
		fgets (dummy, 80, tty);
		fclose(tty);
	}
	else
#endif
		fgets (dummy,80,stdin);
  }
}




void
NormalWait (void)
{
#ifdef	UNIX
  if (getenv("TERM") == (char *) NULL)
	return;
#endif
  Eprintf ("\nPress <Return> to continue ...\n");
  SilentWait ();
}




void
action_oldstyle (GEN_PAR *pg, IN_PAR *pi, OUT_PAR *po)
{
int	err;

  if (!pg->quiet)
	Send_version();

  /**
   ** Phase 1: HP-GL --> TMP file data
   **/
  err = HPGL_to_TMP (pg, pi);
  if (err)
	return;
  cleanup_i (pi);


  /**
   ** Phase 2: TMP file re-scaling
   **/
  adjust_input_transform (pg, pi, po);


  /**
   ** Phase 3: (a) TMP file --> Vector formats
   **/
  err = TMP_to_VEC (pg, po);
  if (err == 0)
	return;
  if (err == ERROR)
  {
	cleanup (pg, pi, po);
	return;
  }

  /**
   ** Phase 3: (b) TMP file --> Raster image
   **/
  if (TMP_to_BUF (pg, po))
  {
	cleanup(pg, pi, po);
	return;
  }

  /**
   ** Phase 3: (c) Raster image --> output formats
   **/
  err = BUF_to_RAS (pg, po);

  if (err == 1)
	Eprintf("%s: Not implemented!\n", pg->mode);

  cleanup(pg, pi, po);
}




static void
process_opts (int argc, char* argv[],
		const char* shortopts, struct option longopts[],
		GEN_PAR* pg, IN_PAR* pi, OUT_PAR* po)
{
int	c, i,j, longind;
char	*p, cdummy;

  while ((c=getopt_long(argc,argv, shortopts, longopts, &longind)) != EOF)
	switch (c)	/* Easy addition of options ... */
	{
	  case 'a':
		pi->aspectfactor = atof (optarg);
		if (pi->aspectfactor <= 0.0)
		{
			Eprintf("Aspect factor: %g illegal\n",
				pi->aspectfactor);
			exit(ERROR);
		}
		break;

	  case 'c':
		i = strlen(optarg);
		if ((i<1) || (i>8))
		{
			Eprintf("Invalid pencolor string: %s\n", optarg);
			exit(ERROR);
		}
		for (j=1, p = optarg; j <= i; j++, p++)
		{
		    switch (*p-'0')
		    {
			case xxBackground:pt.color[j] = xxBackground; break;
			case xxForeground:pt.color[j] = xxForeground; break;
			case xxRed:	  pt.color[j] = xxRed;	  break;
			case xxGreen:	  pt.color[j] = xxGreen;	  break;
			case xxBlue:	  pt.color[j] = xxBlue;	  break;
			case xxCyan:	  pt.color[j] = xxCyan;	  break;
			case xxMagenta:	  pt.color[j] = xxMagenta;  break;
			case xxYellow:	  pt.color[j] = xxYellow;	  break;
			default :
				  Eprintf(
				    "Invalid color of pen %d: %c\n", j, *p);
				  exit(ERROR);
		    }
		    if (pt.color[j] != xxBackground &&
			pt.color[j] != xxForeground)
				pg->is_color = TRUE;
		}
		pi->hwcolor=TRUE;
		break;

	  case 'C':
		pi->center_mode = TRUE;
		break;

	  case 'd':
		switch (po->dpi_x = atoi (optarg))
		{
		  case 75:
			break;
		  case 100:
		  case 150:
		  case 300:
		  case 600:
			if ((!pg->quiet) && (strcmp(pg->mode,"pcl")==0) &&
				po->specials == 0)
			Eprintf(
			  "Warning: DPI setting is no PCL level 3 feature!\n");
			break;
		  default:
			if ((!pg->quiet) && (strcmp(pg->mode,"pcl")==0))
			Eprintf(
			  "Warning: DPI value %d is invalid for PCL mode\n",
				po->dpi_x);
			break;
		}
		break;

	  case 'D':
		po->dpi_y = atoi (optarg);
		if ((!pg->quiet) && strcmp(pg->mode,"pcl")==0 && po->specials==0)
			Eprintf("Warning: %s\n",
			"Different DPI for x & y is invalid for PCL mode");
		break;

	  case 'F':
		po->formfeed = TRUE;
		break;

	  case 'f':
		po->outfile = optarg;
		break;

	  case 'h':
		pi->height = atof (optarg);
		if (pi->height < 0.1)
			Eprintf("Warning: Small height: %g mm\n", pi->height);
		if (pi->height > 300.0)
			Eprintf("Warning: Huge  height: %g mm\n", pi->height);
		break;

	  case 'i':
		po->init_p = TRUE;
		break;

	  case 'l':
		pg->logfile = optarg;
		if (freopen(pg->logfile, "w", stderr) == NULL)
		{
			PError ("Cannot open log file");
			Eprintf("Error redirecting stderr\n");
			Eprintf("Continuing with output to stderr\n");
		}
		else
			Logfile_flag = TRUE;
		break;

	  case 'm':
		pg->mode = optarg;
		for (i=0; ModeList[i].mode != XX_TERM; i++)
			if (strcmp(ModeList[i].modestr, pg->mode) == 0)
				break;
		if (ModeList[i].mode == XX_TERM)
		{
			Eprintf("'%s': unknown mode!\n", pg->mode);
			Eprintf("Supported are:\n\t");
			print_supported_modes();
			Send_Copyright();
		}
		break;

	  case 'n':
	  	pg->nofill = TRUE;
	  	break;

	  case 'N':
	  	pg->no_ps = TRUE;
	  	break;
	  		  	
	  case 'o':
		pi->xoff = atof (optarg);
		if (pi->xoff < 0.0)
		{
			Eprintf("Illegal X offset: %g < 0\n", pi->xoff);
			exit(ERROR);
		}
		if (pi->xoff > 210.0)	/* About DIN A4 width */
		{
			Eprintf("Illegal X offset: %g > 210\n", pi->xoff);
			exit(ERROR);
		}
		break;

	  case 'O':
		pi->yoff = atof (optarg);
		if (pi->yoff < 0.0)
		{
			Eprintf("Illegal Y offset: %g < 0\n", pi->yoff);
			exit(ERROR);
		}
		if (pi->yoff > 300.0)	/* About DIN A4 height */
		{
			Eprintf("Illegal Y offset: %g > 300\n", pi->yoff);
			exit(ERROR);
		}
		break;

	  case 'p':
		i = strlen(optarg);
		if ((i<1) || (i>8))
		{
			Eprintf("Invalid pensize string: %s\n", optarg);
			exit(ERROR);
		}
		for (j=1, p = optarg; j <= i; j++, p++)
		{
			if ((*p < '0') || (*p > '9'))
			{
				Eprintf("Invalid size of pen %d: %c\n",	j, *p);
				exit(ERROR);
			}
			pt.width[j] = *p - '0';
			if (pg->maxpensize < pt.width[j])
				pg->maxpensize = pt.width[j];
		}
		pi->hwsize=TRUE;
		break;

	  case 'P':
		if (*optarg == ':')
		{
			pi->first_page = 0;
			optarg++;
			if (sscanf(optarg,"%d", &pi->last_page) != 1)
				pi->last_page = 0;
		}
		else
			switch (sscanf(optarg,"%d%c%d",
				&pi->first_page, &cdummy, &pi->last_page))
			{
			  case 1:
				pi->last_page = pi->first_page;
				break;

			  case 2:
				if (cdummy == ':')
				{
					pi->last_page = 0;
					break;
				}
				/* not ':' Syntax error -- drop through	*/
			  case 3:
				if (cdummy == ':')
					break;
				/* not ':' Syntax error -- drop through	*/
			  default:
				Eprintf("Illegal page range.\n");
				usage_msg (pg, pi, po);
				exit(ERROR);
			}
		break;

	  case 'q':
		pg->quiet = TRUE;
		break;

	  case 'r':
		pi->rotation = atof(optarg);
		break;

	  case 'S':
		po->specials = atoi (optarg);
		break;

	  case 's':
		pg->swapfile = optarg;
		break;

	  case 't':
		pi->truesize = TRUE;
		break;

	  case 'V':
		po->vga_mode = atoi (optarg);
		break;

	  case 'w':
		pi->width = atof (optarg);
		if (pi->width < 0.1)
			Eprintf("Warning: Small width: %g mm\n", pi->width);
		if (pi->width > 300.0)
			Eprintf("Warning: Huge  width: %g mm\n", pi->width);
		break;

	  case 'v':
		Send_version();
		exit (NOERROR);

	  case 'x':
		pi->x0 = atof (optarg);
		break;

	  case 'X':
		pi->x1 = atof (optarg);
		break;

	  case 'y':
		pi->y0 = atof (optarg);
		break;

	  case 'Y':
		pi->y1 = atof (optarg);
		break;

	  case 'H':
	  case '?':
	  default:
		usage_msg (pg, pi, po);
		exit (ERROR);
	}
}



/**
 ** main(): Process command line & call action routine
 **/

int	main (int argc, char *argv[])
{
GEN_PAR	Pg;
IN_PAR	Pi;
OUT_PAR	Po;
int	i;

char	*shortopts = "a:c:d:D:f:h:l:m:o:O:p:P:r:s:S:V:w:x:X:y:Y:CFHinqtvN";
struct	option longopts[] =
{
	{"mode",	1, NULL,	'm'},
	{"pencolors",	1, NULL,	'c'},
	{"pensizes",	1, NULL,	'p'},
	{"pages",	1, NULL,	'P'},
	{"quiet",	0, NULL,	'q'},
	{"nofill",	0, NULL,	'n'},
	{"no_ps",	0, NULL,	'N'},

	{"DPI",		1, NULL,	'd'},
	{"DPI_x",	1, NULL,	'd'},
	{"DPI_y",	1, NULL,	'D'},

	{"PCL_formfeed",0, NULL,	'F'},
	{"PCL_init",	0, NULL,	'i'},
	{"PCL_Deskjet",	1, NULL,	'S'},

	{"outfile",	1, NULL,	'f'},
	{"logfile",	1, NULL,	'l'},
	{"swapfile",	1, NULL,	's'},

	{"aspectfactor",1, NULL,	'a'},
	{"height",	1, NULL,	'h'},
	{"width",	1, NULL,	'w'},
	{"truesize",	0, NULL,	't'},

	{"x0",		1, NULL,	'x'},
	{"x1",		1, NULL,	'X'},
	{"y0",		1, NULL,	'y'},
	{"y1",		1, NULL,	'Y'},

	{"xoffset",	1, NULL,	'o'},
	{"yoffset",	1, NULL,	'O'},
	{"center",	0, NULL,	'C'},

#ifdef DOS
	{"VGAmodebyte",	1, NULL,	'V'},
#endif
	{"help",	0, NULL,	'H'},
	{"version",	0, NULL,	'v'},
	{NULL,		0, NULL,	'\0'}
};


  preset_par (&Pg, &Pi, &Po);
  if (argc == 1)
  {
	usage_msg (&Pg, &Pi, &Po);
	exit (ERROR);
  }
#ifdef WIN32
   /* set stdin and stdout to binary mode: */
   _setmode( _fileno(stdin), _O_BINARY );
   _setmode( _fileno(stdout), _O_BINARY );
#endif /* WIN32 */
  process_opts (argc, argv, shortopts, longopts, &Pg, &Pi, &Po);

/**
 ** Determine internal mode code
 **/

  for (i=0; ModeList[i].mode != XX_TERM; i++)
/*	if (strncmp(Pg.mode, ModeList[i].modestr,
		strlen(ModeList[i].modestr)) == 0)*/
	if (strcmp(Pg.mode, ModeList[i].modestr)==0)
	{
		Pg.xx_mode = ModeList[i].mode;
		break;
	}
/**
 ** Place consistency checks & adjustments here if you like
 **/

  if (Po.dpi_y == 0)
	Po.dpi_y = Po.dpi_x;

/**
 ** Action loop over all input files
 **/

  if (optind == argc)		/* No  filename: use stdin	*/
  {
	Pi.in_file = "-";
	autoset_outfile_name (Pg.mode, Pi.in_file, &Po.outfile);
	action_oldstyle (&Pg, &Pi, &Po);
  }
  else	for ( ; optind < argc; optind++)
	{			/* Multiple-input file handling: */
		Pi.in_file = argv[optind];
		autoset_outfile_name (Pg.mode, Pi.in_file, &Po.outfile);
		action_oldstyle (&Pg, &Pi, &Po);
		reset_par (&Pi);
	}

  cleanup (&Pg, &Pi, &Po);
  if (*Pg.logfile)
	fclose (stderr);
  return NOERROR;
}
