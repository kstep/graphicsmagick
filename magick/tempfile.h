/*
  Copyright (C) 2003 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Temporary File Management
*/

#ifndef _MAGICK_TEMPORARY_H
#define _MAGICK_TEMPORARY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

typedef enum
{
  BinaryFileIOMode,
  TextFileIOMode
} FileIOMode;

MagickExport void
  DestroyTemporaryFiles(void),
  LiberateTemporaryFile(const char *filename);

MagickExport unsigned int
  AcquireTemporaryFileName(char *filename);

MagickExport int
  AcquireTemporaryFileDescriptor(char *filename);

MagickExport FILE *
  AcquireTemporaryFileStream(char *filename,FileIOMode mode);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* ifndef _MAGICK_TEMPORARY_H */
