/*
  Delegate Application Programming Interface declarations.
*/
#ifndef _DELEGATES_H
#define _DELEGATES_H

/*
  Delegate define definitions.
*/
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#if !defined(DelegatePath)
#define DelegatePath  "/usr/local/share/ImageMagick/"
#endif
#else
#if defined(vms)
#define DelegatePath  "sys$login:"
#endif
#if defined(macintosh)
#define DelegatePath  ""
#endif
#if defined(WIN32)
#define DelegatePath  "c:/ImageMagick/"
#endif
#endif
#define DelegateFilename  "delegates.mgk"

/*
  Delegate structure definitions.
*/
typedef struct _DelegateInfo
{
  char
    decode_tag[MaxTextExtent],
    encode_tag[MaxTextExtent],
    *commands;

  int
    direction;

  struct _DelegateInfo
    *previous,
    *next;
} DelegateInfo;

/*
  Exported delegate methods.
*/
Export char
  *GetDelegateCommand(const ImageInfo *,Image *,const char *,const char *);

Export DelegateInfo
  *SetDelegateInfo(DelegateInfo *);

Export unsigned int
  GetDelegateInfo(const char *,const char *,DelegateInfo *),
  InvokeDelegate(const ImageInfo *,Image *,const char *,const char *);

Export void
  DestroyDelegates(void),
  ListDelegateInfo(FILE *);
#endif
