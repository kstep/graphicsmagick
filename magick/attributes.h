/*
  Methods to Get/Set/Destroy Image Text Attributes.
*/
#ifndef _ATTRIBUTES_H
#define _ATTRIBUTES_H

/*
  MagickExported text attribute methods.
*/
extern MagickExport ImageAttribute
  *GetImageAttribute(const Image *,const char *),
  *GetImageInfoAttribute(const ImageInfo *,Image *,const char *);

extern MagickExport unsigned int
  SetImageAttribute(Image *,const char *,const char *);

extern MagickExport void
  DestroyImageAttributes(Image *),
  StoreImageAttribute(Image *,char *);

#endif
