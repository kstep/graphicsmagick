/*
  Windows NT Utility Methods for ImageMagick.
*/
#ifndef _MAGICK_NTFEATURE_H
#define _MAGICK_NTFEATURE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/*
  Define declarations.
*/

/*
  Typedef declarations.
*/


/*
  NT features routines.
*/
extern MagickExport char
  *XPRegistryKeyLookup(const char *key);

extern MagickExport HBITMAP 
	ImageToHBITMAP(Image* image);

#if !defined(XS_VERSION)

extern MagickExport unsigned int
  XPIsMagickConflict(const char *);

extern MagickExport TypeInfo
  * XPGetTypeList( void );

#endif /* !XS_VERSION */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* !C++ */

#endif /* !_MAGICK_NTFEATURE_H */
