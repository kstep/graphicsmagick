/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        JJJJJ  BBBB   IIIII   GGGG                           %
%                          J    B   B    I    G                               %
%                          J    BBBB     I    G  GG                           %
%                        J J    B   B    I    G   G                           %
%                        JJJ    BBBB   IIIII   GGG                            %
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
  WriteJBIGImage(const ImageInfo *,Image *);

#if defined(HasJBIG)
#include "jbig.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J B I G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJBIGImage reads a JBIG image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadJBIGImage method is:
%
%      Image *ReadJBIGImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJBIGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadJBIGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define MaxBufferSize  8192

  Image
    *image;

  IndexPacket
    index;

  int
    status;

  long
    length,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  size_t
    count;

  struct jbg_dec_state
    jbig_info;

  unsigned char
    bit,
    *buffer;

  unsigned int
    byte;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize JBIG toolkit.
  */
  jbg_dec_init(&jbig_info);
  jbg_dec_maxsize(&jbig_info,(unsigned long) image->columns,
    (unsigned long) image->rows);
  image->columns= jbg_dec_getwidth(&jbig_info);
  image->rows= jbg_dec_getheight(&jbig_info);
  image->depth=8;
  image->storage_class=PseudoClass;
  image->colors=2;
  /*
    Read JBIG file.
  */
  buffer=(unsigned char *) AcquireMemory(MaxBufferSize);
  if (buffer == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  status=JBG_EAGAIN;
  do
  {
    length=(long) ReadBlob(image,MaxBufferSize,(char *) buffer);
    if (length == 0)
      break;
    p=buffer;
    count=0;
    while ((length > 0) && ((status == JBG_EAGAIN) || (status == JBG_EOK)))
    {
      status=jbg_dec_in(&jbig_info,p,length,&count);
      p+=count;
      length-=count;
    }
  } while ((status == JBG_EAGAIN) || (status == JBG_EOK));
  /*
    Create colormap.
  */
  image->columns= jbg_dec_getwidth(&jbig_info);
  image->rows= jbg_dec_getheight(&jbig_info);
  if (!AllocateImageColormap(image,2))
    {
      LiberateMemory((void **) &buffer);
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image)
    }
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=MaxRGB;
  image->colormap[1].green=MaxRGB;
  image->colormap[1].blue=MaxRGB;
  image->x_resolution=300;
  image->y_resolution=300;
  /*
    Convert X bitmap image to pixel packets.
  */
  p=jbg_dec_getimage(&jbig_info,0);
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    bit=0;
    byte=0;
    for (x=0; x < (long) image->columns; x++)
    {
      if (bit == 0)
        byte=(*p++);
      index=(byte & 0x80) ? 0 : 1;
      bit++;
      byte<<=1;
      if (bit == 8)
        bit=0;
      indexes[x]=index;
      *q++=image->colormap[index];
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
  }
  /*
    Free scale resource.
  */
  jbg_dec_free(&jbig_info);
  LiberateMemory((void **) &buffer);
  CloseBlob(image);
  return(image);
}
#else
static Image *ReadJBIGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "JBIG library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r J B I G I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterJBIGImage adds attributes for the JBIG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterJBIGImage method is:
%
%      RegisterJBIGImage(void)
%
*/
ModuleExport void RegisterJBIGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("BIE");
  entry->decoder=ReadJBIGImage;
  entry->encoder=WriteJBIGImage;
  entry->adjoin=False;
  entry->description=
    AllocateString("Joint Bi-level Image experts Group interchange format");
  entry->module=AllocateString("JBIG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JBG");
  entry->decoder=ReadJBIGImage;
  entry->encoder=WriteJBIGImage;
  entry->description=
    AllocateString("Joint Bi-level Image experts Group interchange format");
  entry->module=AllocateString("JBIG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JBIG");
  entry->decoder=ReadJBIGImage;
  entry->encoder=WriteJBIGImage;
  entry->description=
    AllocateString("Joint Bi-level Image experts Group interchange format");
  entry->module=AllocateString("JBIG");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r J B I G I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterJBIGImage removes format registrations made by the
%  JBIG module from the list of supported formats.
%
%  The format of the UnregisterJBIGImage method is:
%
%      UnregisterJBIGImage(void)
%
*/
ModuleExport void UnregisterJBIGImage(void)
{
  (void) UnregisterMagickInfo("BIE");
  (void) UnregisterMagickInfo("JBG");
  (void) UnregisterMagickInfo("JBIG");
}

#if defined(HasJBIG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e J B I G I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteJBIGImage writes an image in the JBIG encoded image format.
%
%  The format of the WriteJBIGImage method is:
%
%      unsigned int WriteJBIGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteJBIGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static void JBIGEncode(unsigned char *pixels,size_t length,void *data)
{
  Image
    *image;

  image=(Image *) data;
  (void) WriteBlob(image,length,pixels);
}

static unsigned int WriteJBIGImage(const ImageInfo *image_info,Image *image)
{
  long
    sans_offset,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *p;

  register unsigned char
    *q;

  struct jbg_enc_state
    jbig_info;

  unsigned char
    bit,
    *pixels,
    polarity;

  unsigned int
    byte,
    scene,
    status;

  unsigned long
    number_packets;

  /*
    Open image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Allocate pixel data.
    */
    (void) TransformRGBImage(image,RGBColorspace);
    number_packets=((image->columns+7) >> 3)*image->rows;
    pixels=(unsigned char *) AcquireMemory(number_packets);
    if (pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    /*
      Convert pixels to a bitmap.
    */
    if (!IsMonochromeImage(image))
      {
        QuantizeInfo
          quantize_info;

        GetQuantizeInfo(&quantize_info);
        quantize_info.number_colors=2;
        quantize_info.dither=image_info->dither;
        quantize_info.colorspace=GRAYColorspace;
        (void) QuantizeImage(&quantize_info,image);
      }
    polarity=Intensity(image->colormap[0]) > (0.5*MaxRGB);
    if (image->colors == 2)
      polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
    q=pixels;
    for (y=0; y < (long) image->rows; y++)
    {
      p=GetImagePixels(image,0,y,image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      bit=0;
      byte=0;
      for (x=0; x < (long) image->columns; x++)
      {
        byte<<=1;
        if (indexes[x] == polarity)
          byte|=0x01;
        bit++;
        if (bit == 8)
          {
            *q++=byte;
            bit=0;
            byte=0;
          }
       }
      if (bit != 0)
        *q++=byte << (8-bit);
      if (QuantumTick(y,image->rows))
        MagickMonitor(SaveImageText,y,image->rows);
    }
    /*
      Initialize JBIG info structure.
    */
    jbg_enc_init(&jbig_info,image->columns,image->rows,1,&pixels,
      (void (*)(unsigned char *,size_t,void *)) JBIGEncode,image);
    if (image_info->subimage != 0)
      jbg_enc_layers(&jbig_info,(int) image_info->subimage);
    else
      {
        unsigned long
          x_resolution,
          y_resolution;

        x_resolution=640;
        y_resolution=480;
        if (image_info->density != (char *) NULL)
          (void) ParseGeometry(image_info->density,&sans_offset,&sans_offset,
            &x_resolution,&y_resolution);
        (void) jbg_enc_lrlmax(&jbig_info,x_resolution,y_resolution);
      }
    (void) jbg_enc_lrange(&jbig_info,-1,-1);
    jbg_enc_options(&jbig_info,JBG_ILEAVE | JBG_SMID,JBG_TPDON | JBG_TPBON |
      JBG_DPON,-1,-1,-1);
    /*
      Write JBIG image.
    */
    jbg_enc_out(&jbig_info);
    jbg_enc_free(&jbig_info);
    LiberateMemory((void **) &pixels);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
#else
static unsigned int WriteJBIGImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(MissingDelegateWarning,"JBIG library is not available",
    image->filename);
}
#endif
