/*
  ImageMagick Utility Methods.
*/
#ifndef _MAGICK_UTILITY_H
#define _MAGICK_UTILITY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


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
extern MagickExport char
  *AllocateString(const char *),
  *GetExecutionPath(const char *),
  **ListFiles(const char *,const char *,int *),
  *PostscriptGeometry(const char *),
  *SetClientName(const char *),
  *SetClientPath(const char *),
  **StringToArgv(const char *,int *),
  **StringToList(const char *),
  *TranslateText(const ImageInfo *,const Image *,const char *);

extern MagickExport int
  GetGeometry(const char *,long *,long *,unsigned long *,unsigned long *),
  GlobExpression(const char *,const char *),
  LocaleNCompare(const char *,const char *,size_t),
  LocaleCompare(const char *,const char *),
  ParseGeometry(const char *,long *,long *,unsigned long *,unsigned long *),
  SystemCommand(const unsigned int,const char *),
  Tokenizer(TokenInfo *,unsigned,char *,long,char *,char *,char *,char *,
    char,char *,int *,char *);

extern MagickExport unsigned int
  CloneString(char **,const char *),
  ConcatenateString(char **,const char *),
  ExecutePostscriptInterpreter(const unsigned int,const char *),
  ExpandFilenames(int *,char ***),
  IsAccessible(const char *);

extern MagickExport unsigned long
  MultilineCensus(const char *);

extern MagickExport void
  *AcquireMemory(const size_t),
  AppendImageFormat(const char *,char *),
  ExpandFilename(char *),
  GetPathComponent(const char *,PathType,char *),
  GetToken(const char *,char **,char *),
  FormatString(char *,const char *,...),
  LiberateMemory(void **),
  LocaleLower(char *),
  LocaleUpper(char *),
  ReacquireMemory(void **,const size_t),
  Strip(char *),
  TemporaryFilename(char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
