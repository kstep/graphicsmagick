/* libwmf (ipa/fig.c): library for wmf conversion
   Copyright (C) 2000 - various; see CREDITS, ChangeLog, and sources

   The libwmf Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The libwmf Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the libwmf Library; see the file COPYING.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

/* If compiling as module, rename global functions to a standard
 */
#ifdef WMF_IPA_MODULE
#define wmf_fig_device_open    wmf_device_open
#define wmf_fig_device_close   wmf_device_close
#define wmf_fig_device_begin   wmf_device_begin
#define wmf_fig_device_end     wmf_device_end
#define wmf_fig_flood_interior wmf_flood_interior
#define wmf_fig_flood_exterior wmf_flood_exterior
#define wmf_fig_draw_pixel     wmf_draw_pixel
#define wmf_fig_draw_pie       wmf_draw_pie
#define wmf_fig_draw_chord     wmf_draw_chord
#define wmf_fig_draw_arc       wmf_draw_arc
#define wmf_fig_draw_ellipse   wmf_draw_ellipse
#define wmf_fig_draw_line      wmf_draw_line
#define wmf_fig_poly_line      wmf_poly_line
#define wmf_fig_draw_polygon   wmf_draw_polygon
#define wmf_fig_draw_rectangle wmf_draw_rectangle
#define wmf_fig_rop_draw       wmf_rop_draw
#define wmf_fig_bmp_draw       wmf_bmp_draw
#define wmf_fig_bmp_read       wmf_bmp_read
#define wmf_fig_bmp_free       wmf_bmp_free
#define wmf_fig_draw_text      wmf_draw_text
#define wmf_fig_udata_init     wmf_udata_init
#define wmf_fig_udata_copy     wmf_udata_copy
#define wmf_fig_udata_set      wmf_udata_set
#define wmf_fig_udata_free     wmf_udata_free
#define wmf_fig_region_frame   wmf_region_frame
#define wmf_fig_region_paint   wmf_region_paint
#define wmf_fig_region_clip    wmf_region_clip
#endif /* WMF_IPA_MODULE */

#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */

#include "wmfdefs.h"

#include "libwmf/fig.h"

#include "ipa/fig.h"
#include "ipa/fig/bmp.h"
#include "ipa/fig/color.h"
#include "ipa/fig/device.h"
#include "ipa/fig/draw.h"
#include "ipa/fig/font.h"
#include "ipa/fig/region.h"

void wmf_fig_function (wmfAPI* API)
{	wmf_fig_t* ddata = 0;

	wmfFunctionReference* FR = (wmfFunctionReference*) API->function_reference;

	fig_t* fig = 0;

	int i;

/* IPA function reference links
 */
	FR->device_open    = wmf_fig_device_open;
	FR->device_close   = wmf_fig_device_close;
	FR->device_begin   = wmf_fig_device_begin;
	FR->device_end     = wmf_fig_device_end;
	FR->flood_interior = wmf_fig_flood_interior;
	FR->flood_exterior = wmf_fig_flood_exterior;
	FR->draw_pixel     = wmf_fig_draw_pixel;
	FR->draw_pie       = wmf_fig_draw_pie;
	FR->draw_chord     = wmf_fig_draw_chord;
	FR->draw_arc       = wmf_fig_draw_arc;
	FR->draw_ellipse   = wmf_fig_draw_ellipse;
	FR->draw_line      = wmf_fig_draw_line;
	FR->poly_line      = wmf_fig_poly_line;
	FR->draw_polygon   = wmf_fig_draw_polygon;
	FR->draw_rectangle = wmf_fig_draw_rectangle;
	FR->rop_draw       = wmf_fig_rop_draw;
	FR->bmp_draw       = wmf_fig_bmp_draw;
	FR->bmp_read       = wmf_fig_bmp_read;
	FR->bmp_free       = wmf_fig_bmp_free;
	FR->draw_text      = wmf_fig_draw_text;
	FR->udata_init     = wmf_fig_udata_init;
	FR->udata_copy     = wmf_fig_udata_copy;
	FR->udata_set      = wmf_fig_udata_set;
	FR->udata_free     = wmf_fig_udata_free;
	FR->region_frame   = wmf_fig_region_frame;
	FR->region_paint   = wmf_fig_region_paint;
	FR->region_clip	   = wmf_fig_region_clip;

/* Allocate device data structure
 */
	ddata = (wmf_fig_t*) wmf_malloc (API,sizeof (wmf_fig_t));

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return;
	}

	API->device_data = (void*) ddata;

/* Device data defaults
 */
	ddata->fig_data = (void*) wmf_malloc (API,sizeof (fig_t));

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return;
	}

	fig = (fig_t*) ddata->fig_data;

	for (i = 0; i < NUMPATTERNS; i++)
	{	fig->equiv[i] = 0;
	}

	ddata->bbox.TL.x = 0;
	ddata->bbox.TL.y = 0;
	ddata->bbox.BR.x = 0;
	ddata->bbox.BR.y = 0;

	ddata->out = 0;

	ddata->Title = 0;
	ddata->Creator = 0;
	ddata->Date = 0;
	ddata->For = 0;

	ddata->fig_x = 0;
	ddata->fig_y = 0;
	ddata->fig_width = 0;
	ddata->fig_height = 0;

	ddata->format = wmf_P_A4;

	ddata->dpi = 1200;

	ddata->depth = 999;
	ddata->ddec = 1;

	ddata->image.context = 0;
	ddata->image.name = 0;

	ddata->flags = 0;

#ifdef HAVE_LIBPNG
	ddata->flags |= WMF_FIG_SUPPORTS_PNG;
#endif
#ifdef HAVE_LIBJPEG
	ddata->flags |= WMF_FIG_SUPPORTS_JPEG;
#endif
	/* Add the 32 standard colors at the head of the IPA color table:
	 */
	fig_std_colors (API);
}

void wmf_fig_udata_init (wmfAPI* API,wmfUserData_t* user_data)
{	/* wmf_fig_t* ddata = WMF_FIG_GetData (API); */

	WMF_DEBUG (API,"~~~~~~~~wmf_[fig_]udata_init");

	
}

void wmf_fig_udata_copy (wmfAPI* API,wmfUserData_t* user_data)
{	/* wmf_fig_t* ddata = WMF_FIG_GetData (API); */

	WMF_DEBUG (API,"~~~~~~~~wmf_[fig_]udata_copy");

	
}

void wmf_fig_udata_set (wmfAPI* API,wmfUserData_t* user_data)
{	/* wmf_fig_t* ddata = WMF_FIG_GetData (API); */

	WMF_DEBUG (API,"~~~~~~~~wmf_[fig_]udata_set");

	
}

void wmf_fig_udata_free (wmfAPI* API,wmfUserData_t* user_data)
{	/* wmf_fig_t* ddata = WMF_FIG_GetData (API); */

	WMF_DEBUG (API,"~~~~~~~~wmf_[fig_]udata_free");

	
}

static int fig_brushstyle (wmfAPI* API,wmfDC* dc)
{	wmfBrush* brush = WMF_DC_BRUSH (dc);

	wmfBMP* bmp = 0;

	int fillstyle = -1; /* Not filled */

	if (WMF_BRUSH_STYLE (brush) == BS_NULL)
	{	return (fillstyle);
	}

	switch (WMF_BRUSH_STYLE (brush))
	{
	case BS_NULL:
		fillstyle = -1;
	break;

	case BS_SOLID:
		fillstyle = 20;
	break;

	case BS_HATCHED:
		switch (WMF_BRUSH_HATCH (brush))
		{
		case HS_HORIZONTAL:
			fillstyle = 49;
		break;

		case HS_VERTICAL:
			fillstyle = 50;
		break;

		case HS_FDIAGONAL:
			fillstyle = 44;
		break;

		case HS_BDIAGONAL:
			fillstyle = 45;
		break;

		case HS_CROSS:
			fillstyle = 51;
		break;

		case HS_DIAGCROSS:
			fillstyle = 46;
		break;

		default:
			fillstyle = 20;
			wmf_printf (API,"Unsupported Hatching: %d\n",(int) (brush->lbHatch));
		break;
		}
	break;

	case BS_DIBPATTERN:
		WMF_DEBUG (API,"Attempt to fill with bitmap! Using random pattern instead...");

		bmp = WMF_BRUSH_BITMAP (brush);

		fillstyle = fig_bmp_pattern (API,bmp->data);
	break;

	default:
		wmf_printf (API,"Unsupported Brush Style: %d\n",(int) (brush->lbStyle));
	break;
	}

	return (fillstyle);
}

static int fig_linestyle (wmfAPI* API,wmfDC* dc)
{	wmfPen* pen = WMF_DC_PEN (dc);

	int linestyle = SOLID_LINE;

	switch (WMF_PEN_STYLE (pen))
	{
	case PS_DASH: /* DASH_LINE */
		linestyle = DASH_LINE;
	break;

	case PS_ALTERNATE:
	case PS_DOT: /* DOTTED_LINE */
		linestyle = DOTTED_LINE;
	break;

	case PS_DASHDOT: /* DASH_DOT_LINE */
		linestyle = DASH_DOT_LINE;
	break;
		
	case PS_DASHDOTDOT: /* DASH_2_DOTS_LINE */
		linestyle = DASH_2_DOTS_LINE;
	break;

	case PS_INSIDEFRAME: /* There is nothing to do in this case... */
	case PS_SOLID:
	default:
		linestyle = SOLID_LINE;
	break;
	}

	return (linestyle);
}

static int fig_joinstyle (wmfAPI* API,wmfDC* dc)
{	wmfPen* pen = WMF_DC_PEN (dc);

	int joinstyle = JOIN_ROUND;

	switch (WMF_PEN_JOIN (pen))
	{
	case PS_JOIN_BEVEL:
		joinstyle = JOIN_BEVEL;
	break;

	case PS_JOIN_MITER:
		joinstyle = JOIN_MITER;
	break;

	case PS_JOIN_ROUND:
	default:
		joinstyle = JOIN_ROUND;
	break;
	}

	return (joinstyle);
}

static int fig_capstyle (wmfAPI* API,wmfDC* dc)
{	wmfPen* pen = WMF_DC_PEN (dc);

	int capstyle = CAP_ROUND;

	switch (WMF_PEN_ENDCAP (pen))
	{
	case PS_ENDCAP_SQUARE:
		capstyle = CAP_PROJECT;
	break;

	case PS_ENDCAP_FLAT:
		capstyle = CAP_PROJECT;
	break;

	case PS_ENDCAP_ROUND:
	default:
		capstyle = CAP_ROUND;
	break;
	}

	return (capstyle);
}

static void fig_set_style (wmfAPI* API,wmfDC* dc,figDC* fig)
{	wmf_fig_t* ddata = WMF_FIG_GetData (API);

	wmfBrush* brush = WMF_DC_BRUSH (dc);

	wmfPen* pen = WMF_DC_PEN (dc);

	fig->pen_style = 0;

	fig->thickness = (fig_height (API,WMF_PEN_HEIGHT (pen)) * 72) / ddata->dpi;

	fig->area_fill  = fig_brushstyle (API,dc);

	fig->line_style = fig_linestyle (API,dc);
	fig->join_style = fig_joinstyle (API,dc);
	fig->cap_style  = fig_capstyle  (API,dc);

	fig->radius = 5;

	fig->forward_arrow = 0;
	fig->backward_arrow = 0;

	fig->pen_color  = wmf_ipa_color_index (API,WMF_PEN_COLOR (pen));
	fig->fill_color = wmf_ipa_color_index (API,WMF_BRUSH_COLOR (brush));

	/* If a pattern is specified, the given fill_color should be used to 
	 * render its lines instead of the given pen_color, and the background 
	 * should be white. Without this, Xfig would use fill_color to fill 
	 * the space between pattern lines drawn in pen_color, which is wrong.
	 */
	if (fig->area_fill > 40)
	{	fig->pen_color = fig->fill_color;
		fig->fill_color = 7;
	}
	else
	{	fig->thickness++;
	}

	fig->style_val = 5.0;
}
