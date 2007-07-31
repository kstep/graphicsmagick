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
%                            PPPP   N   N  M   M                              %
%                            P   P  NN  N  MM MM                              %
%                            PPPP   N N N  M M M                              %
%                            P      N  NN  M   M                              %
%                            P      N   N  M   M                              %
%                                                                             %
%                                                                             %
%              Read/Write PBMPlus Portable Anymap Image Format.               %
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
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/log.h"
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WritePNMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P N M                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPNM returns True if the image format type, identified by the
%  magick string, is PNM.
%
%  The format of the IsPNM method is:
%
%      unsigned int IsPNM(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPNM returns True if the image format type is PNM.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPNM(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if ((*magick == 'P') && isdigit((int) magick[1]))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P N M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPNMImage reads a Portable Anymap image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  The format of the ReadPNMImage method is:
%
%      Image *ReadPNMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPNMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static unsigned int PNMInteger(Image *image,const unsigned int base)
{
#define P7Comment  "END_OF_COMMENTS\n"

  int
    c;

  unsigned long
    value;

  /*
    Skip any leading whitespace.
  */
  do
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      return(0);
    if (c == '#')
      {
        char
          *comment;

        ExtendedSignedIntegralType
          offset;

        register char
          *p,
          *q;

        size_t
          length;

        /*
          Read comment.
        */
        length=MaxTextExtent;
        comment=MagickAllocateMemory(char *,length+strlen(P7Comment)+1);
        p=comment;
        offset=p-comment;
        if (comment != (char *) NULL)
          for ( ; (c != EOF) && (c != '\n'); p++)
          {
            if ((size_t) (p-comment) >= length)
              {
                length<<=1;
                length+=MaxTextExtent;
                MagickReallocMemory(comment,length+strlen(P7Comment)+1);
                if (comment == (char *) NULL)
                  break;
                p=comment+strlen(comment);
              }
            c=ReadBlobByte(image);
            *p=c;
            *(p+1)='\0';
          }
        if (comment == (char *) NULL)
          return(0);
        q=comment+offset;
        if (LocaleCompare(q,P7Comment) == 0)
          *q='\0';
        (void) SetImageAttribute(image,"comment",comment);
        MagickFreeMemory(comment);
        continue;
      }
  } while (!isdigit(c));
  if (base == 2)
    return(c-'0');
  /*
    Evaluate number.
  */
  value=0;
  do
  {
    value*=10;
    value+=c-'0';
    c=ReadBlobByte(image);
    if (c == EOF)
      return(value);
  }
  while (isdigit(c));
  return(value);
}

#define ValidateScalingIndex(image, index, max) \
  do \
  { \
    if (index > max) \
      ThrowReaderException(CorruptImageError,CorruptImage, image); \
  } while (0)

#define ValidateScalingPixel(image, pixel, max) \
  do \
  { \
    ValidateScalingIndex(image, pixel.red, max); \
    ValidateScalingIndex(image, pixel.green, max); \
    ValidateScalingIndex(image, pixel.blue, max); \
  } while (0)

static Image *ReadPNMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    format;

  Image
    *image;

  unsigned long
    y;

  LongPixelPacket
    pixel;

  MonitorHandler
    handler;

  register IndexPacket
    *indexes;

  register unsigned long
    x;

  register PixelPacket
    *q;

  register unsigned long
    i;

  register unsigned char
    *p;

  size_t
    count,
    number_pixels;

  unsigned char
    *pixels;

  unsigned int
    index,
    status;

  unsigned long
    max_value,
    packets;

  Quantum
    *scale;

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
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read PNM image.
  */
  count=ReadBlob(image,1,(char *) &format);
  do
  {
    /*
      Initialize image structure.
    */
    if ((count == 0) || (format != 'P'))
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    format=ReadBlobByte(image);
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Format Id: P%c",
                          format);
    if (format == '7')
      (void) PNMInteger(image,10);
    image->columns=PNMInteger(image,10);
    image->rows=PNMInteger(image,10);
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Dimensions: %lux%lu",
                          image->columns,image->rows);
    if ((format == '1') || (format == '4'))
      max_value=1;  /* bitmap */
    else
      max_value=PNMInteger(image,10);
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Max Value: %lu",
                          max_value);
    image->depth=max_value < 256 ? 8 : QuantumDepth;
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Image Depth: %u",
                          image->depth); 
    if ((format != '3') && (format != '6'))
      {
        image->storage_class=PseudoClass;
        image->colors=
          max_value >= MaxColormapSize ? MaxColormapSize : max_value+1;
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colors: %u",
                              image->colors);
      }
    number_pixels=image->columns*image->rows;
    if (number_pixels == 0)
      ThrowReaderException(CorruptImageError,NegativeOrZeroImageSize,image);
    scale=(Quantum *) NULL;
    if (image->storage_class == PseudoClass)
      {
        /*
          Create colormap.
        */
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        if ((format == '7') && (image->colors == 256))
          {
            /*
              Initialize 332 colormap.
            */
            i=0;
            for (pixel.red=0; pixel.red < 8; pixel.red++)
              for (pixel.green=0; pixel.green < 8; pixel.green++)
                for (pixel.blue=0; pixel.blue < 4; pixel.blue++)
                {
                  image->colormap[i].red=(Quantum)
                    (((double) MaxRGB*pixel.red)/0x07+0.5);
                  image->colormap[i].green=(Quantum)
                    (((double) MaxRGB*pixel.green)/0x07+0.5);
                  image->colormap[i].blue=(Quantum)
                    (((double) MaxRGB*pixel.blue)/0x03+0.5);
                  i++;
                }
          }
      }
    if ((image->storage_class != PseudoClass) || (max_value > MaxRGB))
      {
        /*
          Compute pixel scaling table.
        */
        scale=MagickAllocateMemory(Quantum *,
          (max_value+1)*sizeof(Quantum));
        if (scale == (Quantum *) NULL)
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        for (i=0; i <= max_value; i++)
          scale[i]=ScaleAnyToQuantum((unsigned long) i, max_value);
      }
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Convert PNM pixels to runlength-encoded MIFF packets.
    */
    switch (format)
    {
      case '1':
      {
        /*
          Convert PBM image to pixel packets.
        */
        for (y=0; y < image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < image->columns; x++)
          {
            index=!PNMInteger(image,2);
            if (EOFBlob(image))
               break;
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
          if (EOFBlob(image))
             break;
        }
        image->is_grayscale=MagickTrue;
        image->is_monochrome=MagickTrue;
        if (EOFBlob(image))
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
            image->filename);
        break;
      }
      case '2':
      {
        /*
          Convert PGM image to pixel packets.
        */
        unsigned long
          intensity;

        MagickBool
          is_grayscale,
          is_monochrome;

        is_grayscale=MagickTrue;
        is_monochrome=MagickTrue;
        for (y=0; y < image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < image->columns; x++)
          {
            intensity=PNMInteger(image,10);
            ValidateScalingIndex(image, intensity, max_value);
            if (EOFBlob(image))
               break;
            if (scale != (Quantum *) NULL)
              intensity=scale[intensity];
            index=intensity;
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q=image->colormap[index];
            is_monochrome &= IsMonochrome(*q);
            q++;
          }
          if (EOFBlob(image))
             break;
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        image->is_monochrome=is_monochrome;
        image->is_grayscale=is_grayscale;
        if (EOFBlob(image))
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
            image->filename);
        break;
      }
      case '3':
      {
        /*
          Convert PNM image to pixel packets.
        */
        MagickBool
          is_grayscale,
          is_monochrome;

        is_grayscale=MagickTrue;
        is_monochrome=MagickTrue;
        for (y=0; y < image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < image->columns; x++)
          {
            pixel.red=PNMInteger(image,10);
            pixel.green=PNMInteger(image,10);
            pixel.blue=PNMInteger(image,10);
            if (EOFBlob(image))
               break;
            ValidateScalingPixel(image, pixel, max_value);
            if (scale != (Quantum *) NULL)
              {
                pixel.red=scale[pixel.red];
                pixel.green=scale[pixel.green];
                pixel.blue=scale[pixel.blue];
              }
            q->red=(Quantum) pixel.red;
            q->green=(Quantum) pixel.green;
            q->blue=(Quantum) pixel.blue;
            is_monochrome &= IsMonochrome(*q);
            is_grayscale &= IsGray(*q);
            q++;
          }
          if (EOFBlob(image))
             break;
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        image->is_monochrome=is_monochrome;
        image->is_grayscale=is_grayscale;
        if (EOFBlob(image))
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
            image->filename);
        break;
      }
      case '4':
      {
        unsigned char
          bit,
          byte;

        /*
          Convert PBM raw image to pixel packets.
        */
        for (y=0; y < image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          bit=0;
          byte=0;
          for (x=0; x < image->columns; x++)
          {
            if (bit == 0)
              byte=ReadBlobByte(image);
            if (EOFBlob(image))
               break;
            index=(byte & 0x80) ? 0x00 : 0x01;
            indexes[x]=index;
            *q++=image->colormap[index];
            bit++;
            if (bit == 8)
              bit=0;
            byte<<=1;
          }
          if (EOFBlob(image))
            break;
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        image->is_grayscale=MagickTrue;
        image->is_monochrome=MagickTrue;
        if (EOFBlob(image))
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
            image->filename);
        break;
      }
      case '5':
      case '7':
      {
        /*
          Convert PGM raw image to pixel packets.
        */
        MagickBool
          is_grayscale,
          is_monochrome;

        is_grayscale=MagickTrue;
        is_monochrome=MagickTrue;
        packets=image->depth <= 8 ? 1 : 2;
        pixels=MagickAllocateMemory(unsigned char *,packets*image->columns);
        if (pixels == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        for (y=0; y < image->rows; y++)
        {
          count=ReadBlob(image,packets*image->columns,pixels);
          if (count == 0)
            ThrowReaderException(CorruptImageError,UnableToReadImageData,
              image);
          p=pixels;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          if (image->depth <= 8)
            for (x=0; x < image->columns; x++)
            {
              index=(*p++);
              if (index >= image->colors)
                {
                  ThrowException(&image->exception,CorruptImageError,
                    InvalidColormapIndex,image->filename);
                  index=0;
                }
              indexes[x]=index;
              *q=image->colormap[index];
              is_monochrome &= IsMonochrome(*q);
              q++;
            }
          else
            {
              for (x=0; x < image->columns; x++)
              {
                index=(*p << 8) | *(p+1);
                p+=2;
                VerifyColormapIndex(image,index);
                if (EOFBlob(image))
                  break;
                indexes[x]=index;
                *q=image->colormap[index];
                is_monochrome &= IsMonochrome(*q);
                q++;
              }
              if (EOFBlob(image))
                break;
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        MagickFreeMemory(pixels);
        image->is_monochrome=is_monochrome;
        image->is_grayscale=is_grayscale;
        if (EOFBlob(image))
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
            image->filename);
        break;
      }
      case '6':
      {
        /*
          Convert PPM raw raster image to pixel packets.
        */
        MagickBool
          is_grayscale,
          is_monochrome;

        is_grayscale=MagickTrue;
        is_monochrome=MagickTrue;
        packets=image->depth <= 8 ? 3 : 6;
        pixels=MagickAllocateMemory(unsigned char *,packets*image->columns);
        if (pixels == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        for (y=0; y < image->rows; y++)
        {
          count=ReadBlob(image,packets*image->columns,pixels);
          if (count == 0)
            ThrowReaderException(CorruptImageError,UnableToReadImageData,
              image);
          p=pixels;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          if (image->depth <= 8)
            for (x=0; x < image->columns; x++)
            {
              pixel.red=(*p++);
              pixel.green=(*p++);
              pixel.blue=(*p++);
              ValidateScalingPixel(image, pixel, max_value);
              if (scale != (Quantum *) NULL)
                {
                  pixel.red=scale[pixel.red];
                  pixel.green=scale[pixel.green];
                  pixel.blue=scale[pixel.blue];
                }
              q->red=(Quantum) pixel.red;
              q->green=(Quantum) pixel.green;
              q->blue=(Quantum) pixel.blue;
              is_monochrome &= IsMonochrome(*q);
              is_grayscale &= IsGray(*q);
              q++;
            }
          else
            for (x=0; x < image->columns; x++)
            {
              pixel.red=(*p << 8) | *(p+1);
              p+=2;
              pixel.green=(*p << 8) | *(p+1);
              p+=2;
              pixel.blue=(*p << 8) | *(p+1);
              p+=2;
              ValidateScalingPixel(image, pixel, max_value);
              if (scale != (Quantum *) NULL)
                {
                  pixel.red=scale[pixel.red];
                  pixel.green=scale[pixel.green];
                  pixel.blue=scale[pixel.blue];
                }
              q->red=(Quantum) pixel.red;
              q->green=(Quantum) pixel.green;
              q->blue=(Quantum) pixel.blue;
              is_monochrome &= IsMonochrome(*q);
              is_grayscale &= IsGray(*q);
              q++;
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        MagickFreeMemory(pixels);
        handler=SetMonitorHandler((MonitorHandler) NULL);
        (void) SetMonitorHandler(handler);
        image->is_monochrome=is_monochrome;
        image->is_grayscale=is_grayscale;
        if (EOFBlob(image))
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
            image->filename);
        break;
      }
      default:
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image)
    }
    if (scale != (Quantum *) NULL)
      MagickFreeMemory(scale);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if ((format == '1') || (format == '2') || (format == '3'))
      do
      {
        /*
          Skip to end of line.
        */
        count=ReadBlob(image,1,&format);
        if (count == 0)
          break;
      } while (format != '\n');
    count=ReadBlob(image,1,(char *) &format);
    if ((count != 0) && (format == 'P'))
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        if (!MagickMonitor(LoadImagesText,TellBlob(image),GetBlobSize(image),exception))
          break;
      }
  } while ((count != 0) && (format == 'P'));
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P N M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPNMImage adds attributes for the PNM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPNMImage method is:
%
%      RegisterPNMImage(void)
%
*/
ModuleExport void RegisterPNMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("P7");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description=AcquireString("Xv thumbnail format");
  entry->module=AcquireString("PNM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PBM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description=AcquireString("Portable bitmap format (black and white)");
  entry->module=AcquireString("PNM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PGM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description=AcquireString("Portable graymap format (gray scale)");
  entry->module=AcquireString("PNM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PNM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->magick=(MagickHandler) IsPNM;
  entry->description=AcquireString("Portable anymap");
  entry->module=AcquireString("PNM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PPM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description=AcquireString("Portable pixmap format (color)");
  entry->module=AcquireString("PNM");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P N M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPNMImage removes format registrations made by the
%  PNM module from the list of supported formats.
%
%  The format of the UnregisterPNMImage method is:
%
%      UnregisterPNMImage(void)
%
*/
ModuleExport void UnregisterPNMImage(void)
{
  (void) UnregisterMagickInfo("P7");
  (void) UnregisterMagickInfo("PBM");
  (void) UnregisterMagickInfo("PGM");
  (void) UnregisterMagickInfo("PNM");
  (void) UnregisterMagickInfo("PPM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P N M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WritePNMImage writes an image to a file in the PNM rasterfile
%  format.
%
%  The format of the WritePNMImage method is:
%
%      unsigned int WritePNMImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePNMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WritePNMImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  const ImageAttribute
    *attribute;

  IndexPacket
    index;

  int
    format;

  long
    j,
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  unsigned int
    scene,
    status;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  scene=0;
  do
  {
    /*
      Write PNM file header.
    */
    (void) TransformColorspace(image,RGBColorspace);
    format=6;
    if (LocaleCompare(image_info->magick,"PGM") == 0)
      format=5;
    else
      if (LocaleCompare(image_info->magick,"PBM") == 0)
        format=4;
      else
        if ((LocaleCompare(image_info->magick,"PNM") == 0) &&
            (image_info->type != TrueColorType) &&
            IsGrayImage(image,&image->exception))
          {
            format=5;
            if (IsMonochromeImage(image,&image->exception))
              format=4;
          }
    if (image_info->compression == NoCompression)
      format-=3;
    if (LocaleCompare(image_info->magick,"P7") != 0)
      FormatString(buffer,"P%d\n",format);
    else
      {
        format=7;
        (void) strcpy(buffer,"P7 332\n");
      }
    (void) WriteBlobString(image,buffer);
    attribute=GetImageAttribute(image,"comment");
    if (attribute != (const ImageAttribute *) NULL)
      {
        register char
          *p;

        /*
          Write comments to file.
        */
        (void) WriteBlobByte(image,'#');
        for (p=attribute->value; *p != '\0'; p++)
        {
          (void) WriteBlobByte(image,*p);
          if ((*p == '\n') && (*(p+1) != '\0'))
            (void) WriteBlobByte(image,'#');
        }
        (void) WriteBlobByte(image,'\n');
      }
    if (format != 7)
      {
        FormatString(buffer,"%lu %lu\n",image->columns,image->rows);
        (void) WriteBlobString(image,buffer);
      }
    /*
      Convert runlength encoded to PNM raster pixels.
    */
    switch (format)
    {
      case 1:
      {
        unsigned int
          polarity;

        /*
          Convert image to a PBM image.
        */
        (void) SetImageType(image,BilevelType);
        polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
        if (image->colors == 2)
          polarity=PixelIntensityToQuantum(&image->colormap[0]) <
            PixelIntensityToQuantum(&image->colormap[1]);
        i=0;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            FormatString(buffer,"%u ",indexes[x] == polarity ? 0x00 : 0x01);
            (void) WriteBlobString(image,buffer);
            i++;
            if (i == 36)
              {
                (void) WriteBlobByte(image,'\n');
                i=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        if (i != 0)
          (void) WriteBlobByte(image,'\n');
        break;
      }
      case 2:
      {
        /*
          Convert image to a PGM image.
        */
        if (image->depth <= 8)
          (void) WriteBlobString(image,"255\n");
        else
          (void) WriteBlobString(image,"65535\n");
        i=0;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            index=PixelIntensityToQuantum(p);
            if (image->depth <= 8)
              FormatString(buffer," %u",ScaleQuantumToChar(index));
            else
              FormatString(buffer," %u",ScaleQuantumToShort(index));
            (void) WriteBlobString(image,buffer);
            i++;
            if (i == 12)
              {
                (void) WriteBlobByte(image,'\n');
                i=0;
              }
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        if (i != 0)
          (void) WriteBlobByte(image,'\n');
        break;
      }
      case 3:
      {
        /*
          Convert image to a PNM image.
        */
        if (image->depth <= 8)
          (void) WriteBlobString(image,"255\n");
        else
          (void) WriteBlobString(image,"65535\n");
        i=0;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            if (image->depth <= 8)
              FormatString(buffer,"%u %u %u ",ScaleQuantumToChar(p->red),
                ScaleQuantumToChar(p->green),ScaleQuantumToChar(p->blue));
            else
              FormatString(buffer,"%u %u %u ",ScaleQuantumToShort(p->red),
                ScaleQuantumToShort(p->green),ScaleQuantumToShort(p->blue));
            (void) WriteBlobString(image,buffer);
            i++;
            if (i == 4)
              {
                (void) WriteBlobByte(image,'\n');
                i=0;
              }
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        if (i != 0)
          (void) WriteBlobByte(image,'\n');
        break;
      }
      case 4:
      {
        register unsigned char
          bit,
          byte,
          polarity;

        /*
          Convert image to a PBM image.
        */
        (void) SetImageType(image,BilevelType);
        polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
        if (image->colors == 2)
          polarity=PixelIntensityToQuantum(&image->colormap[0]) <
            PixelIntensityToQuantum(&image->colormap[1]);
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
            if (indexes[x] != polarity)
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                (void) WriteBlobByte(image,byte);
                bit=0;
                byte=0;
              }
            p++;
          }
          if (bit != 0)
            (void) WriteBlobByte(image,byte << (8-bit));
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        break;
      }
      case 5:
      {
        /*
          Convert image to a PGM image.
        */
        if (image->depth <= 8)
          (void) WriteBlobString(image,"255\n");
        else
          (void) WriteBlobString(image,"65535\n");
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          if (image->depth <= 8)
            for (x=0; x < (long) image->columns; x++)
            {
              (void) WriteBlobByte(image,
                ScaleQuantumToChar(PixelIntensityToQuantum(p)));
              p++;
            }
          else
            for (x=0; x < (long) image->columns; x++)
            {
              (void) WriteBlobMSBShort(image,
                ScaleQuantumToShort(PixelIntensityToQuantum(p)));
              p++;
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        break;
      }
      case 6:
      {
        register unsigned char
          *q;

        size_t
          packets;

        unsigned char
          *pixels;

        /*
          Allocate memory for pixels.
        */
        packets=image->depth <= 8 ? 3 : 6;
        pixels=MagickAllocateMemory(unsigned char *,packets*image->columns);
        if (pixels == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
            image);
        /*
          Convert image to a PNM image.
        */
        if (image->depth <= 8)
          (void) WriteBlobString(image,"255\n");
        else
          (void) WriteBlobString(image,"65535\n");
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=pixels;
          if (image->depth <= 8)
            for (x=0; x < (long) image->columns; x++)
            {
              *q++=ScaleQuantumToChar(p->red);
              *q++=ScaleQuantumToChar(p->green);
              *q++=ScaleQuantumToChar(p->blue);
              p++;
            }
          else
            for (x=0; x < (long) image->columns; x++)
            {
              *q++=(unsigned char) (ScaleQuantumToShort(p->red) >> 8);
              *q++=(unsigned char) ScaleQuantumToShort(p->red);
              *q++=(unsigned char) (ScaleQuantumToShort(p->green) >> 8);
              *q++=(unsigned char) ScaleQuantumToShort(p->green);
              *q++=(unsigned char) (ScaleQuantumToShort(p->blue) >> 8);
              *q++=(unsigned char) ScaleQuantumToShort(p->blue);
              p++;
            }
          (void) WriteBlob(image,q-pixels,(char *) pixels);
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        MagickFreeMemory(pixels);
        break;
      }
      case 7:
      {
        static const short int
          dither_red[2][16]=
          {
            {-16,  4, -1, 11,-14,  6, -3,  9,-15,  5, -2, 10,-13,  7, -4,  8},
            { 15, -5,  0,-12, 13, -7,  2,-10, 14, -6,  1,-11, 12, -8,  3, -9}
          },
          dither_green[2][16]=
          {
            { 11,-15,  7, -3,  8,-14,  4, -2, 10,-16,  6, -4,  9,-13,  5, -1},
            {-12, 14, -8,  2, -9, 13, -5,  1,-11, 15, -7,  3,-10, 12, -6,  0}
          },
          dither_blue[2][16]=
          {
            { -3,  9,-13,  7, -1, 11,-15,  5, -4,  8,-14,  6, -2, 10,-16,  4},
            {  2,-10, 12, -8,  0,-12, 14, -6,  3, -9, 13, -7,  1,-11, 15, -5}
          };

        long
          value;

        Quantum
          pixel;

        unsigned short
          *blue_map[2][16],
          *green_map[2][16],
          *red_map[2][16];

        /*
          Allocate and initialize dither maps.
        */
        for (i=0; i < 2; i++)
          for (j=0; j < 16; j++)
          {
            red_map[i][j]=MagickAllocateMemory(unsigned short *,
              256*sizeof(unsigned short));
            green_map[i][j]=MagickAllocateMemory(unsigned short *,
              256*sizeof(unsigned short));
            blue_map[i][j]=MagickAllocateMemory(unsigned short *,
              256*sizeof(unsigned short));
            if ((red_map[i][j] == (unsigned short *) NULL) ||
                (green_map[i][j] == (unsigned short *) NULL) ||
                (blue_map[i][j] == (unsigned short *) NULL))
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                image);
          }
        /*
          Initialize dither tables.
        */
        for (i=0; i < 2; i++)
          for (j=0; j < 16; j++)
            for (x=0; x < 256; x++)
            {
              value=x-16;
              if (x < 48)
                value=x/2+8;
              value+=dither_red[i][j];
              red_map[i][j][x]=(unsigned short)
                ((value < 0) ? 0 : (value > 255) ? 255 : value);
              value=x-16;
              if (x < 48)
                value=x/2+8;
              value+=dither_green[i][j];
              green_map[i][j][x]=(unsigned short)
                ((value < 0) ? 0 : (value > 255) ? 255 : value);
              value=x-32;
              if (x < 112)
                value=x/2+24;
              value+=2*dither_blue[i][j];
              blue_map[i][j][x]=(unsigned short)
                ((value < 0) ? 0 : (value > 255) ? 255 : value);
            }
        /*
          Convert image to a P7 image.
        */
        (void) WriteBlobString(image,"#END_OF_COMMENTS\n");
        FormatString(buffer,"%lu %lu 255\n",image->columns,image->rows);
        (void) WriteBlobString(image,buffer);
        i=0;
        j=0;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            if (!image_info->dither)
              pixel=(Quantum) ((ScaleQuantumToChar(p->red) & 0xe0) |
                ((ScaleQuantumToChar(p->green) & 0xe0) >> 3) |
                ((ScaleQuantumToChar(p->blue) & 0xc0) >> 6));
            else
              pixel=(Quantum)
                ((red_map[i][j][ScaleQuantumToChar(p->red)] & 0xe0) |
                ((green_map[i][j][ScaleQuantumToChar(p->green)] & 0xe0) >> 3) |
                ((blue_map[i][j][ScaleQuantumToChar(p->blue)] & 0xc0) >> 6));
            (void) WriteBlobByte(image,pixel);
            p++;
            j++;
            if (j == 16)
              j=0;
          }
          i++;
          if (i == 2)
            i=0;
          if (QuantumTick(y,image->rows))
            if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
              break;
        }
        /*
          Free allocated memory.
        */
        for (i=0; i < 2; i++)
          for (j=0; j < 16; j++)
          {
            MagickFreeMemory(green_map[i][j]);
            MagickFreeMemory(blue_map[i][j]);
            MagickFreeMemory(red_map[i][j]);
          }
        break;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitor(SaveImagesText,scene++,GetImageListLength(image),
      &image->exception);
    if (status == False)
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
