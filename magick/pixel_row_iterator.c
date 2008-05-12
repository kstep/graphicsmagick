/*
% Copyright (C) 2004, 2008 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Interfaces to support simple iterative pixel read/update access within
% an image or between two images.
%
% Written by Bob Friesenhahn, March 2004, Updated for rows 2008.
%
*/

#include "magick/studio.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/pixel_row_iterator.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l R o w I t e r a t e M o n o R e a d                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% PixelRowIterateMonoRead() iterates through a region of an image and
% invokes a user-provided callback function (of type
% PxelRowIteratorMonoReadCallback) for a row of pixels. This is useful to
% support simple operations such as statistics computation.
%
%  The format of the PixelRowIterateMonoRead method is:
%
%      MagickPassFail PixelRowIterateMonoRead(
%                                 PixelRowIteratorMonoReadCallback call_back,
%                                 const char *description,
%                                 void *user_data,
%                                 const long x,
%                                 const long y,
%                                 const unsigned long columns,
%                                 const unsigned long rows,
%                                 const Image *image,
%                                 ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of a pixel from each image.
%
%    o description: textual description of operation being performed.
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
PixelRowIterateMonoRead(PixelRowIteratorMonoReadCallback call_back,
                        const char *description,
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

  for (row=y; row < (long) (y+rows); row++)
    {
      register const PixelPacket
        *pixels;

      pixels=AcquireImagePixels(image, x, row, columns, 1, exception);
      if (!pixels)
        {
          status=MagickFail;
          break;
        }

      status=(call_back)(user_data,x,row,image,pixels,columns,exception);
      if (status == MagickFail)
        break;

      if (QuantumTick(row-y,rows))
        if (!MagickMonitor(description,row-y,rows,exception))
          status=MagickFail;

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
%   P i x e l R o w I t e r a t e M o n o M o d i f y                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelRowIterateMonoModify() iterates through a region of an image and
%  invokes a user-provided callback function (of type
%  PixelRowIteratorMonoReadCallback) for each row of pixels. This is useful
%  to support simple operations such as level shifting, colorspace translation,
%  or thresholding.
%
%  The format of the PixelRowIterateMonoModify method is:
%
%      MagickPassFail PixelRowIterateMonoModify(
%                              PixelRowIteratorMonoModifyCallback call_back,
%                              const char *description,
%                              void *user_data,
%                              const long x,
%                              const long y,
%                              const unsigned long columns,
%                              const unsigned long rows,
%                              Image *image,
%                              ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of a row of pixels from each image.
%
%    o description: textual description of operation being performed.
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
PixelRowIterateMonoModify(PixelRowIteratorMonoModifyCallback call_back,
                          const char *description,
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

  for (row=y; row < (long) (y+rows); row++)
    {
      register PixelPacket
        *pixels;

      pixels=GetImagePixels(image, x, row, columns, 1);
      if (!pixels)
        {
          status=MagickFail;
          CopyException(exception,&image->exception);
          break;
        }

      status=(call_back)(user_data,x,row,image,pixels,columns,exception);
      if (status == MagickFail)
        break;

      if (!SyncImagePixels(image))
        {
          if (status != MagickFail)
            {
              status=MagickFail;
              CopyException(exception,&image->exception);
            }
        }

      if (QuantumTick(row-y+1,rows))
        if (!MagickMonitor(description,row-y+1,rows,exception))
          status=MagickFail;

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
%   P i x e l R o w I t e r a t e D u a l R e a d                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelRowIterateDualRead() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelRowIteratorDualReadCallback) for each row of pixels. This is useful
%  to support operations such as image comparison.
%
%  The format of the PixelRowIterateDualModify method is:
%
%      MagickPassFail PixelRowIterateDualRead(
%                                PixelRowIteratorDualReadCallback call_back,
%                                const char *description,
%                                void *user_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *first_image,
%                                const long first_x,
%                                const long first_y,
%                                const Image *second_image,
%                                const long second_x,
%                                const long second_y,
%                                ExceptionInfo *exception);
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of a row of pixels from each image.
%
%    o description: textual description of operation being performed.
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
PixelRowIterateDualRead(PixelRowIteratorDualReadCallback call_back,
                        const char *description,
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
       first_row < (long) (first_y+rows);
       first_row++, second_row++)
    {
      register const PixelPacket
        *first_pixels,
        *second_pixels;

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

      status=(call_back)(user_data,
                         first_image,first_x,first_row,first_pixels,
                         second_image,second_x,second_row,second_pixels,
                         columns, exception);
      if (status == MagickFail)
        break;

      if (QuantumTick(first_row-first_y,rows))
        if (!MagickMonitor(description,first_row-first_y,rows,exception))
          status=MagickFail;

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
%   P i x e l R o w I t e r a t e D u a l M o d i f y                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelRowIterateDualModify() iterates through pixel regions of two images
%  and invokes a user-provided callback function (of type
%  PixelRowIteratorDualModifyCallback) for each row of pixels. This is
%  useful to support operations such as composition.
%
%  The format of the PixelRowIterateDualModify method is:
%
%      MagickPassFail PixelRowIterateDualModify(
%                                PixelRowIteratorDualModifyCallback call_back,
%                                const char *description,
%                                void *user_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *source_image,
%                                const long source_x,
%                                const long source_y,
%                                Image *update_image,
%                                const long update_x,
%                                const long update_y,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which reads from
%      a row of source pixels and updates a row of destination pixels.
%
%    o description: textual description of operation being performed.
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
PixelRowIterateDualModify(PixelRowIteratorDualModifyCallback call_back,
                          const char *description,
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
       source_row < (long) (source_y+rows);
       source_row++, update_row++)
    {
      register const PixelPacket
        *source_pixels;

      register PixelPacket
        *update_pixels;

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

      status=(call_back)(user_data,
                         source_image,source_x,source_row,source_pixels,
                         update_image,update_x,update_row,update_pixels,
                         columns,exception);
      if (status == MagickFail)
        break;

      if (!SyncImagePixels(update_image))
        {
          if (status != MagickFail)
            {
              status=MagickFail;
              CopyException(exception,&update_image->exception);
            }
        }

      if (QuantumTick(source_row-source_y,rows))
        if (!MagickMonitor(description,source_row-source_y,rows,exception))
          status=MagickFail;

      if (status == MagickFail)
        break;
    }
  return (status);
}
