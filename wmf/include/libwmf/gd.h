/* libwmf (<libwmf/gd.h>): library for wmf conversion
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


#ifndef LIBWMF_GD_H
#define LIBWMF_GD_H

#include <libwmf/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _wmf_gd_subtype
{	wmf_gd_png = 0,
	wmf_gd_jpeg,
	wmf_gd_image
} wmf_gd_subtype;

typedef struct _wmf_gd_t wmf_gd_t;

struct _wmf_gd_t
{	wmf_gd_subtype type;

	void* gd_data;

	FILE* file;

	char* memory;

	struct _wmf_gd_sink
	{	void* context;
		int (*function) (void* context,char* buffer,int length);
	} sink;

	void* gd_image; /* pointer to gdImage; null prior to wmf_play () */

	unsigned int width;
	unsigned int height;

	wmfD_Rect bbox;

	unsigned long flags;
};

#define WMF_GD_SUPPORTS_PNG  (1 <<  0) /* Set by wmf_api_create () if PNG  supported */
#define WMF_GD_SUPPORTS_JPEG (1 <<  1) /* Set by wmf_api_create () if JPEG supported */

#define WMF_GD_OUTPUT_FILE   (1 << 16)
#define WMF_GD_OUTPUT_MEMORY (1 << 17)
#define WMF_GD_OWN_BUFFER    (1 << 18) /* To be used in conjuction with WMF_GD_OUTPUT_MEMORY */

#define WMF_GD_GetData(Z) ((wmf_gd_t*)((Z)->device_data))

extern void wmf_gd_function (wmfAPI*);

/* The following are necessary only for direct interaction with the
 * device layer. If desired, <libwmf/ipa.h> needs to be pre-included.
 */
#ifdef LIBWMF_IPA_H

extern void wmf_gd_device_open (wmfAPI*);
extern void wmf_gd_device_close (wmfAPI*);
extern void wmf_gd_device_begin (wmfAPI*);
extern void wmf_gd_device_end (wmfAPI*);
extern void wmf_gd_flood_interior (wmfAPI*,wmfFlood_t*);
extern void wmf_gd_flood_exterior (wmfAPI*,wmfFlood_t*);
extern void wmf_gd_draw_pixel (wmfAPI*,wmfDrawPixel_t*);
extern void wmf_gd_draw_pie (wmfAPI*,wmfDrawArc_t*);
extern void wmf_gd_draw_chord (wmfAPI*,wmfDrawArc_t*);
extern void wmf_gd_draw_arc (wmfAPI*,wmfDrawArc_t*);
extern void wmf_gd_draw_ellipse (wmfAPI*,wmfDrawArc_t*);
extern void wmf_gd_draw_line (wmfAPI*,wmfDrawLine_t*);
extern void wmf_gd_poly_line (wmfAPI*,wmfPolyLine_t*);
extern void wmf_gd_draw_polygon (wmfAPI*,wmfPolyLine_t*);
extern void wmf_gd_draw_rectangle (wmfAPI*,wmfDrawRectangle_t*);
extern void wmf_gd_rop_draw (wmfAPI*,wmfROP_Draw_t*);
extern void wmf_gd_bmp_draw (wmfAPI*,wmfBMP_Draw_t*);
extern void wmf_gd_bmp_read (wmfAPI*,wmfBMP_Read_t*);
extern void wmf_gd_bmp_free (wmfAPI*,wmfBMP*);
extern void wmf_gd_draw_text (wmfAPI*,wmfDrawText_t*);
extern void wmf_gd_udata_init (wmfAPI*,wmfUserData_t*);
extern void wmf_gd_udata_copy (wmfAPI*,wmfUserData_t*);
extern void wmf_gd_udata_set (wmfAPI*,wmfUserData_t*);
extern void wmf_gd_udata_free (wmfAPI*,wmfUserData_t*);
extern void wmf_gd_region_frame (wmfAPI*,wmfPolyRectangle_t*);
extern void wmf_gd_region_paint (wmfAPI*,wmfPolyRectangle_t*);
extern void wmf_gd_region_clip (wmfAPI*,wmfPolyRectangle_t*);

#endif /* LIBWMF_IPA_H */

#ifdef __cplusplus
}
#endif

#endif /* ! LIBWMF_GD_H */
