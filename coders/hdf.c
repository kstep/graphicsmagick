/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            H   H  DDDD   FFFFF                              %
%                            H   H  D   D  F                                  %
%                            HHHHH  D   D  FFF                                %
%                            H   H  D   D  F                                  %
%                            H   H  DDDD   F                                  %
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
#if defined(HasHDF)
#include "hdf5.h"
#endif

/*
  Forward declarations.
*/
static unsigned int
  WriteHDFImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s H D F                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsHDF returns True if the image format type, identified by the
%  magick string, is HDF.
%
%  The format of the IsHDF method is:
%
%      unsigned int IsHDF(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsHDF returns True if the image format type is HDF.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsHDF(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick+1,"HDF",3) == 0)
    return(True);
  return(False);
}

#if defined(HasHDF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d H D F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadHDFImage reads a Hierarchical Data Format image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadHDFImage method is:
%
%      Image *ReadHDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadHDFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadHDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  hid_t
    dataset,
    dataspace,
    file;

  Image
    *image;

  int
    rank;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read HDF image.
  */
  file=H5Fopen(image->filename,H5F_ACC_RDONLY,H5P_DEFAULT);
  if (file < 0)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  dataset=H5Dopen(file,"IntArray");
  if (dataset < 0)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  dataspace=H5Dget_space(dataset);
  rank=H5Sget_simple_extent_ndims(dataspace);
  H5Fclose(file);
  CloseBlob(image);
  return(image);
}
#else
static Image *ReadHDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "HDF library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r H D F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterHDFImage adds attributes for the HDF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterHDFImage method is:
%
%      RegisterHDFImage(void)
%
*/
ModuleExport void RegisterHDFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("H5");
  entry->decoder=ReadHDFImage;
  entry->encoder=WriteHDFImage;
  entry->magick=IsHDF;
  entry->blob_support=False;
  entry->description=AllocateString("Hierarchical Data Format");
  entry->module=AllocateString("HDF");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("HDF");
  entry->decoder=ReadHDFImage;
  entry->encoder=WriteHDFImage;
  entry->magick=IsHDF;
  entry->blob_support=False;
  entry->description=AllocateString("Hierarchical Data Format");
  entry->module=AllocateString("HDF");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r H D F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterHDFImage removes format registrations made by the
%  HDF module from the list of supported formats.
%
%  The format of the UnregisterHDFImage method is:
%
%      UnregisterHDFImage(void)
%
*/
ModuleExport void UnregisterHDFImage(void)
{
  UnregisterMagickInfo("HDF");
}

#if defined(HasHDF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H D F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteHDFImage writes an image in the Hierarchial Data Format image
%  format.
%
%  The format of the WriteHDFImage method is:
%
%      unsigned int WriteHDFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteHDFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteHDFImage(const ImageInfo *image_info,Image *image)
{
  hid_t
    dataset,
    dataspace,
    file;

  hsize_t
    dimensions[2];

  int
    **pixels,
    status,
    y;

  register int
    x;

  register PixelPacket
    *q;

  unsigned int
    scene;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  CloseBlob(image);
  file=H5Fcreate(image->filename,H5F_ACC_TRUNC,H5P_DEFAULT,H5P_DEFAULT);
  if (file < 0)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize raster file header.
    */
    TransformRGBImage(image,RGBColorspace);
    dimensions[0]=image->rows;
    dimensions[1]=image->columns;
    dataspace=H5Screate_simple(2,dimensions,NULL);
    dataset=H5Dcreate(file,"IntArray",H5T_STD_U8BE,dataspace,H5P_DEFAULT);
    pixels=(int **) AcquireMemory(image->rows*sizeof(int *));
    for (y=0; y < (int) image->rows; y++)
    {
      pixels[y]=(int *) AcquireMemory(image->columns*sizeof(int));
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) image->columns; x++)
      {
        pixels[y][x]=Intensity(*q);
        q++;
      }
    }
    status=H5Dwrite(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,pixels);
    H5Sclose(dataspace);
    H5Dclose(dataset);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL) 
      image=image->previous;
  H5Fclose(file);
  return(status != -1);
}
#else
static unsigned int WriteHDFImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(MissingDelegateWarning,"HDF library is not available",
    image->filename)
}
#endif
