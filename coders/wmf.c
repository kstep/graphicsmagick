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

/* static void * */
/*   WmfInitialUserData(CSTRUCT *); */

static void
  ExtendMVG(CSTRUCT *cstruct, const char* buff),
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
  ConcatenateString((char**)&cstruct->userdata, buff);
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
/* static void *WmfInitialUserData(CSTRUCT *cstruct) */
/* { */
/*   return (void*)NULL; */
/* } */

/* Set rectangular clipping region */
static void WmfClipRect(CSTRUCT *cstruct)
{
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
  puts("WmfCopyXpm() not implemented");
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
  sprintf(buff, "image %i,%i %i,%i %s\n",
          (int)dest_x,
          (int)dest_y,
          (int)dest_w,
          (int)dest_h,
          filename);
  ExtendMVG(cstruct, buff);
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
      2  ystart, y of endpoint
      3  xstart, x of endpoint
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

  width = cstruct->dc->pen->lopnWidth;

  ExtendMVG(cstruct, "push graphic-context\n");

  yend   = (NormY(wmfrecord->Parameters[0],cstruct));
  xend   = (NormX(wmfrecord->Parameters[1],cstruct));
  ystart = (NormY(wmfrecord->Parameters[2],cstruct));
  xstart = (NormX(wmfrecord->Parameters[3],cstruct));
  bottom = (NormY(wmfrecord->Parameters[4],cstruct));
  right  = (NormX(wmfrecord->Parameters[5],cstruct));
  top    = (NormY(wmfrecord->Parameters[6],cstruct));
  left   = (NormX(wmfrecord->Parameters[7],cstruct));

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
           (float)angle1,
           (float)angle2
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
  puts("WmfDrawText() not implemented");
}

/* static void WmfFrameRgn(CSTRUCT *cstruct,WINEREGION *rgn,U16 width,U16 height) */
/* { */
/*   puts("WmfFrameRgn()"); */
/* } */

/* Extended floodfill. Fill to border color, or fill color at point. */
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

  double
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
  sprintf(buff,"rectangle %f,%f %f,%f\n",x1,y1,x2,y2);
  ExtendMVG(cstruct, buff);

  ExtendMVG(cstruct, "pop graphic-context\n");
}

/* Perform finishing steps */
/* static void WmfFinish(CSTRUCT *cstruct) */
/* { */
/* } */

/* Fill with color until border color */
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
  puts("WmfNoClipRect() not implemented");
}

/* Paint rectangular region using brush color */
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

static void WmfParseROP(CSTRUCT *cstruct, unsigned int dwROP,
                        unsigned short x, unsigned short y,
                        unsigned short width, unsigned short height)
{
  puts("WmfParseROP() not implemented");
}

/* Restore drawing context */
/* static void WmfRestoreUserData(CSTRUCT *cstruct, DC *new) */
/* { */
/*   puts("WmfRestoreUserData()"); */
/* } */

/* Set pixel to specified color */
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
  sprintf(buff, "point %f,%f\n",
          (float)NormX(wmfrecord->Parameters[3],cstruct),
          (float)NormY(wmfrecord->Parameters[2],cstruct));
  ExtendMVG(cstruct, buff);

  ExtendMVG(cstruct, "pop graphic-context\n");
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
  puts("WmfSetFillStyle() not implemented");
}

static void WmfSetPenStyle(CSTRUCT *cstruct, LOGPEN *pen, DC *currentDC)
{
  puts("WmfSetPenStyle() not implemented");
}

static Image *ReadWMFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  CSTRUCT*
    cstruct;

  DrawInfo*
    draw_info;

  HMETAFILE 
    metafile;

  Image*
    image;

  ImageInfo*
    local_info;

  char
    buff[MaxTextExtent],
    filename[MaxTextExtent];

  off_t
    filesize;

  wmffunctions = &WmfFunctions;

  image=AllocateImage(image_info);
  GetExceptionInfo(exception);

  /* Allocate and initialize cstruct */
  cstruct=(CSTRUCT *)AcquireMemory(sizeof(CSTRUCT));
  if(cstruct == (CSTRUCT*)NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  memset((void*)cstruct,0,sizeof(CSTRUCT));
  wmfinit(cstruct);
  (char*)cstruct->userdata=AllocateString("");
  if((char*)cstruct->userdata == (char*)NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  
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
      sprintf( buff, "viewbox 0,0 %i,%i\n",
               (int)cstruct->realwidth, (int)cstruct->realheight );
      ExtendMVG(cstruct, buff);

      /* Create white canvas image */
      local_info = (ImageInfo*)AcquireMemory(sizeof(ImageInfo));
      if(local_info == (ImageInfo*)NULL)
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);

      GetImageInfo( local_info );
      sprintf( buff, "%ix%i", (int)cstruct->realwidth, (int)cstruct->realheight );
      CloneString(&(local_info->size), buff);
      strcpy( local_info->filename, "XC:#FFFFFF" );
      GetExceptionInfo(exception);
      DestroyImage(image);
      image = ReadImage( local_info, exception );
      if(image == (Image*)NULL)
        {
          /* Destroy metafile handle (lacks a convenient Destroy function */
          LiberateMemory((void**)&(metafile->wmfheader));
          LiberateMemory((void**)&(metafile->pmh));
          fclose(metafile->filein);
          LiberateMemory((void**)&(metafile));

          /* Destroy cstruct */
          LiberateMemory((void**)&(cstruct->userdata));
          LiberateMemory((void**)&(cstruct->dc));
          LiberateMemory((void**)&(cstruct));

          /* Destroy other allocations */
          DestroyImageInfo(local_info);
          DestroyImage(image);
          return image;
        }

      /* Scribble on canvas image */
      cstruct->preparse = 0;
      PlayMetaFile((void *)cstruct,metafile,1,NULL);

      draw_info = (DrawInfo*)AcquireMemory(sizeof(DrawInfo));
      GetDrawInfo( local_info, draw_info );
      draw_info->primitive=(char*)cstruct->userdata;
puts(draw_info->primitive);
      DrawImage(image,draw_info);
      draw_info->primitive = (char*)NULL;
      DestroyDrawInfo(draw_info);
      DestroyImageInfo(local_info);
    }

  /* Restore original filename and magick */
  strcpy(image->filename,image_info->filename);
  strcpy(image->magick,image_info->magick);
  image->filesize=filesize;
  image->rows=cstruct->realheight;
  image->columns=cstruct->realwidth;


  /* Destroy metafile handle (lacks a convenient Destroy function) */
  LiberateMemory((void**)&(metafile->wmfheader));
  LiberateMemory((void**)&(metafile->pmh));
  fclose(metafile->filein);
  LiberateMemory((void**)&(metafile));

  /* Destroy cstruct handle */
  LiberateMemory((void**)&(cstruct->userdata));
  LiberateMemory((void**)&(cstruct->dc));
  LiberateMemory((void**)&(cstruct));

  return image;
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
