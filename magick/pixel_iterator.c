/*
% Copyright (C) 2004 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Interfaces to support simple iterative pixel read/update access within
% an image or between two images.
%
% Written by Bob Friesenhahn, March 2004
%
*/

#include "magick/studio.h"
#include "magick/cache.h"
#include "magick/pixel_iterator.h"

MagickExport MagickPassFail
PixelIterateMonoRead(PixelIteratorMonoReadCallback call_back,
                     void *user_data,
                     const long x,
                     const long y,
                     const unsigned long columns,
                     const unsigned long rows,
                     const Image *image,
                     ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  for (row=y; row < y+rows; row++)
    {
      register const PixelPacket
        *pixels;

      register long
        column;
      
      pixels=AcquireImagePixels(image, x, row, columns, 1, exception);
      if (!pixels)
        {
          status=MagickFail;
          break;
        }

      for (column=x; column < x+columns; column++)
        {
          status=(call_back)(user_data,column,row,image,pixels,exception);
          if (status == MagickFail)
            break;
          pixels++;
        }

      if (status == MagickFail)
        break;
    }
  return (status);
}

MagickExport MagickPassFail
PixelIterateMonoModify(PixelIteratorMonoModifyCallback call_back,
                       void *user_data,
                       const long x,
                       const long y,
                       const unsigned long columns,
                       const unsigned long rows,
                       Image *image,
                       ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  for (row=y; row < y+rows; row++)
    {
      register PixelPacket
        *pixels;

      register long
        column;
      
      pixels=GetImagePixels(image, x, row, columns, 1);
      if (!pixels)
        {
          status=MagickFail;
          CopyException(exception,&image->exception);
          break;
        }

      for (column=x; column < x+columns; column++)
        {
          status=(call_back)(user_data,column,row,image,pixels,exception);
          if (status == MagickFail)
            break;
          pixels++;
        }

      if (!SyncImagePixels(image))
        {
          if (status != MagickFail)
            {
              status=MagickFail;
              CopyException(exception,&image->exception);
            }
        }

      if (status == MagickFail)
        break;
    }
  return (status);
}

MagickExport MagickPassFail
PixelIterateDualRead(PixelIteratorDualReadCallback call_back,
                     void *user_data,
                     const long x,
                     const long y,
                     const unsigned long columns,
                     const unsigned long rows,
                     const Image *first_image,
                     const Image *second_image,
                     ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  for (row=y; row < y+rows; row++)
    {
      register const PixelPacket
        *first_pixels,
        *second_pixels;

      register long
        column;
      
      first_pixels=AcquireImagePixels(first_image, x, row, columns, 1, exception);
      if (!first_pixels)
        {
          status=MagickFail;
          break;
        }
      second_pixels=AcquireImagePixels(second_image, x, row, columns, 1, exception);
      if (!second_pixels)
        {
          status=MagickFail;
          break;
        }

      for (column=x; column < x+columns; column++)
        {
          status=(call_back)(user_data,column,row,
                             first_image,first_pixels,
                             second_image,second_pixels,
                             exception);
          if (status == MagickFail)
            break;
          first_pixels++;
          second_pixels++;
        }

      if (status == MagickFail)
        break;
    }
  return (status);
}

MagickExport MagickPassFail
PixelIterateDualModify(PixelIteratorDualModifyCallback call_back,
                       void *user_data,
                       const long x,
                       const long y,
                       const unsigned long columns,
                       const unsigned long rows,
                       const Image *first_image,
                       Image *second_image,
                       ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  for (row=y; row < y+rows; row++)
    {
      register const PixelPacket
        *first_pixels;

      register PixelPacket
        *second_pixels;

      register long
        column;
      
      first_pixels=AcquireImagePixels(first_image, x, row, columns, 1, exception);
      if (!first_pixels)
        {
          status=MagickFail;
          break;
        }
      second_pixels=GetImagePixels(second_image, x, row, columns, 1);
      if (!second_pixels)
        {
          CopyException(exception,&second_image->exception);
          status=MagickFail;
          break;
        }

      for (column=x; column < x+columns; column++)
        {
          status=(call_back)(user_data,column,row,
                             first_image,first_pixels,
                             second_image,second_pixels,
                             exception);
          if (status == MagickFail)
            break;
          first_pixels++;
          second_pixels++;
        }

      if (!SyncImagePixels(second_image))
        {
          if (status != MagickFail)
            {
              status=MagickFail;
              CopyException(exception,&second_image->exception);
            }
        }

      if (status == MagickFail)
        break;
    }
  return (status);
}
