/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        H   H  TTTTT  M   M  L                               %
%                        H   H    T    MM MM  L                               %
%                        HHHHH    T    M M M  L                               %
%                        H   H    T    M   M  L                               %
%                        H   H    T    M   M  LLLLL                           %
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
#include "define.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteHTMLImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s H T M L                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsHTML returns True if the image format type, identified by the
%  magick string, is HTML.
%
%  The format of the IsHTML method is:
%
%      unsigned int IsHTML(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsHTML returns True if the image format type is HTML.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsHTML(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"<html",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r H T M L I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterHTMLImage adds attributes for the HTML image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterHTMLImage method is:
%
%      RegisterHTMLImage(void)
%
*/
ModuleExport void RegisterHTMLImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("HTM");
  entry->encoder=WriteHTMLImage;
  entry->magick=IsHTML;
  entry->adjoin=False;
  entry->description=
    AllocateString("Hypertext Markup Language and a client-side image map");
  entry->module=AllocateString("HTML");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("HTML");
  entry->encoder=WriteHTMLImage;
  entry->magick=IsHTML;
  entry->adjoin=False;
  entry->description=
    AllocateString("Hypertext Markup Language and a client-side image map");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("SHTML");
  entry->encoder=WriteHTMLImage;
  entry->magick=IsHTML;
  entry->adjoin=False;
  entry->description=
    AllocateString("Hypertext Markup Language and a client-side image map");
  entry->module=AllocateString("HTML");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r H T M L I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterHTMLImage removes format registrations made by the
%  HTML module from the list of supported formats.
%
%  The format of the UnregisterHTMLImage method is:
%
%      UnregisterHTMLImage(void)
%
*/
ModuleExport void UnregisterHTMLImage(void)
{
  (void) UnregisterMagickInfo("HTM");
  (void) UnregisterMagickInfo("HTML");
  (void) UnregisterMagickInfo("SHTML");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H T M L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteHTMLImage writes an image in the HTML encoded image format.
%
%  The format of the WriteHTMLImage method is:
%
%      unsigned int WriteHTMLImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteHTMLImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteHTMLImage(const ImageInfo *image_info,Image *image)
{
  char
    basename[MaxTextExtent],
    buffer[MaxTextExtent],
    filename[MaxTextExtent],
    mapname[MaxTextExtent],
    url[MaxTextExtent];

  Image
    *next;

  ImageInfo
    *clone_info;

  long
    x,
    y;

  register char
    *p;

  unsigned int
    status;

  unsigned long
    height,
    width;

  /*
    Open image.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  CloseBlob(image);
  (void) TransformRGBImage(image,RGBColorspace);
  *url='\0';
  if ((LocaleCompare(image_info->magick,"FTP") == 0) ||
      (LocaleCompare(image_info->magick,"HTTP") == 0))
    {
      /*
        Extract URL base from filename.
      */
      p=strrchr(image->filename,'/');
      if (p)
        {
          p++;
          (void) strncpy(url,image_info->magick,MaxTextExtent-1);
          (void) strcat(url,":");
          url[strlen(url)+p-image->filename]='\0';
          (void) strncat(url,image->filename,p-image->filename);
          (void) strncpy(image->filename,p,MaxTextExtent-1);
        }
    }
  /*
    Refer to image map file.
  */
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  AppendImageFormat("map",filename);
  GetPathComponent(filename,BasePath,basename);
  (void) strncpy(mapname,basename,MaxTextExtent-1);
  (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  clone_info=CloneImageInfo(image_info);
  clone_info->adjoin=True;
  status=True;
  if (LocaleCompare(image_info->magick,"SHTML") != 0)
    {
      ImageAttribute
        *attribute;

      /*
        Open output image file.
      */
      status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
      /*
        Write the HTML image file.
      */
      (void) WriteBlobString(image,"<html version=\"2.0\">\n");
      (void) WriteBlobString(image,"<head>\n");
      attribute=GetImageAttribute(image,"label");
      if (attribute != (ImageAttribute *) NULL)
        FormatString(buffer,"<title>%.1024s</title>\n",attribute->value);
      else
        {
          GetPathComponent(filename,BasePath,basename);
          FormatString(buffer,"<title>%.1024s</title>\n",basename);
        }
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"</head>\n");
      (void) WriteBlobString(image,"<body>\n");
      (void) WriteBlobString(image,"<center>\n");
      FormatString(buffer,"<h1>%.1024s</h1>\n",image->filename);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"<br><br>\n");
      (void) strncpy(filename,image->filename,MaxTextExtent-1);
      AppendImageFormat("gif",filename);
      FormatString(buffer,
        "<img ismap usemap=#%.1024s src=\"%.1024s\" border=0>\n",
        mapname,filename);
      (void) WriteBlobString(image,buffer);
      /*
        Determine the size and location of each image tile.
      */
      width=image->columns;
      height=image->rows;
      x=0;
      y=0;
      if (image->montage != (char *) NULL)
        (void) ParseGeometry(image->montage,&x,&y,&width,&height);
      /*
        Write an image map.
      */
      FormatString(buffer,"<map name=\"%.1024s\">\n",mapname);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"  <area href=\"%.1024s",url);
      (void) WriteBlobString(image,buffer);
      if (image->directory == (char *) NULL)
        {
          FormatString(buffer,"%.1024s\" shape=rect coords=0,0,%u,%u>\n",
            image->filename,width-1,height-1);
          (void) WriteBlobString(image,buffer);
        }
      else
        for (p=image->directory; *p != '\0'; p++)
          if (*p != '\n')
            (void) WriteBlobByte(image,*p);
          else
            {
              FormatString(buffer,"\" shape=rect coords=%ld,%ld,%ld,%ld>\n",
                x,y,x+(long) width-1,y+(long) height-1);
              (void) WriteBlobString(image,buffer);
              if (*(p+1) != '\0')
                {
                  FormatString(buffer,"  <area href=%.1024s\"",url);
                  (void) WriteBlobString(image,buffer);
                }
              x+=width;
              if (x >= (long) image->columns)
                {
                  x=0;
                  y+=height;
                }
            }
      (void) WriteBlobString(image,"</map>\n");
      if (image->montage != (char *) NULL)
        (void) TransparentImage(image,GetOnePixel(image,0,0),TransparentOpacity);
      (void) strncpy(filename,image->filename,MaxTextExtent-1);
      (void) WriteBlobString(image,"</center>\n");
      (void) WriteBlobString(image,"</body>\n");
      (void) WriteBlobString(image,"</html>\n");
      CloseBlob(image);
      /*
        Write the image as transparent GIF.
      */
      (void) strncpy(image->filename,filename,MaxTextExtent-1);
      AppendImageFormat("gif",image->filename);
      next=image->next;
      image->next=(Image *) NULL;
      (void) strcpy(image->magick,"GIF");
      (void) WriteImage(clone_info,image);
      image->next=next;
      /*
        Determine image map filename.
      */
      (void) strncpy(image->filename,filename,MaxTextExtent-1);
      for (p=filename+strlen(filename)-1; p > (filename+1); p--)
        if (*p == '.')
          {
            (void) strncpy(image->filename,filename,p-filename);
            image->filename[p-filename]='\0';
            break;
          }
      (void) strcat(image->filename,"_map.shtml");
    }
  /*
    Open image map.
  */
  status=OpenBlob(clone_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  DestroyImageInfo(clone_info);
  /*
    Determine the size and location of each image tile.
  */
  width=image->columns;
  height=image->rows;
  x=0;
  y=0;
  if (image->montage != (char *) NULL)
    (void) ParseGeometry(image->montage,&x,&y,&width,&height);
  /*
    Write an image map.
  */
  FormatString(buffer,"<map name=\"%.1024s\">\n",mapname);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"  <area href=\"%.1024s",url);
  (void) WriteBlobString(image,buffer);
  if (image->directory == (char *) NULL)
    {
      FormatString(buffer,"%.1024s\" shape=rect coords=0,0,%u,%u>\n",
        image->filename,width-1,height-1);
      (void) WriteBlobString(image,buffer);
    }
  else
    for (p=image->directory; *p != '\0'; p++)
      if (*p != '\n')
        (void) WriteBlobByte(image,*p);
      else
        {
          FormatString(buffer," shape=rect coords=%d,%d,%d,%d>\n",x,y,
            x+(long) width-1,y+(long) height-1);
          (void) WriteBlobString(image,buffer);
          if (*(p+1) != '\0')
            {
              FormatString(buffer,"  <area href=%.1024s\"",url);
              (void) WriteBlobString(image,buffer);
            }
          x+=width;
          if (x >= (long) image->columns)
            {
              x=0;
              y+=height;
            }
        }
  (void) WriteBlobString(image,"</map>\n");
  CloseBlob(image);
  (void) strncpy(image->filename,filename,MaxTextExtent-1);
  return(status);
}
