/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Image Command Methods.
*/
#ifndef _MAGICK_COMMAND_H
#define _MAGICK_COMMAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport unsigned int
  AnimateImageCommand(int,char **),
  CompositeImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *),
  ConjureImageCommand(int,char **),
  ConvertImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *),
  DisplayImageCommand(int,char **),
  IdentifyImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *),
  ImportImageCommand(int,char **),
  MogrifyImage(const ImageInfo *,const int,char **,Image **),
  MogrifyImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *),
  MogrifyImages(const ImageInfo *,const int,char **,Image **),
  MontageImageCommand(ImageInfo *,const int,char **,char **,ExceptionInfo *);

extern MagickExport void
  AnimateUsage(void),
  CompositeUsage(void),
  ConjureUsage(void),
  ConvertUsage(void),
  DisplayUsage(void),
  IdentifyUsage(void),
  ImportUsage(void),
  MogrifyUsage(void),
  MontageUsage(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
