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
  *GetImageFromList(const Image *,const long,ExceptionInfo *),
  *GetNextImageInList(const Image *),
  *GetPreviousImageInList(const Image *),
  **ImageListToArray(const Image *,ExceptionInfo *),
  *NewImageList(void),
  *PopImageFromList(Image **),
  *ReverseImageList(const Image *,ExceptionInfo *),
  *RemoveImageFromList(Image **),
  *SpliceImageIntoList(Image *,const long,const unsigned long,const Image *,
    ExceptionInfo *);

extern MagickExport long
  GetImageIndexInList(const Image *);

extern MagickExport unsigned int
  AddImageToList(Image **,const Image *,ExceptionInfo *),
  DeleteImageFromList(Image **,const long),
  InsertImageInList(Image **,const Image *,const long,ExceptionInfo *),
  PushImageOnList(Image **,const Image *,ExceptionInfo *);

extern MagickExport unsigned long
  GetImageFromListSize(const Image *);

extern MagickExport void
  DestroyImageList(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
