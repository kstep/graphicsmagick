/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  L       OOO    CCCC   AAA   L      EEEEE                   %
%                  L      O   O  C      A   A  L      E                       %
%                  L      O   O  C      AAAAA  L      EEE                     %
%                  L      O   O  C      A   A  L      E                       %
%                  LLLLL   OOO    CCCC  A   A  LLLLL  EEEEE                   %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               September 2002                                %
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
  WriteLOCALEImage(const ImageInfo *,Image *);

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
    {
      xml=(char *) FileToBlob(basename,&length,exception);
      (void) SetImageAttribute(image,"[Locale]",xml);
    }
  else
    {
      const ImageAttribute
        *attribute;

      attribute=GetImageAttribute(image,basename);
      if (attribute == (const ImageAttribute *) NULL)
        ThrowBinaryException(ConfigureError,"No [Locale] image attribute",
          basename);
      xml=AllocateString(attribute->value);
      (void) SetImageAttribute(image,"[Locale]",(char *) NULL);
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
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
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
%   R e a d L O C A L E I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadLOCALEImage reads a Magick Configure File as a blob an attaches
%  it as an image attribute to a proxy image.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadLOCALEImage method is:
%
%      Image *ReadLOCALEImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadLOCALEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadLOCALEImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  size_t
    count,
    length;

  unsigned char
    *xml;

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
  SetImage(image,OpaqueOpacity);
  length=GetBlobSize(image);
  xml=(unsigned char *) AcquireMemory(length+1);
  if (xml == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"Memory allocation failed",image);
  count=ReadBlob(image,length,xml);
  if (count != length)
    {
      LiberateMemory((void **) &xml);
      ThrowReaderException(CorruptImageError,"Unexpected end-of-file",image);
    }
  xml[length]='\0';
  if (LocaleNCompare((const char *) xml,"<?xml",5) != 0)
    {
      LiberateMemory((void **) &xml);
      ThrowReaderException(CorruptImageError,"Not a LOCALE file",image);
    }
  if (SetImageAttribute(image,"[Locale]",xml) == False)
    ThrowReaderException(ResourceLimitError,"Memory allocation failed",image);
  (void) ReadConfigureFile(image,"[Locale]",0,&image->exception);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r L O C A L E I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterLOCALEImage adds attributes for the LOCALE image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterLOCALEImage method is:
%
%      RegisterLOCALEImage(void)
%
*/
ModuleExport void RegisterLOCALEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("LOCALE");
  entry->decoder=ReadLOCALEImage;
  entry->encoder=WriteLOCALEImage;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description=AcquireString("Locale Message File");
  entry->module=AcquireString("LOCALE");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r L O C A L E I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterLOCALEImage removes format registrations made by the
%  LOCALE module from the list of supported formats.
%
%  The format of the UnregisterLOCALEImage method is:
%
%      UnregisterLOCALEImage(void)
%
*/
ModuleExport void UnregisterLOCALEImage(void)
{
  (void) UnregisterMagickInfo("LOCALE");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e L O C A L E I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteLOCALEImage() writes a Magick Configure File as C source.
%
%  The format of the WriteLOCALEImage method is:
%
%      unsigned int WriteLOCALEImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteLOCALEImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteLOCALEImage(const ImageInfo *image_info,Image *image)
{
  const ImageAttribute
    *attribute;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"Unable to open file",image);
  (void) WriteBlobString(image,"Not implemented yet!\n");
  attribute=GetImageAttribute(image,"[Locale]");
  if (attribute == (const ImageAttribute *) NULL)
    ThrowWriterException(FileOpenError,"No [LOCALE] image attribute",image);
puts(attribute->value);
  CloseBlob(image);
  return(False);
}
