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
%                            Oct 2001 - Dec 2001                              %
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

#if defined(HasWMF) || defined(HasWMFlite)
#if !defined(WIN32)

#define ERR(API)  ((API)->err != wmf_E_None)
#define XC(x) ((double)x)
#define YC(y) ((double)y)

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

  /* Maximum and current number of temporary images */
  int
    max_temp_image_index,
    cur_temp_image_index;

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

#if defined(HasWMFlite)
static void  magick_font_init (wmfAPI* API, wmfAPI_Options* options);
static void  magick_font_map(wmfAPI* API,wmfFont* font);
static float magick_font_stringwidth(wmfAPI* API, wmfFont* font, char* str);
#endif

/* static const Image* magick_get_registry(wmfAPI * API, const long id); */
static double       magick_font_pointsize( wmfAPI* API, wmfFont* font, char* str, double font_height);
static int          magick_mvg_printf(wmfAPI * API, char *format, ...);
static int          wmf_magick_read(void* context);
static int          wmf_magick_seek(void* context,long position);
static long         magick_add_registry(wmfAPI * API, const Image *image, ExceptionInfo *exception);
static long         wmf_magick_tell(void* context);
static void         magick_brush(wmfAPI * API, wmfDC * dc);
static void         magick_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc,magick_arc_t finish);
static void         magick_pen(wmfAPI * API, wmfDC * dc);
static void         magick_render_mvg(wmfAPI * API);
static void         wmf_magick_bmp_draw(wmfAPI * API, wmfBMP_Draw_t * bmp_draw);
static void         wmf_magick_bmp_free(wmfAPI * API, wmfBMP * bmp);
static void         wmf_magick_bmp_read(wmfAPI * API, wmfBMP_Read_t * bmp_read);
static void         wmf_magick_device_begin(wmfAPI * API);
static void         wmf_magick_device_close(wmfAPI * API);
static void         wmf_magick_device_end(wmfAPI * API);
static void         wmf_magick_device_open(wmfAPI * API);
static void         wmf_magick_draw_arc(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void         wmf_magick_draw_chord(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void         wmf_magick_draw_ellipse(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void         wmf_magick_draw_line(wmfAPI * API, wmfDrawLine_t * draw_line);
static void         wmf_magick_draw_pie(wmfAPI * API, wmfDrawArc_t * draw_arc);
static void         wmf_magick_draw_pixel(wmfAPI * API, wmfDrawPixel_t * draw_pixel);
static void         wmf_magick_draw_polygon(wmfAPI * API, wmfPolyLine_t * poly_line);
static void         wmf_magick_draw_rectangle(wmfAPI * API, wmfDrawRectangle_t * draw_rect);
static void         wmf_magick_draw_text(wmfAPI * API, wmfDrawText_t * draw_text);
static void         wmf_magick_flood_exterior(wmfAPI * API, wmfFlood_t * flood);
static void         wmf_magick_flood_interior(wmfAPI * API, wmfFlood_t * flood);
static void         wmf_magick_function(wmfAPI * API);
static void         wmf_magick_poly_line(wmfAPI * API, wmfPolyLine_t * poly_line);
static void         wmf_magick_region_clip(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void         wmf_magick_region_frame(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void         wmf_magick_region_paint(wmfAPI * API, wmfPolyRectangle_t * poly_rect);
static void         wmf_magick_rop_draw(wmfAPI * API, wmfROP_Draw_t * rop_draw);
static void         wmf_magick_udata_copy(wmfAPI * API, wmfUserData_t * userdata);
static void         wmf_magick_udata_free(wmfAPI * API, wmfUserData_t * userdata);
static void         wmf_magick_udata_init(wmfAPI * API, wmfUserData_t * userdata);
static void         wmf_magick_udata_set(wmfAPI * API, wmfUserData_t * userdata);

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
                    XC(rop_draw->TL.x), YC(rop_draw->TL.y),
                    XC(rop_draw->BR.x), YC(rop_draw->BR.y));

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

/* Register an image with the image registry */
static long magick_add_registry(wmfAPI * API, const Image *image, ExceptionInfo *exception)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  long
    id;

  id = SetMagickRegistry(ImageRegistryType,image,sizeof(Image),exception);

  if( id > -1 )
    {
      (ddata->temp_images)[ddata->cur_temp_image_index] = id;
      ++ddata->cur_temp_image_index;
      if (ddata->cur_temp_image_index == ddata->max_temp_image_index)
        {
          ddata->max_temp_image_index += 2048;
          ReacquireMemory((void **) &ddata->temp_images,
                          ddata->max_temp_image_index * sizeof(long));
        }
    }

  return id;
}

/* Retrieve an image from the image registry */
#if 0
static const Image* magick_get_registry(wmfAPI * API, const long id)
{
  size_t
    length;

  RegistryType
    type;

  return (const Image*)GetMagickRegistry(id,&type,&length);
}
#endif

static void wmf_magick_bmp_draw(wmfAPI *API, wmfBMP_Draw_t *bmp_draw)
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

  long
    id;

  if (bmp_draw->bmp.data == 0)
    return;

  image = (Image *)bmp_draw->bmp.data;

  GetExceptionInfo(&exception);

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

      crop_image = CropImage( image, &crop_info, &exception);
      if(crop_image)
        {
          DestroyImages(image);
          image = crop_image;
        }
      else
        ThrowException(&ddata->image->exception,exception.severity,
                       exception.reason,exception.description);
    }

  width = AbsoluteValue(bmp_draw->pixel_width * (double) bmp_draw->crop.w);
  height = AbsoluteValue(bmp_draw->pixel_height * (double) bmp_draw->crop.h);

  id = magick_add_registry(API, image, &exception);
  if( id > -1 )
    magick_mvg_printf(API, "image Copy %.10g,%.10g %.10g,%.10g 'mpr:%li'\n",
                      XC(bmp_draw->pt.x), YC(bmp_draw->pt.y), width, height, id);
  else
    ThrowException(&ddata->image->exception,exception.severity,
                   exception.reason,exception.description);

#if 0
  printf("bmp_draw->bmp.data   = 0x%lx\n", (long)bmp_draw->bmp.data);
  printf("registry id          = %li\n", id);
  /* printf("pixel_width          = %.10g\n", bmp_draw->pixel_width); */
  /* printf("pixel_height         = %.10g\n", bmp_draw->pixel_height); */
  printf("bmp_draw->bmp WxH    = %ix%i\n", bmp_draw->bmp.width, bmp_draw->bmp.height);
  printf("bmp_draw->crop WxH   = %ix%i\n", bmp_draw->crop.w, bmp_draw->crop.h);
  printf("bmp_draw->crop x,y   = %i,%i\n", bmp_draw->crop.x, bmp_draw->crop.y);
  printf("image size WxH       = %lux%lu\n", image->columns, image->rows);
#endif
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
  printf("wmf_magick_bmp_read: buffer=0x%lx length=%ld, width=%i, height=%i\n",
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
      ThrowException(&ddata->image->exception,CorruptImageWarning,exception.reason,
                     exception.description);
    }
  else
    {
#if 0
      printf("wmf_magick_bmp_read: rows=%ld,columns=%ld\n\n", image->rows, image->columns);
#endif
      bmp_read->bmp.data = (void*)image;
      bmp_read->bmp.width  = (U16)image->columns;
      bmp_read->bmp.height = (U16)image->rows;
    }
}

static void wmf_magick_bmp_free(wmfAPI * API, wmfBMP * bmp)
{
  /*
   * We don't actually free the image here since we need the image
   * to persist until DrawImage() has been executed.
   * The images are freed by wmf_magick_device_close()
   */

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
  ddata->max_temp_image_index = 2048;
  ddata->cur_temp_image_index = 0;
  ddata->temp_images =
    (long *) AcquireMemory(ddata->max_temp_image_index * sizeof(long));
  ddata->pattern_id = 0;
  ddata->clipping = False;

  ddata->mvg = 0;
  ddata->mvg_alloc = 0;
  ddata->mvg_length = 0;
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

    for (index = 0; index < ddata->cur_temp_image_index; index++)
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

  magick_mvg_printf(API, "# Created by ImageMagick %s http://www.imagemagick.org\n",
                    MagickLibVersionText);

  /* Scale width and height to image */
  magick_mvg_printf(API, "scale %.10g,%.10g\n",
		    ddata->scale_x, ddata->scale_y);
  /* Translate to TL corner of bounding box */
  magick_mvg_printf(API, "translate %.10g,%.10g\n",
		    ddata->translate_x,
		    ddata->translate_y);

  /* Apply rotation */
  magick_mvg_printf(API, "rotate %.10g\n", ddata->rotate);

  if(ddata->image_info->texture == NULL)
    {
      /* Draw rectangle in background color */
      if(ddata->image->background_color.opacity == OpaqueOpacity)
        magick_mvg_printf(API,
#if QuantumDepth == 8
                          "fill #%02x%02x%02x\n",
#elif QuantumDepth == 16
                          "fill #%04x%04x%04x\n",
#endif
                          ddata->image->background_color.red,
                          ddata->image->background_color.green,
                          ddata->image->background_color.blue);
      else
        magick_mvg_printf(API,
#if QuantumDepth == 8
                          "fill #%02x%02x%02x%02x\n",
#elif QuantumDepth == 16
                          "fill #%04x%04x%04x%04x\n",
#endif
                          ddata->image->background_color.red,
                          ddata->image->background_color.green,
                          ddata->image->background_color.blue,
                          ddata->image->background_color.opacity);
      magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
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
      
      long
        id;

      GetExceptionInfo(&exception);

      image_info = CloneImageInfo((ImageInfo *) 0);
      strcpy(image_info->filename, ddata->image_info->texture);
      image = ReadImage(image_info,&exception);
      DestroyImageInfo(image_info);
      if(image)
        {
          id = magick_add_registry(API, image, &exception);
          if( id > -1 )
            {
              magick_mvg_printf(API, "push pattern fill_%lu 0,0, %lu,%lu\n",
                                ddata->pattern_id, image->columns, image->rows);
              magick_mvg_printf(API, "image Copy 0,0 %lu,%lu 'mpr:%li'\n",
                                image->columns, image->rows, id);
              magick_mvg_printf(API, "pop pattern\n");
              magick_mvg_printf(API, "fill url(#fill_%lu)\n", ddata->pattern_id);
              ++ddata->pattern_id;

              magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
                                XC(ddata->bbox.TL.x),YC(ddata->bbox.TL.y),
                                XC(ddata->bbox.BR.x),YC(ddata->bbox.BR.y));
            }
        }
      ThrowException(&ddata->image->exception,exception.severity,
                     exception.reason,exception.description);
    }

  magick_mvg_printf(API, "fill-opacity 1\n");
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

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  magick_mvg_printf(API, "fill #%02x%02x%02x\n",
		    (int) rgb->r, (int) rgb->g, (int) rgb->b);

  magick_mvg_printf(API, "color %.10g,%.10g filltoborder\n",
		    XC(flood->pt.x), YC(flood->pt.y));

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_flood_exterior(wmfAPI * API, wmfFlood_t * flood)
{
  wmfRGB
    *rgb = &(flood->color);

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  magick_mvg_printf(API, "fill #%02x%02x%02x\n",
		    (int) rgb->r, (int) rgb->g, (int) rgb->b);

  if (flood->type == FLOODFILLSURFACE)
    magick_mvg_printf(API, "color %.10g,%.10g floodfill\n",
                      XC(flood->pt.x),YC(flood->pt.y));
  else
    magick_mvg_printf(API, "color %.10g,%.10g filltoborder\n",
                      XC(flood->pt.x), YC(flood->pt.y));

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_draw_pixel(wmfAPI * API, wmfDrawPixel_t * draw_pixel)
{
  wmfRGB
    *rgb = &(draw_pixel->color);

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  magick_mvg_printf(API, "stroke none\n");

  magick_mvg_printf(API, "fill-opacity 1\n");

  magick_mvg_printf(API, "fill #%02x%02x%02x\n",
		    (int) rgb->r, (int) rgb->g, (int) rgb->b);

  magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
		    XC(draw_pixel->pt.x),
		    YC(draw_pixel->pt.y),
		    XC(draw_pixel->pt.x + draw_pixel->pixel_width),
		    YC(draw_pixel->pt.y + draw_pixel->pixel_height));

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
			XC(O.x), YC(O.y), Rx, Ry);
    else if (finish == magick_arc_pie)
      magick_mvg_printf(API, "ellipse %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
			XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
    else if (finish == magick_arc_chord)
    {
      magick_mvg_printf(API, "arc %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
			XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
      magick_mvg_printf(API, "line %.10g,%.10g %.10g,%.10g\n",
			XC(start.x), YC(start.y), XC(end.x), YC(end.y));
    }
    else			/* if (finish == magick_arc_open) */
      magick_mvg_printf(API, "arc %.10g,%.10g %.10g,%.10g %.10g,%.10g\n",
			XC(O.x), YC(O.y), Rx, Ry, phi_s, phi_e);
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_draw_line(wmfAPI * API, wmfDrawLine_t * draw_line)
{
  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

  if (TO_DRAW(draw_line))
  {
    magick_pen(API, draw_line->dc);
    magick_mvg_printf(API, "line %.10g,%.10g %.10g,%.10g\n",
		      XC(draw_line->from.x), YC(draw_line->from.y),
		      XC(draw_line->to.x), YC(draw_line->to.y));
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_poly_line(wmfAPI * API, wmfPolyLine_t * poly_line)
{
  U16
    i;

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
      magick_mvg_printf(API, " %.10g,%.10g",
			XC(poly_line->pt[i].x),
			YC(poly_line->pt[i].y));
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
      magick_mvg_printf(API, " %.10g,%.10g",
			XC(poly_line->pt[i].x),
			YC(poly_line->pt[i].y));
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
			XC(draw_rect->TL.x), YC(draw_rect->TL.y),
			XC(draw_rect->BR.x), YC(draw_rect->BR.y),
			draw_rect->width / 2, draw_rect->height / 2);
    else
      magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
			XC(draw_rect->TL.x), YC(draw_rect->TL.y),
			XC(draw_rect->BR.x), YC(draw_rect->BR.y));
  }

  /* Restore graphic context */
  magick_mvg_printf(API, "pop graphic-context\n");
}

static void wmf_magick_region_frame(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{
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
                            XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                            XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
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
                            XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                            XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
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
  ddata->mvg = 0;
  ddata->mvg_alloc = 0;
  ddata->mvg_length = 0;
  ddata->image = 0;
}

static void wmf_magick_draw_text(wmfAPI * API, wmfDrawText_t * draw_text)
{
  double		
    angle = 0,			/* text rotation angle */
    bbox_height,		/* bounding box height */
    bbox_width,			/* bounding box width */
    pointsize = 0;		/* pointsize to output font with desired height */

  TypeMetric
    metrics;

  wmfD_Coord
    BL,				/* bottom left of bounding box */
    BR,				/* bottom right of bounding box */
    TL,				/* top left of bounding box */
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

  /* Convert font_height to equivalent pointsize */
  pointsize = magick_font_pointsize( API, font, draw_text->str, draw_text->font_height);

  /* Save graphic context */
  magick_mvg_printf(API, "push graphic-context\n");

#if 0
  printf("\nwmf_magick_draw_text\n");
  printf("Text                    = \"%s\"\n", draw_text->str);
  /* printf("WMF_FONT_NAME:          = \"%s\"\n", WMF_FONT_NAME(font)); */
  printf("WMF_FONT_PSNAME:        = \"%s\"\n", WMF_FONT_PSNAME(font));
  /* printf("Text box                = %.10gx%.10g\n", bbox_width, bbox_height); */
  /* printf("WMF_FONT_HEIGHT         = %i\n", (int)WMF_FONT_HEIGHT(font)); */
  printf("Pointsize               = %.10g\n", pointsize);
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
      
      double
        text_width;
      
      image_info = CloneImageInfo((ImageInfo *) NULL);
      CloneString(&image_info->font, WMF_FONT_PSNAME(font));
      image_info->pointsize = pointsize;
      GetDrawInfo(image_info, &draw_info);
      CloneString(&draw_info.text, draw_text->str);
      
      if (GetTypeMetrics(image, &draw_info, &metrics) != False)
        {
          /* Center the text if it is not yet centered and should be */
          if ((WMF_DC_TEXTALIGN(draw_text->dc) & TA_CENTER) &&
              (point.x < (BL.x + 1)))
            {
              
              text_width = metrics.width
                * (ddata->scale_y / ddata->scale_x);
              
              point.x += bbox_width / 2 - text_width / 2;
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
  magick_mvg_printf(API, "font '%s'\n", WMF_FONT_PSNAME(font));

  /* Translate coordinates so target is 0,0 */
  magick_mvg_printf(API, "translate %.10g,%.10g\n", XC(point.x), YC(point.y));

  /* Transform horizontal scale to draw text at 1:1 ratio */
  magick_mvg_printf(API, "scale %.10g,%.10g\n",
		    ddata->scale_y / ddata->scale_x, 1.0);

  /* Apply rotation */
  /* ImageMagick's drawing rotation is clockwise from horizontal
     while WMF drawing rotation is counterclockwise from horizontal */
  angle = AbsoluteValue(RadiansToDegrees(2 * MagickPI - WMF_TEXT_ANGLE(font)));
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
          Max(((double) 1 / (ddata->scale_x)),
              ((double) AbsoluteValue(metrics.descent)) * 0.5);
        ulTL.x = 0;
        ulTL.y = AbsoluteValue(metrics.descent) - line_height;
        ulBR.x = metrics.width;
        ulBR.y = AbsoluteValue(metrics.descent);

        magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
                          XC(ulTL.x), YC(ulTL.y), XC(ulBR.x), YC(ulBR.y));

      }

    /* Strikeout text the Windows way */
    if (WMF_TEXT_STRIKEOUT(font))
      {
        double line_height;

        wmfD_Coord
          ulBR,			/* bottom right of strikeout rectangle */
          ulTL;			/* top left of strikeout rectangle */

        line_height =
          Max(((double) 1 / (ddata->scale_x)),
              ((double) AbsoluteValue(metrics.descent)) * 0.5);
        ulTL.x = 0;
        ulTL.y = -(((double) metrics.ascent) / 2 + line_height / 2);
        ulBR.x = metrics.width;
        ulBR.y = -(((double) metrics.ascent) / 2 - line_height / 2);

        magick_mvg_printf(API, "rectangle %.10g,%.10g %.10g,%.10g\n",
                          XC(ulTL.x), YC(ulTL.y), XC(ulBR.x), YC(ulBR.y));

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
#if 0
	/* Set fill opacity 
	 * FIXME: this is probably totally bogus.
	 */
	if (fill_opaque)
          {
            magick_mvg_printf(API, "fill-opacity 1.0\n");
          }
	else
          {
            magick_mvg_printf(API, "fill-opacity 0.5\n"); */	/* semi-transparent?? */
          }
#endif
        magick_mvg_printf(API, "fill #%02x%02x%02x\n",
                          (int) brush_color->r,
                          (int) brush_color->g, (int) brush_color->b);

	break;
      }
    case BS_HATCHED:
      {
	magick_mvg_printf(API, "push pattern fill_%lu 0,0 8,8\n", ddata->pattern_id);
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
	magick_mvg_printf(API, "fill 'url(#fill_%lu)'\n", ddata->pattern_id);
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
            const char
              *mode;

            Image
              *image;

            ExceptionInfo
              exception;
            
            long
              id;

            image = (Image*)brush_bmp->data;

            GetExceptionInfo(&exception);

            mode = "Copy";	/* Default is copy */
            switch (fill_ROP)
              {
              case SRCCOPY:	/* dest = source */
                mode = "Copy";
                break;
              case SRCPAINT:	/* dest = source OR dest */
                break;
              case SRCAND:	/* dest = source AND dest */
                break;
              case SRCINVERT:	/* dest = source XOR dest */
                mode = "Xor";
                break;
              case SRCERASE:	/* dest = source AND (NOT dest) */
                break;
              case NOTSRCCOPY:	/* dest = (NOT source) */
/*                 NegateImage(image, False); */
                /* mode = "Copy"; */
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

            id = magick_add_registry(API, image, &exception);
            if( id > -1 )
              {
                magick_mvg_printf(API, "push pattern fill_%lu 0,0, %u,%u\n",
                                  ddata->pattern_id, brush_bmp->width, brush_bmp->height);
                magick_mvg_printf(API, "image %s 0,0 %u,%u 'mpr:%li'\n",
                                  mode, brush_bmp->width, brush_bmp->height, id);
                magick_mvg_printf(API, "pop pattern\n");
                magick_mvg_printf(API, "fill url(#fill_%lu)\n", ddata->pattern_id);
                ++ddata->pattern_id;
              }
            else
              ThrowException(&ddata->image->exception,exception.severity,
                             exception.reason,exception.description);
          }
	else
	  printf("magick_brush: no BMP image data!\n");

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

  pen = WMF_DC_PEN(dc);

  pen_color = WMF_PEN_COLOR(pen);

  pen_width = (WMF_PEN_WIDTH(pen) + WMF_PEN_HEIGHT(pen)) / 2;

  /* Pixel width is inverse of pixel scale */
  pixel_width = (((double) 1 / (ddata->scale_x)) +
		 ((double) 1 / (ddata->scale_y))) / 2;

  /* Don't allow pen_width to be less than pixel_width in order to
     avoid dissapearing or spider-web lines */
  pen_width = Max(pen_width, pixel_width);

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
  magick_mvg_printf(API, "stroke-width %.10g\n", Max(0, pen_width));

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
      magick_mvg_printf(API, "stroke-antialias 0\n");
      magick_mvg_printf(API, "stroke-dasharray %.10g,%.10g\n",
			pixel_width * 18, pixel_width * 7);
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

/* Estimate font pointsize based on Windows font parameters */
static double magick_font_pointsize( wmfAPI* API, wmfFont* font, char* str, double font_height)
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
      if (strlen(str) <= 1)
        {
          /* For individual characters, FONT_HEIGHT appears to
             specify the hight of the ascent only so calculate final
	   pointsize based on ratio of ascent to ascent+descent */
          pointsize = font_height *
            ((double) font_height /
             (metrics.ascent + AbsoluteValue(metrics.descent)));
          draw_info.pointsize = pointsize;
        }
      else
        {
          /* For multi-character strings, the height metric seems to
             offer the best pointsize estimation */
          pointsize =
            font_height * ((double) font_height / (metrics.height));
          draw_info.pointsize = pointsize;
        }
    }

  return pointsize;
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
static float magick_font_stringwidth( wmfAPI* API, wmfFont* font, char* str)
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
  printf("\nmagick_font_stringwidth\n");
  printf("WMF_FONT_NAME           = \"%s\"\n", WMF_FONT_NAME(font));
  printf("WMF_FONT_PSNAME         = \"%s\"\n", WMF_FONT_PSNAME(font));
  printf("stringwidth             = %.10g\n", stringwidth);
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

/* Mappings to Ghostscript fonts: family, normal, italic, bold, bolditalic */
static wmfFontMap WMFFontMap[] = {
  { "Courier",            "Courier",     "Courier-Oblique",   "Courier-Bold",   "Courier-BoldOblique"   },
  { "Helvetica",          "Helvetica",   "Helvetica-Oblique", "Helvetica-Bold", "Helvetica-BoldOblique" },
  { "Modern",             "Courier",     "Courier-Oblique",   "Courier-Bold",   "Courier-BoldOblique"   },
  { "Monotype Corsiva",   "Courier",     "Courier-Oblique",   "Courier-Bold",   "Courier-BoldOblique"   },
  { "News Gothic",        "Helvetica",   "Helvetica-Oblique", "Helvetica-Bold", "Helvetica-BoldOblique" },
  { "Symbol",             "Symbol",      "Symbol",            "Symbol",         "Symbol"                },
  { "System",             "Courier",     "Courier-Oblique",   "Courier-Bold",   "Courier-BoldOblique"   },
  { "Times",              "Times-Roman", "Times-Italic",      "Times-Bold",     "Times-BoldItalic"      },
  {  NULL,		   NULL,          NULL,                NULL,             NULL                   }
};

/* Mapping between base name and Ghostscript family name */
static wmfMapping SubFontMap[] = {
  { "Arial",		"Helvetica" },
  { "Courier",		"Courier"   },
  { "Fixed",		"Courier"   },
  { "Helvetica",	"Helvetica" },
  { "Sans",		"Helvetica" },
  { "Sym",		"Symbol"    },
  { "Terminal",		"Courier"   },
  { "Times",		"Times"     },
  { "Wingdings",	"Symbol"    },
  {  NULL,	         NULL       }
};

/* Estimate weight based on font name */
static double font_weight( const char* font )
{
  double
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

static void magick_font_map( wmfAPI* API, wmfFont* font)
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

  if (font == 0)
    return;

  font_data = (wmfFontData*)API->font_data;
  font->user_data = font_data->user_data;
  magick_font = (wmf_magick_font_t*)font->user_data;

  LiberateMemory((void**)&magick_font->ps_name);

  GetExceptionInfo(&exception);
  type_info_base=GetTypeInfo("*",&exception);
  if(type_info_base == 0)
    {
      ThrowException(&ddata->image->exception,exception.severity,
                     exception.reason,exception.description);
      return;
    }

  /* Look for exact full match first */
  for ( type_info=type_info_base; type_info != 0; type_info=type_info->next)
    {
      if(LocaleCompare(WMF_FONT_NAME(font),type_info->description) == 0)
        {
          CloneString(&magick_font->ps_name,type_info->name);
          break;
        }
    }

  /* Look for a family-based best-match next */
  if(!magick_font->ps_name)
    {
      double
        target_weight,
        best_weight = 0;

      if( WMF_FONT_WEIGHT(font) == 0 )
        target_weight = 400;
      else
        target_weight = WMF_FONT_WEIGHT(font);

      for ( type_info=type_info_base; type_info != 0; type_info=type_info->next )
        {
          if(LocaleCompare(WMF_FONT_NAME(font),type_info->family) == 0)
            {
              double
                weight;
              
              if( WMF_FONT_ITALIC(font) && !(strstr(type_info->description,"Italic") ||
                                             strstr(type_info->description,"Oblique")) )
                continue;

              weight = font_weight( type_info->description );

              if( abs(weight - target_weight) < abs(best_weight - target_weight) )
                {
                  best_weight = weight;
                  CloneString(&magick_font->ps_name,type_info->name);
                }
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

      if( (target_weight > 550) || ((strstr(WMF_FONT_NAME(font),"Bold") ||
                                     strstr(WMF_FONT_NAME(font),"Heavy") ||
                                     strstr(WMF_FONT_NAME(font),"Black"))) )
        want_bold = True;

      if( (WMF_FONT_ITALIC(font)) || ((strstr(WMF_FONT_NAME(font),"Italic") ||
                                       strstr(WMF_FONT_NAME(font),"Oblique"))) )
        want_italic = True;

      strcpy(target,"Times");
      for( i=0; SubFontMap[i].name != NULL; i++ )
        {
          if(LocaleCompare(WMF_FONT_NAME(font), SubFontMap[i].name) == 0)
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
  printf("\nmagick_font_map\n");
  printf("WMF_FONT_NAME:          = \"%s\"\n", WMF_FONT_NAME(font));
  printf("WMF_FONT_PSNAME         = \"%s\"\n", WMF_FONT_PSNAME(font));
  fflush(stdout);
#endif
  
}

/* Initialize API font structures */
static void magick_font_init( wmfAPI* API, wmfAPI_Options* options)
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
  font_data->map = magick_font_map;

  /* Assign function to return string width in points (type wmfStringWidth) */
  font_data->stringwidth = magick_font_stringwidth;

  /* Assign user data, not used by libwmflite (type void*) */
  font_data->user_data = wmf_malloc(API,sizeof(wmf_magick_font_t));
  if(ERR(API))
    return;
  ((wmf_magick_font_t*)font_data->user_data)->ps_name = 0;
  ((wmf_magick_font_t*)font_data->user_data)->pointsize = 0;
}

#endif /* HasWMFlite */

/* Extend MVG, printf style */
static int magick_mvg_printf(wmfAPI * API, char *format, ...)
{
  const size_t
    alloc_size = MaxTextExtent*2;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Allocate initial memory */
  if(ddata->mvg == 0)
    {
      ddata->mvg = AcquireMemory(alloc_size);
      ddata->mvg_alloc = alloc_size;
      ddata->mvg_length = 0;
      if(ddata->mvg == 0)
        return -1;
    }

  /* Re-allocate additional memory if necessary */
  if(ddata->mvg_alloc < (ddata->mvg_length+MaxTextExtent))
    {
      size_t
        realloc_size = ddata->mvg_alloc + alloc_size;

      ReacquireMemory((void**)&ddata->mvg,realloc_size);
      if(ddata->mvg == NULL)
        return -1;
      ddata->mvg_alloc = realloc_size;
    }

/*   printf("ddata->mvg = %lx  ddata->mvg_length = %d  ddata->mvg_alloc = %d\n", */
/*          (long)ddata->mvg, ddata->mvg_length, ddata->mvg_alloc); */

  /* Write to end of existing MVG string */
  {
    char
      buffer[MaxTextExtent];

    long
      str_length;

    va_list
      argp;

    va_start(argp, format);
    str_length = vsnprintf(buffer, MaxTextExtent - 1, format, argp);
    va_end(argp);
    if(str_length > 0 && str_length < (MaxTextExtent - 1))
      {
        /* printf("%s",buffer); */
        strcpy(ddata->mvg+ddata->mvg_length, buffer);
        ddata->mvg_length += str_length;
        return str_length;
      }

    printf("vsnprintf return out of bounds (%ld)!\n", str_length);
    
    return -1;
  }
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

/* BLOB read byte */
static int wmf_magick_read(void* context)
{
  return ReadBlobByte((Image*)context);
}

/* BLOB seek */
static int wmf_magick_seek(void* context,long position)
{
  return (int)SeekBlob((Image*)context,(off_t)position,SEEK_SET);
}

/* BLOB tell */
static long wmf_magick_tell(void* context)
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
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);

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

  wmf_error = wmf_api_create(&API, wmf_options_flags, &wmf_api_options);
  if (wmf_error != wmf_E_None)
    {
      if (API)
        wmf_api_destroy(API);
      ThrowReaderException(DelegateError, "Failed to intialize libwmf", image);
    }

  ddata = WMF_MAGICK_GetData(API);
  ddata->image = image;
  ddata->image_info = image_info;

#if defined(HasWMFlite)
  /* Must initialize font subystem for WMFlite interface */

  magick_font_init (API,&wmf_api_options); /* similar to wmf_ipa_font_init in src/font.c */
  /* wmf_arg_fontdirs (API,options); */ /* similar to wmf_arg_fontdirs in src/wmf.c */

#endif

  /*
   * Open BLOB input via libwmf API
   *
   */
  wmf_error = wmf_bbuf_input(API,wmf_magick_read,wmf_magick_seek,
                             wmf_magick_tell,(void*)image);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      ThrowReaderException(FileOpenError, "Unable to open file", image);
    }

  /*
   * Scan WMF file
   *
   */
  wmf_error = wmf_scan(API, 0, &bbox);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      ThrowReaderException(CorruptImageError, "Failed to scan file", image);
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
      ThrowReaderException(CorruptImageError,
                           "Failed to compute output size", image);
    }

  /* Obtain (or guess) metafile units */
  if ((API)->File->placeable)
    units_per_inch = (API)->File->pmh->Inch;
  else if( (wmf_width*wmf_height) < 1024*1024)
    units_per_inch = POINTS_PER_INCH;	/* MM_TEXT */
  else
    units_per_inch = TWIPS_PER_INCH;	/* MM_TWIPS */

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
  printf("Size in metafile units:      %.10gx%.10g\n", wmf_width, wmf_height);
  printf("Metafile units/inch:         %.10g\n", units_per_inch);
  printf("Size in inches:              %.10gx%.10g\n",image_width_inch,image_height_inch);
  printf("Bounding Box:                %.10g,%.10g %.10g,%.10g\n",
         bbox.TL.x, bbox.TL.y, bbox.BR.x, bbox.BR.y);
  printf("Bounding width x height:     %.10gx%.10g\n", bounding_width, bounding_height);
  printf("Output resolution:           %.10gx%.10g\n", resolution_x, resolution_y);
  printf("Image size:                  %.10gx%.10g\n", image_width, image_height);
  printf("Bounding box scale factor:   %.10g,%.10g\n",
         ddata->scale_x, ddata->scale_y);
  printf("Translation:                 %.10g,%.10g\n",
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
    printf("dc->pixel_width   = %.10g\n", dc->pixel_width);
    printf("dc->pixel_height  = %.10g\n", dc->pixel_height);
#if 0  /* Only in libwmf 0.3 */
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

  for (row=0; row < (long) image->rows; row++)
    {
      pixel=SetImagePixels(image,0,row,image->columns,1);
      if (pixel == (PixelPacket *) NULL)
        break;
      for (column=image->columns; column; column--)
        *pixel++ = background_color;
      if (!SyncImagePixels(image))
        break;
    }

  /*
   * Play file to generate MVG drawing commands
   *
   */
  ddata->mvg = AcquireMemory(MaxTextExtent);
  wmf_error = wmf_play(API, 0, &bbox);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      ThrowReaderException(CorruptImageError, "Failed to render file", image);
    }

  /*
   * Scribble on canvas image
   *
   */
  magick_render_mvg(API);

  /* Cleanup allocated data */
  wmf_api_destroy(API);
  CloseBlob(image);

  /* Return image */
  return image;
}
#endif
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
  entry->description = AllocateString("Windows Meta File");
#if defined(WIN32)
  entry->blob_support = False;
#endif
  entry->module = AllocateString("WMF");
  (void) RegisterMagickInfo(entry);
#if defined(WIN32)
  entry = SetMagickInfo("EMF");
  entry->decoder = ReadWMFImage;
  entry->description = AllocateString("Windows Enhanced Meta File");
  entry->blob_support = False;
  entry->module = AllocateString("WMF");
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
#if defined(WIN32)
  (void) UnregisterMagickInfo("EMF");
#endif
#endif /* defined(HasWMF) */
}
