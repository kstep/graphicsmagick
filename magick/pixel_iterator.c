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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e M o n o R e a d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateMonoRead() iterates through a region of an image and invokes a
%  user-provided callback function (of type PixelIteratorMonoReadCallback) for
%  each pixel. This is useful to support simple operations such as statistics
%  computation.
%
%  The format of the PixelIterateMonoRead method is:
%
%      MagickPassFail PixelIterateMonoRead(
%                                    PixelIteratorMonoReadCallback call_back,
%                                    void *user_data,
%                                    const long x,
%                                    const long y,
%                                    const unsigned long columns,
%                                    const unsigned long rows,
%                                    const Image *image,
%                                    ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of a pixel from each image.
%
%    o user_data: User-provided context data.
%
%    o x: The horizontal ordinate of the top left corner of the region.
%
%    o y: The vertical ordinate of the top left corner of the region.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o image: The address of the Image.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e M o n o M o d i f y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateMonoModify() iterates through a region of an image and invokes a
%  user-provided callback function (of type PixelIteratorMonoReadCallback) for
%  each pixel. This is useful to support simple operations such as level
%  shifting, colorspace translation, or thresholding.
%
%  The format of the PixelIterateMonoModify method is:
%
%      MagickPassFail PixelIterateMonoModify(
%                                    PixelIteratorMonoModifyCallback call_back,
%                                    void *user_data,
%                                    const long x,
%                                    const long y,
%                                    const unsigned long columns,
%                                    const unsigned long rows,
%                                    Image *image,
%                                    ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of a pixel from each image.
%
%    o user_data: User-provided context data.
%
%    o x: The horizontal ordinate of the top left corner of the region.
%
%    o y: The vertical ordinate of the top left corner of the region.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o image: The address of the Image.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e D u a l R e a d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateDualRead() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelIteratorDualReadCallback) for each pixel. This is useful to support
%  operations such as image comparison.
%
%  The format of the PixelIterateDualModify method is:
%
%      MagickPassFail PixelIterateDualRead(
%                                   PixelIteratorDualReadCallback call_back,
%                                   void *user_data,
%                                   const unsigned long columns,
%                                   const unsigned long rows,
%                                   const Image *first_image,
%                                   const long first_x,
%                                   const long first_y,
%                                   const Image *second_image,
%                                   const long second_x,
%                                   const long second_y,
%                                   ExceptionInfo *exception);
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of a pixel from each image.
%
%    o user_data: User-provided context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o first_image: The address of the first Image.
%
%    o first_x: The horizontal ordinate of the top left corner of the first region.
%
%    o first_y: The vertical ordinate of the top left corner of the first region.
%
%    o second_image: The address of the second Image.
%
%    o second_x: The horizontal ordinate of the top left corner of the second region.
%
%    o second_y: The vertical ordinate of the top left corner of the second region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateDualRead(PixelIteratorDualReadCallback call_back,
                     void *user_data,
                     const unsigned long columns,
                     const unsigned long rows,
                     const Image *first_image,
                     const long first_x,
                     const long first_y,
                     const Image *second_image,
                     const long second_x,
                     const long second_y,
                     ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    first_row,
    second_row;

  for (first_row=first_y, second_row=second_y;
       first_row < first_y+rows;
       first_row++, second_row++)
    {
      register const PixelPacket
        *first_pixels,
        *second_pixels;

      register long
        first_column,
        second_column;
      
      first_pixels=AcquireImagePixels(first_image, first_x, first_row, columns, 1, exception);
      if (!first_pixels)
        {
          status=MagickFail;
          break;
        }
      second_pixels=AcquireImagePixels(second_image, second_x, second_row, columns, 1, exception);
      if (!second_pixels)
        {
          status=MagickFail;
          break;
        }

      for (first_column=first_x, second_column=second_x;
           first_column < first_x+columns;
           first_column++, second_column++)
        {
          status=(call_back)(user_data,
                             first_image,first_column,first_row,first_pixels,
                             second_image,second_column,second_row,second_pixels,
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e D u a l M o d i f y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateDualModify() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelIteratorDualModifyCallback) for each pixel. This is useful to support
%  operations such as composition.
%
%  The format of the PixelIterateDualModify method is:
%
%      MagickPassFail PixelIterateDualModify(
%                                   PixelIteratorDualModifyCallback call_back,
%                                   void *user_data,
%                                   const unsigned long columns,
%                                   const unsigned long rows,
%                                   const Image *source_image,
%                                   const long source_x,
%                                   const long source_y,
%                                   Image *update_image,
%                                   const long update_x,
%                                   const long update_y,
%                                   ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which reads from
%       a source pixel and updates a destination pixel.
%
%    o user_data: User-provided context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o source_image: The address of the constant source Image.
%
%    o source_x: The horizontal ordinate of the top left corner of the source region.
%
%    o source_y: The vertical ordinate of the top left corner of the source region.
%
%    o update_image: The address of the update Image.
%
%    o update_x: The horizontal ordinate of the top left corner of the update region.
%
%    o update_y: The vertical ordinate of the top left corner of the update region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateDualModify(PixelIteratorDualModifyCallback call_back,
                       void *user_data,
                       const unsigned long columns,
                       const unsigned long rows,
                       const Image *source_image,
                       const long source_x,
                       const long source_y,
                       Image *update_image,
                       const long update_x,
                       const long update_y,
                       ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    source_row,
    update_row;

  for (source_row=source_y, update_row=update_y;
       source_row < source_y+rows;
       source_row++, update_row++)
    {
      register const PixelPacket
        *source_pixels;

      register PixelPacket
        *update_pixels;

      register long
        source_column,
        update_column;
      
      source_pixels=AcquireImagePixels(source_image, source_x, source_row,
                                       columns, 1, exception);
      if (!source_pixels)
        {
          status=MagickFail;
          break;
        }
      update_pixels=GetImagePixels(update_image, update_x, update_row, columns, 1);
      if (!update_pixels)
        {
          CopyException(exception,&update_image->exception);
          status=MagickFail;
          break;
        }

      for (source_column=source_x, update_column=update_x;
           source_column < source_x+columns;
           source_column++, update_column++)
        {
          status=(call_back)(user_data,
                             source_image,source_column,source_row,source_pixels,
                             update_image,update_column,update_row,update_pixels,
                             exception);
          if (status == MagickFail)
            break;
          source_pixels++;
          update_pixels++;
        }

      if (!SyncImagePixels(update_image))
        {
          if (status != MagickFail)
            {
              status=MagickFail;
              CopyException(exception,&update_image->exception);
            }
        }

      if (status == MagickFail)
        break;
    }
  return (status);
}
