/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        V   V  IIIII  FFFFF  FFFFF                           %
%                        V   V    I    F      F                               %
%                        V   V    I    FFF    FFF                             %
%                         V V     I    F      F                               %
%                          V    IIIII  F      F                               %
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
  WriteVIFFImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s V I F F                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsVIFF returns True if the image format type, identified by the
%  magick string, is VIFF.
%
%  The format of the IsVIFF method is:
%
%      unsigned int IsVIFF(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsVIFF returns True if the image format type is VIFF.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsVIFF(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if (memcmp(magick,"\253\001",2) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I F F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadVIFFImage reads a Khoros Visualization image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadVIFFImage method is:
%
%      Image *ReadVIFFImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadVIFFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadVIFFImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define VFF_CM_genericRGB  15
#define VFF_CM_ntscRGB  1
#define VFF_CM_NONE  0
#define VFF_DEP_DECORDER  0x4
#define VFF_DEP_NSORDER  0x8
#define VFF_DES_RAW  0
#define VFF_LOC_IMPLICIT  1
#define VFF_MAPTYP_NONE  0
#define VFF_MAPTYP_1_BYTE  1
#define VFF_MAPTYP_2_BYTE  2
#define VFF_MAPTYP_4_BYTE  4
#define VFF_MAPTYP_FLOAT  5
#define VFF_MAPTYP_DOUBLE  7
#define VFF_MS_NONE  0
#define VFF_MS_ONEPERBAND  1
#define VFF_MS_SHARED  3
#define VFF_TYP_BIT  0
#define VFF_TYP_1_BYTE  1
#define VFF_TYP_2_BYTE  2
#define VFF_TYP_4_BYTE  4
#define VFF_TYP_FLOAT  5
#define VFF_TYP_DOUBLE  9

  typedef struct _ViffInfo
  {
    unsigned char
      identifier,
      file_type,
      release,
      version,
      machine_dependency,
      reserve[3];

    char
      comment[512];

    unsigned long
      rows,
      columns,
      subrows;

    long
      x_offset,
      y_offset;

    float
      x_bits_per_pixel,
      y_bits_per_pixel;

    unsigned long
      location_type,
      location_dimension,
      number_of_images,
      number_data_bands,
      data_storage_type,
      data_encode_scheme,
      map_scheme,
      map_storage_type,
      map_rows,
      map_columns,
      map_subrows,
      map_enable,
      maps_per_cycle,
      color_space_model;
  } ViffInfo;

  double
    min_value,
    scale_factor,
    value;

  Image
    *image;

  int
    bit;

  long
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
    buffer[7],
    *viff_pixels;

  unsigned int
    bytes_per_pixel,
    status;

  unsigned long
    lsb_first,
    max_packets,
    number_pixels;

  ViffInfo
    viff_info;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read VIFF header (1024 bytes).
  */
  count=ReadBlob(image,1,(char *) &viff_info.identifier);
  do
  {
    /*
      Verify VIFF identifier.
    */
    if ((count == 0) || ((unsigned char) viff_info.identifier != 0xab))
      ThrowReaderException(CorruptImageWarning,"Not a VIFF raster",image);
    /*
      Initialize VIFF image.
    */
    (void) ReadBlob(image,7,(char *) buffer);
    viff_info.file_type=buffer[0];
    viff_info.release=buffer[1];
    viff_info.version=buffer[2];
    viff_info.machine_dependency=buffer[3];
    (void) ReadBlob(image,512,(char *) viff_info.comment);
    viff_info.comment[511]='\0';
    if (strlen(viff_info.comment) > 4)
      (void) SetImageAttribute(image,"comment",viff_info.comment);
    if ((viff_info.machine_dependency == VFF_DEP_DECORDER) ||
        (viff_info.machine_dependency == VFF_DEP_NSORDER))
      {
        viff_info.rows=ReadBlobLSBLong(image);
        viff_info.columns=ReadBlobLSBLong(image);
        viff_info.subrows=ReadBlobLSBLong(image);
        viff_info.x_offset=ReadBlobLSBLong(image);
        viff_info.y_offset=ReadBlobLSBLong(image);
        viff_info.x_bits_per_pixel=(float) ReadBlobLSBLong(image);
        viff_info.y_bits_per_pixel=(float) ReadBlobLSBLong(image);
        viff_info.location_type=ReadBlobLSBLong(image);
        viff_info.location_dimension=ReadBlobLSBLong(image);
        viff_info.number_of_images=ReadBlobLSBLong(image);
        viff_info.number_data_bands=ReadBlobLSBLong(image);
        viff_info.data_storage_type=ReadBlobLSBLong(image);
        viff_info.data_encode_scheme=ReadBlobLSBLong(image);
        viff_info.map_scheme=ReadBlobLSBLong(image);
        viff_info.map_storage_type=ReadBlobLSBLong(image);
        viff_info.map_rows=ReadBlobLSBLong(image);
        viff_info.map_columns=ReadBlobLSBLong(image);
        viff_info.map_subrows=ReadBlobLSBLong(image);
        viff_info.map_enable=ReadBlobLSBLong(image);
        viff_info.maps_per_cycle=ReadBlobLSBLong(image);
        viff_info.color_space_model=ReadBlobLSBLong(image);
      }
    else
      {
        viff_info.rows=ReadBlobMSBLong(image);
        viff_info.columns=ReadBlobMSBLong(image);
        viff_info.subrows=ReadBlobMSBLong(image);
        viff_info.x_offset=ReadBlobMSBLong(image);
        viff_info.y_offset=ReadBlobMSBLong(image);
        viff_info.x_bits_per_pixel=(float) ReadBlobMSBLong(image);
        viff_info.y_bits_per_pixel=(float) ReadBlobMSBLong(image);
        viff_info.location_type=ReadBlobMSBLong(image);
        viff_info.location_dimension=ReadBlobMSBLong(image);
        viff_info.number_of_images=ReadBlobMSBLong(image);
        viff_info.number_data_bands=ReadBlobMSBLong(image);
        viff_info.data_storage_type=ReadBlobMSBLong(image);
        viff_info.data_encode_scheme=ReadBlobMSBLong(image);
        viff_info.map_scheme=ReadBlobMSBLong(image);
        viff_info.map_storage_type=ReadBlobMSBLong(image);
        viff_info.map_rows=ReadBlobMSBLong(image);
        viff_info.map_columns=ReadBlobMSBLong(image);
        viff_info.map_subrows=ReadBlobMSBLong(image);
        viff_info.map_enable=ReadBlobMSBLong(image);
        viff_info.maps_per_cycle=ReadBlobMSBLong(image);
        viff_info.color_space_model=ReadBlobMSBLong(image);
      }
    for (i=0; i < 420; i++)
      (void) ReadBlobByte(image);
    image->columns= viff_info.rows;
    image->rows= viff_info.columns;
    image->depth=viff_info.x_bits_per_pixel <= 8 ? 8 : QuantumDepth;
    /*
      Verify that we can read this VIFF image.
    */
    number_pixels=viff_info.columns*viff_info.rows;
    if (number_pixels == 0)
      ThrowReaderException(CorruptImageWarning,
        "Image column or row size is not supported",image);
    if ((viff_info.data_storage_type != VFF_TYP_BIT) &&
        (viff_info.data_storage_type != VFF_TYP_1_BYTE) &&
        (viff_info.data_storage_type != VFF_TYP_2_BYTE) &&
        (viff_info.data_storage_type != VFF_TYP_4_BYTE) &&
        (viff_info.data_storage_type != VFF_TYP_FLOAT) &&
        (viff_info.data_storage_type != VFF_TYP_DOUBLE))
      ThrowReaderException(CorruptImageWarning,
        "Data storage type is not supported",image);
    if (viff_info.data_encode_scheme != VFF_DES_RAW)
      ThrowReaderException(CorruptImageWarning,
        "Data encoding scheme is not supported",image);
    if ((viff_info.map_storage_type != VFF_MAPTYP_NONE) &&
        (viff_info.map_storage_type != VFF_MAPTYP_1_BYTE) &&
        (viff_info.map_storage_type != VFF_MAPTYP_2_BYTE) &&
        (viff_info.map_storage_type != VFF_MAPTYP_4_BYTE) &&
        (viff_info.map_storage_type != VFF_MAPTYP_FLOAT) &&
        (viff_info.map_storage_type != VFF_MAPTYP_DOUBLE))
      ThrowReaderException(CorruptImageWarning,
        "Map storage type is not supported",image);
    if ((viff_info.color_space_model != VFF_CM_NONE) &&
        (viff_info.color_space_model != VFF_CM_ntscRGB) &&
        (viff_info.color_space_model != VFF_CM_genericRGB))
      ThrowReaderException(CorruptImageWarning,
        "Colorspace model is not supported",image);
    if (viff_info.location_type != VFF_LOC_IMPLICIT)
      ThrowReaderException(CorruptImageWarning,
        "Location type is not supported",image);
    if (viff_info.number_of_images != 1)
      ThrowReaderException(CorruptImageWarning,
        "Number of images is not supported",image);
    if (viff_info.map_rows == 0)
      viff_info.map_scheme=VFF_MS_NONE;
    switch (viff_info.map_scheme)
    {
      case VFF_MS_NONE:
      {
        if (viff_info.number_data_bands < 3)
          {
            /*
              Create linear color ramp.
            */
            if (viff_info.data_storage_type == VFF_TYP_BIT)
              image->colors=2;
            else
              image->colors=1 << (viff_info.number_data_bands*QuantumDepth);
            if (!AllocateImageColormap(image,image->colors))
              ThrowReaderException(ResourceLimitWarning,
                "Memory allocation failed",image);
          }
        break;
      }
      case VFF_MS_ONEPERBAND:
      case VFF_MS_SHARED:
      {
        unsigned char
          *viff_colormap;

        /*
          Allocate VIFF colormap.
        */
        switch (viff_info.map_storage_type)
        {
          case VFF_MAPTYP_1_BYTE: bytes_per_pixel=1; break;
          case VFF_MAPTYP_2_BYTE: bytes_per_pixel=2; break;
          case VFF_MAPTYP_4_BYTE: bytes_per_pixel=4; break;
          case VFF_MAPTYP_FLOAT: bytes_per_pixel=4; break;
          case VFF_MAPTYP_DOUBLE: bytes_per_pixel=8; break;
          default: bytes_per_pixel=1; break;
        }
        image->colors=viff_info.map_columns;
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        viff_colormap=(unsigned char *)
          AcquireMemory(bytes_per_pixel*image->colors*viff_info.map_rows);
        if (viff_colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        /*
          Read VIFF raster colormap.
        */
        (void) ReadBlob(image,bytes_per_pixel*
          image->colors*viff_info.map_rows,(char *) viff_colormap);
        lsb_first=1;
        if (*(char *) &lsb_first &&
            ((viff_info.machine_dependency != VFF_DEP_DECORDER) &&
             (viff_info.machine_dependency != VFF_DEP_NSORDER)))
          switch (viff_info.map_storage_type)
          {
            case VFF_MAPTYP_2_BYTE:
            {
              MSBOrderShort((char *) viff_colormap,(bytes_per_pixel*
                image->colors*viff_info.map_rows));
              break;
            }
            case VFF_MAPTYP_4_BYTE:
            case VFF_MAPTYP_FLOAT:
            {
              MSBOrderLong((char *) viff_colormap,(bytes_per_pixel*
                image->colors*viff_info.map_rows));
              break;
            }
            default: break;
          }
        for (i=0; i < (long) (viff_info.map_rows*image->colors); i++)
        {
          switch (viff_info.map_storage_type)
          {
            case VFF_MAPTYP_2_BYTE: value=((short *) viff_colormap)[i]; break;
            case VFF_MAPTYP_4_BYTE: value=((int *) viff_colormap)[i]; break;
            case VFF_MAPTYP_FLOAT: value=((float *) viff_colormap)[i]; break;
            case VFF_MAPTYP_DOUBLE: value=((double *) viff_colormap)[i]; break;
            default: value=viff_colormap[i]; break;
          }
          if (i < (long) image->colors)
            {
              image->colormap[i].red=UpScale((unsigned int) value);
              image->colormap[i].green=UpScale((unsigned int) value);
              image->colormap[i].blue=UpScale((unsigned int) value);
            }
          else
            if (i < (long) (2*image->colors))
              image->colormap[i % image->colors].green=
                UpScale((unsigned int) value);
            else
              if (i < (long) (3*image->colors))
                image->colormap[i % image->colors].blue=
                  UpScale((unsigned int) value);
        }
        LiberateMemory((void **) &viff_colormap);
        break;
      }
      default:
        ThrowReaderException(CorruptImageWarning,
          "Colormap type is not supported",image)
    }
    /*
      Allocate VIFF pixels.
    */
    switch (viff_info.data_storage_type)
    {
      case VFF_TYP_2_BYTE: bytes_per_pixel=2; break;
      case VFF_TYP_4_BYTE: bytes_per_pixel=4; break;
      case VFF_TYP_FLOAT: bytes_per_pixel=4; break;
      case VFF_TYP_DOUBLE: bytes_per_pixel=8; break;
      default: bytes_per_pixel=1; break;
    }
    if (viff_info.data_storage_type == VFF_TYP_BIT)
      max_packets=((image->columns+7) >> 3)*image->rows;
    else
      max_packets=number_pixels*viff_info.number_data_bands;
    viff_pixels=(unsigned char *)
      AcquireMemory(bytes_per_pixel*max_packets*sizeof(Quantum));
    if (viff_pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    (void) ReadBlob(image,bytes_per_pixel*max_packets,(char *) viff_pixels);
    lsb_first=1;
    if (*(char *) &lsb_first &&
        ((viff_info.machine_dependency != VFF_DEP_DECORDER) &&
         (viff_info.machine_dependency != VFF_DEP_NSORDER)))
      switch (viff_info.data_storage_type)
      {
        case VFF_TYP_2_BYTE:
        {
          MSBOrderShort((char *) viff_pixels,bytes_per_pixel*max_packets);
          break;
        }
        case VFF_TYP_4_BYTE:
        case VFF_TYP_FLOAT:
        {
          MSBOrderLong((char *) viff_pixels,bytes_per_pixel*max_packets);
          break;
        }
        default: break;
      }
    min_value=0.0;
    scale_factor=1.0;
    if ((viff_info.data_storage_type != VFF_TYP_1_BYTE) &&
        (viff_info.map_scheme == VFF_MS_NONE))
      {
        double
          max_value;

        /*
          Determine scale factor.
        */
        switch (viff_info.data_storage_type)
        {
          case VFF_TYP_2_BYTE: value=((short *) viff_pixels)[0]; break;
          case VFF_TYP_4_BYTE: value=((int *) viff_pixels)[0]; break;
          case VFF_TYP_FLOAT: value=((float *) viff_pixels)[0]; break;
          case VFF_TYP_DOUBLE: value=((double *) viff_pixels)[0]; break;
          default: value=viff_pixels[0]; break;
        }
        max_value=value;
        min_value=value;
        for (i=0; i < (long) max_packets; i++)
        {
          switch (viff_info.data_storage_type)
          {
            case VFF_TYP_2_BYTE: value=((short *) viff_pixels)[i]; break;
            case VFF_TYP_4_BYTE: value=((int *) viff_pixels)[i]; break;
            case VFF_TYP_FLOAT: value=((float *) viff_pixels)[i]; break;
            case VFF_TYP_DOUBLE: value=((double *) viff_pixels)[i]; break;
            default: value=viff_pixels[i]; break;
          }
          if (value > max_value)
            max_value=value;
          else
            if (value < min_value)
              min_value=value;
        }
        if ((min_value == 0) && (max_value == 0))
          scale_factor=0;
        else
          if (min_value == max_value)
            {
              scale_factor=(double) MaxRGB/min_value;
              min_value=0;
            }
          else
            scale_factor=(double) MaxRGB/(max_value-min_value);
      }
    /*
      Convert pixels to Quantum size.
    */
    p=(unsigned char *) viff_pixels;
    for (i=0; i < (long) max_packets; i++)
    {
      switch (viff_info.data_storage_type)
      {
        case VFF_TYP_2_BYTE: value=((short *) viff_pixels)[i]; break;
        case VFF_TYP_4_BYTE: value=((int *) viff_pixels)[i]; break;
        case VFF_TYP_FLOAT: value=((float *) viff_pixels)[i]; break;
        case VFF_TYP_DOUBLE: value=((double *) viff_pixels)[i]; break;
        default: value=viff_pixels[i]; break;
      }
      if (viff_info.map_scheme == VFF_MS_NONE)
        {
          value=(value-min_value)*scale_factor;
          if (value > MaxRGB)
            value=MaxRGB;
          else
            if (value < 0)
              value=0;
        }
      *p=(Quantum) value;
      p++;
    }
    /*
      Initialize image structure.
    */
    image->matte=(viff_info.number_data_bands == 4);
    image->storage_class=
      (viff_info.number_data_bands < 3 ? PseudoClass : DirectClass);
    image->columns= viff_info.rows;
    image->rows= viff_info.columns;
    /*
      Convert VIFF raster image to pixel packets.
    */
    p=(unsigned char *) viff_pixels;
    if (viff_info.data_storage_type == VFF_TYP_BIT)
      {
        unsigned int
          polarity;

        /*
          Convert bitmap scanline.
        */
        polarity=0;
        if (image->colors >= 2)
          polarity=Intensity(image->colormap[0]) <
            Intensity(image->colormap[1]);
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (long) (image->columns-7); x+=8)
          {
            for (bit=0; bit < 8; bit++)
              indexes[x+bit]=(IndexPacket)
                ((*p) & (0x01 << bit) ? !polarity : polarity);
            p++;
          }
          if ((image->columns % 8) != 0)
            {
              for (bit=0; bit < (long) (image->columns % 8); bit++)
                indexes[x+bit]=(IndexPacket)
                  ((*p) & (0x01 << bit) ? !polarity : polarity);
              p++;
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
      }
    else
      if (image->storage_class == PseudoClass)
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (long) image->columns; x++)
            indexes[x]=(*p++);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
      else
        {
          /*
            Convert DirectColor scanline.
          */
          number_pixels=image->columns*image->rows;
          for (y=0; y < (long) image->rows; y++)
          {
            q=SetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              q->red=UpScale(*p);
              q->green=UpScale(*(p+number_pixels));
              q->blue=UpScale(*(p+2*number_pixels));
              if (image->colors != 0)
                {
                  q->red=image->colormap[q->red].red;
                  q->green=image->colormap[q->green].green;
                  q->blue=image->colormap[q->blue].blue;
                }
              q->opacity=UpScale(image->matte ?
                MaxRGB-(*(p+number_pixels*3)) : OpaqueOpacity);
              p++;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                MagickMonitor(LoadImageText,y,image->rows);
          }
        }
    LiberateMemory((void **) &viff_pixels);
    if (image->storage_class == PseudoClass)
      SyncImage(image);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadBlob(image,1,(char *) &viff_info.identifier);
    if ((count != 0) && (viff_info.identifier == 0xab))
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
  } while ((count != 0) && (viff_info.identifier == 0xab));
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
%   R e g i s t e r V I F F I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterVIFFImage adds attributes for the VIFF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterVIFFImage method is:
%
%      RegisterVIFFImage(void)
%
*/
ModuleExport void RegisterVIFFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("VIFF");
  entry->decoder=ReadVIFFImage;
  entry->encoder=WriteVIFFImage;
  entry->magick=IsVIFF;
  entry->description=AllocateString("Khoros Visualization image");
  entry->module=AllocateString("VIFF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("XV");
  entry->decoder=ReadVIFFImage;
  entry->encoder=WriteVIFFImage;
  entry->description=AllocateString("Khoros Visualization image");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r V I F F I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterVIFFImage removes format registrations made by the
%  VIFF module from the list of supported formats.
%
%  The format of the UnregisterVIFFImage method is:
%
%      UnregisterVIFFImage(void)
%
*/
ModuleExport void UnregisterVIFFImage(void)
{
  (void) UnregisterMagickInfo("VIFF");
  (void) UnregisterMagickInfo("XV");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e V I F F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteVIFFImage writes an image to a file in the VIFF image format.
%
%  The format of the WriteVIFFImage method is:
%
%      unsigned int WriteVIFFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteVIFFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteVIFFImage(const ImageInfo *image_info,Image *image)
{
#define VFF_CM_genericRGB  15
#define VFF_CM_NONE  0
#define VFF_DEP_IEEEORDER  0x2
#define VFF_DES_RAW  0
#define VFF_LOC_IMPLICIT  1
#define VFF_MAPTYP_NONE  0
#define VFF_MAPTYP_1_BYTE  1
#define VFF_MS_NONE  0
#define VFF_MS_ONEPERBAND  1
#define VFF_TYP_BIT  0
#define VFF_TYP_1_BYTE  1

  typedef struct _ViffInfo
  {
    char
      identifier,
      file_type,
      release,
      version,
      machine_dependency,
      reserve[3],
      comment[512];

    unsigned long
      rows,
      columns,
      subrows;

    long
      x_offset,
      y_offset;

    unsigned int
      x_bits_per_pixel,
      y_bits_per_pixel;

    unsigned long
      location_type,
      location_dimension,
      number_of_images,
      number_data_bands,
      data_storage_type,
      data_encode_scheme,
      map_scheme,
      map_storage_type,
      map_rows,
      map_columns,
      map_subrows,
      map_enable,
      maps_per_cycle,
      color_space_model;
  } ViffInfo;

  ImageAttribute
    *attribute;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *p;

  register long
    i;

  register unsigned char
    *q;

  unsigned char
    buffer[8],
    *viff_pixels;

  unsigned int
    scene,
    status;

  unsigned long
    number_pixels,
    packets;

  ViffInfo
    viff_info;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize VIFF image structure.
    */
    (void) TransformRGBImage(image,RGBColorspace);
    viff_info.identifier=(char) 0xab;
    viff_info.file_type=1;
    viff_info.release=1;
    viff_info.version=3;
    viff_info.machine_dependency=VFF_DEP_IEEEORDER;  /* IEEE byte ordering */
    *viff_info.comment='\0';
    attribute=GetImageAttribute(image,"comment");
    if (attribute != (ImageAttribute *) NULL)
      {
        (void) strncpy(viff_info.comment,attribute->value,
          Min(strlen(attribute->value),511));
        viff_info.comment[Min(strlen(attribute->value),511)]='\0';
      }
    viff_info.rows=image->columns;
    viff_info.columns=image->rows;
    viff_info.subrows=0;
    viff_info.x_offset=(~0);
    viff_info.y_offset=(~0);
    viff_info.x_bits_per_pixel=0;
    viff_info.y_bits_per_pixel=0;
    viff_info.location_type=VFF_LOC_IMPLICIT;
    viff_info.location_dimension=0;
    viff_info.number_of_images=1;
    viff_info.data_encode_scheme=VFF_DES_RAW;
    viff_info.map_scheme=VFF_MS_NONE;
    viff_info.map_storage_type=VFF_MAPTYP_NONE;
    viff_info.map_rows=0;
    viff_info.map_columns=0;
    viff_info.map_subrows=0;
    viff_info.map_enable=1;  /* no colormap */
    viff_info.maps_per_cycle=0;
    number_pixels=image->columns*image->rows;
    if (image->storage_class == DirectClass)
      {
        /*
          Full color VIFF raster.
        */
        viff_info.number_data_bands=image->matte ? 4 : 3;
        viff_info.color_space_model=VFF_CM_genericRGB;
        viff_info.data_storage_type=VFF_TYP_1_BYTE;
        packets=viff_info.number_data_bands*number_pixels;
      }
    else
      {
        viff_info.number_data_bands=1;
        viff_info.color_space_model=VFF_CM_NONE;
        viff_info.data_storage_type=VFF_TYP_1_BYTE;
        packets=number_pixels;
        if (!IsGrayImage(image))
          {
            /*
              Colormapped VIFF raster.
            */
            viff_info.map_scheme=VFF_MS_ONEPERBAND;
            viff_info.map_storage_type=VFF_MAPTYP_1_BYTE;
            viff_info.map_rows=3;
            viff_info.map_columns=image->colors;
          }
        else
          if (image->colors == 2)
            {
              /*
                Monochrome VIFF raster.
              */
              viff_info.data_storage_type=VFF_TYP_BIT;
              packets=((image->columns+7) >> 3)*image->rows;
            }
      }
    /*
      Write VIFF image header (pad to 1024 bytes).
    */
    buffer[0]=viff_info.identifier;
    buffer[1]=viff_info.file_type;
    buffer[2]=viff_info.release;
    buffer[3]=viff_info.version;
    buffer[4]=viff_info.machine_dependency;
    buffer[5]=viff_info.reserve[0];
    buffer[6]=viff_info.reserve[1];
    buffer[7]=viff_info.reserve[2];
    (void) WriteBlob(image,8,(char *) buffer);
    (void) WriteBlob(image,512,(char *) viff_info.comment);
    (void) WriteBlobMSBLong(image,viff_info.rows);
    (void) WriteBlobMSBLong(image,viff_info.columns);
    (void) WriteBlobMSBLong(image,viff_info.subrows);
    (void) WriteBlobMSBLong(image,(unsigned long) viff_info.x_offset);
    (void) WriteBlobMSBLong(image,(unsigned long) viff_info.y_offset);
    viff_info.x_bits_per_pixel=(63 << 24) | (128 << 16);
    (void) WriteBlobMSBLong(image,(unsigned long) viff_info.x_bits_per_pixel);
    viff_info.y_bits_per_pixel=(63 << 24) | (128 << 16);
    (void) WriteBlobMSBLong(image,(unsigned long) viff_info.y_bits_per_pixel);
    (void) WriteBlobMSBLong(image,viff_info.location_type);
    (void) WriteBlobMSBLong(image,viff_info.location_dimension);
    (void) WriteBlobMSBLong(image,viff_info.number_of_images);
    (void) WriteBlobMSBLong(image,viff_info.number_data_bands);
    (void) WriteBlobMSBLong(image,viff_info.data_storage_type);
    (void) WriteBlobMSBLong(image,viff_info.data_encode_scheme);
    (void) WriteBlobMSBLong(image,viff_info.map_scheme);
    (void) WriteBlobMSBLong(image,viff_info.map_storage_type);
    (void) WriteBlobMSBLong(image,viff_info.map_rows);
    (void) WriteBlobMSBLong(image,viff_info.map_columns);
    (void) WriteBlobMSBLong(image,viff_info.map_subrows);
    (void) WriteBlobMSBLong(image,viff_info.map_enable);
    (void) WriteBlobMSBLong(image,viff_info.maps_per_cycle);
    (void) WriteBlobMSBLong(image,viff_info.color_space_model);
    for (i=0; i < 420; i++)
      (void) WriteBlobByte(image,'\0');
    /*
      Convert MIFF to VIFF raster pixels.
    */
    viff_pixels=(unsigned char *) AcquireMemory(packets);
    if (viff_pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    q=viff_pixels;
    if (image->storage_class == DirectClass)
      {
        /*
          Convert DirectClass packet to VIFF RGB pixel.
        */
        number_pixels=image->columns*image->rows;
        for (y=0; y < (long) image->rows; y++)
        {
          p=GetImagePixels(image,0,y,image->columns,1);
          if (p == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            *q=DownScale(p->red);
            *(q+number_pixels)=DownScale(p->green);
            *(q+number_pixels*2)=DownScale(p->blue);
            if (image->matte)
              *(q+number_pixels*3)=MaxRGB-DownScale(p->opacity);
            p++;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
      }
    else
      if (!IsGrayImage(image))
        {
          unsigned char
            *viff_colormap;

          /*
            Dump colormap to file.
          */
          viff_colormap=(unsigned char *) AcquireMemory(3*image->colors);
          if (viff_colormap == (unsigned char *) NULL)
            ThrowWriterException(ResourceLimitWarning,
              "Memory allocation failed",image);
          q=viff_colormap;
          for (i=0; i < (long) image->colors; i++)
            *q++=DownScale(image->colormap[i].red);
          for (i=0; i < (long) image->colors; i++)
            *q++=DownScale(image->colormap[i].green);
          for (i=0; i < (long) image->colors; i++)
            *q++=DownScale(image->colormap[i].blue);
          (void) WriteBlob(image,3*image->colors,(char *) viff_colormap);
          LiberateMemory((void **) &viff_colormap);
          /*
            Convert PseudoClass packet to VIFF colormapped pixels.
          */
          q=viff_pixels;
          for (y=0; y < (long) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (long) image->columns; x++)
              *q++=indexes[x];
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                MagickMonitor(SaveImageText,y,image->rows);
          }
        }
      else
        if (image->colors == 2)
          {
            int
              x,
              y;

            register unsigned char
              bit,
              byte,
              polarity;

            /*
              Convert PseudoClass image to a VIFF monochrome image.
            */
            polarity=Intensity(image->colormap[0]) > (0.5*MaxRGB);
            if (image->colors == 2)
              polarity=
                Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
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
                byte>>=1;
                if (indexes[x] == polarity)
                  byte|=0x80;
                bit++;
                if (bit == 8)
                  {
                    *q++=byte;
                    bit=0;
                    byte=0;
                  }
              }
              if (bit != 0)
                *q++=byte >> (8-bit);
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(SaveImageText,y,image->rows);
            }
          }
        else
          {
            /*
              Convert PseudoClass packet to VIFF grayscale pixel.
            */
            for (y=0; y < (long) image->rows; y++)
            {
              p=GetImagePixels(image,0,y,image->columns,1);
              if (p == (PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
              {
                *q++=(unsigned char) Intensity(*p);
                p++;
              }
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(SaveImageText,y,image->rows);
            }
          }
    (void) WriteBlob(image,packets,(char *) viff_pixels);
    LiberateMemory((void **) &viff_pixels);
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
