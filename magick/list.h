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
  *RemoveLastImageFromList(Image **),
  *ReverseImageList(const Image *,ExceptionInfo *),
  *RemoveFirstImageFromList(Image **),
  *SplitImageList(Image *,const long);

extern MagickExport long
  GetImageIndexInList(const Image *);

extern MagickExport unsigned int
  AppendImageToList(Image **,Image *),
  DeleteImageFromList(Image **,const long),
  InsertImageInList(Image **,const Image *,const long,ExceptionInfo *),
  PrependImageToList(Image **,Image *),
  SpliceImageIntoList(Image **,const long,const unsigned long,Image *);

extern MagickExport unsigned long
  GetImageListLength(const Image *);

extern MagickExport void
  DestroyImageList(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
