/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
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
%                   Read/Write GraphicsMagick Image Format.                   %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
#if defined(HasJBIG)
static unsigned int
  WriteJBIGImage(const ImageInfo *,Image *);
#endif

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
%    o image_info: Specifies a pointer to a ImageInfo structure.
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
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"UnableToOpenFile",image);
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
  buffer=MagickAllocateMemory(unsigned char *,MaxBufferSize);
  if (buffer == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
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
  image->columns=jbg_dec_getwidth(&jbig_info);
  image->rows=jbg_dec_getheight(&jbig_info);
  if (!AllocateImageColormap(image,2))
    {
      MagickFreeMemory(buffer);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image)
    }
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=MaxRGB;
  image->colormap[1].green=MaxRGB;
  image->colormap[1].blue=MaxRGB;
  image->x_resolution=300;
  image->y_resolution=300;
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
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
      if (!MagickMonitor(LoadImageText,y,image->rows,exception))
        break;
  }
  /*
    Free scale resource.
  */
  jbg_dec_free(&jbig_info);
  MagickFreeMemory(buffer);
  CloseBlob(image);
  return(image);
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
#define JBIGDescription  "Joint Bi-level Image experts Group interchange format"

  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(JBG_VERSION)
  (void) strncpy(version,JBG_VERSION,MaxTextExtent-1);
#endif
  entry=SetMagickInfo("BIE");
#if defined(HasJBIG)
  entry->decoder=(DecoderHandler) ReadJBIGImage;
  entry->encoder=(EncoderHandler) WriteJBIGImage;
#endif
  entry->adjoin=False;
  entry->description=AcquireString(JBIGDescription);
  if (*version != '\0')
    entry->version=AcquireString(version);
  entry->module=AcquireString("JBIG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JBG");
#if defined(HasJBIG)
  entry->decoder=(DecoderHandler) ReadJBIGImage;
  entry->encoder=(EncoderHandler) WriteJBIGImage;
#endif
  entry->description=AcquireString(JBIGDescription);
  if (*version != '\0')
    entry->version=AcquireString(version);
  entry->module=AcquireString("JBIG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JBIG");
#if defined(HasJBIG)
  entry->decoder=(DecoderHandler) ReadJBIGImage;
  entry->encoder=(EncoderHandler) WriteJBIGImage;
#endif
  entry->description=AcquireString(JBIGDescription);
  if (*version != '\0')
    entry->version=AcquireString(version);
  entry->module=AcquireString("JBIG");
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
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
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
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    x;

  register unsigned char
    *q;

  struct jbg_enc_state
    jbig_info;

  unsigned char
    bit,
    byte,
    *pixels,
    polarity;

  unsigned int
    status;

  unsigned long
    number_packets,
    scene;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
  scene=0;
  do
  {
    /*
      Allocate pixel data.
    */
    TransformColorspace(image,RGBColorspace);
    number_packets=((image->columns+7) >> 3)*image->rows;
    pixels=MagickAllocateMemory(unsigned char *,number_packets);
    if (pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",image);
    /*
      Convert pixels to a bitmap.
    */
    SetImageType(image,BilevelType);
    polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
    if (image->colors == 2)
      polarity=PixelIntensityToQuantum(&image->colormap[0]) >
        PixelIntensityToQuantum(&image->colormap[1]);
    q=pixels;
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
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
        if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
          break;
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
        long
          sans_offset;

        unsigned long
          x_resolution,
          y_resolution;

        x_resolution=640;
        y_resolution=480;
        sans_offset=0;
        if (image_info->density != (char *) NULL)
          (void) GetGeometry(image_info->density,&sans_offset,&sans_offset,
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
    MagickFreeMemory(pixels);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    if (!MagickMonitor(SaveImagesText,scene++,GetImageListLength(image),&image->exception))
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
#endif
