/* libwmf (<libwmf/plot.h>): library for wmf conversion
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


#ifndef LIBWMF_PLOT_H
#define LIBWMF_PLOT_H

#include <libwmf/types.h>

typedef enum _wmf_plot_subtype
{	wmf_plot_generic = 0,
	wmf_plot_bitmap,
	wmf_plot_meta,
	wmf_plot_tek,
	wmf_plot_regis,
	wmf_plot_hpgl,
	wmf_plot_pcl,
	wmf_plot_fig,
	wmf_plot_cgm,
	wmf_plot_ps,
	wmf_plot_ai,
	wmf_plot_svg,
	wmf_plot_gif,
	wmf_plot_pnm,
	wmf_plot_png,
	wmf_plot_Xdrawable,
	wmf_plot_X
} wmf_plot_subtype;

typedef struct _wmf_plot_t wmf_plot_t;

struct _wmf_plot_t
{	wmf_plot_subtype type;

	void* plot_data;

	FILE* file;

	unsigned int width;
	unsigned int height;

	wmfD_Rect bbox;

	unsigned long flags;
};

#define WMF_PLOT_GetData(Z) ((wmf_plot_t*)((Z)->device_data))

#ifdef __cplusplus
extern "C" {
#endif

extern void wmf_plot_function (wmfAPI*);

/* The following are necessary only for direct interaction with the
 * device layer. If desired, <libwmf/ipa.h> needs to be pre-included.
 */
#ifdef LIBWMF_IPA_H

extern void wmf_plot_device_open (wmfAPI*);
extern void wmf_plot_device_close (wmfAPI*);
extern void wmf_plot_device_begin (wmfAPI*);
extern void wmf_plot_device_end (wmfAPI*);
extern void wmf_plot_flood_interior (wmfAPI*,wmfFlood_t*);
extern void wmf_plot_flood_exterior (wmfAPI*,wmfFlood_t*);
extern void wmf_plot_draw_pixel (wmfAPI*,wmfDrawPixel_t*);
extern void wmf_plot_draw_pie (wmfAPI*,wmfDrawArc_t*);
extern void wmf_plot_draw_chord (wmfAPI*,wmfDrawArc_t*);
extern void wmf_plot_draw_arc (wmfAPI*,wmfDrawArc_t*);
extern void wmf_plot_draw_ellipse (wmfAPI*,wmfDrawArc_t*);
extern void wmf_plot_draw_line (wmfAPI*,wmfDrawLine_t*);
extern void wmf_plot_poly_line (wmfAPI*,wmfPolyLine_t*);
extern void wmf_plot_draw_polygon (wmfAPI*,wmfPolyLine_t*);
extern void wmf_plot_draw_rectangle (wmfAPI*,wmfDrawRectangle_t*);
extern void wmf_plot_rop_draw (wmfAPI*,wmfROP_Draw_t*);
extern void wmf_plot_bmp_draw (wmfAPI*,wmfBMP_Draw_t*);
extern void wmf_plot_bmp_read (wmfAPI*,wmfBMP_Read_t*);
extern void wmf_plot_bmp_free (wmfAPI*,wmfBMP*);
extern void wmf_plot_draw_text (wmfAPI*,wmfDrawText_t*);
extern void wmf_plot_udata_init (wmfAPI*,wmfUserData_t*);
extern void wmf_plot_udata_copy (wmfAPI*,wmfUserData_t*);
extern void wmf_plot_udata_set (wmfAPI*,wmfUserData_t*);
extern void wmf_plot_udata_free (wmfAPI*,wmfUserData_t*);
extern void wmf_plot_region_frame (wmfAPI*,wmfPolyRectangle_t*);
extern void wmf_plot_region_paint (wmfAPI*,wmfPolyRectangle_t*);
extern void wmf_plot_region_clip (wmfAPI*,wmfPolyRectangle_t*);

#endif /* LIBWMF_IPA_H */

#ifdef __cplusplus
}
#endif

#endif /* ! LIBWMF_PLOT_H */
