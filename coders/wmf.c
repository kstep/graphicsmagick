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
%                               December 2000                                 %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

#if defined(HasWMF)
#include <wmfapi.h>

/*
  Forward declarations.
*/
extern wmf_functions WmfFunctions;
extern int currentx;
extern int currenty;

static int
  WmfMmHeight(CSTRUCT *),
  WmfMmWidth(CSTRUCT *),
  WmfPixelHeight(CSTRUCT *),
  WmfPixelWidth(CSTRUCT *);

static void *
  WmfInitialUserData(CSTRUCT *);

static void
  ExtendMVG(CSTRUCT *cstruct, const char* buff),
  WmfClipRect(CSTRUCT *),
  WmfCopyUserData(CSTRUCT *,DC *,DC *),
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
  WmfFinish(CSTRUCT *),
  WmfFloodFill(CSTRUCT *,WMFRECORD *),
  WmfFrameRgn(CSTRUCT *, WINEREGION *rgn,U16,U16),
  WmfFreeUserData(CSTRUCT *,DC *),
  WmfNoClipRect(CSTRUCT *),
  WmfPaintRgn(CSTRUCT *, WINEREGION *rgn),
  WmfParseROP(CSTRUCT *,U32 ,U16 , U16, U16 , U16 ),
  WmfRestoreUserData(CSTRUCT *,DC *),
  WmfSetFillStyle(CSTRUCT *,LOGBRUSH *,DC *),
  WmfSetPenStyle(CSTRUCT *,LOGPEN *,DC *),
  WmfSetPixel(CSTRUCT *,WMFRECORD *),
  WmfSetPmfSize(CSTRUCT *,HMETAFILE );


  /* Libwmf userdata */
typedef struct _IM
{
  char*             mvg;
  Image*            image;
} IM;


#define SCREEN_WIDTH_PIXELS  1280
#define SCREEN_WIDTH_MM      433
#define SCREEN_HEIGHT_PIXELS 1024
#define SCREEN_HEIGHT_MM     347

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
  WmfFinish
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
  if(((IM*)cstruct->userdata)->mvg == NULL)
    CloneString(&((IM*)cstruct->userdata)->mvg, "");
  ConcatenateString(&((IM*)cstruct->userdata)->mvg, buff);
}

/* Return output height of screen in milimeters? */
static int WmfMmHeight(CSTRUCT *cstruct)
{
  return(347);
}

/* Return output width of screen in milimeters? */
static int WmfMmWidth(CSTRUCT *cstruct)
{
  return(433);
}

/* Return output screen height in pixels */
static int WmfPixelHeight(CSTRUCT *cstruct)
{
  return(1024);
}

/* Return output screen width in pixels */
static int WmfPixelWidth(CSTRUCT *cstruct)
{
  return(1280);
}

/* Return initialized drawing context */
static void *WmfInitialUserData(CSTRUCT *cstruct)
{
  return (void*)NULL;
}

/* Set rectangular clipping region */
static void WmfClipRect(CSTRUCT *cstruct)
{
  puts("WmfClipRect()");
}

/* Copy drawing context */
static void WmfCopyUserData(CSTRUCT *cstruct, DC *old, DC *new)
{
  puts("WmfCopyUserData()");
}

static void WmfCopyXpm(CSTRUCT *cstruct,
                       unsigned short src_x, unsigned short src_y,
                       unsigned short dest_x, unsigned short dest_y,
                       unsigned short dest_w, unsigned short dest_h,
                       char *filename, unsigned int dwROP)
{
  puts("WmfCopyXpm()");
}

/* Draw closed (and optionally filled) arc */
static void WmfDrawArc(CSTRUCT *cstruct, WMFRECORD *wmfrecord,
                       int finishtype)
{
  puts("WmfDrawArc()");
}

/* Draw open arc */
static void WmfDrawChord(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  puts("WmfDrawChord()");
}

/* Draw ellipse */
static void WmfDrawEllipse(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char buff[MaxTextExtent];
  float bottom, right, top, left;

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
      ellipse centerX,centerY width,height arcStart,arcEnd

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

  sprintf( buff, "ellipse %f,%f %f,%f %f,%f\n",
           (float)((right-left)/2)+left,
           (float)((bottom-top)/2)+top,
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
  puts("WmfDrawPie()");
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
  puts("WmfDrawPolyPolygon()");
}

/* Draw a rectangle */
static void WmfDrawRectangle(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  char buff[MaxTextExtent];
  double x1, y1, x2, y2;

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
  sprintf(buff,"rectangle %f,%f %f,%f\n",x1,y1,x2,y2);
  ExtendMVG(cstruct, buff);
  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Draw a rectangle with rounded corners */
static void WmfDrawRoundRectangle(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  puts("WmfDrawRoundRectangle()");
}

/* Draw a simple arc */
static void WmfDrawSimpleArc(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  puts("WmfDrawSimpleArc()");
}

/* Draw text */
static void WmfDrawText(CSTRUCT *cstruct, char *str, RECT *arect,
                 U16 flags, U16 *lpDx, int x, int y)
{
  puts("WmfDrawText()");
}

/* Extended floodfill. Fill to border color, or fill color at point. */
static void WmfExtFloodFill(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  puts("WmfExtFloodFill()");
}

static void WmfFillOpaque(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  puts("WmfFillOpaque()");
}

/* Perform finishing steps */
static void WmfFinish(CSTRUCT *cstruct)
{
  ImageInfo*        image_info;
  DrawInfo*         draw_info;
  ExceptionInfo     exception_info;
  Image*            image;
  char*             mvg;
  char buff[MaxTextExtent];

  if( cstruct->preparse == 0 )
    {
      image_info = (ImageInfo*)AcquireMemory(sizeof(ImageInfo));
      GetImageInfo( image_info );
  
      draw_info = (DrawInfo*)AcquireMemory(sizeof(DrawInfo));
      GetDrawInfo( image_info, draw_info );

      GetExceptionInfo( &exception_info );

      draw_info->primitive = ((IM*)cstruct->userdata)->mvg;
      DrawImage( ((IM*)cstruct->userdata)->image, draw_info );
      draw_info->primitive = (char*)NULL;
      LiberateMemory((void**)&((IM*)cstruct->userdata)->mvg);

      DestroyImageInfo(image_info);
      DestroyDrawInfo(draw_info);
    }
}

/* Fill with color until border color */
static void WmfFloodFill(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  puts("WmfFloodFill()");
}

static void WmfNoClipRect(CSTRUCT *cstruct)
{
  puts("WmfNoClipRect()");
}

/* Paint rectangular region using brush color */
static void WmfPaintRgn(CSTRUCT *cstruct, WINEREGION *rgn)
{
  puts("WmfPaintRgn()");
}

static void WmfParseROP(CSTRUCT *cstruct, unsigned int dwROP,
                 unsigned short x, unsigned short y,
                 unsigned short width, unsigned short height)
{
  puts("WmfParseROP()");
}

/* Restore drawing context */
static void WmfRestoreUserData(CSTRUCT *cstruct, DC *new)
{
  puts("WmfRestoreUserData()");
}

/* Set pixel to specified color */
static void WmfSetPixel(CSTRUCT *cstruct, WMFRECORD *wmfrecord)
{
  puts("WmfSetPixel()");
}

/* Set scaled output size */
static void WmfSetPmfSize(CSTRUCT *cstruct, HMETAFILE file)
{
  float pixperin;

  pixperin = ((float)SCREEN_WIDTH_PIXELS)/(SCREEN_WIDTH_MM/MM_PER_INCH);
  cstruct->xpixeling= file->pmh->Inch/pixperin;
  cstruct->realwidth = (abs(file->pmh->Right-file->pmh->Left)/(float)file->pmh->Inch)*pixperin;
  pixperin = ((float)SCREEN_HEIGHT_PIXELS)/(SCREEN_HEIGHT_MM/MM_PER_INCH);
  cstruct->ypixeling= file->pmh->Inch/pixperin;
  cstruct->realheight = (abs(file->pmh->Bottom-file->pmh->Top)/(float)file->pmh->Inch)*pixperin;
}

static void WmfSetFillStyle(CSTRUCT *cstruct, LOGBRUSH *brush, DC *currentDC)
{
  puts("WmfSetFillStyle()");
}

static void WmfSetPenStyle(CSTRUCT *cstruct, LOGPEN *pen, DC *currentDC)
{
  puts("WmfSetPenStyle()");
}

static Image *ReadWMFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char *in;
  HMETAFILE file;
  CSTRUCT rstruct;
  CSTRUCT *cstruct = &rstruct;
  int brect[8];
  char buff[MaxTextExtent];
  IM* im;
  ImageInfo* local_info;
  Image* image;

  image=AllocateImage(image_info);

  wmfinit(cstruct);
  wmffunctions = &WmfFunctions;

  cstruct->userdata = (void *)&im;
  ((IM*)cstruct->userdata)->mvg=AcquireMemory(sizeof(char)*MaxTextExtent);
  strcpy(((IM*)cstruct->userdata)->mvg,"");
  ((IM*)cstruct->userdata)->image=(Image*)NULL;

  if (1 == FileIsPlaceable(image_info->filename))
    {
      file = GetPlaceableMetaFile(image_info->filename);
      if (file != NULL)
        wmffunctions->set_pmf_size(cstruct,file);
    }
  else
    file = GetMetaFile(image_info->filename);

  if (file == NULL)
    {
      LiberateMemory((void**)&((IM*)cstruct->userdata)->mvg);
      ThrowReaderException(FileOpenWarning,"Unable to open file",image);
    }

  /* Inspect WMF file properties */
  cstruct->preparse = 1;
  PlayMetaFile((void *)cstruct,file,1,NULL);

  /* Create white canvas image */
  local_info = (ImageInfo*)AcquireMemory(sizeof(ImageInfo));
  if(local_info == (char*)NULL)
    {
      LiberateMemory((void**)&((IM*)cstruct->userdata)->mvg);
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
    }
  GetImageInfo( local_info );
  sprintf( buff, "%ix%i", (int)cstruct->realwidth, (int)cstruct->realheight );
  CloneString(&local_info->size, buff);
  strcpy( local_info->filename, "XC:#FFFFFF" );
  GetExceptionInfo(&exception);
  ((IM*)cstruct->userdata)->image = ReadImage( local_info, &exception );
  if(((IM*)cstruct->userdata)->image == (Image*)NULL)
    {
      LiberateMemory((void**)&file->pmh);
      LiberateMemory((void**)&(file->wmfheader));
      LiberateMemory((void**)&(file));
      LiberateMemory((void**)&((IM*)cstruct->userdata)->mvg);
      DestroyImageInfo(local_info);
      DestroyImage(image);
      return image;
    }

  /* Scribble on canvas image */
  if ( ((IM*)cstruct->userdata)->image != (Image*)NULL )
    {
      cstruct->preparse = 0;
      PlayMetaFile((void *)cstruct,file,1,NULL);
    }

  /* Preserve original filename and magick */
  strcpy(((IM*)cstruct->userdata)->image->filename,image_info->filename);
  strcpy(((IM*)cstruct->userdata)->image->magick,image_info->magick);

  LiberateMemory((void**)&file->pmh);
  LiberateMemory((void**)&(file->wmfheader));
  LiberateMemory((void**)&(file));

  DestroyImageInfo(local_info);
  DestroyImage(image);

  return ((IM*)cstruct->userdata)->image;
}
#endif

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
  entry->blob_support=False;
  entry->description=AllocateString("Windows Meta File");
  entry->module=AllocateString("WMF");
  RegisterMagickInfo(entry);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r R G B I m a g e                                       %
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
  UnregisterMagickInfo("WMF");
#endif
}
