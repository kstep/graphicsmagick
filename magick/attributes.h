/*
  Methods to Get/Set/Destroy Image Text Attributes.
*/
#ifndef _ATTRIBUTES_H
#define _ATTRIBUTES_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Delegate structure definitions.
*/
/*
  Exported text attribute methods.
*/
extern Export ImageAttribute
  *GetImageAttribute(Image *,const char *);

extern Export unsigned int
  SetImageAttribute(Image *,const char *,const char *);

extern Export void
  DestroyImageAttributes(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
