/*
  Copyright (C) 2004 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Methods For Manipulating Embedded Image Profiles.
*/
#ifndef _MAGICK_PROFILE_H
#define _MAGICK_PROFILE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport const unsigned char
  *GetImageProfile(const Image* image, const char *name, size_t *length);

extern MagickExport unsigned int
  DeleteImageProfile(Image *image,const char *name),
  ProfileImage(Image *image,const char *name,const unsigned char *profile,
               const size_t length,unsigned int clone),
  SetImageProfile(Image *image,const char *name,const unsigned char *profile,
                  const size_t length);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_PROFILE_H */
