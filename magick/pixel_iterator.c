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
% PixelIterateMonoRead() iterates through a region of an image and invokes a
% user-provided callback function (of type PixelRowIteratorMonoReadCallback)
% for a row of pixels. This is useful to support simple operations such as
% statistics computation.
%
%  The format of the PixelIterateMonoRead method is:
%
%      MagickPassFail PixelIterateMonoRead(
%                                 PixelIteratorMonoReadCallback call_back,
%                                 const char *description,
%                                 void *mutable_data,
%                                 const void *immutable_data,
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
%       address of pixels from each image.
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
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
                     const char *description,
                     void *mutable_data,
                     const void *immutable_data,
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
      const PixelPacket
        *pixels;

      const IndexPacket
        *indexes;

      pixels=AcquireImagePixels(image, x, row, columns, 1, exception);
      if (!pixels)
        {
          status=MagickFail;
          break;
        }
      indexes=GetIndexes(image);

      status=(call_back)(mutable_data,immutable_data,image,pixels,indexes,columns,exception);
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
%   P i x e l I t e r a t e M o n o M o d i f y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateMonoModify() iterates through a region of an image and invokes
%  a user-provided callback function (of type PixelIteratorMonoReadCallback)
%  for a region of pixels. This is useful to support simple operations such as
%  level shifting, colorspace translation, or thresholding.
%
%  The format of the PixelIterateMonoModify method is:
%
%      MagickPassFail PixelIterateMonoModify(
%                              PixelIteratorMonoModifyCallback call_back,
%                              const char *description,
%                              void *mutable_data,
%                              const void *immutable_data,
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
%       address of pixels from each image.
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
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
                       const char *description,
                       void *mutable_data,
                       const void *immutable_data,
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
      PixelPacket
        *pixels;

      IndexPacket
        *indexes;

      pixels=GetImagePixels(image, x, row, columns, 1);
      if (!pixels)
        {
          status=MagickFail;
          CopyException(exception,&image->exception);
          break;
        }
      indexes=GetIndexes(image);

      status=(call_back)(mutable_data,immutable_data,image,pixels,indexes,columns,exception);
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
%   P i x e l I t e r a t e D u a l R e a d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateDualRead() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelIteratorDualReadCallback) for each row of pixels. This is useful to
%  support operations such as image comparison.
%
%  The format of the PixelIterateDualModify method is:
%
%      MagickPassFail PixelIterateDualRead(
%                                PixelIteratorDualReadCallback call_back,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
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
%       address of pixels from each image.
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
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
                     const char *description,
                     void *mutable_data,
                     const void *immutable_data,
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
      const PixelPacket
        *first_pixels,
        *second_pixels;

      const IndexPacket
        *first_indexes,
        *second_indexes;

      first_pixels=AcquireImagePixels(first_image, first_x, first_row, columns, 1, exception);
      if (!first_pixels)
        {
          status=MagickFail;
          break;
        }
      first_indexes=GetIndexes(first_image);
      second_pixels=AcquireImagePixels(second_image, second_x, second_row, columns, 1, exception);
      if (!second_pixels)
        {
          status=MagickFail;
          break;
        }
      second_indexes=GetIndexes(second_image);

      status=(call_back)(mutable_data,immutable_data,
                         first_image,first_pixels,first_indexes,
                         second_image,second_pixels,second_indexes,
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
%   P i x e l I t e r a t e D u a l M o d i f y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateDualModify() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelIteratorDualModifyCallback) for each row of pixels. This is useful to
%  support operations such as composition.
%
%  The format of the PixelIterateDualModify method is:
%
%      MagickPassFail PixelIterateDualModify(
%                                PixelIteratorDualModifyCallback call_back,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
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
%      a region of source pixels and updates a region of destination pixels.
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
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
static MagickPassFail
PixelIterateDualImplementation(PixelIteratorDualModifyCallback call_back,
                               const char *description,
                               void *mutable_data,
                               const void *immutable_data,
                               const unsigned long columns,
                               const unsigned long rows,
                               const Image *source_image,
                               const long source_x,
                               const long source_y,
                               Image *update_image,
                               const long update_x,
                               const long update_y,
                               ExceptionInfo *exception,
                               MagickBool set)
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
      const PixelPacket
        *source_pixels;

      const IndexPacket
        *source_indexes;

      PixelPacket
        *update_pixels;

      IndexPacket
        *update_indexes;

      source_pixels=AcquireImagePixels(source_image, source_x, source_row,
                                       columns, 1, exception);
      if (!source_pixels)
        {
          status=MagickFail;
          break;
        }
      source_indexes=GetIndexes(source_image);
      if (set)
        update_pixels=SetImagePixels(update_image, update_x, update_row, columns, 1);
      else
        update_pixels=GetImagePixels(update_image, update_x, update_row, columns, 1);
      if (!update_pixels)
        {
          CopyException(exception,&update_image->exception);
          status=MagickFail;
          break;
        }
      update_indexes=GetIndexes(update_image);

      status=(call_back)(mutable_data,immutable_data,
                         source_image,source_pixels,source_indexes,
                         update_image,update_pixels,update_indexes,
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

MagickExport MagickPassFail
PixelIterateDualModify(PixelIteratorDualModifyCallback call_back,
                       const char *description,
                       void *mutable_data,
                       const void *immutable_data,
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
  return PixelIterateDualImplementation
    (call_back,description,mutable_data,immutable_data,columns,rows,source_image,
     source_x,source_y,update_image,update_x,update_y,exception,MagickFalse);
}



/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e D u a l N e w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateDualNew() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelIteratorDualNewCallback) for each row of pixels. This is used if a
%  new output image is created based on an input image.  The difference from
%  PixelIterateDualModify() is that the output pixels are not initialized so
%  it is more efficient when outputting a new image.
%
%  The format of the PixelIterateDualNew method is:
%
%      MagickPassFail PixelIterateDualNew(
%                                PixelIteratorDualNewCallback call_back,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *source_image,
%                                const long source_x,
%                                const long source_y,
%                                Image *new_image,
%                                const long new_x,
%                                const long new_y,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which reads from
%      a region of source pixels and initializes a region of destination pixels.
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
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
%    o new_image: The address of the new Image.
%
%    o new_x: The horizontal ordinate of the top left corner of the new region.
%
%    o new_y: The vertical ordinate of the top left corner of the new region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateDualNew(PixelIteratorDualNewCallback call_back,
                    const char *description,
                    void *mutable_data,
                    const void *immutable_data,
                    const unsigned long columns,
                    const unsigned long rows,
                    const Image *source_image,
                    const long source_x,
                    const long source_y,
                    Image *new_image,
                    const long new_x,
                    const long new_y,
                    ExceptionInfo *exception)
{
  return PixelIterateDualImplementation
    (call_back,description,mutable_data,immutable_data,columns,rows,source_image,
     source_x,source_y,new_image,new_x,new_y,exception,MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e T r i p l e M o d i f y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateTripleModify() iterates through pixel regions of three images
%  and invokes a user-provided callback function (of type
%  PixelIteratorTripleModifyCallback) for each row of pixels.  The first two
%  images are read-only, while the third image is read-write for update.
%  Access of the first two images is done lock-step using the same coordinates.
%  This is useful to support operations such as image differencing.
%
%  The format of the PixelIterateTripleModify method is:
%
%      MagickPassFail PixelIterateTripleModify(
%                                PixelIteratorTripleModifyCallback call_back,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *source1_image,
%                                const Image *source2_image,
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
%      a region of source pixels and updates a region of destination pixels.
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o source1_image: The address of the constant source 1 Image.
%
%    o source2_image: The address of the constant source 2 Image.
%
%    o source_x: The horizontal ordinate of the top left corner of the source regions.
%
%    o source_y: The vertical ordinate of the top left corner of the source regions.
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
static MagickPassFail
PixelIterateTripleImplementation(PixelIteratorTripleModifyCallback call_back,
                                 const char *description,
                                 void *mutable_data,
                                 const void *immutable_data,
                                 const unsigned long columns,
                                 const unsigned long rows,
                                 const Image *source1_image,
                                 const Image *source2_image,
                                 const long source_x,
                                 const long source_y,
                                 Image *update_image,
                                 const long update_x,
                                 const long update_y,
                                 ExceptionInfo *exception,
                                 MagickBool set)
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
      const PixelPacket
        *source1_pixels,
        *source2_pixels;

      const IndexPacket
        *source1_indexes,
        *source2_indexes;

      PixelPacket
        *update_pixels;

      IndexPacket
        *update_indexes;

      /*
        First image.
      */
      source1_pixels=AcquireImagePixels(source1_image, source_x, source_row,
                                        columns, 1, exception);
      if (!source1_pixels)
        {
          status=MagickFail;
          break;
        }
      source1_indexes=GetIndexes(source1_image);

      /*
        Second image.
      */
      source2_pixels=AcquireImagePixels(source2_image, source_x, source_row,
                                        columns, 1, exception);
      if (!source2_pixels)
        {
          status=MagickFail;
          break;
        }
      source2_indexes=GetIndexes(source2_image);

      /*
        Third image.
      */
      if (set)
        update_pixels=SetImagePixels(update_image, update_x, update_row, columns, 1);
      else
        update_pixels=GetImagePixels(update_image, update_x, update_row, columns, 1);
      if (!update_pixels)
        {
          CopyException(exception,&update_image->exception);
          status=MagickFail;
          break;
        }
      update_indexes=GetIndexes(update_image);

      status=(call_back)(mutable_data,immutable_data,
                         source1_image,source1_pixels,source1_indexes,
                         source2_image,source2_pixels,source2_indexes,
                         update_image,update_pixels,update_indexes,
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

MagickExport MagickPassFail
PixelIterateTripleModify(PixelIteratorTripleModifyCallback call_back,
                       const char *description,
                       void *mutable_data,
                       const void *immutable_data,
                       const unsigned long columns,
                       const unsigned long rows,
                       const Image *source1_image,
                       const Image *source2_image,
                       const long source_x,
                       const long source_y,
                       Image *update_image,
                       const long update_x,
                       const long update_y,
                       ExceptionInfo *exception)
{
  return PixelIterateTripleImplementation
    (call_back,description,mutable_data,immutable_data,columns,rows,
     source1_image,source2_image,source_x,source_y,
     update_image,update_x,update_y,
     exception,MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e D u a l N e w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateTripleNew() iterates through pixel regions of three images
%  and invokes a user-provided callback function (of type
%  PixelIteratorTripleNewCallback) for each row of pixels. The first two
%  images are read-only, while the third image is read-write for update.
%  Access of the first two images is done lock-step using the same coordinates.
%  This is used if a new output image is created based on two input images.
%  The difference from PixelIterateTripleModify() is that the output pixels
%  are not initialized so it is more efficient when outputting a new image.
%
%  The format of the PixelIterateTripleNew method is:
%
%      MagickPassFail PixelIterateTripleNew(
%                                PixelIteratorTripleNewCallback call_back,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *source1_image,
%                                const Image *source2_image,
%                                const long source_x,
%                                const long source_y,
%                                Image *new_image,
%                                const long new_x,
%                                const long new_y,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which reads from
%      a region of source pixels and initializes a region of destination pixels.
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o source1_image: The address of the constant source 1 Image.
%
%    o source2_image: The address of the constant source 2 Image.
%
%    o source_x: The horizontal ordinate of the top left corner of the source regions.
%
%    o source_y: The vertical ordinate of the top left corner of the source regions.
%
%    o new_image: The address of the new Image.
%
%    o new_x: The horizontal ordinate of the top left corner of the new region.
%
%    o new_y: The vertical ordinate of the top left corner of the new region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateTripleNew(PixelIteratorTripleNewCallback call_back,
                      const char *description,
                      void *mutable_data,
                      const void *immutable_data,
                      const unsigned long columns,
                      const unsigned long rows,
                      const Image *source1_image,
                      const Image *source2_image,
                      const long source_x,
                      const long source_y,
                      Image *new_image,
                      const long new_x,
                      const long new_y,
                      ExceptionInfo *exception)
{
  return PixelIterateTripleImplementation
    (call_back,description,mutable_data,immutable_data,columns,rows,
     source1_image,source2_image,source_x,source_y,
     new_image,new_x,new_y,
     exception,MagickTrue);
}
