/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                             W   W   M   M  FFFFF                            %
%                             W   W   MM MM  F                                %
%                             W W W   M M M  FFF                              %
%                             WW WW   M   M  F                                %
%                             W   W   M   M  F                                %
%                                                                             %
%                                                                             %
%                        Read Windows Metafile Format.                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                     Bob Friesenhahn & Francis J. Franklin                   %
%                            Dec 2000 - Oct 2001                              %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/*
  Should write replacements for:
  WMF_FONT_PSNAME
  wmf_api_create
  wmf_api_destroy
  wmf_ipa_font_lookup
  
*/

/*
 * Include declarations.
 */
#include "magick.h"
#include "define.h"

#if defined(HasWMF)
#if !defined(WIN32)

#ifdef  min
#undef  min
#endif /* min */
#define min(a, b) ((a) < (b) ? (a) : (b))

#ifdef  max
#undef  max
#endif /* max */
#define max(a, b) ((a) < (b) ? (b) : (a))

#ifdef abs
#undef abs
#endif /* abs */
#define abs(x) ((x) >= 0 ? (x) : -(x))

#define ERR(API)  ((API)->err != wmf_E_None)
#define DIAG(API) ((API)->flags & WMF_OPT_DIAGNOSTICS)

/* Unit conversions */
#define TWIPS_PER_INCH        1440
#define CENTIMETERS_PER_INCH  2.54
#define POINTS_PER_CENTIMETER 28.34646
#define CENTIMETERS_PER_POINT 0.03527778
#define POINTS_PER_INCH       72
#define INCHES_PER_POINT      0.01388889

#include "libwmf/fund.h"
#include "libwmf/types.h"
#include "libwmf/api.h"
#include "libwmf/defs.h"
#include "libwmf/ipa.h"
#if HAVE_LIBWMF_FONT_H
# include "libwmf/font.h"
#endif /* HAVE_LIBWMF_FONT_H */
#include "libwmf/color.h"
#include "libwmf/macro.h"

typedef struct _wmf_magick_t wmf_magick_t;

struct _wmf_magick_t
{
  /* Bounding box */
  wmfD_Rect
    bbox;

  double
    bbox_to_pixels_scale_x,
    bbox_to_pixels_scale_y,
    bbox_to_pixels_translate_x,
    bbox_to_pixels_translate_y;

  /* MVG output */
  char
    *mvg;

  long
    mvg_alloc,
    mvg_length;

  /* ImageMagick image */
    Image
      *image;

  /* ImageInfo */
    ImageInfo
      *image_info;

  /* Maximum and current number of temporary files */
  int
    max_temp_file_index,
    cur_temp_file_index;

  /* Temporary image IDs */
  long
    *temp_images;

  /* Pattern ID */
  unsigned long
    pattern_id;

  /* Clip path flag */
  unsigned int
    clipping;
};

#define WMF_MAGICK_GetData(Z) ((wmf_magick_t*)((Z)->device_data))

typedef enum
{
  magick_arc_ellipse = 0,
  magick_arc_open,
  magick_arc_pie,
  magick_arc_chord
}
magick_arc_t;

static int magick_mvg_printf(wmfAPI * API, char *format, ...);
static void magick_brush(wmfAPI * API, wmfDC * dc);
static void magick_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc,
			    magick_arc_t finish);
static void magick_pen(wmfAPI * API, wmfDC * dc);
static void wmf_magick_bmp_draw(wmfAPI * API, wmfBMP_Draw_t * bmp_draw);
static void wmf_magick_bmp_free(wmfAPI * API, wmfBMP * bmp);
static void wmf_magick_bmp_read(wmfAPI * API, wmfBMP_Read_t * bmp_read);
static void wmf_magick_device_begin(wmfAPI * API);
static void wmf_magick_device_close(wmfAPI * API);
static void wmf_magick_device_end(wmfAPI * API);
static void wmf_magick_device_open(wmfAPI * API);
static void wmf_magick_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void wmf_magick_draw_chord(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void wmf_magick_draw_ellipse(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void wmf_magick_draw_line(wmfAPI * API, wmfDrawLine_t * draw_line);
static void wmf_magick_draw_pie(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void wmf_magick_draw_pixel(wmfAPI * API, wmfDrawPixel_t * draw_pixel);
static void wmf_magick_draw_polygon(wmfAPI * API, wmfPolyLine_t * poly_line);
static void wmf_magick_draw_rectangle(wmfAPI * API, wmfDrawRectangle_t * draw_rect);
static void wmf_magick_draw_text(wmfAPI * API, wmfDrawText_t * draw_text);
static void wmf_magick_flood_exterior(wmfAPI * API, wmfFlood_t * flood);
static void wmf_magick_flood_interior(wmfAPI * API, wmfFlood_t * flood);
static void wmf_magick_function(wmfAPI * API);
static void wmf_magick_poly_line(wmfAPI * API, wmfPolyLine_t * poly_line);
static void wmf_magick_region_clip(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void wmf_magick_region_frame(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void wmf_magick_region_paint(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void magick_render_mvg(wmfAPI * API);
static void wmf_magick_rop_draw(wmfAPI * API, wmfROP_Draw_t * rop_draw);
static void wmf_magick_udata_copy(wmfAPI * API, wmfUserData_t * userdata);
static void wmf_magick_udata_free(wmfAPI * API, wmfUserData_t * userdata);
static void wmf_magick_udata_init(wmfAPI * API, wmfUserData_t * userdata);
static void wmf_magick_udata_set(wmfAPI * API, wmfUserData_t * userdata);

static void wmf_magick_rop_draw(wmfAPI * API, wmfROP_Draw_t * rop_draw)
{
  if (!TO_FILL(rop_draw))
    return;

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  /* FIXME: finish implementing (once we know what it is supposed to do!)*/

  magick_brush(API, rop_draw->dc);

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
      break;
    case WHITENESS: /* dest = WHITE */
      break;
    default:
      break;
    }

  magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
                    (double)rop_draw->TL.x, (double)rop_draw->TL.y,
                    (double)rop_draw->BR.x, (double)rop_draw->BR.y);

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_bmp_draw(wmfAPI * API, wmfBMP_Draw_t * bmp_draw)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  double
    height,
    width,
    x,
    y;

  char
    imgspec[30];

  ExceptionInfo
    exception;

  long
    id;

  if (bmp_draw->bmp.data == 0)
    return;

  magick_mvg_printf(API, "# wmf_magick_bmp_draw\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  GetExceptionInfo(&exception);
  id = SetMagickRegistry(ImageRegistryType, bmp_draw->bmp.data,
                         sizeof(Image), &exception);
  (ddata->temp_images)[ddata->cur_temp_file_index] = id;
  ++ddata->cur_temp_file_index;
  if (ddata->cur_temp_file_index == ddata->max_temp_file_index)
  {
    ddata->max_temp_file_index += 2048;
    ReacquireMemory((void **) &ddata->temp_images,
		    ddata->max_temp_file_index * sizeof(long));
  }
  sprintf(imgspec, "mpr:%li", id);

  /* Okay, if we've got this far then "imgspec" is the filename of an png
     (cropped) image */

  width = abs(bmp_draw->pixel_width * (double) bmp_draw->crop.w);
  height = abs(bmp_draw->pixel_height * (double) bmp_draw->crop.h) - 0.0000022;
  x = bmp_draw->pt.x;
  y = bmp_draw->pt.y;

  /*   printf("x=%.10g, y=%.10g, width=%.10g, height=%.10g\n", x,y,width,height); */
  magick_mvg_printf(API, "image Copy %.10g,%.10g %.10g,%.10g '%s'\n",
		    x, y, width, height, imgspec);

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_bmp_read(wmfAPI * API, wmfBMP_Read_t * bmp_read) {
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  Image
    *image;

  bmp_read->bmp.data = (void *) 0;
  image_info = CloneImageInfo((ImageInfo *) 0);
  strcpy(image_info->magick, "DIB");
  GetExceptionInfo(&exception);
#if 0
  printf("offset=%ld, buffer=%lx length=%ld, width=%i, height=%i\n",
	 bmp_read->offset, (unsigned long) bmp_read->buffer, bmp_read->length,
	 bmp_read->width, bmp_read->height);
#endif

  image = BlobToImage(image_info, (const void *) bmp_read->buffer,
		      bmp_read->length, &exception);
  DestroyImageInfo(image_info);
  if (!image)
  {
    char error_message[MaxTextExtent];

    /* Transfer error to image */
    if (ddata->image->exception.severity < CorruptImageWarning)
    {
      ddata->image->exception = exception;
      ddata->image->exception.severity = CorruptImageWarning;
    }
    snprintf(error_message, sizeof(error_message) - 1, "%s (%s)",
	     exception.reason, exception.description);
    WMF_ERROR(API, error_message);
  }
  else
  {
    /* printf("rows=%ld,columns=%ld\n", image->rows, image->columns); */
    bmp_read->bmp.data = image;
    /*       bmp_read->bmp.width  = (U16)image->columns; */
    /*       bmp_read->bmp.height = (U16)image->rows; */
  }

}

static void wmf_magick_bmp_free(wmfAPI * API, wmfBMP * bmp)
{
  /*
   * We don't actually free the image here since we need the image
   * to persist until DrawImage() has been executed.
   * The images are freed by wmf_magick_device_close()
   */

  /* if(bmp->data) */
  /* DestroyImage((Image*)bmp->data); */
  bmp->data = (void *) 0;
  bmp->width = (U16) 0;
  bmp->height = (U16) 0;
}

/*
  This is called by wmf_play() the *first* time the meta file is played
 */
static void wmf_magick_device_open(wmfAPI * API)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  /* Initialize ddata */
  ddata->max_temp_file_index = 2048;
  ddata->cur_temp_file_index = 0;
  ddata->temp_images =
    (long *) AcquireMemory(ddata->max_temp_file_index * sizeof(long));
  ddata->pattern_id = 0;
  ddata->clipping = False;
}

/*
  This is called by wmf_api_destroy()
 */
static void wmf_magick_device_close(wmfAPI * API)
{
  int
    index;

  Image
    *image;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Destroy and de-register images saved in the image registry */
  if (ddata->temp_images != 0)
  {
    RegistryType
      type;

    size_t
      length;

    for (index = 0; index < ddata->cur_temp_file_index; index++)
    {
      image = (Image *) GetMagickRegistry((ddata->temp_images)[index],
					  &type, &length);
      if (type == ImageRegistryType)
	DestroyImage(image);
      DeleteMagickRegistry((ddata->temp_images)[index]);
    }
    LiberateMemory((void **) &ddata->temp_images);
  }
}

/*
  This is called from the beginning of each play for initial page setup
 */
static void wmf_magick_device_begin(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Make SVG output happy */
  magick_mvg_printf(API, "push graphic-context\n");

  magick_mvg_printf(API, "viewbox 0 0 %u %u\n", ddata->image->columns,
		    ddata->image->rows);

  /* Scale width and height to image */
  magick_mvg_printf(API, "scale %.10g,%.10g\n",
		    ddata->bbox_to_pixels_scale_x, ddata->bbox_to_pixels_scale_y);
  /* Translate to TL corner of bounding box */
  magick_mvg_printf(API, "translate %.10g,%.10g\n",
		    ddata->bbox_to_pixels_translate_x,
		    ddata->bbox_to_pixels_translate_y);
  magick_mvg_printf(API, "fill none\n");
  magick_mvg_printf(API, "stroke none\n");
}

/*
  This is called from the end of each play for page termination
 */
static void wmf_magick_device_end(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Reset any existing clip paths by popping context */
  if(ddata->clipping)
    magick_mvg_printf(API, "pop graphic-context\n");
  ddata->clipping = False;

  /* Make SVG output happy */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_flood_interior(wmfAPI * API, wmfFlood_t * flood)
{
  wmfRGB
    *rgb = &(flood->color);

  magick_mvg_printf(API, "# wmf_magick_flood_interior\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  magick_mvg_printf(API, "fill #%02x%02x%02x\n",
		    (int) rgb->r, (int) rgb->g, (int) rgb->b);

  magick_mvg_printf(API, "color %.10g,%.10g filltoborder\n",
		    flood->pt.x, flood->pt.y);

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_flood_exterior(wmfAPI * API, wmfFlood_t * flood)
{
  wmfRGB
    *rgb = &(flood->color);

  magick_mvg_printf(API, "# wmf_magick_flood_exterior\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  magick_mvg_printf(API, "fill #%02x%02x%02x\n",
		    (int) rgb->r, (int) rgb->g, (int) rgb->b);

  if (flood->type == FLOODFILLSURFACE)
    magick_mvg_printf(API, "color %.10g,%.10g floodfill\n", flood->pt.x,
		      flood->pt.y);
  else
    magick_mvg_printf(API, "color %.10g,%.10g filltoborder\n", flood->pt.x,
		      flood->pt.y);

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_draw_pixel(wmfAPI * API, wmfDrawPixel_t * draw_pixel)
{
  wmfRGB
    *rgb = &(draw_pixel->color);

  magick_mvg_printf(API, "# wmf_magick_draw_pixel\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  magick_mvg_printf(API, "stroke none\n");

  magick_mvg_printf(API, "fill-opacity 1\n");

  magick_mvg_printf(API, "fill #%02x%02x%02x\n",
		    (int) rgb->r, (int) rgb->g, (int) rgb->b);

  magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
		    draw_pixel->pt.x, draw_pixel->pt.y,
		    draw_pixel->pt.x + draw_pixel->pixel_width,
		    draw_pixel->pt.y + draw_pixel->pixel_height);

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_draw_pie(wmfAPI * API, wmfDrawArc_t * draw_arc)
{
  magick_draw_arc(API, draw_arc, magick_arc_pie);
}

static void wmf_magick_draw_chord(wmfAPI * API, wmfDrawArc_t * draw_arc)
{
  magick_draw_arc(API, draw_arc, magick_arc_chord);
}

static void wmf_magick_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc)
{
  magick_draw_arc(API, draw_arc, magick_arc_open);
}

static void wmf_magick_draw_ellipse(wmfAPI * API, wmfDrawArc_t * draw_arc)
{
  magick_draw_arc(API, draw_arc, magick_arc_ellipse);
}

static void magick_draw_arc(wmfAPI * API,
			    wmfDrawArc_t * draw_arc, magick_arc_t finish)
{
  wmfD_Coord
    BR,
    O,
    TL,
    centre,
    end,
    start;

  double
    phi_e = 360,
    phi_s = 0;

  double
    Rx,
    Ry;

  magick_mvg_printf(API, "# magick_draw_arc\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  if (TO_FILL(draw_arc) || TO_DRAW(draw_arc))
  {
    centre.x = (draw_arc->TL.x + draw_arc->BR.x) / 2;
    centre.y = (draw_arc->TL.y + draw_arc->BR.y) / 2;

    if (finish != magick_arc_ellipse)
    {
      draw_arc->start.x += centre.x;
      draw_arc->start.y += centre.y;

      draw_arc->end.x += centre.x;
      draw_arc->end.y += centre.y;
    }

    TL = draw_arc->TL;
    BR = draw_arc->BR;

    O = centre;

    if (finish != magick_arc_ellipse)
    {
      start = draw_arc->start;
      end = draw_arc->end;
    }

    Rx = (BR.x - TL.x) / 2;
    Ry = (BR.y - TL.y) / 2;

    if (finish != magick_arc_ellipse)
    {
      start.x -= O.x;
      start.y -= O.y;

      end.x -= O.x;
      end.y -= O.y;

      phi_s = atan2((double) start.y, (double) start.x) * 180 / MagickPI;
      phi_e = atan2((double) end.y, (double) end.x) * 180 / MagickPI;

      if (phi_e <= phi_s)
	phi_e += 360;
    }

    magick_pen(API, draw_arc->dc);
    if (finish == magick_arc_open)
      magick_mvg_printf(API, "fill none\n");
    else
      magick_brush(API, draw_arc->dc);

    if (finish == magick_arc_ellipse)
      magick_mvg_printf(API, "ellipse %.10g,%.10g %.10g,%.10g 0,360\n",
			O.x, O.y, Rx, Ry);
    else if (finish == magick_arc_pie)
      magick_mvg_printf(API, "ellipse %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
			O.x, O.y, Rx, Ry, phi_s, phi_e);
    else if (finish == magick_arc_chord)
    {
      magick_mvg_printf(API, "arc %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
			O.x, O.y, Rx, Ry, phi_s, phi_e);
      magick_mvg_printf(API, "line %.10g,%.10g %.10g,%.10g\n",
			start.x, start.y, end.x, end.y);
    }
    else			/* if (finish == magick_arc_open) */
      magick_mvg_printf(API, "arc %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
			O.x, O.y, Rx, Ry, phi_s, phi_e);
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_draw_line(wmfAPI * API, wmfDrawLine_t * draw_line)
{
  magick_mvg_printf(API, "# wmf_magick_draw_line\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  if (TO_DRAW(draw_line))
  {
    magick_pen(API, draw_line->dc);
    magick_mvg_printf(API, "line %.10g,%.10g %.10g,%.10g\n",
		      draw_line->from.x, draw_line->from.y,
		      draw_line->to.x, draw_line->to.y);
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_poly_line(wmfAPI * API, wmfPolyLine_t * poly_line)
{
  U16
    i;

  magick_mvg_printf(API, "# wmf_magick_poly_line\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  if (poly_line->count <= 1)
    return;

  if (TO_DRAW(poly_line))
  {
    magick_pen(API, poly_line->dc);

    magick_mvg_printf(API, "polyline");

    for (i = 0; i < poly_line->count; i++)
    {
      magick_mvg_printf(API, " %.10g,%.10g", poly_line->pt[i].x, poly_line->pt[i].y);
    }

    magick_mvg_printf(API, "\n");
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_draw_polygon(wmfAPI * API, wmfPolyLine_t * poly_line)
{
  U16
    i;

  magick_mvg_printf(API, "# wmf_magick_draw_polygon\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  if (poly_line->count <= 2)
    return;

  if (TO_FILL(poly_line) || TO_DRAW(poly_line))
  {
    magick_pen(API, poly_line->dc);
    magick_brush(API, poly_line->dc);

    magick_mvg_printf(API, "polygon");

    for (i = 0; i < poly_line->count; i++)
    {
      magick_mvg_printf(API, " %.10g,%.10g", poly_line->pt[i].x, poly_line->pt[i].y);
    }

    magick_mvg_printf(API, "\n");
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_draw_rectangle(wmfAPI * API, wmfDrawRectangle_t * draw_rect)
{
  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  if (TO_FILL(draw_rect) || TO_DRAW(draw_rect))
  {
    magick_pen(API, draw_rect->dc);
    magick_brush(API, draw_rect->dc);

    if ((draw_rect->width > 0) || (draw_rect->height > 0))
      magick_mvg_printf(API, "roundrectangle %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
			draw_rect->TL.x, draw_rect->TL.y,
			draw_rect->BR.x, draw_rect->BR.y,
			draw_rect->width / 2, draw_rect->height / 2);
    else
      magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
			draw_rect->TL.x, draw_rect->TL.y,
			draw_rect->BR.x, draw_rect->BR.y);
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_region_frame(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{
  magick_mvg_printf(API, "# wmf_magick_region_frame\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  /* FIXME: implement */
  printf("wmf_magick_region_frame not implemented\n");

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_region_paint(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{
  unsigned int
    i;

  if (poly_rect->count == 0)
    return;

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  if (TO_FILL (poly_rect))
    {
      magick_mvg_printf(API, "stroke none\n");
      magick_brush(API, poly_rect->dc);

      for (i = 0; i < poly_rect->count; i++)
        {
          magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
                            (double)poly_rect->TL[i].x, (double)poly_rect->TL[i].y,
                            (double)poly_rect->BR[i].x, (double)poly_rect->BR[i].y);
        }
    }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_region_clip(wmfAPI *API, wmfPolyRectangle_t *poly_rect)
{
  unsigned int
    i;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData (API);

  magick_mvg_printf(API, "# wmf_magick_region_clip\n");

  /* Reset any existing clip paths by popping context */
  if(ddata->clipping)
    magick_mvg_printf(API, "pop graphic-context\n");
  ddata->clipping = False;

  if(poly_rect->count > 0)
    {
      /* Push context for new clip paths */
      magick_mvg_printf(API, "push graphic-context\n");

      magick_mvg_printf(API, "push clip-path whatever\n");
      magick_mvg_printf(API, "push graphic-context\n");
      magick_mvg_printf(API, "fill black\n");
      magick_mvg_printf(API, "stroke none\n");
      magick_mvg_printf(API, "stroke-width 1\n");
      for (i = 0; i < poly_rect->count; i++)
        {
          magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
                            (double)poly_rect->TL[i].x, (double)poly_rect->TL[i].y,
                            (double)poly_rect->BR[i].x, (double)poly_rect->BR[i].y);
        }
      magick_mvg_printf(API, "pop graphic-context\n");
      magick_mvg_printf(API, "pop clip-path\n");
      ddata->clipping = True;
    }
}

static void wmf_magick_function(wmfAPI *API)
{
  wmf_magick_t
    *ddata = 0;

  wmfFunctionReference
    *FR = (wmfFunctionReference *) API->function_reference;

  /*
     IPA function reference links
   */
  FR->device_open = wmf_magick_device_open;
  FR->device_close = wmf_magick_device_close;
  FR->device_begin = wmf_magick_device_begin;
  FR->device_end = wmf_magick_device_end;
  FR->flood_interior = wmf_magick_flood_interior;
  FR->flood_exterior = wmf_magick_flood_exterior;
  FR->draw_pixel = wmf_magick_draw_pixel;
  FR->draw_pie = wmf_magick_draw_pie;
  FR->draw_chord = wmf_magick_draw_chord;
  FR->draw_arc = wmf_magick_draw_arc;
  FR->draw_ellipse = wmf_magick_draw_ellipse;
  FR->draw_line = wmf_magick_draw_line;
  FR->poly_line = wmf_magick_poly_line;
  FR->draw_polygon = wmf_magick_draw_polygon;
  FR->draw_rectangle = wmf_magick_draw_rectangle;
  FR->rop_draw = wmf_magick_rop_draw;
  FR->bmp_draw = wmf_magick_bmp_draw;
  FR->bmp_read = wmf_magick_bmp_read;
  FR->bmp_free = wmf_magick_bmp_free;
  FR->draw_text = wmf_magick_draw_text;
  FR->udata_init = wmf_magick_udata_init;
  FR->udata_copy = wmf_magick_udata_copy;
  FR->udata_set = wmf_magick_udata_set;
  FR->udata_free = wmf_magick_udata_free;
  FR->region_frame = wmf_magick_region_frame;
  FR->region_paint = wmf_magick_region_paint;
  FR->region_clip = wmf_magick_region_clip;

  /*
     Allocate device data structure
   */
  ddata = (wmf_magick_t *) wmf_malloc(API, sizeof(wmf_magick_t));

  if (ERR(API))
    return;

  memset((void *) ddata, 0, sizeof(wmf_magick_t));
  API->device_data = (void *) ddata;

  /*
     Device data defaults
   */
  ddata->bbox.TL.x = 0;
  ddata->bbox.TL.y = 0;
  ddata->bbox.BR.x = 0;
  ddata->bbox.BR.y = 0;
  ddata->mvg = 0;
  ddata->mvg_alloc = 0;
  ddata->mvg_length = 0;
  ddata->image = 0;
}

static void wmf_magick_draw_text(wmfAPI * API, wmfDrawText_t * draw_text)
{
  char
    *font_name = 0;		/* final font name */

  double		
    angle = 0,			/* text rotation angle */
    bbox_height,		/* bounding box height */
    bbox_width,			/* bounding box width */
    font_height_points,		/* font height in points */
    font_width_points,		/* font width in points */
    pointsize = 0;		/* pointsize to output font with desired height */

  TypeMetric
    metrics;

  wmfD_Coord
    BL,				/* bottom left of bounding box */
    BR,				/* bottom right of bounding box */
    TL,				/* top let of bounding box */
    TR;				/* top right of bounding box */

  wmfD_Coord
    point;			/* text placement point */

  wmfFont
    *font;

  wmf_magick_t
    * ddata = WMF_MAGICK_GetData(API);

  point = draw_text->pt;

  /* Choose bounding box and calculate its width and height */
  {
    double dx,
      dy;

    if ((draw_text->BR.x > draw_text->TL.x) && (draw_text->BR.y > draw_text->TL.y))
    {
      TL = draw_text->TL;
      BR = draw_text->BR;
      TR.x = draw_text->BR.x;
      TR.y = draw_text->TL.y;
      BL.x = draw_text->TL.x;
      BL.y = draw_text->BR.y;
    }
    else
    {
      TL = draw_text->bbox.TL;
      BR = draw_text->bbox.BR;
      TR = draw_text->bbox.TR;
      BL = draw_text->bbox.BL;
    }
    dx = ((TR.x - TL.x) + (BR.x - BL.x)) / 2;
    dy = ((TR.y - TL.y) + (BR.y - BL.y)) / 2;
    bbox_width = sqrt(dx * dx + dy * dy);
    dx = ((BL.x - TL.x) + (BR.x - TR.x)) / 2;
    dy = ((BL.y - TL.y) + (BR.y - TR.y)) / 2;
    bbox_height = sqrt(dx * dx + dy * dy);
  }

  font = WMF_DC_FONT(draw_text->dc);
  font_name = WMF_FONT_PSNAME(font);

  /* pixel_height and pixel_width provide the conversion factor from
     WMF_FONT_HEIGHT & WMF_FONT_WIDTH units to points */
  font_height_points = abs(WMF_FONT_HEIGHT(font) * draw_text->dc->pixel_height);
  font_width_points = abs(WMF_FONT_WIDTH(font) * draw_text->dc->pixel_width);

  magick_mvg_printf(API, "# wmf_magick_draw_text\n");

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

#if 0
  printf("\nText                    = \"%s\"\n", draw_text->str);
  /* printf("WMF_FONT_NAME:          = %s\n", WMF_FONT_NAME(font)); */
  /* printf("Postscript font         = \"%s\"\n", font_name); */
  /* printf("Text box                = %.10gx%.10g\n", bbox_width, bbox_height); */
  /* printf("WMF_FONT_HEIGHT         = %i\n", (int)WMF_FONT_HEIGHT(font)); */
  /* printf("font height (points)    = %.10g\n", font_height_points); */
  /* printf("WMF_FONT_WIDTH          = %i\n", (int)WMF_FONT_WIDTH(font)); */
  /* printf("font width (points)     = %.10g\n", font_width_points); */
  /* printf("font ratio              = %.10g\n", draw_text->font_ratio ); */
#endif

  /*
   * Correct font pointsize based on font metrics
   *
   */
  {
    Image * image = ddata->image;

    DrawInfo draw_info;

    ImageInfo * image_info;

    image_info = CloneImageInfo((ImageInfo *) NULL);
    CloneString(&image_info->font, font_name);
    image_info->pointsize = font_height_points;
    GetDrawInfo(image_info, &draw_info);
    CloneString(&draw_info.text, draw_text->str);

    if (GetTypeMetrics(image, &draw_info, &metrics) != False)
    {
      if (strlen(draw_text->str) <= 1)
      {
	/* For individual characters, FONT_HEIGHT appears to
	   specify the hight of the ascent only so calculate final
	   pointsize based on ratio of ascent to ascent+descent */
	pointsize = font_height_points *
	  ((double) font_height_points / (metrics.ascent + abs(metrics.descent)));
	draw_info.pointsize = pointsize;
      }
      else
      {
	/* For multi-character strings, the height metric seems to
	   offer the best pointsize estimation */
	pointsize =
	  font_height_points * ((double) font_height_points / (metrics.height));
	draw_info.pointsize = pointsize;

	if ((WMF_DC_TEXTALIGN(draw_text->dc) & TA_CENTER) ||
	    (WMF_TEXT_UNDERLINE(font)) || (WMF_TEXT_STRIKEOUT(font)))
	{
	  GetTypeMetrics(image, &draw_info, &metrics);

	  /* Center the text if it is not yet centered and should be */
	  if ((WMF_DC_TEXTALIGN(draw_text->dc) & TA_CENTER) &&
	      (point.x < (BL.x + 1)))
	  {
	    double
              text_width;

	    text_width = metrics.width
	      * (ddata->bbox_to_pixels_scale_y / ddata->bbox_to_pixels_scale_x);

	    point.x += bbox_width / 2 - text_width / 2;
	  }
	}
      }
    }
  }

  /* Set stroke color */
  magick_mvg_printf(API, "stroke none\n");

  /* Set fill color */
  {
    wmfRGB
      *fill = WMF_DC_TEXTCOLOR(draw_text->dc);

    magick_mvg_printf(API, "fill #%02x%02x%02x\n",
		      (int) fill->r, (int) fill->g, (int) fill->b);
  }

  /* Set under-box color */
  if (WMF_DC_OPAQUE(draw_text->dc))
  {
    wmfRGB
      *box = WMF_DC_BACKGROUND(draw_text->dc);

    magick_mvg_printf(API, "decorate #%02x%02x%02x\n",
		      (int) box->r, (int) box->g, (int) box->b);
  }
  else
    magick_mvg_printf(API, "decorate none\n");

  /* Output font size */
  magick_mvg_printf(API, "font-size %.10g\n", pointsize);

  /* Output Postscript font name */
  magick_mvg_printf(API, "font '%s'\n", font_name);

  /* Translate coordinates so target is 0,0 */
  magick_mvg_printf(API, "translate %.10g,%.10g\n", point.x, point.y);

  /* Transform horizontal scale to draw text at 1:1 ratio */
  magick_mvg_printf(API, "scale %.10g,%.10g\n",
		    ddata->bbox_to_pixels_scale_y / ddata->bbox_to_pixels_scale_x,
		    1.0);

  /* Apply rotation */
  /* ImageMagick's drawing rotation is clockwise from horizontal
     while WMF drawing rotation is counterclockwise from horizontal */
  angle = abs(RadiansToDegrees(2 * MagickPI - WMF_TEXT_ANGLE(font)));
  if (angle == 360)
    angle = 0;
  if (angle != 0)
    magick_mvg_printf(API, "rotate %.10g\n", angle);

  /*
   * Render text
   *
   */

  {
    char
      escaped_string[MaxTextExtent],
     *p,
     *q;

    int string_length;

    /*
     * Build escaped string
     */
    for (p = draw_text->str, q = escaped_string, string_length = 0;
	 *p != 0 && string_length < (sizeof(escaped_string) - 3); ++p)
    {
      if (*p == '\'')
      {
	*q++ = '\\';
	*q++ = '\\';
	string_length += 2;
      }
      else
      {
	*q++ = (*p);
	++string_length;
      }
    }
    *q = 0;

    /* Output string */
    magick_mvg_printf(API, "text 0,0 '%.1024s'\n", escaped_string);

    /* Underline text the Windows way (at the bottom) */
    if (WMF_TEXT_UNDERLINE(font))
    {
      double
        line_height;

      wmfD_Coord
        ulBR,			/* bottom right of underline rectangle */
	ulTL;			/* top left of underline rectangle */

      line_height =
	max(((double) 1 / (ddata->bbox_to_pixels_scale_x)),
	    ((double) abs(metrics.descent)) * 0.5);
      ulTL.x = 0;
      ulTL.y = abs(metrics.descent) - line_height;
      ulBR.x = metrics.width;
      ulBR.y = abs(metrics.descent);

      magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
			ulTL.x, ulTL.y, ulBR.x, ulBR.y);

    }

    /* Strikeout text the Windows way */
    if (WMF_TEXT_STRIKEOUT(font))
    {
      double line_height;

      wmfD_Coord
        ulBR,			/* bottom right of strikeout rectangle */
	ulTL;			/* top left of strikeout rectangle */

      line_height =
	max(((double) 1 / (ddata->bbox_to_pixels_scale_x)),
	    ((double) abs(metrics.descent)) * 0.5);
      ulTL.x = 0;
      ulTL.y = -(((double) metrics.ascent) / 2 + line_height / 2);
      ulBR.x = metrics.width;
      ulBR.y = -(((double) metrics.ascent) / 2 - line_height / 2);

      magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
			ulTL.x, ulTL.y, ulBR.x, ulBR.y);

    }
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_udata_init(wmfAPI * API, wmfUserData_t * userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

}

static void wmf_magick_udata_copy(wmfAPI * API, wmfUserData_t * userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

}

static void wmf_magick_udata_set(wmfAPI * API, wmfUserData_t * userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

}

static void wmf_magick_udata_free(wmfAPI * API, wmfUserData_t * userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

}

static void magick_brush(wmfAPI * API, wmfDC * dc)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  wmfRGB
    *bg_color = 0,
    *brush_color = 0;

  wmfBrush
    *brush = 0;

  unsigned int
    fill_opaque,
    fill_ROP;

  magick_mvg_printf(API, "# magick_brush\n");

  fill_opaque = (unsigned int) WMF_DC_OPAQUE(dc);
  fill_ROP = (unsigned int) WMF_DC_ROP(dc);

  bg_color = WMF_DC_BACKGROUND(dc);

  brush = WMF_DC_BRUSH(dc);

  brush_color = WMF_BRUSH_COLOR(brush);

  /* Set polygon fill rule */
  switch ((unsigned int) WMF_DC_POLYFILL(dc))	/* Is this correct ?? */
  {
    case WINDING:
      magick_mvg_printf(API, "fill-rule nonzero\n");
      break;

    case ALTERNATE:
    default:
      magick_mvg_printf(API, "fill-rule evenodd\n");
      break;
  }

  switch ((unsigned int) WMF_BRUSH_STYLE(brush))
  {
    case BS_NULL:		/* BS_HOLLOW & BS_NULL share enum */
      {
	magick_mvg_printf(API, "fill none\n");
	break;
      }
    case BS_SOLID:
      {
	/* Set fill opacity 
	 * FIXME: this is probably totally bogus.
	 */
	if (fill_opaque)
	  magick_mvg_printf(API, "fill-opacity 1.0\n");
	else
	  magick_mvg_printf(API, "fill-opacity 0.5\n");	/* semi-transparent?? */

	magick_mvg_printf(API, "fill #%02x%02x%02x\n",
			  (int) brush_color->r,
			  (int) brush_color->g, (int) brush_color->b);
	break;
      }
    case BS_HATCHED:
      {
	char
          pattern_id[30];

	sprintf(pattern_id, "fill_%lu", ddata->pattern_id);

	magick_mvg_printf(API, "push pattern %s 0,0 8,8\n", pattern_id);
	magick_mvg_printf(API, "push graphic-context\n");

	if (fill_opaque)
	{
	  magick_mvg_printf(API, "fill #%02x%02x%02x\n",
			    (int) bg_color->r, (int) bg_color->g, (int) bg_color->b);
	  magick_mvg_printf(API, "rectangle 0,0 7,7\n");
	}

	magick_mvg_printf(API, "stroke-antialias 0\n");
	magick_mvg_printf(API, "stroke-width 1\n");

	magick_mvg_printf(API, "stroke #%02x%02x%02x\n",
			  (int) brush_color->r,
			  (int) brush_color->g, (int) brush_color->b);

	switch ((unsigned int) WMF_BRUSH_HATCH(brush))
	{

	  case HS_HORIZONTAL:	/* ----- */
	    {
	      magick_mvg_printf(API, "line 0,3 7,3\n");
	      break;
	    }
	  case HS_VERTICAL:	/* ||||| */
	    {
	      magick_mvg_printf(API, "line 3,0 3,7\n");
	      break;
	    }
	  case HS_FDIAGONAL:	/* \\\\\ */
	    {
	      magick_mvg_printf(API, "line 0,0 7,7\n");
	      break;
	    }
	  case HS_BDIAGONAL:	/* ///// */
	    {
	      magick_mvg_printf(API, "line 0,7 7,0\n");
	      break;
	    }
	  case HS_CROSS:	/* +++++ */
	    {
	      magick_mvg_printf(API, "line 0,3 7,3\n");
	      magick_mvg_printf(API, "line 3,0 3,7\n");
	      break;
	    }
	  case HS_DIAGCROSS:	/* xxxxx */
	    {
	      magick_mvg_printf(API, "line 0,0 7,7\n");
	      magick_mvg_printf(API, "line 0,7 7,0\n");
	      break;
	    }
	  default:
	    {
	    }
	}
	magick_mvg_printf(API, "pop graphic-context\n");
	magick_mvg_printf(API, "pop pattern\n");
	magick_mvg_printf(API, "fill 'url(#%s)'\n", pattern_id);
	++ddata->pattern_id;
	break;
      }
    case BS_PATTERN:
      {
	printf("magick_brush: BS_PATTERN not supported\n");
	break;
      }
    case BS_INDEXED:
      {
	printf("magick_brush: BS_INDEXED not supported\n");
	break;
      }
    case BS_DIBPATTERN:
      {
	wmfBMP
          *brush_bmp = WMF_BRUSH_BITMAP(brush);

	if (brush_bmp && brush_bmp->data != 0)
	{
	  char
            composition_mode[14],
            imgspec[30],
	    pattern_id[30];

          Image
            *image;

          ExceptionInfo
            exception;

          long
            id;

	  image = (Image *) brush_bmp->data;

	  GetExceptionInfo(&exception);
	  id = SetMagickRegistry(ImageRegistryType, (void *) image,
                                 sizeof(Image), &exception);
          if( id<0 || exception.severity != UndefinedException)
            {
              if (ddata->image->exception.severity < exception.severity)
                ddata->image->exception = exception;
              return;
            }
	  sprintf(imgspec, "mpr:%li", id);

          /* Add to ID list */
	  (ddata->temp_images)[ddata->cur_temp_file_index] = id;
	  ++ddata->cur_temp_file_index;
	  if (ddata->cur_temp_file_index == ddata->max_temp_file_index)
	  {
	    ddata->max_temp_file_index += 2048;
	    ReacquireMemory((void **) &ddata->temp_images,
			    ddata->max_temp_file_index * sizeof(long));
	  }

	  sprintf(pattern_id, "fill_%lu", ddata->pattern_id);
	  magick_mvg_printf(API, "push pattern %s 0,0, %lu,%lu\n",
			    pattern_id, image->columns, image->rows);

	  strcpy(composition_mode, "Copy");	/* Default is copy */
	  switch (fill_ROP)
	  {
	    case SRCCOPY:	/* dest = source */
	      strcpy(composition_mode, "Copy");
	      break;
	    case SRCPAINT:	/* dest = source OR dest */
	      break;
	    case SRCAND:	/* dest = source AND dest */
	      break;
	    case SRCINVERT:	/* dest = source XOR dest */
	      strcpy(composition_mode, "Xor");
	      break;
	    case SRCERASE:	/* dest = source AND (NOT dest) */
	      break;
	    case NOTSRCCOPY:	/* dest = (NOT source) */
	      NegateImage(image, False);
	      strcpy(composition_mode, "Copy");
	      break;
	    case NOTSRCERASE:	/* dest = (NOT source) AND (NOT dest) */
	      break;
	    case MERGECOPY:	/* dest = (source AND pattern) */
	      break;
	    case MERGEPAINT:	/* dest = (NOT source) OR dest */
	      break;
	    case PATCOPY:	/* dest = pattern */
	      break;
	    case PATPAINT:	/* dest = DPSnoo */
	      break;
	    case PATINVERT:	/* dest = pattern XOR dest */
	      break;
	    case DSTINVERT:	/* dest = (NOT dest) */
	      break;
	    case BLACKNESS:	/* dest = BLACK bits */
	      break;
	    case WHITENESS:	/* dest = WHITE bits */
	      break;
	    default:
	      {
	      }
	  }
	  magick_mvg_printf(API, "image %s 0,0 %lu,%lu '%s'\n",
			    composition_mode, image->columns, image->rows, imgspec);
	  magick_mvg_printf(API, "pop pattern\n");
	  magick_mvg_printf(API, "fill url(#%s)\n", pattern_id);

	  ++ddata->pattern_id;
	}
	else
	  printf("magick_brush: no image data!\n");

	break;
      }
    case BS_DIBPATTERNPT:
      {
	printf("magick_brush: BS_DIBPATTERNPT not supported\n");
	break;
      }
    case BS_PATTERN8X8:
      {
	printf("magick_brush: BS_PATTERN8X8 not supported\n");
	break;
      }
    case BS_DIBPATTERN8X8:
      {
	printf("magick_brush: BS_DIBPATTERN8X8 not supported\n");
	break;
      }
    default:
      {
      }
  }
}

static void magick_pen(wmfAPI * API, wmfDC * dc)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  wmfPen
    *pen = 0;

  wmfRGB
    *pen_color = 0;

  double
    pen_width,
    pixel_width;

  unsigned int
    pen_endcap,
    pen_join,
    pen_style,
    pen_type;

  magick_mvg_printf(API, "# magick_pen\n");

  pen = WMF_DC_PEN(dc);

  pen_color = WMF_PEN_COLOR(pen);

  pen_width = (WMF_PEN_WIDTH(pen) + WMF_PEN_HEIGHT(pen)) / 2;

  /* Pixel width is inverse of pixel scale */
  pixel_width = (((double) 1 / (ddata->bbox_to_pixels_scale_x)) +
		 ((double) 1 / (ddata->bbox_to_pixels_scale_y))) / 2;

  /* Don't allow pen_width to be less than pixel_width in order to
     avoid dissapearing or spider-web lines */
  pen_width = max(pen_width, pixel_width);

  pen_style = (unsigned int) WMF_PEN_STYLE(pen);
  pen_endcap = (unsigned int) WMF_PEN_ENDCAP(pen);
  pen_join = (unsigned int) WMF_PEN_JOIN(pen);
  pen_type = (unsigned int) WMF_PEN_TYPE(pen);

  /* Pen style specified? */
  if (pen_style == PS_NULL)
  {
    magick_mvg_printf(API, "stroke none\n");
    return;
  }

  magick_mvg_printf(API, "stroke-antialias 1\n");
  magick_mvg_printf(API, "stroke-width %.10g\n", max(0, pen_width));

  switch (pen_endcap)
  {
    case PS_ENDCAP_SQUARE:
      magick_mvg_printf(API, "stroke-linecap square\n");
      break;

    case PS_ENDCAP_ROUND:
      magick_mvg_printf(API, "stroke-linecap round\n");
      break;

    case PS_ENDCAP_FLAT:
    default:
      magick_mvg_printf(API, "stroke-linecap butt\n");
      break;
  }

  switch (pen_join)
  {
    case PS_JOIN_BEVEL:
      magick_mvg_printf(API, "stroke-linejoin bevel\n");
      break;

    case PS_JOIN_ROUND:
      magick_mvg_printf(API, "stroke-linejoin round\n");
      break;

    case PS_JOIN_MITER:
    default:
      magick_mvg_printf(API, "stroke-linejoin miter\n");
      break;
  }

  switch (pen_style)
  {
    case PS_DASH:		/* -------  */
      /* Pattern 18,7 */
      magick_mvg_printf(API, "stroke-dasharray %.10g,%.10g\n",
			pixel_width * 17, pixel_width * 8);
      break;

    case PS_ALTERNATE:
    case PS_DOT:		/* .......  */
      /* Pattern 3,3 */
      magick_mvg_printf(API, "stroke-antialias 0\n");
      magick_mvg_printf(API, "stroke-dasharray %.10g,%.10g\n",
			pixel_width * 3, pixel_width * 3);
      break;

    case PS_DASHDOT:		/* _._._._  */
      /* Pattern 9,6,3,6 */
      magick_mvg_printf(API, "stroke-antialias 0\n");
      magick_mvg_printf(API, "stroke-dasharray %.10g,%.10g,%.10g,%.10g\n",
			pixel_width * 9, pixel_width * 6, pixel_width * 3,
			pixel_width * 6);
      break;

    case PS_DASHDOTDOT:	/* _.._.._  */
      /* Pattern 9,3,3,3,3,3 */
      magick_mvg_printf(API, "stroke-antialias 0\n");
      magick_mvg_printf(API,
			"stroke-dasharray %.10g,%.10g,%.10g,%.10g,%.10g,%.10g\n",
			pixel_width * 9, pixel_width * 3, pixel_width * 3,
			pixel_width * 3, pixel_width * 3, pixel_width * 3);
      break;

    case PS_INSIDEFRAME:	/* There is nothing to do in this case... */
    case PS_SOLID:
    default:
      magick_mvg_printf(API, "stroke-dasharray none\n");
      break;
  }

  magick_mvg_printf(API, "stroke #%02x%02x%02x\n",
		    (int) pen_color->r, (int) pen_color->g, (int) pen_color->b);
}

/* Extend MVG, printf style */
static int magick_mvg_printf(wmfAPI * API, char *format, ...)
{
  long
    str_length;

  va_list
    argp;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Allocate initial memory */
  if(ddata->mvg == 0)
    {
      ddata->mvg = AcquireMemory(MaxTextExtent);
      ddata->mvg_alloc = MaxTextExtent;
      ddata->mvg_length = 0;
    }

  /* Re-allocate additional MaxTextExtent memory if necessary */
  if(ddata->mvg_alloc < (ddata->mvg_length+MaxTextExtent))
    {
      ReacquireMemory((void**)&ddata->mvg,ddata->mvg_length+MaxTextExtent);
      ddata->mvg_alloc = ddata->mvg_length+MaxTextExtent;
    }

  /* Write to end of existing MVG string */
  va_start(argp, format);
  str_length = vsnprintf(ddata->mvg+ddata->mvg_length, MaxTextExtent - 1, format, argp);
  /* printf("%s",ddata->mvg+ddata->mvg_length); */
  ddata->mvg_length += str_length;
  va_end(argp);

  return str_length;
}

/* Scribble MVG on image */
static void magick_render_mvg(wmfAPI * API)
{
  DrawInfo
    *draw_info;

  ImageInfo
    *image_info;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  image_info = (ImageInfo *) AcquireMemory(sizeof(ImageInfo));
  GetImageInfo(image_info);
  draw_info = (DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  GetDrawInfo(image_info, draw_info);
  draw_info->primitive = ddata->mvg;
  /* puts(draw_info->primitive); */
  DrawImage(ddata->image, draw_info);
  draw_info->primitive = (char *) NULL;
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(image_info);
}

static Image *ReadWMFImage(const ImageInfo * image_info, ExceptionInfo * exception)
{
  Image
    *image;

  ImageInfo
    *canvas_info;

  char
    buff[MaxTextExtent],
    font_map_path[MaxTextExtent];

  float
    wmf_width,
    wmf_height;

  double
    bounding_height,
    bounding_width,
    image_height,
    image_height_inch,
    image_width,
    image_width_inch,
    resolution_y,
    resolution_x,
    units_per_inch;

  unsigned long
    wmf_options_flags = 0;

  wmf_error_t
    wmf_error;

  wmf_magick_t
    *ddata = 0;

  wmfAPI
    *API = 0;

  wmfAPI_Options
    wmf_api_options;

  wmfD_Rect
    bounding_box;

  image = AllocateImage(image_info);
  GetExceptionInfo(exception);

  /*
   * Create WMF API
   *
   */

  /* Register callbacks */
  wmf_options_flags |= WMF_OPT_FUNCTION;
  memset(&wmf_api_options, 0, sizeof(wmf_api_options));
  wmf_api_options.function = wmf_magick_function;

  /* Ignore non-fatal errors */
  wmf_options_flags |= WMF_OPT_IGNORE_NONFATAL;

  /* Use ImageMagick's font map file */
  {
    char
      *p = GetMagickConfigurePath(TypeFilename);

    if (p != NULL)
    {
      strcpy(font_map_path, p);
      wmf_options_flags |= WMF_OPT_XTRA_FONTS;
      wmf_options_flags |= WMF_OPT_XTRA_FONTMAP;
      wmf_api_options.xtra_fontmap_file = font_map_path;
    }
  }

  wmf_error = wmf_api_create(&API, wmf_options_flags, &wmf_api_options);
  if (wmf_error != wmf_E_None)
  {
    if (API)
      wmf_api_destroy(API);
    ThrowReaderException(DelegateError, "Failed to intialize libwmf", image);
  }

  /*
   * Open WMF file
   *
   */
  wmf_error = wmf_file_open(API, (char *) image_info->filename);
  if (wmf_error != wmf_E_None)
  {
    wmf_api_destroy(API);
    ThrowReaderException(FileOpenError, "Unable to open file", image);
  }

  /*
   * Scan WMF file
   *
   */
  wmf_error = wmf_scan(API, 0, &bounding_box);
  if (wmf_error != wmf_E_None)
  {
    wmf_api_destroy(API);
    ThrowReaderException(CorruptImageError, "Failed to scan file", image);
  }

  /*
   * Compute dimensions and scale factors
   *
   */

  ddata = WMF_MAGICK_GetData(API);
  ddata->bbox = bounding_box;

  /* User specified resolution */
  resolution_y = 72.0;
  if (image->y_resolution > 0)
  {
    resolution_y = image->y_resolution;
    if (image->units == PixelsPerCentimeterResolution)
      resolution_y *= CENTIMETERS_PER_INCH;
  }

  resolution_x = 72.0;
  if (image->x_resolution > 0)
  {
    resolution_x = image->x_resolution;
    if (image->units == PixelsPerCentimeterResolution)
      resolution_x *= CENTIMETERS_PER_INCH;
  }

  /* Obtain output size expressed in metafile units */
  wmf_error = wmf_size(API, &wmf_width, &wmf_height);
  if (wmf_error != wmf_E_None)
  {
    wmf_api_destroy(API);
    ThrowReaderException(CorruptImageError, "Failed to compute output size", image);
  }

  /* Obtain metafile units */
  units_per_inch = TWIPS_PER_INCH;
  if ((API)->File->placeable)
    units_per_inch = (API)->File->pmh->Inch;

  /* Calculate image width and height based on specified DPI resolution */
  image_width_inch = (double) wmf_width / units_per_inch;
  image_width = image_width_inch * resolution_x;
  image_height_inch = (double) wmf_height / units_per_inch;
  image_height = image_height_inch * resolution_y;

  /* Compute bounding box scale factors */
  bounding_width = ddata->bbox.BR.x - ddata->bbox.TL.x;
  bounding_height = ddata->bbox.BR.y - ddata->bbox.TL.y;

  ddata->bbox_to_pixels_scale_x = image_width / bounding_width;
  ddata->bbox_to_pixels_scale_y = image_height / bounding_height;

  /* Compute translation to place bounding box at image origin */
  ddata->bbox_to_pixels_translate_x = -(ddata->bbox.TL.x);
  ddata->bbox_to_pixels_translate_y = -(ddata->bbox.TL.y);

#if 0
  printf("Size in metafile units:      %.10gx%.10g\n", wmf_width, wmf_height);
  printf("Metafile units/inch:         %.10g\n", units_per_inch);
  printf("Bounding Box:                %.10g,%.10g %.10g,%.10g\n", bounding_box.TL.x,
	 bounding_box.TL.y, bounding_box.BR.x, bounding_box.BR.y);
  printf("Output resolution:           %.10gx%.10g\n", resolution_x, resolution_y);
  printf("Image size:                  %.10gx%.10g\n", image_width, image_height);
  printf("Bounding box scale factor:   %.10g,%.10g\n", ddata->bbox_to_pixels_scale_x,
	 ddata->bbox_to_pixels_scale_y);
  printf("Translation:                 %.10g,%.10g\n",
	 ddata->bbox_to_pixels_translate_x, ddata->bbox_to_pixels_translate_y);
#endif

  /*
   * Create canvas image
   *
   */
  canvas_info = CloneImageInfo(image_info);
  sprintf(buff, "%ix%i", (int) ceil(image_width), (int) ceil(image_height));
  (void) CloneString(&(canvas_info->size), buff);
  if (image_info->texture == (char *) NULL)
  {
    QueryColorDatabase("none", &canvas_info->background_color);
    if (ColorMatch(&image_info->background_color, &canvas_info->background_color, 0))
      QueryColorDatabase("white", &canvas_info->background_color);
    else
      canvas_info->background_color = image_info->background_color;

    sprintf(canvas_info->filename,
#if QuantumDepth == 8
	    "XC:#%02x%02x%02x%02x",
#elif QuantumDepth == 16
	    "XC:#%04x%04x%04x%04x",
#endif
	    canvas_info->background_color.red,
	    canvas_info->background_color.green,
	    canvas_info->background_color.blue,
	    canvas_info->background_color.opacity);
  }
  else
    sprintf(canvas_info->filename, "TILE:%.1024s", image_info->texture);
  GetExceptionInfo(exception);
  DestroyImage(image);
  image = ReadImage(canvas_info, exception);
  if (image == (Image *) NULL)
  {
    wmf_api_destroy(API);
    return image;
  }
  image->units = PixelsPerInchResolution;
  image->x_resolution = resolution_x;
  image->y_resolution = resolution_y;

  strncpy(image->filename, image_info->filename, MaxTextExtent - 1);
  strncpy(image->magick, image_info->magick, MaxTextExtent - 1);
  ddata->image = image;
  ddata->image_info = canvas_info;

  /*
   * Play file to generate MVG drawing commands
   *
   */
  ddata->mvg = AcquireMemory(MaxTextExtent);

  wmf_error = wmf_play(API, 0, &bounding_box);
  if (wmf_error != wmf_E_None)
  {
    DestroyImageInfo(canvas_info);
    wmf_api_destroy(API);
    ThrowReaderException(CorruptImageError, "Failed to render file", image);
  }

  /*
   * Scribble on canvas image
   *
   */
  magick_render_mvg(API);

  /* Cleanup allocated data */
  DestroyImageInfo(canvas_info);
  wmf_api_destroy(API);

  /* Return image */
  return image;
}
#endif
#endif /* HasWMF */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r W M F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterWMFImage adds attributes for the WMF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterWMFImage method is:
%
%      RegisterWMFImage(void)
%
*/
ModuleExport void RegisterWMFImage(void)
{
#if defined(HasWMF)
  MagickInfo
    *entry;

  entry = SetMagickInfo("WMF");
  entry->decoder = ReadWMFImage;
  entry->description = AllocateString("Windows Meta File");
  entry->blob_support = False;
  entry->module = AllocateString("WMF");
  (void) RegisterMagickInfo(entry);
#if defined(WIN32)
  entry = SetMagickInfo("EMF");
  entry->decoder = ReadWMFImage;
  entry->description = AllocateString("Windows Meta File");
  entry->blob_support = False;
  entry->module = AllocateString("WMF");
  (void) RegisterMagickInfo(entry);
#endif
#endif /* defined(HasWMF) */
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r W M F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterWMFImage removes format registrations made by the
%  WMF module from the list of supported formats.
%
%  The format of the UnregisterWMFImage method is:
%
%      UnregisterWMFImage(void)
%
*/
ModuleExport void UnregisterWMFImage(void)
{
#if defined(HasWMF)
  (void) UnregisterMagickInfo("WMF");
#endif /* defined(HasWMF) */
}
