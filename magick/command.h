/*
  Image Command Methods.
*/
#ifndef _MAGICK_COMMAND_H
#define _MAGICK_COMMAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport unsigned int
  CompositeImageCommand(ImageInfo *,const int, char **,ExceptionInfo *),
  ConvertImageCommand(ImageInfo *,const int, char **,ExceptionInfo *),
  IdentifyImageCommand(ImageInfo *,const int, char **,ExceptionInfo *),
  MontageImageCommand(ImageInfo *,const int, char **,ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
