/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                           M   M   GGGG  K   K                               %
%                           MM MM  G      K  K                                %
%                           M M M  G  GG  KKK                                 %
%                           M   M  G   G  K  K                                %
%                           M   M   GGG   K   K                               %
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
%
*/

/*
  Include declarations.
*/
#include "studio.h"
/* 
  Forward declarations.
*/     
static unsigned int
  WriteMGKImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r e F i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadConfigureFile() reads the color configuration file which maps color
%  strings with a particular image format.
%
%  The format of the ReadConfigureFile method is:
%
%      unsigned int ReadConfigureFile(Image *image,const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigureFile returns True if at least one color
%      is defined otherwise False.
%
%    o image: The image.
%
%    o basename:  The color configuration filename.
%
%    o depth: depth of <include /> statements.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigureFile(Image *image,const char *basename,
  const unsigned long depth,ExceptionInfo *exception)
{
  const ImageAttribute
    *attribute;

  char
    keyword[MaxTextExtent],
    path[MaxTextExtent],
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the color configure file.
  */
  (void) strcpy(path,basename);
  if (depth != 0)
    xml=(char *) FileToBlob(basename,&length,exception);
  else
    {
      attribute=GetImageAttribute(image,basename);
      if (attribute == (const ImageAttribute *) NULL)
        ThrowBinaryException(ConfigureError,"No required image attribute",
          basename);
      xml=attribute->value;
    }
  if (xml == (char *) NULL)
    return(False);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    (void) strncpy(keyword,token,MaxTextExtent-1);
    if (LocaleCompare(keyword,"<!") == 0)
      {
        /*
          Comment.
        */
        while ((*token != '>') && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          (void) strncpy(keyword,token,MaxTextExtent-1);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              if (depth > 200)
                ThrowException(exception,ConfigureError,
                  "<include /> nested too deeply",path);
              else
                if (IsAccessible(token))
                  (void) ReadConfigureFile(image,token,depth+1,exception);
                else
                  {
                    char
                      filename[MaxTextExtent];
  
                    GetPathComponent(path,HeadPath,filename);
                    (void) strcat(filename,DirectorySeparator);
                    (void) strncat(filename,token,MaxTextExtent-
                      strlen(filename)-1);
                    (void) ReadConfigureFile(image,filename,depth+1,exception);
                  }
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<color") == 0)
      {
        continue;
      }
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'B':
      case 'b':
      {
        break;
      }
      case 'C':
      case 'c':
      {
        if (LocaleCompare((char *) keyword,"compliance") == 0)
          {
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        break;
      }
      case 'N':
      case 'n':
      {
        break;
      }
      case 'O':
      case 'o':
      {
        break;
      }
      case 'R':
      case 'r':
      {
        break;
      }
      case 'S':
      case 's':
      {
        break;
      }
      default:
        break;
    }
  }
  LiberateMemory((void **) &token);
  if (depth != 0)
    LiberateMemory((void **) &xml);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M G K I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMGKImage reads a Magick Configure File as a blob an attaches
%  it as an image attribute to a proxy image.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadMGKImage method is:
%
%      Image *ReadMGKImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMGKImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMGKImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  size_t
    count,
    length;

  unsigned char
    *blob;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"Unable to open file",image);
  image->columns=1;
  image->rows=1;
  length=GetBlobSize(image);
  blob=(unsigned char *) AcquireMemory(length+1);
  if (blob == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"Memory allocation failed",image);
  count=ReadBlob(image,length,blob);
  if (count != length)
    {
      LiberateMemory((void **) &blob);
      ThrowReaderException(CorruptImageError,"Unexpected end-of-file",image);
    }
  blob[length]='\0';
  if (LocaleNCompare((const char *) blob,"<?xml",5) != 0)
    {
      LiberateMemory((void **) &blob);
      ThrowReaderException(CorruptImageError,"Not a MGK file",image);
    }
  if (SetImageAttribute(image,"[MGK]",blob) == False)
    ThrowReaderException(ResourceLimitError,"Memory allocation failed",image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M G K I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMGKImage adds attributes for the MGK image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMGKImage method is:
%
%      RegisterMGKImage(void)
%
*/
ModuleExport void RegisterMGKImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MGK");
  entry->decoder=ReadMGKImage;
  entry->encoder=WriteMGKImage;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description=AcquireString("Magick Configure File");
  entry->module=AcquireString("MGK");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M G K I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMGKImage removes format registrations made by the
%  MGK module from the list of supported formats.
%
%  The format of the UnregisterMGKImage method is:
%
%      UnregisterMGKImage(void)
%
*/
ModuleExport void UnregisterMGKImage(void)
{
  (void) UnregisterMagickInfo("MGK");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M G K I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteMGKImage() writes a Magick Configure File as C source.
%
%  The format of the WriteMGKImage method is:
%
%      unsigned int WriteMGKImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMGKImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteMGKImage(const ImageInfo *image_info,Image *image)
{
  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"Unable to open file",image);
  (void) ReadConfigureFile(image,"[MGK]",0,&image->exception);
  (void) WriteBlobString(image,"Not implemented yet!\n");
  CloseBlob(image);
  return(False);
}
