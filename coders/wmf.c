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
%                              Bob Friesenhahn                                %
%                            Dec 2000 - May 2001                              %
%                            Oct 2001 - May 2002                              %
%                                                                             %
%                           Port to libwmf 0.2 API                            %
%                            Francis J. Franklin                              %
%                            May 2001 - Oct 2001                              %
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
 * Include declarations.
 */
#include "studio.h"
#if defined(WIN32)
#define M_PI MagickPI
#endif

#if defined(HasWMF) || defined(HasWMFlite)
/* #if !defined(HasWIN32WMFAPI) */

#define ERR(API)  ((API)->err != wmf_E_None)
#define XC(x) ((double)x)
#define YC(y) ((double)y)

#include "libwmf/fund.h"
#include "libwmf/types.h"
#include "libwmf/api.h"
#undef SRCCOPY
#undef SRCPAINT
#undef SRCAND
#undef SRCINVERT
#undef SRCERASE
#undef NOTSRCCOPY
#undef NOTSRCERASE
#undef MERGECOPY
#undef MERGEPAINT
#undef PATCOPY
#undef PATPAINT
#undef PATINVERT
#undef DSTINVERT
#undef BLACKNESS
#undef WHITENESS
#include "libwmf/defs.h"
#include "libwmf/ipa.h"
#include "libwmf/color.h"
#include "libwmf/macro.h"

/* Unit conversions */
#define TWIPS_PER_INCH        1440
#define CENTIMETERS_PER_INCH  2.54
#define POINTS_PER_INCH       72

#if defined(HasWMFlite)
# define wmf_api_create(api,flags,options) wmf_lite_create(api,flags,options)
# define wmf_api_destroy(api) wmf_lite_destroy(api)
# undef WMF_FONT_PSNAME
# define WMF_FONT_PSNAME(F) ((F)->user_data ? ((wmf_magick_font_t*) (F)->user_data)->ps_name : 0)

typedef struct _wmf_magick_font_t wmf_magick_font_t;

struct _wmf_magick_font_t
{
  char*  ps_name;
  double pointsize;
};

#endif

typedef struct _wmf_magick_t wmf_magick_t;

struct _wmf_magick_t
{
  /* Bounding box */
  wmfD_Rect
    bbox;

  /* Scale and translation factors */
  double
    scale_x,
    scale_y,
    translate_x,
    translate_y,
    rotate;

  /* MVG output */
  DrawContext
    draw_context;

  char
    *mvg;

  size_t
    mvg_alloc,
    mvg_length;

  /* ImageMagick image */
    Image
      *image;

  /* ImageInfo */
    const ImageInfo
      *image_info;

  /* Pattern ID */
  unsigned long
    pattern_id;

  /* Clip path flag */
  unsigned int
    clipping;

  /* Clip path ID */
  unsigned long
    clip_path_id;

  /* Push depth */
  long
    push_depth;
};


#define WMF_MAGICK_GetData(Z) ((wmf_magick_t*)((Z)->device_data))

#define WmfDrawContext (((wmf_magick_t*)((API)->device_data))->draw_context)

/* Enum to control whether util_set_brush applies brush to fill or
   stroke. */
typedef enum
{
  BrushApplyFill,
  BrushApplyStroke
} BrushApply;


/* Enum to specify arc type */
typedef enum
{
  magick_arc_ellipse = 0,
  magick_arc_open,
  magick_arc_pie,
  magick_arc_chord
}
magick_arc_t;

#if defined(HasWMFlite)
static void  lite_font_init (wmfAPI* API, wmfAPI_Options* options);
static void  lite_font_map(wmfAPI* API,wmfFont* font);
static float lite_font_stringwidth(wmfAPI* API, wmfFont* font, char* str);
#endif

static void         draw_color_fill_rgb(wmfAPI* API, const wmfRGB* rgb);
static void         draw_color_stroke_rgb(wmfAPI* API, const wmfRGB* rgb);
static void         draw_pattern_push(wmfAPI* API, unsigned long id, unsigned long columns, unsigned long rows);
static int          ipa_blob_read(void* context);
static int          ipa_blob_seek(void* context,long position);
static long         ipa_blob_tell(void* context);
static void         ipa_bmp_draw(wmfAPI * API, wmfBMP_Draw_t * bmp_draw);
static void         ipa_bmp_free(wmfAPI * API, wmfBMP * bmp);
static void         ipa_bmp_read(wmfAPI * API, wmfBMP_Read_t * bmp_read);
static void         ipa_device_begin(wmfAPI * API);
static void         ipa_device_close(wmfAPI * API);
static void         ipa_device_end(wmfAPI * API);
static void         ipa_device_open(wmfAPI * API);
static void         ipa_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void         ipa_draw_chord(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void         ipa_draw_ellipse(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void         ipa_draw_line(wmfAPI * API, wmfDrawLine_t * draw_line);
static void         ipa_draw_pie(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void         ipa_draw_pixel(wmfAPI * API, wmfDrawPixel_t * draw_pixel);
static void         ipa_draw_polygon(wmfAPI * API, wmfPolyLine_t * poly_line);
static void         ipa_draw_rectangle(wmfAPI * API, wmfDrawRectangle_t * draw_rect);
static void         ipa_draw_text(wmfAPI * API, wmfDrawText_t * draw_text);
static void         ipa_flood_exterior(wmfAPI * API, wmfFlood_t * flood);
static void         ipa_flood_interior(wmfAPI * API, wmfFlood_t * flood);
static void         ipa_functions(wmfAPI * API);
static void         ipa_poly_line(wmfAPI * API, wmfPolyLine_t * poly_line);
static void         ipa_region_clip(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void         ipa_region_frame(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void         ipa_region_paint(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void         ipa_rop_draw(wmfAPI * API, wmfROP_Draw_t * rop_draw);
static void         ipa_udata_copy(wmfAPI * API, wmfUserData_t * userdata);
static void         ipa_udata_free(wmfAPI * API, wmfUserData_t * userdata);
static void         ipa_udata_init(wmfAPI * API, wmfUserData_t * userdata);
static void         ipa_udata_set(wmfAPI * API, wmfUserData_t * userdata);
static void         util_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc,magick_arc_t finish);
static int          util_font_weight( const char* font );
static double       util_pointsize( wmfAPI* API, wmfFont* font, char* str, double font_height);
static void         util_set_brush(wmfAPI * API, wmfDC * dc, const BrushApply brush_apply);
static void         util_set_pen(wmfAPI * API, wmfDC * dc);

/* Set fill color */
static void draw_color_fill_rgb( wmfAPI* API, const wmfRGB* rgb )
{
  PixelPacket
    fill_color;

  fill_color.red     = Upscale(rgb->r);
  fill_color.green   = Upscale(rgb->g);
  fill_color.blue    = Upscale(rgb->b);
  fill_color.opacity = OpaqueOpacity;

  DrawSetFillColor(WmfDrawContext,&fill_color);
  
}

/* Set stroke color */
static void draw_color_stroke_rgb( wmfAPI* API, const wmfRGB* rgb )
{
  PixelPacket
    stroke_color;
  
  stroke_color.red     = Upscale(rgb->r);
  stroke_color.green   = Upscale(rgb->g);
  stroke_color.blue    = Upscale(rgb->b);
  stroke_color.opacity = OpaqueOpacity;
  
  DrawSetStrokeColor(WmfDrawContext,&stroke_color);
}

static void draw_pattern_push( wmfAPI* API,
                               unsigned long id,
                               unsigned long columns,
                               unsigned long rows )
{
  char
    pattern_id[30];

  FormatString(pattern_id,"brush_%lu",id);
  DrawPushPattern(WmfDrawContext,pattern_id,0,0,columns,rows);
}

static void ipa_rop_draw(wmfAPI * API, wmfROP_Draw_t * rop_draw)
{
  if (!TO_FILL(rop_draw))
    return;

  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  /* FIXME: finish implementing (once we know what it is supposed to do!)*/

  util_set_brush(API, rop_draw->dc, BrushApplyFill);

  switch (rop_draw->ROP) /* Ternary raster operations */
    {
    case SRCCOPY: /* dest = source */
      printf("ipa_rop_draw SRCCOPY ROP mode not implemented\n");
      break;
    case SRCPAINT: /* dest = source OR dest */
      printf("ipa_rop_draw SRCPAINT ROP mode not implemented\n");
      break;
    case SRCAND: /* dest = source AND dest */
      printf("ipa_rop_draw SRCAND ROP mode not implemented\n");
      break;
    case SRCINVERT: /* dest = source XOR dest */
      printf("ipa_rop_draw SRCINVERT ROP mode not implemented\n");
      break;
    case SRCERASE: /* dest = source AND (NOT dest) */
      printf("ipa_rop_draw SRCERASE ROP mode not implemented\n");
      break;
    case NOTSRCCOPY: /* dest = (NOT source) */
      printf("ipa_rop_draw NOTSRCCOPY ROP mode not implemented\n");
      break;
    case NOTSRCERASE: /* dest = (NOT src) AND (NOT dest) */
      printf("ipa_rop_draw NOTSRCERASE ROP mode not implemented\n");
      break;
    case MERGECOPY: /* dest = (source AND pattern) */
      printf("ipa_rop_draw MERGECOPY ROP mode not implemented\n");
      break;
    case MERGEPAINT: /* dest = (NOT source) OR dest */
      printf("ipa_rop_draw MERGEPAINT ROP mode not implemented\n");
      break;
    case PATCOPY: /* dest = pattern */
      printf("ipa_rop_draw PATCOPY ROP mode not implemented\n");
      break;
    case PATPAINT: /* dest = DPSnoo */
      printf("ipa_rop_draw PATPAINT ROP mode not implemented\n");
      break;
    case PATINVERT: /* dest = pattern XOR dest */
      printf("ipa_rop_draw PATINVERT ROP mode not implemented\n");
      break;
    case DSTINVERT: /* dest = (NOT dest) */
      printf("ipa_rop_draw DSTINVERT ROP mode not implemented\n");
      break;
    case BLACKNESS: /* dest = BLACK */
      printf("ipa_rop_draw BLACKNESS ROP mode not implemented\n");
      break;
    case WHITENESS: /* dest = WHITE */
      printf("ipa_rop_draw WHITENESS ROP mode not implemented\n");
      break;
    default:
      break;
    }

  DrawRectangle(WmfDrawContext,
                 XC(rop_draw->TL.x), YC(rop_draw->TL.y),
                 XC(rop_draw->BR.x), YC(rop_draw->BR.y));

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_bmp_draw(wmfAPI *API, wmfBMP_Draw_t *bmp_draw)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  ExceptionInfo
    exception;

  Image
    *image;

  double
    height,
    width;

  if (bmp_draw->bmp.data == 0)
    return;

  GetExceptionInfo(&exception);
  image = (Image*)bmp_draw->bmp.data;
  if(!image)
    {
       ThrowException(&ddata->image->exception,exception.severity,
                       exception.reason,exception.description);
       return;
    }

  if(bmp_draw->crop.x || bmp_draw->crop.y ||
     (bmp_draw->crop.w != bmp_draw->bmp.width) ||
     (bmp_draw->crop.h != bmp_draw->bmp.height))
    {
      /* Image needs to be cropped */
      Image
        *crop_image;

      RectangleInfo
        crop_info;

      crop_info.x = bmp_draw->crop.x;
      crop_info.y = bmp_draw->crop.y;
      crop_info.width = bmp_draw->crop.w;
      crop_info.height = bmp_draw->crop.h;

      crop_image = CropImage( image, &crop_info, &exception );
      if(crop_image)
        {
          DestroyImageList(image);
          image = crop_image;
          bmp_draw->bmp.data = (void*)image;
        }
      else
        ThrowException(&ddata->image->exception,exception.severity,
                       exception.reason,exception.description);
    }

  width = AbsoluteValue(bmp_draw->pixel_width * (double) bmp_draw->crop.w);
  height = AbsoluteValue(bmp_draw->pixel_height * (double) bmp_draw->crop.h);

  DrawComposite(WmfDrawContext, CopyCompositeOp, XC(bmp_draw->pt.x), YC(bmp_draw->pt.y),
                 width, height, image );

#if 0
  printf("bmp_draw->bmp.data   = 0x%lx\n", (long)bmp_draw->bmp.data);
  printf("registry id          = %li\n", id);
  /* printf("pixel_width          = %.4g\n", bmp_draw->pixel_width); */
  /* printf("pixel_height         = %.4g\n", bmp_draw->pixel_height); */
  printf("bmp_draw->bmp WxH    = %ix%i\n", bmp_draw->bmp.width, bmp_draw->bmp.height);
  printf("bmp_draw->crop WxH   = %ix%i\n", bmp_draw->crop.w, bmp_draw->crop.h);
  printf("bmp_draw->crop x,y   = %i,%i\n", bmp_draw->crop.x, bmp_draw->crop.y);
  printf("image size WxH       = %lux%lu\n", image->columns, image->rows);
#endif
}

static void ipa_bmp_read(wmfAPI * API, wmfBMP_Read_t * bmp_read) {
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  Image
    *image;

  bmp_read->bmp.data = 0;

  GetExceptionInfo(&exception);

  image_info = CloneImageInfo((ImageInfo *) 0);
  strcpy(image_info->magick, "DIB");
  if(bmp_read->width || bmp_read->height)
    {
      char
        size[MaxTextExtent];
      
      FormatString(size,"%ux%u",bmp_read->width,bmp_read->height);
      CloneString(&image_info->size,size);
    }
#if 0
  printf("ipa_bmp_read: buffer=0x%lx length=%ld, width=%i, height=%i\n",
   (long) bmp_read->buffer, bmp_read->length,
   bmp_read->width, bmp_read->height);
#endif
  image = BlobToImage(image_info, (const void *) bmp_read->buffer,
          bmp_read->length, &exception);
  DestroyImageInfo(image_info);
  if (!image)
    {
      char
        description[MaxTextExtent];

      FormatString(description,"packed DIB at offset %ld", bmp_read->offset);
      ThrowException(&ddata->image->exception,CorruptImageError,exception.reason,
                     exception.description);
    }
  else
    {
#if 0
      printf("ipa_bmp_read: rows=%ld,columns=%ld\n\n", image->rows, image->columns);
#endif

      bmp_read->bmp.data   = (void*)image;
      bmp_read->bmp.width  = (U16)image->columns;
      bmp_read->bmp.height = (U16)image->rows;
    }
}

static void ipa_bmp_free(wmfAPI * API, wmfBMP * bmp)
{
  DestroyImageList((Image*)bmp->data);
  bmp->data = (void*) 0;
  bmp->width = (U16) 0;
  bmp->height = (U16) 0;
}

/*
  This is called by wmf_play() the *first* time the meta file is played
 */
static void ipa_device_open(wmfAPI * API)
{
  DrawInfo
    *draw_info;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData (API);

  /* draw_info = CloneDrawInfo(ddata->image_info,(DrawInfo*)NULL); */

  ddata->pattern_id = 0;
  ddata->clipping = False;
  ddata->clip_path_id = 0;

  ddata->push_depth = 0;

  ddata->draw_context = DrawAllocateContext((DrawInfo*)NULL,ddata->image);

  /* DestroyDrawInfo(draw_info); */
}

/*
  This is called by wmf_api_destroy()
 */
static void ipa_device_close(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  DrawDestroyContext(ddata->draw_context);
}

/*
  This is called from the beginning of each play for initial page setup
 */
static void ipa_device_begin(wmfAPI * API)
{
  char
    comment[MaxTextExtent];

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Make SVG output happy */
  DrawPushGraphicContext(WmfDrawContext);

  DrawSetViewbox(WmfDrawContext, 0, 0, ddata->image->columns, ddata->image->rows );

  FormatString(comment,"Created by ImageMagick %s (http://www.imagemagick.org/)",
               MagickLibVersionText);
  DrawComment(WmfDrawContext,comment);

  /* Scale width and height to image */
  DrawSetScale(WmfDrawContext, ddata->scale_x, ddata->scale_y);

  /* Translate to TL corner of bounding box */
  DrawSetTranslate(WmfDrawContext, ddata->translate_x, ddata->translate_y);

  /* Apply rotation */
  DrawSetRotate(WmfDrawContext, ddata->rotate);

  if(ddata->image_info->texture == NULL)
    {
      /* Draw rectangle in background color */
      DrawSetFillColor(WmfDrawContext,&ddata->image->background_color);
      DrawRectangle(WmfDrawContext,
                     XC(ddata->bbox.TL.x),YC(ddata->bbox.TL.y),
                     XC(ddata->bbox.BR.x),YC(ddata->bbox.BR.y));
    }
  else
    {
      /* Draw rectangle with texture image the SVG way */
      Image
        *image;

      ImageInfo
        *image_info;
      
      ExceptionInfo
        exception;

      GetExceptionInfo(&exception);

      image_info = CloneImageInfo((ImageInfo *) 0);
      strcpy(image_info->filename, ddata->image_info->texture);
      image = ReadImage(image_info,&exception);
      DestroyImageInfo(image_info);
      if(image)
        {
          char
            pattern_id[30];

          DrawPushDefs(WmfDrawContext);
          draw_pattern_push(API, ddata->pattern_id, image->columns, image->rows);
          DrawComposite(WmfDrawContext, CopyCompositeOp, 0, 0, image->columns, image->rows, image);
          DrawPopPattern(WmfDrawContext);
          DrawPopDefs(WmfDrawContext);
          FormatString(pattern_id,"#brush_%lu",ddata->pattern_id);
          DrawSetFillPatternURL(WmfDrawContext,pattern_id);
          ++ddata->pattern_id;

          DrawRectangle(WmfDrawContext,
                         XC(ddata->bbox.TL.x),YC(ddata->bbox.TL.y),
                         XC(ddata->bbox.BR.x),YC(ddata->bbox.BR.y));
          DestroyImageList(image);
        }
      else
        {
          ThrowException(&ddata->image->exception,exception.severity,
                         exception.reason,exception.description);
        }
    }

  DrawSetFillOpacity(WmfDrawContext,1.0);
  DrawSetFillColorString(WmfDrawContext,"none");
  DrawSetStrokeColorString(WmfDrawContext,"none");
}

/*
  This is called from the end of each play for page termination
 */
static void ipa_device_end(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Reset any existing clip paths by popping context */
  if(ddata->clipping)
    DrawPopGraphicContext(WmfDrawContext);
  ddata->clipping = False;

  /* Make SVG output happy */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_flood_interior(wmfAPI * API, wmfFlood_t * flood)
{
  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  draw_color_fill_rgb(API,&(flood->color));

  DrawColor(WmfDrawContext,XC(flood->pt.x), YC(flood->pt.y),
            FillToBorderMethod);

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_flood_exterior(wmfAPI * API, wmfFlood_t * flood)
{
  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  draw_color_fill_rgb(API,&(flood->color));

  if (flood->type == FLOODFILLSURFACE)
    DrawColor(WmfDrawContext, XC(flood->pt.x), YC(flood->pt.y),
              FloodfillMethod);
  else
    DrawColor(WmfDrawContext, XC(flood->pt.x), YC(flood->pt.y),
              FillToBorderMethod);

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_draw_pixel(wmfAPI * API, wmfDrawPixel_t * draw_pixel)
{
  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  DrawSetStrokeColorString(WmfDrawContext,"none");

  draw_color_fill_rgb(API,&(draw_pixel->color));

  DrawRectangle(WmfDrawContext,
                 XC(draw_pixel->pt.x),
                 YC(draw_pixel->pt.y),
                 XC(draw_pixel->pt.x + draw_pixel->pixel_width),
                 YC(draw_pixel->pt.y + draw_pixel->pixel_height));

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_draw_pie(wmfAPI * API, wmfDrawArc_t * draw_arc)
{
  util_draw_arc(API, draw_arc, magick_arc_pie);
}

static void ipa_draw_chord(wmfAPI * API, wmfDrawArc_t * draw_arc)
{
  util_draw_arc(API, draw_arc, magick_arc_chord);
}

static void ipa_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc)
{
  util_draw_arc(API, draw_arc, magick_arc_open);
}

static void ipa_draw_ellipse(wmfAPI * API, wmfDrawArc_t * draw_arc)
{
  util_draw_arc(API, draw_arc, magick_arc_ellipse);
}

static void util_draw_arc(wmfAPI * API,
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

  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

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

      util_set_pen(API, draw_arc->dc);
      if (finish == magick_arc_open)
        DrawSetFillColorString(WmfDrawContext,"none");
      else
        util_set_brush(API, draw_arc->dc, BrushApplyFill);

      if (finish == magick_arc_ellipse)
        DrawEllipse(WmfDrawContext, XC(O.x), YC(O.y), Rx, Ry, 0, 360);
      else if (finish == magick_arc_pie)
        DrawEllipse(WmfDrawContext, XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
      else if (finish == magick_arc_chord)
        {
          DrawArc(WmfDrawContext, XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
          DrawLine(WmfDrawContext, XC(start.x), YC(start.y), XC(end.x), YC(end.y));
        }
      else      /* if (finish == magick_arc_open) */
        DrawArc(WmfDrawContext, XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
    }

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_draw_line(wmfAPI * API, wmfDrawLine_t * draw_line)
{
  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  if (TO_DRAW(draw_line))
    {
      util_set_pen(API, draw_line->dc);
      DrawLine(WmfDrawContext,
               XC(draw_line->from.x), YC(draw_line->from.y),
               XC(draw_line->to.x), YC(draw_line->to.y));
    }

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_poly_line(wmfAPI * API, wmfPolyLine_t * poly_line)
{
  U16
    i;

  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  if (poly_line->count <= 1)
    return;

  if (TO_DRAW(poly_line))
    {
      PointInfo
        *points;

      DrawSetFillColorString(WmfDrawContext,"none");
      util_set_pen(API, poly_line->dc);

      points = (PointInfo*)AcquireMemory(poly_line->count * sizeof(PointInfo));
      for (i = 0; i < poly_line->count; i++)
        {
          points[i].x = XC(poly_line->pt[i].x);
          points[i].y = YC(poly_line->pt[i].y);
        }
      DrawPolyline(WmfDrawContext,poly_line->count,points);
      LiberateMemory((void**)&points);
    }

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_draw_polygon(wmfAPI * API, wmfPolyLine_t * poly_line)
{
  U16
    i;

  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  if (poly_line->count <= 2)
    return;

  if (TO_FILL(poly_line) || TO_DRAW(poly_line))
    {
      PointInfo
        *points;

      util_set_pen(API, poly_line->dc);
      util_set_brush(API, poly_line->dc, BrushApplyFill);

      points = (PointInfo*)AcquireMemory(poly_line->count * sizeof(PointInfo));
      for (i = 0; i < poly_line->count; i++)
        {
          points[i].x = XC(poly_line->pt[i].x);
          points[i].y = YC(poly_line->pt[i].y);
        }
      DrawPolygon(WmfDrawContext,poly_line->count,points);
      LiberateMemory((void**)&points);
    }

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_draw_rectangle(wmfAPI * API, wmfDrawRectangle_t * draw_rect)
{
  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  if (TO_FILL(draw_rect) || TO_DRAW(draw_rect))
    {
      util_set_pen(API, draw_rect->dc);
      util_set_brush(API, draw_rect->dc, BrushApplyFill);

      if ((draw_rect->width > 0) || (draw_rect->height > 0))
        DrawRoundRectangle(WmfDrawContext,
                           XC(draw_rect->TL.x), YC(draw_rect->TL.y),
                           XC(draw_rect->BR.x), YC(draw_rect->BR.y),
                           draw_rect->width / 2, draw_rect->height / 2);
      else
        DrawRectangle(WmfDrawContext,
                      XC(draw_rect->TL.x), YC(draw_rect->TL.y),
                      XC(draw_rect->BR.x), YC(draw_rect->BR.y));
    }

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

/* Draw an un-filled rectangle using the current brush */
static void ipa_region_frame(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{
  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  if (TO_FILL(poly_rect) || TO_DRAW(poly_rect))
    {
      unsigned int
        i;

      DrawSetFillColorString(WmfDrawContext,"none");
      util_set_brush(API, poly_rect->dc, BrushApplyStroke);

      for (i = 0; i < poly_rect->count; i++)
        {
          DrawRectangle(WmfDrawContext,
                         XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                         XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
        }
    }

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_region_paint(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{

  if (poly_rect->count == 0)
    return;

  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

  if (TO_FILL (poly_rect))
    {
      unsigned int
        i;

      DrawSetStrokeColorString(WmfDrawContext,"none");
      util_set_brush(API, poly_rect->dc, BrushApplyFill);

      for (i = 0; i < poly_rect->count; i++)
        {
          DrawRectangle(WmfDrawContext,
                         XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                         XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
        }
    }

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);
}

static void ipa_region_clip(wmfAPI *API, wmfPolyRectangle_t *poly_rect)
{
  unsigned int
    i;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData (API);

  /* Reset any existing clip paths by popping context */
  if(ddata->clipping)
    DrawPopGraphicContext(WmfDrawContext);
  ddata->clipping = False;

  if(poly_rect->count > 0)
    {
      char
        clip_path_id[30];

      /* Define clip path */
      ddata->clip_path_id++;
      DrawPushDefs(WmfDrawContext);
      FormatString(clip_path_id,"clip_%lu",ddata->clip_path_id);
      DrawPushClipPath(WmfDrawContext,clip_path_id);
      DrawPushGraphicContext(WmfDrawContext);
      for (i = 0; i < poly_rect->count; i++)
        {
          DrawRectangle(WmfDrawContext,
                         XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                         XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
        }
      DrawPopGraphicContext(WmfDrawContext);
      DrawPopClipPath(WmfDrawContext);
      DrawPopDefs(WmfDrawContext);

      /* Push context for new clip paths */
      DrawPushGraphicContext(WmfDrawContext);
      DrawSetClipPath(WmfDrawContext,clip_path_id);
      ddata->clipping = True;
    }
}

static void ipa_functions(wmfAPI *API)
{
  wmf_magick_t
    *ddata = 0;

  wmfFunctionReference
    *FR = (wmfFunctionReference *) API->function_reference;

  /*
     IPA function reference links
   */
  FR->device_open = ipa_device_open;
  FR->device_close = ipa_device_close;
  FR->device_begin = ipa_device_begin;
  FR->device_end = ipa_device_end;
  FR->flood_interior = ipa_flood_interior;
  FR->flood_exterior = ipa_flood_exterior;
  FR->draw_pixel = ipa_draw_pixel;
  FR->draw_pie = ipa_draw_pie;
  FR->draw_chord = ipa_draw_chord;
  FR->draw_arc = ipa_draw_arc;
  FR->draw_ellipse = ipa_draw_ellipse;
  FR->draw_line = ipa_draw_line;
  FR->poly_line = ipa_poly_line;
  FR->draw_polygon = ipa_draw_polygon;
  FR->draw_rectangle = ipa_draw_rectangle;
  FR->rop_draw = ipa_rop_draw;
  FR->bmp_draw = ipa_bmp_draw;
  FR->bmp_read = ipa_bmp_read;
  FR->bmp_free = ipa_bmp_free;
  FR->draw_text = ipa_draw_text;
  FR->udata_init = ipa_udata_init;
  FR->udata_copy = ipa_udata_copy;
  FR->udata_set = ipa_udata_set;
  FR->udata_free = ipa_udata_free;
  FR->region_frame = ipa_region_frame;
  FR->region_paint = ipa_region_paint;
  FR->region_clip = ipa_region_clip;

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
  ddata->mvg = 0;
  ddata->mvg_alloc = 0;
  ddata->mvg_length = 0;
  ddata->image = 0;
}

static void ipa_draw_text(wmfAPI * API, wmfDrawText_t * draw_text)
{
  double    
    angle = 0,      /* text rotation angle */
    bbox_height,    /* bounding box height */
    bbox_width,      /* bounding box width */
    pointsize = 0;    /* pointsize to output font with desired height */

  TypeMetric
    metrics;

  wmfD_Coord
    BL,        /* bottom left of bounding box */
    BR,        /* bottom right of bounding box */
    TL,        /* top left of bounding box */
    TR;        /* top right of bounding box */

  wmfD_Coord
    point;      /* text placement point */

  wmfFont
    *font;

  wmf_magick_t
    * ddata = WMF_MAGICK_GetData(API);

  point = draw_text->pt;

  /* Choose bounding box and calculate its width and height */
  {
    double dx,
      dy;

    if( draw_text->flags)
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

  /* Convert font_height to equivalent pointsize */
  pointsize = util_pointsize( API, font, draw_text->str, draw_text->font_height);

  /* Save graphic context */
  DrawPushGraphicContext(WmfDrawContext);

#if 0
  printf("\nipa_draw_text\n");
  printf("Text                    = \"%s\"\n", draw_text->str);
  /* printf("WMF_FONT_NAME:          = \"%s\"\n", WMF_FONT_NAME(font)); */
  printf("WMF_FONT_PSNAME:        = \"%s\"\n", WMF_FONT_PSNAME(font));
  printf("Bounding box            TL=%.4g,%.4g BR=%.4g,%.4g\n",
         TL.x, TL.y, BR.x, BR.y );
  /* printf("Text box                = %.4gx%.4g\n", bbox_width, bbox_height); */
  /* printf("WMF_FONT_HEIGHT         = %i\n", (int)WMF_FONT_HEIGHT(font)); */
  printf("Pointsize               = %.4g\n", pointsize);
  fflush(stdout);
#endif

  /*
   * Obtain font metrics if required
   *
   */
  if ((WMF_DC_TEXTALIGN(draw_text->dc) & TA_CENTER) ||
      (WMF_TEXT_UNDERLINE(font)) || (WMF_TEXT_STRIKEOUT(font)))
    {
      Image
        *image = ddata->image;
      
      DrawInfo
        draw_info;
      
      ImageInfo
        *image_info;
      
      image_info = CloneImageInfo((ImageInfo *) NULL);
      CloneString(&image_info->font, WMF_FONT_PSNAME(font));
      image_info->pointsize = pointsize;
      GetDrawInfo(image_info, &draw_info);
      CloneString(&draw_info.text, draw_text->str);
      
      if (GetTypeMetrics(image, &draw_info, &metrics) != False)
        {
          /* Center the text if it is not yet centered and should be */
          if ((WMF_DC_TEXTALIGN(draw_text->dc) & TA_CENTER))
            {
              double
                text_width = metrics.width * (ddata->scale_y / ddata->scale_x);
              
#if defined(HasWMFlite)
              point.x -= text_width / 2;
#else
              point.x += bbox_width / 2 - text_width / 2;
#endif
            }
        }
    }

  /* Set text background color */
  if (draw_text->flags & ETO_OPAQUE)
    {
      /* Draw bounding-box background color (META_EXTTEXTOUT mode) */
      DrawSetStrokeColorString(WmfDrawContext,"none");
      draw_color_fill_rgb(API,WMF_DC_BACKGROUND(draw_text->dc));
      DrawRectangle(WmfDrawContext,
                    XC(draw_text->TL.x),YC(draw_text->TL.y),
                    XC(draw_text->BR.x),YC(draw_text->BR.y));
      DrawSetFillColorString(WmfDrawContext,"none");
    }
  else
    {
      /* Set text undercolor */
      if (WMF_DC_OPAQUE(draw_text->dc))
        {
          wmfRGB
            *box = WMF_DC_BACKGROUND(draw_text->dc);

          PixelPacket
            under_color;

          under_color.red     = Upscale(box->r);
          under_color.green   = Upscale(box->g);
          under_color.blue    = Upscale(box->b);
          under_color.opacity = OpaqueOpacity;

          DrawSetTextUnderColor(WmfDrawContext,&under_color);
        }
      else
        DrawSetTextUnderColorString(WmfDrawContext,"none");
    }

  /* Set text clipping (META_EXTTEXTOUT mode) */
  if( draw_text->flags & ETO_CLIPPED)
    {
    }

  /* Set stroke color */
  DrawSetStrokeColorString(WmfDrawContext,"none");

  /* Set fill color */
  draw_color_fill_rgb(API,WMF_DC_TEXTCOLOR(draw_text->dc));

  /* Output font size */
  DrawSetFontSize(WmfDrawContext,pointsize);

  /* Output Postscript font name */
  DrawSetFont(WmfDrawContext, WMF_FONT_PSNAME(font));

  /* Translate coordinates so target is 0,0 */
  DrawSetTranslate(WmfDrawContext, XC(point.x), YC(point.y));

  /* Transform horizontal scale to draw text at 1:1 ratio */
  DrawSetScale(WmfDrawContext, ddata->scale_y / ddata->scale_x, 1.0);

  /* Apply rotation */
  /* ImageMagick's drawing rotation is clockwise from horizontal
     while WMF drawing rotation is counterclockwise from horizontal */
  angle = AbsoluteValue(RadiansToDegrees(2 * MagickPI - WMF_TEXT_ANGLE(font)));
  if (angle == 360)
    angle = 0;
  if (angle != 0)
    DrawSetRotate(WmfDrawContext, angle);

  /*
   * Render text
   *
   */

  /* Output string */
  DrawAnnotation(WmfDrawContext, 0, 0, draw_text->str);

  /* Underline text the Windows way (at the bottom) */
  if (WMF_TEXT_UNDERLINE(font))
    {
      double
        line_height;

      wmfD_Coord
        ulBR,      /* bottom right of underline rectangle */
        ulTL;      /* top left of underline rectangle */

      line_height = ((double)1/(ddata->scale_x))*metrics.underline_thickness;
      if(metrics.underline_thickness < 1.5)
        line_height *= 0.55;
      ulTL.x = 0;
      ulTL.y = AbsoluteValue(metrics.descent) - line_height;
      ulBR.x = metrics.width;
      ulBR.y = AbsoluteValue(metrics.descent);

      DrawRectangle(WmfDrawContext,
                    XC(ulTL.x), YC(ulTL.y), XC(ulBR.x), YC(ulBR.y));
    }

  /* Strikeout text the Windows way */
  if (WMF_TEXT_STRIKEOUT(font))
    {
      double line_height;

      wmfD_Coord
        ulBR,      /* bottom right of strikeout rectangle */
        ulTL;      /* top left of strikeout rectangle */

      line_height = ((double)1/(ddata->scale_x))*metrics.underline_thickness;

      if(metrics.underline_thickness < 2.0)
        line_height *= 0.55;
      ulTL.x = 0;
      ulTL.y = -(((double) metrics.ascent) / 2 + line_height / 2);
      ulBR.x = metrics.width;
      ulBR.y = -(((double) metrics.ascent) / 2 - line_height / 2);

      DrawRectangle(WmfDrawContext,
                    XC(ulTL.x), YC(ulTL.y), XC(ulBR.x), YC(ulBR.y));

    }

  /* Restore graphic context */
  DrawPopGraphicContext(WmfDrawContext);

#if 0
  DrawPushGraphicContext(WmfDrawContext);
  DrawSetStrokeColorString(WmfDrawContext,"red");
  DrawSetFillColorString(WmfDrawContext,"none");
  DrawRectangle(WmfDrawContext,
                XC(TL.x), YC(TL.y),
                XC(BR.x), YC(BR.y));
  DrawSetStrokeColorString(WmfDrawContext,"none");
  DrawPopGraphicContext(WmfDrawContext);
#endif

}

static void ipa_udata_init(wmfAPI * API, wmfUserData_t * userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

}

static void ipa_udata_copy(wmfAPI * API, wmfUserData_t * userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

}

static void ipa_udata_set(wmfAPI * API, wmfUserData_t * userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

}

static void ipa_udata_free(wmfAPI * API, wmfUserData_t * userdata)
{
  /* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

}

static void util_set_brush(wmfAPI * API, wmfDC * dc, const BrushApply brush_apply)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  wmfBrush
    *brush = WMF_DC_BRUSH(dc);

  /* Set polygon fill rule */
  switch (WMF_DC_POLYFILL(dc))  /* Is this correct ?? */
    {
    case WINDING:
      DrawSetClipRule(WmfDrawContext,NonZeroRule);
      break;

    case ALTERNATE:
    default:
      DrawSetClipRule(WmfDrawContext,EvenOddRule);
      break;
    }

#if 0
  printf("WMF_DC_OPAQUE    : %i\n", WMF_DC_OPAQUE(dc));
  printf("WMF_DC_POLYFILL  : %i\n", (int)WMF_DC_POLYFILL(dc));
  printf("WMF_BRUSH_STYLE  : %i\n", (int)WMF_BRUSH_STYLE(brush));
  if(WMF_BRUSH_COLOR(brush))
    printf("WMF_BRUSH_COLOR  : #%02x%02x%02x\n",
           (int) WMF_BRUSH_COLOR(brush)->r,
           (int) WMF_BRUSH_COLOR(brush)->g,
           (int) WMF_BRUSH_COLOR(brush)->b);
  if(WMF_DC_BACKGROUND(dc))
    printf("WMF_DC_BACKGROUND: #%02x%02x%02x\n",
           (int) WMF_DC_BACKGROUND(dc)->r,
           (int) WMF_DC_BACKGROUND(dc)->g,
           (int) WMF_DC_BACKGROUND(dc)->b);
#endif

  switch (WMF_BRUSH_STYLE(brush))
    {
    case BS_SOLID /* 0 */:
      /* WMF_BRUSH_COLOR specifies brush color, WMF_BRUSH_HATCH
         ignored */
      {
        if( brush_apply == BrushApplyStroke )
          draw_color_stroke_rgb(API,WMF_BRUSH_COLOR(brush));
        else
          draw_color_fill_rgb(API,WMF_BRUSH_COLOR(brush));
        break;
      }
    case BS_HOLLOW /* 1 */:    /* BS_HOLLOW & BS_NULL share enum */
      /* WMF_BRUSH_COLOR and WMF_BRUSH_HATCH ignored */
      {
        if( brush_apply == BrushApplyStroke )
          DrawSetStrokeColorString(WmfDrawContext,"none");
        else
          DrawSetFillColorString(WmfDrawContext,"none");
        break;
      }
    case BS_HATCHED /* 2 */:
      /* WMF_BRUSH_COLOR specifies the hatch color, WMF_BRUSH_HATCH
         specifies the hatch brush style. If WMF_DC_OPAQUE, then
         WMF_DC_BACKGROUND specifies hatch background color.  */
      {
        DrawPushDefs(WmfDrawContext);
        draw_pattern_push(API, ddata->pattern_id, 8, 8);
        DrawPushGraphicContext(WmfDrawContext);
        
        if (WMF_DC_OPAQUE(dc))
          {
            if( brush_apply == BrushApplyStroke )
              draw_color_stroke_rgb(API,WMF_DC_BACKGROUND(dc));
            else
              draw_color_fill_rgb(API,WMF_DC_BACKGROUND(dc));

            DrawRectangle(WmfDrawContext, 0, 0, 7, 7 );
          }

        DrawSetStrokeAntialias(WmfDrawContext, False);
        DrawSetStrokeWidth(WmfDrawContext, 1);
        
        draw_color_stroke_rgb(API,WMF_BRUSH_COLOR(brush));
        
        switch ((unsigned int) WMF_BRUSH_HATCH(brush))
          {
            
          case HS_HORIZONTAL:  /* ----- */
            {
              DrawLine(WmfDrawContext, 0, 3, 7,3);
              break;
            }
          case HS_VERTICAL:  /* ||||| */
            {
              DrawLine(WmfDrawContext, 3, 0, 3, 7);
              break;
            }
          case HS_FDIAGONAL:  /* \\\\\ */
            {
              DrawLine(WmfDrawContext, 0, 0, 7, 7);
              break;
            }
          case HS_BDIAGONAL:  /* ///// */
            {
              DrawLine(WmfDrawContext, 0, 7, 7, 0 );
              break;
            }
          case HS_CROSS:  /* +++++ */
            {
              DrawLine(WmfDrawContext, 0, 3, 7, 3 );
              DrawLine(WmfDrawContext, 3, 0, 3, 7 );
              break;
            }
          case HS_DIAGCROSS:  /* xxxxx */
            {
              DrawLine(WmfDrawContext, 0, 0, 7, 7 );
              DrawLine(WmfDrawContext, 0, 7, 7, 0 );
              break;
            }
          default:
            {
            }
          }
        DrawPopGraphicContext(WmfDrawContext);
        DrawPopPattern(WmfDrawContext);
        DrawPopDefs(WmfDrawContext);
        {
          char
            pattern_id[30];

          FormatString(pattern_id, "#brush_%lu", ddata->pattern_id);

          if( brush_apply == BrushApplyStroke )
            DrawSetStrokePatternURL(WmfDrawContext,pattern_id);
          else
            DrawSetFillPatternURL(WmfDrawContext,pattern_id);
          ++ddata->pattern_id;
        }
        break;
      }
    case BS_PATTERN /* 3 */:
      /* WMF_BRUSH_COLOR ignored, WMF_BRUSH_HATCH provides handle to
         bitmap */
      {
        printf("util_set_brush: BS_PATTERN not supported\n");
        break;
      }
    case BS_INDEXED /* 4 */:
      {
        printf("util_set_brush: BS_INDEXED not supported\n");
        break;
      }
    case BS_DIBPATTERN /* 5 */:
      {
        wmfBMP
          *brush_bmp = WMF_BRUSH_BITMAP(brush);

        if (brush_bmp && brush_bmp->data != 0)
          {
            CompositeOperator            
              mode;

            const Image
              *image;

            ExceptionInfo
              exception;

            GetExceptionInfo(&exception);

            image = (Image*)brush_bmp->data;

            mode = CopyCompositeOp;  /* Default is copy */
            switch (WMF_DC_ROP(dc))
              {
              case SRCCOPY:  /* dest = source */
                mode = CopyCompositeOp;
                break;
              case SRCPAINT:  /* dest = source OR dest */
                printf("util_set_brush SRCPAINT ROP mode not supported!\n");
                break;
              case SRCAND:  /* dest = source AND dest */
                printf("util_set_brush SRCAND ROP mode not supported!\n");
                break;
              case SRCINVERT:  /* dest = source XOR dest */
                mode = XorCompositeOp;
                break;
              case SRCERASE:  /* dest = source AND (NOT dest) */
                printf("util_set_brush SRCERASE ROP mode not supported!\n");
                break;
              case NOTSRCCOPY:  /* dest = (NOT source) */
                /*                 NegateImage(image, False); */
                /* mode = "Copy"; */
                printf("util_set_brush NOTSRCCOPY ROP mode not supported!\n");
                break;
              case NOTSRCERASE:  /* dest = (NOT source) AND (NOT dest) */
                printf("util_set_brush NOTSRCERASE ROP mode not supported!\n");
                break;
              case MERGECOPY:  /* dest = (source AND pattern) */
                printf("util_set_brush MERGECOPY ROP mode not supported!\n");
                break;
              case MERGEPAINT:  /* dest = (NOT source) OR dest */
                printf("util_set_brush MERGEPAINT ROP mode not supported!\n");
                break;
              case PATCOPY:  /* dest = pattern */
                printf("util_set_brush PATCOPY ROP mode not supported!\n");
                break;
              case PATPAINT:  /* dest = DPSnoo */
                printf("util_set_brush PATPAINT ROP mode not supported!\n");
                break;
              case PATINVERT:  /* dest = pattern XOR dest */
                printf("util_set_brush PATINVERT ROP mode not supported!\n");
                break;
              case DSTINVERT:  /* dest = (NOT dest) */
                printf("util_set_brush DSTINVERT ROP mode not supported!\n");
                break;
              case BLACKNESS:  /* dest = BLACK bits */
                printf("util_set_brush BLACKNESS ROP mode not supported!\n");
                break;
              case WHITENESS:  /* dest = WHITE bits */
                printf("util_set_brush WHITENESS ROP mode not supported!\n");
                break;
              default:
                {
                }
              }

            DrawPushDefs(WmfDrawContext);
            draw_pattern_push(API, ddata->pattern_id, brush_bmp->width, brush_bmp->height);
            DrawComposite(WmfDrawContext,mode, 0, 0, brush_bmp->width, brush_bmp->height, image);
            DrawPopPattern(WmfDrawContext);
            DrawPopDefs(WmfDrawContext);

            {
              char
                pattern_id[30];

              FormatString(pattern_id, "#brush_%lu", ddata->pattern_id);

              if( brush_apply == BrushApplyStroke )
                DrawSetStrokePatternURL(WmfDrawContext,pattern_id);
              else
                DrawSetFillPatternURL(WmfDrawContext,pattern_id);
              ++ddata->pattern_id;
            }
          }
        else
          printf("util_set_brush: no BMP image data!\n");

        break;
      }
    case BS_DIBPATTERNPT /* 6 */:
      /* WMF_BRUSH_COLOR ignored, WMF_BRUSH_HATCH provides pointer to
         DIB */
      {
        printf("util_set_brush: BS_DIBPATTERNPT not supported\n");
        break;
      }
    case BS_PATTERN8X8 /* 7 */:
      {
        printf("util_set_brush: BS_PATTERN8X8 not supported\n");
        break;
      }
    case BS_DIBPATTERN8X8 /* 8 */:
      {
        printf("util_set_brush: BS_DIBPATTERN8X8 not supported\n");
        break;
      }
    default:
      {
      }
    }
}

static void util_set_pen(wmfAPI * API, wmfDC * dc)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  wmfPen
    *pen = 0;

  double
    pen_width,
    pixel_width;

  unsigned int
    pen_endcap,
    pen_join,
    pen_style,
    pen_type;

  pen = WMF_DC_PEN(dc);

  pen_width = (WMF_PEN_WIDTH(pen) + WMF_PEN_HEIGHT(pen)) / 2;

  /* Pixel width is inverse of pixel scale */
  pixel_width = (((double) 1 / (ddata->scale_x)) +
                 ((double) 1 / (ddata->scale_y))) / 2;

  /* Don't allow pen_width to be much less than pixel_width in order
     to avoid dissapearing or spider-web lines */
  pen_width = Max(pen_width, pixel_width*0.8);

  pen_style = (unsigned int) WMF_PEN_STYLE(pen);
  pen_endcap = (unsigned int) WMF_PEN_ENDCAP(pen);
  pen_join = (unsigned int) WMF_PEN_JOIN(pen);
  pen_type = (unsigned int) WMF_PEN_TYPE(pen);

  /* Pen style specified? */
  if (pen_style == PS_NULL)
    {
      DrawSetStrokeColorString(WmfDrawContext,"none");
      return;
    }

  DrawSetStrokeAntialias(WmfDrawContext, 1 );
  DrawSetStrokeWidth(WmfDrawContext, Max(0, pen_width));

  {
    LineCap
      linecap;

    switch (pen_endcap)
      {
      case PS_ENDCAP_SQUARE:
        linecap = SquareCap;
        break;
      case PS_ENDCAP_ROUND:
        linecap = RoundCap;
        break;
      case PS_ENDCAP_FLAT:
      default:
        linecap = ButtCap;
        break;
      }
    DrawSetStrokeLineCap(WmfDrawContext, linecap);
  }

  {
    LineJoin
      linejoin;

    switch (pen_join)
      {
      case PS_JOIN_BEVEL:
        linejoin = BevelJoin;
        break;
      case PS_JOIN_ROUND:
        linejoin = RoundJoin;
        break;
      case PS_JOIN_MITER:
      default:
        linejoin = MiterJoin;
        break;
      }
    DrawSetStrokeLineJoin(WmfDrawContext,linejoin);
  }

  switch (pen_style)
    {
    case PS_DASH:    /* -------  */
      {
        /* Pattern 18,7 */
        double
          dasharray[3] =
        { pixel_width * 18, pixel_width * 7, 0 };

        DrawSetStrokeAntialias(WmfDrawContext,False);
        DrawSetStrokeDashArray(WmfDrawContext,dasharray);
        break;
      }
    case PS_ALTERNATE:
    case PS_DOT:    /* .......  */
      {
        /* Pattern 3,3 */
        double
          dasharray[3] =
        { pixel_width * 3, pixel_width * 3, 0 };

        DrawSetStrokeAntialias(WmfDrawContext,False);
        DrawSetStrokeDashArray(WmfDrawContext,dasharray);
        break;
      }
    case PS_DASHDOT:    /* _._._._  */
      {
        /* Pattern 9,6,3,6 */
        double
          dasharray[5] =
        { pixel_width * 9, pixel_width * 6, pixel_width * 3, pixel_width * 6, 0 };

        DrawSetStrokeAntialias(WmfDrawContext,False);
        DrawSetStrokeDashArray(WmfDrawContext,dasharray);
        break;
      }
    case PS_DASHDOTDOT:  /* _.._.._  */
      {
        /* Pattern 9,3,3,3,3,3 */
        double
          dasharray[7] =
        { pixel_width * 9, pixel_width * 3, pixel_width * 3,
          pixel_width * 3, pixel_width * 3, pixel_width * 3, 0 };

        DrawSetStrokeAntialias(WmfDrawContext,False);
        DrawSetStrokeDashArray(WmfDrawContext,dasharray);
        break;
      }
    case PS_INSIDEFRAME:  /* There is nothing to do in this case... */
    case PS_SOLID:
    default:
      {
        DrawSetStrokeDashArray(WmfDrawContext,(double *)NULL);
        break;
      }
    }

  draw_color_stroke_rgb(API,WMF_PEN_COLOR(pen));
}

/* Estimate font pointsize based on Windows font parameters */
static double util_pointsize( wmfAPI* API, wmfFont* font, char* str, double font_height)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  Image
    *image = ddata->image;

  TypeMetric
    metrics;

  DrawInfo
    draw_info;

  ImageInfo
    *image_info;

  double
    pointsize = 0;

  image_info = CloneImageInfo((ImageInfo *) NULL);
  CloneString(&image_info->font, WMF_FONT_PSNAME(font));
  image_info->pointsize = font_height;
  GetDrawInfo(image_info, &draw_info);
  CloneString(&draw_info.text, str);

  if (GetTypeMetrics(image, &draw_info, &metrics) != False)
    {

      if(strlen(str) == 1)
        {
          pointsize = (font_height *
                       ( font_height / (metrics.ascent + AbsoluteValue(metrics.descent))));
          draw_info.pointsize = pointsize;
          if (GetTypeMetrics(image, &draw_info, &metrics) != False)
            pointsize *= (font_height / ( metrics.ascent + AbsoluteValue(metrics.descent)));
        }
      else
        {
          pointsize = (font_height * (font_height / (metrics.height)));
          draw_info.pointsize = pointsize;
          if (GetTypeMetrics(image, &draw_info, &metrics) != False)
            pointsize *= (font_height / metrics.height);
          
        }


#if 0
      draw_info.pointsize = pointsize;
      if (GetTypeMetrics(image, &draw_info, &metrics) != False)
        pointsize *= (font_height / (metrics.ascent + AbsoluteValue(metrics.descent)));
      pointsize *= 1.114286; /* Magic number computed through trial and error */
#endif
    }
#if 0
  printf("String    = %s\n", str);
  printf("Font      = %s\n", WMF_FONT_PSNAME(font));
  printf("lfHeight  = %.4g\n", font_height);
  printf("bounds    = %.4g,%.4g %.4g,%.4g\n", metrics.bounds.x1, metrics.bounds.y1,
         metrics.bounds.x2,metrics.bounds.y2);
  printf("ascent    = %.4g\n", metrics.ascent);
  printf("descent   = %.4g\n", metrics.descent);
  printf("height    = %.4g\n", metrics.height);
  printf("Pointsize = %.4g\n", pointsize);
#endif

  return floor(pointsize);
}

/* Estimate weight based on font name */
static int util_font_weight( const char* font )
{
  int
    weight;

  weight = 400;
  if((strstr(font,"Normal") || strstr(font,"Regular")))
    weight = 400;
  else if( strstr(font,"Bold") )
    {
      weight = 700;
      if((strstr(font,"Semi") || strstr(font,"Demi")))
        weight = 600;
      if( (strstr(font,"Extra") || strstr(font,"Ultra")))
        weight = 800;
    }
  else if( strstr(font,"Light") )
    {
      weight = 300;
      if( (strstr(font,"Extra") || strstr(font,"Ultra")))
        weight = 200;
    }
  else if((strstr(font,"Heavy") || strstr(font,"Black")))
    weight = 900;
  else if( strstr(font,"Thin") )
    weight = 100;
  return weight;
}

#if defined(HasWMFlite)
/*
 * Returns width of string in points, assuming (unstretched) font size of 1pt
 * (similar to wmf_ipa_font_stringwidth)
 *
 * This is extremely odd at best, particularly since player/meta.h has access
 * to the corrected font_height (as drawtext.font_height) when it invokes the
 * stringwidth callback.  It should be possible to compute the real stringwidth!
 */
static float lite_font_stringwidth( wmfAPI* API, wmfFont* font, char* str)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  Image
    *image = ddata->image;

  DrawInfo
    draw_info;

  ImageInfo
    *image_info;

  TypeMetric
    metrics;

  float
    stringwidth = 0;

  double
    orig_x_resolution,
    orig_y_resolution;

  ResolutionType
    orig_resolution_units;

  orig_x_resolution = image->x_resolution;
  orig_y_resolution = image->y_resolution;
  orig_resolution_units = image->units;

  image_info = CloneImageInfo((ImageInfo *) NULL);
  CloneString(&image_info->font, WMF_FONT_PSNAME(font));
  image_info->pointsize = 12;
  GetDrawInfo(image_info, &draw_info);
  CloneString(&draw_info.text, str);
  image->x_resolution = 72;
  image->y_resolution = 72;
  image->units = PixelsPerInchResolution;

  if (GetTypeMetrics(image, &draw_info, &metrics) != False)
    stringwidth = ((metrics.width * 72)/(image->x_resolution * image_info->pointsize)); /* *0.916348; */

#if 0
  printf("\nlite_font_stringwidth\n");
  printf("string                  = \"%s\"\n", str);
  printf("WMF_FONT_NAME           = \"%s\"\n", WMF_FONT_NAME(font));
  printf("WMF_FONT_PSNAME         = \"%s\"\n", WMF_FONT_PSNAME(font));
  printf("stringwidth             = %.4g\n", stringwidth);
  /* printf("WMF_FONT_HEIGHT         = %i\n", (int)WMF_FONT_HEIGHT(font)); */
  /* printf("WMF_FONT_WIDTH          = %i\n", (int)WMF_FONT_WIDTH(font)); */
  fflush(stdout);
#endif

  image->x_resolution = orig_x_resolution;
  image->y_resolution = orig_y_resolution;
  image->units = orig_resolution_units;

  return 0;
}

/* Map font (similar to wmf_ipa_font_map) */

/* Mappings to Postscript fonts: family, normal, italic, bold, bolditalic */
static wmfFontMap WMFFontMap[] = {
  { "Courier",            "Courier",     "Courier-Oblique",   "Courier-Bold",   "Courier-BoldOblique"   },
  { "Helvetica",          "Helvetica",   "Helvetica-Oblique", "Helvetica-Bold", "Helvetica-BoldOblique" },
  { "Modern",             "Courier",     "Courier-Oblique",   "Courier-Bold",   "Courier-BoldOblique"   },
  { "Monotype Corsiva",   "Courier",     "Courier-Oblique",   "Courier-Bold",   "Courier-BoldOblique"   },
  { "News Gothic",        "Helvetica",   "Helvetica-Oblique", "Helvetica-Bold", "Helvetica-BoldOblique" },
  { "Symbol",             "Symbol",      "Symbol",            "Symbol",         "Symbol"                },
  { "System",             "Courier",     "Courier-Oblique",   "Courier-Bold",   "Courier-BoldOblique"   },
  { "Times",              "Times-Roman", "Times-Italic",      "Times-Bold",     "Times-BoldItalic"      },
  {  NULL,       NULL,          NULL,                NULL,             NULL                   }
};

/* Mapping between base name and Ghostscript family name */
static wmfMapping SubFontMap[] = {
  { "Arial",    "Helvetica" },
  { "Courier",    "Courier"   },
  { "Fixed",    "Courier"   },
  { "Helvetica",  "Helvetica" },
  { "Sans",    "Helvetica" },
  { "Sym",    "Symbol"    },
  { "Terminal",    "Courier"   },
  { "Times",    "Times"     },
  { "Wingdings",  "Symbol"    },
  {  NULL,           NULL       }
};

static void lite_font_map( wmfAPI* API, wmfFont* font)
{
  wmfFontData
    *font_data;

  wmf_magick_font_t
    *magick_font;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  ExceptionInfo
    exception;

  const TypeInfo
    *type_info,
    *type_info_base;

  const char
    *wmf_font_name;

  if (font == 0)
    return;

  font_data = (wmfFontData*)API->font_data;
  font->user_data = font_data->user_data;
  magick_font = (wmf_magick_font_t*)font->user_data;
  wmf_font_name = WMF_FONT_NAME(font);

  LiberateMemory((void**)&magick_font->ps_name);

  GetExceptionInfo(&exception);
  type_info_base=GetTypeInfo("*",&exception);
  if(type_info_base == 0)
    {
      ThrowException(&ddata->image->exception,exception.severity,
                     exception.reason,exception.description);
      return;
    }

  /* Certain short-hand font names are not the proper Windows names
     and should be promoted to the proper names */
  if(LocaleCompare(wmf_font_name,"Times") == 0)
    wmf_font_name = "Times New Roman";
  else if(LocaleCompare(wmf_font_name,"Courier") == 0)
    wmf_font_name = "Courier New";

  /* Look for a family-based best-match */
  if(!magick_font->ps_name)
    {
      int
        target_weight,
        best_weight = 0;

      if( WMF_FONT_WEIGHT(font) == 0 )
        target_weight = 400;
      else
        target_weight = WMF_FONT_WEIGHT(font);

      /* printf("Desired weight  = %i\n", WMF_FONT_WEIGHT(font)); */
      for ( type_info=type_info_base; type_info != 0; type_info=type_info->next )
        {
          if(LocaleCompare(wmf_font_name,type_info->family) == 0)
            {
              int
                weight;

              /* printf("Considering font %s\n", type_info->description); */
              
              if( WMF_FONT_ITALIC(font) && !(strstr(type_info->description,"Italic") ||
                                             strstr(type_info->description,"Oblique")) )
                continue;

              weight = util_font_weight( type_info->description );
              /* printf("Estimated weight =  %.4g\n", weight); */

              if( abs(weight - target_weight) < abs(best_weight - target_weight) )
                {
                  best_weight = weight;
                  CloneString(&magick_font->ps_name,type_info->name);
                }
            }
        }
    }

  /* Look for exact full match */
  if(!magick_font->ps_name)
    {
      for ( type_info=type_info_base; type_info != 0; type_info=type_info->next)
        {
          if(LocaleCompare(wmf_font_name,type_info->description) == 0)
            {
              CloneString(&magick_font->ps_name,type_info->name);
              break;
            }
        }
    }

  /* Now let's try simple substitution mappings from WMFFontMap */
  if(!magick_font->ps_name)
    {
      char
        target[MaxTextExtent];

      int
        target_weight = 400,
        want_italic = False,
        want_bold = False,
        i;

      if( WMF_FONT_WEIGHT(font) != 0 )
        target_weight = WMF_FONT_WEIGHT(font);

      if( (target_weight > 550) || ((strstr(wmf_font_name,"Bold") ||
                                     strstr(wmf_font_name,"Heavy") ||
                                     strstr(wmf_font_name,"Black"))) )
        want_bold = True;

      if( (WMF_FONT_ITALIC(font)) || ((strstr(wmf_font_name,"Italic") ||
                                       strstr(wmf_font_name,"Oblique"))) )
        want_italic = True;

      strcpy(target,"Times");
      for( i=0; SubFontMap[i].name != NULL; i++ )
        {
          if(LocaleCompare(wmf_font_name, SubFontMap[i].name) == 0)
            {
              strcpy(target,SubFontMap[i].mapping);
              break;
            }
        }

      for( i=0; WMFFontMap[i].name != NULL; i++ )
        {
          if(LocaleNCompare(WMFFontMap[i].name,target,strlen(WMFFontMap[i].name)) == 0)
            {
              if(want_bold && want_italic)
                CloneString(&magick_font->ps_name,WMFFontMap[i].bolditalic);
              else if(want_italic)
                CloneString(&magick_font->ps_name,WMFFontMap[i].italic);
              else if(want_bold)
                CloneString(&magick_font->ps_name,WMFFontMap[i].bold);
              else
                CloneString(&magick_font->ps_name,WMFFontMap[i].normal);
            }
        }
    }

#if 0
  printf("\nlite_font_map\n");
  printf("WMF_FONT_NAME           = \"%s\"\n", WMF_FONT_NAME(font));
  printf("WMF_FONT_WEIGHT         = %i\n",  WMF_FONT_WEIGHT(font));
  printf("WMF_FONT_PSNAME         = \"%s\"\n", WMF_FONT_PSNAME(font));
  fflush(stdout);
#endif
  
}

/* Initialize API font structures */
static void lite_font_init( wmfAPI* API, wmfAPI_Options* options)
{
  wmfFontData
    *font_data;

  API->fonts = 0;

  /* Allocate wmfFontData data structure */
  API->font_data = wmf_malloc(API,sizeof(wmfFontData));
  if (ERR (API))
    return;

  font_data = (wmfFontData*)API->font_data;

  /* Assign function to map font (type wmfMap) */
  font_data->map = lite_font_map;

  /* Assign function to return string width in points (type wmfStringWidth) */
  font_data->stringwidth = lite_font_stringwidth;

  /* Assign user data, not used by libwmflite (type void*) */
  font_data->user_data = wmf_malloc(API,sizeof(wmf_magick_font_t));
  if(ERR(API))
    return;
  ((wmf_magick_font_t*)font_data->user_data)->ps_name = 0;
  ((wmf_magick_font_t*)font_data->user_data)->pointsize = 0;
}

#endif /* HasWMFlite */

/* BLOB read byte */
static int ipa_blob_read(void* context)
{
  return ReadBlobByte((Image*)context);
}

/* BLOB seek */
static int ipa_blob_seek(void* context,long position)
{
  return (int)SeekBlob((Image*)context,(off_t)position,SEEK_SET);
}

/* BLOB tell */
static long ipa_blob_tell(void* context)
{
  return (long)TellBlob((Image*)context);
}

static Image *ReadWMFImage(const ImageInfo * image_info, ExceptionInfo * exception)
{
  Image
    *image;

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
    bbox;

  image = AllocateImage(image_info);
  if (!OpenBlob(image_info,image,ReadBinaryType,exception))
    ThrowReaderException(FileOpenError,"Unable to open file",image);

  /*
   * Create WMF API
   *
   */

  /* Register callbacks */
  wmf_options_flags |= WMF_OPT_FUNCTION;
  memset(&wmf_api_options, 0, sizeof(wmf_api_options));
  wmf_api_options.function = ipa_functions;

  /* Ignore non-fatal errors */
  wmf_options_flags |= WMF_OPT_IGNORE_NONFATAL;

  wmf_error = wmf_api_create(&API, wmf_options_flags, &wmf_api_options);
  if (wmf_error != wmf_E_None)
    {
      if (API)
        wmf_api_destroy(API);
      ThrowReaderException(DelegateFatalError, "Failed to intialize libwmf", image);
    }

  ddata = WMF_MAGICK_GetData(API);
  ddata->image = image;
  ddata->image_info = image_info;

#if defined(HasWMFlite)
  /* Must initialize font subystem for WMFlite interface */

  lite_font_init (API,&wmf_api_options); /* similar to wmf_ipa_font_init in src/font.c */
  /* wmf_arg_fontdirs (API,options); */ /* similar to wmf_arg_fontdirs in src/wmf.c */

#endif

  /*
   * Open BLOB input via libwmf API
   *
   */
  wmf_error = wmf_bbuf_input(API,ipa_blob_read,ipa_blob_seek,
                             ipa_blob_tell,(void*)image);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      ThrowReaderException(FileOpenFatalError, "Unable to open file", image);
    }

  /*
   * Scan WMF file
   *
   */
  wmf_error = wmf_scan(API, 0, &bbox);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      ThrowReaderException(CorruptImageFatalError, "Failed to scan file", image);
    }

  /*
   * Compute dimensions and scale factors
   *
   */

  ddata->bbox = bbox;

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
      ThrowReaderException(CorruptImageFatalError,
                           "Failed to compute output size", image);
    }

  /* Obtain (or guess) metafile units */
  if ((API)->File->placeable)
    units_per_inch = (API)->File->pmh->Inch;
  else if( (wmf_width*wmf_height) < 1024*1024)
    units_per_inch = POINTS_PER_INCH;  /* MM_TEXT */
  else
    units_per_inch = TWIPS_PER_INCH;  /* MM_TWIPS */

  /* Calculate image width and height based on specified DPI
     resolution */
  image_width_inch  = (double) wmf_width / units_per_inch;
  image_height_inch = (double) wmf_height / units_per_inch;
  image_width       = image_width_inch * resolution_x;
  image_height      = image_height_inch * resolution_y;

  /* Compute bounding box scale factors and origin translations
   *
   * This is all just a hack since libwmf does not currently seem to
   * provide the mapping between LOGICAL coordinates and DEVICE
   * coordinates. This mapping is necessary in order to know
   * where to place the logical bounding box within the image.
   *
   */

  bounding_width  = bbox.BR.x - bbox.TL.x;
  bounding_height = bbox.BR.y - bbox.TL.y;

  ddata->scale_x = image_width/bounding_width;
  ddata->translate_x = 0-bbox.TL.x;
  ddata->rotate = 0;

  /* Heuristic: guess that if the vertical coordinates mostly span
     negative values, then the image must be inverted. */
  if( AbsoluteValue(bbox.BR.y) > AbsoluteValue(bbox.TL.y) )
    {
      /* Normal (Origin at top left of image) */
      ddata->scale_y = (image_height/bounding_height);
      ddata->translate_y = 0-bbox.TL.y;
    }
  else
    {
      /* Inverted (Origin at bottom left of image) */
      ddata->scale_y = (-image_height/bounding_height);
      ddata->translate_y = 0-bbox.BR.y;
    }

#if 0
  printf("\nPlaceable metafile:          ");
  if ((API)->File->placeable)
    printf("Yes\n");
  else
    printf("No\n");
  printf("Size in metafile units:      %.4gx%.4g\n", wmf_width, wmf_height);
  printf("Metafile units/inch:         %.4g\n", units_per_inch);
  printf("Size in inches:              %.4gx%.4g\n",image_width_inch,image_height_inch);
  printf("Bounding Box:                %.4g,%.4g %.4g,%.4g\n",
         bbox.TL.x, bbox.TL.y, bbox.BR.x, bbox.BR.y);
  printf("Bounding width x height:     %.4gx%.4g\n", bounding_width, bounding_height);
  printf("Output resolution:           %.4gx%.4g\n", resolution_x, resolution_y);
  printf("Image size:                  %.4gx%.4g\n", image_width, image_height);
  printf("Bounding box scale factor:   %.4g,%.4g\n",
         ddata->scale_x, ddata->scale_y);
  printf("Translation:                 %.4g,%.4g\n",
   ddata->translate_x, ddata->translate_y);


#if 0
  {
    typedef struct _wmfPlayer_t wmfPlayer_t;
    struct _wmfPlayer_t
    {
      wmfPen   default_pen;
      wmfBrush default_brush;
      wmfFont  default_font;
      
      wmfDC* dc; /* current dc */
    };

    wmfDC
      *dc;

#define WMF_ELICIT_DC(API) (((wmfPlayer_t*)((API)->player_data))->dc)

    dc = WMF_ELICIT_DC(API);

    printf("dc->Window.Ox     = %d\n", dc->Window.Ox);
    printf("dc->Window.Oy     = %d\n", dc->Window.Oy);
    printf("dc->Window.width  = %d\n", dc->Window.width);
    printf("dc->Window.height = %d\n", dc->Window.height);
    printf("dc->pixel_width   = %.4g\n", dc->pixel_width);
    printf("dc->pixel_height  = %.4g\n", dc->pixel_height);
#if defined(HasWMFlite)  /* Only in libwmf 0.3 */
    printf("dc->Ox            = %.d\n", dc->Ox);
    printf("dc->Oy            = %.d\n", dc->Oy);
    printf("dc->width         = %.d\n", dc->width);
    printf("dc->height        = %.d\n", dc->height);
#endif

  }
#endif

#endif

  /*
   * Create canvas image
   *
   */

  image->rows = (unsigned long)ceil(image_height);
  image->columns = (unsigned long)ceil(image_width);

  if (image_info->ping)
    {
      wmf_api_destroy(API);
      CloseBlob(image);
      return(image);
    }

  /*
   * Set solid background color
   */
  {
    unsigned long
      column,
      row;

    PixelPacket
      *pixel,
      background_color;

    background_color = image_info->background_color;
    image->background_color = background_color;
    if(background_color.opacity != OpaqueOpacity)
      image->matte = True;

    for (row=0; row < image->rows; row++)
      {
        pixel=SetImagePixels(image,0,row,image->columns,1);
        if (pixel == (PixelPacket *) NULL)
          break;
        for (column=image->columns; column; column--)
          *pixel++ = background_color;
        if (!SyncImagePixels(image))
          break;
      }
  }
  /*
   * Play file to generate MVG drawing commands
   *
   */
  ddata->mvg = NULL;
  wmf_error = wmf_play(API, 0, &bbox);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      ThrowReaderException(CorruptImageFatalError, "Failed to render file", image);
    }

  /*
   * Scribble on canvas image
   *
   */
  
  DrawRender(ddata->draw_context);

  /* Cleanup allocated data */
  wmf_api_destroy(API);
  CloseBlob(image);

  /* Return image */
  return image;
}
/* #endif */
#endif /* HasWMF || HasWMFlite */

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
#if defined(HasWMF) || defined(HasWMFlite)
  MagickInfo
    *entry;

  entry = SetMagickInfo("WMF");
  entry->decoder = ReadWMFImage;
  entry->description = AcquireString("Windows Meta File");
  entry->blob_support = False;
  entry->module = AcquireString("WMF");
  (void) RegisterMagickInfo(entry);
#if defined(HasWIN32WMFAPI)
  entry = SetMagickInfo("EMF");
  entry->decoder = ReadEMFImage;
  entry->description = AcquireString("Windows WIN32 API rendered Enhanced Meta File");
  entry->blob_support = False;
  entry->module = AcquireString("WMF");
  (void) RegisterMagickInfo(entry);
  entry = SetMagickInfo("WMFWIN32");
  entry->decoder = ReadEMFImage;
  entry->description = AcquireString("Windows WIN32 API rendered Meta File");
  entry->blob_support = False;
  entry->module = AcquireString("WMFWIN32");
  (void) RegisterMagickInfo(entry);
#endif
#endif /* HasWMF || HasWMFlite */
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
#if defined(HasWMF) || defined(HasWMFlite)
  (void) UnregisterMagickInfo("WMF");
#if defined(HasWIN32WMFAPI)
  (void) UnregisterMagickInfo("EMF");
  (void) UnregisterMagickInfo("WMFWIN32");
#endif
#endif /* defined(HasWMF) */
}
