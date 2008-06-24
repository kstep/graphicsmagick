/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Image Composite Methods.
*/
#ifndef _MAGICK_COMPOSITE_H
#define _MAGICK_COMPOSITE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport MagickPassFail
  CompositeImage(Image *canvas_image,const CompositeOperator compose,
                 const Image *composite_image,
                 const long x_offset,const long y_offset);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_COMPOSITE_H */
