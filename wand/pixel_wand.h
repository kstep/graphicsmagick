/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2003 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Pixel Wand Methods.
*/

#ifndef _MAGICK_PIXEL_WAND_H
#define _MAGICK_PIXEL_WAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct _PixelWand PixelWand;

extern WandExport char
  *PixelGetColorAsString(const PixelWand *);

extern WandExport double
  PixelGetBlue(const PixelWand *),
  PixelGetGreen(const PixelWand *),
  PixelGetOpacity(const PixelWand *),
  PixelGetRed(const PixelWand *);

extern WandExport PixelWand
  *NewPixelWand(void);

extern WandExport Quantum
  PixelGetBlueQuantum(const PixelWand *),
  PixelGetGreenQuantum(const PixelWand *),
  PixelGetOpacityQuantum(const PixelWand *),
  PixelGetRedQuantum(const PixelWand *);

extern WandExport unsigned int
  PixelSetColor(PixelWand *,const char *);

extern WandExport void
  DestroyPixelWand(PixelWand *),
  PixelGetQuantumColor(const PixelWand *,PixelPacket *),
  PixelSetBlue(PixelWand *,const double),
  PixelSetBlueQuantum(PixelWand *,const Quantum),
  PixelSetGreen(PixelWand *,const double),
  PixelSetGreenQuantum(PixelWand *,const Quantum),
  PixelSetOpacity(PixelWand *,const double),
  PixelSetOpacityQuantum(PixelWand *,const Quantum),
  PixelSetRed(PixelWand *,const double),
  PixelSetRedQuantum(PixelWand *,const Quantum);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
