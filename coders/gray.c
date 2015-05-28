/*
% Copyright (C) 2003 - 2015 GraphicsMagick Group
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
%                         GGGG  RRRR    AAA   Y   Y                           %
%                        G      R   R  A   A   Y Y                            %
%                        G  GG  RRRR   AAAAA    Y                             %
%                        G   G  R R    A   A    Y                             %
%                         GGG   R  R   A   A    Y                             %
%                                                                             %
%                                                                             %
%                    Read/Write RAW Gray Image Format.                        %
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
#include "magick/pixel_cache.h"
#include "magick/constitute.h"
#include "magick/enum_strings.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteGRAYImage(const ImageInfo *,Image *);

/*
  Return an appropriate channel quantum type depending on the magick
  format specifier.
*/
static QuantumType MagickToQuantumType(const char *magick,
                                       const MagickBool gray_only)
{
  QuantumType
    quantum_type=GrayQuantum;

  /*
    Reader returns GRAY or GRAYA images while writer writes GRAY or
    GRAYA, or individual channels as GRAY.

    For the reader, if all channels are treated as in the writer case,
    then a color image would be returned, with only the specified
    channel set.  However, that is not what is done here.
  */
  if (gray_only)
    {
      if (strcmp(magick,"GRAY") == 0)
        quantum_type=GrayQuantum;
      else if (strcmp(magick,"GRAYA") == 0)
        quantum_type=GrayAlphaQuantum;
    }
  else
    {
      if (strcmp(magick,"GRAY") == 0)
        quantum_type=GrayQuantum;
      else if (strcmp(magick,"GRAYA") == 0)
        quantum_type=GrayAlphaQuantum;
      else if (strcmp(magick,"R") == 0)
        quantum_type=RedQuantum;
      else if (strcmp(magick,"G") == 0)
        quantum_type=GreenQuantum;
      else if (strcmp(magick,"B") == 0)
        quantum_type=BlueQuantum;
      else if (strcmp(magick,"O") == 0)
        quantum_type=AlphaQuantum;
      else if (strcmp(magick,"C") == 0)
        quantum_type=CyanQuantum;
      else if (strcmp(magick,"M") == 0)
        quantum_type=MagentaQuantum;
      else if (strcmp(magick,"Y") == 0)
        quantum_type=YellowQuantum;
      else if (strcmp(magick,"K") == 0)
        quantum_type=BlackQuantum;
    }

  return quantum_type;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G R A Y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGRAYImage reads an image of raw grayscale samples and returns
%  it.  It allocates the memory necessary for the new Image structure and
%   returns a pointer to the new image.
%
%  Gray with an alpha channel is supported via magick "CMYK".
%  Red, Green, Blue, or Alpha from an RGB(A) image may be individually
%  selected for input via magick specifiers "R", "G", "B", and "A".
%  Cyan, Magenta, Yellow, Black, or Alpha from an CMYK(A) image may be
%  individually selected for input via magick specifiers "C", "M", "Y",
%  "K", and "A".
%
%  The format of the ReadGRAYImage method is:
%
%      Image *ReadGRAYImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGRAYImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadGRAYImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  long
    j,
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  size_t
    count;

  unsigned char
    *scanline;

  unsigned int
    status;

  unsigned int
    depth,
    quantum_size,
    packet_size,
    samples_per_pixel;

  ImportPixelAreaOptions
    import_options;

  QuantumType
    quantum_type;

  MagickBool
    is_grayscale;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,MustSpecifyImageSize,image);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  for (i=0; i < image->offset; i++)
    {
      if (EOF == ReadBlobByte(image))
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
    }

  quantum_type=MagickToQuantumType(image_info->magick,MagickTrue);
  is_grayscale=((quantum_type == GrayQuantum) ||
                (quantum_type == GrayAlphaQuantum));
  /*
    Support depth in multiples of 8 bits.
  */
  if (image->depth > 16)
    depth=32;
  else if (image->depth > 8)
    depth=16;
  else
    depth=8;
  /*
    Allocate memory for a scanline.
  */
  if (depth <= 8)
    quantum_size=8;
  else if (depth <= 16)
    quantum_size=16;
  else
    quantum_size=32;
  samples_per_pixel=MagickGetQuantumSamplesPerPixel(quantum_type);
  packet_size=(quantum_size*samples_per_pixel)/8;
  scanline=MagickAllocateArray(unsigned char *,packet_size,image->tile_info.width);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Initialize import options.
  */
  ImportPixelAreaOptionsInit(&import_options);
  if (image_info->endian != UndefinedEndian)
    import_options.endian=image_info->endian;
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			  "Depth: %u bits, "
                          "Type: %s, "
                          "Samples/Pixel: %u, "
                          "Endian %s, "
                          "Tile: %lux%lu%+ld%+ld",
			  quantum_size,
                          QuantumTypeToString(quantum_type),
                          samples_per_pixel,
			  EndianTypeToString(import_options.endian),
                          image->tile_info.width,
                          image->tile_info.height,
                          image->tile_info.x,
                          image->tile_info.y);
  /*
    Support starting at intermediate image frame.
  */
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      for (y=0; y < (long) image->rows; y++)
        (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    }
  x=(long) (packet_size*image->tile_info.x);
  do
  {
    /*
      Convert raster image to pixel packets.
    */
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    for (y=0; y < image->tile_info.y; y++)
      (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    /*
      Support GRAYA with matte channel
    */
    if (quantum_type == GrayAlphaQuantum)
      image->matte=MagickTrue;
    for (y=0; y < (long) image->rows; y++)
    {
      if ((y > 0) || (image->previous == (Image *) NULL))
        (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
      q=SetImagePixelsEx(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      if (!is_grayscale)
        (void) memset(q,0,sizeof(PixelPacket)*image->columns);
      (void) ImportImagePixelArea(image,quantum_type,quantum_size,scanline+x,
        			  &import_options,0);
      if (!SyncImagePixelsEx(image,exception))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename,
				      image->columns,image->rows))
            break;
    }
    image->is_grayscale=is_grayscale;
    count=image->tile_info.height-image->rows-image->tile_info.y;
    for (j=0; j < (long) count; j++)
      (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    if (EOFBlob(image))
      {
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadBlob(image,packet_size*image->tile_info.width,scanline);
    if (count != 0)
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
        if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),exception,
                                    LoadImagesText,image->filename))
          break;
      }
  } while (count != 0);
  MagickFreeMemory(scanline);
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
%   R e g i s t e r G R A Y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterGRAYImage adds attributes for the GRAY image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterGRAYImage method is:
%
%      RegisterGRAYImage(void)
%
*/
ModuleExport void RegisterGRAYImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("GRAY");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->description="Raw gray samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("GRAYA");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->description="Raw gray samples + alpha";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("R");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->description="Raw red samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("C");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->description="Raw cyan samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("G");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->description="Raw green samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("M");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->description="Raw magenta samples";
  entry->module="GRAY";

  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("B");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->description="Raw blue samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("Y");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->description="Raw yellow samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("O");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->description="Raw opacity samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("K");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->description="Raw black samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r G R A Y I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterGRAYImage removes format registrations made by the
%  GRAY module from the list of supported formats.
%
%  The format of the UnregisterGRAYImage method is:
%
%      UnregisterGRAYImage(void)
%
*/
ModuleExport void UnregisterGRAYImage(void)
{
  (void) UnregisterMagickInfo("GRAY");
  (void) UnregisterMagickInfo("GRAYA");
  (void) UnregisterMagickInfo("R");
  (void) UnregisterMagickInfo("C");
  (void) UnregisterMagickInfo("G");
  (void) UnregisterMagickInfo("M");
  (void) UnregisterMagickInfo("B");
  (void) UnregisterMagickInfo("Y");
  (void) UnregisterMagickInfo("O");
  (void) UnregisterMagickInfo("K");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G R A Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteGRAYImage writes an image to a file as gray scale intensity
%  values.  Gray with an alpha channel is supported via magick "CMYK".
%  Red, Green, Blue, or Alpha from an RGB(A) image may be individually
%  selected for output via magick specifiers "R", "G", "B", and "A".
%  Cyan, Magenta, Yellow, Black, or Alpha from an CMYK(A) image may be
%  individually selected for output via magick specifiers "C", "M", "Y",
%  "K", and "A".
%
%  The format of the WriteGRAYImage method is:
%
%      unsigned int WriteGRAYImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteGRAYImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

static unsigned int WriteGRAYImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  register const PixelPacket
    *p;

  unsigned char
    *scanline=0;

  unsigned int
    depth,
    quantum_size,
    packet_size,
    samples_per_pixel,
    scene,
    status;

  ExportPixelAreaOptions
    export_options;

  ExportPixelAreaInfo
    export_info;

  QuantumType
    quantum_type;

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

  quantum_type=MagickToQuantumType(image_info->magick,MagickFalse);
  
  /*
    Support depth in multiples of 8 bits.
  */
  if (image->depth > 16)
    depth=32;
  else if (image->depth > 8)
    depth=16;
  else
    depth=8;
  /*
    Convert image to gray scale PseudoColor class.
  */
  scene=0;
  do
  {
    /*
      Allocate memory for scanline.
    */
    if (depth <= 8)
      quantum_size=8;
    else if (depth <= 16)
      quantum_size=16;
    else
      quantum_size=32;
    samples_per_pixel=MagickGetQuantumSamplesPerPixel(quantum_type);
    packet_size=(quantum_size*samples_per_pixel)/8;
    /*
      Allocate scanline
    */
    scanline=MagickAllocateArray(unsigned char *,packet_size,image->columns);
    if (scanline == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    /*
      Transform to target color space
    */
    switch (quantum_type)
      {
      case CyanQuantum:
      case MagentaQuantum:
      case YellowQuantum:
      case BlackQuantum:
        {
          (void) TransformColorspace(image,CMYKColorspace);
          break;
        }
      default:
        {
          (void) TransformColorspace(image,RGBColorspace);
        }
      }
    if (!(image->matte) &&
        ((quantum_type == GrayAlphaQuantum) || (quantum_type == AlphaQuantum)))
      (void) SetImageOpacity(image,OpaqueOpacity);
    /*
      Initialize export options.
    */
    ExportPixelAreaOptionsInit(&export_options);
    if (image->endian != UndefinedEndian)
      export_options.endian=image->endian;
    else if (image_info->endian != UndefinedEndian)
      export_options.endian=image_info->endian;
    if (image->logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "Depth: %u bits, "
                            "Type: %s, "
                            "Samples/Pixel: %u, "
                            "Endian %s",
                            quantum_size,
                            QuantumTypeToString(quantum_type),
                            samples_per_pixel,
                            EndianTypeToString(export_options.endian));
    /*
      Convert MIFF to GRAY raster scanline.
    */
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      (void) ExportImagePixelArea(image,quantum_type,quantum_size,scanline,
				  &export_options,&export_info);
      (void) WriteBlob(image,export_info.bytes_exported,scanline);
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                      SaveImageText,image->filename,
				      image->columns,image->rows))
            break;
    }
    MagickFreeMemory(scanline);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    if (!MagickMonitorFormatted(scene++,GetImageListLength(image),&image->exception,
                                SaveImagesText,image->filename))
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
