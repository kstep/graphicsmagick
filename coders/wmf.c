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
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
% libwmf's `magick' device layer made possible by Bob Friesenhahn who wrote
% ImageMagick's WMF coder... any bugs are, however, my fault        --- fjf
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

#if defined(HasWMF)
#if !defined(WIN32)

#ifdef  min
#undef  min
#endif  /* min */
#define min(a, b) ((a) < (b) ? (a) : (b))

#ifdef  max
#undef  max
#endif  /* max */
#define max(a, b) ((a) < (b) ? (b) : (a))

#ifdef abs
#undef abs
#endif /* abs */
#define abs(x) ((x) >= 0 ? (x) : -(x))

#define ERR(API)  ((API)->err != wmf_E_None)
#define DIAG(API) ((API)->flags & WMF_OPT_DIAGNOSTICS)

#define TWIPS_INCH 1440
#define CENTIMETERS_INCH 2.54

#include "libwmf/fund.h"
#include "libwmf/types.h"
#include "libwmf/api.h"
#include "libwmf/defs.h"
#include "libwmf/ipa.h"
#include "libwmf/color.h"
#include "libwmf/macro.h"

typedef struct _wmf_magick_t wmf_magick_t;

struct _wmf_magick_t
{
  /* Bounding box */
  wmfD_Rect bbox;

  /* Output stream */
  wmfStream* out;

  /* ImageMagick image */
  Image *image;

  /* Maximum number of temporary files */
  int max_temp_file_index;

  /* Current number of temporary files */
  int cur_temp_file_index;

  /* Temporary file names */
  char **temp_files;
};

#define WMF_MAGICK_GetData(Z) ((wmf_magick_t*)((Z)->device_data))

typedef enum
{
  magick_arc_ellipse = 0,
  magick_arc_open,
  magick_arc_pie,
  magick_arc_chord
} magick_arc_t;

typedef struct _magickPoint magickPoint;

struct _magickPoint
{
  double x;
  double y;
};

static double magick_height(wmfAPI *API, double wmf_height);
static double magick_width(wmfAPI *API, double wmf_width);
static magickPoint magick_translate(wmfAPI *API, wmfD_Coord d_pt);
static void magick_brush(wmfAPI *API, wmfDC *dc);
static void magick_draw_arc(wmfAPI *API, wmfDrawArc_t *draw_arc,
                            magick_arc_t finish);
static void magick_pen(wmfAPI *API, wmfDC *dc);
static void wmf_magick_bmp_draw(wmfAPI *API, wmfBMP_Draw_t *bmp_draw);
static void wmf_magick_bmp_free(wmfAPI *API, wmfBMP *bmp);
static void wmf_magick_bmp_read(wmfAPI *API, wmfBMP_Read_t *bmp_read);
static void wmf_magick_device_begin(wmfAPI *API);
static void wmf_magick_device_close(wmfAPI *API);
static void wmf_magick_device_end(wmfAPI *API);
static void wmf_magick_device_open(wmfAPI *API);
static void wmf_magick_draw_arc(wmfAPI *API, wmfDrawArc_t *draw_arc);
static void wmf_magick_draw_chord(wmfAPI *API, wmfDrawArc_t *draw_arc);
static void wmf_magick_draw_ellipse(wmfAPI *API, wmfDrawArc_t *draw_arc);
static void wmf_magick_draw_line(wmfAPI *API, wmfDrawLine_t *draw_line);
static void wmf_magick_draw_pie(wmfAPI *API, wmfDrawArc_t *draw_arc);
static void wmf_magick_draw_pixel(wmfAPI *API, wmfDrawPixel_t *draw_pixel);
static void wmf_magick_draw_polygon(wmfAPI *API, wmfPolyLine_t *poly_line);
static void wmf_magick_draw_rectangle(wmfAPI *API,
                                      wmfDrawRectangle_t *draw_rect);
static void wmf_magick_draw_text(wmfAPI *API, wmfDrawText_t *draw_text);
static void wmf_magick_flood_exterior(wmfAPI *API, wmfFlood_t *flood);
static void wmf_magick_flood_interior(wmfAPI *API, wmfFlood_t *flood);
static void wmf_magick_function(wmfAPI *API);
static void wmf_magick_poly_line(wmfAPI *API, wmfPolyLine_t *poly_line);
static void wmf_magick_region_clip(wmfAPI *API, wmfPolyRectangle_t *poly_rect);
static void wmf_magick_region_frame(wmfAPI *API,
                                    wmfPolyRectangle_t *poly_rect);
static void wmf_magick_region_paint(wmfAPI *API,
                                    wmfPolyRectangle_t *poly_rect);
static void wmf_magick_rop_draw(wmfAPI *API, wmfROP_Draw_t *rop_draw);
static void wmf_magick_udata_copy(wmfAPI *API, wmfUserData_t *userdata);
static void wmf_magick_udata_free(wmfAPI *API, wmfUserData_t *userdata);
static void wmf_magick_udata_init(wmfAPI *API, wmfUserData_t *userdata);
static void wmf_magick_udata_set(wmfAPI *API, wmfUserData_t *userdata);


static void wmf_magick_rop_draw (wmfAPI* API,wmfROP_Draw_t* rop_draw)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]rop_draw");

	
}

/* TODO ?? Care about bmp_draw->type
 */
static void wmf_magick_bmp_draw (wmfAPI* API,wmfBMP_Draw_t* bmp_draw)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  double
    height,
    width;

  char
    *tmpname;

  magickPoint
    pt;

  wmfStream
    *out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]bmp_draw");

  if (out == 0) return;

  if (bmp_draw->bmp.data == 0) return;

  tmpname = (char*)AcquireMemory(MaxTextExtent*sizeof(char));
  *tmpname = '\0';
  TemporaryFilename(tmpname);
  (ddata->temp_files)[ddata->cur_temp_file_index] = tmpname;
  ++ddata->cur_temp_file_index;
  if(ddata->cur_temp_file_index == ddata->max_temp_file_index)
    {
      ddata->max_temp_file_index += 2048;
      ReacquireMemory((void **) &ddata->temp_files, ddata->max_temp_file_index*sizeof(char *));
    }

  wmf_ipa_bmp_png (API,bmp_draw,tmpname);

  if (ERR (API))
    {	WMF_DEBUG (API,"bailing...");
    return;
    }

  /* Okay, if we've got this far then "tmpname" is the filename of an png
     (cropped) image */

  pt = magick_translate (API,bmp_draw->pt);

  width  = ceil(abs(magick_width(API,bmp_draw->pixel_width*(double)bmp_draw->crop.w)));
  height = ceil(abs(magick_height(API,bmp_draw->pixel_height*(double)bmp_draw->crop.h)));

  wmf_stream_printf (API,out,"image Copy %f,%f %i,%i '%s'\n",
                     pt.x,pt.y,(int)width,(int)height,tmpname);
}

static void wmf_magick_bmp_read (wmfAPI* API,wmfBMP_Read_t* bmp_read)
{
  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]bmp_read");

  wmf_ipa_bmp_read (API,bmp_read);
}

static void wmf_magick_bmp_free (wmfAPI* API,wmfBMP* bmp)
{
  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]bmp_free");

  wmf_ipa_bmp_free (API,bmp);
}


/* This is called by wmf_play() the *first* time the meta file is played
 */
static void wmf_magick_device_open (wmfAPI* API)
{
/*   wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_open");

	
}

/* This is called by wmf_api_destroy()
 */
static void wmf_magick_device_close (wmfAPI* API)
{
  int
    index;

  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_close");

  /* Remove any temporary files */
  if(ddata->temp_files != (char **)NULL)
    {
      for( index=0; index<ddata->cur_temp_file_index; index++ )
        {
          remove((ddata->temp_files)[index]);
          LiberateMemory((void**)&(ddata->temp_files)[index]);
        }
      LiberateMemory((void**)&ddata->temp_files);
    }
}

/* This is called from the beginning of each play for initial page setup
 */
static void wmf_magick_device_begin (wmfAPI* API)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_begin");

  if (out == 0) return;

  if ((out->reset (out->context)) &&
      ((API->flags & WMF_OPT_IGNORE_NONFATAL) == 0))
    {
      WMF_ERROR (API,"unable to reset output stream!");
      API->err = wmf_E_DeviceError;
      return;
    }

  if ((ddata->bbox.BR.x <= ddata->bbox.TL.x) ||
      (ddata->bbox.BR.y <= ddata->bbox.TL.y))
    {
      WMF_ERROR (API,"~~~~~~~~wmf_[magick_]device_begin: "
                 "bounding box has null or negative size!");
      API->err = wmf_E_Glitch;
      return;
    }

  ddata->max_temp_file_index = 2048;
  ddata->cur_temp_file_index = 0;
  ddata->temp_files = (char**)AcquireMemory(ddata->max_temp_file_index*sizeof(char *));
  
  wmf_stream_printf (API,out,"viewbox 0 0 %u %u\n",ddata->image->columns,ddata->image->rows);
}

/* This is called from the end of each play for page termination
 */
static void wmf_magick_device_end (wmfAPI* API)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_end");

  if (out == 0) return;

	
}

static magickPoint magick_translate (wmfAPI* API,wmfD_Coord d_pt)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  magickPoint g_pt;

  double x;
  double y;

  x = ((double) d_pt.x - (double) ddata->bbox.TL.x);
  x /= ((double) ddata->bbox.BR.x - (double) ddata->bbox.TL.x);
  x *= (double) ddata->image->columns;

  y = ((double) d_pt.y - (double) ddata->bbox.TL.y);
  y /= ((double) ddata->bbox.BR.y - (double) ddata->bbox.TL.y);
  y *= (double) ddata->image->rows;

  g_pt.x = x;
  g_pt.y = y;

  return (g_pt);
}

static double magick_width (wmfAPI* API,double wmf_width)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  double width;

  width = wmf_width * ddata->image->columns;
  width /= ((double) ddata->bbox.BR.x - (double) ddata->bbox.TL.x);

  return (width);
}

static double magick_height (wmfAPI* API,double wmf_height)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  double height;

  height = wmf_height * ddata->image->rows;
  height /= ((double) ddata->bbox.BR.y - (double) ddata->bbox.TL.y);

  return (height);
}

static void wmf_magick_flood_interior (wmfAPI* API,wmfFlood_t* flood)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfRGB* rgb = &(flood->color);

  magickPoint pt;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]flood_interior");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",
                     (int)rgb->r,(int)rgb->g,(int)rgb->b);

  pt = magick_translate (API,flood->pt);
  wmf_stream_printf (API,out,"color %f,%f filltoborder\n",pt.x,pt.y);

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_flood_exterior (wmfAPI* API,wmfFlood_t* flood)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfRGB* rgb = &(flood->color);

  magickPoint pt;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]flood_exterior");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  pt = magick_translate (API,flood->pt);

  wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",
                     (int)rgb->r,(int)rgb->g,(int)rgb->b);

  if (flood->type == FLOODFILLSURFACE)
    wmf_stream_printf (API,out,"color %f,%f floodfill\n",pt.x,pt.y);
  else
    wmf_stream_printf (API,out,"color %f,%f filltoborder\n",pt.x,pt.y);

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_draw_pixel (wmfAPI* API,wmfDrawPixel_t* draw_pixel)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfRGB* rgb = &(draw_pixel->color);

  magickPoint pt;

  double width;
  double height;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_pixel");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  pt = magick_translate (API,draw_pixel->pt);

  width  = magick_width  (API,draw_pixel->pixel_width);
  height = magick_height (API,draw_pixel->pixel_height);

  wmf_stream_printf (API,out,"stroke none\n");

  wmf_stream_printf (API,out,"fill-opacity 1\n");

  wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",
                     (int)rgb->r,(int)rgb->g,(int)rgb->b);

  wmf_stream_printf (API,out,"rectangle %f,%f %f,%f\n",
                     pt.x,pt.y,pt.x+width,pt.y+height);

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_draw_pie (wmfAPI* API,
                                 wmfDrawArc_t* draw_arc)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_pie");

  magick_draw_arc (API,draw_arc,magick_arc_pie);
}

static void wmf_magick_draw_chord (wmfAPI* API,
                                   wmfDrawArc_t* draw_arc)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_chord");

  magick_draw_arc (API,draw_arc,magick_arc_chord);
}

static void wmf_magick_draw_arc (wmfAPI* API,
                                 wmfDrawArc_t* draw_arc)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_arc");

  magick_draw_arc (API,draw_arc,magick_arc_open);
}

static void wmf_magick_draw_ellipse (wmfAPI* API,
                                     wmfDrawArc_t* draw_arc)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_arc");

  magick_draw_arc (API,draw_arc,magick_arc_ellipse);
}

static void magick_draw_arc (wmfAPI* API,
                             wmfDrawArc_t* draw_arc,
                             magick_arc_t finish)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfD_Coord centre;

  magickPoint TL;
  magickPoint BR;

  magickPoint O;

  magickPoint start;
  magickPoint end;

  double phi_s = 0;
  double phi_e = 360;

  double Rx;
  double Ry;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~magick_draw_arc");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  if (TO_FILL (draw_arc) || TO_DRAW (draw_arc))
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

      TL = magick_translate (API,draw_arc->TL);
      BR = magick_translate (API,draw_arc->BR);

      O = magick_translate (API,centre);

      if (finish != magick_arc_ellipse)
        {
          start = magick_translate (API,draw_arc->start);
          end   = magick_translate (API,draw_arc->end  );
        }

      Rx = (BR.x - TL.x) / 2;
      Ry = (BR.y - TL.y) / 2;

      if (finish != magick_arc_ellipse)
        {
          start.x -= O.x;
          start.y -= O.y;

          end.x -= O.x;
          end.y -= O.y;

          phi_s = atan2((double)start.y,(double) start.x)*180/MagickPI;
          phi_e = atan2((double)end.y,(double)end.x)*180/MagickPI;

          if (phi_e <= phi_s) phi_e += 360;
        }

      if (finish == magick_arc_open)
	wmf_stream_printf (API,out,"fill none\n");
      else
	magick_brush (API,draw_arc->dc);
      magick_pen (API,draw_arc->dc);

      if (finish == magick_arc_ellipse)
	wmf_stream_printf (API,out,"ellipse %f,%f %f,%f 0,360\n",
                           O.x,O.y,Rx,Ry);
      else if (finish == magick_arc_pie)
	wmf_stream_printf (API,out,"ellipse %f,%f %f,%f %f,%f\n",
                           O.x,O.y,Rx,Ry,phi_s,phi_e);
      else if (finish == magick_arc_chord)
        {
          wmf_stream_printf (API,out,"arc %f,%f %f,%f %f,%f\n",
                             O.x,O.y,Rx,Ry,phi_s,phi_e);
          wmf_stream_printf (API,out,"line %f,%f %f,%f\n",
                             start.x,start.y,end.x,end.y);
        }
      else /* if (finish == magick_arc_open) */
        wmf_stream_printf (API,out,"arc %f,%f %f,%f %f,%f\n",
                           O.x,O.y,Rx,Ry,phi_s,phi_e);
    }

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_draw_line (wmfAPI* API,
                                  wmfDrawLine_t* draw_line)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  magickPoint from;
  magickPoint to;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_line");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  if (TO_DRAW (draw_line))
    { 
      from = magick_translate (API,draw_line->from);
      to   = magick_translate (API,draw_line->to  );

      magick_pen (API,draw_line->dc);

      wmf_stream_printf (API,out,"line %f,%f %f,%f\n",from.x,from.y,to.x,to.y);
    }

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_poly_line (wmfAPI* API,wmfPolyLine_t* poly_line)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  magickPoint pt;

  U16 i;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]poly_line");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  if (poly_line->count <= 1) return;

  if (TO_DRAW (poly_line))
    {
      magick_pen (API,poly_line->dc);

      wmf_stream_printf (API,out,"polyline");

      for (i = 0; i < poly_line->count; i++)
        {
          pt = magick_translate (API,poly_line->pt[i]);
          wmf_stream_printf (API,out," %f,%f",pt.x,pt.y);
        }

      wmf_stream_printf (API,out,"\n");
    }

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_draw_polygon (wmfAPI* API,wmfPolyLine_t* poly_line)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  magickPoint pt;

  U16 i;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_polygon");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  if (poly_line->count <= 2) return;

  if (TO_FILL (poly_line) || TO_DRAW (poly_line))
    {
      magick_brush (API,poly_line->dc);
      magick_pen (API,poly_line->dc);

      wmf_stream_printf (API,out,"polygon");

      for (i = 0; i < poly_line->count; i++)
        {
          pt = magick_translate (API,poly_line->pt[i]);
          wmf_stream_printf (API,out," %f,%f",pt.x,pt.y);
        }

      wmf_stream_printf (API,out,"\n");
    }

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_draw_rectangle (wmfAPI* API,
                                       wmfDrawRectangle_t* draw_rect)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  magickPoint TL;
  magickPoint BR;

  double width;
  double height;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_rectangle");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  if (TO_FILL (draw_rect) || TO_DRAW (draw_rect))
    {
      magick_brush (API,draw_rect->dc);
      magick_pen (API,draw_rect->dc);

      TL = magick_translate (API,draw_rect->TL);
      BR = magick_translate (API,draw_rect->BR);

      if ((draw_rect->width > 0) || (draw_rect->height > 0))
        {
          width  = magick_width  (API,draw_rect->width ) / 2;
          height = magick_height (API,draw_rect->height) / 2;

          wmf_stream_printf (API,out,"roundrectangle %f,%f %f,%f %f,%f\n",
                             TL.x,TL.y,BR.x,BR.y,width,height);
        }
      else
	wmf_stream_printf (API,out,"rectangle %f,%f %f,%f\n",
                           TL.x,TL.y,BR.x,BR.y);
    }

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_region_frame (wmfAPI* API,
                                     wmfPolyRectangle_t* poly_rect)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]region_frame");

	
}

static void wmf_magick_region_paint (wmfAPI* API,
                                     wmfPolyRectangle_t* poly_rect)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]region_paint");

	
}

static void wmf_magick_region_clip (wmfAPI* API,
                                    wmfPolyRectangle_t* poly_rect)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]region_clip");

	
}

static void wmf_magick_function (wmfAPI* API)
{
  wmf_magick_t* ddata = 0;

  wmfFunctionReference* FR = (wmfFunctionReference*) API->function_reference;

  /* IPA function reference links
   */
  FR->device_open    = wmf_magick_device_open;
  FR->device_close   = wmf_magick_device_close;
  FR->device_begin   = wmf_magick_device_begin;
  FR->device_end     = wmf_magick_device_end;
  FR->flood_interior = wmf_magick_flood_interior;
  FR->flood_exterior = wmf_magick_flood_exterior;
  FR->draw_pixel     = wmf_magick_draw_pixel;
  FR->draw_pie       = wmf_magick_draw_pie;
  FR->draw_chord     = wmf_magick_draw_chord;
  FR->draw_arc       = wmf_magick_draw_arc;
  FR->draw_ellipse   = wmf_magick_draw_ellipse;
  FR->draw_line      = wmf_magick_draw_line;
  FR->poly_line      = wmf_magick_poly_line;
  FR->draw_polygon   = wmf_magick_draw_polygon;
  FR->draw_rectangle = wmf_magick_draw_rectangle;
  FR->rop_draw       = wmf_magick_rop_draw;
  FR->bmp_draw       = wmf_magick_bmp_draw;
  FR->bmp_read       = wmf_magick_bmp_read;
  FR->bmp_free       = wmf_magick_bmp_free;
  FR->draw_text      = wmf_magick_draw_text;
  FR->udata_init     = wmf_magick_udata_init;
  FR->udata_copy     = wmf_magick_udata_copy;
  FR->udata_set      = wmf_magick_udata_set;
  FR->udata_free     = wmf_magick_udata_free;
  FR->region_frame   = wmf_magick_region_frame;
  FR->region_paint   = wmf_magick_region_paint;
  FR->region_clip    = wmf_magick_region_clip;

  /* Allocate device data structure
   */
  ddata = (wmf_magick_t*) wmf_malloc (API,sizeof (wmf_magick_t));

  if (ERR (API))
    {
      WMF_DEBUG (API,"bailing...");
      return;
    }

  API->device_data = (void*) ddata;

  /* Device data defaults
   */
  ddata->bbox.TL.x = 0;
  ddata->bbox.TL.y = 0;
  ddata->bbox.BR.x = 0;
  ddata->bbox.BR.y = 0;

  ddata->out = 0;

  ddata->image = 0;
}

static void wmf_magick_draw_text (wmfAPI* API,
                                  wmfDrawText_t* draw_text)
{
  char
    buff[MaxTextExtent],
    *p,
    *font_name=0,
    *font_path = 0,
    *q;

  int
    pointsize,
    string_length;

  double
    angle = 0;

  wmfFont
    *font;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData (API);

  wmfStream
    *out = ddata->out;

  WMF_DEBUG (API,"wmf_[magick_]draw_text");

  if (out == 0) return;

  font = WMF_DC_FONT (draw_text->dc);
/*   printf("WMF_FONT_NAME: %s\n", WMF_FONT_NAME(font)); */

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  /* Set stroke color */
  wmf_stream_printf (API,out,"stroke none\n");

  /* Set fill color */
  {
    wmfRGB* fill = WMF_DC_TEXTCOLOR (draw_text->dc);
    wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",
                       (int)fill->r,(int)fill->g,(int)fill->b);
  }

  /* Set box color */
  if(WMF_DC_OPAQUE(draw_text->dc))
    {
      wmfRGB* box = WMF_DC_BACKGROUND (draw_text->dc);
      wmf_stream_printf (API,out,"decorate #%02x%02x%02x\n",
                         (int)box->r,(int)box->g,(int)box->b);
    }

  /* Output font file name */
  font_name = WMF_FONT_PSNAME (font);
/*   printf("WMF_FONT_PSNAME: %s\n", font_name); */
  font_path = wmf_ipa_font_lookup (API,font_name);
/*   if (font_path) */
/*     printf("Font path: %s\n", font_path); */
  wmf_stream_printf (API,out,"font '%s'\n",font_name);

  /* Set underline */
  if(WMF_TEXT_UNDERLINE(font))
    wmf_stream_printf (API,out,"decorate underline\n");

  /* Set strikeout */
  if(WMF_TEXT_STRIKEOUT(font))
    wmf_stream_printf (API,out,"decorate line-through\n");

  /* Set font size */
  /*   printf("========= Font Height  : %i\n", (int)WMF_FONT_HEIGHT(font)); */
  /*   printf("========= Font Width   : %i\n", (int)WMF_FONT_WIDTH(font)); */
  pointsize = ceil((((double)2.54*ddata->image->y_resolution*WMF_FONT_HEIGHT(font)))/TWIPS_INCH);
  /*   printf("========= Pointsize    : %i\n", (int)pointsize); */
  wmf_stream_printf (API,out,"font-size %i\n", pointsize);

  /* Translate coordinates so target is 0,0 */
  {
    magickPoint
      pt;

    pt = magick_translate (API,draw_text->pt);
    wmf_stream_printf (API,out,"translate %f,%f\n",pt.x,pt.y);
  }

  /* Apply rotation */
  /* ImageMagick's drawing rotation is clockwise from horizontal
     while WMF drawing rotation is counterclockwise from horizontal */
  /* FIXME: rotation is backwards */
  angle = abs(RadiansToDegrees(2*MagickPI-WMF_TEXT_ANGLE(font)));
  if(angle == 360)
    angle = 0;
  if (angle != 0)
    wmf_stream_printf (API,out,"rotate %f\n",angle);
          
  /*
   * Render text
   *
   */

  /* Build escaped string */
  for( p=draw_text->str, q=buff, string_length=0;
       *p!=0 && string_length < (sizeof(buff)-3);
       ++p )
    {
      if(*p == '\'')
        {
          *q++='\\';
          *q++='\\';
          string_length += 2;
        }
      else
        {
          *q++=*p;
          ++string_length;
        }
    }
  *q=0;
  wmf_stream_printf (API,out,"text 0,0 '%.1024s'\n",buff);

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_udata_init (wmfAPI* API,
                                   wmfUserData_t* userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"wmf_[magick_]udata_init");

}

static void wmf_magick_udata_copy (wmfAPI* API,
                                   wmfUserData_t* userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"wmf_[magick_]udata_copy");

}

static void wmf_magick_udata_set (wmfAPI* API,
                                  wmfUserData_t* userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"wmf_[magick_]udata_set");

}

static void wmf_magick_udata_free (wmfAPI* API,
                                   wmfUserData_t* userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"wmf_[magick_]udata_free");

}

static void magick_brush (wmfAPI* API,wmfDC* dc)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfRGB* bg_color = 0;
  wmfRGB* brush_color = 0;

  wmfBMP* brush_bmp = 0;

  wmfBrush* brush = 0;

  unsigned int fill_opaque;
  unsigned int fill_polyfill;
  unsigned int fill_ROP;

  unsigned int brush_style;
  unsigned int brush_hatch;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~magick_brush");

  if (out == 0) return;

  fill_opaque   = (unsigned int) WMF_DC_OPAQUE (dc);
  fill_polyfill = (unsigned int) WMF_DC_POLYFILL (dc);
  fill_ROP      = (unsigned int) WMF_DC_ROP (dc);

  bg_color = WMF_DC_BACKGROUND (dc);

  brush = WMF_DC_BRUSH (dc);

  brush_style = (unsigned int) WMF_BRUSH_STYLE (brush);
  brush_hatch = (unsigned int) WMF_BRUSH_HATCH (brush);

  brush_color = WMF_BRUSH_COLOR (brush);

  brush_bmp = WMF_BRUSH_BITMAP (brush);

  if (brush_style == BS_NULL)
    {
      wmf_stream_printf (API,out,"fill none\n");
      return;
    }

  if (fill_opaque)
    wmf_stream_printf (API,out,"fill-opacity 1.0\n");
  else
    wmf_stream_printf (API,out,"fill-opacity 0.5\n"); /* semi-transparent?? */

  switch (fill_polyfill) /* Is this correct ?? */
    {
    case WINDING:
      wmf_stream_printf (API,out,"fill-rule nonzero\n");
      break;

    case ALTERNATE:
    default:
      wmf_stream_printf (API,out,"fill-rule evenodd\n");
      break;
    }

  switch (brush_style)
    {
#ifdef WRITE_EPS_NOT_MAGICK
      /* float side; */

    case BS_HATCHED:
      wmf_stream_printf (API,out,"clip ");

      if (dc->bgmode != TRANSPARENT)
        {
          rgb = dc->bgcolor;

          red   = (float) ((int) rgb.r) / 255;
          green = (float) ((int) rgb.g) / 255;
          blue  = (float) ((int) rgb.b) / 255;

          wmf_stream_printf (API,out,"%f %f %f setrgbcolor ",red,green,blue);

          wmf_stream_printf (API,out,"fill ");
        }

      wmf_stream_printf (API,out,"\n");
      wmf_stream_printf (API,out,"1 setlinewidth ");
      wmf_stream_printf (API,out,"[] 0 setdash ");

      rgb = brush->lbColor;

      red   = (float) ((int) rgb.r) / 255;
      green = (float) ((int) rgb.g) / 255;
      blue  = (float) ((int) rgb.b) / 255;

      wmf_stream_printf (API,out,"%f %f %f setrgbcolor\n",red,green,blue);

      switch (brush->lbHatch)
        {
        case HS_HORIZONTAL:
          wmf_stream_printf (API,out,"%f 5 %f { newpath dup %f exch moveto %f exch lineto stroke } for\n",
                             bbox->TL.y,bbox->BR.y,bbox->TL.x,bbox->BR.x);
          break;

        case HS_VERTICAL:
          wmf_stream_printf (API,out,"%f 5 %f { newpath dup %f moveto %f lineto stroke } for\n",
                             bbox->TL.x,bbox->BR.x,bbox->TL.y,bbox->BR.y);
          break;

        case HS_FDIAGONAL:
          wmf_stream_printf (API,out,"gsave %% HS_FDIAGONAL\n");
          wmf_stream_printf (API,out,"%f %f translate -45 rotate ",
                             bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
          side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
          wmf_stream_printf (API,out,"0 5 %f { newpath dup 0 moveto %f lineto stroke } for ",
                             side,side);
          wmf_stream_printf (API,out,"grestore\n");
          break;

        case HS_BDIAGONAL:
          wmf_stream_printf (API,out,"gsave %% HS_BDIAGONAL\n");
          wmf_stream_printf (API,out,"%f %f translate -45 rotate ",
                             bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
          side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
          wmf_stream_printf (API,out,"0 5 %f { newpath dup 0 exch moveto %f exch lineto stroke } for ",
                             side,side);
          wmf_stream_printf (API,out,"grestore\n");
          break;

        case HS_CROSS:
          wmf_stream_printf (API,out,"%f 5 %f { newpath dup %f exch moveto %f exch lineto stroke } for\n",
                             bbox->TL.y,bbox->BR.y,bbox->TL.x,bbox->BR.x);
          wmf_stream_printf (API,out,"%f 5 %f { newpath dup %f moveto %f lineto stroke } for\n",
                             bbox->TL.x,bbox->BR.x,bbox->TL.y,bbox->BR.y);
          break;

        case HS_DIAGCROSS:
          wmf_stream_printf (API,out,"gsave %% HS_DIAGCROSS\n");
          wmf_stream_printf (API,out,"%f %f translate -45 rotate ",
                             bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
          side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
          wmf_stream_printf (API,out,"0 5 %f { newpath dup 0 moveto %f lineto stroke } for ",
                             side,side);
          wmf_stream_printf (API,out,"0 5 %f { newpath dup 0 exch moveto %f exch lineto stroke } for ",
                             side,side);
          wmf_stream_printf (API,out,"grestore\n");
          break;

        default:
          if (API->flags & WMF_OPT_IGNORE_NONFATAL)
            WMF_DEBUG (API,"Unsupported brush/hatch style!");
          else
            {
              WMF_ERROR (API,"Unsupported brush/hatch style!");
              API->err = wmf_E_Glitch;
            }
          break;
        }
      break;
#endif /* WRITE_EPS_NOT_MAGICK */

    case BS_DIBPATTERN:
      if (brush_bmp->data == 0)
        {
          if (API->flags & WMF_OPT_IGNORE_NONFATAL)
            WMF_DEBUG (API,"Attempt to fill with non-existent pattern!");
          else
            {
              WMF_ERROR (API,"Attempt to fill with non-existent pattern!");
              API->err = wmf_E_Glitch;
              break;
            }
        }
      /* no break here - TODO: implement bitmap fill */
    default:
      if (API->flags & WMF_OPT_IGNORE_NONFATAL)
        {
          WMF_DEBUG (API,"Unsupported brush style!");
          /* no break here */
        }
      else
        {
          WMF_ERROR (API,"Unsupported brush style!");
          API->err = wmf_E_Glitch;
          break;
        }
    case BS_SOLID:
      break;
    }

  wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",
                     (int)brush_color->r,
                     (int)brush_color->g,
                     (int)brush_color->b);
}

static void magick_pen (wmfAPI* API, wmfDC* dc)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfPen* pen = 0;

  wmfRGB* pen_color = 0;

  double pen_width;

  unsigned int pen_style;
  unsigned int pen_endcap;
  unsigned int pen_join;
  unsigned int pen_type;

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~magick_pen");

  if (out == 0) return;

  pen = WMF_DC_PEN (dc);

  pen_color = WMF_PEN_COLOR (pen);

  pen_width = ( magick_width  (API, WMF_PEN_WIDTH  (pen)) +
                magick_height (API, WMF_PEN_HEIGHT (pen)) ) / 2;

  pen_style  = (unsigned int) WMF_PEN_STYLE (pen);
  pen_endcap = (unsigned int) WMF_PEN_ENDCAP (pen);
  pen_join   = (unsigned int) WMF_PEN_JOIN (pen);
  pen_type   = (unsigned int) WMF_PEN_TYPE (pen);

  if (pen_style == PS_NULL)
    {
      wmf_stream_printf (API,out,"stroke none\n");
      return;
    }

  wmf_stream_printf (API,out,"stroke-width %f\n", max(0,pen_width));

  switch (pen_endcap)
    {
    case PS_ENDCAP_SQUARE:
      wmf_stream_printf (API,out,"stroke-linecap square\n");
      break;

    case PS_ENDCAP_ROUND:
      wmf_stream_printf (API,out,"stroke-linecap round\n");
      break;

    case PS_ENDCAP_FLAT:
    default:
      wmf_stream_printf (API,out,"stroke-linecap butt\n");
      break;
    }

  switch (pen_join)
    {
    case PS_JOIN_BEVEL:
      wmf_stream_printf (API,out,"stroke-linejoin bevel\n");
      break;

    case PS_JOIN_ROUND:
      wmf_stream_printf (API,out,"stroke-linejoin round\n");
      break;

    case PS_JOIN_MITER:
    default:
      wmf_stream_printf (API,out,"stroke-linejoin miter\n");
      break;
    }

  switch (pen_style)
    {
    case PS_DASH: /* DASH_LINE */
      wmf_stream_printf (API,out,"stroke-dasharray %f,%f\n",
		         pen_width*10,pen_width*10);
      break;

    case PS_ALTERNATE:
    case PS_DOT: /* DOTTED_LINE */
      wmf_stream_printf (API,out,"stroke-dasharray %f,%f\n",
		         pen_width,pen_width*2);
      break;

    case PS_DASHDOT: /* DASH_DOT_LINE */
      wmf_stream_printf (API,out,"stroke-dasharray %f,%f,%f,%f\n",
		         pen_width*10,pen_width*2,pen_width,pen_width*2);
      break;

    case PS_DASHDOTDOT: /* DASH_2_DOTS_LINE */
      wmf_stream_printf (API,out,"stroke-dasharray %f,%f,%f,%f,%f,%f\n",
		         pen_width*10,pen_width*2,pen_width,pen_width*2,
                         pen_width,pen_width*2);
      break;

    case PS_INSIDEFRAME: /* There is nothing to do in this case... */
    case PS_SOLID:
    default:
      wmf_stream_printf (API,out,"stroke-dasharray none\n");
      break;
    }

  wmf_stream_printf (API,out,"stroke #%02x%02x%02x\n",
                     (int)pen_color->r,(int)pen_color->g,(int)pen_color->b);
}

/* Scribble MVG on image */
static void ScribbleMVG(Image* image, const char* mvg)
{
  DrawInfo*
    draw_info;

  ImageInfo*
    image_info;

  image_info=(ImageInfo*)AcquireMemory(sizeof(ImageInfo));
  GetImageInfo(image_info);
  draw_info = (DrawInfo*)AcquireMemory(sizeof(DrawInfo));
  GetDrawInfo( image_info, draw_info );
  draw_info->primitive=(char*)mvg;
/*   puts(draw_info->primitive); */
  DrawImage(image,draw_info);
  draw_info->primitive = (char*)NULL;
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(image_info);
}

static Image *ReadWMFImage(const ImageInfo *image_info,
                           ExceptionInfo *exception)
{
  Image*
    image;

  ImageInfo*
    clone_info;

  char
    buff[MaxTextExtent],
    font_map_path[MaxTextExtent],
    *mvg;

  float
    wmf_width,
    wmf_height;

  double
    y_resolution = 72.0,
    x_resolution = 72.0;

  unsigned long
    mvg_length,
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

  image=AllocateImage(image_info);
  GetExceptionInfo(exception);

  /*
   * Create WMF API
   *
   */

  /* Register callbacks */
  wmf_options_flags |= WMF_OPT_FUNCTION;
  memset(&wmf_api_options,0,sizeof(wmf_api_options));
  wmf_api_options.function = wmf_magick_function;

  /* Ignore non-fatal errors */
  wmf_options_flags |= WMF_OPT_IGNORE_NONFATAL;

  /* Use ImageMagick's font map file */
  {
    char *p = GetMagickConfigurePath(TypeFilename);
    if(p!=NULL)
      {
        strcpy(font_map_path, p);
        wmf_options_flags |= WMF_OPT_XTRA_FONTS;
        wmf_options_flags |= WMF_OPT_XTRA_FONTMAP;
        wmf_api_options.xtra_fontmap_file = font_map_path;
      }
  }
  wmf_error = wmf_api_create (&API,wmf_options_flags,&wmf_api_options);
  if ( wmf_error != wmf_E_None )
    {
      if (API)
        wmf_api_destroy (API);
      ThrowReaderException(DelegateError,"Failed to intialize libwmf",image);
    }

  /* Open WMF file */
  wmf_error = wmf_file_open (API, (char*)image_info->filename);
  if ( wmf_error != wmf_E_None )
    {
      wmf_api_destroy (API);
      ThrowReaderException(FileOpenError,"Unable to open file",image);
    }

  /* Scan WMF file */
  wmf_error = wmf_scan (API,0,&bounding_box);
  if ( wmf_error != wmf_E_None )
    {
      wmf_api_destroy (API);
      ThrowReaderException(CorruptImageError,"Failed to scan file",image);
    }
  /*   printf("Bounding Box: %f,%f %f,%f\n", bounding_box.TL.x, bounding_box.TL.y, */
  /*          bounding_box.BR.x,bounding_box.BR.y); */

  /* Compute output width and height */

  /* A twip (meaning "twentieth of a point") is the logical unit of
     measurement used in Windows Metafiles. A twip is equal to 1/1440
     of an inch. Thus 720 twips equal 1/2 inch, while 32,768 twips is
     22.75 inches.

     The units returned by wmf_size are in centimeters
  */
  wmf_error = wmf_size (API,&wmf_width,&wmf_height);
  if ( wmf_error != wmf_E_None )
    {
      wmf_api_destroy (API);
      ThrowReaderException(CorruptImageError,"Failed to compute output size",
                           image);
    }
  /*   printf("wmf_size: %fx%f\n", wmf_width, wmf_height); */

  if(image->y_resolution > 0)
    {
      y_resolution = image->y_resolution;
      if ( image->units == PixelsPerCentimeterResolution )
        y_resolution *= CENTIMETERS_INCH;
    }
  
  if(image->x_resolution > 0)
    {
      x_resolution = image->x_resolution;
      if ( image->units == PixelsPerCentimeterResolution )
        x_resolution *= CENTIMETERS_INCH;
    }

  wmf_width  = ceil(( wmf_width*CENTIMETERS_INCH)/TWIPS_INCH*x_resolution);
  wmf_height = ceil((wmf_height*CENTIMETERS_INCH)/TWIPS_INCH*y_resolution);

  /*   printf("Size: %fx%f\n", wmf_width, wmf_height); */

  ddata = WMF_MAGICK_GetData (API);
  ddata->bbox = bounding_box;

  /* Create canvas image */
  clone_info = (ImageInfo*)AcquireMemory(sizeof(ImageInfo));
  GetImageInfo( clone_info );
  sprintf( buff, "%ix%i", (int)wmf_width, (int)wmf_height );
  (void) CloneString(&(clone_info->size), buff);
  if(image_info->texture == (char*)NULL)
    sprintf(clone_info->filename,
#if QuantumDepth == 8
            "XC:#%02x%02x%02x%02x",
#elif QuantumDepth == 16
            "XC:#%04x%04x%04x%04x",
#endif    
            image_info->background_color.red,
            image_info->background_color.green,
            image_info->background_color.blue,
            image_info->background_color.opacity);
  else
    sprintf(clone_info->filename,"TILE:%.1024s",image_info->texture);
  GetExceptionInfo(exception);
  DestroyImage(image);
  image=ReadImage( clone_info, exception );
  DestroyImageInfo(clone_info);
  if ( image == (Image*)NULL )
    {
      wmf_api_destroy (API);
      return image;
    }
  strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  strncpy(image->magick,image_info->magick,MaxTextExtent-1);
  ddata->image = image;
  ddata->image->x_resolution = x_resolution;
  ddata->image->y_resolution = y_resolution;
  ddata->image->units = PixelsPerInchResolution;

  /* Create MVG drawing commands*/
  ddata->out = wmf_stream_create (API,0);
  wmf_error = wmf_play (API,0,&bounding_box);
  if ( wmf_error != wmf_E_None )
    {
      wmf_api_destroy (API);
      ThrowReaderException(CorruptImageError,"Failed to render file",image);
    }
  wmf_stream_destroy(API,
                     ddata->out,	/* Stream */
                     &mvg,		/* MVG data */
                     &mvg_length);	/* MVG length */

  /* Scribble on canvas image */
  ScribbleMVG(image,mvg);

  /* Cleanup allocated data */
  wmf_api_destroy(API);

  /* Return image */
  return image;
}
#else
// Aldus Placeable Header
#pragma pack( push )
#pragma pack( 2 )
typedef struct
{
	DWORD		dwKey;
	WORD		hmf;
	SMALL_RECT	bbox;
	WORD		wInch;
	DWORD		dwReserved;
	WORD		wCheckSum;
} APMHEADER, *PAPMHEADER;
#pragma pack( pop )
/*
  FUNCTION   : ReadEnhMetaFile

  PARAMETERS : LPTSTR  szFileName  -  Filename for the metafile to open

  PURPOSE    : Read a metafile as an enhanced metafile

  RETURNS    : HENHMETAFILE - the handle of the new enhanced metafile

  COMMENTS   : This function will read either an enhanced metafile, or a
               regular 16bit windows metafile, or an Aldus Placeable metafile
               and convert it into an enhanced metafile.
               This function first tries to load the metafile as an enhanced
               metafile. If that fails, it tries to load it as a 16bit windows
               metafile. If that fails, it tries to load it as a 16bit Aldus
               Placeable metafile. If all fail, NULL is returned.
*/
static HENHMETAFILE ReadEnhMetaFile( const char *szFileName,
  int *width, int *height )
{
	HENHMETAFILE hTemp;
	HMETAFILE hOld;
	DWORD dwSize;
	LPBYTE pBits;
	METAFILEPICT mp;
	HDC hDC;
	HANDLE hFile;

	// First try to read it as an enhanced metafile
	// If it works, simply return the handle
	if( (hTemp = GetEnhMetaFile( szFileName )) != NULL )
    {
      ENHMETAHEADER emfh;

      GetEnhMetaFileHeader(hTemp, sizeof(ENHMETAHEADER), &emfh);
      // picture size in 0.01 mm
      *width  = emfh.rclFrame.right  - emfh.rclFrame.left;
      *height = emfh.rclFrame.bottom - emfh.rclFrame.top;
		  return hTemp;
    }
	// It was apparently not an enhanced metafile, so try 16bit windows metafile
	if( (hOld = GetMetaFile( szFileName )) != NULL )
	  {
		  // Ok, it is a 16bit windows metafile
		  // How big are the bits?
		  if( (dwSize = GetMetaFileBitsEx( hOld, 0, NULL )) == 0 )
		    {
			    DeleteMetaFile( hOld );
			    return NULL;
		    }
		  // Allocate that much memory
		  if( (pBits = malloc( dwSize )) == NULL )
		    {
			    DeleteMetaFile( hOld );
			    return NULL;
		    }
		  // Get the metafile bits
		  if( GetMetaFileBitsEx( hOld, dwSize, pBits ) == 0 )
		    {
			    free( pBits );
			    DeleteMetaFile( hOld );
			    return NULL;
		    }
		  // Fill out a METAFILEPICT structure
		  mp.mm = MM_ANISOTROPIC;
		  mp.xExt = 1000;
		  mp.yExt = 1000;
		  mp.hMF = NULL;
		  // Get a reference DC
		  hDC = GetDC( NULL );
		  // Make an enhanced metafile from the windows metafile
		  hTemp = SetWinMetaFileBits( dwSize, pBits, hDC, &mp );
		  // Clean up
		  ReleaseDC( NULL, hDC );
		  DeleteMetaFile( hOld );
		  free( pBits );
      {
        ENHMETAHEADER emfh;

        GetEnhMetaFileHeader(hTemp, sizeof(ENHMETAHEADER), &emfh);
        // picture size in 0.01 mm
        *width  = emfh.rclFrame.right  - emfh.rclFrame.left;
        *height = emfh.rclFrame.bottom - emfh.rclFrame.top;
      }
		  return hTemp;
	  }
	// If we got here, the file of interest is neither an enhanced metafile nor a
	// 16bit windows metafile. Let's assume it's a placeable metafile and move on.
	// First, open the file for reading
	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return NULL;
	// How big is the file?
	dwSize = GetFileSize( hFile, NULL );
	// Allocate enough memory to hold it
	pBits = malloc( dwSize );
	// Read it in
	ReadFile( hFile, pBits, dwSize, &dwSize, NULL );
	// Close the file
	CloseHandle( hFile );
	// Is it a placeable metafile? (check the key)
	if( ((PAPMHEADER)pBits)->dwKey != 0x9ac6cdd7l )
	  {
		  // Not a metafile that we know how to recognise - bail out
		  free( pBits );
		  return NULL;
	  }
	// Ok, its a placeable metafile
	// Fill out a METAFILEPICT structure
	mp.mm = MM_ANISOTROPIC;
	mp.xExt = ((PAPMHEADER)pBits)->bbox.Right - ((PAPMHEADER)pBits)->bbox.Left;
  *width = mp.xExt;
	mp.xExt = ( mp.xExt * 2540l ) / (DWORD)(((PAPMHEADER)pBits)->wInch);
	mp.yExt = ((PAPMHEADER)pBits)->bbox.Bottom - ((PAPMHEADER)pBits)->bbox.Top;
  *height = mp.yExt;
	mp.yExt = ( mp.yExt * 2540l ) / (DWORD)(((PAPMHEADER)pBits)->wInch);
	mp.hMF = NULL;
	// Get a reference DC
	hDC = GetDC( NULL );
	// Create an enhanced metafile from the bits
	hTemp = SetWinMetaFileBits( dwSize, &(pBits[sizeof(APMHEADER)]), hDC, &mp );
	// Clean up
	ReleaseDC( NULL, hDC );
	free( pBits );
	return hTemp;
}

static Image *ReadWMFImage(const ImageInfo *image_info,
                           ExceptionInfo *exception)
{
  Image
    *image;

  PixelPacket
    *q;

  HDC
    hDC;

  HBITMAP
    hBitmap,
    hOldBitmap;

  HENHMETAFILE
    hemf;

  BITMAPINFO
    DIBinfo;

  RGBQUAD
    *ppBits;

  int
    height,
    width;

  RECT
    rect;

  image=AllocateImage(image_info);

  width=height=512;
  hemf = ReadEnhMetaFile(image_info->filename, &width, &height);
  if (!hemf)
  {
    ThrowReaderException(FatalException,"file is not a metafile",image);
  }
  image->rows=height;
  image->columns=width;
  if (image_info->size != (char *) NULL)
    {
      int
        x,
        y;

      unsigned int
        flags;

      x=y=0;
      flags=ParseImageGeometry(image_info->size,&x,&y,
        &image->columns,&image->rows);
    }
  if (image_info->page != (char *) NULL)
    {
      char
        *geometry,
        *p;

      int
        x,
        y;

      unsigned int
        flags;

      x=y=0;
      geometry=PostscriptGeometry(image_info->page);
      p=strchr(geometry,'>');
      if (!p)
        {
          flags=ParseImageGeometry(geometry,&x,&y,
            &image->columns,&image->rows);
          if (image->x_resolution != 0.0)
            image->columns=(unsigned int)
              ((image->columns*image->x_resolution)+0.5);
          if (image->y_resolution != 0.0)
            image->rows=(unsigned int)
              ((image->rows*image->y_resolution)+0.5);
        }
      else
        {
          *p='\0'; /* must zap the format control */
          flags=ParseImageGeometry(geometry,&x,&y,
            &image->columns,&image->rows);
          if (image->x_resolution != 0.0)
            image->columns=(unsigned int)
              (((image->columns*image->x_resolution)/72.0)+0.5);
          if (image->y_resolution != 0.0)
            image->rows=(unsigned int)
              (((image->rows*image->y_resolution)/72.0)+0.5);
        }
      LiberateMemory((void **) &geometry);
    }
  // Create a DC which will be used to get DIB
  hDC = GetDC(NULL);
  if (!hDC) 
  {
    ThrowReaderException(FatalException,"failed to create a DC",image);
  }
	// Initialize the bitmap header info.
	memset(&DIBinfo, 0, sizeof(BITMAPINFO));
	DIBinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	DIBinfo.bmiHeader.biWidth = image->columns;
	DIBinfo.bmiHeader.biHeight = (-1)*(image->rows);
	DIBinfo.bmiHeader.biPlanes = 1;
	DIBinfo.bmiHeader.biBitCount = 32;
	DIBinfo.bmiHeader.biCompression	= BI_RGB;
	//DIBinfo.bmiHeader.biSizeImage = image->columns * image->rows * 3;
  hBitmap = CreateDIBSection(hDC, &DIBinfo, DIB_RGB_COLORS, (void**)&ppBits, NULL, 0);
  ReleaseDC(NULL, hDC);
  if (!hBitmap)
  {
    ThrowReaderException(FatalException,"failed to create bitmap",image);
  }
  // Create a DC which will be used to get DIB, then create DIBsection
  hDC = CreateCompatibleDC(NULL);
  if (!hDC) 
  {
    DeleteObject(hBitmap);
    ThrowReaderException(FatalException,"failed to create a memory DC",image);
  }
  hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
  if (!hOldBitmap)
  {
		DeleteDC(hDC);
    DeleteObject(hBitmap);
    ThrowReaderException(FatalException,"failed to create bitmap",image);
  }
  (void) QueryColorDatabase((char *) "white",&image->background_color);
  SetImage(image,OpaqueOpacity);
  q = GetImagePixels(image,0,0,image->columns,image->rows);
  if (q != (PixelPacket *) NULL)
    {
      RGBQUAD
        *pBits;

      unsigned long
        nPixels;

      nPixels = image->columns * image->rows;
      pBits = ppBits;
#if QuantumDepth == 8
      // Form of PixelPacket is identical to RGBQUAD when QuantumDepth==8
      memcpy((void *)pBits,(const void *)q,sizeof(PixelPacket)*nPixels);
#elif QuantumDepth == 16
      // Transfer pixels, scaling to Quantum
      for( unsigned long nPixelCount = nPixels; nPixelCount ; nPixelCount-- )
        {
          pBits->rgbRed = DownScale(q->red);
          pBits->rgbGreen = DownScale(q->green);
          pBits->rgbBlue = DownScale(q->blue);
          pBits->rgbReserved = DownScale(q->opacity);
          ++pBits;
          ++q;
        }
#endif
    }
  rect.top=0;
  rect.left=0;
  rect.right=image->columns;
  rect.bottom=image->rows;
  PlayEnhMetaFile(hDC, hemf, &rect);
  if (q != (PixelPacket *) NULL)
    {
      RGBQUAD
        *pBits;

      unsigned long
        nPixels;

      nPixels = image->columns * image->rows;
      pBits = ppBits;
#if QuantumDepth == 8
      // Form of PixelPacket is identical to RGBQUAD when QuantumDepth==8
      memcpy((void *)q,(const void *)pBits,sizeof(PixelPacket)*nPixels);
#elif QuantumDepth == 16
      // Transfer pixels, scaling to Quantum
      for( unsigned long nPixelCount = nPixels; nPixelCount ; nPixelCount-- )
        {
          UpScale(q->red) = pBits->rgbRed;
          UpScale(q->green) = pBits->rgbGreen;
          UpScale(q->blue)= pBits->rgbBlue;
          q->opacity = 0;
          ++pBits;
          ++q;
        }
#endif
    }
  if (!SyncImagePixels(image))
    ThrowReaderException(FatalException,"sync on pixel buffer failed",image);

  if (hemf)
    DeleteEnhMetaFile(hemf);
	SelectObject(hDC, hOldBitmap);
	DeleteDC(hDC);
  DeleteObject(hBitmap);
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

  entry=SetMagickInfo("WMF");
  entry->decoder=ReadWMFImage;
  entry->description=AllocateString("Windows Meta File");
  entry->blob_support=False;
  entry->module=AllocateString("WMF");
  (void) RegisterMagickInfo(entry);
#if defined(WIN32)
  entry=SetMagickInfo("EMF");
  entry->decoder=ReadWMFImage;
  entry->description=AllocateString("Windows Meta File");
  entry->blob_support=False;
  entry->module=AllocateString("WMF");
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
