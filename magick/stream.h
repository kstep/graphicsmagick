/*
  ImageMagick Image Stream Methods.
*/
#ifndef _MAGICK_STREAM_H
#define _MAGICK_STREAM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport Image
  *ReadStream(const ImageInfo *,StreamHandler,ExceptionInfo *); /* stream.c */

extern MagickExport unsigned int
  WriteStream(const ImageInfo *,Image *,StreamHandler); /* stream.c */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_STREAM_H */
