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

  Should write replacements for:
  WMF_FONT_PSNAME
  wmf_api_create
  wmf_api_destroy
  wmf_ipa_font_lookup
  wmf_stream_create
  wmf_stream_destroy
  wmf_stream_printf
  
 */

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

#if defined(HasWMF)
#if !defined(WIN32)

#define WMF_USE_NATIVE_READ 1

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
    scale_x,
    scale_y,
    translate_x,
    translate_y,
    units_per_inch;

  /* Output stream */
  wmfStream
    *out;

  /* ImageMagick image */
  Image
    *image;

  /* Maximum and current number of temporary files */
  int
    max_temp_file_index,
    cur_temp_file_index;

  /* Temporary image IDs */
#if WMF_USE_NATIVE_READ
  long
    *temp_images;
#else
  char
    **temp_images;
#endif /* WMF_USE_NATIVE_READ */
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

/*
  TODO ?? Care about bmp_draw->type
 */
#if 0
/* passed as argument to wmf_magick_bmp_read */
struct _wmfBMP_Read_t    /* Two means available for accessing BMP image:        */
{
  long offset;           /* (1) position in source file of start of BMP;        *
                          * use API->bbuf.seek to set pos(ition), etc.          */
  long length;           /* (2) buffer of length length containing image of BMP */
  unsigned char* buffer;

  U16 width;  /* WMF player may preset these values; zero otherwise. */
  U16 height; /* Use caution - may be buggy... ?? [TODO]             */

  wmfBMP bmp;
};

/* passed as argument to wmf_magick_bmp_draw */
struct _wmfBMP_Draw_t
{
  wmfDC* dc;

  wmfD_Coord pt;
  wmfBMP bmp;

  U32 type;

  struct
  {
    U16 x;
    U16 y;
    U16 w;
    U16 h;
  } crop;

  double pixel_width;
  double pixel_height;
};

/* passed as argument to wmf_magick_bmp_free */
struct _wmfBMP
{
  U16 width;
  U16 height;
  
  void* data;
};

#endif /* out */
static void wmf_magick_bmp_draw (wmfAPI* API,wmfBMP_Draw_t* bmp_draw)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  double
    height,
    width,
    x,
    y;

  char
    *imgspec;

  wmfStream
    *out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]bmp_draw");

  if (out == 0) return;

  if (bmp_draw->bmp.data == 0) return;

  imgspec = (char*)AcquireMemory(MaxTextExtent*sizeof(char));
  *imgspec = '\0';

#if WMF_USE_NATIVE_READ
  {
    ExceptionInfo
      exception;
    long
      id;

    GetExceptionInfo(&exception);
    id = SetMagickRegistry(ImageRegistryType,bmp_draw->bmp.data,sizeof(Image),&exception);
  (ddata->temp_images)[ddata->cur_temp_file_index] = id;
  ++ddata->cur_temp_file_index;
  if(ddata->cur_temp_file_index == ddata->max_temp_file_index)
    {
      ddata->max_temp_file_index += 2048;
      ReacquireMemory((void **) &ddata->temp_images, ddata->max_temp_file_index*sizeof(long));
    }
  sprintf(imgspec,"mpr:%li", id);
  }
#else
  TemporaryFilename(imgspec);
  (ddata->temp_images)[ddata->cur_temp_file_index] = imgspec;
  ++ddata->cur_temp_file_index;
  if(ddata->cur_temp_file_index == ddata->max_temp_file_index)
    {
      ddata->max_temp_file_index += 2048;
      ReacquireMemory((void **) &ddata->temp_images, ddata->max_temp_file_index*sizeof(char *));
    }

  wmf_ipa_bmp_png (API,bmp_draw,imgspec);

  if (ERR (API))
    {	WMF_DEBUG (API,"bailing...");
    return;
    }
#endif /* WMF_USE_NATIVE_READ */

  /* Okay, if we've got this far then "imgspec" is the filename of an png
     (cropped) image */

  width  = abs(bmp_draw->pixel_width*(double)bmp_draw->crop.w);
  height = abs(bmp_draw->pixel_height*(double)bmp_draw->crop.h) - 0.0000022;
  x=bmp_draw->pt.x;
  y=bmp_draw->pt.y;

/*   printf("x=%.10g, y=%.10g, width=%.10g, height=%.10g\n", x,y,width,height); */
  wmf_stream_printf (API,out,"image Copy %.10g,%.10g %.10g,%.10g '%s'\n",
                     x,y,width,height,imgspec);
}

#if 0
/* passed as argument to wmf_magick_bmp_read */
struct _wmfBMP_Read_t    /* Two means available for accessing BMP image:        */
{
  long offset;           /* (1) position in source file of start of BMP;        *
                          * use API->bbuf.seek to set pos(ition), etc.          */
  long length;           /* (2) buffer of length length containing image of BMP */
  unsigned char* buffer;

  U16 width;  /* WMF player may preset these values; zero otherwise. */
  U16 height; /* Use caution - may be buggy... ?? [TODO]             */

  wmfBMP bmp;
};

(gdb) p *bmp_read
$23 = {
  offset = 96, 
  length = 99400, 
  buffer = 0x2fac4e "(", 
  width = 0, 
  height = 0, 
  bmp = {
    width = 61439, 
    height = 48296, 
    data = 0x0
  }
}

#endif /* out */
static void wmf_magick_bmp_read (wmfAPI* API,wmfBMP_Read_t* bmp_read)
{
  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]bmp_read");

#if WMF_USE_NATIVE_READ

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  Image
    *image;

  image_info=CloneImageInfo((ImageInfo *)0);
  strcpy(image_info->magick,"DIB");
  GetExceptionInfo( &exception );
/* printf("offset=%ld, length=%ld, width=%i, height=%i\n", */
/*        bmp_read->offset,bmp_read->length,bmp_read->width,bmp_read->height); */

  image = BlobToImage(image_info,(const void*)bmp_read->buffer,
                      bmp_read->length,&exception);
  DestroyImageInfo(image_info);
  if(!image)
    {
      char error_message[MaxTextExtent];
      snprintf(error_message,sizeof(error_message)-1,"%s (%s)",
               exception.reason, exception.description);
      bmp_read->bmp.data   = (void *)0;
/*       WMF_ERROR(API,error_message); */
      switch(exception.severity)
        {
        case ResourceLimitWarning:
        case ResourceLimitError:
          API->err = wmf_E_InsMem;
          break;
        case CorruptImageWarning:
        case CorruptImageError:
          API->err = wmf_E_BadFormat;
          break;
        default:
          {
            API->err = wmf_E_DeviceError;
          }
        }
    }
  else
    {
      /* printf("rows=%ld,columns=%ld\n", image->rows, image->columns); */
      bmp_read->bmp.data   = image;
/*       bmp_read->bmp.width  = (U16)image->columns; */
/*       bmp_read->bmp.height = (U16)image->rows; */
    }
#else
  wmf_ipa_bmp_read (API,bmp_read);
#endif
}

static void wmf_magick_bmp_free (wmfAPI* API,wmfBMP* bmp)
{
  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]bmp_free");

#if WMF_USE_NATIVE_READ
  DestroyImage((Image*)bmp->data);
  bmp->data   = (void*)0;
  bmp->width  = (U16)0;
  bmp->height = (U16)0;
#else
  wmf_ipa_bmp_free (API,bmp);
#endif
}


/*
  This is called by wmf_play() the *first* time the meta file is played
 */
static void wmf_magick_device_open (wmfAPI* API)
{
/*   wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_open");

	
}

/*
  This is called by wmf_api_destroy()
 */
static void wmf_magick_device_close (wmfAPI* API)
{
  int
    index;

  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_close");

  /* Remove any temporary files */
  if(ddata->temp_images != 0)
    {
      for( index=0; index<ddata->cur_temp_file_index; index++ )
        {
#if WMF_USE_NATIVE_READ
          DeleteMagickRegistry((ddata->temp_images)[index]);
#else
          remove((ddata->temp_images)[index]);
          LiberateMemory((void**)&(ddata->temp_images)[index]);
#endif /* WMF_USE_NATIVE_READ */
        }
      LiberateMemory((void**)&ddata->temp_images);
    }
}

/*
  This is called from the beginning of each play for initial page setup
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
#if WMF_USE_NATIVE_READ
  ddata->temp_images = (long*)AcquireMemory(ddata->max_temp_file_index*sizeof(long));
#else
  ddata->temp_images = (char**)AcquireMemory(ddata->max_temp_file_index*sizeof(char *));
#endif /* WMF_USE_NATIVE_READ */

  wmf_stream_printf (API,out,"viewbox 0 0 %u %u\n",ddata->image->columns,ddata->image->rows);

  /* Make SVG output happy */
  wmf_stream_printf (API,out,"push graphic-context\n");

  /* Scale width and height to image */
  wmf_stream_printf (API,out,"scale %.10g,%.10g\n",
                     ddata->scale_x,
                     ddata->scale_y);
  /* Translate to TL corner of bounding box */
  wmf_stream_printf (API,out,"translate %.10g,%.10g\n",
                     ddata->translate_x,
                     ddata->translate_y);
  wmf_stream_printf (API,out,"fill none\n");
  wmf_stream_printf (API,out,"stroke none\n");
}

/*
  This is called from the end of each play for page termination
 */
static void wmf_magick_device_end (wmfAPI* API)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_end");

  if (out == 0) return;

  /* Make SVG output happy */
  wmf_stream_printf (API,out,"pop graphic-context\n");

}

static void wmf_magick_flood_interior (wmfAPI* API,wmfFlood_t* flood)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfRGB* rgb = &(flood->color);

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]flood_interior");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",
                     (int)rgb->r,(int)rgb->g,(int)rgb->b);

  wmf_stream_printf (API,out,"color %.10g,%.10g filltoborder\n",
                     flood->pt.x,flood->pt.y);

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_flood_exterior (wmfAPI* API,wmfFlood_t* flood)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfRGB* rgb = &(flood->color);

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]flood_exterior");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",
                     (int)rgb->r,(int)rgb->g,(int)rgb->b);

  if (flood->type == FLOODFILLSURFACE)
    wmf_stream_printf (API,out,"color %.10g,%.10g floodfill\n",flood->pt.x,flood->pt.y);
  else
    wmf_stream_printf (API,out,"color %.10g,%.10g filltoborder\n",flood->pt.x,flood->pt.y);

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_draw_pixel (wmfAPI* API,wmfDrawPixel_t* draw_pixel)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfRGB* rgb = &(draw_pixel->color);

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_pixel");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  wmf_stream_printf (API,out,"stroke none\n");

  wmf_stream_printf (API,out,"fill-opacity 1\n");

  wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",
                     (int)rgb->r,(int)rgb->g,(int)rgb->b);

  wmf_stream_printf (API,out,"rectangle %.10g,%.10g %.10g,%.10g\n",
                     draw_pixel->pt.x,draw_pixel->pt.y,
                     draw_pixel->pt.x+draw_pixel->pixel_width,
                     draw_pixel->pt.y+draw_pixel->pixel_height);

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

  wmfD_Coord TL;
  wmfD_Coord BR;

  wmfD_Coord O;

  wmfD_Coord start;
  wmfD_Coord end;

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

      TL = draw_arc->TL;
      BR = draw_arc->BR;

      O = centre;

      if (finish != magick_arc_ellipse)
        {
          start = draw_arc->start;
          end   = draw_arc->end;
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
	wmf_stream_printf (API,out,"ellipse %.10g,%.10g %.10g,%.10g 0,360\n",
                           O.x,O.y,Rx,Ry);
      else if (finish == magick_arc_pie)
	wmf_stream_printf (API,out,"ellipse %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                           O.x,O.y,Rx,Ry,phi_s,phi_e);
      else if (finish == magick_arc_chord)
        {
          wmf_stream_printf (API,out,"arc %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                             O.x,O.y,Rx,Ry,phi_s,phi_e);
          wmf_stream_printf (API,out,"line %.10g,%.10g %.10g,%.10g\n",
                             start.x,start.y,end.x,end.y);
        }
      else /* if (finish == magick_arc_open) */
        wmf_stream_printf (API,out,"arc %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                           O.x,O.y,Rx,Ry,phi_s,phi_e);
    }

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_draw_line (wmfAPI* API,
                                  wmfDrawLine_t* draw_line)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_line");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  if (TO_DRAW (draw_line))
    { 
      magick_pen (API,draw_line->dc);
      wmf_stream_printf (API,out,"line %.10g,%.10g %.10g,%.10g\n",
                         draw_line->from.x,draw_line->from.y,
                         draw_line->to.x,draw_line->to.y);
    }

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_poly_line (wmfAPI* API,wmfPolyLine_t* poly_line)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

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
          wmf_stream_printf (API,out," %.10g,%.10g",
                             poly_line->pt[i].x,poly_line->pt[i].y);
        }

      wmf_stream_printf (API,out,"\n");
    }

  /* Restore graphic context */
  wmf_stream_printf (API,out,"pop graphic-context\n");
}

static void wmf_magick_draw_polygon (wmfAPI* API,wmfPolyLine_t* poly_line)
{
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

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
          wmf_stream_printf (API,out," %.10g,%.10g",
                             poly_line->pt[i].x,poly_line->pt[i].y);
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

  wmfStream* out = ddata->out;

  WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_rectangle");

  if (out == 0) return;

  /* Save graphic context */
  wmf_stream_printf (API,out,"push graphic-context\n");

  if (TO_FILL (draw_rect) || TO_DRAW (draw_rect))
    {
      magick_brush (API,draw_rect->dc);
      magick_pen (API,draw_rect->dc);

      if ((draw_rect->width > 0) || (draw_rect->height > 0))
        wmf_stream_printf (API,out,"roundrectangle %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
                           draw_rect->TL.x,draw_rect->TL.y,
                           draw_rect->BR.x,draw_rect->BR.y,
                           draw_rect->width/2,draw_rect->height/2);
      else
	wmf_stream_printf (API,out,"rectangle %.10g,%.10g %.10g,%.10g\n",
                           draw_rect->TL.x,draw_rect->TL.y,
                           draw_rect->BR.x,draw_rect->BR.y);
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

  /*
    IPA function reference links
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

  /*
    Allocate device data structure
  */
  ddata = (wmf_magick_t*) wmf_malloc (API,sizeof (wmf_magick_t));

  if (ERR (API))
    {
      WMF_DEBUG (API,"bailing...");
      return;
    }

  API->device_data = (void*) ddata;

  /*
    Device data defaults
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
  font_path = wmf_ipa_font_lookup (API,font_name);
  wmf_stream_printf (API,out,"font '%s'\n",font_name);

  /* Set underline */
  if(WMF_TEXT_UNDERLINE(font))
    wmf_stream_printf (API,out,"decorate underline\n");

  /* Set strikeout */
  if(WMF_TEXT_STRIKEOUT(font))
    wmf_stream_printf (API,out,"decorate line-through\n");

  /* Set font size */
  pointsize = ceil(abs(WMF_FONT_HEIGHT(font)*draw_text->dc->pixel_height));
  
/*   printf("WMF_FONT_NAME:        = %s\n", WMF_FONT_NAME(font)); */
/*   printf("WMF_FONT_HEIGHT       = %i\n", (int)WMF_FONT_HEIGHT(font)); */
/*   printf("WMF_FONT_WIDTH        = %i\n", (int)WMF_FONT_WIDTH(font)); */
/*   printf("pointsize             = %i\n", (int)pointsize); */
/*   printf("scaled pointsize      = %.10g\n", (float)pointsize*ddata->scale_y); */
  wmf_stream_printf (API,out,"font-size %i\n", pointsize);

  /* Translate coordinates so target is 0,0 */
  wmf_stream_printf (API,out,"translate %.10g,%.10g\n",
                     draw_text->pt.x,draw_text->pt.y);
  
  /* Apply rotation */
  /* ImageMagick's drawing rotation is clockwise from horizontal
     while WMF drawing rotation is counterclockwise from horizontal */
  /* FIXME: rotation is backwards */
  angle = abs(RadiansToDegrees(2*MagickPI-WMF_TEXT_ANGLE(font)));
  if(angle == 360)
    angle = 0;
  if (angle != 0)
    wmf_stream_printf (API,out,"rotate %.10g\n",angle);
          
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

          wmf_stream_printf (API,out,"%.10g %.10g %.10g setrgbcolor ",red,green,blue);

          wmf_stream_printf (API,out,"fill ");
        }

      wmf_stream_printf (API,out,"\n");
      wmf_stream_printf (API,out,"1 setlinewidth ");
      wmf_stream_printf (API,out,"[] 0 setdash ");

      rgb = brush->lbColor;

      red   = (float) ((int) rgb.r) / 255;
      green = (float) ((int) rgb.g) / 255;
      blue  = (float) ((int) rgb.b) / 255;

      wmf_stream_printf (API,out,"%.10g %.10g %.10g setrgbcolor\n",red,green,blue);

      switch (brush->lbHatch)
        {
        case HS_HORIZONTAL:
          wmf_stream_printf (API,out,"%.10g 5 %.10g { newpath dup %.10g exch moveto %.10g exch lineto stroke } for\n",
                             bbox->TL.y,bbox->BR.y,bbox->TL.x,bbox->BR.x);
          break;

        case HS_VERTICAL:
          wmf_stream_printf (API,out,"%.10g 5 %.10g { newpath dup %.10g moveto %.10g lineto stroke } for\n",
                             bbox->TL.x,bbox->BR.x,bbox->TL.y,bbox->BR.y);
          break;

        case HS_FDIAGONAL:
          wmf_stream_printf (API,out,"gsave %% HS_FDIAGONAL\n");
          wmf_stream_printf (API,out,"%.10g %.10g translate -45 rotate ",
                             bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
          side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
          wmf_stream_printf (API,out,"0 5 %.10g { newpath dup 0 moveto %.10g lineto stroke } for ",
                             side,side);
          wmf_stream_printf (API,out,"grestore\n");
          break;

        case HS_BDIAGONAL:
          wmf_stream_printf (API,out,"gsave %% HS_BDIAGONAL\n");
          wmf_stream_printf (API,out,"%.10g %.10g translate -45 rotate ",
                             bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
          side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
          wmf_stream_printf (API,out,"0 5 %.10g { newpath dup 0 exch moveto %.10g exch lineto stroke } for ",
                             side,side);
          wmf_stream_printf (API,out,"grestore\n");
          break;

        case HS_CROSS:
          wmf_stream_printf (API,out,"%.10g 5 %.10g { newpath dup %.10g exch moveto %.10g exch lineto stroke } for\n",
                             bbox->TL.y,bbox->BR.y,bbox->TL.x,bbox->BR.x);
          wmf_stream_printf (API,out,"%.10g 5 %.10g { newpath dup %.10g moveto %.10g lineto stroke } for\n",
                             bbox->TL.x,bbox->BR.x,bbox->TL.y,bbox->BR.y);
          break;

        case HS_DIAGCROSS:
          wmf_stream_printf (API,out,"gsave %% HS_DIAGCROSS\n");
          wmf_stream_printf (API,out,"%.10g %.10g translate -45 rotate ",
                             bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
          side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
          wmf_stream_printf (API,out,"0 5 %.10g { newpath dup 0 moveto %.10g lineto stroke } for ",
                             side,side);
          wmf_stream_printf (API,out,"0 5 %.10g { newpath dup 0 exch moveto %.10g exch lineto stroke } for ",
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
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData (API);

  wmfPen
    *pen = 0;

  wmfRGB
    *pen_color = 0;

  wmfStream
    *out = ddata->out;

  double
    pen_width,
    pixel_width;

  unsigned int
    pen_endcap,
    pen_join,
    pen_style,
    pen_type;


  WMF_DEBUG (API,"~~~~~~~~magick_pen");

  if (out == 0) return;

  pen = WMF_DC_PEN (dc);

  pen_color = WMF_PEN_COLOR (pen);

  pen_width = (WMF_PEN_WIDTH(pen) +
               WMF_PEN_HEIGHT(pen))/2;

  /* Pixel width is inverse of pixel scale */
  pixel_width = (((double)1/(ddata->scale_x)) +
                 ((double)1/(ddata->scale_y)))/2;

  /* Don't allow pen_width to be less than pixel_width in order to
     avoid dissapearing or spider-web lines*/
  pen_width = max(pen_width,pixel_width);

  pen_style  = (unsigned int) WMF_PEN_STYLE (pen);
  pen_endcap = (unsigned int) WMF_PEN_ENDCAP (pen);
  pen_join   = (unsigned int) WMF_PEN_JOIN (pen);
  pen_type   = (unsigned int) WMF_PEN_TYPE (pen);

  if (pen_style == PS_NULL)
    {
      wmf_stream_printf (API,out,"stroke none\n");
      return;
    }

  wmf_stream_printf (API,out,"stroke-antialias 1\n");
  wmf_stream_printf (API,out,"stroke-width %.10g\n", max(0,pen_width));

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
    case PS_DASH: /* DASH_LINE, 18,7 */
      wmf_stream_printf (API,out,"stroke-dasharray %.10g,%.10g\n",
		         pixel_width*17,pixel_width*8);
      break;

    case PS_ALTERNATE:
    case PS_DOT: /* DOTTED_LINE, 3,3 */
      wmf_stream_printf (API,out,"stroke-antialias 0\n");
      wmf_stream_printf (API,out,"stroke-dasharray %.10g,%.10g\n",
		         pixel_width*3,pixel_width*3);
      break;

    case PS_DASHDOT: /* DASH_DOT_LINE, 9,6,3,6 */
      wmf_stream_printf (API,out,"stroke-antialias 0\n");
      wmf_stream_printf (API,out,"stroke-dasharray %.10g,%.10g,%.10g,%.10g\n",
		         pixel_width*9,pixel_width*6,pixel_width*3,pixel_width*6);
      break;

    case PS_DASHDOTDOT: /* DASH_2_DOTS_LINE, 9,3,3,3,3,3 */
      wmf_stream_printf (API,out,"stroke-antialias 0\n");
      wmf_stream_printf (API,out,"stroke-dasharray %.10g,%.10g,%.10g,%.10g,%.10g,%.10g\n",
		         pixel_width*9,pixel_width*3,pixel_width*3,pixel_width*3,
                         pixel_width*3,pixel_width*3);
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
    canvas_info;

  char
    buff[MaxTextExtent],
    font_map_path[MaxTextExtent],
    *mvg;

  int
    image_height,
    image_width;

  float
    wmf_width,
    wmf_height;

  double
    resolution_y,
    resolution_x,
    units_per_inch;

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

  /*
   * Open WMF file
   *
   */
  wmf_error = wmf_file_open (API, (char*)image_info->filename);
  if ( wmf_error != wmf_E_None )
    {
      wmf_api_destroy (API);
      ThrowReaderException(FileOpenError,"Unable to open file",image);
    }

  /*
   * Scan WMF file
   *
   */
  wmf_error = wmf_scan (API,0,&bounding_box);
  if ( wmf_error != wmf_E_None )
    {
      wmf_api_destroy (API);
      ThrowReaderException(CorruptImageError,"Failed to scan file",image);
    }

  /*
   * Compute dimensions and scale factors
   *
   */

  ddata = WMF_MAGICK_GetData (API);
  ddata->bbox = bounding_box;

  /* User specified resolution */
  resolution_y = 72.0;
  if(image->y_resolution > 0)
    {
      resolution_y = image->y_resolution;
      if ( image->units == PixelsPerCentimeterResolution )
        resolution_y *= CENTIMETERS_PER_INCH;
    }

  resolution_x = 72.0;
  if(image->x_resolution > 0)
    {
      resolution_x = image->x_resolution;
      if ( image->units == PixelsPerCentimeterResolution )
        resolution_x *= CENTIMETERS_PER_INCH;
    }

  /* Obtain output size expressed in metafile units */
  wmf_error = wmf_size (API,&wmf_width,&wmf_height);
  if ( wmf_error != wmf_E_None )
    {
      wmf_api_destroy (API);
      ThrowReaderException(CorruptImageError,"Failed to compute output size",
                           image);
    }

  /* Obtain metafile units */
  units_per_inch = TWIPS_PER_INCH;
  if((API)->File->placeable)
    units_per_inch = (API)->File->pmh->Inch;
  ddata->units_per_inch = units_per_inch;

  /* Calculate image width and height based on specified resolution */
  image_width  = ceil(((double)wmf_width/units_per_inch)*resolution_x);
  image_height = ceil(((double)wmf_height/units_per_inch)*resolution_y);

  /* Compute scale factors */
  ddata->scale_x = ((double)image_width)/(ddata->bbox.BR.x - ddata->bbox.TL.x);
  ddata->scale_y = ((double)image_height)/(ddata->bbox.BR.y - ddata->bbox.TL.y);

  /* Compute translation to place bounding box at image origin */
  ddata->translate_x = -(ddata->bbox.TL.x);
  ddata->translate_y = -(ddata->bbox.TL.y);

  /*   printf("Size in metafile units: %.10gx%.10g\n", wmf_width, wmf_height); */
  /*   printf("Metafile units/inch:    %.10g\n", ddata->units_per_inch); */
  /*   printf("Bounding Box:           %.10g,%.10g %.10g,%.10g\n", bounding_box.TL.x, bounding_box.TL.y, */
  /*          bounding_box.BR.x,bounding_box.BR.y); */
  /*   printf("Output resolution:      %.10gx%.10g\n", resolution_x, resolution_y); */
  /*   printf("Image size:             %ix%i\n", image_width, image_height); */
  /*   printf("Scale factor:           %.10g,%.10g\n", ddata->scale_x, ddata->scale_y ); */
  /*   printf("Translation:            %.10g,%.10g\n", ddata->translate_x, ddata->translate_y ); */
/*   { */

/*     wmfPlayer_t* P = (wmfPlayer_t*) API->player_data; */
/*     printf("Pixel width:            %.10g\n", P->dc->pixel_width); */
/*   } */

  /*
   * Create canvas image
   *
   */
  canvas_info = CloneImageInfo((ImageInfo *) NULL);
  sprintf( buff, "%ix%i", image_width, image_height );
  (void) CloneString(&(canvas_info->size), buff);
  if(image_info->texture == (char*)NULL)
    {
      QueryColorDatabase("none", &canvas_info->background_color);
      if(ColorMatch(&image_info->background_color,
                    &canvas_info->background_color,0))
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
    sprintf(canvas_info->filename,"TILE:%.1024s",image_info->texture);
  GetExceptionInfo(exception);
  DestroyImage(image);
  image=ReadImage( canvas_info, exception );
  DestroyImageInfo(canvas_info);
  if ( image == (Image*)NULL )
    {
      wmf_api_destroy (API);
      return image;
    }
  strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  strncpy(image->magick,image_info->magick,MaxTextExtent-1);
  ddata->image = image;

  /*
   * Play file to generate MVG drawing commands
   *
   */
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

  /*
   * Scribble on canvas image
   *
   */
  ScribbleMVG(image,mvg);

  /* Cleanup allocated data */
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
