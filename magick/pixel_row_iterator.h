/*
  Copyright (C) 2004, 2008 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  Interfaces to support simple iterative pixel row read/update access
  within an image or between two images.  Also see the interfaces in
  pixel_iterator.h which pass a single pixel to the callback function
  rather than a row.

  WARNING!  These interfaces are still subject to change. WARNING!

  Written by Bob Friesenhahn, March 2004, Updated for rows 2008.
 
*/
#ifndef _PIXEL_ROW_ITERATOR_H
#define _PIXEL_ROW_ITERATOR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  /*
    Read-only access across pixel region.
  */

  typedef MagickPassFail (*PixelRowIteratorMonoReadCallback)
    (void *user_data,                   /* User provided mutable data */
     const long x,                      /* X-offset in base image */
     const long y,                      /* Y-offset in base image */
     const Image *const_image,          /* Input image */
     const PixelPacket *pixels,         /* Pixel row */
     const long npixels,                /* Number of pixels in row */
     ExceptionInfo *exception           /* Exception report */
     );

  extern MagickExport MagickPassFail
  PixelRowIterateMonoRead(PixelRowIteratorMonoReadCallback call_back,
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

  typedef MagickPassFail (*PixelRowIteratorMonoModifyCallback)
    (void *user_data,                   /* User provided mutable data */
     const long x,                      /* X-offset in base image */
     const long y,                      /* Y-offset in base image */
     const Image *const_image,          /* Input image */
     PixelPacket *pixels,               /* Pixel row */
     const long npixels,                /* Number of pixels in row */
     ExceptionInfo *exception           /* Exception report */
     );

  extern MagickExport MagickPassFail
  PixelRowIterateMonoModify(PixelRowIteratorMonoModifyCallback call_back,
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

  typedef MagickPassFail (*PixelRowIteratorDualReadCallback)
    (void *user_data,                   /* User provided mutable data */
     const Image *first_image,          /* First Input image */
     const long first_x,                /* X-offset in first image */
     const long first_y,                /* Y-offset in first image */
     const PixelPacket *first_pixels,   /* Pixel row in first image */
     const Image *second_image,         /* Second Input image */
     const long second_x,               /* X-offset in second image */
     const long second_y,               /* Y-offset in second image */
     const PixelPacket *second_pixels,  /* Pixel row in second image */
     const long npixels,                /* Number of pixels in row */
     ExceptionInfo *exception           /* Exception report */
     );

  extern MagickExport MagickPassFail
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
                          ExceptionInfo *exception);

  /*
    Read-write access across pixel regions of two images. The first
    (source) image is accessed read-only while the second (update)
    image is accessed as read-write.
  */

  typedef MagickPassFail (*PixelRowIteratorDualModifyCallback)
    (void *user_data,                   /* User provided mutable data */
     const Image *source_image,         /* Source image */
     const long source_x,               /* X-offset in source image */
     const long source_y,               /* Y-offset in source image */
     const PixelPacket *source_pixels,  /* Pixel row in source image */
     Image *update_image,               /* Update image */
     const long update_x,               /* X-offset in update image */
     const long update_y,               /* Y-offset in update image */
     PixelPacket *update_pixels,        /* Pixel row in update image */
     const long npixels,                /* Number of pixels in row */
     ExceptionInfo *exception           /* Exception report */
     );

  extern MagickExport MagickPassFail
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
                            ExceptionInfo *exception);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _PIXEL_ROW_ITERATOR_H */
