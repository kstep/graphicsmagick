/*
Copyright (c) 2001  Bengt-Arne Fjellner  All rights reserved.

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

/* to_emf.c: 	Converter to Windows Enhanced meta file format+printer for windows;
**				And windows preview
**				tested with visualc 6.0 and c++builder 5.0
**				known to work under win98 and win2000
**
** 01/10/15  V 1.00  BAF  Derived from to_eps.c to create Enhanced MetaFile
** 01/10/17  V 1.01  BAF  Added Windows Print
** 01/10/17  V 1.02  BAF  Added Windows Preview
** 01/10/23  V 1.04  BAF  Updated to 3.4.1
** 01/11/25  V 1.05  BAF  Updated to 3.4.1b
*/

#ifndef EMF
#error Compiling to_emf.c without having defined EMF. this is meaningless.
#endif

#include <windows.h>
#undef ERROR
#undef NUMPENS
#ifdef NOERROR
#undef NOERROR
#endif

#include <stdio.h>
//#include <string.h>
#include <math.h>
#include "bresnham.h"
#include "hp2xx.h"
#include "pendef.h"

extern void reset_tmpfile(void); // in hpgl.c


typedef struct{  // for use by preview Dialog
	const GEN_PAR *pg;
	const OUT_PAR *po;
}PGPO;

#define PRINT 128 // Message number for print menu
#define MARGIN 20 // How wide Empty box around it

static int lines=0,moves=0;  // counters to show how "difficult" this plot is

//*******************************************************************
// move dont draw
static void
emf_move_to(HPGL_Pt *ppt, HANDLE outDC)
{
	MoveToEx( outDC,(int)(ppt->x) ,(int)(ppt->y) ,NULL);
	moves++;
}


//*******************************************************************
// new_pen
static void
emf_new_pen(int pensize,double red, double green, double blue,
				 HPGL_Pt *ppt, HANDLE outDC)
{
	HANDLE pen;
	emf_move_to (ppt, outDC);
	pen=CreatePen( (pensize)?PS_SOLID:PS_NULL,pensize*5,RGB(red,green,blue));
	DeleteObject(SelectObject(outDC,pen));
}

//*******************************************************************
// Possibly draw dependent on mode
static void
emf_line_to (HPGL_Pt *ppt, char mode, HANDLE outDC)
{
	if(mode=='D')
	{
		LineTo(outDC,(int)(ppt->x),(int)(ppt->y) );
		lines++;
	}
	else
	{
		MoveToEx( outDC,(int)(ppt->x) ,(int)(ppt->y) ,NULL);
		moves++;
	}
}

//*******************************************************************
//draw an invisible box around the area so that the metafile gets the size
static void
emf_init(const OUT_PAR *po, HANDLE outDC)
{
	long left, right, low, high;

	low=(int)(po->ymin-MARGIN);
	high=(int)(po->ymax+MARGIN);
	left=(int)po->xmin-MARGIN;
	right=(int)po->xmax+MARGIN;
	MoveToEx(outDC,left,low,NULL);
	LineTo(outDC,right,low);
	LineTo(outDC,right,high);
	LineTo(outDC,left,high);
	LineTo(outDC,left,low);
}

//*******************************************************************
// command loop over tmp_file
static int
plotit(HANDLE outDC,const GEN_PAR *pg, const OUT_PAR *po)
{
	PlotCmd	cmd;
	HPGL_Pt	pt1 = {0};
	int	pen_no=0, pensize=0, pencolor=0, err=0;

	pensize = pt.width[DEFAULT_PEN_NO]; /* Default pen	*/
	pencolor = pt.color[DEFAULT_PEN_NO];
	emf_new_pen(0,pt.clut[pencolor][0],  // no draw pen
		pt.clut[pencolor][1],pt.clut[pencolor][2],&pt1, outDC);

	emf_init ( po, outDC);  // invisible boundingbox
	emf_new_pen(pensize, pt.clut[pencolor][0], pt.clut[pencolor][1],
		pt.clut[pencolor][2], &pt1, outDC);

	/**
	** Command loop: While temporary file not empty: process command.
	**/

	while ((cmd = PlotCmd_from_tmpfile()) != CMD_EOF)
	{
		switch (cmd)
		{
		case NOP:
			break;
		case SET_PEN:
			if ((pen_no = fgetc(pg->td)) == EOF)
			{
				PError("Unexpected end of temp. file: ");
				err = ERROR;
				goto emf_exit;
			}
			pensize = pt.width[pen_no];
			pencolor = pt.color[pen_no];
			emf_new_pen(pensize,pt.clut[pencolor][0],
				pt.clut[pencolor][1],
				pt.clut[pencolor][2],
				&pt1, outDC);
			break;
		case DEF_PW: // DEFine penwidth
			if(!load_pen_width_table(pg->td)) {
				PError("Unexpected end of temp. file");
				err = ERROR;
				goto emf_exit;
			}
			break;
		case DEF_PC: //DEFpen color
			err=load_pen_color_table(pg->td);
			if (err==0) {
				PError("Unexpected end of temp. file");
				err = ERROR;
				goto emf_exit;
			}
			if (err==pencolor) pencolor *=-1; /*current pen changed*/
			break;
		case MOVE_TO: // Moveto

			HPGL_Pt_from_tmpfile (&pt1);
			if (pensize != 0)
				emf_move_to (&pt1, outDC);
			break;
		case DRAW_TO: // Draw line
			HPGL_Pt_from_tmpfile (&pt1);
			if (pensize != 0)
				emf_line_to (&pt1, 'D', outDC);
			break;
		case PLOT_AT:
			HPGL_Pt_from_tmpfile (&pt1);
			if (pensize != 0)
			{
				emf_line_to (&pt1, 'M', outDC);
				emf_line_to (&pt1, 'D', outDC);
			}
			break;
		default:
			Eprintf ("Illegal cmd in temp. file!");
			err = ERROR;
			goto emf_exit;
		}
	}
	/* Finish up */
emf_exit:
	{
		HANDLE old=SelectObject(outDC,GetStockObject(BLACK_PEN));
		DeleteObject(old);
	}
	return err;
}

//*******************************************************************
// helper to set scaling for print/preview
static void
SetScale(HDC dc,int uthei,int utwi,const OUT_PAR *po)
{
	int width,height,mul,div;
	float yfact,xfact;
	long  left, right, low, high;
	low=(int)(po->ymin-MARGIN);
	high=(int)(po->ymax+MARGIN);
	left=(int)po->xmin-MARGIN;
	right=(int)po->xmax+MARGIN;

	width=right-left;
	height=high-low;
	//to this point only used dest coords
	yfact=uthei/(float)height;
	xfact=utwi/(float)width;
	SetMapMode(dc,MM_ANISOTROPIC);
	if(fabs(yfact)<fabs(xfact))
	{
		div=height;
		mul=uthei;
	}
	else
	{
		div=width;
		mul=utwi;
	}

	SetViewportExtEx(dc,mul,-mul,NULL); // size mult
	SetWindowExtEx(dc,div,div,NULL); // size divisor
	SetWindowOrgEx(dc,left,high,NULL); // where was origo
	SetViewportOrgEx(dc,0,0,NULL); // where do i want origo
}

//*******************************************************************
// Mesage handler for Preview Dialog.
static INT_PTR CALLBACK
Preview(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static const GEN_PAR *pg;
	static const OUT_PAR *po;
	static int first=1;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			HMENU meny;
			PGPO *p=(PGPO *)lParam;
			po=p->po;  // save them for print/draw later on
			pg=p->pg;
			SetWindowText(hDlg,"Preview Enter to close");
			meny=GetSystemMenu(hDlg,0);
			// add print item to bottom of system menu
			InsertMenu(meny,(unsigned)-1,MF_BYPOSITION,PRINT,"Print");
		}
		return TRUE;
	case WM_SIZE:
		InvalidateRect(hDlg,0,TRUE); // redraw all
		return TRUE;

	case WM_SYSCOMMAND:
		if(LOWORD(wParam) == PRINT)
		{
			reset_tmpfile(); //rewind so i can reuse the data  (in hpgl.c)
			to_emp (pg,po);
			return TRUE;
		}
		return FALSE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	case WM_PAINT:
		{
			RECT rt;
			PAINTSTRUCT ps;
			HDC dc;
			dc=BeginPaint(hDlg,&ps);
			GetClientRect(hDlg, &rt);
			SetScale(dc,rt.bottom,rt.right,po);

			reset_tmpfile(); //rewind so i can redraw it in hpgl.c
			plotit(dc,pg,po);
			EndPaint(hDlg,&ps);
			if(first){
				Eprintf ("\nWaiting for preview to end\n");
				first=0;
			}
		}
		return TRUE;
	case WM_ERASEBKGND: // fix white background
		{
			RECT rt;
			GetClientRect(hDlg, &rt);
			rt.bottom+=1;
			rt.right+=1;
			FillRect((HDC)wParam,&rt,GetStockObject( WHITE_BRUSH));
		}
		return TRUE;
	}
    return FALSE;
}

//*******************************************************************
// everything above is local
// here starts the part that is visible from outside
//*******************************************************************

/**
** Higher-level interface: Windows  print it  (-m emp)
**/
int
to_emp (const GEN_PAR *pg, const OUT_PAR *po)
{
	DEVMODE *dev;
	PRINTDLG pd;
	DOCINFO di={0};

	int xpix,ypix; //DPI

	int yprinter,xprinter,err;
	// Initialize PRINTDLG
	ZeroMemory(&pd, sizeof(PRINTDLG));
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner   = NULL;
	pd.hDevMode    = NULL;     // Don't forget to free or store hDevMode
	pd.hDevNames   = NULL;     // Don't forget to free or store hDevNames
	pd.Flags       = PD_RETURNDEFAULT; // gives default printer
	pd.nCopies     = 1;
	pd.nFromPage   = 0xFFFF;
	pd.nToPage     = 0xFFFF;
	pd.nMinPage    = 1;
	pd.nMaxPage    = 0xFFFF;

	PrintDlg(&pd);  // first call to fill devmode struct from default printer

	dev=GlobalLock(pd.hDevMode);

	//Auto orient paper
	if(fabs(po->xmax-po->xmin)<fabs(po->ymax-po->ymin))
		dev->dmOrientation=DMORIENT_PORTRAIT;
	else
		dev->dmOrientation=DMORIENT_LANDSCAPE ;
	GlobalUnlock(pd.hDevMode);
	pd.Flags= PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;

	// now check which printer he wants
	if (PrintDlg(&pd)!=TRUE)
		return 0;

	Eprintf ("\n\n- Printing it -\n");
	GlobalFree(pd.hDevMode);
	GlobalFree(pd.hDevNames);

	xprinter=GetDeviceCaps(pd.hDC,HORZRES); // papper width in pixels
	yprinter=GetDeviceCaps(pd.hDC,VERTRES); // height in pixels
	xpix=GetDeviceCaps(pd.hDC,LOGPIXELSX);  // DPI x to be checked
	ypix=GetDeviceCaps(pd.hDC,LOGPIXELSY);  // DPI y
	// the following code is an attempt to compensate for printers with different
	// X and y resolution ( Not tested !!!)
	if (xpix<ypix)
	{
		xprinter=(int)(xprinter*((float)xpix)/ypix);
	}else if (xpix>ypix)
	{
		yprinter=(int)(yprinter*((float)ypix)/xpix);
	}
	SetScale(pd.hDC,yprinter,xprinter,po);

    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = "HPGL File";

    // Begin a print job by calling the StartDoc function.
	StartDoc(pd.hDC, &di);

    // Inform the driver that the application is about to begin
    // sending data.
	StartPage(pd.hDC);

    err=plotit(pd.hDC,pg,po);
	EndPage(pd.hDC);
	EndDoc(pd.hDC);

    DeleteDC(pd.hDC);
	if (!pg->quiet)
	{
		Eprintf ("\nlines=%d moves=%d",lines,moves);
		Eprintf ("\n");
	}
	return err;
}

/**
** Higher-level interface: Windows Preview (-m pre or nothing)
**/
int
to_emw (const GEN_PAR *pg, const OUT_PAR *po)
{
	PGPO par;
	PWORD p;
	LPDLGTEMPLATE templ;
	p= (PWORD) GlobalAlloc (GPTR,sizeof(DLGTEMPLATE)+8);
	templ=(LPDLGTEMPLATE)p;
	// Memory is already zeroed so i dont set zeroes
	// Start to fill in the dlgtemplate information, addressing by WORDs.
	templ->style=WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU |
		WS_THICKFRAME;
	templ->cx=480;	// size in "dialog units"
	templ->cy=320;
	// since i allocated 8 extra bytes i have menu=0 class=0 title=0
	// which means no menu standard dialog and no title
	// i fix title in WM_INIT and background by WM_ERASEBKGND
	par.pg=pg;
	par.po=po;
	DialogBoxIndirectParam(0,templ,0,Preview,(LPARAM)&par);
	GlobalFree (GlobalHandle (p));

	return 0;
}

/**
** Higher-level interface: Output Enhanced META File format (-m emf)
**/
int
to_emf (const GEN_PAR *pg, const OUT_PAR *po)
{
	HANDLE outDC;
	int err=0;

	if (!pg->quiet)
		Eprintf ("\n\n- Writing emf code to \"%s\"\n",
		*po->outfile == '-' ? "stdout" : po->outfile);

	/* Init.  */

	if (*po->outfile != '-')
	{
		if ((outDC=CreateEnhMetaFile(NULL,po->outfile,NULL,"hej\0hopp\0"))==0)
		{
			PError("hp2xx (emf)");
			return ERROR;
		}
	}
	else
	{
		PError("hp2xx (Cant send metafile to stdout emf)");
		return ERROR;
	}
	SetMapMode(outDC,MM_ANISOTROPIC);
	SetViewportExtEx(outDC,10,-10,NULL); // size mult
	err=plotit(outDC,pg,po);
	CloseEnhMetaFile(outDC);

	if (!pg->quiet)
	{
		Eprintf ("\nlines=%d moves=%d",lines,moves);
		Eprintf ("\n");
	}
	return err;
}

