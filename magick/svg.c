/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS  V   V   GGGG                              %
%                            SS     V   V  G                                  %
%                             SSS   V   V  G GG                               %
%                               SS   V V   G   G                              %
%                            SSSSS    V     GGG                               %
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

/*
  Forward declarations.
*/
static unsigned int
  WriteSVGImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S V G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSVG returns True if the image format type, identified by the
%  magick string, is SVG.
%
%  The format of the IsSVG method is:
%
%      unsigned int IsSVG(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSVG returns True if the image format type is SVG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSVG(const unsigned char *magick,
  const unsigned int length)
{
  if (length < 5)
    return(False);
  if (strncmp((char *) magick,"<?XML",5) == 0)
    return(True);
  if (strncmp((char *) magick,"<?xml",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S V G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSVGImage reads a Scalable Vector Gaphics file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSVGImage method is:
%
%      Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSVGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    **tokens,
    *type,
    *values;

  DrawInfo
    *draw_info;

  Image
    *canvas,
    *clone_image,
    *image;

  ImageInfo
    *clone_info;

  int
    c,
    length,
    number_tokens;

  register int
    i;

  register unsigned char
    *p;

  unsigned int
    fill,
    quote,
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  clone_info=CloneImageInfo(image_info);
  (void) strcpy(clone_info->filename,"xc:white");
  canvas=ReadImage(clone_info,exception);
  if (canvas == (Image *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  canvas->colormap[0]=image->background_color;
  /*
    Interpret SVG language.
  */
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  c=ReadByte(image);
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  type="line";
  fill=False;
  /*
    Decode image header;  header terminates one character beyond a ':'.
  */
  length=MaxTextExtent;
  values=(char *) AllocateMemory(length);
  if (values == (char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Unable to allocate memory",
      image);
  while (c != EOF)
  {
    register char
      *p;

    if (!isalnum(c))
      c=ReadByte(image);
    else
      {
        /*
          Find a keyword.
        */
        p=keyword;
        do
        {
          if ((p-keyword) < (MaxTextExtent-1))
            *p++=c;
          c=ReadByte(image);
        } while (isalnum(c) || (c == '-'));
        *p='\0';
        while (isspace(c))
          c=ReadByte(image);
        *values='\0';
        if (c == '=')
          {
            /*
              Find a value.
            */
            c=ReadByte(image);
            while (isspace(c))
              c=ReadByte(image);
            quote=c == '"';
            if (quote)
              c=ReadByte(image);
            p=values;
            while (c != EOF)
            {
              if ((p-values+1) >= (int) length)
                {
                  *p='\0';
                  length<<=1;
                  values=(char *) ReallocateMemory(values,length);
                  if (values == (char *) NULL)
                    break;
                  p=values+Extent(values);
                }
              if (values == (char *) NULL)
                ThrowReaderException(ResourceLimitWarning,
                  "Memory allocation failed",image);
              *p++=c;
              if (quote)
                {
                  if (c == '"')
                    {
                      p--;
                      break;
                    }
                }
              else
                if (isspace(c))
                  break;
              c=ReadByte(image);
            }
            *p='\0';
          }
        /*
          Interpret the SVG keywords.
        */
        if (Latin1Compare(keyword,"height") == 0)
          {
            unsigned int
              height;

            height=canvas->rows;
            (void) sscanf(values,"%u",&height);
            clone_image=SampleImage(canvas,canvas->columns,height,exception);
            if (clone_image == (Image *) NULL)
              return((Image *) NULL);
            DestroyImage(canvas);
            canvas=clone_image;
          }
        if (Latin1Compare(keyword,"polyline") == 0)
          {
            type=fill ? "fillPolygon" : "polygon";
          }
        if (Latin1Compare(keyword,"style") == 0)
          {
            type="line";
            fill=False;
            tokens=StringToArgv(values,&number_tokens);
            for (i=0; i < number_tokens; i++)
            {
              if (Latin1Compare(tokens[i],"fill"))
                fill=True;
              if (*tokens[i] == '#')
                {
                  (void) CloneString(&draw_info->pen,tokens[i]);
                  (void) FormatString(clone_info->filename,"xc:%.1024s",
                    draw_info->pen);
                  if (draw_info->tile != (Image *) NULL)
                    DestroyImage(draw_info->tile);
                  draw_info->tile=ReadImage(clone_info,exception);
                }
              FreeMemory((void *) &tokens[i]);
            }
            FreeMemory((void *) &tokens);
          }
        if (Latin1Compare(keyword,"width") == 0)
          {
            unsigned int
              width;

            width=canvas->columns;
            (void) sscanf(values,"%u",&width);
            clone_image=SampleImage(canvas,width,canvas->rows,exception);
            if (clone_image == (Image *) NULL)
              return((Image *) NULL);
            DestroyImage(canvas);
            canvas=clone_image;
          }
        if (Latin1Compare(keyword,"verts") == 0)
          {
            char
              *primitive;

            primitive=(char *) AllocateMemory(strlen(type)+strlen(values)+2);
            if (primitive == (char *) NULL)
              ThrowReaderException(ResourceLimitWarning,
                "Unable to allocate memory",image);
            (void) strcpy(primitive,type);
            (void) strcat(primitive," ");
            (void) strcat(primitive,values);
            (void) CloneString(&draw_info->primitive,primitive);
            FreeMemory((void *) &primitive);
            status=DrawImage(canvas,draw_info);
            if (status == False)
              ThrowReaderException(ResourceLimitWarning,
                "Unable to draw primitive",image);
          }
      }
    while (isspace(c))
      c=ReadByte(image);
  }
  FreeMemory((void *) &values);
  DestroyDrawInfo(draw_info);
  DestroyImage(image);
  return(canvas);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S V G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSVGImage adds attributes for the SVG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSVGImage method is:
%
%      RegisterSVGImage(void)
%
*/
Export void RegisterSVGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SVG");
  entry->magick=IsSVG;
  entry->decoder=ReadSVGImage;
  entry->description=AllocateString("Scalable Vector Gaphics");
  RegisterMagickInfo(entry);
}
