/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
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
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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


#if defined(WIN32)
#include <windows.h>
#endif

/*
  Include declarations.
*/
#include "studio.h"


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C L I P B O A R D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadClipboardImage reads an image from the system clipboard and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadClipboardImage method is:
%
%      Image *ReadClipboardImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadClipboardImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadClipboardImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  size_t
    count;

  unsigned int
    packet_size,
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"Must specify image size",image);
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      status=OpenBlob(image_info,image,ReadBinaryType,exception);
      if (status == False)
        ThrowReaderException(FileOpenError,"Unable to open file",image);
      for (i=0; i < image->offset; i++)
        (void) ReadBlobByte(image);
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
%   W r i t e C L I P B O A R D I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteClipboardImage writes an image to the system clipboard.
%
%  The format of the WriteClipboardImage method is:
%
%      unsigned int WriteClipboardImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteClipboardImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteClipboardImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  register const PixelPacket
    *p;

  unsigned char
    *pixels;

  unsigned int
    packet_size,
    scene,
    status;

  /*
    Allocate memory for pixels.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

#if defined(WIN32)
  {
    unsigned long 
		nPixels,
		nPixelCount;
	long
		memSize;
	PixelPacket *pPixels;
    BITMAP  bitmap;
	HBITMAP	bitmapH;
	HANDLE  theBitsH;

	OpenClipboard( NULL );
    EmptyClipboard();

    nPixels = image->columns * image->rows;

    bitmap.bmType = 0;
    bitmap.bmWidth = image->columns;
    bitmap.bmHeight = image->rows;
    bitmap.bmWidthBytes = bitmap.bmWidth * 4;
    bitmap.bmPlanes = 1;
    bitmap.bmBitsPixel = 32;
    bitmap.bmBits = NULL;

    memSize = nPixels * bitmap.bmBitsPixel;
    theBitsH = (HANDLE) GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, memSize);
    if (theBitsH == NULL)
	 return( False );	/* DoDisplayError( "OnEditCopy", GetLastError() ); */
    else {
		RGBQUAD * theBits = (RGBQUAD *) GlobalLock((HGLOBAL) theBitsH);
		RGBQUAD *pDestPixel = theBits;
		if ( bitmap.bmBits == NULL )
			bitmap.bmBits = theBits;

		pPixels = AcquireImagePixels(image,0,0,image->columns,image->rows,&image->exception);
		for( nPixelCount = nPixels; nPixelCount ; nPixelCount-- )
		{
			pDestPixel->rgbRed	    = Downscale(pPixels->red);
			pDestPixel->rgbGreen    = Downscale(pPixels->green);
			pDestPixel->rgbBlue	    = Downscale(pPixels->blue);
			pDestPixel->rgbReserved = 0;
			++pDestPixel;
			++pPixels;
		}

		bitmap.bmBits = theBits;
		bitmapH = CreateBitmapIndirect( &bitmap );

		GlobalUnlock((HGLOBAL) theBitsH);

		SetClipboardData(CF_BITMAP, bitmapH);
		CloseClipboard();
    }

	return(True);
  }
#endif

  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C L I P B O A R D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterClipboardImage adds attributes for the clipboard "image format" to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterClipboardImage method is:
%
%      RegisterClipboardImage(void)
%
*/
ModuleExport void RegisterClipboardImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CLIPBOARD");
  entry->decoder=ReadClipboardImage;
  entry->encoder=WriteClipboardImage;
  entry->adjoin = False;
  entry->description=AcquireString("the system clipboard");
  entry->module=AcquireString("CLIPBOARD");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C L I P B O A R D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterClipboardImage removes format registrations made by the
%  RGB module from the list of supported formats.
%
%  The format of the UnregisterClipboardImage method is:
%
%      UnregisterClipboardImage(void)
%
*/
ModuleExport void UnregisterClipboardImage(void)
{
  (void) UnregisterMagickInfo("CLIPBOARD");
}

