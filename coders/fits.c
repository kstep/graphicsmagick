/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        FFFFF  IIIII  TTTTT  SSSSS                           %
%                        F        I      T    SS                              %
%                        FFF      I      T     SSS                            %
%                        F        I      T       SS                           %
%                        F      IIIII    T    SSSSS                           %
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
  WriteFITSImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s F I T S                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsFITS returns True if the image format type, identified by the
%  magick string, is FITS.
%
%  The format of the IsFITS method is:
%
%      unsigned int IsFITS(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsFITS returns True if the image format type is FITS.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsFITS(const unsigned char *magick,const size_t length)
{
  if (length < 6)
    return(False);
  if (LocaleNCompare((char *) magick,"IT0",3) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"SIMPLE",6) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d F I T S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadFITSImage reads a FITS image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadFITSImage method is:
%
%      Image *ReadFITSImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadFITSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadFITSImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  typedef struct _FITSInfo
  {
    int
      simple,
      bits_per_pixel,
      columns,
      rows,
      number_axes,
      number_scenes;

    double
      min_data,
      max_data,
      zero,
      scale;
  } FITSInfo;

  char
    long_quantum[8],
    keyword[MaxTextExtent],
    value[MaxTextExtent];

  double
    pixel,
    scale,
    scaled_pixel;

  FITSInfo
    fits_info;

  Image
    *image;

  IndexPacket
    index;

  int
    c,
    j,
    packet_size;

  long
    quantum,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *p;

  size_t
    count;

  unsigned char
    *fits_pixels;

  unsigned int
    status,
    value_expected;

  unsigned long
    scene,
    number_pixels;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image header.
  */
  fits_info.simple=False;
  fits_info.bits_per_pixel=8;
  fits_info.columns=1;
  fits_info.rows=1;
  fits_info.number_scenes=1;
  fits_info.min_data=0.0;
  fits_info.max_data=0.0;
  fits_info.zero=0.0;
  fits_info.scale=1.0;
  /*
    Decode image header.
  */
  c=ReadBlobByte(image);
  count=1;
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  for ( ; ; )
  {
    if (!isalnum(c))
      {
        c=ReadBlobByte(image);
        count++;
      }
    else
      {
        register char
          *p;

        /*
          Determine a keyword and its value.
        */
        p=keyword;
        do
        {
          if ((p-keyword) < (MaxTextExtent-1))
            *p++=c;
          c=ReadBlobByte(image);
          count++;
        } while (isalnum(c) || (c == '_'));
        *p='\0';
        if (LocaleCompare(keyword,"END") == 0)
          break;
        value_expected=False;
        while (isspace(c) || (c == '='))
        {
          if (c == '=')
            value_expected=True;
          c=ReadBlobByte(image);
          count++;
        }
        if (value_expected == False)
          continue;
        p=value;
        while (isalnum(c) || (c == '-') || (c == '+') || (c == '.'))
        {
          if ((p-value) < (MaxTextExtent-1))
            *p++=c;
          c=ReadBlobByte(image);
          count++;
        }
        *p='\0';
        /*
          Assign a value to the specified keyword.
        */
        if (LocaleCompare(keyword,"SIMPLE") == 0)
          fits_info.simple=(*value == 'T') || (*value == 't');
        if (LocaleCompare(keyword,"BITPIX") == 0)
          fits_info.bits_per_pixel=atoi(value);
        if (LocaleCompare(keyword,"NAXIS") == 0)
          fits_info.number_axes=atoi(value);
        if (LocaleCompare(keyword,"NAXIS1") == 0)
          fits_info.columns= atoi(value);
        if (LocaleCompare(keyword,"NAXIS2") == 0)
          fits_info.rows= atoi(value);
        if (LocaleCompare(keyword,"NAXIS3") == 0)
          fits_info.number_scenes=atoi(value);
        if (LocaleCompare(keyword,"DATAMAX") == 0)
          fits_info.max_data=atof(value);
        if (LocaleCompare(keyword,"DATAMIN") == 0)
          fits_info.min_data=atof(value);
        if (LocaleCompare(keyword,"BZERO") == 0)
          fits_info.zero=atof(value);
        if (LocaleCompare(keyword,"BSCALE") == 0)
          fits_info.scale=atof(value);
      }
    while (isspace(c))
    {
      c=ReadBlobByte(image);
      count++;
    }
  }
  while (count > 2880)
    count-=2880;
  for ( ; count < 2880; count++)
    (void) ReadBlobByte(image);
  /*
    Verify that required image information is defined.
  */
  number_pixels=fits_info.columns*fits_info.rows;
  if ((!fits_info.simple) || (fits_info.number_axes < 1) ||
      (fits_info.number_axes > 4) || (number_pixels == 0))
    ThrowReaderException(CorruptImageWarning,"image type not supported",image);
  for (scene=0; scene < (long) fits_info.number_scenes; scene++)
  {
    /*
      Create linear colormap.
    */
    image->columns=fits_info.columns;
    image->rows=fits_info.rows;
    image->depth=fits_info.bits_per_pixel <= 8 ? 8 : QuantumDepth;
    image->storage_class=PseudoClass;
    image->scene=scene;
    if (!AllocateImageColormap(image,1 << image->depth))
      ThrowReaderException(FileOpenWarning,"Unable to open file",image);
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Initialize image structure.
    */
    packet_size=fits_info.bits_per_pixel/8;
    if (packet_size < 0)
      packet_size=(-packet_size);
    number_pixels=image->columns*image->rows;
    fits_pixels=(unsigned char *) AcquireMemory(packet_size*number_pixels);
    if (fits_pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    /*
      Convert FITS pixels to pixel packets.
    */
    count=ReadBlob(image,packet_size*number_pixels,fits_pixels);
    if (count == 0)
      ThrowReaderException(CorruptImageWarning,
        "Insufficient image data in file",image);
    if ((fits_info.min_data == 0.0) && (fits_info.max_data == 0.0))
      {
        /*
          Determine minimum and maximum intensity.
        */
        p=fits_pixels;
        long_quantum[0]=(*p);
        quantum=(*p++);
        for (j=0; j < (packet_size-1); j++)
        {
          long_quantum[j+1]=(*p);
          quantum=(quantum << 8) | (*p++);
        }
        pixel=(double) quantum;
        if (fits_info.bits_per_pixel == 16)
          if (pixel > 32767)
            pixel-=65536;
        if (fits_info.bits_per_pixel == -32)
          pixel=(double) (*((float *) &quantum));
        if (fits_info.bits_per_pixel == -64)
          pixel=(double) (*((double *) long_quantum));
        fits_info.min_data=pixel*fits_info.scale-fits_info.zero;
        fits_info.max_data=pixel*fits_info.scale-fits_info.zero;
        for (i=1; i < (long) number_pixels; i++)
        {
          long_quantum[0]=(*p);
          quantum=(*p++);
          for (j=0; j < (packet_size-1); j++)
          {
            long_quantum[j+1]=(*p);
            quantum=(quantum << 8) | (*p++);
          }
          pixel=(double) quantum;
          if (fits_info.bits_per_pixel == 16)
            if (pixel > 32767)
              pixel-=65536;
          if (fits_info.bits_per_pixel == -32)
            pixel=(double) (*((float *) &quantum));
          if (fits_info.bits_per_pixel == -64)
            pixel=(double) (*((double *) long_quantum));
          scaled_pixel=pixel*fits_info.scale-fits_info.zero;
          if (scaled_pixel < fits_info.min_data)
            fits_info.min_data=scaled_pixel;
          if (scaled_pixel > fits_info.max_data)
            fits_info.max_data=scaled_pixel;
        }
      }
    /*
      Convert FITS pixels to pixel packets.
    */
    scale=1.0;
    if (((fits_info.max_data-fits_info.min_data) <= 1.0) ||
        ((fits_info.max_data-fits_info.min_data) > ((1 << image->depth)-1)))
      scale=((1 << image->depth)-1)/(fits_info.max_data-fits_info.min_data);
    p=fits_pixels;
    for (y=image->rows-1; y >= 0; y--)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      for (x=0; x < (long) image->columns; x++)
      {
        long_quantum[0]=(*p);
        quantum=(*p++);
        for (j=0; j < (packet_size-1); j++)
        {
          long_quantum[j+1]=(*p);
          quantum=(quantum << 8) | (*p++);
        }
        pixel=(double) quantum;
        if (fits_info.bits_per_pixel == 16)
          if (pixel > 32767)
            pixel-=65536;
        if (fits_info.bits_per_pixel == -32)
          pixel=(double) (*((float *) &quantum));
        if (fits_info.bits_per_pixel == -64)
          pixel=(double) (*((double *) long_quantum));
        scaled_pixel=scale*
          (pixel*fits_info.scale-fits_info.min_data-fits_info.zero);
        index=(IndexPacket) ((scaled_pixel < 0) ? 0 :
          (scaled_pixel > ((1 << image->depth)-1)) ? ((1 << image->depth)-1) :
          scaled_pixel+0.5);
        index=ValidateColormapIndex(image,index);
        indexes[x]=index;
        *q++=image->colormap[index];
      }
      if (!SyncImagePixels(image))
        break;
      if (QuantumTick(y,image->rows))
        MagickMonitor(LoadImageText,y,image->rows);
    }
    LiberateMemory((void **) &fits_pixels);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (scene < (long) (fits_info.number_scenes-1))
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image=image->next;
        MagickMonitor(LoadImagesText,TellBlob(image),SizeBlob(image));
      }
  }
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
%   R e g i s t e r F I T S I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterFITSImage adds attributes for the FITS image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterFITSImage method is:
%
%      RegisterFITSImage(void)
%
*/
ModuleExport void RegisterFITSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("FITS");
  entry->decoder=ReadFITSImage;
  entry->encoder=WriteFITSImage;
  entry->magick=IsFITS;
  entry->adjoin=False;
  entry->description=AllocateString("Flexible Image Transport System");
  entry->module=AllocateString("FITS");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r F I T S I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterFITSImage removes format registrations made by the
%  FITS module from the list of supported formats.
%
%  The format of the UnregisterFITSImage method is:
%
%      UnregisterFITSImage(void)
%
*/
ModuleExport void UnregisterFITSImage(void)
{
  (void) UnregisterMagickInfo("FITS");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F I T S I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteFITSImage writes a Flexible Image Transport System image to a
%  file as gray scale intensities [0..255].
%
%  The format of the WriteFITSImage method is:
%
%      unsigned int WriteFITSImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteFITSImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteFITSImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[81],
    *fits_info;

  long
    y;

  register const PixelPacket
    *p;

  register int
    i;

  unsigned char
    *pixels;

  unsigned int
    packet_size,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) TransformRGBImage(image,RGBColorspace);
  /*
    Allocate image memory.
  */
  packet_size=image->depth > 8 ? 2 : 1;
  fits_info=(char *) AcquireMemory(2880);
  pixels=(unsigned char *) AcquireMemory(packet_size*image->columns);
  if ((fits_info == (char *) NULL) || (pixels == (unsigned char *) NULL))
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Initialize image header.
  */
  for (i=0; i < 2880; i++)
    fits_info[i]=' ';
  (void) strcpy(buffer,"SIMPLE  =                    T");
  (void) strncpy(fits_info+0,buffer,strlen(buffer));
  FormatString(buffer,"BITPIX  =                    %d",image->depth);
  (void) strncpy(fits_info+80,buffer,strlen(buffer));
  (void) strcpy(buffer,"NAXIS   =                    2");
  (void) strncpy(fits_info+160,buffer,strlen(buffer));
  FormatString(buffer,"NAXIS1  =           %10u",image->columns);
  (void) strncpy(fits_info+240,buffer,strlen(buffer));
  FormatString(buffer,"NAXIS2  =           %10u",image->rows);
  (void) strncpy(fits_info+320,buffer,strlen(buffer));
  FormatString(buffer,"DATAMIN =           %10u",0);
  (void) strncpy(fits_info+400,buffer,strlen(buffer));
  FormatString(buffer,"DATAMAX =           %10u",(1 << image->depth)-1);
  (void) strncpy(fits_info+480,buffer,strlen(buffer));
  (void) strcpy(buffer,"HISTORY Created by ImageMagick.");
  (void) strncpy(fits_info+560,buffer,strlen(buffer));
  (void) strcpy(buffer,"END");
  (void) strncpy(fits_info+640,buffer,strlen(buffer));
  (void) WriteBlob(image,2880,(char *) fits_info);
  LiberateMemory((void **) &fits_info);
  /*
    Convert image to fits scale PseudoColor class.
  */
  for (y=image->rows-1; y >= 0; y--)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    (void) PopImagePixels(image,GrayQuantum,pixels);
    (void) WriteBlob(image,packet_size*image->columns,pixels);
    if (QuantumTick(image->rows-y-1,image->rows))
      MagickMonitor(SaveImageText,image->rows-y-1,image->rows);
  }
  LiberateMemory((void **) &pixels);
  CloseBlob(image);
  return(True);
}
