/* libwmf (ipa/gd.c): library for wmf conversion
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
#define wmf_gd_device_open    wmf_device_open
#define wmf_gd_device_close   wmf_device_close
#define wmf_gd_device_begin   wmf_device_begin
#define wmf_gd_device_end     wmf_device_end
#define wmf_gd_flood_interior wmf_flood_interior
#define wmf_gd_flood_exterior wmf_flood_exterior
#define wmf_gd_draw_pixel     wmf_draw_pixel
#define wmf_gd_draw_pie       wmf_draw_pie
#define wmf_gd_draw_chord     wmf_draw_chord
#define wmf_gd_draw_arc       wmf_draw_arc
#define wmf_gd_draw_ellipse   wmf_draw_ellipse
#define wmf_gd_draw_line      wmf_draw_line
#define wmf_gd_poly_line      wmf_poly_line
#define wmf_gd_draw_polygon   wmf_draw_polygon
#define wmf_gd_draw_rectangle wmf_draw_rectangle
#define wmf_gd_rop_draw       wmf_rop_draw
#define wmf_gd_bmp_draw       wmf_bmp_draw
#define wmf_gd_bmp_read       wmf_bmp_read
#define wmf_gd_bmp_free       wmf_bmp_free
#define wmf_gd_draw_text      wmf_draw_text
#define wmf_gd_udata_init     wmf_udata_init
#define wmf_gd_udata_copy     wmf_udata_copy
#define wmf_gd_udata_set      wmf_udata_set
#define wmf_gd_udata_free     wmf_udata_free
#define wmf_gd_region_frame   wmf_region_frame
#define wmf_gd_region_paint   wmf_region_paint
#define wmf_gd_region_clip    wmf_region_clip
#endif /* WMF_IPA_MODULE */

#include <math.h>

#include "wmfdefs.h"

#include "libwmf/gd.h"

#include "ipa/gd.h"
#include "ipa/gd/bmp.h"
#include "ipa/gd/device.h"
#include "ipa/gd/draw.h"
#include "ipa/gd/region.h"

void wmf_gd_function (wmfAPI* API)
{	wmf_gd_t* ddata = 0;

	wmfFunctionReference* FR = (wmfFunctionReference*) API->function_reference;

	gd_t* gd = 0;

/* IPA function reference links
 */
	FR->device_open    = wmf_gd_device_open;
	FR->device_close   = wmf_gd_device_close;
	FR->device_begin   = wmf_gd_device_begin;
	FR->device_end     = wmf_gd_device_end;
	FR->flood_interior = wmf_gd_flood_interior;
	FR->flood_exterior = wmf_gd_flood_exterior;
	FR->draw_pixel     = wmf_gd_draw_pixel;
	FR->draw_pie       = wmf_gd_draw_pie;
	FR->draw_chord     = wmf_gd_draw_chord;
	FR->draw_arc       = wmf_gd_draw_arc;
	FR->draw_ellipse   = wmf_gd_draw_ellipse;
	FR->draw_line      = wmf_gd_draw_line;
	FR->poly_line      = wmf_gd_poly_line;
	FR->draw_polygon   = wmf_gd_draw_polygon;
	FR->draw_rectangle = wmf_gd_draw_rectangle;
	FR->rop_draw       = wmf_gd_rop_draw;
	FR->bmp_draw       = wmf_gd_bmp_draw;
	FR->bmp_read       = wmf_gd_bmp_read;
	FR->bmp_free       = wmf_gd_bmp_free;
	FR->draw_text      = wmf_gd_draw_text;
	FR->udata_init     = wmf_gd_udata_init;
	FR->udata_copy     = wmf_gd_udata_copy;
	FR->udata_set      = wmf_gd_udata_set;
	FR->udata_free     = wmf_gd_udata_free;
	FR->region_frame   = wmf_gd_region_frame;
	FR->region_paint   = wmf_gd_region_paint;
	FR->region_clip    = wmf_gd_region_clip;

/* Allocate device data structure
 */
	ddata = (wmf_gd_t*) wmf_malloc (API,sizeof (wmf_gd_t));

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return;
	}

	API->device_data = (void*) ddata;

/* Device data defaults
 */
	ddata->type = wmf_gd_png;

	ddata->gd_data = (void*) wmf_malloc (API,sizeof (gd_t));

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return;
	}

	gd = (gd_t*) ddata->gd_data;

	gd->image = 0;

	ddata->file   = 0;
	ddata->memory = 0;

	ddata->sink.context  = 0;
	ddata->sink.function = 0;

	ddata->gd_image = 0;

	ddata->width  = 0;
	ddata->height = 0;

	ddata->bbox.TL.x = 0;
	ddata->bbox.TL.y = 0;
	ddata->bbox.BR.x = 0;
	ddata->bbox.BR.y = 0;

	ddata->flags = 0;
#ifdef HAVE_LIBPNG
	ddata->flags |= WMF_GD_SUPPORTS_PNG;
#endif
#ifdef HAVE_LIBJPEG
	ddata->flags |= WMF_GD_SUPPORTS_JPEG;
#endif
}

void wmf_gd_draw_text (wmfAPI* API,wmfDrawText_t* draw_text)
{	wmf_gd_t* ddata = WMF_GD_GetData (API);

	gd_t* gd = (gd_t*) ddata->gd_data;

	gdPoint pt;
	gdPoint pts[4];

	wmfFont* font = 0;

	wmfRGB* rgb = 0;

	int color;

	float font_height;
	float font_ratio;

	double theta;
	double phi;

	FT_Face face;

	FT_F26Dot6 width;
	FT_F26Dot6 height;

	FT_Matrix matrix;

	FT_Vector pen;

	WMF_DEBUG (API,"wmf_[gd_]draw_text");

	if ((WMF_DC_OPAQUE (draw_text->dc)) && (draw_text->bbox.BR.x > draw_text->bbox.TL.x))
	{	rgb = WMF_DC_BACKGROUND (draw_text->dc);

		color = gdImageColorResolve (gd->image,rgb->r,rgb->g,rgb->b);

		pts[0] = gd_translate (API,draw_text->bbox.TL);
		pts[1] = gd_translate (API,draw_text->bbox.TR);
		pts[2] = gd_translate (API,draw_text->bbox.BR);
		pts[3] = gd_translate (API,draw_text->bbox.BL);

		gdImageFilledPolygon (gd->image,pts,4,color);
	}

	if (strlen (draw_text->str) > 1)
	{	wmf_ipa_draw_text (API,draw_text,wmf_gd_draw_text);
		return;
	}

	font = WMF_DC_FONT (draw_text->dc);

	face = font->ft_face;

	font_height = gd_height (API,(float)  draw_text->font_height);
	font_ratio  = gd_width  (API,(float) (draw_text->font_height * draw_text->font_ratio));
	font_ratio /= font_height;

	width  = (FT_F26Dot6) (64 * font_height * font_ratio);
	height = (FT_F26Dot6) (64 * font_height);

	FT_Set_Char_Size (face,width,height,0,0);

	theta = - WMF_TEXT_ANGLE (font);

	phi = atan2 (sin (theta),font_ratio * cos (theta));

	pen.x = 0;
	pen.y = 0;

	matrix.xx = (FT_Fixed) (  cos (phi) * 0x10000L);
	matrix.xy = (FT_Fixed) (  sin (phi) * 0x10000L);
	matrix.yx = (FT_Fixed) (- sin (phi) * 0x10000L);
	matrix.yy = (FT_Fixed) (  cos (phi) * 0x10000L);

	FT_Set_Transform (face,&matrix,&pen);

	FT_Load_Char (face,draw_text->str[0],FT_LOAD_RENDER);

	rgb = WMF_DC_TEXTCOLOR (draw_text->dc);
	color = gdImageColorResolve (gd->image,rgb->r,rgb->g,rgb->b);

	pt = gd_translate (API,draw_text->pt);

	pt.x += face->glyph->bitmap_left;
	pt.y -= face->glyph->bitmap_top;

	/* Sneaky use of normally-static gd function...
	 */

	gdft_draw_bitmap (gd->image,color,face->glyph->bitmap,pt.x,pt.y);
}

void wmf_gd_udata_init (wmfAPI* API,wmfUserData_t* userdata)
{	/* wmf_gd_t* ddata = WMF_GD_GetData (API); */

	/* gd_t* gd = (gd_t*) ddata->gd_data; */

	WMF_DEBUG (API,"wmf_[gd_]udata_init");

}

void wmf_gd_udata_copy (wmfAPI* API,wmfUserData_t* userdata)
{	/* wmf_gd_t* ddata = WMF_GD_GetData (API); */

	/* gd_t* gd = (gd_t*) ddata->gd_data; */

	WMF_DEBUG (API,"wmf_[gd_]udata_copy");

}

void wmf_gd_udata_set (wmfAPI* API,wmfUserData_t* userdata)
{	/* wmf_gd_t* ddata = WMF_GD_GetData (API); */

	/* gd_t* gd = (gd_t*) ddata->gd_data; */

	WMF_DEBUG (API,"wmf_[gd_]udata_set");

}

void wmf_gd_udata_free (wmfAPI* API,wmfUserData_t* userdata)
{	/* wmf_gd_t* ddata = WMF_GD_GetData (API); */

	/* gd_t* gd = (gd_t*) ddata->gd_data; */

	WMF_DEBUG (API,"wmf_[gd_]udata_free");

}

static int setbrushstyle (wmfAPI* API,wmfDC* dc)
{	wmf_gd_t* ddata = WMF_GD_GetData (API);

	gd_t* gd = (gd_t*) ddata->gd_data;

	wmfBrush* brush = 0;

	wmfBMP* bmp = 0;

	wmfRGB* rgb = 0;

	wmfRGB pixel;

	int i;
	int j;

	int fg;
	int bg;

	int brushstyle;

	int opacity;

	unsigned int x;
	unsigned int y;

	unsigned char* bits = 0;

	brush = WMF_DC_BRUSH (dc);

	rgb = WMF_BRUSH_COLOR (brush);
	brushstyle = gdImageColorResolve (gd->image,rgb->r,rgb->g,rgb->b);

	switch (WMF_BRUSH_STYLE (brush))
	{
	case BS_NULL:
		WMF_ERROR (API,"Attempt to set null fill-style!");
		API->err = wmf_E_Glitch;
	break;

	case BS_HATCHED:
		switch (WMF_BRUSH_HATCH (brush))
		{
		case HS_HORIZONTAL:
			bits = HS_HORIZONTAL_bits;
		break;

		case HS_VERTICAL:
			bits = HS_VERTICAL_bits;
		break;

		case HS_FDIAGONAL:
			bits = HS_FDIAGONAL_bits;
		break;

		case HS_BDIAGONAL:
			bits = HS_BDIAGONAL_bits;
		break;

		case HS_CROSS:
			bits = HS_CROSS_bits;
		break;

		case HS_DIAGCROSS:
			bits = HS_DIAGCROSS_bits;
		break;

		default:
			if (API->flags & WMF_OPT_IGNORE_NONFATAL)
			{	WMF_DEBUG (API,"Unsupported brush/hatch style!");
				bits = HS_DIAGCROSS_bits;
			}
			else
			{	WMF_ERROR (API,"Unsupported brush/hatch style!");
				API->err = wmf_E_Glitch;
				bits = 0;
			}
		break;
		}
		if (bits == 0) break;

		if (gd->brush.hatch == 0) gd->brush.hatch = gdImageCreateTrueColor (8,8);

		if (gd->brush.hatch)
		{	rgb = WMF_DC_BACKGROUND (dc);
			bg = gdImageColorResolve (gd->brush.hatch,rgb->r,rgb->g,rgb->b);

			if (!WMF_DC_OPAQUE (dc))
			{	gdImageColorTransparent (gd->brush.hatch,bg);
			}

			rgb = WMF_BRUSH_COLOR (brush);
			fg = gdImageColorResolve (gd->brush.hatch,rgb->r,rgb->g,rgb->b);

			for (j = 0; j < 8; j++)
			{	for (i = 0; i < 8; i++)
				{	if (bits[j] & (1<<(7-i)))
					{	gdImageSetPixel (gd->brush.hatch,i,j,fg);
					}
					else
					{	gdImageSetPixel (gd->brush.hatch,i,j,bg);
					}
				}
			}

			gdImageSetTile (gd->image,gd->brush.hatch);

			brushstyle = gdTiled;
		}
	break;

	case BS_DIBPATTERN:
		bmp = WMF_BRUSH_BITMAP (brush);

		if (bmp->data == 0)
		{	if (API->flags & WMF_OPT_IGNORE_NONFATAL)
			{	WMF_DEBUG (API,"Attempt to fill with non-existent pattern!");
				break;
			}
			else
			{	WMF_ERROR (API,"Attempt to fill with non-existent pattern!");
				API->err = wmf_E_Glitch;
				break;
			}
		}
		if (gd->brush.image)
		{	if ((gd->brush.width  != (int) bmp->width )
			 || (gd->brush.height != (int) bmp->height))
			{	gdImageDestroy (gd->brush.image);
				gd->brush.image = 0;
			}
		}
		if (gd->brush.image == 0)
		{	gd->brush.width  = (int) bmp->width;
			gd->brush.height = (int) bmp->height;

			gd->brush.image = gdImageCreateTrueColor (gd->brush.width,gd->brush.height);
		}
		if (gd->brush.image)
		{	for (y = 0; y < (unsigned int) gd->brush.height; y++)
			{	for (x = 0; x < (unsigned int) gd->brush.width; x++)
				{	opacity = wmf_ipa_bmp_color (API,bmp,&pixel,x,y);

					fg = gdImageColorResolve (gd->brush.image,pixel.r,pixel.g,pixel.b);

					gdImageSetPixel (gd->brush.image,(int) x,(int) y,fg);
				}
			}

			gdImageSetTile (gd->image,gd->brush.image);

			brushstyle = gdTiled;
		}
	break;

	default:
		if (API->flags & WMF_OPT_IGNORE_NONFATAL)
		{	WMF_DEBUG (API,"Unsupported brush style!");
			/* no break here */
		}
		else
		{	WMF_ERROR (API,"Unsupported brush style!");
			API->err = wmf_E_Glitch;
			break;
		}
	case BS_SOLID:
	break;
	}

	return (brushstyle);
}

static int setlinestyle (wmfAPI* API,wmfDC* dc)
{	wmf_gd_t* ddata = WMF_GD_GetData (API);

	gd_t* gd = (gd_t*) ddata->gd_data;

	wmfPen* pen = 0;

	wmfRGB* rgb = 0;

	int pen_width;
	int pen_height;

	int fg;
	int bg;

	int linestyle;

	int i;

	int style[14];

	pen = WMF_DC_PEN (dc);

	/* Color
	 */
	rgb = WMF_PEN_COLOR (pen);
	fg = gdImageColorResolve (gd->image,rgb->r,rgb->g,rgb->b);

	rgb = WMF_DC_BACKGROUND (dc);
	bg = gdImageColorResolve (gd->image,rgb->r,rgb->g,rgb->b);

	linestyle = fg;

	/* Size
	 */
	pen_width  = (int) ceil ((double) gd_width  (API,WMF_PEN_WIDTH  (pen)));
	pen_height = (int) ceil ((double) gd_height (API,WMF_PEN_HEIGHT (pen)));

	if (pen_width  < 1) pen_width  = 1;
	if (pen_height < 1) pen_height = 1;

	if ((pen_width == 1) && (pen_height == 1))
	{	/* Ordinary line */
	}
	else
	{	if (gd->pen.image == 0)
		{	/* Need to create a brush */
			gd->pen.image = gdImageCreateTrueColor (pen_width,pen_height);
		}
		else if ((pen_width == gd->pen.width) && (pen_height == gd->pen.height))
		{	/* Already have brush of correct size */
		}
		else
		{	/* Need to free current brush & create new one */
			gdImageDestroy (gd->pen.image);

			gd->pen.image = gdImageCreateTrueColor (pen_width,pen_height);
		}
		if (gd->pen.image)
		{	gd->pen.width  = pen_width;
			gd->pen.height = pen_height;

			rgb = WMF_PEN_COLOR (pen);
			fg = gdImageColorResolve (gd->pen.image,rgb->r,rgb->g,rgb->b);

			rgb = WMF_DC_BACKGROUND (dc);
			bg = gdImageColorResolve (gd->image,rgb->r,rgb->g,rgb->b);

			if (bg != fg) gdImageColorTransparent (gd->pen.image,bg);

			gdImageFilledRectangle (gd->pen.image,0,0,gd->pen.width,gd->pen.height,bg);

			/* `Square' nib
			 */
			gdImageFilledRectangle (gd->pen.image,0,0,gd->pen.width,gd->pen.height,fg);

			gdImageSetBrush (gd->image,gd->pen.image);

			linestyle = gdBrushed;
		}
	}

	switch (WMF_PEN_STYLE (pen))
	{
	case PS_DASH:
		for (i =  0; i <  5; i++) style[i] = fg;
		for (i =  5; i <  8; i++) style[i] = bg;

		gdImageSetStyle (gd->image,style, 8);

		if (linestyle == gdBrushed)
		{	linestyle = gdStyledBrushed;
		}
		else
		{	linestyle = gdStyled;
		}
	break;

	case PS_DOT:
		for (i =  0; i <  2; i++) style[i] = fg;
		for (i =  2; i <  4; i++) style[i] = bg;

		gdImageSetStyle (gd->image,style, 4);

		if (linestyle == gdBrushed)
		{	linestyle = gdStyledBrushed;
		}
		else
		{	linestyle = gdStyled;
		}
	break;

	case PS_DASHDOT:
		for (i =  0; i <  4; i++) style[i] = fg;
		for (i =  4; i <  7; i++) style[i] = bg;
		for (i =  7; i <  9; i++) style[i] = fg;
		for (i =  9; i < 12; i++) style[i] = bg;

		gdImageSetStyle (gd->image,style,12);

		if (linestyle == gdBrushed)
		{	linestyle = gdStyledBrushed;
		}
		else
		{	linestyle = gdStyled;
		}
	break;

	case PS_DASHDOTDOT:
		for (i =  0; i <  4; i++) style[i] = fg;
		for (i =  4; i <  6; i++) style[i] = bg;
		for (i =  6; i <  8; i++) style[i] = fg;
		for (i =  8; i < 10; i++) style[i] = bg;
		for (i = 10; i < 12; i++) style[i] = fg;
		for (i = 12; i < 14; i++) style[i] = bg;

		gdImageSetStyle (gd->image,style,14);

		if (linestyle == gdBrushed)
		{	linestyle = gdStyledBrushed;
		}
		else
		{	linestyle = gdStyled;
		}
	break;

	case PS_INSIDEFRAME: /* There is nothing to do in this case... */
	case PS_SOLID:
	default:
	break;
	}

	return (linestyle);
}
