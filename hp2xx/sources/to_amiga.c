/*
   Copyright (c) 1992 - 1994 Claus Langhans.  All rights reserved.
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

/**
 ** to_amiga.c: GNU-C++ (AMIGA) preview part of project "hp2xx"
 **
 ** 92/04/10  V 1.00  CHL  Some ideas taken from to_gnu.c and from the RKMs
 ** 92/04/11  V 1.01  CHL  Preview finished
 ** 92/04/12  V 1.02  CHL  Stop ^C
 ** 92/04/13  V 1.03  CHL  Too small windows bug cleaned
 ** 92/12/09  V 2.00  CHL  Added the color preview.
 ** 94/02/14  V 2.10  HWW  Adapted to changes in hp2xx.h. WARNING:
 **			   Adaptations done without compile tests
 **/

/* Standard includes */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>


/* Project includes */

#include "bresnham.h"
#include "hp2xx.h"

/* Amiga includes */

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>

UBYTE           vers[] = "$VER: hp2xx 2.10 (14.02.94)\0\n";

WORD            width_super = 800;
WORD            height_super = 600;

#define LEFTBORDERSIZE 	4
#define RIGHTBORDERSIZE	18
#define TOPBORDERSIZE	11
#define BOTTOMBORDERSIZE	9
#define OPENWIDTH	425
#define OPENHEIGHT	178
#define MINWINWIDTH	90
#define MINWINHEIGHT	40
#define MAXWINWIDTH	32000 - LEFTBORDERSIZE + RIGHTBORDERSIZE
#define MAXWINHEIGHT	32000 - TOPBORDERSIZE + BOTTOMBORDERSIZE
#define	DEPTH_SUPER	3

#ifndef __GNUC__
extern struct GfxBase *GfxBase;
extern struct IntuitionBase *IntuitionBase;
#else
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
#endif				/* __GNUC__ */

struct LayersBase *LayersBase;

#define GetGadgetID(x) (((struct Gadget *)(msg->IAddress))->GadgetID)

#define GetLayerXOffset(x) (x->RPort->Layer->Scroll_X)
#define GetLayerYOffset(x) (x->RPort->Layer->Scroll_Y)

#define UP_DOWN_GADGET    0
#define LEFT_RIGHT_GADGET 1
#define NO_GADGET         2

#define MAXVAL 0xFFFFL

struct Image    Images[2];
struct Screen  *PreviewScreen;

/* The special data needed for the two proportional gadgets */
struct PropInfo GadgetsSInfo[2] =
{
    {FREEVERT | AUTOKNOB, 0, 0, -1, -1,},
    {FREEHORIZ | AUTOKNOB, 0, 0, -1, -1,}
};

/* The usual data needed for any gadget */
struct Gadget   Gadgets[2] =
{

/* Gadgets[0] */
    {&Gadgets[1], -15, 10, 16, -18,
     GRELRIGHT | GRELHEIGHT,
     RELVERIFY | GADGIMMEDIATE | RIGHTBORDER, PROPGADGET | GZZGADGET,
     (APTR) & Images[0], NULL, NULL, NULL,
     (APTR) & GadgetsSInfo[0], UP_DOWN_GADGET, NULL},

/* Gadgets[1] */
    {NULL, 0, -8, -14, 9,
     GRELBOTTOM | GRELWIDTH,
     RELVERIFY | GADGIMMEDIATE | BOTTOMBORDER, PROPGADGET | GZZGADGET,
     (APTR) & Images[1], NULL, NULL, NULL,
     (APTR) & GadgetsSInfo[1], LEFT_RIGHT_GADGET, NULL}
};

struct NewScreen NewPreviewScreen =
{
/* LeftEdge, TopEdge, Width, Height, Depth, */
    0, 0, STDSCREENHEIGHT, STDSCREENWIDTH, DEPTH_SUPER,

/* DetailPen, BlockPen, */
    0, 1,

/* ViewModes, */
    HIRES | LACE,

/* Type, */
    CUSTOMSCREEN,

/* Font */
    NULL,

/* DefaultTitle */
    NULL,

/* Gadgets */
    NULL,

/* CustomBitMap */
    NULL
};


static struct NewWindow NewPreviewWindow =
{
    0, 12,			/* window XY origin relative to TopLeft of
				 * screen */
    OPENWIDTH, OPENHEIGHT,	/* window width and height */
    0, 1,			/* detail and block pens */

    GADGETUP |
    GADGETDOWN |
    NEWSIZE |
    INTUITICKS |
    CLOSEWINDOW,		/* IDCMP flags */

    WINDOWSIZING |
    WINDOWDRAG |
    WINDOWDEPTH |
    WINDOWCLOSE |
    SUPER_BITMAP |
    GIMMEZEROZERO |
    NOCAREREFRESH,		/* other window flags */

    Gadgets,			/* first gadget in gadget list */
    NULL,			/* custom CHECKMARK imagery */
    "hp2xx",			/* window title */
    NULL,			/* custom screen pointer */
    NULL,			/* custom bitmap */
    MINWINWIDTH, MINWINHEIGHT,	/* minimum width and height */
    MAXWINWIDTH, MAXWINHEIGHT,	/* maximum width and height */
    CUSTOMSCREEN		/* destination screen type */
};


static void
brkfunc(int signo)
{
    Eprintf ("Don't send a BREAK, click the close gadget!\n");
}



/* This function provides a simple interface to ScrollLayer */
static VOID
Slide_BitMap(struct Window * window, SHORT Dx, SHORT Dy)
{
    ScrollLayer(0, window->RPort->Layer, Dx, Dy);
}

static VOID
Do_NewSize(struct Window * window)
{
    ULONG           tmp;

    tmp = GetLayerXOffset(window) + window->GZZWidth;
    if (tmp >= width_super)
	Slide_BitMap(window, width_super - tmp, 0);

    NewModifyProp(&Gadgets[LEFT_RIGHT_GADGET], window, NULL, AUTOKNOB | FREEHORIZ,
		  ((GetLayerXOffset(window) * MAXVAL) /
		   (width_super - window->GZZWidth)),
		  NULL,
		  ((window->GZZWidth * MAXVAL) / width_super),
		  MAXVAL,
		  1);

    tmp = GetLayerYOffset(window) + window->GZZHeight;
    if (tmp >= height_super)
	Slide_BitMap(window, 0, height_super - tmp);

    NewModifyProp(&Gadgets[UP_DOWN_GADGET], window, NULL, AUTOKNOB | FREEVERT,
		  NULL,
		  ((GetLayerYOffset(window) * MAXVAL) /
		   (height_super - window->GZZHeight)),
		  MAXVAL,
		  ((window->GZZHeight * MAXVAL) / height_super),
		  1);
}

static VOID
Check_Gadget(struct Window * window, USHORT gadgetID)
{
    ULONG           tmp;
    SHORT           dX = 0;
    SHORT           dY = 0;
    switch (gadgetID) {
    case UP_DOWN_GADGET:
	tmp = height_super - window->GZZHeight;
	tmp = tmp * GadgetsSInfo[UP_DOWN_GADGET].VertPot;
	tmp = tmp / MAXVAL;
	dY = tmp - GetLayerYOffset(window);
	break;
    case LEFT_RIGHT_GADGET:
	tmp = width_super - window->GZZWidth;
	tmp = tmp * GadgetsSInfo[LEFT_RIGHT_GADGET].HorizPot;
	tmp = tmp / MAXVAL;
	dX = tmp - GetLayerXOffset(window);
	break;
    }
    if (dX || dY)
	Slide_BitMap(window, dX, dY);
}

static VOID
Do_MainLoop(struct Window * window)
{
    struct IntuiMessage *msg;
    SHORT           flag = TRUE;
    USHORT          CurrentGadget = NO_GADGET;

    SetDrMd(window->RPort, JAM1);
    Do_NewSize(window);
    while (flag) {

	/* Whenever you want to wait on just one message port */
	/* you can use WaitPort(). WaitPort() doesn't require */
	/* the setting of a signal bit. The only argument it  */
	/* requires is the pointer to the window's UserPort   */
	WaitPort(window->UserPort);
	while (msg = (struct IntuiMessage *) GetMsg(window->UserPort)) {
	    switch (msg->Class) {
	    case CLOSEWINDOW:
		flag = FALSE;
		break;
	    case NEWSIZE:
		Do_NewSize(window);
		break;
	    case GADGETDOWN:
		CurrentGadget = GetGadgetID(msg);
		break;
	    case GADGETUP:
		Check_Gadget(window, CurrentGadget);
		CurrentGadget = NO_GADGET;
		break;
	    }
	    ReplyMsg((struct Message *) msg);
	}
    }
}

int
PicBuf_to_AMIGA(const GEN_PAR *pg, const OUT_PAR *po)
{
    int             row_c, x, y, xoff, yoff, color_index;
    int             NoScreen = FALSE;
    RowBuf         *row;

    struct BitMap  *BigOne;
    struct Window  *window;
    ULONG           RasterSize;
    SHORT           Loop;
    SHORT           Flag;
    const PicBuf   *pb;

    pb = po->picbuf;
    if (pb->depth > 1)
	NoScreen = TRUE;

    if (!pg->quiet)
	Eprintf ("\nAMIGA preview follows.\n");

    xoff = po->xoff * po->dpi_x / 25.4;
    yoff = po->yoff * po->dpi_y / 25.4;


    if (!pg->quiet)
	Eprintf ("Current range: (%d..%d) x (%d..%d) pels\n",
		xoff, (pb->nb << 3) + xoff, yoff, pb->nr + yoff);
    if ((!pg->quiet) &&
	(((pb->nb << 3) + xoff > MAXWINWIDTH) || (pb->nr + yoff > MAXWINHEIGHT))) {
	Eprintf ("\n\007WARNING: Picture ist too large!\n");
	Eprintf ("Continue anyway (^C): ");
	getchar();		/* Simple: Chance for ^C */
    }
    width_super = (pb->nb << 3) + xoff + LEFTBORDERSIZE + RIGHTBORDERSIZE;
    height_super = pb->nr + yoff + TOPBORDERSIZE + BOTTOMBORDERSIZE;
    if (width_super < MINWINWIDTH)
	width_super = MINWINWIDTH;
    if (height_super < MINWINHEIGHT)
	height_super = MINWINHEIGHT;
    if (width_super > MAXWINWIDTH) {
	width_super = MAXWINWIDTH;
	Eprintf ("Error: Window width too big:%d\n", width_super);
	 return ERROR;
    }
    if (height_super > MAXWINHEIGHT) {
	height_super = MAXWINHEIGHT;
	Eprintf ("Error: Window height too big:%d\n", height_super);
	return ERROR;
    }
    NewPreviewWindow.Width = width_super;
    NewPreviewWindow.Height = height_super;
    NewPreviewWindow.MaxWidth = width_super;
    NewPreviewWindow.MaxHeight = height_super;

    if (NewPreviewWindow.Width > NewPreviewWindow.MaxWidth)
	NewPreviewWindow.Width = NewPreviewWindow.MaxWidth;

    if (NewPreviewWindow.Height > NewPreviewWindow.MaxHeight)
	NewPreviewWindow.Height = NewPreviewWindow.MaxHeight;

    if (NewPreviewWindow.Width < NewPreviewWindow.MinWidth)
	NewPreviewWindow.Width = NewPreviewWindow.MinWidth;

    if (NewPreviewWindow.Height < NewPreviewWindow.MinHeight)
	NewPreviewWindow.Height = NewPreviewWindow.MinHeight;

    /*
     * We don 't like a BREAK beyond this point - the window would stay open
     * and the memory allocated !
     */
    signal(SIGINT, brkfunc);

    if (IntuitionBase = (struct IntuitionBase *)
	OpenLibrary("intuition.library", 33L)) {
	if (GfxBase = (struct GfxBase *)
	    OpenLibrary("graphics.library", 33L)) {
	    if (LayersBase = (struct LayersBase *)
		OpenLibrary("layers.library", 33L)) {
		if (PreviewScreen = (struct Screen *) OpenScreen(&NewPreviewScreen)) {
		    if (BigOne = (struct BitMap *) AllocMem(sizeof(struct BitMap), MEMF_PUBLIC | MEMF_CLEAR)) {
			InitBitMap(BigOne, DEPTH_SUPER, width_super, height_super);
			RasterSize = BigOne->BytesPerRow * BigOne->Rows;
			Flag = TRUE;
			for (Loop = 0; Loop < DEPTH_SUPER; Loop++) {
			    BigOne->Planes[Loop] = (PLANEPTR) AllocMem(RasterSize,
				      MEMF_CHIP | MEMF_CLEAR | MEMF_PUBLIC);
			    if (!BigOne->Planes[Loop])
				Flag = FALSE;
			}
			if (Flag) {
			    NewPreviewWindow.BitMap = BigOne;
			    NewPreviewWindow.Screen = PreviewScreen;
			    if (window = (struct Window *) OpenWindow(&NewPreviewWindow)) {
				for (row_c = 0, y = pb->nr + yoff - 1; row_c < pb->nr; row_c++, y--) {
				    if ((!pg->quiet) && (row_c % 10 == 0))
					Eprintf(".");

				    row = get_RowBuf(pb, pb->nr - row_c - 1);
				    for (x = 0; x < pb->nc; x++) {
					color_index = index_from_RowBuf(row, x, pb);
					if (color_index != xxBackground) {
					    SetAPen(window->RPort, color_index);
					    WritePixel(window->RPort, x + xoff, row_c + yoff);
					}
				    }
				}
				if ((!pg->quiet))
				    Eprintf ("\nClick Close Gadget to continue!\n");

				Do_MainLoop(window);

				CloseWindow(window);
			    }
			} else {
			    Eprintf ("Error: Not enough graphics memory to allocate BitPlanes!\n");
			}
			for (Loop = 0; Loop < DEPTH_SUPER; Loop++) {
			    if (BigOne->Planes[Loop]) {
				FreeMem(BigOne->Planes[Loop], RasterSize);
			    }
			}
			FreeMem(BigOne, sizeof(struct BitMap));
		    } else {
			Eprintf ("Error: Not enough graphics memory to allocate BitPlane Structure!\n");
		    }
		    CloseScreen(PreviewScreen);
		} else {
		    Eprintf ("Oh! Couldn't open Screen!\n");
		}
		CloseLibrary((struct Library *) LayersBase);
	    } else {
		Eprintf ("Oh! Couldn't open Layers Library!\n");
	    }

	    CloseLibrary((struct Library *) GfxBase);
	} else {
	    Eprintf ("Oh! Couldn't open Graphics Library!\n");
	}
	CloseLibrary((struct Library *) IntuitionBase);
    } else {
	Eprintf ("Oh! Couldn't open Intuition Library!\n");
    }
    signal(SIGINT, SIG_DFL);
  ERROR_EXIT_2:;
  return 0;	/* HWW: Error conditions should actually produce	*/
		/*	non-zero return codes!				*/
}
