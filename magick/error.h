/*
  ImageMagick Exception Methods.
*/
#ifndef _MAGICK_ERROR_H
#define _MAGICK_ERROR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Exception define definitions.
*/
#define ThrowBinaryException(code,reason,description) \
{ \
  if (image != (Image *) NULL) \
    ThrowException(&image->exception,code,reason,description); \
  return(False); \
}
#define ThrowImageException(code,reason,description) \
{ \
  ThrowException(exception,code,reason,description); \
  return((Image *) NULL); \
}
#define ThrowReaderException(code,reason,image) \
{ \
  if ((image) == (Image *) NULL) \
    ThrowException(exception,code,reason,(char *) NULL); \
  else \
    { \
      ThrowException(exception,code,reason,(image)->filename); \
      DestroyImages(image); \
    } \
  return((Image *) NULL); \
}
#define ThrowWriterException(code,reason,image) \
{ \
  assert(image != (Image *) NULL); \
  ThrowException(&(image)->exception,code,reason,(image)->filename); \
  if (image_info->adjoin) \
    while ((image)->previous != (Image *) NULL) \
      (image)=(image)->previous; \
  CloseBlob(image); \
  return(False); \
}

/*
  Exception typedef declarations.
*/
typedef void
  (*ErrorHandler)(const ExceptionType,const char *,const char *);

typedef void
  (*WarningHandler)(const ExceptionType,const char *,const char *);

/*
  Exception declarations.
*/
extern MagickExport ErrorHandler
  SetErrorHandler(ErrorHandler);

extern MagickExport void
  CatchImageException(Image *),
  DestroyExceptionInfo(ExceptionInfo *),
  GetImageException(Image *,ExceptionInfo *),
  GetExceptionInfo(ExceptionInfo *),
  MagickError(const ExceptionType,const char *,const char *),
  MagickWarning(const ExceptionType,const char *,const char *),
  ThrowException(ExceptionInfo *,const ExceptionType,const char *,const char *);

extern MagickExport WarningHandler
  SetWarningHandler(WarningHandler);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
