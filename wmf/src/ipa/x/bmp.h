/* libwmf ("ipa/x/bmp.h"): library for wmf conversion
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


void wmf_x_rop_draw (wmfAPI* API,wmfROP_Draw_t* ropdraw)
{	/* wmf_x_t* ddata = WMF_X_GetData (API); */

	WMF_DEBUG (API,"wmf_[x_]rop_draw");

}

void wmf_x_bmp_draw (wmfAPI* API,wmfBMP_Draw_t* bmp_draw)
{	wmf_x_t* ddata = WMF_X_GetData (API);

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

	XPoint pt;

	WMF_DEBUG (API,"wmf_[x_]bmp_draw");

	setdefaultstyle (API);

	pt = x_translate (API,bmp_draw->pt);

	bmp_width  = x_width  (API,(float) ((double) bmp_draw->crop.w * bmp_draw->pixel_width ));
	bmp_height = x_height (API,(float) ((double) bmp_draw->crop.h * bmp_draw->pixel_height));

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

			XSetForeground (ddata->display,ddata->gc,get_color (API,&rgb));

			if (ddata->window != None)
			{	XDrawPoint (ddata->display,ddata->window,ddata->gc,i+pt.x,(height-1-j)+pt.y);
			}
			if (ddata->pixmap != None)
			{	XDrawPoint (ddata->display,ddata->pixmap,ddata->gc,i+pt.x,(height-1-j)+pt.y);
			}
		}
	}
}

void wmf_x_bmp_read (wmfAPI* API,wmfBMP_Read_t* bmp_read)
{	WMF_DEBUG (API,"wmf_[x_]bmp_read");

	wmf_ipa_bmp_read (API,bmp_read);
}

void wmf_x_bmp_free (wmfAPI* API,wmfBMP* bmp)
{	WMF_DEBUG (API,"wmf_[x_]bmp_free");

	wmf_ipa_bmp_free (API,bmp);
}
