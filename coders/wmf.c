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

/*
need drawing commands for:
  decorate
  fill 'url(
  fill-rule
  stroke 'url(
  stroke-antialias
  stroke-dasharray ?
  stroke-linecap
  stroke-linejoin
  stroke-width
 */

static void         draw_clip_pop(wmfAPI* API);
static void         draw_clip_push(wmfAPI* API, unsigned long id);
static void         draw_color_fill_pixelpacket(wmfAPI* API, const PixelPacket* color);
static void         draw_color_fill_reset(wmfAPI* API);
static void         draw_color_fill_rgb(wmfAPI* API, const wmfRGB* rgb);
static void         draw_color_stroke_reset( wmfAPI* API );
static void         draw_color_stroke_rgb(wmfAPI* API, const wmfRGB* rgb);
static void         draw_context_pop(wmfAPI* API);
static void         draw_context_push(wmfAPI* API);
static void         draw_defs_pop(wmfAPI* API);
static void         draw_defs_push(wmfAPI* API);
static void         draw_pattern_pop(wmfAPI* API);
static void         draw_pattern_push(wmfAPI* API, unsigned long id, unsigned long columns, unsigned long rows);
static void         draw_rectangle(wmfAPI* API, double x1, double y1, double x2, double y2);
static void         draw_round_rectangle(wmfAPI* API, double x1, double y1, double x2, double y2, double rx, double ry);
static void         draw_scale(wmfAPI* API, double x, double y);
static void         draw_translate(wmfAPI* API, double x, double y);
static void         draw_rotate(wmfAPI* API, double degrees);
static void         draw_viewbox(wmfAPI* API, unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2);
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
static int          util_append_mvg(wmfAPI * API, char *format, ...);
static void         util_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc,magick_arc_t finish);
static int          util_font_weight( const char* font );
static double       util_pointsize( wmfAPI* API, wmfFont* font, char* str, double font_height);
static void         util_render_mvg(wmfAPI * API);
static void         util_set_brush(wmfAPI * API, wmfDC * dc, const BrushApply brush_apply);
static void         util_set_pen(wmfAPI * API, wmfDC * dc);

/* Set fill color */
static void draw_color_fill_rgb( wmfAPI* API, const wmfRGB* rgb )
{
  util_append_mvg(API, "fill #%02x%02x%02x\n",
                  (int) rgb->r, (int) rgb->g, (int) rgb->b);
}
static void draw_color_fill_pixelpacket( wmfAPI* API, const PixelPacket* color )
{
  if(color->opacity == OpaqueOpacity)
    util_append_mvg(API,
#if QuantumDepth == 8
                    "fill #%02x%02x%02x\n",
#elif QuantumDepth == 16
                    "fill #%04x%04x%04x\n",
#endif
                    color->red, color->green, color->blue);
  else
    util_append_mvg(API,
#if QuantumDepth == 8
                    "fill #%02x%02x%02x%02x\n",
#elif QuantumDepth == 16
                    "fill #%04x%04x%04x%04x\n",
#endif
                    color->red, color->green, color->blue, color->opacity);
}

/* Reset fill color */
static void draw_color_fill_reset( wmfAPI* API )
{
  util_append_mvg(API, "fill none\n");
}

/* Set stroke color */
static void draw_color_stroke_rgb( wmfAPI* API, const wmfRGB* rgb )
{
  util_append_mvg(API, "stroke #%02x%02x%02x\n",
        (int) rgb->r, (int) rgb->g, (int) rgb->b);
}

/* Reset stroke color */
static void draw_color_stroke_reset( wmfAPI* API )
{
  util_append_mvg(API, "stroke none\n");
}

static void draw_composite(wmfAPI* API,
                           const CompositeOperator composite_operator,
                           const double x, const double y,
                           const double width, const double height,
                           const Image * image )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  ExceptionInfo
    exception_info;

  ImageInfo
    *image_info;

  Image
    *clone_image;

  char
    *media_type = NULL,
    *base64 = NULL;

  const char
    *mode = NULL;

  unsigned char
    *blob = (unsigned char*)NULL;

  size_t
    blob_length = 2048;

  assert(API != (wmfAPI*)NULL);
  assert(image != (Image *) NULL);
  assert(width != 0);
  assert(height != 0);
  assert(*image->magick != '\0');

  GetExceptionInfo( &exception_info );

  clone_image = CloneImage(image,0,0,True,&exception_info);
  if(!clone_image)
    {
      ThrowException(&ddata->image->exception,exception_info.severity,
                     exception_info.reason,exception_info.description);
      return;
    }
  image_info = CloneImageInfo((const ImageInfo *)NULL);
  blob = (unsigned char*)ImageToBlob( image_info, clone_image, &blob_length, &exception_info );
  DestroyImageList(clone_image);
  if(!blob)
    {
      ThrowException(&ddata->image->exception,exception_info.severity,
                     exception_info.reason,exception_info.description);
      return;
    }

  base64 = Base64Encode(blob,blob_length);
  LiberateMemory((void**)&blob);
  if(!base64)
    {
      char
        buffer[MaxTextExtent];

      FormatString(buffer,"%d bytes", (4*blob_length/3+4));
      ThrowException(&ddata->image->exception,ResourceLimitWarning,
                     "allocating Base64 memory",buffer);
      return;
    }

  mode = "copy";
  switch (composite_operator)
    {
    case AddCompositeOp:
      mode = "add";
      break;
    case AtopCompositeOp:
      mode = "atop";
      break;
    case BumpmapCompositeOp:
      mode = "bumpmap";
      break;
    case ClearCompositeOp:
      mode = "clear";
      break;
    case ColorizeCompositeOp:
      mode = "colorize_not_supported";
      break;
    case CopyBlueCompositeOp:
      mode = "copyblue";
      break;
    case CopyCompositeOp:
      mode = "copy";
      break;
    case CopyGreenCompositeOp:
      mode = "copygreen";
      break;
    case CopyOpacityCompositeOp:
      mode = "copyopacity";
      break;
    case CopyRedCompositeOp:
      mode = "copyred";
      break;
    case DarkenCompositeOp:
      mode = "darken_not_supported";
      break;
    case DifferenceCompositeOp:
      mode = "difference";
      break;
    case DisplaceCompositeOp:
      mode = "displace_not_supported";
      break;
    case DissolveCompositeOp:
      mode = "dissolve_not_supported";
      break;
    case HueCompositeOp:
      mode = "hue_not_supported";
      break;
    case InCompositeOp:
      mode = "in";
      break;
    case LightenCompositeOp:
      mode = "lighten_not_supported";
      break;
    case LuminizeCompositeOp:
      mode = "luminize_not_supported";
      break;
    case MinusCompositeOp:
      mode = "minus";
      break;
    case ModulateCompositeOp:
      mode = "modulate_not_supported";
      break;
    case MultiplyCompositeOp:
      mode = "multiply";
      break;
    case NoCompositeOp:
      mode = "no_not_supported";
      break;
    case OutCompositeOp:
      mode = "out";
      break;
    case OverCompositeOp:
      mode = "over";
      break;
    case OverlayCompositeOp:
      mode = "overlay_not_supported";
      break;
    case PlusCompositeOp:
      mode = "plus";
      break;
    case SaturateCompositeOp:
      mode = "saturate_not_supported";
      break;
    case ScreenCompositeOp:
      mode = "screen_not_supported";
      break;
    case SubtractCompositeOp:
      mode = "subtract";
      break;
    case ThresholdCompositeOp:
      mode = "threshold";
      break;
    case XorCompositeOp:
      mode = "xor";
      break;
    default:
      break;
    }

  media_type = MagickToMime( clone_image->magick );

  if( media_type != NULL )
    {
      long
        remaining;

      char
        *str;

      util_append_mvg(API, "image %s %.4g,%.4g %.4g,%.4g 'data:%s;base64,\n",
                      mode, x, y, width, height, media_type);

      remaining = strlen(base64);
      for( str = base64; remaining > 0; )
        {
          util_append_mvg(API,"%.76s", str);
          remaining -= 76;
          str += 76;
          if(remaining > 0)
            util_append_mvg(API,"\n");
        }

      util_append_mvg(API,"'\n");
    }

  LiberateMemory((void**)&media_type);
  DestroyExceptionInfo(&exception_info);
  DestroyImageInfo(image_info);
}

static void draw_clip_pop( wmfAPI* API )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);
  
  ddata->push_depth--;
  util_append_mvg(API, "pop clip-path\n");
}

static void draw_clip_push( wmfAPI* API, unsigned long id )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);
 
  util_append_mvg(API, "push clip-path clip_%lu\n", id);
  ddata->push_depth++;
}

/* Pop graphic context */
static void draw_context_pop( wmfAPI* API )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);
  
  ddata->push_depth--;
  util_append_mvg(API, "pop graphic-context\n");
}

/* Push graphic context */
static void draw_context_push( wmfAPI* API )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);
 
  util_append_mvg(API, "push graphic-context\n");
  ddata->push_depth++;
}

static void draw_defs_pop( wmfAPI* API )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);
  
  ddata->push_depth--;
  util_append_mvg(API, "pop defs\n");
}

static void draw_defs_push( wmfAPI* API )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);
 
  util_append_mvg(API, "push defs\n");
  ddata->push_depth++;
}

static void draw_pattern_pop( wmfAPI* API )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);
  
  ddata->push_depth--;
  util_append_mvg(API, "pop pattern\n");
}

static void draw_pattern_push( wmfAPI* API,
                               unsigned long id,
                               unsigned long columns,
                               unsigned long rows )
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);
 
  util_append_mvg(API, "push pattern brush_%lu 0,0, %lu,%lu\n",
                  id, columns, rows);
  ddata->push_depth++;
}

static void draw_rectangle( wmfAPI* API, double x1, double y1, double x2, double y2 )
{
  util_append_mvg(API, "rectangle %.4g,%.4g %.4g,%.4g\n", x1,y1, x2,y2);
}

static void draw_round_rectangle( wmfAPI* API, double x1, double y1, double x2, double y2, double rx, double ry )
{
  util_append_mvg(API, "roundrectangle %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
                  x1,y1, x2,y2, rx,ry );
}

static void draw_rotate(wmfAPI* API, double degrees)
{
  util_append_mvg(API, "rotate %.4g\n", degrees);
}

static void draw_scale(wmfAPI* API, double x, double y)
{
  util_append_mvg(API, "scale %.4g,%.4g\n", x, y);
}

static void  draw_translate(wmfAPI* API, double x, double y)
{
  util_append_mvg(API, "translate %.4g,%.4g\n", x, y);
}

static void draw_viewbox(wmfAPI* API, unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2)
{
  util_append_mvg(API, "viewbox %lu %lu %lu %lu\n", x1,y1, x2,y2);
}

static void ipa_rop_draw(wmfAPI * API, wmfROP_Draw_t * rop_draw)
{
  if (!TO_FILL(rop_draw))
    return;

  /* Save graphic context */
  draw_context_push(API);

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

  draw_rectangle(API,
                 XC(rop_draw->TL.x), YC(rop_draw->TL.y),
                 XC(rop_draw->BR.x), YC(rop_draw->BR.y));

  /* Restore graphic context */
  draw_context_pop(API);
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

  draw_composite(API, CopyCompositeOp, XC(bmp_draw->pt.x), YC(bmp_draw->pt.y),
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
  wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

  ddata->pattern_id = 0;
  ddata->clipping = False;
  ddata->clip_path_id = 0;

  ddata->push_depth = 0;

  ddata->mvg = 0;
  ddata->mvg_alloc = 0;
  ddata->mvg_length = 0;
}

/*
  This is called by wmf_api_destroy()
 */
static void ipa_device_close(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  LiberateMemory((void **)&ddata->mvg);
}

/*
  This is called from the beginning of each play for initial page setup
 */
static void ipa_device_begin(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Make SVG output happy */
  draw_context_push(API);

  draw_viewbox( API, 0, 0, ddata->image->columns, ddata->image->rows );

  util_append_mvg(API, "#Created by ImageMagick %s (http://www.imagemagick.org/)\n",
                  MagickLibVersionText);

  /* Scale width and height to image */
  draw_scale(API, ddata->scale_x, ddata->scale_y);

  /* Translate to TL corner of bounding box */
  draw_translate(API, ddata->translate_x, ddata->translate_y);

  /* Apply rotation */
  draw_rotate(API, ddata->rotate);

  if(ddata->image_info->texture == NULL)
    {
      /* Draw rectangle in background color */
      draw_color_fill_pixelpacket(API,&ddata->image->background_color);
      draw_rectangle(API,
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
          draw_defs_push(API);
          draw_pattern_push(API, ddata->pattern_id, image->columns, image->rows);
          draw_composite(API, CopyCompositeOp, 0, 0, image->columns, image->rows, image);
          draw_pattern_pop(API);
          draw_defs_pop(API);
          util_append_mvg(API, "fill url(#brush_%lu)\n", ddata->pattern_id);
          ++ddata->pattern_id;

          draw_rectangle(API,
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

  util_append_mvg(API, "fill-opacity 1\n");
  draw_color_fill_reset(API);
  draw_color_stroke_reset(API);
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
    draw_context_pop(API);
  ddata->clipping = False;

  /* Make SVG output happy */
  draw_context_pop(API);
}

static void ipa_flood_interior(wmfAPI * API, wmfFlood_t * flood)
{
  /* Save graphic context */
  draw_context_push(API);

  draw_color_fill_rgb(API,&(flood->color));

  util_append_mvg(API, "color %.4g,%.4g filltoborder\n",
                  XC(flood->pt.x), YC(flood->pt.y));

  /* Restore graphic context */
  draw_context_pop(API);
}

static void ipa_flood_exterior(wmfAPI * API, wmfFlood_t * flood)
{
  /* Save graphic context */
  draw_context_push(API);

  draw_color_fill_rgb(API,&(flood->color));

  if (flood->type == FLOODFILLSURFACE)
    util_append_mvg(API, "color %.4g,%.4g floodfill\n",
                    XC(flood->pt.x),YC(flood->pt.y));
  else
    util_append_mvg(API, "color %.4g,%.4g filltoborder\n",
                    XC(flood->pt.x), YC(flood->pt.y));

  /* Restore graphic context */
  draw_context_pop(API);
}

static void ipa_draw_pixel(wmfAPI * API, wmfDrawPixel_t * draw_pixel)
{
  /* Save graphic context */
  draw_context_push(API);

  draw_color_stroke_reset(API);

  draw_color_fill_rgb(API,&(draw_pixel->color));

  draw_rectangle(API,
                 XC(draw_pixel->pt.x),
                 YC(draw_pixel->pt.y),
                 XC(draw_pixel->pt.x + draw_pixel->pixel_width),
                 YC(draw_pixel->pt.y + draw_pixel->pixel_height));

  /* Restore graphic context */
  draw_context_pop(API);
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
  draw_context_push(API);

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
        draw_color_fill_reset(API);
      else
        util_set_brush(API, draw_arc->dc, BrushApplyFill);

      if (finish == magick_arc_ellipse)
        util_append_mvg(API, "ellipse %.4g,%.4g %.4g,%.4g 0,360\n",
                        XC(O.x), YC(O.y), Rx, Ry);
      else if (finish == magick_arc_pie)
        util_append_mvg(API, "ellipse %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
                        XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
      else if (finish == magick_arc_chord)
        {
          util_append_mvg(API, "arc %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
                          XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
          util_append_mvg(API, "line %.4g,%.4g %.4g,%.4g\n",
                          XC(start.x), YC(start.y), XC(end.x), YC(end.y));
        }
      else      /* if (finish == magick_arc_open) */
        util_append_mvg(API, "arc %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
                        XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
    }

  /* Restore graphic context */
  draw_context_pop(API);
}

static void ipa_draw_line(wmfAPI * API, wmfDrawLine_t * draw_line)
{
  /* Save graphic context */
  draw_context_push(API);

  if (TO_DRAW(draw_line))
    {
      util_set_pen(API, draw_line->dc);
      util_append_mvg(API, "line %.4g,%.4g %.4g,%.4g\n",
                      XC(draw_line->from.x), YC(draw_line->from.y),
                      XC(draw_line->to.x), YC(draw_line->to.y));
    }

  /* Restore graphic context */
  draw_context_pop(API);
}

static void ipa_poly_line(wmfAPI * API, wmfPolyLine_t * poly_line)
{
  U16
    i;

  /* Save graphic context */
  draw_context_push(API);

  if (poly_line->count <= 1)
    return;

  if (TO_DRAW(poly_line))
    {
      draw_color_fill_reset(API);
      util_set_pen(API, poly_line->dc);

      util_append_mvg(API, "polyline");

      for (i = 0; i < poly_line->count; i++)
        {
          util_append_mvg(API, " %.4g,%.4g",
                          XC(poly_line->pt[i].x),
                          YC(poly_line->pt[i].y));
        }

      util_append_mvg(API, "\n");
    }

  /* Restore graphic context */
  draw_context_pop(API);
}

static void ipa_draw_polygon(wmfAPI * API, wmfPolyLine_t * poly_line)
{
  U16
    i;

  /* Save graphic context */
  draw_context_push(API);

  if (poly_line->count <= 2)
    return;

  if (TO_FILL(poly_line) || TO_DRAW(poly_line))
    {
      util_set_pen(API, poly_line->dc);
      util_set_brush(API, poly_line->dc, BrushApplyFill);

      util_append_mvg(API, "polygon");

      for (i = 0; i < poly_line->count; i++)
        {
          util_append_mvg(API, " %.4g,%.4g",
                          XC(poly_line->pt[i].x),
                          YC(poly_line->pt[i].y));
        }

      util_append_mvg(API, "\n");
    }

  /* Restore graphic context */
  draw_context_pop(API);
}

static void ipa_draw_rectangle(wmfAPI * API, wmfDrawRectangle_t * draw_rect)
{
  /* Save graphic context */
  draw_context_push(API);

  if (TO_FILL(draw_rect) || TO_DRAW(draw_rect))
    {
      util_set_pen(API, draw_rect->dc);
      util_set_brush(API, draw_rect->dc, BrushApplyFill);

      if ((draw_rect->width > 0) || (draw_rect->height > 0))
        draw_round_rectangle(API,
                        XC(draw_rect->TL.x), YC(draw_rect->TL.y),
                        XC(draw_rect->BR.x), YC(draw_rect->BR.y),
                        draw_rect->width / 2, draw_rect->height / 2);
      else
        draw_rectangle(API,
                       XC(draw_rect->TL.x), YC(draw_rect->TL.y),
                       XC(draw_rect->BR.x), YC(draw_rect->BR.y));
    }

  /* Restore graphic context */
  draw_context_pop(API);
}

/* Draw an un-filled rectangle using the current brush */
static void ipa_region_frame(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{
  /* Save graphic context */
  draw_context_push(API);

  if (TO_FILL(poly_rect) || TO_DRAW(poly_rect))
    {
      unsigned int
        i;

      draw_color_fill_reset(API);
      util_set_brush(API, poly_rect->dc, BrushApplyStroke);

      for (i = 0; i < poly_rect->count; i++)
        {
          draw_rectangle(API,
                         XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                         XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
        }
    }

  /* Restore graphic context */
  draw_context_pop(API);
}

static void ipa_region_paint(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{

  if (poly_rect->count == 0)
    return;

  /* Save graphic context */
  draw_context_push(API);

  if (TO_FILL (poly_rect))
    {
      unsigned int
        i;

      draw_color_stroke_reset(API);
      util_set_brush(API, poly_rect->dc, BrushApplyFill);

      for (i = 0; i < poly_rect->count; i++)
        {
          draw_rectangle(API,
                         XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                         XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
        }
    }

  /* Restore graphic context */
  draw_context_pop(API);
}

static void ipa_region_clip(wmfAPI *API, wmfPolyRectangle_t *poly_rect)
{
  unsigned int
    i;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData (API);

  /* Reset any existing clip paths by popping context */
  if(ddata->clipping)
    draw_context_pop(API);
  ddata->clipping = False;

  if(poly_rect->count > 0)
    {

      /* Define clip path */
      ddata->clip_path_id++;
      draw_defs_push(API);
      draw_clip_push(API, ddata->clip_path_id);
      draw_context_push(API);
      for (i = 0; i < poly_rect->count; i++)
        {
          draw_rectangle(API,
                         XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                         XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
        }
      draw_context_pop(API);
      draw_clip_pop(API);
      draw_defs_pop(API);

      /* Push context for new clip paths */
      draw_context_push(API);
      util_append_mvg(API, "clip-path url(#clip_%lu)\n", ddata->clip_path_id);
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
  draw_context_push(API);

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
      draw_color_stroke_reset(API);
      draw_color_fill_rgb(API,WMF_DC_BACKGROUND(draw_text->dc));
      draw_rectangle(API,
                     XC(draw_text->TL.x),YC(draw_text->TL.y),
                     XC(draw_text->BR.x),YC(draw_text->BR.y));
      draw_color_fill_reset(API);
    }
  else
    {
      /* Set text undercolor */
      if (WMF_DC_OPAQUE(draw_text->dc))
        {
          wmfRGB
            *box = WMF_DC_BACKGROUND(draw_text->dc);

          util_append_mvg(API, "decorate #%02x%02x%02x\n",
                          (int) box->r, (int) box->g, (int) box->b);
        }
      else
        util_append_mvg(API, "decorate none\n");
    }

  /* Set text clipping (META_EXTTEXTOUT mode) */
  if( draw_text->flags & ETO_CLIPPED)
    {
    }

  /* Set stroke color */
  draw_color_stroke_reset(API);

  /* Set fill color */
  draw_color_fill_rgb(API,WMF_DC_TEXTCOLOR(draw_text->dc));

  /* Output font size */
  util_append_mvg(API, "font-size %.4g\n", pointsize);

  /* Output Postscript font name */
  util_append_mvg(API, "font '%s'\n", WMF_FONT_PSNAME(font));

  /* Translate coordinates so target is 0,0 */
  draw_translate(API, XC(point.x), YC(point.y));

  /* Transform horizontal scale to draw text at 1:1 ratio */
  draw_scale(API, ddata->scale_y / ddata->scale_x, 1.0);

  /* Apply rotation */
  /* ImageMagick's drawing rotation is clockwise from horizontal
     while WMF drawing rotation is counterclockwise from horizontal */
  angle = AbsoluteValue(RadiansToDegrees(2 * MagickPI - WMF_TEXT_ANGLE(font)));
  if (angle == 360)
    angle = 0;
  if (angle != 0)
    draw_rotate(API, angle);

  /*
   * Render text
   *
   */

  {
    char
      escaped_string[MaxTextExtent];

    unsigned char
      *p,
      *q;

    int string_length;

    /*
     * Build escaped string
     */
    for (p = (unsigned char *) draw_text->str, q = (unsigned char *) escaped_string, string_length = 0;
         *p != 0 && string_length < ((int) sizeof(escaped_string) - 3); ++p)
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
    util_append_mvg(API, "text 0,0 '%.1024s'\n", escaped_string);

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

        draw_rectangle(API,
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

        draw_rectangle(API,
                       XC(ulTL.x), YC(ulTL.y), XC(ulBR.x), YC(ulBR.y));

      }
  }

  /* Restore graphic context */
  draw_context_pop(API);

#if 0
  draw_context_push(API);
  util_append_mvg(API, "stroke red\n");
  draw_color_fill_reset(API);
  draw_rectangle(API,
                 XC(TL.x), YC(TL.y),
                 XC(BR.x), YC(BR.y));
  draw_color_stroke_reset(API);
  draw_context_pop(API);
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
      util_append_mvg(API, "fill-rule nonzero\n");
      break;

    case ALTERNATE:
    default:
      util_append_mvg(API, "fill-rule evenodd\n");
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
          draw_color_stroke_reset(API);
        else
          draw_color_fill_reset(API);
        break;
      }
    case BS_HATCHED /* 2 */:
      /* WMF_BRUSH_COLOR specifies the hatch color, WMF_BRUSH_HATCH
         specifies the hatch brush style. If WMF_DC_OPAQUE, then
         WMF_DC_BACKGROUND specifies hatch background color.  */
      {
        draw_defs_push(API);
        draw_pattern_push(API, ddata->pattern_id, 8, 8);
        draw_context_push(API);
        
        if (WMF_DC_OPAQUE(dc))
          {
            if( brush_apply == BrushApplyStroke )
              draw_color_stroke_rgb(API,WMF_DC_BACKGROUND(dc));
            else
              draw_color_fill_rgb(API,WMF_DC_BACKGROUND(dc));
            util_append_mvg(API, "rectangle 0,0 7,7\n");
          }
        
        util_append_mvg(API, "stroke-antialias 0\n");
        util_append_mvg(API, "stroke-width 1\n");
        
        draw_color_stroke_rgb(API,WMF_BRUSH_COLOR(brush));
        
        switch ((unsigned int) WMF_BRUSH_HATCH(brush))
          {
            
          case HS_HORIZONTAL:  /* ----- */
            {
              util_append_mvg(API, "line 0,3 7,3\n");
              break;
            }
          case HS_VERTICAL:  /* ||||| */
            {
              util_append_mvg(API, "line 3,0 3,7\n");
              break;
            }
          case HS_FDIAGONAL:  /* \\\\\ */
            {
              util_append_mvg(API, "line 0,0 7,7\n");
              break;
            }
          case HS_BDIAGONAL:  /* ///// */
            {
              util_append_mvg(API, "line 0,7 7,0\n");
              break;
            }
          case HS_CROSS:  /* +++++ */
            {
              util_append_mvg(API, "line 0,3 7,3\n");
              util_append_mvg(API, "line 3,0 3,7\n");
              break;
            }
          case HS_DIAGCROSS:  /* xxxxx */
            {
              util_append_mvg(API, "line 0,0 7,7\n");
              util_append_mvg(API, "line 0,7 7,0\n");
              break;
            }
          default:
            {
            }
          }
        draw_context_pop(API);
        draw_pattern_pop(API);
        draw_defs_pop(API);
        if( brush_apply == BrushApplyStroke )
          util_append_mvg(API, "stroke 'url(#brush_%lu)'\n", ddata->pattern_id);
        else
          util_append_mvg(API, "fill 'url(#brush_%lu)'\n", ddata->pattern_id);
        ++ddata->pattern_id;
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

            draw_defs_push(API);
            draw_pattern_push(API, ddata->pattern_id, brush_bmp->width, brush_bmp->height);
            draw_composite(API,mode, 0, 0, brush_bmp->width, brush_bmp->height, image);
            draw_pattern_pop(API);
            draw_defs_pop(API);
            if( brush_apply == BrushApplyStroke )
              util_append_mvg(API, "stroke url(#brush_%lu)\n", ddata->pattern_id);
            else
              util_append_mvg(API, "fill url(#brush_%lu)\n", ddata->pattern_id);
            ++ddata->pattern_id;

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
    draw_color_stroke_reset(API);
    return;
  }

  util_append_mvg(API, "stroke-antialias 1\n");
  util_append_mvg(API, "stroke-width %.4g\n", Max(0, pen_width));

  switch (pen_endcap)
  {
    case PS_ENDCAP_SQUARE:
      util_append_mvg(API, "stroke-linecap square\n");
      break;

    case PS_ENDCAP_ROUND:
      util_append_mvg(API, "stroke-linecap round\n");
      break;

    case PS_ENDCAP_FLAT:
    default:
      util_append_mvg(API, "stroke-linecap butt\n");
      break;
  }

  switch (pen_join)
  {
    case PS_JOIN_BEVEL:
      util_append_mvg(API, "stroke-linejoin bevel\n");
      break;

    case PS_JOIN_ROUND:
      util_append_mvg(API, "stroke-linejoin round\n");
      break;

    case PS_JOIN_MITER:
    default:
      util_append_mvg(API, "stroke-linejoin miter\n");
      break;
  }

  switch (pen_style)
  {
    case PS_DASH:    /* -------  */
      /* Pattern 18,7 */
      util_append_mvg(API, "stroke-antialias 0\n");
      util_append_mvg(API, "stroke-dasharray %.4g,%.4g\n",
      pixel_width * 18, pixel_width * 7);
      break;

    case PS_ALTERNATE:
    case PS_DOT:    /* .......  */
      /* Pattern 3,3 */
      util_append_mvg(API, "stroke-antialias 0\n");
      util_append_mvg(API, "stroke-dasharray %.4g,%.4g\n",
      pixel_width * 3, pixel_width * 3);
      break;

    case PS_DASHDOT:    /* _._._._  */
      /* Pattern 9,6,3,6 */
      util_append_mvg(API, "stroke-antialias 0\n");
      util_append_mvg(API, "stroke-dasharray %.4g,%.4g,%.4g,%.4g\n",
      pixel_width * 9, pixel_width * 6, pixel_width * 3,
      pixel_width * 6);
      break;

    case PS_DASHDOTDOT:  /* _.._.._  */
      /* Pattern 9,3,3,3,3,3 */
      util_append_mvg(API, "stroke-antialias 0\n");
      util_append_mvg(API,
      "stroke-dasharray %.4g,%.4g,%.4g,%.4g,%.4g,%.4g\n",
      pixel_width * 9, pixel_width * 3, pixel_width * 3,
      pixel_width * 3, pixel_width * 3, pixel_width * 3);
      break;

    case PS_INSIDEFRAME:  /* There is nothing to do in this case... */
    case PS_SOLID:
    default:
      util_append_mvg(API, "stroke-dasharray none\n");
      break;
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

/* Extend MVG, printf style */
static int util_append_mvg(wmfAPI * API, char *format, ...)
{
  const size_t
    alloc_size = MaxTextExtent*20; /* 40K */

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Allocate initial memory */
  if(ddata->mvg == 0)
    {
      ddata->mvg = (char*)AcquireMemory(alloc_size);
      ddata->mvg_alloc = alloc_size;
      ddata->mvg_length = 0;
      if(ddata->mvg == 0)
        return -1;
    }

  /* Re-allocate additional memory if necessary (ensure 20K unused) */
  if(ddata->mvg_alloc < (ddata->mvg_length+MaxTextExtent*10))
    {
      size_t
        realloc_size = ddata->mvg_alloc + alloc_size;

      ReacquireMemory((void**)&ddata->mvg,realloc_size);
      if(ddata->mvg == NULL)
        return -1;
      ddata->mvg_alloc = realloc_size;
    }

  /* Write to end of existing MVG string */
  {
    long
      str_length;

    va_list
      argp;

    /* Pretty-print indentation */
    if( *(ddata->mvg+ddata->mvg_length - 1) == '\n' )
      {
        long
          i;

        for( i=ddata->push_depth; i; i--)
          {
            *(ddata->mvg+ddata->mvg_length)=' ';
            ++ddata->mvg_length;
          }
        *(ddata->mvg+ddata->mvg_length)=0;
      }

    va_start(argp, format);
#if !defined(HAVE_VSNPRINTF)
    str_length = vsprintf(ddata->mvg+ddata->mvg_length, format, argp);
#else
    str_length = vsnprintf(ddata->mvg+ddata->mvg_length, ddata->mvg_alloc-ddata->mvg_length-1, format, argp);
#endif
    va_end(argp);

    ddata->mvg_length += str_length;
    *(ddata->mvg+ddata->mvg_length)=0;

    assert(ddata->mvg_length+1<ddata->mvg_alloc);

    return str_length;
  }
}

/* Scribble MVG on image */
static void util_render_mvg(wmfAPI * API)
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
  draw_info->debug = ddata->image_info->debug;
  draw_info->primitive = ddata->mvg;
  /* puts(draw_info->primitive); */
  DrawImage(ddata->image, draw_info);
  draw_info->primitive = (char *) NULL;
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(image_info);
}

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
  util_render_mvg(API);

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
