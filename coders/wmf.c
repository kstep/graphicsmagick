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
%                          December 2000 - May 2001                           %
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
%
%
%
Work remaining to be completed on libwmf portion of this module:
 - Clipping rectangles
 - Overlay/inset images
 - Text annotation
 - Pie arc
 - Framed regions
 - Painting regions
 - Setting ROP
 - Setting fill style
 - Setting pen style
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

#if defined(HasWMF2)
 /* HasWMF2 */
static Image *ReadWMFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  return (Image *)NULL;
}
#elif defined(HasWMF)
#include <wmfapi.h>

/*
  Forward declarations.
*/
extern wmf_functions WmfFunctions;
extern int currentx;
extern int currenty;


#define MVG_PTR ((ImUserData*)(cstruct->userdata))->mvg
#define IMG_PTR ((ImUserData*)(cstruct->userdata))->image
typedef struct _ImUserData
{
  char*    mvg;
  Image*   image;
} ImUserData;

typedef struct _ImWmfFont
{
  char*    family;
  char*    face;
  char*    fontspec;
} ImWmfFont;

/*
  Windows for Workgroups Fonts:

    Filename       Size    Date     Time     1.44 MB   1.2 MB
    ---------------------------------------------------------
    ARIAL.TTF      52532  10-01-92   3.11a       6       6
    ARIALBD.TTF    51841  10-01-92   3.11a       7       7
    ARIALBI.TTF    57729  10-01-92   3.11a       7       6
    ARIALI.TTF     47643  10-01-92   3.11a       7       7
    COUR.TTF       53733  10-01-92   3.11a       7       6
    COURBD.TTF     56871  10-01-92   3.11a       7       6
    COURBI.TTF     64330  10-01-92   3.11a       7       6
    COURI.TTF      60757  10-01-92   3.11a       7       6
    TIMES.TTF      69074  10-01-92   3.11a       6       6
    TIMESBD.TTF    63489  10-01-92   3.11a       7       6
    TIMESBI.TTF    60539  10-01-92   3.11a       7       6
    TIMESI.TTF     61880  10-01-92   3.11a       7       6
    SYMBOL.TTF     50450  10-01-92   3.11a       6       6
    WINGDING.TTF   50450  10-01-92   3.11a       7       6

  Libwmf's text.wmf uses the fonts:
    Arial
    Bookman Old Style
    Comic Sans MS
    Courier
    Courier New
    Garamond
    Impact
    MS Sans Serif
    MS Serif
    Modern
    Small Fonts
    Symbol
    Tahoma
    Times New Roman
    Times New Roman
    Verdana
    Wingdings

 */
static const ImWmfFont FontList [] =
{
  { (char *) "Arial",  (char *) "Arial", (char *) "arial.ttf" },
  { (char *) "Arial",  (char *) "Arial Bold", (char *) "arialbd.ttf" },
  { (char *) "Arial",  (char *) "Arial Bold Italic", (char *) "arialbi.ttf" },
  { (char *) "Arial",  (char *) "Arial Italic", (char *) "ariali.ttf" },
  { (char *) "Bookman Old Style",  (char *) "Bookman Old Style", (char *) "bkmnos.ttf" },
  { (char *) "Bookman Old Style",  (char *) "Bookman Old Style Bold", (char *) "bookosb.ttf" },
  { (char *) "Bookman Old Style",  (char *) "Bookman Old Style Bold Italic", (char *) "bookosbi.ttf" },
  { (char *) "Bookman Old Style",  (char *) "Bookman Old Style Italic", (char *) "boookosi.ttf" },
  { (char *) "Century Schoolbook",  (char *) "Century Schoolbook", (char *) "censcbk.ttf" },
  { (char *) "Century Schoolbook",  (char *) "Century Schoolbook Bold", (char *) "schlbkb.ttf" },
  { (char *) "Century Schoolbook",  (char *) "Century Schoolbook Bold Italic", (char *) "schlbkbi.ttf" },
  { (char *) "Century Schoolbook",  (char *) "Century Schoolbook Italic", (char *) "schlbki.ttf" },
  { (char *) "Comic Sans MS",  (char *) "Comic Sans MS", (char *) "comic.ttf" },
  { (char *) "Comic Sans MS",  (char *) "Comic Sans MS Bold", (char *) "comicbd.ttf" },
  { (char *) "Courier New",  (char *) "Courier New", (char *) "cour.ttf" },
  { (char *) "Courier New",  (char *) "Courier New Bold", (char *) "courbd.ttf" },
  { (char *) "Courier New",  (char *) "Courier New Bold Italic", (char *) "courbi.ttf" },
  { (char *) "Courier New",  (char *) "Courier New Italic", (char *) "couri.ttf" },
  { (char *) "Courier",  (char *) "Courier", (char *) "cour.ttf" },
  { (char *) "Garamond",  (char *) "Garamond", (char *) "gara.ttf" },
  { (char *) "Garamond",  (char *) "Garamond Bold", (char *) "garabd.ttf" },
  { (char *) "Garamond",  (char *) "Garamond Italic", (char *) "garait.ttf" },
/*   { (char *) "Gill Sans MT",  (char *) "Gill Sans MT Ext Condensed Bold", (char *) "glsnecb.ttf" }, */
  { (char *) "Impact",  (char *) "Impact", (char *) "impact.ttf" },
/*   { (char *) "Lucida Blackletter",  (char *) "Lucida Blackletter", (char *) "lblack.ttf" }, */
/*   { (char *) "Lucida Bright",  (char *) "Lucida Bright", (char *) "lbrite.ttf" }, */
/*   { (char *) "Lucida Bright",  (char *) "Lucida Bright Demibold", (char *) "lbrited.ttf" }, */
/*   { (char *) "Lucida Bright",  (char *) "Lucida Bright Demibold Italic", (char *) "lbritedi.ttf" }, */
/*   { (char *) "Lucida Bright",  (char *) "Lucida Bright Italic", (char *) "lbritei.ttf" }, */
/*   { (char *) "Lucida Caligraphy",  (char *) "Lucida Caligraphy Italic", (char *) "lcalig.ttf" }, */
/*   { (char *) "Lucida Console",  (char *) "Lucida Console", (char *) "lucon.ttf" }, */
/*   { (char *) "Lucida Fax",  (char *) "Lucida Fax Demibold", (char *) "lfaxd.ttf" }, */
/*   { (char *) "Lucida Fax",  (char *) "Lucida Fax Demibold Italic", (char *) "lfaxdi.ttf" }, */
/*   { (char *) "Lucida Fax",  (char *) "Lucida Fax Italic", (char *) "lfaxi.ttf" }, */
/*   { (char *) "Lucida Fax",  (char *) "Lucida Fax Regular", (char *) "lfax.ttf" }, */
/*   { (char *) "Lucida Handwriting",  (char *) "Lucida Handwriting Italic", (char *) "lhandw.ttf" }, */
/*   { (char *) "Lucida Sans Typewriter",  (char *) "Lucida Sans Typewriter Bold", (char *) "ltypeb.ttf" }, */
/*   { (char *) "Lucida Sans Typewriter",  (char *) "Lucida Sans Typewriter Bold Oblique", (char *) "ltypebo.ttf" }, */
/*   { (char *) "Lucida Sans Typewriter",  (char *) "Lucida Sans Typewriter Oblique", (char *) "ltypeo.ttf" }, */
/*   { (char *) "Lucida Sans Typewriter",  (char *) "Lucida Sans Typewriter Regular", (char *) "ltype.ttf" }, */
/*   { (char *) "Lucida Sans",  (char *) "Lucida Sans Demibold Italic", (char *) "lsansdi.ttf" }, */
/*   { (char *) "Lucida Sans",  (char *) "Lucida Sans Demibold Roman", (char *) "lsansd.ttf" }, */
/*   { (char *) "Lucida Sans",  (char *) "Lucida Sans Demibold Italic", (char *) "lsansdi.ttf" }, */
/*   { (char *) "Lucida Sans",  (char *) "Lucida Sans Regular", (char *) "lsans.ttf" }, */
  { (char *) "MS Sans Serif",  (char *) "MS Sans Serif", (char *) "sseriff.ttf" },
  { (char *) "MS Serif",  (char *) "MS Serif", (char *) "seriff.ttf" },
  { (char *) "Modern",  (char *) "Modern", (char *) "modern.ttf" },
  { (char *) "Monotype Corsiva",  (char *) "Monotype Corsiva", (char *) "mtcorsva.ttf" },
  { (char *) "Small Fonts",  (char *) "Small Fonts", (char *) "smallf.ttf" },
  { (char *) "Symbol",  (char *) "Symbol", (char *) "symbol.ttf" },
  { (char *) "Tahoma",  (char *) "Tahoma Bold", (char *) "tahomabd.ttf" },
  { (char *) "Tahoma",  (char *) "Tahoma", (char *) "tahoma.ttf" },
  { (char *) "Times New Roman",  (char *) "Times New Roman", (char *) "times.ttf" },
  { (char *) "Times New Roman",  (char *) "Times New Roman Bold", (char *) "timesbd.ttf" },
  { (char *) "Times New Roman",  (char *) "Times New Roman Bold Italic", (char *) "timesbi.ttf" },
  { (char *) "Times New Roman",  (char *) "Times New Roman Italic", (char *) "timesi.ttf" },
  { (char *) "Times New Roman MT",  (char *) "Times New Roman MT Extra Bold", (char *) "timnreb.ttf" },
  { (char *) "Times",  (char *) "Times", (char *) "times.ttf" },
  { (char *) "Verdana",  (char *) "Verdana Bold", (char *) "verdanab.ttf" },
  { (char *) "Verdana",  (char *) "Verdana Bold Italic", (char *) "verdanaz.ttf" },
  { (char *) "Verdana",  (char *) "Verdana Italic", (char *) "verdanai.ttf" },
  { (char *) "Verdana",  (char *) "Verdana", (char *) "verdana.ttf" },
  { (char *) "WingDings",  (char *) "WingDings", (char *) "wingding.ttf" },
/*   { (char *) "WingDings",  (char *) "Wingdings 2", (char *) "wingdng2.ttf" }, */
/*   { (char *) "WingDings",  (char *) "Wingdings 3", (char *) "wingdng3.ttf" }, */
  { (char *) NULL,  (char *) NULL, (char *) NULL }
};

/* Hatch images */
#define HatchBDIAGONALImageExtent  53
#define HatchCROSSImageExtent      54
#define HatchDIAGCROSSImageExtent  54
#define HatchFDIAGONALImageExtent  53
#define HatchHORIZONTALImageExtent 50
#define HatchVERTICALImageExtent   53
static const unsigned char
  HatchBDIAGONALImage[]=
  {
    0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x08, 0x00, 0x08, 0x00, 0xf0, 0x01, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x21, 0xf9, 0x04, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 
    0x00, 0x02, 0x0c, 0x8c, 0x1f, 0x80, 0xa6, 0xdb, 0xf8, 0x4e, 0x64, 0x2b, 
    0xd5, 0x69, 0x0a, 0x00, 0x3b, 
  },
  HatchCROSSImage[]=
  {
    0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x08, 0x00, 0x08, 0x00, 0xf0, 0x01, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x21, 0xf9, 0x04, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 
    0x00, 0x02, 0x0d, 0x8c, 0x61, 0x79, 0x9a, 0xd8, 0x00, 0xa3, 0x0b, 0x53, 
    0xd5, 0xc6, 0x56, 0x2a, 0x00, 0x3b, 
  },
  HatchDIAGCROSSImage[]=
  {
    0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x08, 0x00, 0x08, 0x00, 0xf0, 0x01, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x21, 0xf9, 0x04, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 
    0x00, 0x02, 0x0d, 0x44, 0x8e, 0x66, 0xc9, 0xb1, 0xf9, 0x20, 0x3a, 0xeb, 
    0x45, 0x35, 0x69, 0x01, 0x00, 0x3b, 
  },
  HatchFDIAGONALImage[]=
  {
    0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x08, 0x00, 0x08, 0x00, 0xf0, 0x01, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x21, 0xf9, 0x04, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 
    0x00, 0x02, 0x0c, 0x44, 0x8e, 0x67, 0x89, 0xcb, 0xe1, 0x58, 0x4c, 0xb3, 
    0x3d, 0x55, 0x00, 0x00, 0x3b, 
  },
  HatchHORIZONTALImage[]=
  {
    0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x08, 0x00, 0x08, 0x00, 0xf0, 0x01, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x21, 0xf9, 0x04, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 
    0x00, 0x02, 0x09, 0x8c, 0x8f, 0xa9, 0x07, 0xed, 0xbd, 0xa2, 0x8c, 0x05, 
    0x00, 0x3b, 
  },
  HatchVERTICALImage[]=
  {
    0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x08, 0x00, 0x08, 0x00, 0xf0, 0x01, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x21, 0xf9, 0x04, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 
    0x00, 0x02, 0x0c, 0x8c, 0x61, 0x79, 0x9a, 0xc8, 0xe1, 0x58, 0x54, 0xb3, 
    0x3d, 0xa8, 0x0a, 0x00, 0x3b, 
  };


static int
  WmfMmHeight(CSTRUCT *),
  WmfMmWidth(CSTRUCT *),
  WmfPixelHeight(CSTRUCT *),
  WmfPixelWidth(CSTRUCT *);

/* static void * */
/*   WmfInitialUserData(CSTRUCT *); */

static void
  ExtendMVG(CSTRUCT *cstruct, const char* buff),
  ScribbleMVG(CSTRUCT *cstruct),
  WmfClipRect(CSTRUCT *),
/*   WmfCopyUserData(CSTRUCT *,DC *,DC *), */
  WmfCopyXpm(CSTRUCT *,U16, U16, U16 , U16 ,U16,U16,char *,U32 ),
  WmfDrawArc(CSTRUCT*,WMFRECORD*,int finishing),
  WmfDrawChord(CSTRUCT *,WMFRECORD *),
  WmfDrawEllipse(CSTRUCT*,WMFRECORD*),
  WmfDrawLine(CSTRUCT*,WMFRECORD*),
  WmfDrawPie(CSTRUCT *,WMFRECORD *),
  WmfDrawPolyLines(CSTRUCT *,WMFRECORD *),
  WmfDrawPolyPolygon(CSTRUCT*,WMFRECORD*),
  WmfDrawPolygon(CSTRUCT*,WMFRECORD*),
  WmfDrawRectangle(CSTRUCT*,WMFRECORD*),
  WmfDrawRoundRectangle(CSTRUCT *,WMFRECORD *),
  WmfDrawSimpleArc(CSTRUCT *,WMFRECORD *),
  WmfDrawText(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,int x,int y),
  WmfExtFloodFill(CSTRUCT *,WMFRECORD *),
  WmfFillOpaque(CSTRUCT *,WMFRECORD *),
/*   WmfFinish(CSTRUCT *), */
  WmfFloodFill(CSTRUCT *,WMFRECORD *),
/*   WmfFrameRgn(CSTRUCT *cstruct,WINEREGION *rgn,U16 width,U16 height), */
/*   WmfFreeUserData(CSTRUCT *,DC *), */
  WmfNoClipRect(CSTRUCT *),
  WmfPaintRgn(CSTRUCT *, WINEREGION *rgn),
  WmfParseROP(CSTRUCT *,U32 ,U16 , U16, U16 , U16 ),
/*   WmfRestoreUserData(CSTRUCT *,DC *), */
  WmfSetFillStyle(CSTRUCT *,LOGBRUSH *,DC *),
  WmfSetPenStyle(CSTRUCT *,LOGPEN *,DC *),
  WmfSetPixel(CSTRUCT *,WMFRECORD *),
  WmfSetPmfSize(CSTRUCT *,HMETAFILE );



/* WMF Library Callbacks */
wmf_functions WmfFunctions =
{
  WmfPixelWidth,
  WmfPixelHeight,
  WmfMmWidth,
  WmfMmHeight,
  WmfDrawEllipse,
  WmfDrawSimpleArc,
  WmfDrawArc,
  WmfDrawPie,
  WmfDrawChord,
  WmfDrawPolygon,
  WmfDrawPolyPolygon,
  WmfDrawRectangle,
  WmfDrawRoundRectangle,
  WmfDrawLine,
  WmfDrawPolyLines,
  WmfDrawText,
  WmfSetPixel,
  WmfFloodFill,
  WmfExtFloodFill,
  WmfFillOpaque,
  WmfParseROP,
  WmfSetFillStyle,
  WmfSetPenStyle,
  WmfSetPmfSize,
  WmfClipRect,
  WmfNoClipRect,
  WmfCopyXpm,
  WmfPaintRgn,
  NULL, /* WmfFrameRgn */
  NULL, /*WmfCopyUserData, */
  NULL, /* WmfRestoreUserData, */
  NULL, /* WmfFreeUserData */
  NULL, /* WmfInitialUserData, */
  NULL, /* WmfFinish */
};

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d W M F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadWMFImage reads a Windows Metafile and constructs an image.
%  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadWMFImage method is:
%
%      Image *ReadWMFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadWMFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
/* Extend MVG drawing primitives in cstruct userdata */
static void ExtendMVG(CSTRUCT *cstruct, const char* buff)
{
  ConcatenateString((char**)&MVG_PTR, buff);
}
/* Render and then clear MVG drawing primitives in cstruct userdata */
static void ScribbleMVG(CSTRUCT *cstruct)
{
  DrawInfo*
    draw_info;

  ImageInfo*
    image_info;

  image_info=(ImageInfo*)AcquireMemory(sizeof(ImageInfo));
  GetImageInfo(image_info);
  draw_info = (DrawInfo*)AcquireMemory(sizeof(DrawInfo));
  GetDrawInfo( image_info, draw_info );
  draw_info->primitive=MVG_PTR;
/*   puts(draw_info->primitive); */
  DrawImage(IMG_PTR,draw_info);
  draw_info->primitive = (char*)NULL;
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(image_info);
  LiberateMemory((void**)&MVG_PTR);
  MVG_PTR=AllocateString("");
}

#define SCREEN_WIDTH_PIXELS 1280
#define SCREEN_HEIGHT_PIXELS 1024
/* Return output height of screen in millimeters */
static int WmfMmHeight(CSTRUCT *cstruct)
{
  double
    y_resolution;

  y_resolution=72.0;

  if(IMG_PTR->y_resolution > 0)
    y_resolution=IMG_PTR->y_resolution;

  return(((double)SCREEN_HEIGHT_PIXELS*MM_PER_INCH)/y_resolution);
}

static int WmfMmWidth(CSTRUCT *cstruct)
{
  double
    x_resolution;
  
  x_resolution=72.0;

  if(IMG_PTR->x_resolution > 0)
    x_resolution=IMG_PTR->x_resolution;

  return(((double)SCREEN_WIDTH_PIXELS*MM_PER_INCH)/x_resolution);
}

/* Return output screen height in pixels */
static int WmfPixelHeight(CSTRUCT *cstruct)
{
  return(SCREEN_HEIGHT_PIXELS);
}

/* Return output screen width in pixels */
static int WmfPixelWidth(CSTRUCT *cstruct)
{
  return(SCREEN_WIDTH_PIXELS);
}

/* Return initialized drawing context */
/* static void *WmfInitialUserData(CSTRUCT *cstruct) */
/* { */
/*   return (void*)NULL; */
/* } */

/* Set rectangular clipping region */
static void WmfClipRect(CSTRUCT *cstruct)
{
  /* FIXME */
  puts("WmfClipRect() not implemented");
}

/* Copy drawing context */
/* static void WmfCopyUserData(CSTRUCT *cstruct, DC *old, DC *new) */
/* { */
/*   puts("WmfCopyUserData()"); */
/* } */

/* Copy Xpm onto image. (supports DIBBITBLT) */
static void WmfCopyXpm(CSTRUCT *cstruct,
                       unsigned short src_x, unsigned short src_y,
                       unsigned short dest_x, unsigned short dest_y,
                       unsigned short dest_w, unsigned short dest_h,
                       char *filename, unsigned int dwROP)
{
#if 0
  /* FIXME: this trivial implementation only implements pixel
     replacement and ignores ROP entirely.  More support is needed in
     ImageMagick to support setting the fill style.

     Also, if this is based on a temporary file, how is this temporary
     file cleaned up, and who creates it?

     This API should simply pass a pointer to the unprocessed
     in-memory BMP!
  */
  char
    buff[MaxTextExtent];

  /* image x,y width,height filename */
  sprintf(buff, "image Copy %i,%i %i,%i XPM:%.1024s\n",
          (int)dest_x,
          (int)dest_y,
          (int)dest_w,
          (int)dest_h,
          filename);
  ExtendMVG(cstruct, buff);
  ScribbleMVG(cstruct);
#endif
}

/*
  Draw closed (and optionally filled) arc
  
    ImageMagick Parameters:
      ellipse centerX,centerY radiusX,radiusY arcStart,arcEnd

      Note that width and height are the distance from the center
      coordinate, not the overall width and height!
  
    WMF Array Parameters:
      0  yend, y of endpoint
      1  xend, x of endpoint
      2  ystart, y of startpoint
      3  xstart, x of startpoint
      4  bottom, bottom of bounding box
      5  right, right of bounding box
      6  top, top of bounding box
      7  left, left of bounding box

     Note that the arc starts at the point on the ellipse where a line
     from the origin to xstart,ystart intersects the ellipse, and that
     the arc ends at the point on the ellipse where the where a line
     from the origin from xend,yend intersets the ellipse.

    Interpretation of finishtype:
     0  Simple arc (open and unfilled)
     1  Chord arc (line between end points, and optionally filled with brush)
     2  Pie arc (lines from end points to center, and optionally filled with brush)
*/
static void WmfDrawArc(CSTRUCT *cstruct, WMFRECORD *wmfrecord,
                       int finishtype)
{
  /* FIXME: Implementation not completed */
  char
    buff[MaxTextExtent];

  double
    angle1,
    angle2,
    oangle1,
    oangle2;

  float
    bottom,
    centerx,
    centery,
    left,
    radiusx,
    radiusy,
    right,
    tmp,
    top,
    x1,
    x2,
    xend,
    xstart,
    y1,
    y2,
    yend,
    ystart;

  int
    width;

  ExtendMVG(cstruct, "push graphic-context\n");

  width = cstruct->dc->pen->lopnWidth;

  top    = (NormY(wmfrecord->Parameters[6],cstruct)); /* top of bounding box */
  left   = (NormX(wmfrecord->Parameters[7],cstruct)); /* left of bounding box */
  bottom = (NormY(wmfrecord->Parameters[4],cstruct)); /* bottom of bounding box */
  right  = (NormX(wmfrecord->Parameters[5],cstruct)); /* right of bounding box */
  xstart = (NormX(wmfrecord->Parameters[3],cstruct)); /* x of startpoint */
  ystart = (NormY(wmfrecord->Parameters[2],cstruct)); /* y of startpoint */
  xend   = (NormX(wmfrecord->Parameters[1],cstruct)); /* x of endpoint */
  yend   = (NormY(wmfrecord->Parameters[0],cstruct)); /* y of endpoint */


/*   printf("DrawArc: yend=%f, xend=%f, ystart=%f, xstart=%f, bottom=%f right=%f, top=%f left=%f, width=%i\n", */
/*          yend,xend,ystart,xstart,bottom,right,top,left,width); */

  if (right < left) { tmp = right; right = left; left = tmp; }
  if (bottom < top) { tmp = bottom; bottom = top; top = tmp; }

  if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
    {
      if (2*width > (right-left)) width=(right-left + 1)/2;
      if (2*width > (bottom-top)) width=(bottom-top + 1)/2;
      left   += width / 2;
      right  -= (width - 1) / 2;
      top    += width / 2;
      bottom -= (width - 1) / 2;
    }

  centerx = (right+left)/2;
  centery = (bottom+top)/2;
  radiusx = (right-left)/2;
  radiusy = (bottom-top)/2;

  angle1 = atan2( (double)(centery-yend)*(right-left),(double)(xend-centerx)*(bottom-top) );
  angle2 = atan2( (double)(centery-ystart)*(right-left),(double)(xstart-centerx)*(bottom-top) );

  if ((xstart==xend)&&(ystart==yend))
    { /* A lazy program delivers xstart=xend=ystart=yend=0) */
      angle2 = 0;
      angle1 = 2* PI;
    }
  else if ((angle2 == PI)&&( angle1 <0))
    angle2 = - PI;
  else if ((angle1 == PI)&&( angle2 <0))
    angle2 = - PI;

  oangle1 = angle1;
  oangle2 = angle2;

  angle2 = (oangle2 * 180 * 64 / PI + 0.5);
  angle1  = ((oangle1 - oangle2) * 180 * 64 / PI + 0.5);
  if (angle1 <= 0) angle1 += 360 * 64;

  /* Intersecting points on ellipse */
  x1 = floor((right+left)/2.0 + cos(oangle2) * (right-left-width/**2+2*/) / 2. /*+ 0.5*/);
  y1 = floor((top+bottom)/2.0 - sin(oangle2) * (bottom-top-width/**2+2*/) / 2. /*+ 0.5*/);
  x2 = floor((right+left)/2.0 + cos(oangle1) * (right-left-width/**2+2*/) / 2. /*+ 0.5*/);
  y2 = floor((top+bottom)/2.0 - sin(oangle1) * (bottom-top-width/**2+2*/) / 2. /*+ 0.5*/);

  if ((cstruct->dc->brush->lbStyle != BS_NULL) && (finishtype != 0))
    {
      /* Set fill color for filled ellipse*/
      sprintf(buff, "fill #%02x%02x%02x\n",
              (cstruct->dc->brush->lbColor[0]& 0x00FF),
              (cstruct->dc->brush->lbColor[0]& 0xFF00)>>8,
              (cstruct->dc->brush->lbColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "fill none\n");

  if (cstruct->dc->pen->lopnStyle != PS_NULL)
    {
      /* Set stroke color for stroked ellipse */
      sprintf(buff, "stroke #%02x%02x%02x\n",
              (cstruct->dc->pen->lopnColor[0]& 0x00FF),
              ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8),
              (cstruct->dc->pen->lopnColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "stroke none\n");

  /* Draw the arc */
  sprintf( buff, "ellipse %f,%f %f,%f %f,%f\n",
           (float)centerx,
           (float)centery,
           (float)radiusx,
           (float)radiusy,
           (float)RadiansToDegrees(oangle1),
           (float)RadiansToDegrees(oangle2)
           );
  ExtendMVG(cstruct, buff);

  if (finishtype == 2)
  {
    /* Draw lines for pie arc */
    sprintf(buff,"line %f,%f %f,%f\n",x1,y1,centerx,centery);
    ExtendMVG(cstruct, buff);
    sprintf(buff,"line %f,%f %f,%f\n",x2,y2,centerx,centery);
    ExtendMVG(cstruct, buff);
  }
  else if (finishtype == 1)
  {
    /* Draw line for chord arc */
    sprintf(buff,"line %f,%f %f,%f\n",x1,y1,x2,y2);
    ExtendMVG(cstruct, buff);
  }

  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw closed (and optionally filled) arc */
static void WmfDrawChord(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  WmfDrawArc(cstruct,wmfrecord,1);
}

/* Draw ellipse */
static void WmfDrawEllipse(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char
    buff[MaxTextExtent];
  float
    bottom,
    left,
    right,
    tmp,
    top;

  ExtendMVG(cstruct, "push graphic-context\n");
  if (cstruct->dc->brush->lbStyle != BS_NULL)
    {
      /* Set fill color for filled ellipse*/
      sprintf(buff, "fill #%02x%02x%02x\n",
              (cstruct->dc->brush->lbColor[0]& 0x00FF),
              (cstruct->dc->brush->lbColor[0]& 0xFF00)>>8,
              (cstruct->dc->brush->lbColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "fill none\n");

  if (cstruct->dc->pen->lopnStyle != PS_NULL)
    {
      /* Set stroke color for stroked ellipse */
      sprintf(buff, "stroke #%02x%02x%02x\n",
              (cstruct->dc->pen->lopnColor[0]& 0x00FF),
              ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8),
              (cstruct->dc->pen->lopnColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "stroke none\n");

  /* Add ellipse primitive and points */
  /*
    ImageMagick Parameters:
      ellipse centerX,centerY radiusX,radiusY arcStart,arcEnd

      Note that width and height are the distance from the center
      coordinate, not the overall width and height!
  
    WMF Array Parameters:
      0  bottom, bottom of bounding box
      1  right, right of bounding box
      2  top, top of bounding box
      3 left, left of bounding box */

  bottom = (NormY(wmfrecord->Parameters[0],cstruct));
  right  = (NormX(wmfrecord->Parameters[1],cstruct));
  top    = (NormY(wmfrecord->Parameters[2],cstruct));
  left   = (NormX(wmfrecord->Parameters[3],cstruct));

  if (right < left) { tmp = right; right = left; left = tmp; }
  if (bottom < top) { tmp = bottom; bottom = top; top = tmp; }

  sprintf( buff, "ellipse %f,%f %f,%f %f,%f\n",
           (float)(right+left)/2,
           (float)(bottom+top)/2,
           (float)(right-left)/2,
           (float)(bottom-top)/2,
           (float)0,
           (float)360
           );

  ExtendMVG(cstruct, buff);
  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw line segment */
static void WmfDrawLine(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char buff[MaxTextExtent];

  ExtendMVG(cstruct, "push graphic-context\n");
  /* Set stroke color */
  sprintf(buff, "stroke #%02x%02x%02x\n",
          (cstruct->dc->pen->lopnColor[0]& 0x00FF),
          ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8),
          (cstruct->dc->pen->lopnColor[1]& 0x00FF)
          );
  ExtendMVG(cstruct, buff);

  /* Add line primitive and points */
  sprintf( buff, "line %i,%i %i %i\n",
           currentx, currenty,
           NormX(wmfrecord->Parameters[1],cstruct),
           NormY(wmfrecord->Parameters[0],cstruct) );
  ExtendMVG(cstruct, buff);
  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw pie chart (open arc with endpoints connected to center) */
static void WmfDrawPie(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  WmfDrawArc(cstruct,wmfrecord,2);
}

/* Draw a filled polygon */
static void WmfDrawPolygon(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  int i;
  char buff[MaxTextExtent];

  ExtendMVG(cstruct, "push graphic-context\n");
  if (cstruct->dc->brush->lbStyle != BS_NULL)
    {
      /* Set fill color for filled polygon*/
      sprintf(buff, "fill #%02x%02x%02x\n",
              (cstruct->dc->brush->lbColor[0]& 0x00FF),
              (cstruct->dc->brush->lbColor[0]& 0xFF00)>>8,
              (cstruct->dc->brush->lbColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "fill none\n");

  if (cstruct->dc->pen->lopnStyle != PS_NULL)
    {
      /* Set stroke color for stroked polygon */
      sprintf(buff, "stroke #%02x%02x%02x\n",
              (cstruct->dc->pen->lopnColor[0]& 0x00FF),
              ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8),
              (cstruct->dc->pen->lopnColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "stroke none\n");

  /* Add polygon primitive with points */
  ExtendMVG(cstruct, "polygon");
  for(i=1;i<wmfrecord->Parameters[0]+1;i++)
    {
      ExtendMVG(cstruct, " ");
      sprintf( buff, "%i,%i",
               NormX(wmfrecord->Parameters[(i*2)-1],cstruct),
               NormY(wmfrecord->Parameters[i*2],cstruct)
               );
      ExtendMVG(cstruct, buff);
    }
  ExtendMVG(cstruct, "\n");
  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw connected lines */
static void WmfDrawPolyLines(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  int i;
  char buff[MaxTextExtent];

  ExtendMVG(cstruct, "push graphic-context\n");
  /* Set stroke color */
  sprintf(buff, "stroke #%02x%02x%02x\n",
          (cstruct->dc->pen->lopnColor[0]& 0x00FF),
          ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8),
          (cstruct->dc->pen->lopnColor[1]& 0x00FF)
          );
  ExtendMVG(cstruct,buff);

  ExtendMVG(cstruct,"polyline");

  for(i=1;i<wmfrecord->Parameters[0]+1;i++)
    {
      ExtendMVG(cstruct," ");
      sprintf( buff, "%i,%i",
               NormX(wmfrecord->Parameters[(i*2)-1],cstruct),
               NormY(wmfrecord->Parameters[i*2],cstruct)
               );
      ExtendMVG(cstruct,buff);
    }
  ExtendMVG(cstruct,"\n");
  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw multiple polygons */
static void WmfDrawPolyPolygon(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char
    buff[MaxTextExtent];

  int
    *counts,
    i,
    k,
    j;

  ExtendMVG(cstruct, "push graphic-context\n");
  if (cstruct->dc->brush->lbStyle != BS_NULL)
    {
      /* Set fill color for filled polygon*/
      sprintf(buff, "fill #%02x%02x%02x\n",
              (cstruct->dc->brush->lbColor[0]& 0x00FF),
              (cstruct->dc->brush->lbColor[0]& 0xFF00)>>8,
              (cstruct->dc->brush->lbColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "fill none\n");

  if (cstruct->dc->pen->lopnStyle != PS_NULL)
    {
      /* Set stroke color for stroked polygon */
      sprintf(buff, "stroke #%02x%02x%02x\n",
              (cstruct->dc->pen->lopnColor[0]& 0x00FF),
              ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8),
              (cstruct->dc->pen->lopnColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "stroke none\n");

  counts = (int *) malloc(sizeof(int) * wmfrecord->Parameters[0]);
  for (i=0;i<wmfrecord->Parameters[0];i++)
    {
      counts[i] = wmfrecord->Parameters[1+i];
    }

  /* Add polygon primitive with points */
  for (k=0;k<wmfrecord->Parameters[0];k++)
    {
      ExtendMVG(cstruct, "polygon");
      for(j=0;j<counts[k];j++)
        {
          ExtendMVG(cstruct, " ");
          sprintf( buff, "%i,%i",
                   NormX(wmfrecord->Parameters[++i],cstruct),
                   NormY(wmfrecord->Parameters[++i],cstruct)
                   );
          ExtendMVG(cstruct, buff);
        }
      ExtendMVG(cstruct, "\n");
    }
  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw a rectangle */
static void WmfDrawRectangle(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char
    buff[MaxTextExtent];

  int
    x1,
    y1,
    x2,
    y2;

  ExtendMVG(cstruct, "push graphic-context\n");
  if (cstruct->dc->brush->lbStyle != BS_NULL)
    {
      /* Set fill color for filled rectangle*/
      sprintf(buff, "fill #%02x%02x%02x\n",
              (cstruct->dc->brush->lbColor[0]& 0x00FF),
              (cstruct->dc->brush->lbColor[0]& 0xFF00)>>8,
              (cstruct->dc->brush->lbColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "fill none\n");

  if (cstruct->dc->pen->lopnStyle != PS_NULL)
    {
      /* Set stroke color for stroked rectangle */
      sprintf(buff, "stroke #%02x%02x%02x\n",
              (cstruct->dc->pen->lopnColor[0]& 0x00FF),
              ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8),
              (cstruct->dc->pen->lopnColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "stroke none\n");

  /* Add rectangle primitive and points */
  x1 = NormX(wmfrecord->Parameters[3]+(cstruct->dc->pen->lopnWidth/2),cstruct);
  y1 = NormY(wmfrecord->Parameters[2]+(cstruct->dc->pen->lopnWidth/2),cstruct);
  x2 = NormX(wmfrecord->Parameters[1]-(cstruct->dc->pen->lopnWidth/2),cstruct);
  y2 = NormY(wmfrecord->Parameters[0]-(cstruct->dc->pen->lopnWidth/2),cstruct);
  sprintf(buff,"rectangle %i,%i %i,%i\n",x1,y1,x2,y2);
  ExtendMVG(cstruct, buff);
  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw a rectangle with rounded corners */
static void WmfDrawRoundRectangle(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char
    buff[MaxTextExtent];

  int
    ch,
    cw,
    x1,
    y1,
    x2,
    y2;

  ExtendMVG(cstruct, "push graphic-context\n");
  if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
    {
      sprintf(buff, "fill #%02x%02x%02x\n",
              (cstruct->dc->brush->lbColor[0]& 0x00FF),
              (cstruct->dc->brush->lbColor[0]& 0xFF00)>>8,
              (cstruct->dc->brush->lbColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "fill none\n");

  if (cstruct->dc->pen->lopnStyle != PS_NULL)
    {
      sprintf(buff, "stroke #%02x%02x%02x\n",
              (cstruct->dc->pen->lopnColor[0]& 0x00FF),
              ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8),
              (cstruct->dc->pen->lopnColor[1]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);
    }
  else
    ExtendMVG(cstruct, "stroke none\n");

  x1 = NormX(wmfrecord->Parameters[5]+(cstruct->dc->pen->lopnWidth/2),cstruct); /* left */
  y1 = NormY(wmfrecord->Parameters[4]+(cstruct->dc->pen->lopnWidth/2),cstruct); /* top */
  x2 = NormX(wmfrecord->Parameters[3]-(cstruct->dc->pen->lopnWidth/2),cstruct); /* right */
  y2 = NormY(wmfrecord->Parameters[2]-(cstruct->dc->pen->lopnWidth/2),cstruct); /* bottom */
  cw = ScaleX(wmfrecord->Parameters[1],cstruct); /* ell_width */
  ch = ScaleY(wmfrecord->Parameters[0],cstruct); /* ell_height */
  sprintf(buff,"roundrectangle %i,%i %i,%i %i,%i\n",x1,y1,x2,y2, cw, ch);
  ExtendMVG(cstruct, buff);
  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw a simple arc */
static void WmfDrawSimpleArc(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  WmfDrawArc(cstruct,wmfrecord,0);
}

/* Draw text */
static void WmfDrawText(CSTRUCT *cstruct, char *str, RECT *arect,
                        U16 flags, U16 *lpDx, int x, int y)
{
  /* FIXME: incomplete */

  char
    buff[MaxTextExtent],
    *facename,
    fontspec[MaxTextExtent],
    gravity[10];

  double
    angle;

  int
    i,
    pointsize,
    rotate,
    strikeout,
    textalign,
    underline,
    fontoffset,
    y_resolution;

  
  pointsize = ScaleY(cstruct->dc->font->lfHeight,cstruct);
  facename = cstruct->dc->font->lfFaceName;
  textalign = cstruct->dc->textalign;
  underline = cstruct->dc->font->lfUnderline;
  strikeout = cstruct->dc->font->lfStrikeOut;
  rotate = cstruct->dc->font->lfEscapement;
  angle = (double)(-cstruct->dc->font->lfEscapement)/10.0 * PI / 180;

  y_resolution=72.0;
  if(IMG_PTR->y_resolution > 0)
    y_resolution=IMG_PTR->y_resolution;
  fontoffset=0;

  ExtendMVG(cstruct, "push graphic-context\n");

  /* Set underline */
  if(underline)
    ExtendMVG(cstruct,"decorate underline\n");

  /* Set strike-Out */
  if(strikeout)
    ExtendMVG(cstruct,"decorate line-through\n");

  /* Determine font specification */
  *fontspec='\0';
  /* First look for an exact match */
  for( i=0; FontList[i].family != (char*)NULL; ++i )
  {
    if(LocaleCompare(facename,FontList[i].face) == 0)
      {
        strcpy(fontspec,FontList[i].fontspec);
        break;
      }
  }
  /* Look for an approximate match */
  if( *fontspec == '\0' )
    {
      char
        *candidate;

      int
        is_bold,
        is_italic,
        is_oblique,
        maxscore,
        score,
        target_bold,
        target_italic,
        target_oblique;

      target_bold=target_italic=target_oblique=False;
      is_bold=is_italic=is_oblique=False;
      maxscore=score=0;
      candidate=NULL;

      if(strstr(facename, "Bold") != 0 || strstr(facename, "Demibold") != 0)
        target_bold=True;
      if(strstr(facename, "Italic") != 0)
        target_italic=True;
      if(strstr(facename, "Oblique") != 0)
        target_oblique=True;

      for( i=0; FontList[i].family != (char*)NULL; ++i )
        {
          if(LocaleNCompare(facename,FontList[i].family,strlen(FontList[i].family)) == 0)
            {
              if(candidate==NULL)
                candidate=FontList[i].fontspec;
              if(strstr(FontList[i].face, "Bold") != 0 || strstr(FontList[i].face, "Demibold") != 0)
                is_bold=True;
              if(strstr(FontList[i].face, "Italic") != 0)
                is_italic=True;
              if(strstr(FontList[i].face, "Oblique") != 0)
                is_oblique=True;
              score=(target_bold&is_bold)+(target_italic&is_italic)+(target_oblique&is_oblique);
              if(score>maxscore)
                {
                  maxscore=score;
                  candidate=FontList[i].fontspec;
                }
            }
        }
      if(candidate!=NULL)
        strcpy(fontspec,candidate);
    }
  /* Last resort */
  if( *fontspec == '\0' )
    strcpy(fontspec,"arial.ttf");


  printf("facename=\"%.1024s\", fontspec=\"%.1024s\"\n", facename, fontspec);
  sprintf(buff, "font %.1024s\n", fontspec);
  ExtendMVG(cstruct,buff);

  /* Compute gravity */
  *gravity='\0';
  if(textalign & TA_TOP)
    {
      fontoffset=(pointsize*y_resolution)/72.0;
      if(textalign & TA_LEFT)
        strcpy(gravity,"NorthWest");
      else if(textalign & TA_CENTER)
        strcpy(gravity,"North");
      else if(textalign & TA_RIGHT)
        strcpy(gravity,"NorthEast");
    }
  else if(textalign & TA_BOTTOM)
    {
      fontoffset=0;
      if(textalign & TA_LEFT)
        strcpy(gravity,"SouthWest");
      else if(textalign & TA_CENTER)
        strcpy(gravity,"South");
      else if(textalign & TA_RIGHT)
        strcpy(gravity,"SouthEast");
    }
  else if(textalign & TA_BASELINE)
    {
      fontoffset=(pointsize*y_resolution)/(72.0*2);
      if(textalign & TA_RIGHT)
        strcpy(gravity,"South");
      else if(textalign & TA_CENTER)
        strcpy(gravity,"Center");
      else if(textalign & TA_RIGHT)
        strcpy(gravity,"East");
    }
  else
    {
      /* Default gravity is NorthWest */
      fontoffset=(pointsize*y_resolution)/72.0;
    }
  if(*gravity!='\0')
    {
      sprintf(buff, "gravity %.1024s\n", gravity);
      ExtendMVG(cstruct, buff);
    }

  /* Set point size */
/*   printf("pointsize=%i\n", pointsize); */
  sprintf(buff, "font-size %i\n", pointsize);
  ExtendMVG(cstruct, buff);

  /* Translate coordinates so target is 0,0 */
  sprintf(buff, "translate %i,%i\n",x,y+fontoffset);
  ExtendMVG(cstruct, buff);

  /* Rotation */
  if(rotate)
    {
      sprintf(buff, "rotate %f\n",angle);
      ExtendMVG(cstruct, buff);
    }

  /* Render text */
  if(str!=NULL)
    sprintf(buff, "text 0,0 \"%.1024s\"\n",str);
  ExtendMVG(cstruct, buff);

  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* META_FRAMEREGION */
/* static void WmfFrameRgn(CSTRUCT *cstruct,WINEREGION *rgn,U16 width,U16 height) */
/* { */
/*   puts("WmfFrameRgn()"); */
/* } */

/* Extended floodfill. Fill to border color, or fill color at point. (META_EXTFLOODFILL) */
static void WmfExtFloodFill(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char
    buff[MaxTextExtent];

  ExtendMVG(cstruct, "push graphic-context\n");

  if(wmfrecord->Parameters[0] == FLOODFILLSURFACE)
    {
      /* Fill color at point */
      sprintf(buff, "fill #%02x%02x%02x\n",
              (wmfrecord->Parameters[1]& 0x00FF),
              (wmfrecord->Parameters[1]& 0xFF00)>>8,
              (wmfrecord->Parameters[2]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);

      sprintf(buff,"color %i,%i floodfill\n",
              (int)NormX(wmfrecord->Parameters[4],cstruct),
              (int)NormY(wmfrecord->Parameters[3],cstruct));
      ExtendMVG(cstruct, buff);
    }
  else
    {
      /* Fill to border color */
      sprintf(buff, "fill #%02x%02x%02x\n",
              (wmfrecord->Parameters[1]& 0x00FF),
              (wmfrecord->Parameters[1]& 0xFF00)>>8,
              (wmfrecord->Parameters[2]& 0x00FF)
              );
      ExtendMVG(cstruct, buff);

      /* color x,y (point|replace|floodfill|filltoborder|reset) */
      sprintf(buff,"color %i,%i filltoborder\n",
              (int)NormX(wmfrecord->Parameters[4],cstruct),
              (int)NormY(wmfrecord->Parameters[3],cstruct));
      ExtendMVG(cstruct, buff);
    }

  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw filled but borderless rectangle */
static void WmfFillOpaque(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char
    buff[MaxTextExtent];

  int
    x1,
    y1,
    x2,
    y2;

  ExtendMVG(cstruct, "push graphic-context\n");

  /* Set fill color */
  sprintf(buff, "fill #%02x%02x%02x\n",
          (cstruct->dc->bgcolor[0]& 0x00FF),
          (cstruct->dc->bgcolor[0]& 0xFF00)>>8,
          (cstruct->dc->bgcolor[1]& 0x00FF)
          );
  ExtendMVG(cstruct, buff);

  /* Unset stroke color */
  ExtendMVG(cstruct, "stroke none\n");

  /* Add rectangle primitive and points */
  x1 = NormX(wmfrecord->Parameters[4]+(cstruct->dc->pen->lopnWidth/2),cstruct);
  y1 = NormY(wmfrecord->Parameters[5]+(cstruct->dc->pen->lopnWidth/2),cstruct);
  x2 = NormX(wmfrecord->Parameters[6]-(cstruct->dc->pen->lopnWidth/2),cstruct);
  y2 = NormY(wmfrecord->Parameters[7]-(cstruct->dc->pen->lopnWidth/2),cstruct);
  sprintf(buff,"rectangle %i,%i %i,%i\n",x1,y1,x2,y2);
  ExtendMVG(cstruct, buff);

  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Perform finishing steps */
/* static void WmfFinish(CSTRUCT *cstruct) */
/* { */
/* } */

/* Fill with color until border color (META_FLOODFILL) */
static void WmfFloodFill(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char
    buff[MaxTextExtent];

  ExtendMVG(cstruct, "push graphic-context\n");

  /* Set fill color */
  sprintf(buff, "fill #%02x%02x%02x\n",
          (wmfrecord->Parameters[0]& 0x00FF),
          (wmfrecord->Parameters[0]& 0xFF00)>>8,
          (wmfrecord->Parameters[1]& 0x00FF)
          );
  ExtendMVG(cstruct, buff);

  /* color x,y (point|replace|floodfill|filltoborder|reset) */
  sprintf(buff,"color %i,%i filltoborder\n",
          (int)NormX(wmfrecord->Parameters[3],cstruct),
          (int)NormY(wmfrecord->Parameters[2],cstruct));
  ExtendMVG(cstruct, buff);

  ExtendMVG(cstruct, "pop graphic-context\n");
}

static void WmfNoClipRect(CSTRUCT *cstruct)
{
  /* FIXME */
  puts("WmfNoClipRect() not implemented");
}

/* Paint rectangular region using brush color
  Invoked by:
  META_FILLREGION with cstruct->dc->brush set
  META_INVERTREGION with cstruct->dc->ROPmode set to R2_NOT
  META_PAINTREGION 
*/
static void WmfPaintRgn(CSTRUCT *cstruct, WINEREGION *rgn)
{
  /* FIXME: this is probably supposed to be drawing with texture rather than solid color */
  char
    buff[MaxTextExtent];

  double
    x1,
    y1,
    x2,
    y2;

  if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
    {
      /* Fill Region */
      ExtendMVG(cstruct, "push graphic-context\n");
      if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
        {
          sprintf(buff, "fill #%02x%02x%02x\n",
                  (cstruct->dc->brush->lbColor[0]& 0x00FF),
                  (cstruct->dc->brush->lbColor[0]& 0xFF00)>>8,
                  (cstruct->dc->brush->lbColor[1]& 0x00FF)
                  );
          ExtendMVG(cstruct, buff);
        }
      else
        ExtendMVG(cstruct, "fill none\n");

      ExtendMVG(cstruct, "stroke none\n");

      /* Add rectangle primitive and points */
      x1 = NormX(rgn->extents.left,cstruct);
      y1 = NormY(rgn->extents.top,cstruct);
      x2 = NormX(rgn->extents.left,cstruct)+ScaleX(rgn->extents.right-rgn->extents.left,cstruct);
      y2 = NormY(rgn->extents.top,cstruct)+ScaleY(rgn->extents.bottom-rgn->extents.top,cstruct);
      sprintf(buff,"rectangle %f,%f %f,%f\n",x1,y1,x2,y2);
      ExtendMVG(cstruct, buff);
      ExtendMVG(cstruct, "pop graphic-context\n");
    }
}

static void WmfParseROP(CSTRUCT *cstruct, unsigned int dwROP,
                        unsigned short x, unsigned short y,
                        unsigned short width, unsigned short height)
{
  /* FIXME: not implemented */

  if (width == 0)
    width = 1;
  if (height == 0)
    height = 1;

  switch (dwROP)
    {
    case PATCOPY:
      /*paint the region with the current brush*/
      break;
    case DSTINVERT:
      break;
    case PATINVERT:
      break;
    case BLACKNESS:
      break;
    case WHITENESS:
      break;
    default:
      break;
    }

  puts("WmfParseROP() not implemented");
}

/* Restore drawing context */
/* static void WmfRestoreUserData(CSTRUCT *cstruct, DC *new) */
/* { */
/*   puts("WmfRestoreUserData()"); */
/* } */

/* Set pixel to specified color (META_SETPIXEL) */
static void WmfSetPixel(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char
    buff[MaxTextExtent];

  ExtendMVG(cstruct, "push graphic-context\n");

  /* Set stroke color */
  sprintf(buff, "stroke #%02x%02x%02x\n",
          (wmfrecord->Parameters[0]& 0x00FF),
          ((wmfrecord->Parameters[0]& 0xFF00)>>8),
          (wmfrecord->Parameters[1]& 0x00FF)
          );
  ExtendMVG(cstruct, buff);

  /* Draw point */
  sprintf(buff, "point %i,%i\n",
          (int)NormX(wmfrecord->Parameters[3],cstruct),
          (int)NormY(wmfrecord->Parameters[2],cstruct));
  ExtendMVG(cstruct, buff);

  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Set scaled output size */
static void WmfSetPmfSize(CSTRUCT *cstruct, HMETAFILE file)
{
  double
    x_resolution,
    y_resolution;
  
  x_resolution=72.0;
  y_resolution=72.0;

  if(IMG_PTR->x_resolution > 0)
    x_resolution=IMG_PTR->x_resolution;

  if(IMG_PTR->y_resolution > 0)
    y_resolution=IMG_PTR->y_resolution;

  cstruct->xpixeling= file->pmh->Inch/x_resolution;
  cstruct->realwidth = floor(((double)x_resolution*file->pmh->Right-file->pmh->Left)/file->pmh->Inch);
  cstruct->ypixeling= file->pmh->Inch/y_resolution;
  cstruct->realheight = floor(((double)y_resolution*file->pmh->Bottom-file->pmh->Top)/file->pmh->Inch);
}

static void WmfSetFillStyle(CSTRUCT *cstruct, LOGBRUSH *brush, DC *currentDC)
{
  /* FIXME: not implemented */

  /* Fill brush style */
  if (brush->lbStyle == BS_NULL)
    return;
 
  switch(brush->lbStyle)
    {
    case BS_NULL:
      break;
    case BS_SOLID:
      break;
    case BS_HATCHED:
      break;
    case BS_PATTERN:
      break;
    case BS_DIBPATTERN:
      break;
    }

  /* Polygon filling mode (META_SETPOLYFILLMODE) */
  switch(cstruct->dc->polyfillmode)
    {
    case ALTERNATE:
      break;
    case WINDING:
      break;
    }

  puts("WmfSetFillStyle() not implemented");
}

static void WmfSetPenStyle(CSTRUCT *cstruct, LOGPEN *pen, DC *currentDC)
{
  /* FIXME: not implemented */

  if (pen->lopnStyle == PS_NULL)
    return;

  switch(cstruct->dc->ROPmode)
    {
    case R2_BLACK :
      /* Boolean 0, clears pixels to black */
      break;
    case R2_WHITE :
      /* Boolean 1, sets pixels to white */
      break;
    case R2_XORPEN :
      /* Source XOR Dest */
      break;
    default:
      {
      }
    }

  if (pen->lopnWidth <= 1)
    {
      switch(pen->lopnStyle & PS_STYLE_MASK)
        {
        case PS_DASH:
          break;
        case PS_DOT:
          break;
        case PS_DASHDOT:
          break;
        case PS_DASHDOTDOT:
          break;
        case PS_ALTERNATE:
        case PS_USERSTYLE:
          break;
        default:
          {
          }
        }
    }

  switch(pen->lopnStyle & PS_ENDCAP_MASK)
    {
    case PS_ENDCAP_SQUARE:
      break;
    case PS_ENDCAP_FLAT:
      break;
    case PS_ENDCAP_ROUND:
      break;
    default:
      {
      }
    }

  switch(pen->lopnStyle & PS_JOIN_MASK)
    {
    case PS_JOIN_BEVEL:
      break;
    case PS_JOIN_MITER:
      break;
    case PS_JOIN_ROUND:
      break;
    default:
      {
      }
    }

  puts("WmfSetPenStyle() not implemented");
}

static Image *ReadWMFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  CSTRUCT*
    cstruct;

  HMETAFILE 
    metafile;

  Image*
    image;

  ImageInfo*
    clone_info;

  char
    buff[MaxTextExtent],
    filename[MaxTextExtent];

  off_t
    filesize;

  image=AllocateImage(image_info);
  GetExceptionInfo(exception);

/*       printf("magick=%.1024s\n", image_info->magick); */
  /* Return WFM hatch images */
/*   if(LocaleCompare(image_info->magick,"WMF") != 0) */
/*     { */
/*       clone_info=CloneImageInfo(image_info); */
/*       (void) strcpy(clone_info->magick,"GIF");  */
/*       if (LocaleCompare(image_info->magick,"HATCH_BDIAGONAL") == 0) */
/*         image=BlobToImage(clone_info,HatchBDIAGONALImage,HatchBDIAGONALImageExtent,exception); */
/*       else if (LocaleCompare(image_info->magick,"HATCH_CROSS") == 0) */
/*         image=BlobToImage(clone_info,HatchCROSSImage,HatchCROSSImageExtent,exception); */
/*       else if (LocaleCompare(image_info->magick,"HATCH_DIAGCROSS") == 0) */
/*         image=BlobToImage(clone_info,HatchDIAGCROSSImage,HatchDIAGCROSSImageExtent,exception); */
/*       else if (LocaleCompare(image_info->magick,"HATCH_FDIAGONAL") == 0) */
/*         image=BlobToImage(clone_info,HatchFDIAGONALImage,HatchFDIAGONALImageExtent,exception); */
/*       else if (LocaleCompare(image_info->magick,"HATCH_HORIZONTAL") == 0) */
/*         image=BlobToImage(clone_info,HatchHORIZONTALImage,HatchHORIZONTALImageExtent,exception); */
/*       else if (LocaleCompare(image_info->magick,"HATCH_VERTICAL") == 0) */
/*         image=BlobToImage(clone_info,HatchVERTICALImage,HatchVERTICALImageExtent,exception); */
/*       else */
/*         image=BlobToImage(clone_info,HatchCROSSImage,HatchCROSSImageExtent,exception); */
/*       DestroyImageInfo(clone_info); */
/*       return(image); */
/*     } */

  wmffunctions = &WmfFunctions;

  /* Allocate and initialize cstruct */
  cstruct=(CSTRUCT *)AcquireMemory(sizeof(CSTRUCT));
  if(cstruct == (CSTRUCT*)NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  memset((void*)cstruct,0,sizeof(CSTRUCT));
  wmfinit(cstruct); /* Simply initializes realwidth & realheight */
  cstruct->userdata=(void*)AcquireMemory(sizeof(ImUserData));
  if(cstruct->userdata == (void*)NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  MVG_PTR=AllocateString("");
  if(MVG_PTR == NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  IMG_PTR=image;

  /* Open metafile */
  strcpy(filename,image_info->filename);
  {
    /* Implement the function of FileIsPlaceable() because it has file
       descriptor leak */
    FILE*
      file;
    
    U32
      testlong;

    file = fopen(filename,"rb");
    if(file == NULL)
      {
        LiberateMemory((void**)&(cstruct->userdata));
        LiberateMemory((void**)&(cstruct));
        ThrowReaderException(FileOpenWarning,"Unable to open file",image);
      }
    testlong = wmfReadU32bit(file);
    fseek(file,0,SEEK_END);
    filesize=ftell(file);
    fclose(file);
    if(testlong == 0x9ac6cdd7)
      {
        /* Placeable metafile */
        metafile = GetPlaceableMetaFile(filename);
        if (metafile != NULL)
          wmffunctions->set_pmf_size(cstruct,metafile);
      }
    else
      {
        /* Ordinary metafile */
        metafile = GetMetaFile(filename);
      }
  }  
  if (metafile == NULL)
    {
      LiberateMemory((void**)&(cstruct->userdata));
      LiberateMemory((void**)&(cstruct));
      ThrowReaderException(FileOpenWarning,"Unable to open file",image);
    }
  
  /* Parse metafile to determine properties */
  cstruct->preparse = 1;
  PlayMetaFile((void *)cstruct,metafile,1,NULL);

  if(!image_info->ping)
    {
      /* Add viewbox primitive to support a stand-alone mvg file */
      sprintf( buff, "viewbox 0 0 %i %i\n",
               (int)cstruct->realwidth, (int)cstruct->realheight );
      ExtendMVG(cstruct, buff);

      /* Create white canvas image */
      clone_info = (ImageInfo*)AcquireMemory(sizeof(ImageInfo));

      GetImageInfo( clone_info );
      sprintf( buff, "%ix%i", (int)cstruct->realwidth, (int)cstruct->realheight );
      CloneString(&(clone_info->size), buff);
      if(image_info->texture == (char*)NULL)
        sprintf(clone_info->filename,"XC:#%02x%02x%02x%02x",
                image_info->background_color.red,
                image_info->background_color.green,
                image_info->background_color.blue,
                image_info->background_color.opacity);
      else
        sprintf(clone_info->filename,"TILE:%.1024s",image_info->texture);
      GetExceptionInfo(exception);
      DestroyImage(image);
      image=ReadImage( clone_info, exception );
      if(image == (Image*)NULL)
        {
          /* Destroy metafile handle (lacks a convenient Destroy function */
          LiberateMemory((void**)&(metafile->wmfheader));
          LiberateMemory((void**)&(metafile->pmh));
          fclose(metafile->filein);
          LiberateMemory((void**)&(metafile));

          /* Destroy cstruct */
          LiberateMemory((void**)&MVG_PTR);
          LiberateMemory((void**)&(cstruct->userdata));
          LiberateMemory((void**)&(cstruct->dc));
          LiberateMemory((void**)&(cstruct));

          /* Destroy other allocations */
          DestroyImageInfo(clone_info);
          DestroyImage(image);
          return image;
        }

      /* Scribble on canvas image */
      IMG_PTR=image;
      cstruct->preparse = 0;
      PlayMetaFile((void *)cstruct,metafile,1,NULL);

      ScribbleMVG(cstruct);
      DestroyImageInfo(clone_info);
    }

  /* Restore original filename and magick */
  strcpy(image->filename,image_info->filename);
  strcpy(image->magick,image_info->magick);
  image->blob->filesize=filesize;
  image->rows=cstruct->realheight;
  image->columns=cstruct->realwidth;


  /* Destroy metafile handle (lacks a convenient Destroy function) */
  LiberateMemory((void**)&(metafile->wmfheader));
  LiberateMemory((void**)&(metafile->pmh));
  fclose(metafile->filein);
  LiberateMemory((void**)&(metafile));

  /* Destroy cstruct handle */
  LiberateMemory((void**)&MVG_PTR);
  LiberateMemory((void**)&(cstruct->userdata));
  LiberateMemory((void**)&(cstruct->dc));
  LiberateMemory((void**)&(cstruct));

  return image;
}
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
#if defined(HasWMF) || defined(HasWMF2)
  MagickInfo
    *entry;

/*   entry=SetMagickInfo("HATCH_BDIAGONAL"); */
/*   entry->decoder=ReadWMFImage; */
/*   entry->description=AllocateString("WMF decoder internal format"); */
/*   entry->stealth=True; */
/*   entry->module=AllocateString("WMF"); */
/*   RegisterMagickInfo(entry); */

/*   entry=SetMagickInfo("HATCH_CROSS"); */
/*   entry->decoder=ReadWMFImage; */
/*   entry->description=AllocateString("WMF decoder internal format"); */
/*   entry->stealth=True; */
/*   entry->module=AllocateString("WMF"); */
/*   RegisterMagickInfo(entry); */

/*   entry=SetMagickInfo("HATCH_DIAGCROSS"); */
/*   entry->decoder=ReadWMFImage; */
/*   entry->description=AllocateString("WMF decoder internal format"); */
/*   entry->stealth=True; */
/*   entry->module=AllocateString("WMF"); */
/*   RegisterMagickInfo(entry); */

/*   entry=SetMagickInfo("HATCH_FDIAGONAL"); */
/*   entry->decoder=ReadWMFImage; */
/*   entry->description=AllocateString("WMF decoder internal format"); */
/*   entry->stealth=True; */
/*   entry->module=AllocateString("WMF"); */
/*   RegisterMagickInfo(entry); */

/*   entry=SetMagickInfo("HATCH_HORIZONTAL"); */
/*   entry->decoder=ReadWMFImage; */
/*   entry->description=AllocateString("WMF decoder internal format"); */
/*   entry->stealth=True; */
/*   entry->module=AllocateString("WMF"); */
/*   RegisterMagickInfo(entry); */

/*   entry=SetMagickInfo("HATCH_VERTICAL"); */
/*   entry->decoder=ReadWMFImage; */
/*   entry->description=AllocateString("WMF decoder internal format"); */
/*   entry->stealth=True; */
/*   entry->module=AllocateString("WMF"); */
/*   RegisterMagickInfo(entry); */

  entry=SetMagickInfo("WMF");
  entry->decoder=ReadWMFImage;
  entry->description=AllocateString("Windows Meta File");
  entry->blob_support=False;
  entry->module=AllocateString("WMF");
  RegisterMagickInfo(entry);
#endif /* defined(HasWMF) || defined(HasWMF2) */
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
#if defined(HasWMF) || defined(HasWMF2)
  UnregisterMagickInfo("WMF");
#endif /* defined(HasWMF) || defined(HasWMF2) */
}
