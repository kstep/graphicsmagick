/*
  ImageMagick Image List Methods.
*/
#ifndef _MAGICK_LIST_H
#define _MAGICK_LIST_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport Image
  *CloneImageList(const Image *,ExceptionInfo *),
  *GetImageList(const Image *,const long,ExceptionInfo *),
  *GetNextImage(const Image *),
  **ImageListToArray(const Image *,ExceptionInfo *),
  *NewImageList(void),
  *PopImageList(Image **),
  *ReverseImageList(const Image *,ExceptionInfo *),
  *ShiftImageList(Image **),
  *SpliceImageList(Image *,const long,const unsigned long,const Image *,
    ExceptionInfo *);

extern MagickExport long
  GetImageListIndex(const Image *);

extern MagickExport unsigned int
  DeleteImageList(Image *,const long),
  PushImageList(Image **,const Image *,ExceptionInfo *),
  SetImageList(Image **,const Image *,const long,ExceptionInfo *),
  UnshiftImageList(Image **,const Image *,ExceptionInfo *);

extern MagickExport unsigned long
  GetImageListSize(const Image *);

extern MagickExport void
  DestroyImageList(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
