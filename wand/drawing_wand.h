/*
  Copyright (C) 2003 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Wand Drawing Methods.
*/

#ifndef _MAGICK_DRAWING_WAND_H
#define _MAGICK_DRAWING_WAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#include "magick/draw.h"
typedef struct _DrawContext DrawingWand;
#define DrawAllocateWand(draw_info,image) (DrawAllocateContext(draw_info,image))
#define NewDrawingWand() (DrawAllocateWand(0,AllocateImage(0)))
#define DestroyDrawingWand(drawing_wand) (DrawDestroyContext(drawing_wand))

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_DRAWING_WAND_H */
