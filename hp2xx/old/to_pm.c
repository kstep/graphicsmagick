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

/** to_pm.c: OS/2 Presentation Manager preview for HP2xx
 **
 ** 92/10/26  V 1.00  HWW  Originating, based on some code from H. Szillat
 ** 92/12/20  V 1.10  HWW  Considered usable after trial-and-error improvements
 ** 93/09/03  V 1.11  HWW  Adapted to emx 0.8g
 ** 94/02/14  V 1.20a HWW  Adapted to changes in hp2xx.h
 **
 ** NOTES:
 **   1) This is beta software (actually, my first PM project)
 **      - so don't expect a clean code (yet).
 **
 **   2) Currently, I do not intend to support this PM previewer beyond
 **      the time when the full-screen OS/2 previewer (based on emx 0.8f)
 **      becomes available. This is mainly due to two reasons:
 **         -- The PM version lacks a redirector of stderr into a separate
 **            window. I don't know how to write one nor am I willing to.
 **         -- The full-screen version should run on both DOS (386, 486)
 **            and OS/2 2.x.
 **      However, I am aware that a PM version is desirable. Does anyone
 **      out there know an EASY way how to redirect hp2xx's outputs from
 **      stderr into a window without changing hp2xx itself??
 **/

#define	INCL_PM

#include <os2emx.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bresnham.h"
#include "hp2xx.h"

#define far	/* Not needed in 32 bit mode	*/

HAB	hab;
HPS	hps;
HMQ	hmq;
HWND	hwndFrame, hwndClient;
QMSG	qmsg;
RECTL	rctl;

static	PicBuf	*pbuf;


/* Leftover from HS's compiler ? :	*/

#pragma call(same_ds => off)
#pragma data(heap_size=> 3000)


/**
 ** Screen sizes (in pels):
 **/
static int	scr_width;
static int	scr_height;


static void
repaint (HWND hwnd)
{
int	row_c, x;
POINTL	ptl;
RowBuf	*row;

  WinQueryUpdateRect (hwnd, &rctl);
  hps = WinBeginPaint(hwnd,NULL,&rctl);
  if (hps == NULL)
  {
	DosBeep( 1760, 300);
	return;
  }
  GpiErase(hps);	/* Should fill rctl with "background"	*/

/**
 ** Drawing routine: Set all non-background pels within invalid rctl
 **
 ** NOTE: There might be much faster bitblt ops available, but we have
 ** to cope here with the internal bitmap, which must remain portable,
 ** and the easiest *portable* bitblt works bit-by-bit.
 **/
  for (row_c=ptl.y=rctl.yBottom; row_c < rctl.yTop; row_c++,ptl.y++)
  {
	row = get_RowBuf (pbuf, row_c);
	for (x=rctl.xLeft; x < rctl.xRight; x++)
	{
		switch (index_from_RowBuf(row, x, pbuf))
		{
		  case xxBackground:					continue;
		  case xxForeground:	GpiSetColor (hps, CLR_BLACK);	break;
		  case xxRed:		GpiSetColor (hps, CLR_RED);	break;
		  case xxGreen:		GpiSetColor (hps, CLR_GREEN);	break;
		  case xxBlue:		GpiSetColor (hps, CLR_BLUE);	break;
		  case xxCyan:		GpiSetColor (hps, CLR_CYAN);	break;
		  case xxMagenta:	GpiSetColor (hps, CLR_PINK);	break;
		  case xxYellow:	GpiSetColor (hps, CLR_YELLOW);	break;
		}
		ptl.x = x;
		GpiSetPel (hps, &ptl);
	}
  }
  WinEndPaint(hps);
};



MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
  switch(msg){
    case WM_SIZE:
      WinInvalidateRect(hwnd, NULL, FALSE); 
      return 0;

    case WM_PAINT:
      repaint (hwnd);
      return 0;

    case WM_CHAR:
      if(!(CHARMSG(&msg)->fs &KC_KEYUP))
         return 0;

      switch((CHARMSG(&msg)->chr)%256)
      {
          case   /*1*256+*/ 27 /* esc  */:
            WinInvalidateRect(hwnd,NULL,FALSE);
            return 0;

          case /* 28*256+*/13 /* Ent1 */:
            DosBeep(1000,300);
            return 0;
      };
      return 0;

    case WM_BUTTON1DOWN:
    case WM_BUTTON2DOWN:
    case WM_BUTTON3DOWN:
      break;
  }
  return WinDefWindowProc(hwnd, msg, mp1, mp2);
}




CHAR szClientClass[]="HP2xx";
static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
			    FCF_BORDER        | FCF_MINBUTTON|
			    FCF_SHELLPOSITION | FCF_TASKLIST |
			    FCF_NOBYTEALIGN;



static void
win_close()
{
  WinDestroyWindow   (hwndFrame);
  WinDestroyMsgQueue (hmq);
  WinTerminate	     (hab);
}



static int
win_open (int x, int y, int w, int h)
{
int	 cx_frame, cy_frame;
/* emx0.8g: not needed!
HPOINTER WinQuerySysPointer(HWND, LONG, BOOL);
*/

  hab = WinInitialize(0);
  hmq = WinCreateMsgQueue(hab, 0);

  WinRegisterClass(hab, szClientClass, (PFNWP) ClientWndProc, 0L, 0);

  hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
				 WS_VISIBLE | WS_MAXIMIZED,
				 (ULONG far *) &flFrameFlags,
                                 szClientClass, NULL, 0L,
                                 (HMODULE) NULL, 0, &hwndClient);

  scr_width	= WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);
  scr_height	= WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);

  cx_frame	=   WinQuerySysValue (HWND_DESKTOP, SV_CXBORDER)  << 1;
  cy_frame	= ( WinQuerySysValue (HWND_DESKTOP, SV_CYBORDER)  << 1 ) +
		    WinQuerySysValue (HWND_DESKTOP, SV_CYTITLEBAR);

  if (x+w+cx_frame > scr_width || y+h+cy_frame > scr_height)
  {
	win_close();
	Eprintf ("Window exceeds screen limits !\n" );
	return ERROR;
  }

  WinSetWindowPos(hwndFrame, HWND_TOP,
		x - WinQuerySysValue (HWND_DESKTOP, SV_CXBORDER) + 1,
		scr_height - h - y - cy_frame,
		w + cx_frame, h + cy_frame,
		SWP_MOVE | SWP_SIZE | SWP_ACTIVATE | SWP_SHOW);

  WinSendMsg(hwndFrame, WM_SETICON,
	     WinQuerySysPointer(HWND_DESKTOP, SPTR_APPICON, FALSE),
	     NULL);
  return 0;
}



int
PicBuf_to_PM (const GEN_PAR *pg, const OUT_PAR *po)
{
  if (!pg->quiet)
  {
	Eprintf ("\nPM preview follows.\n");
	Eprintf ("Close window to end graphics mode\n");
  }

  pbuf = po->picbuf;

  if (win_open( (int)(po->xoff * po->dpi_x / 25.4),
		(int)(po->yoff * po->dpi_y / 25.4),
		po->picbuf->nb << 3, po->picbuf->nr ) )
	return ERROR;

  while(WinGetMsg(hab, (QMSG far *) &qmsg, NULL, 0, 0))
	WinDispatchMsg(hab, (QMSG far *) &qmsg);

  win_close();
  return 0;
}

