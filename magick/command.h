/*
  Image Command Methods.
*/
#ifndef _MAGICK_COMMAND_H
#define _MAGICK_COMMAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport unsigned int
  AnimateImageCommand(int,char **),
  ConjureImageCommand(int,char **),
  DisplayImageCommand(int,char **),
  ImportImageCommand(int,char **),
  CompositeImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *),
  ConvertImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *),
  IdentifyImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *),
  MogrifyImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *),
  MontageImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
