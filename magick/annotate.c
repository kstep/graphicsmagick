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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
#if defined(HAVE_FREETYPE_FREETYPE_H)
#include "freetype/freetype.h"
#else
#include "freetype.h"
#endif
#if FREETYPE_MAJOR > 1
#include "freetype/ftglyph.h"
#endif
#endif

/*
  Forward declarations.
*/
static unsigned int
  RenderFont(Image *,const AnnotateInfo *,const PointInfo *,SegmentInfo *);

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
%      unsigned int AnnotateImage(Image *image,AnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o status: Method AnnotateImage returns True if the image is annotated
%      otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o annotate_info: The address of a AnnotateInfo structure.
%
%
*/
MagickExport unsigned int AnnotateImage(Image *image,
  const AnnotateInfo *annotate_info)
{
  AnnotateInfo
    *clone_info;

  char
    *text,
    **textlist;

  double
    font_height,
    font_width;

  int
    j,
    x,
    y;

  PointInfo
    offset;

  register int
    i;

  register PixelPacket
    *q;

  SegmentInfo
    bounds;

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
  assert(annotate_info != (AnnotateInfo *) NULL);
  assert(annotate_info->signature == MagickSignature);
  if (annotate_info->text == (char *) NULL)
    return(False);
  if (*annotate_info->text == '\0')
    return(False);
  text=TranslateText((ImageInfo *) NULL,image,annotate_info->text);
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
  if (annotate_info->geometry != (char *) NULL)
    {
      unsigned int
        flags;

      /*
        User specified annotation geometry.
      */
      flags=ParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
      if ((flags & XNegative) != 0)
        x+=image->columns;
      if ((flags & WidthValue) == 0)
        width-=2*x > width ? width : 2*x;
      if ((flags & YNegative) != 0)
        y+=image->rows;
      if ((flags & HeightValue) == 0)
        height-=2*y > height ? height : 2*y;
    }
  clone_info=CloneAnnotateInfo((ImageInfo *) NULL,annotate_info);
  font_height=AffineExpansion(&annotate_info->affine)*
    annotate_info->pointsize;
  matte=image->matte;
  for (i=0; textlist[i] != (char *) NULL; i++)
  {
    if (*textlist[i] == '\0')
      continue;
    /*
      Position text relative to image.
    */
    (void) CloneString(&clone_info->text,textlist[i]);
    status=GetFontMetrics(clone_info,&bounds);
    if (status == False)
      continue;
    font_width=bounds.x2-bounds.x1;
    offset.x=x;
    offset.y=y+i*font_height;
    switch (clone_info->gravity)
    {
      case NorthWestGravity:
      {
        offset.x=x;
        offset.y=y+i*font_height;
        break;
      }
      case NorthGravity:
      {
        offset.x=x+0.5*width-0.5*font_width;
        offset.y=y+i*font_height;
        break;
      }
      case NorthEastGravity:
      {
        offset.x=width-font_width+x;
        offset.y=y+i*font_height;
        break;
      }
      case WestGravity:
      {
        offset.x=x;
        offset.y=y+0.5*height+i*font_height-0.5*font_height*number_lines+
          0.5*font_height-(font_height/-4.0)+number_lines;
        break;
      }
      case ForgetGravity:
      case StaticGravity:
      case CenterGravity:
      default:
      {
        offset.x=x+0.5*width-0.5*font_width;
        offset.y=y+0.5*height+i*font_height-0.5*font_height*number_lines+
          0.5*font_height-(font_height/-4.0)+number_lines;
        break;
      }
      case EastGravity:
      {
        offset.x=x+width-font_width;
        offset.y=y+0.5*height+i*font_height-0.5*font_height*number_lines+
          0.5*font_height-(font_height/-4.0)+number_lines;
        break;
      }
      case SouthWestGravity:
      {
        offset.x=x;
        offset.y=y+height+i*font_height+(3.0*font_height/4.0);
        break;
      }
      case SouthGravity:
      {
        offset.x=x+0.5*width-0.5*font_width;
        offset.y=y+height+i*font_height+(3.0*font_height/4.0);
        break;
      }
      case SouthEastGravity:
      {
        offset.x=x+width-font_width;
        offset.y=y+height+i*font_height+(3.0*font_height/4.0);
        break;
      }
    }
    /*
      Annotate image with text.
    */
    status=RenderFont(image,clone_info,&offset,&bounds);
    if (status == False)
      continue;
    if (clone_info->decorate == NoDecoration)
      continue;
    /*
      Decorate text.
    */
    if (clone_info->decorate == OverlineDecoration)
      q=GetImagePixels(image,(int) ceil(offset.x-0.5),(int)
        ceil(offset.y-3.0*font_height/4.0+1.5),(int) font_width,1);
    else
      if (clone_info->decorate == UnderlineDecoration)
        q=GetImagePixels(image,(int) ceil(offset.x+0.5),
          (int) ceil(offset.y+0.5),font_width,1);
      else
        q=GetImagePixels(image,(int) (offset.x-0.5),(int)
          ceil(offset.y-font_height/4.0-0.5),(int) font_width,1);
    if (q == (PixelPacket *) NULL)
      continue;
    for (j=0; j < (int) font_width; j++)
      *q++=clone_info->stroke;
    SyncImagePixels(image);
  }
  image->matte=matte;
  /*
    Free resources.
  */
  DestroyAnnotateInfo(clone_info);
  LiberateMemory((void **) &text);
  for (i=0; textlist[i] != (char *) NULL; i++)
    LiberateMemory((void **) &textlist[i]);
  LiberateMemory((void **) &textlist);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e A n n o t a t e I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneAnnotateInfo makes a duplicate of the given annotate info, or if
%  annotate info is NULL, a new one.
%
%  The format of the CloneAnnotateInfo method is:
%
%      AnnotateInfo *CloneAnnotateInfo(const ImageInfo *image_info,
%        const AnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o clone_info: Method CloneAnnotateInfo returns a duplicate of the given
%      annotate info, or if annotate info is NULL a new one.
%
%    o image_info: a structure of type info.
%
%    o annotate_info: a structure of type info.
%
%
*/
MagickExport AnnotateInfo *CloneAnnotateInfo(const ImageInfo *image_info,
  const AnnotateInfo *annotate_info)
{
  AnnotateInfo
    *clone_info;

  clone_info=(AnnotateInfo *) AcquireMemory(sizeof(AnnotateInfo));
  if (clone_info == (AnnotateInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone annotate info",
      "Memory allocation failed");
  if (annotate_info == (AnnotateInfo *) NULL)
    {
      GetAnnotateInfo(image_info,clone_info);
      return(clone_info);
    }
  *clone_info=(*annotate_info);
  if (annotate_info->geometry != (char *) NULL)
    clone_info->geometry=AllocateString(annotate_info->geometry);
  if (annotate_info->text != (char *) NULL)
    clone_info->text=AllocateString(annotate_info->text);
  if (annotate_info->font != (char *) NULL)
    clone_info->font=AllocateString(annotate_info->font);
  if (annotate_info->density != (char *) NULL)
    clone_info->density=AllocateString(annotate_info->density);
  if (annotate_info->server_name != (char *) NULL)
    clone_info->server_name=AllocateString(annotate_info->server_name);
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y A n n o t a t e I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyAnnotateInfo deallocates memory associated with an
%  AnnotateInfo structure.
%
%  The format of the DestroyAnnotateInfo method is:
%
%      void DestroyAnnotateInfo(AnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o annotate_info: Specifies a pointer to an AnnotateInfo structure.
%
%
*/
MagickExport void DestroyAnnotateInfo(AnnotateInfo *annotate_info)
{
  assert(annotate_info != (AnnotateInfo *) NULL);
  assert(annotate_info->signature == MagickSignature);
  if (annotate_info->geometry != (char *) NULL)
    LiberateMemory((void **) &annotate_info->geometry);
  if (annotate_info->text != (char *) NULL)
    LiberateMemory((void **) &annotate_info->text);
  if (annotate_info->font != (char *) NULL)
    LiberateMemory((void **) &annotate_info->font);
  if (annotate_info->density != (char *) NULL)
    LiberateMemory((void **) &annotate_info->density);
  if (annotate_info->server_name != (char *) NULL)
    LiberateMemory((void **) &annotate_info->server_name);
  LiberateMemory((void **) &annotate_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t A n n o t a t e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetAnnotateInfo initializes the AnnotateInfo structure.
%
%  The format of the GetAnnotateInfo method is:
%
%      void GetAnnotateInfo(const ImageInfo *image_info,
%        AnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o annotate_info: Specifies a pointer to a AnnotateInfo structure.
%
%
*/
MagickExport void GetAnnotateInfo(const ImageInfo *image_info,
  AnnotateInfo *annotate_info)
{
  ImageInfo
    *clone_info;

  /*
    Initialize annotate attributes;
  */
  assert(annotate_info != (AnnotateInfo *) NULL);
  clone_info=CloneImageInfo(image_info);
  memset(annotate_info,0,sizeof(AnnotateInfo));
  if (clone_info->font != (char *) NULL)
    annotate_info->font=AllocateString(clone_info->font);
  if (clone_info->density != (char *) NULL)
    annotate_info->density=AllocateString(clone_info->density);
  annotate_info->antialias=clone_info->antialias;
  annotate_info->gravity=NorthWestGravity;
  annotate_info->pointsize=clone_info->pointsize;
  annotate_info->affine=clone_info->affine;
  annotate_info->fill=clone_info->fill;
  annotate_info->stroke=clone_info->stroke;
  (void) QueryColorDatabase("none",&annotate_info->box);
  annotate_info->decorate=NoDecoration;
  if (clone_info->server_name != (char *) NULL)
    annotate_info->server_name=AllocateString(clone_info->server_name);
  annotate_info->signature=MagickSignature;
  DestroyImageInfo(clone_info);
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
%  Method GetFontMetrics returns the bounding box of the specified font and
%  text.
%
%  The format of the GetFontMetrics method is:
%
%      void GetFontMetrics(const AnnotateInfo *annotate_info,SegmentInfo bounds)
%
%  A description of each parameter follows:
%
%    o annotate_info: Specifies a pointer to a AnnotateInfo structure.
%
%    o bounds: Method GetFontMetrics returns the bounding box of text.
%
%
*/
MagickExport unsigned int GetFontMetrics(const AnnotateInfo *annotate_info,
  SegmentInfo *bounds)
{
  PointInfo
    offset;

  unsigned int
    status;

  assert(annotate_info != (AnnotateInfo *) NULL);
  assert(annotate_info->signature == MagickSignature);
  offset.x=0.0;
  offset.y=0.0;
  status=RenderFont((Image *) NULL,annotate_info,&offset,bounds);
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
%      unsigned int RenderFont(Image *image,AnnotateInfo *annotate_info,
%        const PointInfo *offset,SegmentInfo *bounds)
%
%  A description of each parameter follows:
%
%    o status: Method RenderFont returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o annotate_info: The address of a AnnotateInfo structure.
%
%    o offset: (x,y) location of text relative to image.
%
%    o bounds: bounding box of text.
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

#if defined(HasTTF)
static unsigned int RenderTruetype(Image *image,
  const AnnotateInfo *annotate_info,const PointInfo *offset,SegmentInfo *bounds)
{
  typedef struct TGlyph_
  {
    FT_UInt
      id;

    FT_Vector
      origin;

    FT_Glyph
      image;
  } TGlyph;

  char
    filename[MaxTextExtent];

  double
    alpha,
    opacity;

  FT_BBox
    bounding_box;

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
    delta,
    origin;

  int
    length,
    y;

  PixelPacket
    box_color,
    fill_color;

  PointInfo
    point,
    resolution;

  register unsigned char
    *p;

  register int
    i,
    x;

  register PixelPacket
    *q;

  TGlyph
    *glyph,
    *glyphs;

  unsigned short
    *unicode;

  /*
    Initialize Truetype library.
  */
  status=FT_Init_FreeType(&library);
  if (!status)
    {
      register char
        *p,
        *q;

      /*
        Search for Truetype font filename.
      */
      status=True;
      p=getenv("TT_FONT_PATH");
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
          (void) strcat(filename,annotate_info->font);
          status=FT_New_Face(library,filename,0,&face);
          if (!status || (q == (char *) NULL) || (*q == '\0'))
            break;
          p=q+1;
        }
#if defined(TT_FONT_PATH)
      if (status)
        {
          /*
            Configured Truetype font path.
          */
          p=TT_FONT_PATH;
          for ( ; ; )
          {
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
            (void) strcat(filename,annotate_info->font);
            status=FT_New_Face(library,filename,0,&face);
            if (!status || (q == (char *) NULL) || (*q == '\0'))
              break;
            p=q+1;
          }
        }
#endif
      if (status)
        status=FT_New_Face(library,annotate_info->font,0,&face);
    }
  if (status)
    {
      FT_Done_FreeType(library);
      ThrowBinaryException(DelegateWarning,"Unable to read font",
        annotate_info->font);
    }
  resolution.x=72.0;
  resolution.y=72.0;
  if (annotate_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(annotate_info->density,"%lfx%lf",&resolution.x,
        &resolution.y);
      if (count != 2)
        resolution.y=resolution.x;
    }
  (void) FT_Set_Char_Size(face,(long int) (64.0*annotate_info->pointsize),
    (long int) (64.0*annotate_info->pointsize),(unsigned int) resolution.x,
    (unsigned int) resolution.y);
  /*
    Convert to Unicode.
  */
  unicode=ConvertTextToUnicode(annotate_info->text,&length);
  glyphs=(TGlyph *) AcquireMemory((length+1)*sizeof(TGlyph));
  if ((unicode == (unsigned short *) NULL) || (glyphs == (TGlyph *) NULL))
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
  bounds->x1=32000;
  bounds->x2=(-32000);
  bounds->y1=32000;
  bounds->y2=(-32000);
  affine.xx=(FT_Fixed) (65536.0*annotate_info->affine.sx);
  affine.xy=(FT_Fixed) (65536.0*annotate_info->affine.rx);
  affine.yx=(FT_Fixed) (65536.0*annotate_info->affine.ry);
  affine.yy=(FT_Fixed) (65536.0*annotate_info->affine.sy);
  glyph=glyphs;
  for (i=0; i < length; i++)
  {
    glyph->id=FT_Get_Char_Index(face,unicode[i]);
    if ((glyph > glyphs) && glyph->id && FT_HAS_KERNING(face))
      {
        FT_Get_Kerning(face,(glyph-1)->id,glyph->id,ft_kerning_default,&delta);
        origin.x+=delta.x;
      }
    glyph->origin=origin;
    status=FT_Load_Glyph(face,glyph->id,FT_LOAD_DEFAULT);
    if (status)
      continue;
    status=FT_Get_Glyph(face->glyph,&glyph->image);
    if (status)
      continue;
    FT_Vector_Transform(&glyph->origin,&affine);
    FT_Glyph_Transform(glyphs[i].image,&affine,&glyph->origin);
    FT_Glyph_Get_CBox(glyph->image,ft_glyph_bbox_pixels,&bounding_box);
    if (status)
      continue;
    if (bounding_box.xMin < bounds->x1)
      bounds->x1=bounding_box.xMin;
    if (bounding_box.xMax > bounds->x2)
      bounds->x2=bounding_box.xMax;
    if (bounding_box.yMin < bounds->y1)
      bounds->y1=bounding_box.yMin;
    if (bounding_box.yMax > bounds->y2)
      bounds->y2=bounding_box.yMax;
    origin.x+=face->glyph->advance.x;
    glyph++;
  }
  glyph->id=0;
  LiberateMemory((void **) &unicode);
  if (image == (Image *) NULL)
    {
      for (glyph=glyphs; glyph->id != 0; glyph++)
        FT_Done_Glyph(glyph->image);
      LiberateMemory((void **) &glyphs);
      FT_Done_Face(face);
      FT_Done_FreeType(library);
      return(True);
    }
  /*
    Render label.
  */
  alpha=1.0/MaxRGB;
  box_color=annotate_info->box;
  fill_color=annotate_info->fill;
  for (glyph=glyphs; glyph->id != 0; glyph++)
  {
    if (glyph->image == (FT_Glyph) NULL)
      continue;
    status=FT_Glyph_To_Bitmap(&glyph->image,ft_render_mode_normal,False,False);
    bitmap=(FT_BitmapGlyph) glyph->image;
    if ((bitmap->bitmap.width == 0) || (bitmap->bitmap.rows == 0))
      continue;
    point.y=offset->y+bounds->y2-bitmap->top-3.0*
      AffineExpansion(&annotate_info->affine)*annotate_info->pointsize/4.0+1.5;
    p=bitmap->bitmap.buffer;
    for (y=0; y < bitmap->bitmap.rows; y++)
    {
      if (ceil(point.y+y-0.5) < 0)
        continue;
      if (ceil(point.y+y-0.5) >= image->rows)
        break;
      point.x=offset->x+bitmap->left-bounds->x1;
      for (x=0; x < bitmap->bitmap.width; x++)
      {
        if ((ceil(point.x+x-0.5) < 0) ||
            (ceil(point.x+x-0.5) >= image->columns))
          {
            p++;
            continue;
          }
        if (*p == 0)
          {
            p++;
            continue;
          }
        q=GetImagePixels(image,(int) ceil(point.x+x-0.5),
          (int) ceil(point.y+y-0.5),1,1);
        if (q == (PixelPacket *) NULL)
          break;
        opacity=MaxRGB-UpScale(*p);
        q->red=(Quantum) (((double) fill_color.red*(MaxRGB-opacity)+
          (double) q->red*opacity)*alpha);
        q->green=(Quantum) (((double) fill_color.green*(MaxRGB-opacity)+
          (double) q->green*opacity)*alpha);
        q->blue=(Quantum) (((double) fill_color.blue*(MaxRGB-opacity)+
          (double) q->blue*opacity)*alpha);
        q->opacity=(Quantum) (((double) opacity*(MaxRGB-opacity)+
          (double) q->opacity*opacity)*alpha);
        if (!SyncImagePixels(image))
          break;
        p++;
      }
    }
  }
  /*
    Free resources.
  */
  for (glyph=glyphs; glyph->id != 0; glyph++)
    FT_Done_Glyph(glyph->image);
  LiberateMemory((void **) &glyphs);
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return(True);
}
#else
static unsigned int RenderTruetype(Image *image,
  const AnnotateInfo *annotate_info,const PointInfo *offset,SegmentInfo *bounds)
{
  ThrowBinaryException(MissingDelegateWarning,
    "FreeType library is not available",annotate_info->font);
}
#endif


static unsigned int RenderPostscript(Image *image,
  const AnnotateInfo *annotate_info,const PointInfo *offset,SegmentInfo *bounds)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent];

  double
    font_height;

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
    point;

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
  identity=(annotate_info->affine.sx == annotate_info->affine.sy) &&
    (annotate_info->affine.rx == 0.0) && (annotate_info->affine.ry == 0.0);
  extent.x=0.0;
  extent.y=0.0;
  for (i=0; i <= (Extent(annotate_info->text)+2); i++)
  {
    point.x=fabs(annotate_info->affine.sx*i*annotate_info->pointsize+
      annotate_info->affine.ry*2.0*annotate_info->pointsize);
    point.y=fabs(annotate_info->affine.rx*i*annotate_info->pointsize+
      annotate_info->affine.sy*2.0*annotate_info->pointsize);
    if (point.x > extent.x)
      extent.x=point.x;
    if (point.y > extent.y)
      extent.y=point.y;
  }
  (void) fprintf(file,"%g %g moveto\n",identity ? 0.0 : extent.x/2.0,
    extent.y/2.0);
  (void) fprintf(file,"%g %g scale\n",annotate_info->pointsize,
    annotate_info->pointsize);
  (void) fprintf(file,
    "/%.1024s-ISO dup /%.1024s ReencodeFont findfont setfont\n",
    annotate_info->font ? annotate_info->font : "Times-Roman",
    annotate_info->font ? annotate_info->font : "Times-Roman");
  (void) fprintf(file,"[%g %g %g %g 0 0] concat\n",annotate_info->affine.sx,
    -annotate_info->affine.rx,-annotate_info->affine.ry,
    annotate_info->affine.sy);
  if (!identity)
    (void) fprintf(file,"(%.1024s) stringwidth pop -0.5 mul -0.5 rmoveto\n",
      EscapeParenthesis(annotate_info->text));
  (void) fprintf(file,"(%.1024s) show\n",
    EscapeParenthesis(annotate_info->text));
  (void) fprintf(file,"showpage\n");
  (void) fclose(file);
  FormatString(geometry,"%dx%d+0+0!",(int) ceil(extent.x-0.5),
    (int) ceil(extent.y-0.5));
  clone_info=CloneImageInfo((ImageInfo *) NULL);
  (void) FormatString(clone_info->filename,"ps:%.1024s",filename);
  (void) CloneString(&clone_info->page,geometry);
  GetExceptionInfo(&exception);
  annotate_image=ReadImage(clone_info,&exception);
  DestroyImageInfo(clone_info);
  DestroyExceptionInfo(&exception);
  (void) remove(filename);
  if (annotate_image == (Image *) NULL)
    return(False);
  if (!identity)
    TransformImage(&annotate_image,"0x0",(char *) NULL);
  else
    {
      RectangleInfo
        crop_info;

      crop_info=GetImageBoundingBox(annotate_image);
      crop_info.height=(unsigned int) ceil(
        AffineExpansion(&annotate_info->affine)*annotate_info->pointsize-0.5);
      crop_info.y=(int) ceil(extent.y/8.0-0.5);
      (void) FormatString(geometry,"%ux%u%+d%+d",crop_info.width,
        crop_info.height,crop_info.x,crop_info.y);
      TransformImage(&annotate_image,geometry,(char *) NULL);
    }
  font_height=AffineExpansion(&annotate_info->affine)*annotate_info->pointsize;
  bounds->x1=0.0;
  bounds->y1=(font_height/-4.0)+1.5;
  bounds->x2=annotate_image->columns;
  bounds->y2=(3.0*font_height/4.0)-1.5;
  if (image == (Image *) NULL)
    {
      DestroyImage(annotate_image);
      return(True);
    }
  annotate_image->matte=True;
  for (y=0; y < (int) annotate_image->rows; y++)
  {
    q=GetImagePixels(annotate_image,0,y,annotate_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) annotate_image->columns; x++)
    {
      q->opacity=(Quantum) Intensity(*q);
      q->red=annotate_info->fill.red;
      q->green=annotate_info->fill.green;
      q->blue=annotate_info->fill.blue;
      q++;
    }
    if (!SyncImagePixels(annotate_image))
      break;
  }
  CompositeImage(image,OverCompositeOp,annotate_image,
    (int) ceil(offset->x-0.5),(int) ceil(offset->y-bounds->y2-1.5));
  DestroyImage(annotate_image);
  return(True);
}

#if defined(HasX11)
static unsigned int RenderX11(Image *image,const AnnotateInfo *annotate_info,
  const PointInfo *offset,SegmentInfo *bounds)
{
  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  static AnnotateInfo
    cache_info;

  static Display
    *display = (Display *) NULL;

  static XAnnotateInfo
    xannotate_info;

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
      display=XOpenDisplay(annotate_info->server_name);
      if (display == (Display *) NULL)
        ThrowBinaryException(XServerWarning,"Unable to open X server",
          annotate_info->server_name);
      /*
        Get user defaults from X resource database.
      */
      XSetErrorHandler(XError);
      client_name=SetClientName((char *) NULL);
      resource_database=XGetResourceDatabase(display,client_name);
      XGetResourceInfo(resource_database,client_name,&resource_info);
      resource_info.close_server=False;
      resource_info.colormap=PrivateColormap;
      resource_info.font=AllocateString(annotate_info->font);
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
          annotate_info->server_name);
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
          annotate_info->font);
      if ((map_info == (XStandardColormap *) NULL) ||
          (visual_info == (XVisualInfo *) NULL) ||
          (font_info == (XFontStruct *) NULL))
        {
          XFreeResources(display,visual_info,map_info,&pixel,font_info,
            &resource_info,(XWindowInfo *) NULL);
          ThrowBinaryException(XServerWarning,"Unable to get X server font",
            annotate_info->server_name);
        }
      cache_info=(*annotate_info);
    }
  /*
    Initialize annotate info.
  */
  XGetAnnotateInfo(&xannotate_info);
  xannotate_info.stencil=OpaqueStencil;
  if (cache_info.font != annotate_info->font)
    {
      /*
        Font name has changed.
      */
      XFreeFont(display,font_info);
      (void) CloneString(&resource_info.font,annotate_info->font);
      font_info=XBestFont(display,&resource_info,False);
      if (font_info == (XFontStruct *) NULL)
        ThrowBinaryException(ResourceLimitWarning,"Unable to load font",
          annotate_info->font);
    }
  cache_info=(*annotate_info);
  xannotate_info.font_info=font_info;
  xannotate_info.text=(char *) annotate_info->text;
  xannotate_info.width=XTextWidth(font_info,annotate_info->text,
    Extent(annotate_info->text));
  xannotate_info.height=font_info->ascent+font_info->descent;
  bounds->x1=0.0;
  bounds->y1=(-font_info->descent);
  bounds->x2=xannotate_info.width;
  bounds->y2=font_info->ascent;
  if (image == (Image *) NULL)
    return(True);
  /*
    Render label with a X11 server font.
  */
  width=xannotate_info.width;
  height=xannotate_info.height;
  if ((annotate_info->affine.rx != 0.0) || (annotate_info->affine.ry != 0.0))
    {
      if (((annotate_info->affine.sx-annotate_info->affine.sy) == 0.0) &&
          ((annotate_info->affine.rx+annotate_info->affine.ry) == 0.0))
        xannotate_info.degrees=(180.0/M_PI)*
          atan2(annotate_info->affine.rx,annotate_info->affine.sx);
    }
  FormatString(xannotate_info.geometry,"%ux%u+%d+%d",width,height,
    (int) ceil(offset->x-1.5),(int) ceil(offset->y-bounds->y2+0.5));
  pixel.pen_color.red=XUpScale(annotate_info->fill.red);
  pixel.pen_color.green=XUpScale(annotate_info->fill.green);
  pixel.pen_color.blue=XUpScale(annotate_info->fill.blue);
  status=XAnnotateImage(display,&pixel,&xannotate_info,image);
  if (status == 0)
    ThrowBinaryException(ResourceLimitWarning,"Unable to annotate image",
      "Memory allocation failed");
  return(True);
}
#else
static unsigned int RenderX11(Image *image,const AnnotateInfo *annotate_info,
  const PointInfo *offset,SegmentInfo *bounds)
{
  ThrowBinaryException(MissingDelegateWarning,
    "X11 library is not available",annotate_info->font);
}
#endif

static unsigned int RenderFont(Image *image,const AnnotateInfo *annotate_info,
  const PointInfo *offset,SegmentInfo *bounds)
{
  AnnotateInfo
    *clone_info;

  ImageInfo
    *image_info;

  unsigned int
    status;

  if (annotate_info->font == (char *) NULL)
    return(RenderPostscript(image,annotate_info,offset,bounds));
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strcpy(image_info->filename,annotate_info->font);
  (void) strcpy(image_info->magick,"PS");
  if (*image_info->filename == '@')
    (void) strcpy(image_info->magick,"TTF");
  if (*image_info->filename == '-')
    (void) strcpy(image_info->magick,"X");
  (void) SetImageInfo(image_info,False);
  clone_info=CloneAnnotateInfo(image_info,annotate_info);
  if (*image_info->filename != '@')
    CloneString(&clone_info->font,image_info->filename);
  else
    CloneString(&clone_info->font,image_info->filename+1);
  if (LocaleCompare(image_info->magick,"ps") == 0)
    status=RenderPostscript(image,clone_info,offset,bounds);
  else
    if (LocaleCompare(image_info->magick,"ttf") == 0)
      status=RenderTruetype(image,clone_info,offset,bounds);
    else
      if (LocaleCompare(image_info->magick,"x") == 0)
        status=RenderX11(image,clone_info,offset,bounds);
      else
        status=RenderPostscript(image,clone_info,offset,bounds);
  DestroyAnnotateInfo(clone_info);
  DestroyImageInfo(image_info);
  return(status);
}
