/*
  Methods to Get/Set/Destroy Image Text Attributes.
*/
#ifndef _MAGICK_ATTRIBUTE_H
#define _MAGICK_ATTRIBUTE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  MagickExported text attribute methods.
*/
extern MagickExport const ImageAttribute
  *GetImageAttribute(const Image *,const char *),
  *GetImageClippingPathAttribute(const Image *),
  *GetImageInfoAttribute(const ImageInfo *,const Image *,const char *);

extern MagickExport unsigned int
  SetImageAttribute(Image *,const char *,const char *);

extern MagickExport void
  DestroyImageAttributes(Image *),
  StoreImageAttribute(Image *,char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
