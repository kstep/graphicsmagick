/* libwmf (<libwmf/eps.h>): library for wmf conversion
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


#ifndef LIBWMF_EPS_H
#define LIBWMF_EPS_H

#include <stdio.h>

#include <libwmf/types.h>

typedef struct _wmf_eps_t wmf_eps_t;

struct _wmf_eps_t
{	/* other */

	wmfD_Rect bbox;

	wmfStream* out;   /* Output stream */

	char* Title;
	char* Creator;
	char* Date;
	char* For;

	int eps_x;   /* Desired location & size of eps output */
	int eps_y;

	unsigned int eps_width;  /* 0 = use wmf width  */
	unsigned int eps_height; /* 0 = use wmf height */

	unsigned int page_width; /* Page size if (style_eps) */
	unsigned int page_height;

	unsigned long flags;
};

#define WMF_EPS_STYLE_PS   (1 << 0)
#define WMF_EPS_LANDSCAPE  (1 << 1)

#define WMF_EPS_GetData(Z) ((wmf_eps_t*)((Z)->device_data))

#ifdef __cplusplus
extern "C" {
#endif

extern void wmf_eps_function (wmfAPI*);

/* The following are necessary only for direct interaction with the
 * device layer. If desired, <libwmf/ipa.h> needs to be pre-included.
 */
#ifdef LIBWMF_IPA_H

extern void wmf_eps_device_open (wmfAPI*);
extern void wmf_eps_device_close (wmfAPI*);
extern void wmf_eps_device_begin (wmfAPI*);
extern void wmf_eps_device_end (wmfAPI*);
extern void wmf_eps_flood_interior (wmfAPI*,wmfFlood_t*);
extern void wmf_eps_flood_exterior (wmfAPI*,wmfFlood_t*);
extern void wmf_eps_draw_pixel (wmfAPI*,wmfDrawPixel_t*);
extern void wmf_eps_draw_pie (wmfAPI*,wmfDrawArc_t*);
extern void wmf_eps_draw_chord (wmfAPI*,wmfDrawArc_t*);
extern void wmf_eps_draw_arc (wmfAPI*,wmfDrawArc_t*);
extern void wmf_eps_draw_ellipse (wmfAPI*,wmfDrawArc_t*);
extern void wmf_eps_draw_line (wmfAPI*,wmfDrawLine_t*);
extern void wmf_eps_poly_line (wmfAPI*,wmfPolyLine_t*);
extern void wmf_eps_draw_polygon (wmfAPI*,wmfPolyLine_t*);
extern void wmf_eps_draw_rectangle (wmfAPI*,wmfDrawRectangle_t*);
extern void wmf_eps_rop_draw (wmfAPI*,wmfROP_Draw_t*);
extern void wmf_eps_bmp_draw (wmfAPI*,wmfBMP_Draw_t*);
extern void wmf_eps_bmp_read (wmfAPI*,wmfBMP_Read_t*);
extern void wmf_eps_bmp_free (wmfAPI*,wmfBMP*);
extern void wmf_eps_draw_text (wmfAPI*,wmfDrawText_t*);
extern void wmf_eps_udata_init (wmfAPI*,wmfUserData_t*);
extern void wmf_eps_udata_copy (wmfAPI*,wmfUserData_t*);
extern void wmf_eps_udata_set (wmfAPI*,wmfUserData_t*);
extern void wmf_eps_udata_free (wmfAPI*,wmfUserData_t*);
extern void wmf_eps_region_frame (wmfAPI*,wmfPolyRectangle_t*);
extern void wmf_eps_region_paint (wmfAPI*,wmfPolyRectangle_t*);
extern void wmf_eps_region_clip (wmfAPI*,wmfPolyRectangle_t*);

#endif /* LIBWMF_IPA_H */

#ifdef __cplusplus
}
#endif

#endif /* ! LIBWMF_EPS_H */
