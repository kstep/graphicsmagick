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
  *GetImageFromList(const Image *,const long),
  *GetNextImageInList(const Image *),
  *GetPreviousImageInList(const Image *),
  **ImageListToArray(const Image *,ExceptionInfo *),
  *NewImageList(void),
  *RemoveLastImageFromList(Image **),
  *RemoveFirstImageFromList(Image **),
  *SplitImageList(Image *);

extern MagickExport long
  GetImageIndexInList(const Image *);

extern MagickExport unsigned int
  DeleteImageFromList(Image **),
  InsertImageInList(Image **,Image *),
  ReverseImageList(Image **),
  SpliceImageIntoList(Image **,const unsigned long,Image *);

extern MagickExport unsigned long
  GetImageListLength(const Image *);

extern MagickExport void
  AppendImageToList(Image **,Image *),
  DestroyImageList(Image *),
  PrependImageToList(Image **,Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
