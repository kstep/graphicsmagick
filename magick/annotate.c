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
#include "define.h"
#if defined(HasX11)
#include "xwindow.h"
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
  RenderType(Image *,const DrawInfo *,const PointInfo *,
    const unsigned int,TypeMetric *),
  RenderPostscript(Image *,const DrawInfo *,const PointInfo *,
    const unsigned int,TypeMetric *),
  RenderTruetype(Image *,const DrawInfo *,const PointInfo *,
    const unsigned int,TypeMetric *),
  RenderX11(Image *,const DrawInfo *,const PointInfo *,const unsigned int,
    TypeMetric *);

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
%  AnnotateImage() annotates an image with text.  Optionally you can include
%  any of the following bits of information about the image by embedding
%  the appropriate special characters:
%
%    %b   file size in bytes.
%    %c   comment.
%    %d   directory in which the image resides.
%    %e   extension of the image file.
%    %f   original filename of the image.
%    %h   height of image.
%    %i   filename of the image.
%    %k   number of unique colors.
%    %l   image label.
%    %m   image file format.
%    %n   number of images in a image sequence.
%    %o   output image filename.
%    %p   page number of the image.
%    %q   image depth (8 or 16).
%    %p   page number of the image.
%    %q   image depth (8 or 16).
%    %s   image scene number.
%    %t   image filename without any extension.
%    %u   a unique temporary filename.
%    %w   image width.
%    %x   x resolution of the image.
%    %y   y resolution of the image.
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
%    o image: The image.
%
%    o draw_info: The draw info.
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

  PointInfo
    offset;

  RectangleInfo
    geometry;

  register int
    i;

  size_t
    length;

  TypeMetric
    metrics;

  unsigned int
    matte,
    status;

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
  length=strlen(textlist[0]);
  for (i=1; textlist[i] != (char *) NULL; i++)
    if (strlen(textlist[i]) > length)
      length=strlen(textlist[i]);
  text=(char *) AcquireMemory(length+MaxTextExtent);
  if (text == (char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to annotate image",
      "Memory allocation failed");
  SetGeometry(image,&geometry);
  if (draw_info->geometry != (char *) NULL)
    (void) GetGeometry(draw_info->geometry,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
  annotate=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
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
    status=GetTypeMetrics(image,annotate,&metrics);
    if (status == False)
      break;
    switch (annotate->gravity)
    {
      case ForgetGravity:
      case NorthWestGravity:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*metrics.height;
        offset.y=geometry.y+i*draw_info->affine.sy*metrics.height;
        break;
      }
      case NorthGravity:
      {
        offset.x=geometry.x+geometry.width/2+i*draw_info->affine.ry*
          metrics.height-draw_info->affine.sx*metrics.width/2;
        offset.y=geometry.y+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width/2;
        break;
      }
      case NorthEastGravity:
      {
        offset.x=geometry.width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width-geometry.x;
        offset.y=geometry.y+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width;
        break;
      }
      case WestGravity:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*metrics.height+
          draw_info->affine.ry*(metrics.ascent+metrics.descent)/2;
        offset.y=geometry.y+geometry.height/2+i*draw_info->affine.sy*
          metrics.height+draw_info->affine.sy*(metrics.ascent+
          metrics.descent)/2;
        break;
      }
      case StaticGravity:
      case CenterGravity:
      default:
      {
        offset.x=geometry.x+geometry.width/2+i*draw_info->affine.ry*
          metrics.height-draw_info->affine.sx*metrics.width/2+
          draw_info->affine.ry*(metrics.ascent+metrics.descent)/2;
        offset.y=geometry.y+geometry.height/2+i*draw_info->affine.sy*
          metrics.height-draw_info->affine.rx*metrics.width/2+
          draw_info->affine.sy*(metrics.ascent+metrics.descent)/2;
        break;
      }
      case EastGravity:
      {
        offset.x=geometry.width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width+draw_info->affine.ry*
          (metrics.ascent+metrics.descent)/2-geometry.x;
        offset.y=geometry.y+geometry.height/2+i*draw_info->affine.sy*
          metrics.height-draw_info->affine.rx*metrics.width+
          draw_info->affine.sy*(metrics.ascent+metrics.descent)/2;
        break;
      }
      case SouthWestGravity:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*metrics.height+
          draw_info->affine.ry*(metrics.ascent+metrics.descent);
        offset.y=geometry.height+i*draw_info->affine.sy*metrics.height-
          geometry.y;
        break;
      }
      case SouthGravity:
      {
        offset.x=geometry.width/2+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width/2+draw_info->affine.ry*
          (metrics.ascent+metrics.descent)-geometry.x;
        offset.y=geometry.height+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width/2-geometry.y;
        break;
      }
      case SouthEastGravity:
      {
        offset.x=geometry.width+i*draw_info->affine.ry*metrics.height-
          draw_info->affine.sx*metrics.width+draw_info->affine.ry*
          (metrics.ascent+metrics.descent)-geometry.x;
        offset.y=geometry.height+i*draw_info->affine.sy*metrics.height-
          draw_info->affine.rx*metrics.width-geometry.y;
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
        FormatString(primitive,"rectangle 0,0 %g,%g",metrics.width,
          metrics.height);
        (void) CloneString(&clone_info->primitive,primitive);
        (void) DrawImage(image,clone_info);
      }
    clone_info->affine.tx=offset.x;
    clone_info->affine.ty=offset.y;
    FormatString(primitive,"stroke-width %g line 0,0 %g,0",
      metrics.underline_thickness,metrics.width);
    if (annotate->decorate == OverlineDecoration)
      {
        clone_info->affine.ty-=draw_info->affine.sy*
          (metrics.ascent+metrics.descent)-metrics.underline_position;
        (void) CloneString(&clone_info->primitive,primitive);
        (void) DrawImage(image,clone_info);
      }
    else
      if (annotate->decorate == UnderlineDecoration)
        {
          clone_info->affine.ty-=metrics.underline_position;
          (void) CloneString(&clone_info->primitive,primitive);
          (void) DrawImage(image,clone_info);
        }
    /*
      Annotate image with text.
    */
    status=RenderType(image,annotate,&offset,True,&metrics);
    if (status == False)
      break;
    if (annotate->decorate == LineThroughDecoration)
      {
        clone_info->affine.ty-=draw_info->affine.sy*
          (metrics.ascent+metrics.descent)/2.0-metrics.underline_position/2.0;
        (void) CloneString(&clone_info->primitive,primitive);
        (void) DrawImage(image,clone_info);
      }
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
%   C o n v e r t T e x t T o U n i c o d e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvertTextToUnicode converts an ASCII text string to Unicode and
%  returns the Unicode translation and the character count.  Normal characters
%  are just copied.
%
%  The format of the ConvertTextToUnicode function is:
%
%      unicode=ConvertTextToUnicode(text,count)
%
%  A description of each parameter follows:
%
%    o unicode:  Method ConvertTextToUnicode returns a pointer to an
%      unsigned short array representing the Unicode translation of the
%      ASCII string.
%
%    o text: Specifies a pointer to a character array that contains the
%      text to convert.
%
%    o count: The number of characters that were translated from ASCII to
%      Unicode is returned in this unsigned integer pointer.
%
%
*/

static int GetUnicodeCharacter(const unsigned char *text,size_t *length)
{
  unsigned int
    c;

  if (*length < 1)
    return(-1);
  c=text[0];
  if (!(c & 0x80))
    {
      *length=1;
      return(c);
    }
  if ((*length < 2) || ((text[1] & 0xc0) != 0x80))
    {
      *length=0;
      return(-1);
    }
  if ((c & 0xe0) != 0xe0)
    {
      *length=2;
      c=(text[0] & 0x1f) << 6;
      c|=text[1] & 0x3f;
      return(c);
    }
  if ((*length < 3) || ((text[2] & 0xc0) != 0x80))
    {
      *length=0;
      return(-1);
    }
  if ((c & 0xf0) != 0xf0)
    {
      *length=3;
      c=(text[0] & 0xf) << 12;
      c|=(text[1] & 0x3f) << 6;
      c|=text[2] & 0x3f;
      return(c);
    }
  if ((*length < 4) || ((c & 0xf8) != 0xf0) || ((text[3] & 0xc0) != 0x80))
    {
      *length=0;
      return(-1);
    }
  *length=4;
  c=(text[0] & 0x7) << 18;
  c|=(text[1] & 0x3f) << 12;
  c|=(text[2] & 0x3f) << 6;
  c|=text[3] & 0x3f;
  return(c);
}

static unsigned short *ConvertTextToUnicode(const char *text,size_t *count)
{
  int
    c;

  register int
    i;

  register const char
    *p;

  register unsigned short
    *q;

  size_t
    length;

  unsigned short
    *unicode;

  *count=0;
  if ((text == (char *) NULL) || (*text == '\0'))
    return((unsigned short *) NULL);
  unicode=(unsigned short *)
    AcquireMemory((strlen(text)+MaxTextExtent)*sizeof(unsigned short));
  if (unicode == (unsigned short *) NULL)
    MagickError(ResourceLimitError,"Unable to convert text to Unicode",
      "Memory allocation failed");
  q=unicode;
  for (p=text; *p != '\0'; p+=length)
  {
    length=strlen(p);
    c=GetUnicodeCharacter((const unsigned char *) p,&length);
    if (c < 0)
      {
        *count=strlen(text);
        for (i=0; i < (long) *count; i++)
          unicode[i]=(unsigned char) text[i];
        return(unicode);
      }
    *q=(unsigned short) c;
    q++;
  }
  *count=q-unicode;
  return(unicode);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t T y p e M e t r i c s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetTypeMetrics() returns the following information for the specified font
%  and text:
%
%    o character width
%    o character height
%    o ascent
%    o descent
%    o text width
%    o text height
%    o maximum horizontal advance
%    o underline position
%    o underline thickness
%
%  The format of the GetTypeMetrics method is:
%
%      unsigned int GetTypeMetrics(Image *image,const DrawInfo *draw_info,
%        TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o metrics: Return the font metrics in this structure.
%
%
*/
MagickExport unsigned int GetTypeMetrics(Image *image,const DrawInfo *draw_info,
  TypeMetric *metrics)
{
  PointInfo
    offset;

  unsigned int
    status;

  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->text != (char *) NULL);
  assert(draw_info->signature == MagickSignature);
  memset(metrics,0,sizeof(TypeMetric));
  offset.x=0.0;
  offset.y=0.0;
  status=RenderType(image,draw_info,&offset,False,metrics);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r T y p e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderType renders text on the image.  It also returns the bounding
%  box of the text relative to the image.
%
%  The format of the RenderType method is:
%
%      unsigned int RenderType(Image *image,DrawInfo *draw_info,
%        const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderType returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
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
static unsigned int RenderType(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
{
  const TypeInfo
    *type_info;

  DrawInfo
    *clone_info;

  ImageInfo
    *image_info;

  unsigned int
    status;

  image_info=CloneImageInfo((ImageInfo *) NULL);
  if (draw_info->font != (char *) NULL)
    (void) strncpy(image_info->filename,draw_info->font,MaxTextExtent-1);
  type_info=GetTypeInfo(draw_info->font,&image->exception);
  if ((draw_info->family != (char *) NULL) &&
      (draw_info->style != (char *) NULL) &&
      (draw_info->weight != (char *) NULL))
    type_info=GetTypeInfoByFamily(draw_info->family,draw_info->style,
      draw_info->weight,&image->exception);
  if ((type_info != (const TypeInfo *) NULL) &&
      (type_info->glyphs != (char *) NULL))
    (void) strncpy(image_info->filename,type_info->glyphs,MaxTextExtent-1);
  (void) strcpy(image_info->magick,"PS");
  if (*image_info->filename == '@')
    (void) strcpy(image_info->magick,"TTF");
  if (*image_info->filename == '-')
    (void) strcpy(image_info->magick,"X");
  (void) SetImageInfo(image_info,False,&image->exception);
  clone_info=CloneDrawInfo(image_info,draw_info);
  if (*image_info->filename != '@')
    (void) CloneString(&clone_info->font,image_info->filename);
  else
    (void) CloneString(&clone_info->font,image_info->filename+1);
  if ((type_info != (const TypeInfo *) NULL) &&
      (type_info->encoding != (char *) NULL))
    (void) CloneString(&clone_info->encoding,type_info->encoding);
  if (LocaleCompare(image_info->magick,"PS") == 0)
    status=RenderPostscript(image,clone_info,offset,render,metrics);
  else
    if ((LocaleCompare(image_info->magick,"TTF") == 0) ||
        (LocaleCompare(image_info->magick,"AFM") == 0) ||
        (LocaleCompare(image_info->magick,"PFA") == 0) ||
        (LocaleCompare(image_info->magick,"PFB") == 0) ||
        (LocaleCompare(image_info->magick,"PFM") == 0))
      status=RenderTruetype(image,clone_info,offset,render,metrics);
    else
      if (LocaleCompare(image_info->magick,"X") == 0)
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
%        const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderPostscript returns True if the text is rendered on
%      the image, otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
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
  char
    *buffer;

  register char
    *p;

  register long
    i;

  unsigned long
    escapes;

  escapes=0;
  buffer=AllocateString(text);
  p=buffer;
  for (i=0; i < (long) Min(strlen(text),(MaxTextExtent-escapes-1)); i++)
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
  const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    *text;

  FILE
    *file;

  Image
    *annotate_image,
    *pattern;

  ImageInfo
    *clone_info;

  long
    y;

  PointInfo
    extent,
    point,
    resolution;

  register long
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
  (void) fprintf(file,"/ReencodeType\n");
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
  for (i=0; i <= (long) (strlen(draw_info->text)+2); i++)
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
      "/Times-Roman-ISO dup /Times-Roman ReencodeType findfont setfont\n");
  else
    (void) fprintf(file,
      "/%.1024s-ISO dup /%.1024s ReencodeType findfont setfont\n",
      draw_info->font,draw_info->font);
  (void) fprintf(file,"[%g %g %g %g 0 0] concat\n",draw_info->affine.sx,
    -draw_info->affine.rx,-draw_info->affine.ry,draw_info->affine.sy);
  text=EscapeParenthesis(draw_info->text);
  if (!identity)
    (void) fprintf(file,"(%.1024s) stringwidth pop -0.5 mul -0.5 rmoveto\n",
      text);
  (void) fprintf(file,"(%.1024s) show\n",text);
  LiberateMemory((void **) &text);
  (void) fprintf(file,"showpage\n");
  (void) fclose(file);
  FormatString(geometry,"%ldx%ld+0+0!",(long) ceil(extent.x-0.5),
    (long) ceil(extent.y-0.5));
  clone_info=CloneImageInfo((ImageInfo *) NULL);
  (void) FormatString(clone_info->filename,"ps:%.1024s",filename);
  (void) CloneString(&clone_info->page,geometry);
  if (draw_info->density != (char *) NULL)
    (void) CloneString(&clone_info->density,draw_info->density);
  clone_info->antialias=draw_info->text_antialias;
  annotate_image=ReadImage(clone_info,&image->exception);
  if (image->exception.severity != UndefinedException)
    MagickWarning(image->exception.severity,image->exception.reason,
      image->exception.description);
  DestroyImageInfo(clone_info);
  (void) remove(filename);
  if (annotate_image == (Image *) NULL)
    return(False);
  resolution.x=72.0;
  resolution.y=72.0;
  if (draw_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(draw_info->density,"%lfx%lf",&resolution.x,&resolution.y);
      if (count != 2)
        resolution.y=resolution.x;
    }
  if (!identity)
    TransformImage(&annotate_image,"0x0",(char *) NULL);
  else
    {
      RectangleInfo
        crop_info;

      crop_info=GetImageBoundingBox(annotate_image,&annotate_image->exception);
      crop_info.height=(unsigned int) ceil((resolution.y/72.0)*
        ExpandAffine(&draw_info->affine)*draw_info->pointsize-0.5);
      crop_info.y=(long) ceil((resolution.y/72.0)*extent.y/8.0-0.5);
      (void) FormatString(geometry,"%lux%lu%+ld%+ld",crop_info.width,
        crop_info.height,crop_info.x,crop_info.y);
      TransformImage(&annotate_image,geometry,(char *) NULL);
    }
  metrics->pixels_per_em.x=(resolution.y/72.0)*
    ExpandAffine(&draw_info->affine)*draw_info->pointsize;
  metrics->pixels_per_em.y=metrics->pixels_per_em.x;
  metrics->ascent=metrics->pixels_per_em.x;
  metrics->descent=metrics->pixels_per_em.y/-5.0;
  metrics->width=annotate_image->columns/ExpandAffine(&draw_info->affine);
  metrics->height=1.152*metrics->pixels_per_em.x;
  metrics->max_advance=metrics->pixels_per_em.x;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=(-2.0);
  metrics->underline_thickness=1.0;
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
      SetImageType(annotate_image,TrueColorMatteType);
      fill_color=draw_info->fill;
      pattern=draw_info->fill_pattern;
      for (y=0; y < (long) annotate_image->rows; y++)
      {
        q=GetImagePixels(annotate_image,0,y,annotate_image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) annotate_image->columns; x++)
        {
          if (pattern != (Image *) NULL)
            fill_color=GetOnePixel(pattern,
              (x-pattern->tile_info.x) % pattern->columns,
              (y-pattern->tile_info.y) % pattern->rows);
          q->opacity=(Quantum) (MaxRGB-((unsigned long) ((MaxRGB-Intensity(q))*
            (MaxRGB-fill_color.opacity))/MaxRGB));
          q->red=fill_color.red;
          q->green=fill_color.green;
          q->blue=fill_color.blue;
          q++;
        }
        if (!SyncImagePixels(annotate_image))
          break;
      }
      (void) CompositeImage(image,OverCompositeOp,annotate_image,(long)
        ceil(offset->x-0.5),(long) ceil(offset->y-(metrics->ascent+
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
%        const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderTruetype returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
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
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  FormatString(path,"C%g,%g %g,%g %g,%g",affine.tx+p->x/64.0,
    affine.ty-p->y/64.0,affine.tx+q->x/64.0,affine.ty-q->y/64.0,
    affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceLineTo(FT_Vector *to,DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  FormatString(path,"L%g,%g",affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceMoveTo(FT_Vector *to,DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  FormatString(path,"M%g,%g",affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceQuadraticBezier(FT_Vector *control,FT_Vector *to,
  DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  FormatString(path,"Q%g,%g %g,%g",affine.tx+control->x/64.0,
    affine.ty-control->y/64.0,affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static unsigned int RenderTruetype(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
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
    font[MaxTextExtent];

  DrawInfo
    *clone_info;

  FT_BBox
    bounds;

  FT_BitmapGlyph
    bitmap;

  FT_Error
    status;

  FT_Face
    face;

  FT_Library
    library;

  FT_Matrix
    affine;

  FT_Vector
    origin;

  GlyphInfo
    glyph,
    last_glyph;

  Image
    *pattern;

  long
    y;

  PixelPacket
    fill_color;

  PointInfo
    point,
    resolution;

  Quantum
    opacity;

  register long
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  size_t
    length;

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

  unsigned int
    active;

  unsigned short
    *unicode;

  /*
    Initialize Truetype library.
  */
  status=FT_Init_FreeType(&library);
  if (status)
    ThrowBinaryException(DelegateWarning,"Unable to open freetype library",
      draw_info->font);
  status=FT_New_Face(library,draw_info->font,0,&face);
  if (status)
    ThrowBinaryException(DelegateWarning,"Unable to read font",
      draw_info->font);
  (void) strncpy(font,draw_info->font,MaxTextExtent-1);
  if (draw_info->encoding != (char *) NULL)
    {
      if (LocaleCompare(draw_info->encoding,"AdobeCustom") == 0)
        FT_Select_Charmap(face,ft_encoding_adobe_custom);
      if (LocaleCompare(draw_info->encoding,"AdobeExpert") == 0)
        FT_Select_Charmap(face,ft_encoding_adobe_expert);
      if (LocaleCompare(draw_info->encoding,"AdobeStandard") == 0)
        FT_Select_Charmap(face,ft_encoding_adobe_standard);
      if (LocaleCompare(draw_info->encoding,"AppleRoman") == 0)
        FT_Select_Charmap(face,ft_encoding_apple_roman);
      if (LocaleCompare(draw_info->encoding,"BIG5") == 0)
        FT_Select_Charmap(face,ft_encoding_big5);
      if (LocaleCompare(draw_info->encoding,"GB2312") == 0)
        FT_Select_Charmap(face,ft_encoding_gb2312);
      if (LocaleCompare(draw_info->encoding,"Latin 2") == 0)
        FT_Select_Charmap(face,ft_encoding_latin_2);
      if (LocaleCompare(draw_info->encoding,"None") == 0)
        FT_Select_Charmap(face,ft_encoding_none);
      if (LocaleCompare(draw_info->encoding,"SJIS") == 0)
        FT_Select_Charmap(face,ft_encoding_sjis);
      if (LocaleCompare(draw_info->encoding,"Symbol") == 0)
        FT_Select_Charmap(face,ft_encoding_symbol);
      if (LocaleCompare(draw_info->encoding,"Unicode") == 0)
        FT_Select_Charmap(face,ft_encoding_unicode);
      if (LocaleCompare(draw_info->encoding,"Wansung") == 0)
        FT_Select_Charmap(face,ft_encoding_wansung);
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
  (void) FT_Set_Char_Size(face,(FT_F26Dot6) (64.0*draw_info->pointsize),
    (FT_F26Dot6) (64.0*draw_info->pointsize),(FT_UInt) resolution.x,
    (FT_UInt) resolution.y);
  metrics->pixels_per_em.x=face->size->metrics.x_ppem;
  metrics->pixels_per_em.y=face->size->metrics.y_ppem;
  metrics->ascent=(double) face->size->metrics.ascender/64.0;
  metrics->descent=(double) face->size->metrics.descender/64.0;
  metrics->width=0;
  metrics->height=(double) face->size->metrics.height/64.0;
  metrics->max_advance=(double) face->size->metrics.max_advance/64.0;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=face->underline_position/64.0;
  metrics->underline_thickness=face->underline_thickness/64.0;
  /*
    Convert to Unicode.
  */
  unicode=ConvertTextToUnicode(draw_info->text,&length);
  if (unicode == (unsigned short *) NULL)
    {
      (void) FT_Done_Face(face);
      (void) FT_Done_FreeType(library);
      ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
        "Memory allocation failed")
    }
  /*
    Compute bounding box.
  */
  glyph.id=0;
  last_glyph.id=0;
  origin.x=0;
  origin.y=0;
  affine.xx=(FT_Fixed) (65536.0*draw_info->affine.sx);
  affine.yx=(FT_Fixed) (-65536.0*draw_info->affine.rx);
  affine.xy=(FT_Fixed) (-65536.0*draw_info->affine.ry);
  affine.yy=(FT_Fixed) (65536.0*draw_info->affine.sy);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) QueryColorDatabase("#000000ff",&clone_info->fill);
  (void) CloneString(&clone_info->primitive,"path '");
  pattern=draw_info->fill_pattern;
  for (i=0; i < (long) length; i++)
  {
    glyph.id=FT_Get_Char_Index(face,unicode[i]);
    if ((glyph.id != 0) && (last_glyph.id != 0) && FT_HAS_KERNING(face))
      {
        FT_Vector
          delta;

        (void) FT_Get_Kerning(face,last_glyph.id,glyph.id,ft_kerning_default,
          &delta);
        origin.x+=delta.x;
      }
    glyph.origin=origin;
    status=FT_Load_Glyph(face,glyph.id,FT_LOAD_DEFAULT);
    if (status != False)
      continue;
    status=FT_Get_Glyph(face->glyph,&glyph.image);
    if (status != False)
      continue;
    FT_Glyph_Get_CBox(glyph.image,0,&bounds);
    if ((i == 0) || (bounds.xMin < metrics->bounds.x1))
      metrics->bounds.x1=bounds.xMin;
    if ((i == 0) || (bounds.yMin < metrics->bounds.y1))
      metrics->bounds.y1=bounds.yMin;
    if ((i == 0) || (bounds.xMax > metrics->bounds.x2))
      metrics->bounds.x2=bounds.xMax;
    if ((i == 0) || (bounds.yMax > metrics->bounds.y2))
      metrics->bounds.y2=bounds.yMax;
    if (render)
      if ((draw_info->stroke.opacity != TransparentOpacity) ||
          (draw_info->stroke_pattern != (Image *) NULL))
        {
          /*
            Trace the glyph.
          */
          clone_info->affine.tx=glyph.origin.x/64.0;
          clone_info->affine.ty=glyph.origin.y/64.0;
          (void) FT_Outline_Decompose(&((FT_OutlineGlyph)
            glyph.image)->outline,&OutlineMethods,clone_info);
        }
    FT_Vector_Transform(&glyph.origin,&affine);
    (void) FT_Glyph_Transform(glyph.image,&affine,&glyph.origin);
    if (render)
      {
        if ((draw_info->fill.opacity != TransparentOpacity) ||
            (pattern != (Image *) NULL))
          {
            /*
              Rasterize the glyph.
            */
            status=FT_Glyph_To_Bitmap(&glyph.image,ft_render_mode_normal,
              (FT_Vector *) NULL,True);
            if (status != False)
              continue;
            bitmap=(FT_BitmapGlyph) glyph.image;
            SetImageType(image,TrueColorMatteType);
            point.x=offset->x+bitmap->left;
            point.y=offset->y-bitmap->top;
            p=bitmap->bitmap.buffer;
            for (y=0; y < (long) bitmap->bitmap.rows; y++)
            {
              if ((ceil(point.y+y-0.5) < 0) ||
                  (ceil(point.y+y-0.5) >= image->rows))
                {
                  p+=bitmap->bitmap.width;
                  continue;
                }
              q=GetImagePixels(image,(long) ceil(point.x-0.5),
                (long) ceil(point.y+y-0.5),bitmap->bitmap.width,1);
              active=q != (PixelPacket *) NULL;
              for (x=0; x < (long) bitmap->bitmap.width; x++)
              {
                if (((long) ceil(point.x+x-0.5) < 0) || (*p == 0) ||
                    ((unsigned long) ceil(point.x+x-0.5) >= image->columns))
                  {
                    p++;
                    q++;
                    continue;
                  }
                if (draw_info->text_antialias)
                  opacity=Upscale(*p);
                else
                  opacity=(Quantum)
                    ((*p) >= 64 ? TransparentOpacity : OpaqueOpacity);
                fill_color=draw_info->fill;
                if (pattern != (Image *) NULL)
                  fill_color=GetOnePixel(pattern,
                    (long) (point.x+x-pattern->tile_info.x) % pattern->columns,
                    (long) (point.y+y-pattern->tile_info.y) % pattern->rows);
                opacity=(Quantum) ((unsigned long) ((MaxRGB-opacity)*
                  (MaxRGB-fill_color.opacity))/MaxRGB);
                if (!active)
                  q=GetImagePixels(image,(long) ceil(point.x+x-0.5),
                    (long) ceil(point.y+y-0.5),1,1);
                if (q == (PixelPacket *) NULL)
                  {
                    p++;
                    q++;
                    continue;
                  }
                *q=AlphaComposite(&fill_color,opacity,q,q->opacity);
                if (!active)
                  (void) SyncImagePixels(image);
                p++;
                q++;
              }
              (void) SyncImagePixels(image);
            }
          }
      }
    origin.x+=face->glyph->advance.x;
    if (origin.x > metrics->width)
      metrics->width=origin.x;
    if (last_glyph.id != 0)
      FT_Done_Glyph(last_glyph.image);
    last_glyph=glyph;
  }
  metrics->width/=64.0;
  metrics->bounds.x1/=64.0;
  metrics->bounds.y1/=64.0;
  metrics->bounds.x2/=64.0;
  metrics->bounds.y2/=64.0;
  if (render)
    if ((draw_info->stroke.opacity != TransparentOpacity) ||
        (draw_info->stroke_pattern != (Image *) NULL))
      {
        /*
          Draw text stroke.
        */
        clone_info->affine.tx=offset->x;
        clone_info->affine.ty=offset->y;
        (void) ConcatenateString(&clone_info->primitive,"'");
        (void) DrawImage(image,clone_info);
      }
  if (glyph.id != 0)
    FT_Done_Glyph(glyph.image);
  /*
    Free resources.
  */
  LiberateMemory((void **) &unicode);
  DestroyDrawInfo(clone_info);
  (void) FT_Done_Face(face);
  (void) FT_Done_FreeType(library);
  return(True);
}
#else
static unsigned int RenderTruetype(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
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
%        const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderX11 returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
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
  const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
{
  static DrawInfo
    cache_info;

  static Display
    *display = (Display *) NULL;

  static XAnnotateInfo
    annotate_info;

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
      (void) XSetErrorHandler(XError);
      client_name=SetClientName((char *) NULL);
      resource_database=XGetResourceDatabase(display,client_name);
      XGetResourceInfo(resource_database,client_name,&resource_info);
      resource_info.close_server=False;
      resource_info.colormap=PrivateColormap;
      resource_info.font=AllocateString(draw_info->font);
      resource_info.background_color=AllocateString("#ffffff");
      resource_info.foreground_color=AllocateString("#000000");
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
            draw_info->server_name)
        }
      cache_info=(*draw_info);
    }
  /*
    Initialize annotate info.
  */
  XGetAnnotateInfo(&annotate_info);
  annotate_info.stencil=ForegroundStencil;
  if (cache_info.font != draw_info->font)
    {
      /*
        Type name has changed.
      */
      (void) XFreeFont(display,font_info);
      (void) CloneString(&resource_info.font,draw_info->font);
      font_info=XBestFont(display,&resource_info,False);
      if (font_info == (XFontStruct *) NULL)
        ThrowBinaryException(ResourceLimitWarning,"Unable to load font",
          draw_info->font);
    }
  cache_info=(*draw_info);
  annotate_info.font_info=font_info;
  annotate_info.text=(char *) draw_info->text;
  annotate_info.width=XTextWidth(font_info,draw_info->text,
    (int) strlen(draw_info->text));
  annotate_info.height=font_info->ascent+font_info->descent;
  metrics->pixels_per_em.x=font_info->max_bounds.width;
  metrics->pixels_per_em.y=font_info->max_bounds.width;
  metrics->ascent=font_info->ascent;
  metrics->descent=(-font_info->descent);
  metrics->width=annotate_info.width/ExpandAffine(&draw_info->affine);
  metrics->height=metrics->pixels_per_em.x+4;
  metrics->max_advance=font_info->max_bounds.width;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=(-2.0);
  metrics->underline_thickness=1.0;
  if (!render)
    return(True);
  if (draw_info->fill.opacity == TransparentOpacity)
    return(True);
  /*
    Render fill color.
  */
  width=annotate_info.width;
  height=annotate_info.height;
  if ((draw_info->affine.rx != 0.0) || (draw_info->affine.ry != 0.0))
    {
      if (((draw_info->affine.sx-draw_info->affine.sy) == 0.0) &&
          ((draw_info->affine.rx+draw_info->affine.ry) == 0.0))
        annotate_info.degrees=(180.0/MagickPI)*
          atan2(draw_info->affine.rx,draw_info->affine.sx);
    }
  FormatString(annotate_info.geometry,"%ux%u+%d+%d",width,height,
    (int) ceil(offset->x-0.5),
    (int) ceil(offset->y-metrics->ascent-metrics->descent-0.5));
  pixel.pen_color.red=XUpscale(draw_info->fill.red);
  pixel.pen_color.green=XUpscale(draw_info->fill.green);
  pixel.pen_color.blue=XUpscale(draw_info->fill.blue);
  status=XAnnotateImage(display,&pixel,&annotate_info,image);
  if (status == 0)
    ThrowBinaryException(ResourceLimitWarning,"Unable to annotate image",
      "Memory allocation failed");
  return(True);
}
#else
static unsigned int RenderX11(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,const unsigned int render,TypeMetric *metrics)
{
  ThrowBinaryException(MissingDelegateWarning,
    "X11 library is not available",draw_info->font);
}
#endif
