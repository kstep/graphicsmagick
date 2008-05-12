/*
  Copyright (C) 2004, 2008 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  Interfaces to support simple iterative pixel read/update access
  within an image or between two images.   Also see the interfaces in
  pixel_row_iterator.h which pass entire rows to the callback function
  rather than one pixel.

  WARNING!  These interfaces are still subject to change. WARNING!

  Written by Bob Friesenhahn, March 2004
 
*/
#ifndef _PIXEL_ITERATOR_H
#define _PIXEL_ITERATOR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  /*
    Read-only access across pixel region.
  */

  typedef MagickPassFail (*PixelIteratorMonoReadCallback)
    (void *user_data,
     const long x,
     const long y,
     const Image *const_image,
     const PixelPacket *pixel,
     ExceptionInfo *exception);

  extern MagickExport MagickPassFail
  PixelIterateMonoRead(PixelIteratorMonoReadCallback call_back,
                       const char *description,
                       void *user_data,
                       const long x,
                       const long y,
                       const unsigned long columns,
                       const unsigned long rows,
                       const Image *image,
                       ExceptionInfo *exception);
  /*
    Read-write access across pixel region.
  */

  typedef MagickPassFail (*PixelIteratorMonoModifyCallback)
    (void *user_data,
     const long x,
     const long y,
     const Image *const_image,
     PixelPacket *pixel,
     ExceptionInfo *exception);

  extern MagickExport MagickPassFail
  PixelIterateMonoModify(PixelIteratorMonoModifyCallback call_back,
                         const char *description,
                         void *user_data,
                         const long x,
                         const long y,
                         const unsigned long columns,
                         const unsigned long rows,
                         Image *image,
                         ExceptionInfo *exception);

  /*
    Read-only access across pixel regions of two images.
  */

  typedef MagickPassFail (*PixelIteratorDualReadCallback)
    (void *user_data,
     const Image *first_image,
     const long first_x,
     const long first_y,
     const PixelPacket *first_pixel,
     const Image *second_image,
     const long second_x,
     const long second_y,
     const PixelPacket *second_pixel,
     ExceptionInfo *exception);

  extern MagickExport MagickPassFail
  PixelIterateDualRead(PixelIteratorDualReadCallback call_back,
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
                       ExceptionInfo *exception);

  /*
    Read-write access across pixel regions of two images. The first
    (source) image is accessed read-only while the second (update)
    image is accessed as read-write.
  */

  typedef MagickPassFail (*PixelIteratorDualModifyCallback)
    (void *user_data,
     const Image *source_image,
     const long source_x,
     const long source_y,
     const PixelPacket *source_pixel,
     Image *update_image,
     const long update_x,
     const long update_y,
     PixelPacket *update_pixel,
     ExceptionInfo *exception);

  extern MagickExport MagickPassFail
  PixelIterateDualModify(PixelIteratorDualModifyCallback call_back,
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
                         ExceptionInfo *exception);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _PIXEL_ITERATOR_H */
