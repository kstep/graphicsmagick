/*
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
#if defined(HasTIFF)
#define CCITTParam  "-1"
#else
#define CCITTParam  "0"
#endif

/*
  Forward declarations.
*/
static unsigned int
  WritePS3Image(const ImageInfo *,Image *);

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
  entry->encoder=WritePS3Image;
  entry->description=AllocateString("Adobe Level III Encapsulated PostScript");
  entry->module=AllocateString("PS3");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PS3");
  entry->encoder=WritePS3Image;
  entry->description=AllocateString("Adobe Level III PostScript");
  entry->module=AllocateString("PS3");
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
%    o image_info: Specifies a pointer to an ImageInfo structure.
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
    geometry[MaxTextExtent];

  CompressionType
    compression;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  ImageAttribute
    *attribute;

  int
    count,
    status,
    x,
    y;

  register PixelPacket
    *p;

  SegmentInfo
    bounds;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned int
    height,
    page,
    scene,
    text_size,
    width;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  switch (compression)
  {
#if !defined(HasJPEG)
    case JPEGCompression:
    {
      compression=RunlengthEncodedCompression;
      ThrowException(&image->exception,MissingDelegateWarning,
        "JPEG compression is not available",image->filename);
      break;
    }
#endif
#if !defined(HasLZW)
    case LZWCompression:
    {
      compression=RunlengthEncodedCompression;
      ThrowException(&image->exception,MissingDelegateWarning,
        "LZW compression is not available",image->filename);
      break;
    }
#endif
#if !defined(HasZLIB)
    case ZipCompression:
    {
      compression=RunlengthEncodedCompression;
      ThrowException(&image->exception,MissingDelegateWarning,
        "ZLIB compression is not available",image->filename);
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
    if ((compression == FaxCompression) && !IsMonochromeImage(image))
      {
        QuantizeInfo
          quantize_info;

        /*
          Convert image to monochrome.
        */
        image=CloneImage(image,0,0,True,&image->exception);
        if (image == (Image *) NULL)
          return(False);
        GetQuantizeInfo(&quantize_info);
        quantize_info.number_colors=2;
        quantize_info.dither=image_info->dither;
        quantize_info.colorspace=GRAYColorspace;
        (void) QuantizeImage(&quantize_info,image);
      }
    if ((compression != NoCompression) &&
        (compression != RunlengthEncodedCompression))
      image->storage_class=DirectClass;
    /*
      Scale image to size of Postscript page.
    */
    text_size=0;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (ImageAttribute *) NULL)
      text_size=(unsigned int)
        (MultilineCensus(attribute->value)*image_info->pointsize+12);
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    FormatString(geometry,"%ux%u",image->columns,image->rows);
    if (image_info->page != (char *) NULL)
      (void) strcpy(geometry,image_info->page);
    else
      if ((image->page.width != 0) && (image->page.height != 0))
        (void) FormatString(geometry,"%ux%u%+d%+d",image->page.width,
          image->page.height,image->page.x,image->page.y);
      else
        if (LocaleCompare(image_info->magick,"PS3") == 0)
          (void) strcpy(geometry,PSPageGeometry);
    (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
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
        count=sscanf(image_info->density,"%lfx%lf",&x_resolution,&y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(width*dx_resolution)/x_resolution;
    width=(unsigned int) ceil(x_scale-0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) ceil(y_scale-0.5);
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        if (LocaleCompare(image_info->magick,"PS3") == 0)
          (void) strcpy(buffer,"%!PS-Adobe-3.0 Resource-ProcSet\n");
        else
          (void) strcpy(buffer,"%!PS-Adobe-3.0 EPSF-3.0 Resource-ProcSet\n");
        (void) WriteBlobString(image,"%%Creator: (ImageMagick)\n");
        FormatString(buffer,"%%%%Title: (%.1024s)\n",image->filename);
        (void) WriteBlobString(image,buffer);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strcpy(date,ctime(&timer));
        date[strlen(date)-1]='\0';
        FormatString(buffer,"%%%%CreationDate: (%.1024s)\n",date);
        (void) WriteBlobString(image,buffer);
        bounds.x1=x;
        bounds.y1=y;
        bounds.x2=x+width;
        bounds.y2=y+(height+text_size);
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) strcpy(buffer,"%%BoundingBox: (atend)\n");
        else
          FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",
            floor(bounds.x1+0.5),floor(bounds.y1+0.5),ceil(bounds.x2-0.5),
            ceil(bounds.y2-0.5));
        (void) WriteBlobString(image,buffer);
        attribute=GetImageAttribute(image,"label");
        if (attribute != (ImageAttribute *) NULL)
          {
            (void) WriteBlobString(image,
              "%%%%DocumentNeededResources: font Helvetica\n");
          }
        (void) WriteBlobString(image,"%%LanguageLevel: 3\n");
        if (LocaleCompare(image_info->magick,"PS3") != 0)
          (void) WriteBlobString(image,"%%%%Pages: 0\n");
        else
          {
            (void) WriteBlobString(image,"%%Orientation: Portrait\n");
            (void) WriteBlobString(image,"%%PageOrder: Ascend\n");
            if (!image_info->adjoin)
              (void) strcpy(buffer,"%%Pages: 0\n");
            else
              FormatString(buffer,"%%%%Pages: %u\n",GetNumberScenes(image));
            (void) WriteBlobString(image,buffer);
          }
        (void) WriteBlobString(image,"%%EndComments\n");
      }
    FormatString(buffer,"%%%%Page:  1 %u\n",page++);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%%%%PageBoundingBox: %d %d %d %d\n",x,y,
      x+(int) width,y+(int) (height+text_size));
    (void) WriteBlobString(image,buffer);
    if (x < bounds.x1)
      bounds.x1=x;
    if (y < bounds.y1)
      bounds.y1=y;
    if ((x+(int) width-1) > bounds.x2)
      bounds.x2=x+width-1;
    if ((y+(int) (height+text_size)-1) > bounds.y2)
      bounds.y2=y+(height+text_size)-1;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (ImageAttribute *) NULL)
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
          "<< /K %.1024s /Columns %d /Rows %d >> /CCITTFaxDecode filter\n",
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
        char
          filename[MaxTextExtent];

        FILE
          *file;

        Image
          *jpeg_image;

        int
          c;

        /*
          Write image to temporary file in JPEG format.
        */
        TemporaryFilename(filename);
        jpeg_image=CloneImage(image,0,0,True,&image->exception);
        if (jpeg_image == (Image *) NULL)
          ThrowWriterException(DelegateWarning,"Unable to clone image",image);
        (void) FormatString(jpeg_image->filename,"jpeg:%.1024s",filename);
        status=WriteImage(image_info,jpeg_image);
        DestroyImage(jpeg_image);
        if (status == False)
          ThrowWriterException(DelegateWarning,"Unable to write image",image);
        file=fopen(filename,ReadBinaryType);
        if (file == (FILE *) NULL)
          ThrowWriterException(FileOpenWarning,"Unable to open file",image);
        for (c=fgetc(file); c != EOF; c=fgetc(file))
          (void) WriteBlobByte(image,c);
        (void) fclose(file);
        (void) remove(filename);
        break;
      }
      case RunlengthEncodedCompression:
      default:
      {
        register unsigned char
          *q;

        size_t
          length;

        unsigned long
          number_pixels;

        /*
          Allocate pixel array.
        */
        number_pixels=image->columns*image->rows;
        length=4*number_pixels;
        pixels=(unsigned char *) AcquireMemory(length);
        if (pixels == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
            image);
        /*
          Dump Packbit encoded pixels.
        */
        q=pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          p=GetImagePixels(image,0,y,image->columns,1);
          if (p == (PixelPacket *) NULL)
            break;
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=MaxRGB-DownScale(p->opacity);
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
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
          (void) HuffmanEncodeImage((ImageInfo *) image_info,image);
        else
          (void) Huffman2DEncodeImage((ImageInfo *) image_info,image);
        break;
      }
      case NoCompression:
      {
        /*
          Dump uncompressed DirectColor packets.
        */
        Ascii85Initialize(image);
        for (y=0; y < (int) image->rows; y++)
        {
          p=GetImagePixels(image,0,y,image->columns,1);
          if (p == (PixelPacket *) NULL)
            break;
          for (x=0; x < (int) image->columns; x++)
          {
            Ascii85Encode(image,(int) (MaxRGB-DownScale(p->opacity)));
            Ascii85Encode(image,DownScale(p->red));
            Ascii85Encode(image,DownScale(p->green));
            Ascii85Encode(image,DownScale(p->blue));
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
        Ascii85Flush(image);
        break;
      }
    }
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobString(image,"/datastream exch def\n");
    (void) WriteBlobString(image,"/DeviceRGB setcolorspace\n");
    (void) WriteBlobString(image,"/ImageDataDictionary 8 dict def\n");
    (void) WriteBlobString(image,"ImageDataDictionary begin\n");
    (void) WriteBlobString(image,"  /ImageType 1 def\n");
    FormatString(buffer,"  /Width %u def\n",image->columns);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"  /Height %u def\n",image->rows);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"  /BitsPerComponent 8 def\n");
    (void) WriteBlobString(image,"  /DataSource datastream def\n");
    (void) WriteBlobString(image,"  /MultipleDataSources false def\n");
    FormatString(buffer,"  /ImageMatrix [ %d 0 0 %d neg 0 %d ] def\n",
      image->columns,image->rows,image->rows);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"  /Decode [ 0 1 0 1 0 1 ] def\n");
    (void) WriteBlobString(image,"end\n");
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobString(image,"/ImageMaskDictionary 8 dict def\n");
    (void) WriteBlobString(image,"ImageMaskDictionary begin\n");
    (void) WriteBlobString(image,"  /ImageType 1 def\n");
    FormatString(buffer,"  /Width %u def\n",image->columns);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"  /Height %u def\n",image->rows);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"  /BitsPerComponent 8 def\n");
    (void) WriteBlobString(image,"  /MultipleDataSources false def\n");
    FormatString(buffer,"  /ImageMatrix [ %d 0 0 %d neg 0 %d ] def\n",
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
    FormatString(buffer,"%d %d translate\n",x,y);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%g %g scale\n",x_scale,y_scale);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"ImageMaskDictionary /Decode [ 1 0 ] put\n");
    (void) WriteBlobString(image,"MaskedImageDictionary image\n");
    (void) WriteBlobString(image,"grestore                    \n");
    (void) WriteBlobByte(image,'\n');
    if (LocaleCompare(image_info->magick,"PS3") == 0)
      (void) WriteBlobString(image,"  showpage\n");
    (void) WriteBlobString(image,"%%EndData\n");
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
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
