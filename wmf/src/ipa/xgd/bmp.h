/* libwmf ("ipa/xgd/bmp.h"): library for wmf conversion
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


void wmf_gd_rop_draw (wmfAPI* API,wmfROP_Draw_t* rop_draw)
{	wmf_gd_t* ddata = WMF_GD_GetData (API);

	gd_t* gd = (gd_t*) ddata->gd_data;

	gdPoint TL;
	gdPoint BR;

	int brushstyle;

	WMF_DEBUG (API,"wmf_[gd_]rop_draw");

	if (!TO_FILL (rop_draw)) return;

	brushstyle = setbrushstyle (API,rop_draw->dc);

	switch (rop_draw->ROP) /* Ternary raster operations */
	{
	case SRCCOPY: /* dest = source */
	break;
	case SRCPAINT: /* dest = source OR dest */
	break;
	case SRCAND: /* dest = source AND dest */
	break;
	case SRCINVERT: /* dest = source XOR dest */
	break;
	case SRCERASE: /* dest = source AND (NOT dest) */
	break;
	case NOTSRCCOPY: /* dest = (NOT source) */
	break;
	case NOTSRCERASE: /* dest = (NOT src) AND (NOT dest) */
	break;
	case MERGECOPY: /* dest = (source AND pattern) */
	break;
	case MERGEPAINT: /* dest = (NOT source) OR dest */
	break;
	case PATCOPY: /* dest = pattern */
	break;
	case PATPAINT: /* dest = DPSnoo */
	break;
	case PATINVERT: /* dest = pattern XOR dest */
	break;
	case DSTINVERT: /* dest = (NOT dest) */
	break;
	case BLACKNESS: /* dest = BLACK */
		brushstyle = gdImageColorResolve (gd->image,0,0,0);
	break;
	case WHITENESS: /* dest = WHITE */
		brushstyle = gdImageColorResolve (gd->image,255,255,255);
	break;
	default:
	break;
	}

	TL = gd_translate (API,rop_draw->TL);
	BR = gd_translate (API,rop_draw->BR);

	gdImageFilledRectangle (gd->image,TL.x,TL.y,BR.x,BR.y,brushstyle);
}

void wmf_gd_bmp_draw (wmfAPI* API,wmfBMP_Draw_t* bmp_draw)
{	wmf_gd_t* ddata = WMF_GD_GetData (API);

	gd_t* gd = (gd_t*) ddata->gd_data;

	wmfRGB rgb;

	int opacity;

	float bmp_width;
	float bmp_height;

	float x;
	float y;

	unsigned int i;
	unsigned int j;

	unsigned int width;
	unsigned int height;

	int color;

	gdPoint pt;

	WMF_DEBUG (API,"wmf_[gd_]bmp_draw");

	pt = gd_translate (API,bmp_draw->pt);

	bmp_width  = gd_width  (API,(float) ((double) bmp_draw->crop.w * bmp_draw->pixel_width ));
	bmp_height = gd_height (API,(float) ((double) bmp_draw->crop.h * bmp_draw->pixel_height));

	width  = (unsigned int) ceil (1.0 + bmp_width ); /* The 1.0 is a bit of a fudge factor... */
	height = (unsigned int) ceil (1.0 + bmp_height); /* Works with test11.wmf, anyway.   [??] */

	for (j = 0; j < height; j++)
	{	y = (float) ((double) j * (double) (bmp_draw->crop.h - 1) / (double) (height - 1));
		y += (float) bmp_draw->crop.y;

		for (i = 0; i < width; i++)
		{	x = (float) ((double) i * (double) (bmp_draw->crop.w - 1) / (double) (width - 1));
			x += (float) bmp_draw->crop.x;

			opacity = wmf_ipa_bmp_interpolate (API,&(bmp_draw->bmp),&rgb,x,y);

			if (opacity < 0) break;

			color = gdImageColorResolve (gd->image,rgb.r,rgb.g,rgb.b);

			gdImageSetPixel (gd->image,i+pt.x,(height-1-j)+pt.y,color);
		}
	}
}

void wmf_gd_bmp_read (wmfAPI* API,wmfBMP_Read_t* bmp_read)
{	WMF_DEBUG (API,"wmf_[gd_]bmp_read");

	wmf_ipa_bmp_read (API,bmp_read);
}

void wmf_gd_bmp_free (wmfAPI* API,wmfBMP* bmp)
{	WMF_DEBUG (API,"wmf_[gd_]bmp_free");

	wmf_ipa_bmp_free (API,bmp);
}
