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
%                            PPPP   SSSSS  33333                              %
%                            P   P  SS        33                              %
%                            PPPP    SSS    333                               %
%                            P         SS     33                              %
%                            P      SSSSS  33333                              %
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
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/compress.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#if defined(HasTIFF)
#define CCITTParam  "-1"
#else
#define CCITTParam  "0"
#endif

/*
  Forward declarations.
*/
static unsigned int
  WritePS3Image(const ImageInfo *,Image *),
  ZLIBEncodeImage(Image *,const size_t,const unsigned long,unsigned char *);

#if defined(HasTIFF)
#if defined(HAVE_TIFFCONF_H)
#include "tiffconf.h"
#endif
#include "tiffio.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n 2 D E n c o d e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Huffman2DEncodeImage compresses an image via two-dimensional
%  Huffman-coding.
%
%  The format of the Huffman2DEncodeImage method is:
%
%      unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method Huffman2DEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image_info: The image info..
%
%    o image: The image.
%
*/
static unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  char
    filename[MaxTextExtent];

  Image
    *huffman_image;

  ImageInfo
    *clone_info;

  long
    count,
    j;

  register long
    i;

  TIFF
    *tiff;

  uint16
    fillorder;

  unsigned char
    *buffer;

  unsigned int
    status;

  unsigned long
    *byte_count,
    strip_size;

  /*
    Write image as CCITTFax4 TIFF image to a temporary file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if(!AcquireTemporaryFileName(filename))
    {
      ThrowBinaryException(FileOpenError,"UnableToCreateTemporaryFile",
        filename)
    }
  huffman_image=CloneImage(image,0,0,True,&image->exception);
  if (huffman_image == (Image *) NULL)
    return(False);
  SetImageType(huffman_image,BilevelType);
  FormatString(huffman_image->filename,"tiff:%s",filename);
  clone_info=CloneImageInfo(image_info);
  clone_info->compression=Group4Compression;
  status=WriteImage(clone_info,huffman_image);
  DestroyImageInfo(clone_info);
  DestroyImage(huffman_image);
  if (status == False)
    {
      LiberateTemporaryFile(filename);
      return(False);
    }
  tiff=TIFFOpen(filename,"rb");
  if (tiff == (TIFF *) NULL)
    {
      LiberateTemporaryFile(filename);
      ThrowBinaryException(FileOpenError,"UnableToOpenFile",
        image_info->filename)
    }
  /*
    Allocate raw strip buffer.
  */
  (void) TIFFGetField(tiff,TIFFTAG_STRIPBYTECOUNTS,&byte_count);
  strip_size=byte_count[0];
  for (i=1; i < (long) TIFFNumberOfStrips(tiff); i++)
    if (byte_count[i] > strip_size)
      strip_size=byte_count[i];
  buffer=(unsigned char *) AcquireMemory(strip_size);
  if (buffer == (unsigned char *) NULL)
    {
      TIFFClose(tiff);
      LiberateTemporaryFile(filename);
      ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
        (char *) NULL)
    }
  /*
    Compress runlength encoded to 2D Huffman pixels.
  */
  (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&fillorder);
  for (i=0; i < (long) TIFFNumberOfStrips(tiff); i++)
  {
    Ascii85Initialize(image);
    count=TIFFReadRawStrip(tiff,(uint32) i,buffer,(long) byte_count[i]);
    if (fillorder == FILLORDER_LSB2MSB)
      TIFFReverseBits(buffer,count);
    for (j=0; j < count; j++)
      Ascii85Encode(image,(unsigned long) buffer[j]);
    Ascii85Flush(image);
  }
  LiberateMemory((void **) &buffer);
  TIFFClose(tiff);
  LiberateTemporaryFile(filename);
  return(True);
}
#else
static unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,"TIFFLibraryIsNotAvailable",image->filename)
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P S 3 I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPS3Image adds attributes for the PS3 image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPS3Image method is:
%
%      RegisterPS3Image(void)
%
*/
ModuleExport void RegisterPS3Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPS3");
  entry->encoder=(EncoderHandler) WritePS3Image;
  entry->description=AcquireString("Adobe Level III Encapsulated PostScript");
  entry->module=AcquireString("PS3");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PS3");
  entry->encoder=(EncoderHandler) WritePS3Image;
  entry->description=AcquireString("Adobe Level III PostScript");
  entry->module=AcquireString("PS3");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P S 3 I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPS3Image removes format registrations made by the
%  PS3 module from the list of supported formats.
%
%  The format of the UnregisterPS3Image method is:
%
%      UnregisterPS3Image(void)
%
*/
ModuleExport void UnregisterPS3Image(void)
{
  (void) UnregisterMagickInfo("EPS3");
  (void) UnregisterMagickInfo("PS3");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S 3 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePS3Image translates an image to encapsulated Postscript
%  Level III for printing.  If the supplied geometry is null, the image is
%  centered on the Postscript page.  Otherwise, the image is positioned as
%  specified by the geometry.
%
%  The format of the WritePS3Image method is:
%
%      unsigned int WritePS3Image(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePS3Image return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
static unsigned int WritePS3Image(const ImageInfo *image_info,Image *image)
{
#define CFormat  "currentfile /%.1024s filter\n"

  char
    buffer[MaxTextExtent],
    date[MaxTextExtent],
    density[MaxTextExtent],
    page_geometry[MaxTextExtent];

  CompressionType
    compression;

  const ImageAttribute
    *attribute;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  int
    count,
    status;

  long
    y;

  RectangleInfo
    geometry;

  register const PixelPacket
    *p;

  register long
    x;

  SegmentInfo
    bounds;

  size_t
    length;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned long
    page,
    scene,
    text_size;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  switch (compression)
  {
#if !defined(HasJPEG)
    case JPEGCompression:
    {
      compression=RLECompression;
      ThrowException(&image->exception,MissingDelegateError,
        "JPEGLibraryIsNotAvailable",image->filename);
      break;
    }
#endif
#if !defined(HasLZW)
    case LZWCompression:
    {
      compression=RLECompression;
      ThrowException(&image->exception,MissingDelegateError,
        "LZWEncodingNotEnabled",image->filename);
      break;
    }
#endif
#if !defined(HasZLIB)
    case ZipCompression:
    {
      compression=RLECompression;
      ThrowException(&image->exception,MissingDelegateError,
        "ZipLibraryIsNotAvailable",image->filename);
      break;
    }
#endif
    default:
      break;
  }
  page=1;
  scene=0;
  do
  {
    if ((compression != NoCompression) &&
        (compression != RLECompression))
      image->storage_class=DirectClass;
    if (compression == FaxCompression)
      SetImageType(image,BilevelType);
    /*
      Scale image to size of Postscript page.
    */
    text_size=0;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      text_size=(unsigned int)
        (MultilineCensus(attribute->value)*image_info->pointsize+12);
    SetGeometry(image,&geometry);
    geometry.y=(long) text_size;
    FormatString(page_geometry,"%lux%lu",image->columns,image->rows);
    if (image_info->page != (char *) NULL)
      (void) strncpy(page_geometry,image_info->page,MaxTextExtent-1);
    else
      if ((image->page.width != 0) && (image->page.height != 0))
        (void) FormatString(page_geometry,"%lux%lu%+ld%+ld",image->page.width,
          image->page.height,image->page.x,image->page.y);
      else
        if (LocaleCompare(image_info->magick,"PS3") == 0)
          (void) strcpy(page_geometry,PSPageGeometry);
    (void) GetMagickGeometry(page_geometry,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
    /*
      Scale relative to dots-per-inch.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=sscanf(density,"%lfx%lf",&x_resolution,&y_resolution);
    if (count != 2)
      y_resolution=x_resolution;
    if (image_info->density != (char *) NULL)
      {
        count=sscanf(image_info->density,"%lfx%lf",&x_resolution,
          &y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(geometry.width*dx_resolution)/x_resolution;
    geometry.width=(unsigned long) (x_scale+0.5);
    y_scale=(geometry.height*dy_resolution)/y_resolution;
    geometry.height=(unsigned long) (y_scale+0.5);
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        if (LocaleCompare(image_info->magick,"PS3") == 0)
          (void) strcpy(buffer,"%!PS-Adobe-3.0 Resource-ProcSet\n");
        else
          (void) strcpy(buffer,"%!PS-Adobe-3.0 EPSF-3.0 Resource-ProcSet\n");
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"%%Creator: (GraphicsMagick)\n");
        FormatString(buffer,"%%%%Title: (%.1024s)\n",image->filename);
        (void) WriteBlobString(image,buffer);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strncpy(date,ctime(&timer),MaxTextExtent-1);
        date[strlen(date)-1]='\0';
        FormatString(buffer,"%%%%CreationDate: (%.1024s)\n",date);
        (void) WriteBlobString(image,buffer);
        bounds.x1=geometry.x;
        bounds.y1=geometry.y;
        bounds.x2=geometry.x+geometry.width;
        bounds.y2=geometry.y+geometry.height+text_size;
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) strcpy(buffer,"%%BoundingBox: (atend)\n");
        else
          FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",
            floor(bounds.x1+0.5),floor(bounds.y1+0.5),ceil(bounds.x2-0.5),
            ceil(bounds.y2-0.5));
        (void) WriteBlobString(image,buffer);
        attribute=GetImageAttribute(image,"label");
        if (attribute != (const ImageAttribute *) NULL)
          (void) WriteBlobString(image,
            "%%DocumentNeededResources: font Helvetica\n");
        (void) WriteBlobString(image,"%%LanguageLevel: 3\n");
        if (LocaleCompare(image_info->magick,"PS3") != 0)
          (void) WriteBlobString(image,"%%%%Pages: 1\n");
        else
          {
            (void) WriteBlobString(image,"%%Orientation: Portrait\n");
            (void) WriteBlobString(image,"%%PageOrder: Ascend\n");
            if (!image_info->adjoin)
              (void) strcpy(buffer,"%%Pages: 1\n");
            else
              FormatString(buffer,"%%%%Pages: %lu\n",(unsigned long)
                GetImageListLength(image));
            (void) WriteBlobString(image,buffer);
          }
        (void) WriteBlobString(image,"%%EndComments\n");
      }
    FormatString(buffer,"%%%%Page:  1 %lu\n",page++);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%%%%PageBoundingBox: %ld %ld %ld %ld\n",geometry.x,
      geometry.y,geometry.x+(long) geometry.width,geometry.y+(long)
      (geometry.height+text_size));
    (void) WriteBlobString(image,buffer);
    if (geometry.x < bounds.x1)
      bounds.x1=geometry.x;
    if (geometry.y < bounds.y1)
      bounds.y1=geometry.y;
    if ((geometry.x+(long) geometry.width-1) > bounds.x2)
      bounds.x2=geometry.x+geometry.width-1;
    if ((geometry.y+(long) (geometry.height+text_size)-1) > bounds.y2)
      bounds.y2=geometry.y+geometry.height+text_size-1;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      (void) WriteBlobString(image,"%%PageResources: font Times-Roman\n");
    /*
      Output image data.
    */
    switch (compression)
    {
      case NoCompression: FormatString(buffer,CFormat,"ASCII85Decode"); break;
      case JPEGCompression: FormatString(buffer,CFormat,"DCTDecode"); break;
      case LZWCompression: FormatString(buffer,CFormat,"LZWDecode"); break;
      case ZipCompression: FormatString(buffer,CFormat,"FlateDecode"); break;
      case FaxCompression:
      {
        (void) strcpy(buffer,"currentfile /ASCII85Decode filter\n");
        (void) WriteBlobString(image,buffer);
        FormatString(buffer,
          "<< /K %.1024s /Columns %lu /Rows %lu >> /CCITTFaxDecode filter\n",
          CCITTParam,image->columns,image->rows);
        break;
      }
      default: FormatString(buffer,CFormat,"RunLengthDecode"); break;
    }
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"/ReusableStreamDecode filter\n");
    switch (compression)
    {
      case JPEGCompression:
      {
        Image
          *jpeg_image;

        void
          *blob;

        /*
          Write image in JPEG format.
        */
        jpeg_image=CloneImage(image,0,0,True,&image->exception);
        if (jpeg_image == (Image *) NULL)
          ThrowWriterException(CoderError,image->exception.reason,image);
        (void) strcpy(jpeg_image->magick,"JPEG");
        blob=ImageToBlob(image_info,jpeg_image,&length,&image->exception);
        (void) WriteBlob(image,length,blob);
        DestroyImage(jpeg_image);
        LiberateMemory((void **) &blob);
        break;
      }
      case RLECompression:
      default:
      {
        register unsigned char
          *q;

        unsigned long
          number_pixels;

        /*
          Allocate pixel array.
        */
        number_pixels=image->columns*image->rows;
        length=4*number_pixels;
        pixels=(unsigned char *) AcquireMemory(length);
        if (pixels == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",
            image);
        /*
          Dump Packbit encoded pixels.
        */
        q=pixels;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=ScaleQuantumToChar(MaxRGB-p->opacity);
            *q++=ScaleQuantumToChar(p->red);
            *q++=ScaleQuantumToChar(p->green);
            *q++=ScaleQuantumToChar(p->blue);
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        if (compression == ZipCompression)
          status=ZLIBEncodeImage(image,length,image_info->quality,pixels);
        else
          if (compression == LZWCompression)
            status=LZWEncodeImage(image,length,pixels);
          else
            status=PackbitsEncodeImage(image,length,pixels);
        if (!status)
          {
            CloseBlob(image);
            return(False);
          }
        LiberateMemory((void **) &pixels);
        break;
      }
      case FaxCompression:
      {
        if (LocaleCompare(CCITTParam,"0") == 0)
          (void) HuffmanEncodeImage(image_info,image);
        else
          (void) Huffman2DEncodeImage(image_info,image);
        break;
      }
      case NoCompression:
      {
        /*
          Dump uncompressed DirectColor packets.
        */
        Ascii85Initialize(image);
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            Ascii85Encode(image,ScaleQuantumToChar(MaxRGB-p->opacity));
            Ascii85Encode(image,ScaleQuantumToChar(p->red));
            Ascii85Encode(image,ScaleQuantumToChar(p->green));
            Ascii85Encode(image,ScaleQuantumToChar(p->blue));
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        Ascii85Flush(image);
        break;
      }
    }
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobString(image,"/datastream exch def\n");
    if (image->colorspace != CMYKColorspace)
      (void) WriteBlobString(image,"/DeviceRGB setcolorspace\n");
    else
      (void) WriteBlobString(image,"/DeviceCMYK setcolorspace\n");
    (void) WriteBlobString(image,"/ImageDataDictionary 8 dict def\n");
    (void) WriteBlobString(image,"ImageDataDictionary begin\n");
    (void) WriteBlobString(image,"  /ImageType 1 def\n");
    FormatString(buffer,"  /Width %lu def\n",image->columns);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"  /Height %lu def\n",image->rows);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"  /BitsPerComponent 8 def\n");
    (void) WriteBlobString(image,"  /DataSource datastream def\n");
    (void) WriteBlobString(image,"  /MultipleDataSources false def\n");
    FormatString(buffer,"  /ImageMatrix [ %lu 0 0 %lu neg 0 %lu ] def\n",
      image->columns,image->rows,image->rows);
    (void) WriteBlobString(image,buffer);
    if (image->colorspace != CMYKColorspace)
      (void) WriteBlobString(image,"  /Decode [ 0 1 0 1 0 1 ] def\n");
    else
      (void) WriteBlobString(image,"  /Decode [ 1 0 1 0 1 0 1 0 ] def\n");
    (void) WriteBlobString(image,"end\n");
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobString(image,"/ImageMaskDictionary 8 dict def\n");
    (void) WriteBlobString(image,"ImageMaskDictionary begin\n");
    (void) WriteBlobString(image,"  /ImageType 1 def\n");
    FormatString(buffer,"  /Width %lu def\n",image->columns);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"  /Height %lu def\n",image->rows);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"  /BitsPerComponent 8 def\n");
    (void) WriteBlobString(image,"  /MultipleDataSources false def\n");
    FormatString(buffer,"  /ImageMatrix [ %ld 0 0 %ld neg 0 %ld ] def\n",
      image->columns,image->rows,image->rows);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"  /Decode [ 0 1 ] def\n");
    (void) WriteBlobString(image,"end\n");
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobString(image,"/MaskedImageDictionary 7 dict def\n");
    (void) WriteBlobString(image,"MaskedImageDictionary begin\n");
    (void) WriteBlobString(image,"  /ImageType 3 def\n");
    (void) WriteBlobString(image,"  /InterleaveType 1 def\n");
    (void) WriteBlobString(image,"  /MaskDict ImageMaskDictionary def\n");
    (void) WriteBlobString(image,"  /DataDict ImageDataDictionary def\n");
    (void) WriteBlobString(image,"end\n");
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobString(image,"gsave\n");
    FormatString(buffer,"%ld %ld translate\n",geometry.x,geometry.y);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%g %g scale\n",x_scale,y_scale);
    (void) WriteBlobString(image,buffer);
    if (compression == JPEGCompression)
      (void) WriteBlobString(image,"ImageDataDictionary image\n");
    else
      {
        (void) WriteBlobString(image,
          "ImageMaskDictionary /Decode [ 1 0 ] put\n");
        (void) WriteBlobString(image,"MaskedImageDictionary image\n");
      }
    (void) WriteBlobString(image,"grestore                    \n");
    (void) WriteBlobByte(image,'\n');
    if (LocaleCompare(image_info->magick,"PS3") == 0)
      (void) WriteBlobString(image,"  showpage\n");
    (void) WriteBlobString(image,"%%EndData\n");
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
  if (page > 1)
    {
      FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",floor(bounds.x1+0.5),
        floor(bounds.y1+0.5),ceil(bounds.x2-0.5),ceil(bounds.y2-0.5));
      (void) WriteBlobString(image,buffer);
    }
  (void) WriteBlobString(image,"%%EOF\n");
  CloseBlob(image);
  return(True);
}

#if defined(HasZLIB)
#include "zlib.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Z L I B E n c o d e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ZLIBEncodeImage compresses an image via ZLIB-coding specific to
%  Postscript Level II or Portable Document Format.  To ensure portability, the
%  binary ZLIB bytes are encoded as ASCII base-85.
%
%  The format of the ZLIBEncodeImage method is:
%
%      unsigned int ZLIBEncodeImage(Image *image,const size_t length,
%        const unsigned long quality,unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method ZLIBEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o file: The address of a structure of type FILE.  ZLIB encoded pixels
%      are written to this file.
%
%    o length:  A value that specifies the number of pixels to compress.
%
%    o quality: the compression level (0-100).
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%
*/
static unsigned int ZLIBEncodeImage(Image *image,const size_t length,
  const unsigned long quality,unsigned char *pixels)
{
  int
    status;

  register long
    i;

  unsigned char
    *compressed_pixels;

  unsigned long
    compressed_packets;

  z_stream
    stream;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  compressed_packets=(unsigned long) (1.001*length+12);
  compressed_pixels=(unsigned char *) AcquireMemory(compressed_packets);
  if (compressed_pixels == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      (char *) NULL);
  stream.next_in=pixels;
  stream.avail_in=(unsigned int) length;
  stream.next_out=compressed_pixels;
  stream.avail_out=(unsigned int) compressed_packets;
  stream.zalloc=(alloc_func) NULL;
  stream.zfree=(free_func) NULL;
  stream.opaque=(voidpf) NULL;
  status=deflateInit(&stream,(int) Min(quality/10,9));
  if (status == Z_OK)
    {
      status=deflate(&stream,Z_FINISH);
      if (status == Z_STREAM_END)
        status=deflateEnd(&stream);
      else
        (void) deflateEnd(&stream);
      compressed_packets=stream.total_out;
    }
  if (status)
    ThrowBinaryException(CoderError,"UnableToZipCompressImage",(char *) NULL)
  else
    for (i=0; i < (long) compressed_packets; i++)
      (void) WriteBlobByte(image,compressed_pixels[i]);
  LiberateMemory((void **) &compressed_pixels);
  return(!status);
}
#else
static unsigned int ZLIBEncodeImage(Image *image,const size_t length,
  const unsigned long quality,unsigned char *pixels)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(CoderError,"ZipLibraryIsNotAvailable",image->filename);
  return(False);
}
#endif
