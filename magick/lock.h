/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        L      OOO    CCCC  K   K                            %
%                        L     O   O  C      K  K                             %
%                        L     O   O  C      KKK                              %
%                        L     O   O  C      K  K                             %
%                        LLLL   OOO    CCCC  K   K                            %
%                                                                             %
%                                                                             %
%                        ImageMagick Mutex Methods                            %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                             William Radcliffe                               %
%                                 June 2000                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/* The mutex structure, defined in lock.c */
struct MagickMutex;
typedef struct MagickMutex MagickMutex;

/* Create a mutex, initialized unlocked */
extern MagickExport MagickMutex * MagickCreateMutex(void);

/* Lock the mutex  (Returns 0, or -1 on error) */
#define Magick_LockMutex(m)	\
{ \
  if (!m) \
    m=MagickCreateMutex(); \
  MagickMutexP(m); \
}
extern MagickExport int MagickMutexP(MagickMutex *mutex);

/* Unlock the mutex  (Returns 0, or -1 on error) */
#define Magick_UnlockMutex(m) \
{ \
  if (!m) \
    m=MagickCreateMutex(); \
  MagickMutexV(m); \
}
extern MagickExport int MagickMutexV(MagickMutex *mutex);

/* Destroy a mutex */
extern MagickExport void MagickDestroyMutex(MagickMutex *mutex);


