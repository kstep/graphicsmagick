/*
  ImageMagick Image List Methods.
*/
#ifndef _MAGICK_LIST_H
#define _MAGICK_LIST_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport Image
  *GetImageList(Image *,const unsigned long,ExceptionInfo *),
	*GetNextImage(Image *),
  **ListToGroupImage(const Image *,unsigned long *),
  *NewImageList(void),
  *PopImageList(Image **);

extern MagickExport unsigned int
  PushImageList(Image **,const Image *,ExceptionInfo *),
  SetImageList(Image **,const Image *,const unsigned long,ExceptionInfo *);

extern MagickExport unsigned long
  GetImageListSize(const Image *);

extern MagickExport void
  DestroyImageList(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
