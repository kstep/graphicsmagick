/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     L       AAA   BBBB   EEEEE  L                           %
%                     L      A   A  B   B  E      L                           %
%                     L      AAAAA  BBBB   EEE    L                           %
%                     L      A   A  B   B  E      L                           %
%                     LLLLL  A   A  BBBB   EEEEE  LLLLL                       %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
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
#if FREETYPE_MAJOR > 1
#include "freetype/ftglyph.h"
#endif
#endif

/*
  Font declaration.
*/
ModuleExport const char
  *DefaultXFont = "-adobe-helvetica-medium-r-*-*-14-*-*-*-*-*-iso8859-*";

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d L A B E L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadLABELImage reads a LABEL image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadLABELImage method is:
%
%      Image *ReadLABELImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadLABELImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
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
#if !defined(FREETYPE_MAJOR)
static void GetFontInfo(TT_Face face,TT_Face_Properties *face_properties,
  Image *image)
{
  char
    *label,
    *name;

  static const unsigned short
    ids[] = { 4, 5, 0, 7 };

  register char
    *p;

  register int
    i,
    j;

  unsigned short
    encoding,
    id,
    language,
    length,
    platform;

  /*
    Note font info as image comment.
  */
  if (face_properties->num_Names == 0)
    return;
  label=(char *) AcquireMemory(face_properties->num_Names*MaxTextExtent);
  if (label == (char *) NULL)
    return;
  *label='\0';
  for (i=0; i < (int) (sizeof(ids)/sizeof(unsigned short)); i++)
  {
    TT_Get_Name_ID(face,ids[i],&platform,&encoding,&language,&id);
    if (((platform != 0) || (language != 0)) &&
        ((platform != 3) || (encoding > 1) || ((language & 0x3FF) != 0x009)))
      continue;
    TT_Get_Name_String(face,ids[i],&name,&length);
    p=label+strlen(label);
    for (j=1; j < (int) Min(length,MaxTextExtent); j+=2)
      *p++=name[j];
    *p='\0';
    break;
  }
  ReacquireMemory((void **) &label,strlen(label)+1);
  (void) SetImageAttribute(image,"Label",label);
  LiberateMemory((void **) &label);
}

static void RenderGlyph(TT_Raster_Map *canvas,TT_Raster_Map *character,
  TT_Glyph glyph,int x_offset,int y_offset,TT_Glyph_Metrics *glyph_metrics)
{
  int
    y;

  register int
    i,
    x;

  register unsigned char
    *p,
    *q;

  SegmentInfo
    bounds;

  /*
    Render Glyph.
  */
  q=(unsigned char *) character->bitmap;
  for (i=0; i < character->size; i++)
    *q++=0;
  TT_Get_Glyph_Pixmap(glyph,character,-(glyph_metrics->bbox.xMin & -64),
    -(glyph_metrics->bbox.yMin & -64));
  /*
    Composite character on canvas.
  */
  x_offset+=(glyph_metrics->bbox.xMin & -64)/64;
  y_offset=(-(glyph_metrics->bbox.yMin & -64)/64)-y_offset;
  bounds.x1=x_offset < 0 ? -x_offset : 0;
  bounds.y1=y_offset < 0 ? -y_offset : 0;
  bounds.x2=canvas->cols-x_offset;
  if (bounds.x2 > character->cols)
    bounds.x2=character->cols;
  bounds.y2=canvas->rows-y_offset;
  if (bounds.y2 > character->rows)
    bounds.y2=character->rows;
  if (bounds.x1 >= bounds.x2)
    return;
  for (y=(int) bounds.y1; y < (int) bounds.y2; y++)
  {
    p=((unsigned char *) character->bitmap)+y*character->cols+(int) bounds.x1;
    q=((unsigned char *) canvas->bitmap)+(y+y_offset)*canvas->cols+
      (int) bounds.x1+x_offset;
    for (x=(int) bounds.x1; x < bounds.x2; x++)
      *q++|=(*p++);
  }
}
#endif
#endif

static Image *RenderFreetype(const ImageInfo *image_info,const char *text,
  ExceptionInfo *exception)
{
#if defined(HasTTF)
#if FREETYPE_MAJOR > 1

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
    filename[MaxTextExtent],
    label[MaxTextExtent];

  FT_BBox
    bounds;

  FT_Bitmap
    *glyph;

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
    bitmap_origin,
    kern,
    origin;

  Image
    *image;

  int
    length,
    y;

  register unsigned char
    *p;

  register int
    i,
    x;

  register PixelPacket
    *q;

  TGlyph
    *glyphs;

  unsigned short
    *unicode;

  /*
    Initialize Freetype library.
  */
  image=AllocateImage(image_info);
  status=FT_Init_FreeType(&library);
  if (!status)
    {
      register char
        *p,
        *q;

      /*
        Search for Freetype font filename.
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
          (void) strcat(filename,image_info->font+1);
          status=FT_New_Face(library,filename,0,&face);
          if (!status || (q == (char *) NULL) || (*q == '\0'))
            break;
          p=q+1;
        }
#if defined(TT_FONT_PATH)
      if (status)
        {
          /*
            Configured Freetype font path.
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
            (void) strcat(filename,image_info->font+1);
            status=FT_New_Face(library,filename,0,&face);
            if (!status || (q == (char *) NULL) || (*q == '\0'))
              break;
            p=q+1;
          }
        }
#endif
      if (status)
        status=FT_New_Face(library,image_info->font+1,0,&face);
    }
  if (status)
    {
      FT_Done_FreeType(library);
      ThrowReaderException(DelegateWarning,"Unable to read font",image);
    }
  (void) FT_Set_Char_Size(face,
    64.0*image_info->pointsize,64.0*image_info->pointsize,
    image->x_resolution == 0.0 ? 72.0 : image->x_resolution,
    image->y_resolution == 0.0 ? 72.0 : image->y_resolution);
  /*
    Convert to Unicode.
  */
  unicode=ConvertTextToUnicode(text,&length);
  glyphs=(TGlyph *) AcquireMemory(length*sizeof(TGlyph));
  if ((unicode == (unsigned short *) NULL) || (glyphs == (TGlyph *) NULL))
    {
      FT_Done_FreeType(library);
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    }
  /*
    Compute bounding box.
  */
  origin.x=0;
  origin.y=0;
  image->bounds.x1=32000;
  image->bounds.x2=(-32000);
  image->bounds.y1=32000;
  image->bounds.y2=(-32000);
  affine.xx=65536.0*image_info->affine[0];
  affine.yx=(-65536.0*image_info->affine[1]);
  affine.xy=(-65536.0*image_info->affine[2]);
  affine.yy=65536.0*image_info->affine[3];
  for (i=0; i < length; i++)
  {
    glyphs[i].id=FT_Get_Char_Index(face,unicode[i]);
    if (i > 0)
      {
        FT_Get_Kerning(face,glyphs[i-1].id,glyphs[i].id,ft_kerning_default,
          &kern);
        origin.x+=kern.x;
      }
    origin.y=0;
    status=FT_Load_Glyph(face,glyphs[i].id,FT_LOAD_DEFAULT);
    status|=FT_Get_Glyph(face->glyph,&glyphs[i].image);
    if (status)
      continue;
    glyphs[i].origin=origin;
    bitmap_origin=origin;
    FT_Vector_Transform(&bitmap_origin,&affine);
    (void) FT_Glyph_Transform(glyphs[i].image,&affine,&bitmap_origin);
    FT_Glyph_Get_CBox(glyphs[i].image,ft_glyph_bbox_pixels,&bounds);
    status=FT_Glyph_To_Bitmap(&glyphs[i].image,
      image_info->antialias ? ft_render_mode_normal : ft_render_mode_mono,
      False,False);
    if (status)
      continue;
    if (bounds.xMin < image->bounds.x1)
      image->bounds.x1=bounds.xMin;
    if (bounds.xMax > image->bounds.x2)
      image->bounds.x2=bounds.xMax;
    if (bounds.yMin < image->bounds.y1)
      image->bounds.y1=bounds.yMin;
    if (bounds.yMax > image->bounds.y2)
      image->bounds.y2=bounds.yMax;
    origin.x+=face->glyph->advance.x;
  }
  image->columns=image->bounds.x2-image->bounds.x1+1.0;
  image->rows=image->bounds.y2-image->bounds.y1+1.0;
  SetImage(image,TransparentOpacity);
  if (face->family_name != (char *) NULL)
    {
      (void) strcpy(label,face->family_name);
      if (face->family_name != (char *) NULL)
        {
          (void) strcat(label," ");
          (void) strcat(label,face->style_name);
        }
      (void) SetImageAttribute(image,"Label",label);
    }
  /*
    Render label.
  */
  for (i=0; i < length; i++)
  {
    if (glyphs[i].image == (FT_Glyph) NULL)
      continue;
    bitmap=(FT_BitmapGlyph) glyphs[i].image;
    glyph=(&bitmap->bitmap);
    if ((glyph->width == 0) || (glyph->rows == 0))
      continue;
    x=bitmap->left-image->bounds.x1+0.5;
    y=image->rows-bitmap->top+image->bounds.y1+0.5;
    q=GetImagePixels(image,x,y,glyph->width,glyph->rows);
    if (q == (PixelPacket *) NULL)
      continue;
    p=glyph->buffer;
    for (y=0; y < glyph->rows; y++)
    {
      for (x=0; x < glyph->width; x++)
      {
        if (*p != 0)
          {
            *q=image_info->fill;
            if (image_info->stroke.opacity != TransparentOpacity)
              if (*p < 255)
                *q=image_info->stroke;
            q->opacity=UpScale(DownScale(MaxRGB)-(*p));
          }
        p++;
        q++;
      }
    }
    if (!SyncImagePixels(image))
      continue;
  }
  /*
    Free resources.
  */
  for (i=0; i < length; i++)
    FT_Done_Glyph(glyphs[i].image);
  LiberateMemory((void **) &glyphs);
  LiberateMemory((void **) &unicode);
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return(image);
#else
#define MaxGlyphs  65535

  char
    filename[MaxTextExtent],
    *path,
    *path_end;

  double
    pointsize;

  Image
    *image;

  int
    character_map,
    length,
    number_glyphs,
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  TT_CharMap
    char_map;

  TT_Engine
    engine;

  TT_Error
    status;

  TT_Face
    face;

  TT_Face_Properties
    face_properties;

  TT_Glyph
    *glyphs;

  TT_Glyph_Metrics
    glyph_metrics;

  TT_Instance
    instance;

  TT_Instance_Metrics
    instance_metrics;

  TT_Raster_Map
    canvas,
    character;

  TT_UShort
    code;

  unsigned short
    encoding,
    platform,
    *unicode;

  /*
    Initialize font engine.
  */
  image=AllocateImage(image_info);
  status=TT_Init_FreeType(&engine);
  if (status)
    ThrowReaderException(DelegateWarning,"Cannot initialize TTF engine",image);
  /*
    Search for Truetype font filename.
  */
  status=True;
  path=getenv("TT_FONT_PATH");
  if (path != (char *) NULL)
    {
      /*
        Environment variable TT_FONT_PATH.
      */
      for ( ; ; )
      {
        path_end=strchr(path,DirectoryListSeparator);
        if (path_end == (char *) NULL)
          (void) strcpy(filename,path);
        else
          {
            i=(int) (path_end-path);
            (void) strncpy(filename,path,i);
            filename[i]='\0';
          }
        i=strlen(filename);
        if ((i > 0) && (!IsBasenameSeparator(filename[i-1])))
          (void) strcat(filename,DirectorySeparator);
        (void) strcat(filename,image_info->font+1);
        status=TT_Open_Face(engine,filename,&face);
        if (!status || (path_end == (char *) NULL) || (*path_end == '\0'))
          break;
        path=path_end+1;
      }
   }
#if defined(TT_FONT_PATH)
  if (status)
    {
      /*
        Configured Truetype font path.
      */
      path=TT_FONT_PATH;
      for ( ; ; )
      {
        path_end=strchr(path,DirectoryListSeparator);
        if (path_end == (char *) NULL)
          (void) strcpy(filename,path);
        else
          {
            i=(int) (path_end-path);
            (void) strncpy(filename,path,i);
            filename[i]='\0';
          }
        i=strlen(filename);
        if ((i > 0) && (!IsBasenameSeparator(filename[i-1])))
          (void) strcat(filename,DirectorySeparator);
        (void) strcat(filename,image_info->font+1);
        status=TT_Open_Face(engine,filename,&face);
        if (!status || (path_end == (char *) NULL) || (*path_end == '\0'))
          break;
        path=path_end+1;
      }
    }
#endif
  if (status)
    status=TT_Open_Face(engine,image_info->font+1,&face);
  if (status)
    ThrowReaderException(DelegateWarning,"Unable to open TTF font",image);
  TT_Get_Face_Properties(face,&face_properties);
  if (strcmp(text,Alphabet) == 0)
    GetFontInfo(face,&face_properties,image);
  status=TT_New_Instance(face,&instance);
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      image->x_resolution=72.0;
      image->y_resolution=72.0;
    }
  status|=TT_Set_Instance_Resolutions(instance,(unsigned short)
    image->x_resolution,(unsigned short) image->y_resolution);
  pointsize=image_info->pointsize;
  if (image_info->affine[1] == 0.0)
    pointsize*=image_info->affine[0];
  status|=TT_Set_Instance_CharSize(instance,(int) (64.0*pointsize));
  if (status)
    ThrowReaderException(DelegateWarning,"Cannot initialize TTF instance",
      image);
  for (code=0; (int) code < (int) face_properties.num_CharMaps; code++)
  {
    TT_Get_CharMap_ID(face,code,&platform,&encoding);
    if (((platform == 3) && (encoding == 1)) ||
        ((platform == 0) && (encoding == 0)))
      {
        TT_Get_CharMap(face,code,&char_map);
        break;
      }
  }
  number_glyphs=0;
  character_map=True;
  if (code == face_properties.num_CharMaps)
    {
      TT_Get_Face_Properties(face,&face_properties);
      number_glyphs=face_properties.num_Glyphs;
      character_map=False;
    }
  glyphs=(TT_Glyph *) AcquireMemory(MaxGlyphs*sizeof(TT_Glyph));
  if (glyphs == (TT_Glyph *) NULL)
    ThrowReaderException(DelegateWarning,"Memory allocation failed",image);
  for (i=0; i < MaxGlyphs; i++)
    glyphs[i].z=(TT_Glyph *) NULL;
  unicode=ConvertTextToUnicode(text,&length);
  if (unicode == (unsigned short *) NULL)
    ThrowReaderException(DelegateWarning,"Memory allocation failed",image);
  for (i=0; i < length; i++)
  {
    if (glyphs[unicode[i]].z != (TT_Glyph *) NULL)
      continue;
    if (character_map)
      code=TT_Char_Index(char_map,unicode[i]);
    else
      {
        code=((int) unicode[i]-' '+1) < 0 ? 0 : ((int) unicode[i]-' '+1);
        if ((int) code >= number_glyphs)
          code=0;
      }
    status=TT_New_Glyph(face,&glyphs[unicode[i]]);
    status|=TT_Load_Glyph(instance,glyphs[unicode[i]],code,
      TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH);
    if (status)
      ThrowReaderException(DelegateWarning,"Cannot initialize TTF glyph",image);
  }
  TT_Get_Face_Properties(face,&face_properties);
  TT_Get_Instance_Metrics(instance,&instance_metrics);
  x=0;
  for (i=0; i < length; i++)
  {
    if (glyphs[unicode[i]].z == (TT_Glyph *) NULL)
      continue;
    TT_Get_Glyph_Metrics(glyphs[unicode[i]],&glyph_metrics);
    x+=glyph_metrics.advance;
  }
  canvas.width=(x/64+3) & -4;
  canvas.rows=instance_metrics.y_ppem*(face_properties.horizontal->Ascender-
    face_properties.horizontal->Descender)/
    face_properties.header->Units_Per_EM;
  canvas.flow=TT_Flow_Down;
  canvas.cols=canvas.width;
  canvas.size=canvas.rows*canvas.width;
  canvas.bitmap=(void *) AcquireMemory(canvas.size);
  if (!canvas.bitmap)
    ThrowReaderException(DelegateWarning,"Memory allocation failed",image);
  p=(unsigned char *) canvas.bitmap;
  for (i=0; i < canvas.size; i++)
    *p++=0;
  character.rows=canvas.rows;
  character.width=(instance_metrics.x_ppem+32+3) & -4;
  character.flow=TT_Flow_Down;
  character.cols=character.width;
  character.size=character.rows*character.width;
  character.bitmap=(void *) AcquireMemory(character.size);
  if (!character.bitmap)
    ThrowReaderException(DelegateWarning,"Memory allocation failed",image);
  x=0;
  y=(-instance_metrics.y_ppem*face_properties.horizontal->Descender)/
    face_properties.header->Units_Per_EM+1;
  for (i=0; i < length; i++)
  {
    if (glyphs[unicode[i]].z == (TT_Glyph *) NULL)
      continue;
    TT_Get_Glyph_Metrics(glyphs[unicode[i]],&glyph_metrics);
    RenderGlyph(&canvas,&character,glyphs[unicode[i]],x,y,&glyph_metrics);
    x+=glyph_metrics.advance/64;
  }
  /*
    Render label with a TrueType font.
  */
  image->columns=canvas.width;
  image->rows=canvas.rows;
  SetImage(image,TransparentOpacity);
  p=(unsigned char *) canvas.bitmap;
  for (y=0; y < (int) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      *q=image_info->fill;
      if ((image_info->stroke.opacity != TransparentOpacity) && (*p < 4))
        *q=image_info->stroke;
      if (image_info->antialias)
        q->opacity=MaxRGB-(MaxRGB*Min(*p,4))/4;
      else
        q->opacity=(*p) > 0 ? OpaqueOpacity : TransparentOpacity;
      if (q->opacity == TransparentOpacity)
        {
          q->red=(~q->red);
          q->green=(~q->green);
          q->blue=(~q->blue);
        }
      p++;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if ((image->columns % 2) != 0)
      p++;
  }
  if ((image_info->affine[1] == 0.0) && (image_info->affine[2] == 0.0))
    {
      if ((image_info->affine[0] != 1.0) || (image_info->affine[0] != 1.0))
        {
          Image
            *scale_image;

          unsigned int
            height,
            width;

          width=image_info->affine[0]*image->columns;
          height=image_info->affine[3]*image->rows;
          scale_image=ZoomImage(image,width,height,exception);
          if (scale_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=scale_image;
            }
        }
    }
  else
    {
      if (((image_info->affine[0]-image_info->affine[3]) == 0.0) &&
          ((image_info->affine[1]+image_info->affine[2]) == 0.0))
        {
          double
            theta;

          Image
            *rotate_image;

          theta=(180.0/M_PI)*atan2(image_info->affine[1],image_info->affine[0]);
          rotate_image=RotateImage(image,theta,exception);
          if (rotate_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=rotate_image;
            }
        }
    }
  image->bounds.x2=Max(image->columns,image->rows);
  image->bounds.y2=image_info->affine[0]*image_info->pointsize;
  image->bounds.y2-=image->bounds.y2/3.0;
  image->bounds.x1=0.0;
  image->bounds.y1=(double) image->rows/-4.0;
  /*
    Free TrueType resources.
  */
  LiberateMemory((void **) &canvas.bitmap);
  LiberateMemory((void **) &character.bitmap);
  for (i=0; i < MaxGlyphs; i++)
    TT_Done_Glyph(glyphs[i]);
  LiberateMemory((void **) &glyphs);
  LiberateMemory((void **) &unicode);
  TT_Done_Instance(instance);
  TT_Close_Face(face);
  TT_Done_FreeType(engine);
  return(image);
#endif
#else
  Image
    *image;

  image=AllocateImage(image_info);
  ThrowReaderException(MissingDelegateWarning,
    "FreeType library is not available",image);
#endif
}

static Image *RenderPostscript(const ImageInfo *image_info,const char *text,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    *font,
    geometry[MaxTextExtent];

  FILE
    *file;

  int
    y;

  Image
    *image;

  ImageInfo
    *clone_info;

  PointInfo
    extent,
    point;

  register int
    x;

  register PixelPacket
    *q;

  unsigned int
    identity;

  /*
    Render label with a Postscript font.
  */
  image=AllocateImage(image_info);
  TemporaryFilename(filename);
  file=fopen(filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  (void) fprintf(file,"%%!PS-Adobe-3.0\n");
  (void) fprintf(file,"/ReencodeFont\n");
  (void) fprintf(file,"{\n");
  (void) fprintf(file,"  findfont dup length\n");
  (void) fprintf(file,
    "  dict begin { 1 index /FID ne {def} {pop pop} ifelse } forall\n");
  (void) fprintf(file,
    "  /Encoding ISOLatin1Encoding def currentdict end definefont pop\n");
  (void) fprintf(file,"} bind def\n");
  font=image_info->font;
  if (font == (char *) NULL)
    font="Times-Roman";
  /*
    Sample to compute bounding box.
  */
  identity=(image_info->affine[0] == image_info->affine[3]) &&
    (image_info->affine[1] == 0.0) && (image_info->affine[2] == 0.0);
  extent.x=0;
  extent.y=0;
  for (x=0; x <= (Extent(text)+2); x++)
  {
    point.x=fabs(image_info->affine[0]*x*image_info->pointsize+
      image_info->affine[2]*2.0*image_info->pointsize);
    point.y=fabs(image_info->affine[1]*x*image_info->pointsize+
      image_info->affine[3]*2.0*image_info->pointsize);
    if (point.x > extent.x)
      extent.x=point.x;
    if (point.y > extent.y)
      extent.y=point.y;
  }
  (void) fprintf(file,"%g %g moveto\n",identity ? 0.0 : extent.x/2.0,
    extent.y/2.0);
  (void) fprintf(file,"%g %g scale\n",image_info->pointsize,
    image_info->pointsize);
  (void) fprintf(file,
    "/%.1024s-ISO dup /%.1024s ReencodeFont findfont setfont\n",font,font);
  (void) fprintf(file,"[%g %g %g %g 0 0] concat\n",image_info->affine[0],
    -image_info->affine[1],-image_info->affine[2],
    image_info->affine[3]);
  if (!identity)
    (void) fprintf(file,"(%.1024s) stringwidth pop -0.5 mul -0.5 rmoveto\n",
      EscapeParenthesis(text));
  (void) fprintf(file,"(%.1024s) show\n",EscapeParenthesis(text));
  (void) fprintf(file,"showpage\n");
  (void) fclose(file);
  clone_info=CloneImageInfo(image_info);
  FormatString(geometry,"%dx%d+0+0!",(int) ceil(extent.x),(int) ceil(extent.y));
  (void) FormatString(clone_info->filename,"ps:%.1024s",filename);
  (void) CloneString(&clone_info->page,geometry);
  DestroyImage(image);
  image=ReadImage(clone_info,exception);
  (void) remove(filename);
  DestroyImageInfo(clone_info);
  if (image == (Image *) NULL)
    return(False);
  if (!identity)
    TransformImage(&image,"0x0",(char *) NULL);
  else
    {
      char
        geometry[MaxTextExtent];

      PixelPacket
        target;

      RectangleInfo
        crop_info;

      register PixelPacket
        *p;

      crop_info.width=0;
      crop_info.height=ceil(extent.y/2.0);
      crop_info.x=0;
      crop_info.y=floor(extent.y/8.0);
      if (image == (Image *) NULL)
        return(image);
      target=GetOnePixel(image,0,0);
      for (y=0; y < (int) image->rows; y++)
      {
        p=GetImagePixels(image,0,y,image->columns,1);
        if (p == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          if (!ColorMatch(*p,target,0))
            if (x > (int) crop_info.width)
              crop_info.width=x;
          p++;
        }
      }
      (void) sprintf(geometry,"%ux%u%+d%+d",crop_info.width+1,crop_info.height,
        crop_info.x,crop_info.y);
      TransformImage(&image,geometry,(char *) NULL);
    }
  image->matte=True;
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      q->opacity=Intensity(*q);
      q->red=image_info->fill.red;
      q->green=image_info->fill.green;
      q->blue=image_info->fill.blue;
      if (q->opacity == TransparentOpacity)
        {
          q->red=(~q->red);
          q->green=(~q->green);
          q->blue=(~q->blue);
        }
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  image->bounds.x2=Max(image->columns,image->rows);
  image->bounds.y2=image_info->affine[0]*image_info->pointsize;
  image->bounds.y2-=image->bounds.y2/3.0;
  image->bounds.x1=0.0;
  image->bounds.y1=(double) image->rows/-4.0;
  return(image);
}

static Image *RenderX11(const ImageInfo *image_info,const char *text,
  ExceptionInfo *exception)
{
#if defined(HasX11)
  Image
    *image;

  int
    status,
    y;

  register int
    x;

  register PixelPacket
    *q;

  static Display
    *display = (Display *) NULL;

  static ImageInfo
    cache_info;

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

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (display == (Display *) NULL)
    {
      /*
        Open X server connection.
      */
      display=XOpenDisplay(image_info->server_name);
      if (display != (Display *) NULL)
        {
          char
            *client_name;

          /*
            Get user defaults from X resource database.
          */
          XSetErrorHandler(XError);
          client_name=SetClientName((char *) NULL);
          resource_database=XGetResourceDatabase(display,client_name);
          XGetResourceInfo(resource_database,client_name,&resource_info);
          resource_info.close_server=False;
          resource_info.colormap=PrivateColormap;
          resource_info.font=AllocateString(image_info->font);
          resource_info.background_color=AllocateString("black");
          resource_info.foreground_color=AllocateString("white");
          map_info=XAllocStandardColormap();
          if (map_info == (XStandardColormap *) NULL)
            ThrowReaderException(ResourceLimitWarning,
              "Memory allocation failed",image);
          /*
            Initialize visual info.
          */
          visual_info=XBestVisualInfo(display,map_info,&resource_info);
          if (visual_info == (XVisualInfo *) NULL)
            ThrowReaderException(XServerWarning,"Unable to get visual",image);
          map_info->colormap=(Colormap) NULL;
          pixel.pixels=(unsigned long *) NULL;
          pixel.gamma_map=(XColor *) NULL;
          /*
            Initialize Standard Colormap info.
          */
          XGetMapInfo(visual_info,XDefaultColormap(display,
            visual_info->screen),map_info);
          XGetPixelPacket(display,visual_info,map_info,&resource_info,
            (Image *) NULL,&pixel);
          pixel.annotate_context=
            XDefaultGC(display,visual_info->screen);
          /*
            Initialize font info.
          */
          font_info=XBestFont(display,&resource_info,False);
          if (font_info == (XFontStruct *) NULL)
            ThrowReaderException(XServerWarning,"Unable to load font",image);
          if ((map_info == (XStandardColormap *) NULL) ||
              (visual_info == (XVisualInfo *) NULL) ||
              (font_info == (XFontStruct *) NULL))
            {
              XFreeResources(display,visual_info,map_info,&pixel,
                font_info,&resource_info,(XWindowInfo *) NULL);
              display=(Display *) NULL;
            }
          cache_info=(*image_info);
        }
    }
  if (display == (Display *) NULL)
    ThrowReaderException(XServerWarning,"Unable to open X server",image);
  /*
    Initialize annotate info.
  */
  XGetAnnotateInfo(&annotate_info);
  annotate_info.stencil=OpaqueStencil;
  if (cache_info.font != image_info->font)
    {
      /*
        Font name has changed.
      */
      XFreeFont(display,font_info);
      (void) CloneString(&resource_info.font,image_info->font);
      font_info=XBestFont(display,&resource_info,False);
      if (font_info == (XFontStruct *) NULL)
        ThrowReaderException(ResourceLimitWarning,"Unable to load font",image);
    }
  annotate_info.font_info=font_info;
  annotate_info.text=(char *) text;
  annotate_info.width=XTextWidth(font_info,text,Extent(text));
  annotate_info.height=font_info->ascent+font_info->descent;
  FormatString(annotate_info.geometry,"%ux%u+0+0",annotate_info.width,
    annotate_info.height);
  cache_info=(*image_info);
  /*
    Render label with a X11 server font.
  */
  image->columns=annotate_info.width;
  image->rows=annotate_info.height;
  SetImage(image,OpaqueOpacity);
  status=XAnnotateImage(display,&pixel,&annotate_info,image);
  if (status == 0)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  image->matte=True;
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      q->opacity=Intensity(*q);
      q->red=image_info->fill.red;
      q->green=image_info->fill.green;
      q->blue=image_info->fill.blue;
      if (q->opacity == TransparentOpacity)
        {
          q->red=(~q->red);
          q->green=(~q->green);
          q->blue=(~q->blue);
        }
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  if ((image_info->affine[1] == 0.0) && (image_info->affine[2] == 0.0))
    {
      if ((image_info->affine[0] != 1.0) || (image_info->affine[0] != 1.0))
        {
          Image
            *scale_image;

          unsigned int
            height,
            width;

          width=image_info->affine[0]*image->columns;
          height=image_info->affine[3]*image->rows;
          scale_image=ZoomImage(image,width,height,exception);
          if (scale_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=scale_image;
            }
        }
    }
  else
    {
      if (((image_info->affine[0]-image_info->affine[3]) == 0.0) &&
          ((image_info->affine[1]+image_info->affine[2]) == 0.0))
        {
          double
            theta;

          Image
            *rotate_image;

          theta=(180.0/M_PI)*atan2(image_info->affine[1],image_info->affine[0]);
          rotate_image=RotateImage(image,theta,exception);
          if (rotate_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=rotate_image;
            }
        }
    }
  image->bounds.x2=Max(image->columns,image->rows);
  image->bounds.y2=image_info->affine[0]*image_info->pointsize;
  image->bounds.y2-=image->bounds.y2/3.0;
  image->bounds.x1=0.0;
  image->bounds.y1=(double) image->rows/-4.0;
  return(image);
#else
  Image
    *image;

  image=AllocateImage(image_info);
  ThrowReaderException(MissingDelegateWarning,"X11 library is not available",
    image);
#endif
}

static Image *ReadLABELImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  char
    *label;

  /*
    Create image label.
  */
  if (image_info->filename[0] != '@')
    label=AllocateString(image_info->filename);
  else
    {
      FILE
        *file;

      int
        c;

      char
        *s,
        *q;

      unsigned int
        length;

      /*
        Read text from a file.
      */
      file=(FILE *) fopen(&(image_info->filename[1]),"r");
      if (file == (FILE *) NULL)
        {
          ThrowException(exception,FileOpenWarning,
            "Unable to read label data from file",&(image_info->filename[1]));
          LiberateMemory((void **) &label);
          return((Image *) NULL);
        }
      length=MaxTextExtent;
      s=(char *) AcquireMemory(length);
      q=s;
      while (s != (char *) NULL)
      {
        c=fgetc(file);
        if (c == EOF)
          break;
        if ((c == '\n') || (c == '\r') || (c == '\t'))
          continue;
        if ((q-s+1) >= (int) length)
          {
            *q='\0';
            length<<=1;
            ReacquireMemory((void **) &s,length);
            if (s == (char *) NULL)
              break;
            q=s+Extent(s);
          }
        *q++=c;
      }
      (void) fclose(file);
      if (s == (char *) NULL)
        {
          ThrowException(exception,FileOpenWarning,
            "Unable to read label data from file","Memory allocation failed");
          LiberateMemory((void **) &label);
          return((Image *) NULL);
        }
      *q='\0';
      label=s;
    }
  if (image_info->font == (char *) NULL)
    image=RenderPostscript(image_info,label,exception);
  else
    if (*image_info->font == '@')
      image=RenderFreetype(image_info,label,exception);
    else
      if (*image_info->font == '-')
        image=RenderX11(image_info,label,exception);
      else
        image=RenderPostscript(image_info,label,exception);
  LiberateMemory((void **) &label);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r L A B E L I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterLABELImage adds attributes for the LABEL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterLABELImage method is:
%
%      RegisterLABELImage(void)
%
*/
ModuleExport void RegisterLABELImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("LABEL");
  entry->decoder=ReadLABELImage;
  entry->adjoin=False;
  entry->description=AllocateString("Text image format");
  entry->module=AllocateString("LABEL");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r L A B E L I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterLABELImage removes format registrations made by the
%  LABEL module from the list of supported formats.
%
%  The format of the UnregisterLABELImage method is:
%
%      UnregisterLABELImage(void)
%
*/
ModuleExport void UnregisterLABELImage(void)
{
  UnregisterMagickInfo("LABEL");
}
