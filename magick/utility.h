/*
  ImageMagick Utility Methods.
*/
#ifndef _UTILITY_H
#define _UTILITY_H

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

  unsigned
    flag;

  char
    quote;

  int
    offset;
} TokenInfo;

/*
  Utilities methods.
*/
extern MagickExport char
  *AllocateString(const char *),
  *BaseFilename(const char *),
  *GetExecutionPath(const char *),
  **ListFiles(const char *,const char *,int *),
  *PostscriptGeometry(const char *),
  *SetClientName(const char *),
  *SetClientPath(const char *),
  **StringToArgv(const char *,int *),
  **StringToList(const char *),
  *TranslateText(const ImageInfo *,Image *,const char *);

extern MagickExport int
  GetGeometry(const char *,int *,int *,unsigned int *,unsigned int *),
  GlobExpression(const char *,const char *),
  LocaleNCompare(const char *,const char *,size_t),
  LocaleCompare(const char *,const char *),
  MultilineCensus(const char *),
  SystemCommand(const unsigned int,const char *),
  Tokenizer(TokenInfo *,unsigned,char *,int,char *,char *,char *,char *,
    char,char *,int *,char *);

extern MagickExport unsigned int
  CloneString(char **,const char *),
  ConcatenateString(char **,const char *),
  ExpandFilenames(int *,char ***),
  IsAccessible(const char *),
  IsDirectory(const char *);

extern MagickExport unsigned short
  *ConvertTextToUnicode(const char *,int *);

extern MagickExport void
  *AcquireMemory(const size_t),
  AppendImageFormat(const char *,char *),
  DestroyPostscriptGeometry(char *),
  ExpandFilename(char *),
  FormatString(char *,const char *,...),
  LiberateMemory(void **),
  LocaleLower(char *),
  LocaleUpper(char *),
  LocaleFilename(char *),
  ReacquireMemory(void **,const size_t),
  Strip(char *),
  TemporaryFilename(char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
