/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        M   M  EEEEE  TTTTT   AAA                            %
%                        MM MM  E        T    A   A                           %
%                        M M M  EEE      T    AAAAA                           %
%                        M   M  E        T    A   A                           %
%                        M   M  EEEEE    T    A   A                           %
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
  WriteMETAImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M E T A                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMETA returns True if the image format type, identified by the
%  magick string, is META.
%
%  The format of the IsMETA method is:
%
%      unsigned int IsMETA(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMETA returns True if the image format type is META.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsMETA(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"8BIM",4) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"APP1",4) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"\034\002",2) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M E T A I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMETAImage reads a META image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadMETAImage method is:
%
%      Image *ReadMETAImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  Decompression code contributed by Kyle Shorter.
%
%  A description of each parameter follows:
%
%    o image: Method ReadMETAImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMETAImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;
    
  int
    c;

  register int
    i;
    
  register unsigned char
    *q;
    
  size_t
    length;
    
  unsigned int
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  image->columns=1;
  image->rows=1;
  SetImage(image,OpaqueOpacity);
  length=MaxTextExtent;
  if (LocaleCompare(image_info->magick,"8BIM") == 0)
    {
      /*
        Read 8BIM image.
      */
      image->iptc_profile.info=(unsigned char *) AcquireMemory(length+2);
      for (q=image->iptc_profile.info; q != (unsigned char *) NULL; q++)
      {
        c=ReadBlobByte(image);
        if (c == EOF)
          break;
        if ((size_t) (q-image->iptc_profile.info+1) >= length)
          {
            image->iptc_profile.length=q-image->iptc_profile.info;
            length<<=1;
            ReacquireMemory((void **) &image->iptc_profile.info,length+2);
            if (image->iptc_profile.info == (unsigned char *) NULL)
              break;
            q=image->iptc_profile.info+image->iptc_profile.length;
          }
        *q=(unsigned char) c;
      }
      if (image->iptc_profile.info == (unsigned char *) NULL)
        ThrowReaderException(FileOpenWarning,"Memory allocation failed",image);
      image->iptc_profile.length=q-image->iptc_profile.info;
    }
  if (LocaleCompare(image_info->magick,"APP1") == 0)
    {
      /*
        Read APP1 image.
      */
      i=image->generic_profiles;
      if (image->generic_profile == (ProfileInfo *) NULL)
        image->generic_profile=(ProfileInfo *) AcquireMemory(sizeof(ProfileInfo));
      else
        ReacquireMemory((void **) &image->generic_profile,
          (i+1)*sizeof(ProfileInfo));
      if (image->generic_profile == (ProfileInfo *) NULL)
        {
          image->generic_profiles=0;
          ThrowReaderException(FileOpenWarning,"Memory allocation failed",image)
        }
      image->generic_profiles++;
      image->generic_profile[i].name=AllocateString((char *) NULL);
      FormatString(image->generic_profile[i].name,"APP%d",1);
      image->generic_profile[i].info=(unsigned char *) AcquireMemory(length+2);
      for (q=image->generic_profile[i].info; image->generic_profile[i].info; q++)
      {
        c=ReadBlobByte(image);
        if (c == EOF)
          break;
        if ((q-image->generic_profile[i].info+1) >= (int) length)
          {
            image->generic_profile[i].length=q-image->generic_profile[i].info;
            length<<=1;
            ReacquireMemory((void **) &image->generic_profile[i].info,length+2);
            if (image->generic_profile[i].info == (unsigned char *) NULL)
              break;
            q=image->generic_profile[i].info+image->generic_profile[i].length;
          }
        *q=(unsigned char) c;
      }
      if (image->generic_profile[i].info == (unsigned char *) NULL)
        ThrowReaderException(FileOpenWarning,"Memory allocation failed",image);
      image->generic_profile[i].length=q-image->generic_profile[i].info;
    }
  if (LocaleCompare(image_info->magick,"ICM") == 0)
    {
      image->color_profile.info=(unsigned char *) AcquireMemory(length);
      for (q=image->color_profile.info; ; q++)
      {
        c=ReadBlobByte(image);
        if (c == EOF)
          break;
        if ((q-image->color_profile.info+1) >= (int) length)
          {
            image->color_profile.length=q-image->color_profile.info;
            length<<=1;
            ReacquireMemory((void **) &image->color_profile.info,length);
            if (image->color_profile.info == (unsigned char *) NULL)
              break;
            q=image->color_profile.info+image->color_profile.length;
          }
        *q=(unsigned char) c;
      }
      image->color_profile.length=0;
      if (image->color_profile.info != (unsigned char *) NULL)
        image->color_profile.length=q-image->color_profile.info;
    }
  if (LocaleCompare(image_info->magick,"IPTC") == 0)
    {
      unsigned char
        *data;

      unsigned int
        tag_length;

      /*
        Read IPTC image.
      */
      tag_length=12;
      data=(unsigned char *) AcquireMemory(length+2);
      if (data == (unsigned char *) NULL)
        ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
          image);
      (void) memcpy(data,"8BIM\04\04\0\0\0\0\0\0",tag_length);
      q=data;
      q+=tag_length;
      for ( ; ; )
      {
        c=ReadBlobByte(image);
        if (c == EOF)
          break;
        if ((size_t) (q-data+1) >= length)
          {
            image->iptc_profile.length=q-data;
            length<<=1;
            ReacquireMemory((void **) &data,length+2);
            if (data == (unsigned char *) NULL)
              break;
            q=data+image->iptc_profile.length;
          }
        *q++=(unsigned char) c;
      }
      image->iptc_profile.length=0;
      if (data != (unsigned char *) NULL)
        {
          image->iptc_profile.length=q-data;
          length=image->iptc_profile.length-tag_length;
          data[10]=length >> 8;
          data[11]=length & 0xff;
          image->iptc_profile.info=data;
        }
    }
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M E T A I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMETAImage adds attributes for the META image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMETAImage method is:
%
%      RegisterMETAImage(void)
%
*/
ModuleExport void RegisterMETAImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("8BIM");
  entry->decoder=ReadMETAImage;
  entry->encoder=WriteMETAImage;
  entry->magick=IsMETA;
  entry->adjoin=False;
  entry->description=AllocateString("Photoshop resource format");
  entry->module=AllocateString("8BIM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("APP1");
  entry->decoder=ReadMETAImage;
  entry->encoder=WriteMETAImage;
  entry->magick=IsMETA;
  entry->adjoin=False;
  entry->description=AllocateString("Photoshop resource format");
  entry->module=AllocateString("APP1");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("ICM");
  entry->decoder=ReadMETAImage;
  entry->encoder=WriteMETAImage;
  entry->adjoin=False;
  entry->description=AllocateString("ICC Color Profile");
  entry->module=AllocateString("ICM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("IPTC");
  entry->decoder=ReadMETAImage;
  entry->encoder=WriteMETAImage;
  entry->magick=IsMETA;
  entry->adjoin=False;
  entry->description=AllocateString("IPTC Newsphoto");
  entry->module=AllocateString("IPTC");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M E T A I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMETAImage removes format registrations made by the
%  META module from the list of supported formats.
%
%  The format of the UnregisterMETAImage method is:
%
%      UnregisterMETAImage(void)
%
*/
ModuleExport void UnregisterMETAImage(void)
{
  (void) UnregisterMagickInfo("8BIM");
  (void) UnregisterMagickInfo("APP1");
  (void) UnregisterMagickInfo("ICM");
  (void) UnregisterMagickInfo("IPTC");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M E T A I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMETAImage writes a META image to a file.
%
%  The format of the WriteMETAImage method is:
%
%      unsigned int WriteMETAImage(const ImageInfo *image_info,Image *image)
%
%  Compression code contributed by Kyle Shorter.
%
%  A description of each parameter follows:
%
%    o status: Method WriteMETAImage return True if the image is written.
%      False is returned if there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: A pointer to a Image structure.
%
%
*/

static long GetIPTCStream(unsigned char **info,unsigned long length)
{
  unsigned char
    buffer[4];

  register int
    i;

  register unsigned char
    *p;

  unsigned char
    c;

  unsigned int
    marker;

  unsigned long
    info_length,
    tag_length;

  /*
    Find the beginning of the IPTC info.
  */
  p=(*info);
  tag_length=0;
  info_length=0;
  marker=False;
  while (length != 0)
  {
    c=(*p++);
    length--;
    if (length == 0)
      break; 
    if (c == 0x1c)
      {
        p--;
        *info=p; /* let the caller know were it is */
        break;
      }
  }
  /*
    Determine the length of the IPTC info.
  */
  while (length != 0)
  {
    c=(*p++);
    length--;
    if (length == 0)
      break; 
    if (c == 0x1c)
      marker=True;
    else
      if (marker)
        break; 
      else
        continue;
    info_length++;
    /*
      Found the 0x1c tag; skip the dataset and record number tags.
    */
    p++; /* dataset */
    length--;
    if (length == 0)
      break; 
    info_length++;
    p++; /* record number */
    length--;
    if (length == 0)
      break; 
    info_length++;
    /*
      Decode the length of the block that follows - long or short format.
    */
    c=(*p++);
    length--;
    if (length == 0)
      break; 
    info_length++;
    if (c & (unsigned char) 0x80)
      {
        for (i=0; i < 4; i++)
        {
          buffer[i]=(*p++);
          length--;
          if (length == 0)
            break; 
          info_length++;
        }
        tag_length=(((long) buffer[0]) << 24) | (((long) buffer[1]) << 16) |
          (((long) buffer[2]) << 8) | (((long) buffer[3]));
      }
    else
      {
        tag_length=((long) c) << 8;
        c=(*p++);
        length--;
        if (length == 0)
          break; 
        info_length++;
        tag_length|=(long) c;
      }
    if (tag_length > length)
      break;
    p+=tag_length;
    length-=tag_length;
    if (length == 0)
      break; 
    info_length+=tag_length;  
  }
  return(info_length);
}

static unsigned int WriteMETAImage(const ImageInfo *image_info,Image *image)
{
  register int
    i;

  unsigned int
    status;

  /*
    Open image file.
  */   
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (LocaleCompare(image_info->magick,"8BIM") == 0)
    {
      /*
        Write 8BIM image.
      */
      if (image->iptc_profile.length == 0) 
        ThrowWriterException(FileOpenWarning,"No 8BIM data is available",image);
      status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
      (void) WriteBlob(image,image->iptc_profile.length,
        (char *) image->iptc_profile.info);
    }
  if (LocaleCompare(image_info->magick,"8BIM") == 0)
    {
      /*
        Write APP1 image.
      */
      for (i=0; i < (int) image->generic_profiles; i++)
      {
        if ((LocaleCompare(image->generic_profile[i].name,"APP1") == 0) &&
            (image->generic_profile[i].length != 0))
          {
            /*
              Open image file.
            */
            status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
            if (status == False)
              ThrowWriterException(FileOpenWarning,"Unable to open file",image);
            (void) WriteBlob(image,(int) image->generic_profile[i].length,
              (char *) image->generic_profile[i].info);
            CloseBlob(image);
            return(True);
          }
      }
      ThrowWriterException(FileOpenWarning,"No APP1 data is available",image)
    }
  if (LocaleCompare(image_info->magick,"ICM") == 0)
    {
      /*
        Write ICM image.
      */
      if (image->color_profile.length == 0)
        ThrowWriterException(FileOpenWarning,"No color profile available",image);
      status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
      (void) WriteBlob(image,image->color_profile.length,
        (char *) image->color_profile.info);
    }
  if (LocaleCompare(image_info->magick,"IPTC") == 0)
    {
      size_t
        length;

      unsigned char
        *info;

      if (image->iptc_profile.length == 0)
        ThrowWriterException(FileOpenWarning,"No IPTC profile available",image);
      info=image->iptc_profile.info;
      length=image->iptc_profile.length;
      length=GetIPTCStream(&info,length);
      if (length == 0)
        ThrowWriterException(FileOpenWarning,"No IPTC info was found",image);
      (void) WriteBlob(image,length,info);
    }
  CloseBlob(image);
  return(True);
}
