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
  *GetImageList(const Image *,const unsigned long,ExceptionInfo *),
	*GetNextImage(Image *),
  **ImageListToGroup(const Image *,ExceptionInfo *),
  *NewImageList(void),
  *PopImageList(Image **),
  *ReverseImageList(const Image *,ExceptionInfo *),
  *ShiftImageList(Image **);

extern MagickExport size_t
  GetImageListSize(const Image *);

extern MagickExport unsigned int
  PushImageList(Image **,const Image *,ExceptionInfo *),
  SetImageList(Image **,const Image *,const unsigned long,ExceptionInfo *),
  UnshiftImageList(Image **,const Image *,ExceptionInfo *);

extern MagickExport void
  DestroyImageList(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
