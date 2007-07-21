/*
  Copyright (C) 2003, 2004 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Utility Methods.
*/
#ifndef _MAGICK_UTILITY_H
#define _MAGICK_UTILITY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Enum declarations.
*/
typedef enum
{
  RootPath,
  HeadPath,
  TailPath,
  BasePath,
  ExtensionPath,
  MagickPath,
  SubImagePath,
  FullPath
} PathType;

/*
  Typedef declarations.
*/
typedef struct _TokenInfo
{
  int
    state;

  unsigned int
    flag;

  long
    offset;

  char
    quote;
} TokenInfo;

/*
  Utilities methods.
*/
#if !defined(__GNUC__) && !defined(__attribute__)
#  define __attribute__(x) /*nothing*/
#endif

#undef ARGUNUSED
#define ARGUNUSED(arg) arg __attribute__((unused))
#undef ARG_NOT_USED
#define ARG_NOT_USED(arg) (void) arg

/*
  A callback function which behaves similar to strlcpy() except which
  optionally translates text while it is copied and always returns
  the number of characters which were actually copied rather than
  the number of characters which were available to copy.
*/
typedef size_t
  (*MagickTextTranslate)(char *dst, const char *src, const size_t size);

extern MagickExport char
  *AcquireString(const char *),
  *AllocateString(const char *),
  *Base64Encode(const unsigned char *,const size_t,size_t *),
  *EscapeString(const char *,const char),
  *GetPageGeometry(const char *),
  **ListFiles(const char *,const char *,long *),
  *SetClientName(const char *),
  **StringToArgv(const char *,int *),
  **StringToList(const char *),
  *TranslateText(const ImageInfo *,Image *,const char *),
  *TranslateTextEx(const ImageInfo *,Image *,const char *,MagickTextTranslate);

extern MagickExport const char
  *GetClientFilename(void),
  *SetClientFilename(const char *),
  *GetClientName(void),
  *GetClientPath(void),
  *SetClientPath(const char *);

extern MagickExport double
  StringToDouble(const char *,const double);

extern MagickExport int
  GetGeometry(const char *,long *,long *,unsigned long *,unsigned long *),
  GlobExpression(const char *,const char *),
  LocaleNCompare(const char *,const char *,const size_t),
  LocaleCompare(const char *,const char *),
  GetMagickDimension(const char *str,double *width,double *height),
  GetMagickGeometry(const char *,long *,long *,unsigned long *,unsigned long *),
  SubstituteString(char **,const char*,const char *),
  SystemCommand(const unsigned int,const char *),
  Tokenizer(TokenInfo *,unsigned,char *,size_t,char *,char *,char *,char *,
    char,char *,int *,char *);

extern MagickExport unsigned char
  *Base64Decode(const char *, size_t *);

extern MagickExport unsigned int
  CloneString(char **,const char *),
  ConcatenateString(char **,const char *),
  ExpandFilenames(int *,char ***),
  GetExecutionPath(char *),
  GetExecutionPathUsingName(char *);

extern MagickExport MagickBool
  IsAccessible(const char *),
  IsAccessibleNoLogging(const char *),
  IsAccessibleAndNotEmpty(const char *),
  IsGeometry(const char *),
  IsGlob(const char *),
  IsWriteable(const char *),
  MagickSceneFileName(char *filename,const char* filename_template,
    const char* scene_template,const MagickBool force,unsigned long scene);

extern MagickExport unsigned long
  MultilineCensus(const char *);

extern MagickExport void
  *AcquireMemory(const size_t),
  AppendImageFormat(const char *,char *),
  *CloneMemory(void *,const void *,const size_t),
  DefineClientName(const char *),
  DefineClientPathAndName(const char *),
  ExpandFilename(char *),
  FormatSize(const magick_int64_t size,char *format),
  GetPathComponent(const char *,PathType,char *),
  GetToken(const char *,char **,char *),
  LiberateMemory(void **),
  LocaleLower(char *),
  LocaleUpper(char *),
  ReacquireMemory(void **,const size_t),
  Strip(char *),
  SetGeometry(const Image *,RectangleInfo *);

extern MagickExport void
  FormatString(char *,const char *,...) __attribute__((format (printf,2,3))),
  FormatStringList(char *string,const char *format,va_list operands);

extern MagickExport size_t
  MagickStrlCat(char *dst, const char *src, const size_t size),
  MagickStrlCpy(char *dst, const char *src, const size_t size),
  MagickStrlCpyTrunc(char *dst, const char *src, const size_t size);

#if defined(MAGICK_IMPLEMENTATION)

/*
  Allocate memory
*/
#define MagickAllocateMemory(type,size) ((type) malloc((size_t) (size)))
/* #define MagickAllocateMemory(type,size) ((type) malloc((size))) */
/* #define MagickAllocateMemory(type,size) ((size != ((size_t) size)) ? ((type) 0) : ((type) malloc((size_t) (size)))) */

/*
  Free memory and set pointer to NULL
*/
#define MagickFreeMemory(memory) \
{ \
    void *_magick_mp; \
    if (memory != 0) \
      { \
        _magick_mp=memory; \
        free(_magick_mp); \
        memory=0; \
      } \
}

/*
  Reallocate memory using provided pointer.  If pointer value is null,
  then allocate new memory. If reallocation fails then free memory,
  setting pointer to null.  If size is 0 and memory is not a null
  pointer, then free memory.  This interface behaves similar to
  realloc() except that memory is always freed (and pointer set to
  null) if a memory allocation failure occurs.

  C++ does not accept the final memory=_magick_mp without a cast so
  use smart casting.
*/
#if defined(__cplusplus) || defined(c_plusplus)
#define MagickTypeOf(var) __typeof__(var)
#else
#define MagickTypeOf(var) void*
#endif
#define MagickReallocMemory(memory,size) \
{ \
    size_t _new_size = (size_t) (size); \
    void *_magick_mp = 0; \
    _magick_mp=realloc(memory,_new_size); \
    if ((_magick_mp == 0) && (memory != 0) && (_new_size != 0)) \
       free(memory); \
    memory=(MagickTypeOf(memory))_magick_mp; \
}

/*
  Force argument into range accepted by <ctype.h> functions.
*/
#define CTYPE_ARG(value) ((int) ((unsigned char) (value)))

#if !defined(HAVE_STRLCAT)
#  define strlcat(dst,src,size) MagickStrlCat(dst,src,size)
#endif

#if !defined(HAVE_STRLCPY)
#  define strlcpy(dst,src,size) MagickStrlCpy(dst,src,size)
#endif

#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
