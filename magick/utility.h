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
  *TranslateText(const ImageInfo *,Image *,const char *);

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
  GetExecutionPathUsingName(char *),
  IsAccessible(const char *),
  IsAccessibleNoLogging(const char *),
  IsAccessibleAndNotEmpty(const char *),
  IsGeometry(const char *),
  IsGlob(const char *);

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
  SetGeometry(const Image *,RectangleInfo *),
  TemporaryFilename(char *);

extern MagickExport void
  FormatString(char *,const char *,...) __attribute__((format (printf,2,3)));

#if defined(MAGICK_IMPLEMENTATION)

/*
  Allocate memory
*/
#define MagickAllocateMemory(type,size) ((type) malloc((size_t) (size)))

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
  setting pointer to NULL.
  C++ does not accept the final memory=_magick_mp without a cast.
*/
#define MagickReallocMemory(memory,size) \
{ \
    void *_magick_mp; \
    if (memory == 0) \
      _magick_mp=malloc((size_t) (size)); \
    else \
      { \
        _magick_mp=realloc(memory,(size_t) (size)); \
        if (_magick_mp == 0) \
          free(memory); \
      } \
    memory=_magick_mp; \
}

/*
  Force argument into range accepted by <ctype.h> functions.
*/
#define CTYPE_ARG(value) ((int) ((unsigned char) (value)))

#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
