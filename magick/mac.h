/*
  Macintosh Utility Methods for ImageMagick.
*/
#ifndef _MAGICK_MAC_H
#define _MAGICK_MAC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include <locale.h>
#include <stat.h>
#include <errno.h>
#include <Errors.h>
#include <Files.h>

/*
  Define declarations.
*/
#define S_IREAD  00400
#define S_IWRITE  00200

/*
  Typedef declarations.
*/
typedef struct _DIR
{
  int
    d_VRefNum;

  long int
    d_DirID;

  int
    d_index;
} DIR;

struct dirent
{
  char
    d_name[255];
 
  int
    d_namlen;
};

/*
  Macintosh utilities routines.
*/
extern MagickExport DIR
  *opendir(const char *);

MagickExport Image
  *ReadPICTImage(const ImageInfo *,ExceptionInfo *);
 
extern MagickExport int
  Exit(int),
  MACSystemCommand(const char *);

extern MagickExport long
  telldir(DIR *);

extern MagickExport struct dirent
  *readdir(DIR *);

extern MagickExport unsigned int
  MACIsMagickConflict(const char *);

extern MagickExport void
  closedir(DIR *),
  MACErrorHandler(const ExceptionType,const char *,const char *),
  MACWarningHandler(const ExceptionType,const char *,const char *),
  ProcessPendingEvents(const char *),
  seekdir(DIR *,long),
  SetApplicationType(const char *,const char *,OSType);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
