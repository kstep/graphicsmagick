/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           AAA   N   N  N   N   OOO   TTTTT   AAA   TTTTT  EEEEE             %
%          A   A  NN  N  NN  N  O   O    T    A   A    T    E                 %
%          AAAAA  N N N  N N N  O   O    T    AAAAA    T    EEE               %
%          A   A  N  NN  N  NN  O   O    T    A   A    T    E                 %
%          A   A  N   N  N   N   OOO     T    A   A    T    EEEEE             %
%                                                                             %
%                                                                             %
%                  ImageMagick Image Annotation Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
% Digital Applications (www.digapp.com) contributed the stroked text algorithm.
% It was written by Leonard Rosenthol.
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#if defined(HasX11)
#include "xwindows.h"
#endif
#if defined(HasTTF)
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#endif

/*
  Forward declarations.
*/
static unsigned int
  RenderFont(Image *,const DrawInfo *,const PointInfo *,
    const unsigned int,FontMetrics *),
  RenderPostscript(Image *,const DrawInfo *,const PointInfo *,
    const unsigned int,FontMetrics *),
  RenderTruetype(Image *,const DrawInfo *,const PointInfo *,
    const unsigned int,FontMetrics *),
  RenderX11(Image *,const DrawInfo *,const PointInfo *,const unsigned int,
    FontMetrics *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n n o t a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AnnotateImage annotates an image with text.  Optionally the
%  annotation can include the image filename, type, width, height, or scene
%  number by embedding special format characters (e.g. %f for image filename).
%
%  The format of the AnnotateImage method is:
%
%      unsigned int AnnotateImage(Image *image,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o status: Method AnnotateImage returns True if the image is annotated
%      otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%
*/
MagickExport unsigned int AnnotateImage(Image *image,const DrawInfo *draw_info)
{
  char
    primitive[MaxTextExtent],
    *text,
    **textlist;

  DrawInfo
    *annotate,
    *clone_info;

  FontMetrics
    metrics;

  int
    x,
    y;

  PointInfo
    offset;

  register int
    i;

  unsigned int
    height,
    length,
    matte,
    number_lines,
    status,
    width;

  /*
    Translate any embedded format characters (e.g. %f).
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if (draw_info->text == (char *) NULL)
    return(False);
  if (*draw_info->text == '\0')
    return(False);
  text=TranslateText((ImageInfo *) NULL,image,draw_info->text);
  if (text == (char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to annotate image",
      "Memory allocation failed");
  textlist=StringToList(text);
  LiberateMemory((void **) &text);
  if (textlist == (char **) NULL)
    return(False);
  length=Extent(textlist[0]);
  for (i=1; textlist[i] != (char *) NULL; i++)
    if (Extent(textlist[i]) > (int) length)
      length=Extent(textlist[i]);
  number_lines=i;
  text=(char *) AcquireMemory(length+MaxTextExtent);
  if (text == (char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to annotate image",
      "Memory allocation failed");
  width=image->columns;
  height=image->rows;
  x=0;
  y=0;
  if (draw_info->geometry != (char *) NULL)
    {
      unsigned int
        flags;

      /*
        User specified annotation geometry.
      */
      flags=ParseGeometry(draw_info->geometry,&x,&y,&width,&height);
      if ((flags & XNegative) != 0)
        x+=image->columns;
      if ((flags & WidthValue) == 0)
        width-=2*x > width ? width : 2*x;
      if ((flags & YNegative) != 0)
        y+=image->rows;
      if ((flags & HeightValue) == 0)
        height-=2*y > height ? height : 2*y;
    }
  annotate=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  if ((annotate->fill.opacity == TransparentOpacity) &&
      (annotate->stroke.opacity == TransparentOpacity))
    QueryColorDatabase("black",&annotate->fill);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->primitive=AllocateString(primitive);
  matte=image->matte;
  status=True;
  for (i=0; textlist[i] != (char *) NULL; i++)
  {
    if (*textlist[i] == '\0')
      continue;
    /*
      Position text relative to image.
    */
    (void) CloneString(&annotate->text,textlist[i]);
    status=GetFontMetrics(image,annotate,&metrics);
    if (status == False)
      break;
    switch (annotate->gravity)
    {
      case NorthWestGravity:
      {
        offset.x=x+i*draw_info->affine.ry*metrics.height;
        offset.y=y+i*draw_info->affine.sy*metrics.height;
        break;
      }
      case NorthGravity:
      {
        offset.x=x+0.5*width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width/2;
        offset.y=y+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width/2;
        break;
      }
      case NorthEastGravity:
      {
        offset.x=x+width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width+1;
        offset.y=y+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width;
        break;
      }
      case WestGravity:
      {
        offset.x=x+i*draw_info->affine.ry*metrics.height+
          draw_info->affine.ry*(metrics.ascent+metrics.descent)/2;
        offset.y=y+0.5*height+i*draw_info->affine.sy*metrics.height+
          draw_info->affine.sy*(metrics.ascent+metrics.descent)/2+1;
        break;
      }
      case ForgetGravity:
      case StaticGravity:
      case CenterGravity:
      default:
      {
        offset.x=x+0.5*width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width/2+draw_info->affine.ry*
          (metrics.ascent+metrics.descent)/2;
        offset.y=y+0.5*height+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width/2+draw_info->affine.sy*
          (metrics.ascent+metrics.descent)/2+1;
        break;
      }
      case EastGravity:
      {
        offset.x=x+width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width+draw_info->affine.ry*
          (metrics.ascent+metrics.descent)/2+1;
        offset.y=y+0.5*height+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width+draw_info->affine.sy*
          (metrics.ascent+metrics.descent)/2+1;
        break;
      }
      case SouthWestGravity:
      {
        offset.x=x+i*draw_info->affine.ry*metrics.height+
          draw_info->affine.ry*(metrics.ascent+metrics.descent);
        offset.y=y+height+i*draw_info->affine.sy*metrics.height+
          draw_info->affine.sy*(metrics.ascent+metrics.descent)+1;
        break;
      }
      case SouthGravity:
      {
        offset.x=x+0.5*width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width/2+draw_info->affine.ry*
          (metrics.ascent+metrics.descent);
        offset.y=y+height+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width/2+draw_info->affine.sy*
          (metrics.ascent+metrics.descent)+1;
        break;
      }
      case SouthEastGravity:
      {
        offset.x=x+width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width+draw_info->affine.ry*
          (metrics.ascent+metrics.descent)+1;
        offset.y=y+height+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width+draw_info->affine.sy*
          (metrics.ascent+metrics.descent)+1;
        break;
      }
    }
    if (draw_info->box.opacity != TransparentOpacity)
      {
        /*
          Text box.
        */
        clone_info->fill=draw_info->box;
        clone_info->affine.tx=offset.x-draw_info->affine.ry*metrics.ascent;
        clone_info->affine.ty=offset.y-draw_info->affine.sy*metrics.ascent;
        FormatString(primitive,"rect 0,0 %d,%d",(int) metrics.width,
          (int) metrics.height);
        CloneString(&clone_info->primitive,primitive);
        DrawImage(image,clone_info);
      }
    /*
      Annotate image with text.
    */
    status=RenderFont(image,annotate,&offset,True,&metrics);
    if (status == False)
      break;
    if (annotate->decorate == NoDecoration)
      continue;
    /*
      Decorate text.
    */
    clone_info->affine.tx=offset.x;
    clone_info->affine.ty=offset.y;
    if (annotate->decorate == OverlineDecoration)
      {
        clone_info->affine.tx-=draw_info->affine.ry*
          (metrics.ascent+metrics.descent)+1;
        clone_info->affine.ty-=draw_info->affine.sy*
          (metrics.ascent+metrics.descent)+1;
      }
    else
      if (annotate->decorate == UnderlineDecoration)
        {
          clone_info->affine.tx++;
          clone_info->affine.ty++;
        }
      else
        {
          clone_info->affine.tx-=draw_info->affine.ry*
            (metrics.ascent+metrics.descent)/2;
          clone_info->affine.ty-=draw_info->affine.sy*
            (metrics.ascent+metrics.descent)/2;
        }
    clone_info->fill=draw_info->fill;
    FormatString(primitive,"line 0,0 %d,0",(int) metrics.width);
    CloneString(&clone_info->primitive,primitive);
    DrawImage(image,clone_info);
  }
  image->matte=matte;
  /*
    Free resources.
  */
  DestroyDrawInfo(clone_info);
  DestroyDrawInfo(annotate);
  LiberateMemory((void **) &text);
  for (i=0; textlist[i] != (char *) NULL; i++)
    LiberateMemory((void **) &textlist[i]);
  LiberateMemory((void **) &textlist);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t F o n t M e t r i c s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetFontMetrics returns the following information for the specified
%  font and text:
%
%    o character width, expressed in integer pixels
%    o character height, expressed in integer pixels
%    o ascent, expressed in 26.6 fixed point pixels
%    o descent, expressed in 26.6 fixed point pixels
%    o text width, expressed in 26.6 fixed point pixels
%    o text height, expressed in 26.6 fixed point pixels
%    o maximum horizontal advance, expressed in 26.6 fixed point pixels
%
%  The format of the GetFontMetrics method is:
%
%      unsigned int GetFontMetrics(Image *image,
%        const DrawInfo *draw_info,FontInfo metrics)
%
%  A description of each parameter follows:
%
%    o status: Method GetFontMetrics returns True if the metrics are
%      available otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: Specifies a pointer to a DrawInfo structure.
%
%    o metrics: Method GetFontMetrics returns the font metrics.
%
%
*/
MagickExport unsigned int GetFontMetrics(Image *image,
  const DrawInfo *draw_info,FontMetrics *metrics)
{
  PointInfo
    offset;

  unsigned int
    status;

  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->text != (char *) NULL);
  assert(draw_info->signature == MagickSignature);
  offset.x=0.0;
  offset.y=0.0;
  status=RenderFont(image,draw_info,&offset,False,metrics);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r F o n t                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderFont renders text on the image.  It also returns the bounding
%  box of the text relative to the image.
%
%  The format of the RenderFont method is:
%
%      unsigned int RenderFont(Image *image,DrawInfo *draw_info,
%        const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderFont returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%    o offset: (x,y) location of text relative to image.
%
%    o render: a value other than zero renders the text otherwise just the
%      font metric is returned.
%
%    o metrics: bounding box of text.
%
%
*/
static unsigned int RenderFont(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
{
  DrawInfo
    *clone_info;

  ImageInfo
    *image_info;

  unsigned int
    status;

  if (draw_info->font == (char *) NULL)
    return(RenderPostscript(image,draw_info,offset,render,metrics));
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strcpy(image_info->filename,draw_info->font);
  (void) strcpy(image_info->magick,"PS");
  if (*image_info->filename == '@')
    (void) strcpy(image_info->magick,"TTF");
  if (*image_info->filename == '-')
    (void) strcpy(image_info->magick,"X");
  (void) SetImageInfo(image_info,False);
  clone_info=CloneDrawInfo(image_info,draw_info);
  if (*image_info->filename != '@')
    CloneString(&clone_info->font,image_info->filename);
  else
    CloneString(&clone_info->font,image_info->filename+1);
  if (LocaleCompare(image_info->magick,"ps") == 0)
    status=RenderPostscript(image,clone_info,offset,render,metrics);
  else
    if ((LocaleCompare(image_info->magick,"ttf") == 0) ||
        (LocaleCompare(image_info->magick,"afm") == 0) ||
        (LocaleCompare(image_info->magick,"pfb") == 0) ||
        (LocaleCompare(image_info->magick,"pfm") == 0))
      status=RenderTruetype(image,clone_info,offset,render,metrics);
    else
      if (LocaleCompare(image_info->magick,"x") == 0)
        status=RenderX11(image,clone_info,offset,render,metrics);
      else
        status=RenderPostscript(image,clone_info,offset,render,metrics);
  DestroyDrawInfo(clone_info);
  DestroyImageInfo(image_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r P o s t s c r i p t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderPostscript renders text on the image with a Postscript font.
%  It also returns the bounding box of the text relative to the image.
%
%  The format of the RenderPostscript method is:
%
%      unsigned int RenderPostscript(Image *image,DrawInfo *draw_info,
%        const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderPostscript returns True if the text is rendered on
%      the image, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%    o offset: (x,y) location of text relative to image.
%
%    o render: a value other than zero renders the text otherwise just the
%      font metric is returned.
%
%    o metrics: bounding box of text.
%
%
*/

static char *EscapeParenthesis(const char *text)
{
  int
    escapes;

  register char
    *p;

  register int
    i;

  static char
    buffer[MaxTextExtent];

  escapes=0;
  p=buffer;
  for (i=0; i < Min((int) strlen(text),(MaxTextExtent-escapes-1)); i++)
  {
    if ((text[i] == '(') || (text[i] == ')'))
      {
        *p++='\\';
        escapes++;
      }
    *p++=text[i];
  }
  *p='\0';
  return(buffer);
}

static unsigned int RenderPostscript(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent];

  FILE
    *file;

  ExceptionInfo
    exception;

  Image
    *annotate_image;

  ImageInfo
    *clone_info;

  int
    y;

  PointInfo
    extent,
    point,
    resolution;

  register int
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    identity;

  /*
    Render label with a Postscript font.
  */
  TemporaryFilename(filename);
  file=fopen(filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    ThrowBinaryException(FileOpenWarning,"Unable to open file",filename);
  (void) fprintf(file,"%%!PS-Adobe-3.0\n");
  (void) fprintf(file,"/ReencodeFont\n");
  (void) fprintf(file,"{\n");
  (void) fprintf(file,"  findfont dup length\n");
  (void) fprintf(file,
    "  dict begin { 1 index /FID ne {def} {pop pop} ifelse } forall\n");
  (void) fprintf(file,
    "  /Encoding ISOLatin1Encoding def currentdict end definefont pop\n");
  (void) fprintf(file,"} bind def\n");
  /*
    Sample to compute bounding box.
  */
  identity=(draw_info->affine.sx == draw_info->affine.sy) &&
    (draw_info->affine.rx == 0.0) && (draw_info->affine.ry == 0.0);
  extent.x=0.0;
  extent.y=0.0;
  for (i=0; i <= (Extent(draw_info->text)+2); i++)
  {
    point.x=fabs(draw_info->affine.sx*i*draw_info->pointsize+
      draw_info->affine.ry*2.0*draw_info->pointsize);
    point.y=fabs(draw_info->affine.rx*i*draw_info->pointsize+
      draw_info->affine.sy*2.0*draw_info->pointsize);
    if (point.x > extent.x)
      extent.x=point.x;
    if (point.y > extent.y)
      extent.y=point.y;
  }
  (void) fprintf(file,"%g %g moveto\n",identity ? 0.0 : extent.x/2.0,
    extent.y/2.0);
  (void) fprintf(file,"%g %g scale\n",draw_info->pointsize,
    draw_info->pointsize);
  if ((draw_info->font == (char *) NULL) || (*draw_info->font == '\0'))
    (void) fprintf(file,
      "/Times-ISO dup /Times ReencodeFont findfont setfont\n");
  else
    (void) fprintf(file,
      "/%.1024s-ISO dup /%.1024s ReencodeFont findfont setfont\n",
      draw_info->font,draw_info->font);
  (void) fprintf(file,"[%g %g %g %g 0 0] concat\n",draw_info->affine.sx,
    -draw_info->affine.rx,-draw_info->affine.ry,
    draw_info->affine.sy);
  if (!identity)
    (void) fprintf(file,"(%.1024s) stringwidth pop -0.5 mul -0.5 rmoveto\n",
      EscapeParenthesis(draw_info->text));
  (void) fprintf(file,"(%.1024s) show\n",EscapeParenthesis(draw_info->text));
  (void) fprintf(file,"showpage\n");
  (void) fclose(file);
  FormatString(geometry,"%dx%d+0+0!",(int) ceil(extent.x-0.5),
    (int) ceil(extent.y-0.5));
  clone_info=CloneImageInfo((ImageInfo *) NULL);
  (void) FormatString(clone_info->filename,"ps:%.1024s",filename);
  (void) CloneString(&clone_info->page,geometry);
  if (draw_info->density != (char *) NULL)
    (void) CloneString(&clone_info->density,draw_info->density);
  GetExceptionInfo(&exception);
  annotate_image=ReadImage(clone_info,&exception);
  DestroyImageInfo(clone_info);
  DestroyExceptionInfo(&exception);
  (void) remove(filename);
  if (annotate_image == (Image *) NULL)
    return(False);
  resolution.x=72.0;
  resolution.y=72.0;
  if (draw_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(draw_info->density,"%lfx%lf",&resolution.x,
        &resolution.y);
      if (count != 2)
        resolution.y=resolution.x;
    }
  if (!identity)
    TransformImage(&annotate_image,"0x0",(char *) NULL);
  else
    {
      RectangleInfo
        crop_info;

      crop_info=GetImageBoundingBox(annotate_image);
      crop_info.height=(unsigned int) ceil((resolution.y/72.0)*
        ExpandAffine(&draw_info->affine)*draw_info->pointsize-0.5);
      crop_info.y=(int) ceil((resolution.y/72.0)*extent.y/8.0-0.5);
      (void) FormatString(geometry,"%ux%u%+d%+d",crop_info.width,
        crop_info.height,crop_info.x,crop_info.y);
      TransformImage(&annotate_image,geometry,(char *) NULL);
    }
  metrics->pixels_per_em.x=(resolution.y/72.0)*
    ExpandAffine(&draw_info->affine)*draw_info->pointsize;
  metrics->pixels_per_em.y=metrics->pixels_per_em.x;
  metrics->ascent=metrics->pixels_per_em.x;
  metrics->descent=(int) metrics->pixels_per_em.y/-5;
  metrics->width=annotate_image->columns/ExpandAffine(&draw_info->affine);
  metrics->height=1.152*metrics->pixels_per_em.x;
  metrics->max_advance=metrics->pixels_per_em.x;
  if (!render)
    {
      DestroyImage(annotate_image);
      return(True);
    }
  if (draw_info->fill.opacity != TransparentOpacity)
    {
      PixelPacket
        fill_color;

      /*
        Render fill color.
      */
      annotate_image->matte=True;
      fill_color=draw_info->fill;
      for (y=0; y < (int) annotate_image->rows; y++)
      {
        q=GetImagePixels(annotate_image,0,y,annotate_image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) annotate_image->columns; x++)
        {
          q->opacity=MaxRGB-((unsigned long) ((MaxRGB-Intensity(*q))*
            (MaxRGB-fill_color.opacity))/MaxRGB);
          q->red=fill_color.red;
          q->green=fill_color.green;
          q->blue=fill_color.blue;
          q++;
        }
        if (!SyncImagePixels(annotate_image))
          break;
      }
      CompositeImage(image,OverCompositeOp,annotate_image,(int)
        ceil(offset->x-0.5),(int) ceil(offset->y-(metrics->ascent+
        metrics->descent)-0.5));
    }
  DestroyImage(annotate_image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r T r u e t y p e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderTruetype renders text on the image with a Truetype font.  It
%  also returns the bounding box of the text relative to the image.
%
%  The format of the RenderTruetype method is:
%
%      unsigned int RenderTruetype(Image *image,DrawInfo *draw_info,
%        const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderTruetype returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%    o offset: (x,y) location of text relative to image.
%
%    o render: a value other than zero renders the text otherwise just the
%      font metric is returned.
%
%    o metrics: bounding box of text.
%
%
*/

#if defined(HasTTF)

static int TraceCubicBezier(FT_Vector *p,FT_Vector *q,FT_Vector *to,
  DrawInfo *draw_info)
{
  char
    path[MaxTextExtent];

  FormatString(path,"C%g,%g %g,%g %g,%g",(p->x/64.0),(-p->y/64.0),(q->x/64.0),
    (-q->y/64.0),(to->x/64.0),(-to->y/64.0));
  ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceLineTo(FT_Vector *to,DrawInfo *draw_info)
{
  char
    path[MaxTextExtent];

  FormatString(path,"L%g,%g",(to->x/64.0),(-to->y/64.0));
  ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceMoveTo(FT_Vector *to,DrawInfo *draw_info)
{
  char
    path[MaxTextExtent];

  FormatString(path,"M%g,%g",(to->x/64.0),(-to->y/64.0));
  ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceQuadraticBezier(FT_Vector *control,FT_Vector *to,
  DrawInfo *draw_info)
{
  char
    path[MaxTextExtent];

  FormatString(path,"Q%g,%g %g,%g",(control->x/64.0),(-control->y/64.0),
    (to->x/64.0),(-to->y/64.0));
  ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static unsigned int RenderTruetype(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
{
  typedef struct _GlyphInfo
  {
    FT_UInt
      id;

    FT_Vector
      origin;

    FT_Glyph
      image;
  } GlyphInfo;

  char
    filename[MaxTextExtent];

  DrawInfo
    *clone_info;

  FT_BBox
    bounding_box;

  FT_BitmapGlyph
    bitmap;

  FT_Error
    status;

  FT_Matrix
    affine;

  FT_Vector
    origin;

  GlyphInfo
    glyph,
    last_glyph;

  int
    length,
    y;

  PointInfo
    point,
    resolution;

  Quantum
    opacity;

  register int
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  SegmentInfo
    extent;

  static char
    font[MaxTextExtent];

  static FT_Face
    face = (FT_Face) NULL;

  static FT_Library
    library = (FT_Library) NULL;

  static FT_Outline_Funcs
    OutlineMethods =
    {
      (FT_Outline_MoveTo_Func) TraceMoveTo,
      (FT_Outline_LineTo_Func) TraceLineTo,
      (FT_Outline_ConicTo_Func) TraceQuadraticBezier,
      (FT_Outline_CubicTo_Func) TraceCubicBezier,
      0,
      0
    };

  unsigned short
    *unicode;

  /*
    Initialize Truetype library.
  */
  if (library == (FT_Library) NULL)
    {
      status=FT_Init_FreeType(&library);
      if (status)
        ThrowBinaryException(DelegateWarning,"Unable to open freetype library",
          draw_info->font);
    }
  if ((face == (FT_Face) NULL) ||
      (LocaleCompare(draw_info->font,font) != 0))
    {
      register char
        *p,
        *q;

      /*
        Search for Truetype font filename.
      */
      if (face != (FT_Face) NULL)
        FT_Done_Face(face);
      status=True;
      p=getenv("TT_FONT_PATH");
#if defined(TT_FONT_PATH)
      if (p != (char *) NULL)
        p=TT_FONT_PATH;
#endif
      if (p != (char *) NULL)
        for ( ; ; )
        {
          /*
            Environment variable TT_FONT_PATH.
          */
          q=strchr(p,DirectoryListSeparator);
          if (q == (char *) NULL)
            (void) strcpy(filename,p);
          else
            {
              (void) strncpy(filename,p,q-p);
              filename[q-p]='\0';
            }
          i=strlen(filename);
          if ((i > 0) && (!IsBasenameSeparator(filename[i-1])))
            (void) strcat(filename,DirectorySeparator);
          (void) strcat(filename,draw_info->font);
          status=FT_New_Face(library,filename,0,&face);
          if (!status || (q == (char *) NULL) || (*q == '\0'))
            break;
          p=q+1;
        }
      if (status)
        status=FT_New_Face(library,draw_info->font,0,&face);
      if (status)
        ThrowBinaryException(DelegateWarning,"Unable to read font",
          draw_info->font);
    }
  /*
    Set text size.
  */
  resolution.x=72.0;
  resolution.y=72.0;
  if (draw_info->density != (char *) NULL)
    {
      i=sscanf(draw_info->density,"%lfx%lf",&resolution.x,&resolution.y);
      if (i != 2)
        resolution.y=resolution.x;
    }
  (void) FT_Set_Char_Size(face,(long int) (64.0*draw_info->pointsize),
    (long int) (64.0*draw_info->pointsize),(unsigned int) resolution.x,
    (unsigned int) resolution.y);
  /*
    Convert to Unicode.
  */
  unicode=ConvertTextToUnicode(draw_info->text,&length);
  if (unicode == (unsigned short *) NULL)
    {
      FT_Done_FreeType(library);
      ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
        "Memory allocation failed");
    }
  /*
    Compute bounding box.
  */
  origin.x=0;
  origin.y=0;
  extent.x1=32000;
  extent.x2=(-32000);
  extent.y1=32000;
  extent.y2=(-32000);
  affine.xx=(FT_Fixed) (65536.0*draw_info->affine.sx);
  affine.yx=(FT_Fixed) (-65536.0*draw_info->affine.rx);
  affine.xy=(FT_Fixed) (-65536.0*draw_info->affine.ry);
  affine.yy=(FT_Fixed) (65536.0*draw_info->affine.sy);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  CloneString(&clone_info->primitive,"");
  for (i=0; i < length; i++)
  {
    glyph.id=FT_Get_Char_Index(face,unicode[i]);
    if ((glyph.id != 0) && (last_glyph.id != 0) && FT_HAS_KERNING(face))
      {
        FT_Vector
          delta;

        FT_Get_Kerning(face,last_glyph.id,glyph.id,ft_kerning_default,&delta);
        origin.x+=delta.x;
      }
    glyph.origin=origin;
    status=FT_Load_Glyph(face,glyph.id,FT_LOAD_DEFAULT);
    if (status != False)
      continue;
    status=FT_Get_Glyph(face->glyph,&glyph.image);
    if (status != False)
      continue;
    FT_Vector_Transform(&glyph.origin,&affine);
    if (render && (clone_info->stroke.opacity != TransparentOpacity))
      {
        /*
          Trace the glyph.
        */
        clone_info->affine.tx=offset->x+(glyph.origin.x >> 6);
        clone_info->affine.ty=offset->y-(glyph.origin.y >> 6);
        CloneString(&clone_info->primitive,"path '");
        status=FT_Outline_Decompose(&((FT_OutlineGlyph) glyph.image)->outline,
          &OutlineMethods,clone_info);
        if (status == False)
          {
            /*
              Draw the glyph.
            */
            ConcatenateString(&clone_info->primitive,"'");
            DrawImage(image,clone_info);
          }
      }
    FT_Glyph_Transform(glyph.image,&affine,&glyph.origin);
    if ((clone_info->fill.opacity != TransparentOpacity) &&
        (clone_info->stroke.opacity == TransparentOpacity) &&  render)
      {
        /*
          Rasterize the glyph.
        */
        status=FT_Glyph_To_Bitmap(&glyph.image,ft_render_mode_normal,
          (FT_Vector *) NULL,False);
        if (status != False)
          continue;
        bitmap=(FT_BitmapGlyph) glyph.image;
        point.x=offset->x+bitmap->left;
        point.y=offset->y-bitmap->top;
        p=bitmap->bitmap.buffer;
        for (y=0; y < bitmap->bitmap.rows; y++)
        {
          if ((ceil(point.y+y-0.5) < 0) || (ceil(point.y+y-0.5) >= image->rows))
            {
              p+=bitmap->bitmap.width;
              continue;
            }
          for (x=0; x < bitmap->bitmap.width; x++)
          {
            if ((ceil(point.x+x-0.5) < 0) ||
                (ceil(point.x+x-0.5) >= image->columns) || (*p == 0))
              {
                p++;
                continue;
              }
            q=GetImagePixels(image,(int) ceil(point.x+x-0.5),
              (int) ceil(point.y+y-0.5),1,1);
            if (q == (PixelPacket *) NULL)
              {
                p++;
                continue;
              }
            opacity=MaxRGB-((unsigned long) (UpScale(*p)*
              (MaxRGB-clone_info->fill.opacity))/MaxRGB);
            q->red=((unsigned long) (clone_info->fill.red*(MaxRGB-opacity)+
              q->red*opacity)/MaxRGB);
            q->green=((unsigned long) (clone_info->fill.green*(MaxRGB-opacity)+
              q->green*opacity)/MaxRGB);
            q->blue=((unsigned long) (clone_info->fill.blue*(MaxRGB-opacity)+
              q->blue*opacity)/MaxRGB);
            q->opacity=((unsigned long) (opacity*(MaxRGB-opacity)+
              q->opacity*opacity)/MaxRGB);
            p++;
            if (!SyncImagePixels(image))
              continue;
          }
        }
      }
    FT_Glyph_Get_CBox(glyph.image,ft_glyph_bbox_pixels,&bounding_box);
    if (bounding_box.xMin < extent.x1)
      extent.x1=bounding_box.xMin;
    if (bounding_box.xMax > extent.x2)
      extent.x2=bounding_box.xMax;
    if (bounding_box.yMin < extent.y1)
      extent.y1=bounding_box.yMin;
    if (bounding_box.yMax > extent.y2)
      extent.y2=bounding_box.yMax;
    origin.x+=face->glyph->advance.x;
    FT_Done_Glyph(glyph.image);
    last_glyph=glyph;
  }
  metrics->pixels_per_em.x=face->size->metrics.x_ppem;
  metrics->pixels_per_em.y=face->size->metrics.y_ppem;
  metrics->ascent=face->size->metrics.ascender >> 6;
  metrics->descent=face->size->metrics.descender >> 6;
  metrics->width=Max(origin.x >> 6,extent.x2);
  metrics->height=face->size->metrics.height >> 6;
  metrics->max_advance=face->size->metrics.max_advance >> 6;
  /*
    Free resources.
  */
  LiberateMemory((void **) &unicode);
  DestroyDrawInfo(clone_info);
  return(True);
}
#else
static unsigned int RenderTruetype(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
{
  ThrowBinaryException(MissingDelegateWarning,
    "FreeType library is not available",draw_info->font);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r X 1 1                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderX11 renders text on the image with an X11 font.  It also
%  returns the bounding box of the text relative to the image.
%
%  The format of the RenderX11 method is:
%
%      unsigned int RenderX11(Image *image,DrawInfo *draw_info,
%        const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderX11 returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%    o offset: (x,y) location of text relative to image.
%
%    o render: a value other than zero renders the text otherwise just the
%      font metric is returned.
%
%    o metrics: bounding box of text.
%
%
*/
#if defined(HasX11)
static unsigned int RenderX11(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
{
  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  static DrawInfo
    cache_info;

  static Display
    *display = (Display *) NULL;

  static XAnnotateInfo
    xdraw_info;

  static XFontStruct
    *font_info;

  static XPixelInfo
    pixel;

  static XResourceInfo
    resource_info;

  static XrmDatabase
    resource_database;

  static XStandardColormap
    *map_info;

  static XVisualInfo
    *visual_info;

  unsigned int
    height,
    status,
    width;

  if (display == (Display *) NULL)
    {
      char
        *client_name;

      /*
        Open X server connection.
      */
      display=XOpenDisplay(draw_info->server_name);
      if (display == (Display *) NULL)
        ThrowBinaryException(XServerWarning,"Unable to open X server",
          draw_info->server_name);
      /*
        Get user defaults from X resource database.
      */
      XSetErrorHandler(XError);
      client_name=SetClientName((char *) NULL);
      resource_database=XGetResourceDatabase(display,client_name);
      XGetResourceInfo(resource_database,client_name,&resource_info);
      resource_info.close_server=False;
      resource_info.colormap=PrivateColormap;
      resource_info.font=AllocateString(draw_info->font);
      resource_info.background_color=AllocateString("white");
      resource_info.foreground_color=AllocateString("black");
      map_info=XAllocStandardColormap();
      if (map_info == (XStandardColormap *) NULL)
        ThrowBinaryException(ResourceLimitWarning,"Unable to allocate colormap",
          "Memory allocation failed");
      /*
        Initialize visual info.
      */
      visual_info=XBestVisualInfo(display,map_info,&resource_info);
      if (visual_info == (XVisualInfo *) NULL)
        ThrowBinaryException(XServerWarning,"Unable to get visual",
          draw_info->server_name);
      map_info->colormap=(Colormap) NULL;
      pixel.pixels=(unsigned long *) NULL;
      pixel.gamma_map=(XColor *) NULL;
      /*
        Initialize Standard Colormap info.
      */
      XGetMapInfo(visual_info,XDefaultColormap(display,visual_info->screen),
        map_info);
      XGetPixelPacket(display,visual_info,map_info,&resource_info,
        (Image *) NULL,&pixel);
      pixel.annotate_context=XDefaultGC(display,visual_info->screen);
      /*
        Initialize font info.
      */
      font_info=XBestFont(display,&resource_info,False);
      if (font_info == (XFontStruct *) NULL)
        ThrowBinaryException(XServerWarning,"Unable to load font",
          draw_info->font);
      if ((map_info == (XStandardColormap *) NULL) ||
          (visual_info == (XVisualInfo *) NULL) ||
          (font_info == (XFontStruct *) NULL))
        {
          XFreeResources(display,visual_info,map_info,&pixel,font_info,
            &resource_info,(XWindowInfo *) NULL);
          ThrowBinaryException(XServerWarning,"Unable to get X server font",
            draw_info->server_name);
        }
      cache_info=(*draw_info);
    }
  /*
    Initialize annotate info.
  */
  XGetAnnotateInfo(&xdraw_info);
  xdraw_info.stencil=OpaqueStencil;
  if (cache_info.font != draw_info->font)
    {
      /*
        Font name has changed.
      */
      XFreeFont(display,font_info);
      (void) CloneString(&resource_info.font,draw_info->font);
      font_info=XBestFont(display,&resource_info,False);
      if (font_info == (XFontStruct *) NULL)
        ThrowBinaryException(ResourceLimitWarning,"Unable to load font",
          draw_info->font);
    }
  cache_info=(*draw_info);
  xdraw_info.font_info=font_info;
  xdraw_info.text=(char *) draw_info->text;
  xdraw_info.width=XTextWidth(font_info,draw_info->text,
    Extent(draw_info->text));
  xdraw_info.height=font_info->ascent+font_info->descent;
  metrics->pixels_per_em.x=font_info->max_bounds.width;
  metrics->pixels_per_em.y=font_info->max_bounds.width;
  metrics->ascent=font_info->ascent;
  metrics->descent=(-font_info->descent);
  metrics->width=xdraw_info.width/ExpandAffine(&draw_info->affine);
  metrics->height=metrics->pixels_per_em.x+4;
  metrics->max_advance=font_info->max_bounds.width;
  if (!render)
    return(True);
  if (draw_info->fill.opacity != TransparentOpacity)
    {
      /*
        Render fill color.
      */
      width=xdraw_info.width;
      height=xdraw_info.height;
      if ((draw_info->affine.rx != 0.0) || (draw_info->affine.ry != 0.0))
        {
          if (((draw_info->affine.sx-draw_info->affine.sy) == 0.0) &&
              ((draw_info->affine.rx+draw_info->affine.ry) == 0.0))
            xdraw_info.degrees=(180.0/M_PI)*
              atan2(draw_info->affine.rx,draw_info->affine.sx);
        }
      FormatString(xdraw_info.geometry,"%ux%u+%d+%d",width,height,
        (int) ceil(offset->x-0.5),(int) ceil(offset->y-metrics->ascent-
        metrics->descent-1.5));
      pixel.pen_color.red=XUpScale(draw_info->fill.red);
      pixel.pen_color.green=XUpScale(draw_info->fill.green);
      pixel.pen_color.blue=XUpScale(draw_info->fill.blue);
      status=XAnnotateImage(display,&pixel,&xdraw_info,image);
      if (status == 0)
        ThrowBinaryException(ResourceLimitWarning,"Unable to annotate image",
          "Memory allocation failed");
    }
  return(True);
}
#else
static unsigned int RenderX11(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,FontMetrics *metrics)
{
  ThrowBinaryException(MissingDelegateWarning,
    "X11 library is not available",draw_info->font);
}
#endif
